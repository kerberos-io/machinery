#include "capture/RaspiCamera.h"

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

void process_image(kerberos::RaspiCamera * capture, uint8_t* data )
{
	uint8_t* y_plane = data;
	uint8_t* u_plane = &data[ 1280 * 720 ];
	uint8_t* v_plane = &data[ 1280 * 720 + ( 1280 / 2 ) * ( 720 / 2 ) ];

	int width = 1280;
	int height = 720;

	for ( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			int xx = x >> 1;
			int yy = y >> 1;
			int Y = y_plane[ y * width + x ] - 16;
			int U = u_plane[ yy * width/2 + xx ] - 128;
			int V = v_plane[ yy * width/2 + xx ] - 128;
			int r = ( 298 * Y + 409 * V + 128 ) >> 8;
			int g = ( 298 * Y - 100 * U - 208 * V + 128 ) >> 8;
			int b = ( 298 * Y + 516 * U + 128 ) >> 8;
			// put [r,g,b] in cvMat at [x,y]
		}
	}

	/*pthread_mutex_lock(&capture->m_lock);
	capture->mjpeg_data_length = datalen;
	capture->mjpeg_data_buffer = mjpeg_data;
	pthread_mutex_unlock(&capture->m_lock);*/

	// TODO : process image here (i.e. pass it to OpenCV, export to external program, ...)
}

void* preview_thread( void* self )
{

	kerberos::RaspiCamera * capture = (kerberos::RaspiCamera *) self;

	bool zero_copy = true;
	uint8_t* data = nullptr;
	uint8_t* mjpeg_data = nullptr;
	if ( not zero_copy ) {
		// Allocate space for image
		data = new uint8_t[1280*720*sizeof(uint16_t)];
		mjpeg_data = new uint8_t[80000];
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
			// Process the image
			process_image( capture, data );
		}

		pthread_mutex_lock(&capture->m_lock);
		while ( ( datalen = state.preview_encode->getOutputData( zero_copy ? nullptr : mjpeg_data, false ) ) > 0 ) {
				capture->mjpeg_data_length = datalen;
				capture->mjpeg_data_buffer = mjpeg_data;
		}
		pthread_mutex_unlock(&capture->m_lock);
	}

	return nullptr;
}


void* record_thread( void* argp )
{
	uint8_t* data = new uint8_t[65536];
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
        Image * image = new Image();

				uint8_t* data = new uint8_t[80000];
				int32_t length = retrieveRAW(data);
				std::vector<uint8_t> buffer;
				cv::Mat builded_img;
				for (uint ptr = 0; ptr < length; ptr++) {
					buffer.push_back(data[ptr]);
				}
				builded_img = cv::imdecode(buffer, CV_LOAD_IMAGE_COLOR);
				image->setImage(builded_img);
				delete data;

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
