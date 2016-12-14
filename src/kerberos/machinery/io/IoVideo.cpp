#include "machinery/io/IoVideo.h"

namespace kerberos
{
    void IoVideo::setup(const StringMap & settings)
    {
        Io::setup(settings);
        
        m_writer = 0;
        m_recording = false;
        pthread_mutex_init(&m_lock, NULL);
        startRetrieveThread();
        
        // --------------------------
        // Get name from instance
        
        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);

        m_fps = std::stoi(settings.at("ios.Video.fps"));
        m_width = std::stoi(settings.at("capture.width"));
        m_height = std::stoi(settings.at("capture.height"));
        m_recordingTimeAfter = std::stoi(settings.at("ios.Video.recordAfter")); // in seconds
        m_extension = settings.at("ios.Video.extension");
        std::string codec = settings.at("ios.Video.codec");
        
        if(codec == "h264")
        {
            m_codec = CV_FOURCC('H','2','6','4');
        }
        else
        {
            m_codec = CV_FOURCC('M','J','P','G');
        }

        // --------------------------
        // Check if need to draw timestamp
        
        bool drawTimestamp = (settings.at("ios.Video.markWithTimestamp") == "true");
        setDrawTimestamp(drawTimestamp);
        cv::Scalar color = getColor(settings.at("ios.Video.timestampColor"));
        setTimestampColor(color);
        
        std::string timezone = settings.at("timezone");
        std::replace(timezone.begin(), timezone.end(), '-', '/');
        std::replace(timezone.begin(), timezone.end(), '$', '_');
        setTimezone(timezone);
        
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

    void IoVideo::fire()
    {
        m_recording = true;

        // ----------------------------------------------------------
        // If a video is recording, and a new detection is coming in,
        // we'll reset the timer. So the video is expaned.
        // timer = ...
        
        
        // -----------------------------------------------------
        // Check if already recording, if not start a new video
        
        if(m_writer == 0)
        {
            srand(time(NULL));
            std::string timestamp = kerberos::helper::getTimestamp();
            std::string file = timestamp + "_" + helper::to_string(rand());
            
            m_writer = new cv::VideoWriter();
            m_writer->open(m_directory + file + "." + m_extension, CV_FOURCC('H','2','6','4'), m_fps, cv::Size(m_width, m_height), true);
            
            stopRecordThread();
            startRecordThread();
        }
    }

    void IoVideo::disableCapture()
    {
        stopRecordThread();
        stopRetrieveThread();
        m_capture = 0;
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
        
        pthread_mutex_lock(&(video->m_lock));
        Image image = video->m_capture->retrieve();
        pthread_mutex_unlock(&(video->m_lock));
        
        int i = 0;
        while(i < video->m_recordingTimeAfter * video->m_fps) // this will need to be replaced by realtime.
        {
            try
            {
                // -----------------------------
                // Write the frames to the video

                pthread_mutex_lock(&video->m_lock);
                Image image = video->m_mostRecentImage;
                video->m_writer->write(image.getImage());
                pthread_mutex_unlock(&video->m_lock);
                
                i++;
            }
            catch(cv::Exception & ex)
            {
                LERROR << ex.what();
            }

            usleep((int)(1000*1000/video->m_fps)); // check how long need to sleep depends on time that can be writter to video.
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

        for(;;)
        {
            if(video->m_capture && video->m_recording)
            {
                // -----------------------------
                // Write the frames to the video
                Image image = video->m_capture->retrieve();
                
                pthread_mutex_lock(&video->m_lock);
                video->m_mostRecentImage = image;
                pthread_mutex_unlock(&video->m_lock);
                usleep((int)(1000*1000/video->m_fps));
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