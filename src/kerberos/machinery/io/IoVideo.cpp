#include "machinery/io/IoVideo.h"

namespace kerberos
{
    void IoVideo::setup(const StringMap & settings)
    {
        Io::setup(settings);
        
        m_writer = 0;
        m_recording = false;
        pthread_mutex_init(&m_lock, NULL);
        pthread_mutex_init(&m_time_lock, NULL);
        pthread_mutex_init(&m_capture_lock, NULL);
        startRetrieveThread();
        
        // --------------------------
        // Get name from instance
        
        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);

        m_fps = std::atoi(settings.at("ios.Video.fps").c_str());
        m_width = std::atoi(settings.at("capture.width").c_str());
        m_height = std::atoi(settings.at("capture.height").c_str());
        m_recordingTimeAfter = std::atoi(settings.at("ios.Video.recordAfter").c_str()); // in seconds
        m_extension = settings.at("ios.Video.extension");
        std::string codec = settings.at("ios.Video.codec");
        
        if(codec == "h264")
        {
            m_codec = 0x00000021; //CV_FOURCC('X','2','6','4');
        }
        else
        {
            m_codec = -1;
        }

        // --------------------------
        // Check if need to draw timestamp
        
        /*bool drawTimestamp = (settings.at("ios.Video.markWithTimestamp") == "true");
        setDrawTimestamp(drawTimestamp);
        cv::Scalar color = getColor(settings.at("ios.Video.timestampColor"));
        setTimestampColor(color);
        
        std::string timezone = settings.at("timezone");
        std::replace(timezone.begin(), timezone.end(), '-', '/');
        std::replace(timezone.begin(), timezone.end(), '$', '_');
        setTimezone(timezone);*/
        
        // -------------------------------------------------------------
        // Filemanager is mapped to a directory and is used by an image
        // to save to the correct directory.
        
        setFileFormat(settings.at("ios.Video.fileFormat"));
        m_directory = settings.at("ios.Video.directory");
    }
    
    cv::Scalar IoVideo::getColor(const std::string name)
    {
        std::map<std::string, cv::Scalar> m_colors;
        
        m_colors["white"] = cv::Scalar(255,255,255);
        m_colors["black"] = cv::Scalar(0,0,0);
        m_colors["red"] = cv::Scalar(0,0,255);
        m_colors["green"] = cv::Scalar(0,255,0);
        m_colors["blue"] = cv::Scalar(255,0,0);
        
        return m_colors.at(name);
    }

    std::string IoVideo::buildPath(std::string pathToVideo)
    {
        // -----------------------------------------------
        // Get timestamp, microseconds, random token, and instance name
        
        std::string instanceName = getInstanceName();
        kerberos::helper::replace(pathToVideo, "instanceName", instanceName);

        std::string timestamp = kerberos::helper::getTimestamp();
        kerberos::helper::replace(pathToVideo, "timestamp", timestamp);

        std::string microseconds = kerberos::helper::getMicroseconds();
        std::string size = kerberos::helper::to_string((int)microseconds.length());
        kerberos::helper::replace(pathToVideo, "microseconds", size + "-" + microseconds);

        std::string token = kerberos::helper::to_string(rand()%1000);
        kerberos::helper::replace(pathToVideo, "token", token);

        return pathToVideo;
    }

    void IoVideo::fire()
    {
        m_recording = true;

        // ----------------------------------------------------------
        // If a video is recording, and a new detection is coming in,
        // we'll reset the timer. So the video is expaned.
        // timer = ...

        pthread_mutex_lock(&m_time_lock);
        m_timeStartedRecording = (double) (cv::getTickCount() / cv::getTickFrequency());
        pthread_mutex_unlock(&m_time_lock);
        
        // -----------------------------------------------------
        // Check if already recording, if not start a new video
        
        if(m_writer == 0)
        {
            // ----------------------------------------
            // The naming convention that will be used
            // for the image.
            
            std::string pathToVideo = getFileFormat();
            std::string file = buildPath(pathToVideo);
            
            m_writer = new cv::VideoWriter();
            m_writer->open(m_directory + file + "." + m_extension, m_codec, m_fps, cv::Size(m_width, m_height));
            
            stopRecordThread();
            startRecordThread();
        }
    }

    void IoVideo::disableCapture()
    {
        pthread_mutex_lock(&m_capture_lock);
        m_capture = 0;
        pthread_mutex_unlock(&m_capture_lock);

        stopRecordThread();
        stopRetrieveThread();
    }

    bool IoVideo::save(Image & image)
    {
        return true;
    }
    
    bool IoVideo::save(Image & image, JSON & data)
    {
        return true;
    }

    // -------------------------------------------
    // Function ran in a thread, which records for
    // a specific amount of time.
    
    void * recordContinuously(void * self)
    {
        IoVideo * video = (IoVideo *) self;
        
        double cronoPause = (double)cvGetTickCount();
        double cronoFPS = cronoPause;
        double cronoTime = 0;
        double timeElapsed = 0;
        double timeToSleep = 0;

        pthread_mutex_lock(&video->m_time_lock);
        double timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
        pthread_mutex_unlock(&video->m_time_lock);

        pthread_mutex_lock(&video->m_lock);
        Image image = video->m_capture->retrieve();
        video->m_mostRecentImage = image;
        pthread_mutex_unlock(&video->m_lock);
        
        while(video->m_capture && cronoTime < timeToRecord)
        {
            cronoFPS = (double) cv::getTickCount();
            
            try
            {
                // -----------------------------
                // Write the frames to the video

                pthread_mutex_lock(&video->m_lock);
                Image image = video->m_mostRecentImage;
                video->m_writer->write(image.getImage());
                pthread_mutex_unlock(&video->m_lock);
            }
            catch(cv::Exception & ex)
            {
                LERROR << ex.what();
            }

            // update time to record; (locking)
            pthread_mutex_lock(&video->m_time_lock);
            timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
            pthread_mutex_unlock(&video->m_time_lock);
            
            cronoPause = (double) cv::getTickCount();
            cronoTime = cronoPause / cv::getTickFrequency();
            timeElapsed = (cronoPause - cronoFPS) / cv::getTickFrequency();
            double fpsToTime = 1. / video->m_fps;
            timeToSleep = fpsToTime - timeElapsed;
            
            if(timeToSleep > 0)
            {
                usleep(timeToSleep * 1000 * 1000);
            }
            else
            {
                LINFO << "IoVideo: framerate is too fast, can't record video at this speed (" << video->m_fps << "/FPS)";
            }
        }
        
        video->m_recording = false;
        video->m_writer->release();
        delete video->m_writer;
        video->m_writer = 0;
    }
    
    // -------------------------------------------
    // Function ran in a thread, which records for
    // a specific amount of time.
    
    void * retrieveContinuously(void * self)
    {
        IoVideo * video = (IoVideo *) self;

        while(video->m_capture != 0)
        {
            if(video->m_recording)
            {
                pthread_mutex_lock(&video->m_capture_lock);

                // -----------------------------
                // Write the frames to the video
                Image image = video->m_capture->retrieve();
                
                pthread_mutex_lock(&video->m_lock);
                video->m_mostRecentImage = image;
                pthread_mutex_unlock(&video->m_lock);
                usleep((int)(1000*1000/video->m_fps));

                pthread_mutex_unlock(&video->m_capture_lock);
            }
            else
            {
                usleep(500*1000);
            }
        }
    }
    
    void IoVideo::startRecordThread()
    {
        pthread_create(&m_recordThread, NULL, recordContinuously, this);
    }

    void IoVideo::stopRecordThread()
    {
        pthread_cancel(m_recordThread);
        pthread_join(m_recordThread, NULL);
    }

    void IoVideo::startRetrieveThread()
    {
        pthread_create(&m_retrieveThread, NULL, retrieveContinuously, this);
    }
    
    void IoVideo::stopRetrieveThread()
    {
        pthread_cancel(m_retrieveThread);
        pthread_join(m_retrieveThread, NULL);
    }
}