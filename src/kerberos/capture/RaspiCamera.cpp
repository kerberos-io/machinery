#include "capture/RaspiCamera.h"
#include <signal.h>
#include <sched.h>

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

std::ofstream file;

static int HighPri(const int pri)
{
		struct sched_param sched;
		memset(&sched, 0, sizeof(sched));

		if (pri > sched_get_priority_max(SCHED_RR))
		{
				sched.sched_priority = sched_get_priority_max( SCHED_RR );
		}
		else
		{
				sched.sched_priority = pri;
		}

		return sched_setscheduler(0, SCHED_RR, &sched);
}

void* preview_thread(void* self)
{
		kerberos::RaspiCamera * capture = (kerberos::RaspiCamera *) self;

		// Retrieve OMX buffer
		capture->data_buffer = state.camera->outputPorts()[70].buffer->pBuffer;
		capture->mjpeg_data_buffer = state.preview_encode->outputPorts()[201].buffer->pBuffer;

		HighPri(99); // Mark the thread as high priority

		// ATTENTION : Each loop must take less time than it takes to the camera to take one frame
		// otherwise it will cause underflow which can lead to camera stalling
		// a good solution is to implement frame skipping (measure time between to loops, if this time
		// is too big, just skip image processing and MJPEG sendout)

		BINFO << "RaspiCamera: Entering preview thread.";
		while (state.running)
		{
				capture->healthCounter = std::rand() % 10000;

				// Get YUV420 image from preview port, this is a blocking call
				// If zero-copy is activated, we don't pass any buffer
				capture->data_length  = state.camera->getOutputData(70, nullptr);
		}
		BINFO << "RaspiCamera: Exiting preview thread.";

		return nullptr;
}


void* record_thread(void* self)
{
		kerberos::RaspiCamera * capture = (kerberos::RaspiCamera *) self;

		uint8_t* data = new uint8_t[65536*4];

		BINFO << "RaspiCamera: Entering record thread.";
		while(state.running)
		{
				// Consume h264 data, this is a blocking call
				int32_t datalen = state.record_encode->getOutputData(data);
				if(datalen > 0 && state.recording)
				{
						file.write((char*) data, datalen);
						file.flush();
				}
		}
		BINFO << "RaspiCamera: Exiting record thread.";

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
        m_framerate = std::atoi(settings.at("captures.RaspiCamera.framerate").c_str());
        m_sharpness = std::atoi(settings.at("captures.RaspiCamera.sharpness").c_str());
        m_brightness = std::atoi(settings.at("captures.RaspiCamera.brightness").c_str());
        m_contrast = std::atoi(settings.at("captures.RaspiCamera.contrast").c_str());
        m_saturation = std::atoi(settings.at("captures.RaspiCamera.saturation").c_str());

        // Initialize executor
        tryToUpdateCapture.setAction(this, &RaspiCamera::update);
        tryToUpdateCapture.setInterval("once at 1000 calls");

        // Save width and height in settings.
        Capture::setup(settings, width, height, angle);
        setImageSize(width, height);
        setRotation(angle);
        setDelay(delay);

				m_onBoardRecording = true;
				m_hardwareMJPEGEncoding = true;

        // Open camera
        open();
    }

    void RaspiCamera::grab(){}

		Image RaspiCamera::retrieve()
		{
				// Delay camera for some time..
				usleep(m_delay*1000);

				Image image;

				try
				{
						cv::Mat mYUV(m_frameHeight + m_frameHeight / 2, m_frameWidth, CV_8UC1, (void*) data_buffer);
						cvtColor(mYUV, image.getImage(), CV_YUV2BGR_I420, 3);
				}
				catch(cv::Exception & ex)
				{
						throw OpenCVException(ex.msg.c_str());
				}

				return image;
		}

		int32_t RaspiCamera::retrieveRAW(uint8_t * data)
		{
				try
				{
						if (data_length > 0) {
							// Send it to the MJPEG encoder
							state.preview_encode->fillInput(200, data_buffer, data_length, false, true);
						}

						int length = 0;
						while ( ( length = state.preview_encode->getOutputData(data, false ) ) > 0 ) {
							mjpeg_data_length = length;
						}

						return mjpeg_data_length;
				}
				catch(cv::Exception & ex)
				{
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
						cv::Mat mYUV(m_frameHeight + m_frameHeight / 2, m_frameWidth, CV_8UC1, (void*) data_buffer);
						cvtColor(mYUV, image->getImage(), CV_YUV2BGR_I420, 3);
				}
				catch(cv::Exception & ex)
				{
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
				state.camera = new Camera(m_frameWidth, m_frameHeight, 0, false, 0, false);
				state.preview_encode = new VideoEncode(16192, VideoEncode::CodingMJPEG, false, false);
				state.record_encode = new VideoEncode(4096, VideoEncode::CodingAVC, false, false);

				// Setup camera
				state.camera->setFramerate(m_framerate);
				state.camera->setBrightness(m_brightness);
				state.camera->setSaturation(m_saturation);
				state.camera->setContrast(m_contrast);
				state.camera->setSharpness(m_sharpness);

				if(m_angle > 0)
				{
						state.camera->setRotation(m_angle);
				}

				// Copy preview port definition to the encoder to help it handle incoming data
				Component::CopyPort( &state.camera->outputPorts()[70], &state.preview_encode->inputPorts()[200] );

				// Tunnel video port to AVC encoder
				state.camera->SetupTunnelVideo( state.record_encode );

				// Prepare components for next step
				state.camera->SetState(Component::StateIdle);
				state.preview_encode->SetState(Component::StateIdle);
				state.record_encode->SetState(Component::StateIdle);

				// Allocate buffers that will be processed manually
				state.camera->AllocateOutputBuffer(70);
				state.preview_encode->AllocateInputBuffer(200);

				// Start components
				state.camera->SetState(Component::StateExecuting);
				state.preview_encode->SetState(Component::StateExecuting);
				state.record_encode->SetState(Component::StateExecuting);

				// Start capturing
				state.camera->SetCapturing(true);
				state.running = true;
				state.recording = false;

				// Start threads
				pthread_create(&state.preview_thid, nullptr, &preview_thread, this);
				pthread_detach(state.preview_thid);

				pthread_create(&state.record_thid, nullptr, &record_thread, this);
				pthread_detach(state.record_thid);
    }

		void RaspiCamera::stopThreads()
		{
				state.running = false;

				// -------------------------
				// Cancel the record thread.

				pthread_join(state.record_thid, nullptr);

				// -------------------------
				// Cancel the preview thread.

				pthread_join(state.preview_thid, nullptr);
		}

		RaspiCamera::~RaspiCamera()
		{
				Component::DestroyAll();
		}

    void RaspiCamera::close()
    {
				stopThreads();
    }

    void RaspiCamera::update(){}

    bool RaspiCamera::isOpened()
    {
        return state.running;
    }

		void RaspiCamera::startRecord(std::string path)
		{
				// Restart record_encode component before recording
				state.record_encode->SetState(Component::StateIdle);
				state.record_encode->SetState(Component::StateExecuting);

				// Create a new file to which the recording is written
				file.open(path, std::ofstream::out | std::ofstream::binary);

				// Write some headers
				const std::map< uint32_t, uint8_t* > headers = state.record_encode->headers();
				if ( headers.size() > 0 ) {
					for ( auto hdr : headers ) {
						file.write((char*) hdr.second, hdr.first);
						file.flush();
					}
				}

				// Enable recording
				state.recording = true;
		}

		void RaspiCamera::stopRecord()
		{
				file.close();
				state.recording = false;
		}
}
