#include "capture/RaspiCamera.h"
#include <signal.h>

using namespace IL;

struct State {
	Camera* camera;
	VideoEncode* preview_encode;
	VideoEncode* record_encode;
	bool recording; // TODO : set this variable to true from an external thread to activate recording
	bool running;
	pthread_t preview_thid;
	pthread_t record_thid;
} state = { nullptr, nullptr, nullptr, false, false };

void* preview_thread( void* self )
{
	kerberos::RaspiCamera * capture = (kerberos::RaspiCamera *) self;

	bool zero_copy = true;
	uint8_t* data = nullptr;
	uint8_t* mjpeg_data = nullptr;
	if ( not zero_copy ) {
		// Allocate space for image
		data = new uint8_t[1280*720*sizeof(uint16_t)];
		mjpeg_data = new uint8_t[(int)(1280*720*1.5)];
	}

	// ATTENTION : Each loop must take less time than it takes to the camera to take one frame
	// otherwise it will cause underflow which can lead to camera stalling
	// a good solution is to implement frame skipping (measure time between to loops, if this time
	// is too big, just skip image processing and MJPEG sendout)
	while ( state.running ) {
		if ( zero_copy ) {
			// Retrieve OMX buffer
			data = state.camera->outputPorts()[70].buffer->pBuffer;
			mjpeg_data = state.preview_encode->outputPorts()[201].buffer->pBuffer;
		}
		// Get YUV420 image from preview port, this is a blocking call
		// If zero-copy is activated, we don't pass any buffer
		int32_t datalen = state.camera->getOutputData( 70, zero_copy ? nullptr : data );

		if ( datalen > 0 ) {
			// Send it to the MJPEG encoder
			state.preview_encode->fillInput( 200, data, datalen, false, true );
			// Grab the image
			pthread_mutex_lock(&capture->m_lock);
			capture->data_length = datalen;
			capture->data_buffer = data;
			pthread_mutex_unlock(&capture->m_lock);
		}

		while ( ( datalen = state.preview_encode->getOutputData( zero_copy ? nullptr : mjpeg_data, false ) ) > 0 ) {
				pthread_mutex_lock(&capture->m_lock);
				capture->mjpeg_data_length = datalen;
				capture->mjpeg_data_buffer = mjpeg_data;
				pthread_mutex_unlock(&capture->m_lock);
		}
	}

	return nullptr;
}


void* record_thread( void* argp )
{
	uint8_t* data = new uint8_t[65536*4];
	std::ofstream file( "/tmp/test.h264", std::ofstream::out | std::ofstream::binary );
	while ( state.running ) {
		// Consume h264 data, this is a blocking call
		int32_t datalen = state.record_encode->getOutputData( data );
		if ( datalen > 0 && state.recording ) {
			// TODO : save data somewhere
			file.write((char*) data, datalen);
			file.flush();
		}
	}

	return nullptr;
}

void terminate( int sig )
{
	state.running = false;
	pthread_join( state.record_thid, nullptr );
	pthread_join( state.preview_thid, nullptr );
	exit(0);
}

namespace kerberos
{
    void RaspiCamera::setup(kerberos::StringMap &settings)
    {
        int width = std::atoi(settings.at("captures.RaspiCamera.frameWidth").c_str());
        int height = std::atoi(settings.at("captures.RaspiCamera.frameHeight").c_str());
        int angle = std::atoi(settings.at("captures.RaspiCamera.angle").c_str());
        int delay = std::atoi(settings.at("captures.RaspiCamera.delay").c_str());

        // Initialize executor
        tryToUpdateCapture.setAction(this, &RaspiCamera::update);
        tryToUpdateCapture.setInterval("once at 1000 calls");

        // Save width and height in settings.
        Capture::setup(settings, width, height, angle);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);

				// Allocate space for grabbing images
				image_data = new uint8_t[1280*720*sizeof(uint16_t)];

        // Open camera
        open();
    }

    void RaspiCamera::grab(){}

		Image RaspiCamera::retrieve(){}

		int32_t RaspiCamera::retrieveRAW(uint8_t * data)
		{
				try
				{
						int32_t length = 0;
						pthread_mutex_lock(&m_lock);
						length = mjpeg_data_length;
						memcpy(data, mjpeg_data_buffer, length);
						pthread_mutex_unlock(&m_lock);

						return length;
				}
				catch(cv::Exception & ex)
				{
						pthread_mutex_unlock(&m_lock);
						throw OpenCVException(ex.msg.c_str());
				}
		}

    Image * RaspiCamera::takeImage()
    {
				// Delay camera for some time..
		 		usleep(m_delay*1000);

				Image * image = new Image();

				try
				{
						int32_t length = 0;
						pthread_mutex_lock(&m_lock);
						length = data_length;
						memcpy(image_data, data_buffer, length);
						pthread_mutex_unlock(&m_lock);

						int width = 1280;
						int height = 720;
						cv::Mat mYUV(height + height / 2, width, CV_8UC1, (void*)image_data);
						cvtColor( mYUV, image->getImage(), CV_YUV2BGR_I420, 3 );
				}
				catch(cv::Exception & ex)
				{
						pthread_mutex_unlock(&m_lock);
						throw OpenCVException(ex.msg.c_str());
				}

        return image;
    }

    void RaspiCamera::setImageSize(int width, int height)
    {
        Capture::setImageSize(width, height);
				// .. set size
    }

    void RaspiCamera::setRotation(int angle)
    {
        Capture::setRotation(angle);
    }

    void RaspiCamera::setDelay(int msec)
    {
        Capture::setDelay(msec);
    }

    void RaspiCamera::open()
    {
				// Initialize hardware
				bcm_host_init();

				// Create components
				state.camera = new Camera( 1280, 720, 0, false, 0, false );
				state.preview_encode = new VideoEncode( 8192, VideoEncode::CodingMJPEG, false, false );
				state.record_encode = new VideoEncode( 4096, VideoEncode::CodingAVC, false, false );

				// Setup camera
				state.camera->setFramerate( 30 );

				// Copy preview port definition to the encoder to help it handle incoming data
				Component::CopyPort( &state.camera->outputPorts()[70], &state.preview_encode->inputPorts()[200] );

				// Tunnel video port to AVC encoder
				state.camera->SetupTunnelVideo( state.record_encode );

				// Prepare components for next step
				state.camera->SetState( Component::StateIdle );
				state.preview_encode->SetState( Component::StateIdle );
				state.record_encode->SetState( Component::StateIdle );

				// Allocate buffers that will be processed manually
				state.camera->AllocateOutputBuffer( 70 );
				state.preview_encode->AllocateInputBuffer( 200 );

				// Start components
				state.camera->SetState( Component::StateExecuting );
				state.preview_encode->SetState( Component::StateExecuting );
				state.record_encode->SetState( Component::StateExecuting );

				// Start capturing
				state.camera->SetCapturing( true );
				state.running = true;
				//state.recording = true;

				// Start threads
				pthread_create( &state.preview_thid, nullptr, &preview_thread, this );
				pthread_create( &state.record_thid, nullptr, &record_thread, this );
    }

    void RaspiCamera::close()
    {
        m_camera->release();
    }

    void RaspiCamera::update(){}

    bool RaspiCamera::isOpened()
    {
        return m_camera->isOpened();
    }
}
