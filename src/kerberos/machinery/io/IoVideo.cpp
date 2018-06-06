#include "machinery/io/IoVideo.h"

namespace kerberos
{
    void IoVideo::setup(const StringMap & settings)
    {
        Io::setup(settings);

        throttle.setRate(std::stoi(settings.at("ios.Video.throttler")));

        m_currentVideoPath = "";
        m_writer = 0;
        m_recording = false;
        pthread_mutex_init(&m_lock, NULL);
        pthread_mutex_init(&m_time_lock, NULL);
        pthread_mutex_init(&m_capture_lock, NULL);
        pthread_mutex_init(&m_write_lock, NULL);
        pthread_mutex_init(&m_release_lock, NULL);

        // ----------------------------------------
        // If privacy mode is enabled, we calculate
        // a mask to remove the public area.

        m_privacy = (settings.at("ios.Video.privacy") == "true");

        if(m_privacy)
        {
            // --------------------------------
            // Parse coordinates from config file
            //  - x,y|x,y|x,y|... => add to vectory as Point2f

            std::vector<cv::Point2f> coor;

            std::vector<std::string> coordinates;
            helper::tokenize(settings.at("expositors.Hull.region"), coordinates, "|");

            for(int i = 0; i < coordinates.size(); i++)
            {
                std::vector<std::string> fromAndTo;
                helper::tokenize(coordinates[i], fromAndTo, ",");
                int from = std::atoi(fromAndTo[0].c_str());
                int to = std::atoi(fromAndTo[1].c_str());
                Point2f p(from ,to);
                coor.push_back(p);
            }

            // -------------------------------
            // Get width and height of image

            Image preview = m_capture->retrieve();
            int captureWidth = preview.getColumns();
            int captureHeight = preview.getRows();

            // --------------------------------
            // Calculate points in hull

            PointVector points;
            points.clear();
            for(int j = 0; j < captureHeight; j++)
            {
                for(int i = 0; i < captureWidth; i++)
                {
                    cv::Point2f p(i,j);
                    if(cv::pointPolygonTest(coor, p, false) >= 0)
                    {
                        points.push_back(p);
                    }
                }
            }

            m_mask.createMask(captureWidth, captureHeight, points);
        }

        // --------------------------
        // Get name from instance

        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);

        m_fps = std::atoi(settings.at("ios.Video.fps").c_str());
        m_width = std::atoi(settings.at("capture.width").c_str());
        m_height = std::atoi(settings.at("capture.height").c_str());
        m_recordingTimeAfter = std::atoi(settings.at("ios.Video.recordAfter").c_str()); // in seconds
        m_maxDuration = std::atoi(settings.at("ios.Video.maxDuration").c_str()); // in seconds
        m_extension = settings.at("ios.Video.extension");
        std::string codec = settings.at("ios.Video.codec");

        m_publicKey = settings.at("clouds.S3.publicKey");
        m_privateKey = settings.at("clouds.S3.privateKey");

        m_createSymbol = false;
        if(m_privateKey != "" && m_publicKey != "")
        {
            m_createSymbol = true;
        }

        if(codec == "h264")
        {
            m_codec = 0x00000021;//CV_FOURCC('H','2','6','4');
        }
        else
        {
            m_codec = -1;
        }

        m_encodingBinary = "ffmpeg";
        if(!system("which avconv > /dev/null 2>&1")){
            m_encodingBinary = "avconv";
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

        setVideoFileFormat(settings.at("ios.Video.fileFormat"));
        setImageFileFormat(settings.at("ios.Disk.fileFormat"));
        m_directory = settings.at("ios.Video.directory");
        m_fileManager.setBaseDirectory(m_directory);

        m_hardwareDirectory = settings.at("ios.Video.hardwareDirectory");
        m_enableHardwareEncoding = (settings.at("ios.Video.enableHardwareEncoding") == "true");

        // ------------------------
        // Start conversion thread.

        startConvertThread();
    }

    cv::Scalar IoVideo::getColor(const std::string name)
    {
        std::map<std::string, cv::Scalar> m_colors;

        m_colors["white"] = cv::Scalar(255,255,255);
        m_colors["black"] = cv::Scalar(0,0,0);
        m_colors["red"] = cv::Scalar(0,0,255);
        m_colors["green"] = cv::Scalar(0,255,0);
        m_colors["blue"] = cv::Scalar(255,0,0);

        if(name == "none")
        {
            return m_colors.at("white");
        }
        else
        {
            return m_colors.at(name);
        }
    }

    std::string IoVideo::buildPath(std::string pathToImage)
    {
        // -----------------------------------------------
        // Get timestamp, microseconds, random token, and instance name

        std::string instanceName = getInstanceName();
        kerberos::helper::replace(pathToImage, "instanceName", instanceName);

        std::string timestamp = kerberos::helper::getTimestamp();
        kerberos::helper::replace(pathToImage, "timestamp", timestamp);

        std::string microseconds = kerberos::helper::getMicroseconds();
        std::string size = kerberos::helper::to_string((int)microseconds.length());
        kerberos::helper::replace(pathToImage, "microseconds", size + "-" + microseconds);

        std::string token = kerberos::helper::to_string(rand()%1000);
        kerberos::helper::replace(pathToImage, "token", token);

        return pathToImage;
    }

    std::string IoVideo::buildPath(std::string pathToVideo, JSON & data)
    {
         // ------------------------------------------
        // Stringify data object: build image path
        // with data information.

        static const std::string kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
        for (JSONValue::ConstMemberIterator itr = data.MemberBegin(); itr != data.MemberEnd(); ++itr)
        {
            std::string name = itr->name.GetString();
            std::string type = kTypeNames[itr->value.GetType()];

            if(type == "String")
            {
                std::string value = itr->value.GetString();
                kerberos::helper::replace(pathToVideo, name, value);
            }
            else if(type == "Number")
            {
                std::string value = kerberos::helper::to_string(itr->value.GetInt());
                kerberos::helper::replace(pathToVideo, name, value);
            }
            else if(type == "Array")
            {
                std::string arrayString = "";
                for (JSONValue::ConstValueIterator itr2 = itr->value.Begin(); itr2 != itr->value.End(); ++itr2)
                {
                    type = kTypeNames[itr2->GetType()];

                    if(type == "String")
                    {
                        arrayString += itr2->GetString();
                    }
                    else if(type == "Number")
                    {
                       arrayString += kerberos::helper::to_string(itr2->GetInt());
                    }

                    arrayString += "-";
                }
                kerberos::helper::replace(pathToVideo, name, arrayString.substr(0, arrayString.size()-1));
            }
        }
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

    void IoVideo::fire(JSON & data)
    {
        if(throttle.canExecute())
        {
            // ----------------------------------------------------------
            // If a video is recording, and a new detection is coming in,
            // we'll reset the timer. So the video is expaned.
            // timer = ...

            pthread_mutex_lock(&m_time_lock);
            m_timeStartedRecording = (double) (cv::getTickCount() / cv::getTickFrequency());
            pthread_mutex_unlock(&m_time_lock);

            // -----------------------------------------------------
            // Check if already recording, if not start a new video

            pthread_mutex_lock(&m_release_lock);

            LINFO << "IoVideo: firing";

            // ------------------
            // Check if the camera supports on board recording (camera specific),
            // and if you want to use it. If not it will fallback on the video writer
            // that ships with OpenCV/FFmpeg.

            if(m_capture->m_onBoardRecording && m_enableHardwareEncoding)
            {
                if(!m_recording)
                {
                    // ----------------------------------------
                    // The naming convention that will be used
                    // for the image.

                    std::string pathToVideo = getVideoFormat();
                    m_fileName = buildPath(pathToVideo, data);
                    m_path = m_hardwareDirectory + m_fileName + ".h264";

                    std::string expectedPath = m_fileName + "." + m_extension;
                    m_currentVideoPath = expectedPath;

                    // ---------------
                    // Start recording

                    startOnboardRecordThread();
                    m_recording = true;
                }
            }
            // Won't use as we use hardware encoding in the else branch beneath.
            // Found a work-a-round how to use h264_omx directly with OpenCV and FFMPEG.
            /*else if(m_capture->m_onFFMPEGrecording)
            {
                if(!m_recording)
                {
                    // ----------------------------------------
                    // The naming convention that will be used
                    // for the image.

                    std::string pathToVideo = getVideoFormat();
                    m_fileName = buildPath(pathToVideo, data) + "." + m_extension;
                    m_path = m_directory + m_fileName;

                    startFFMPEGRecordThread();
                    m_recording = true;
                }
            }*/
            else // Use built-in OpenCV
            {
                if(m_capture && m_writer == 0 && !m_recording)
                {
                    // ----------------------------------------
                    // The naming convention that will be used
                    // for the image.

                    std::string pathToVideo = getVideoFormat();
                    m_fileName = buildPath(pathToVideo, data) + "." + m_extension;
                    m_path = m_directory + m_fileName;
                    Image image = m_capture->retrieve();
                    m_currentVideoPath = m_fileName;

                    // ---------------
                    // Start recording

                    LINFO << "IoVideo: start new recording " << m_fileName;

                    m_writer = new cv::VideoWriter();
                    m_writer->open(m_path, m_codec, m_fps, cv::Size(image.getColumns(), image.getRows()));

                    startRecordThread();
                    m_recording = true;
                }
            }

            // -------------------------------------------------------
            // Add path to JSON object, so other IO devices can use it

            JSONValue path;
            JSON::AllocatorType& allocator = data.GetAllocator();
            path.SetString(m_currentVideoPath.c_str(), allocator);
            data.AddMember("pathToVideo", path, allocator);

            pthread_mutex_unlock(&m_release_lock);
        }
    }

    void IoVideo::disableCapture()
    {
        pthread_mutex_lock(&m_write_lock);
        pthread_mutex_lock(&m_capture_lock);
        pthread_mutex_lock(&m_release_lock);

        m_recording = false;
        m_writer = 0;
        m_capture = 0; // remove capture device

        pthread_mutex_unlock(&m_release_lock);
        pthread_mutex_unlock(&m_capture_lock);
        pthread_mutex_unlock(&m_write_lock);
    }

    bool IoVideo::save(Image & image)
    {
        // ---------------------
        // Apply mask if enabled

        if(m_privacy)
        {
            image.bitwiseAnd(m_mask, image);
        }

        // ----------------------------------------
        // The naming convention that will be used
        // for the image.

        std::string pathToImage = getImageFormat();

        // ---------------------
        // Replace variables

        pathToImage = buildPath(pathToImage);

        if(!m_capture->m_onBoardRecording && !m_enableHardwareEncoding)
        {
            std::string timestamp = kerberos::helper::getTimestamp();
            kerberos::helper::replace(pathToImage, "timestamp", timestamp);
            drawDateOnImage(image, timestamp);
        }

        // ---------------------------------------------------------------------
        // Save original version & generate unique timestamp for current image

        return m_fileManager.save(image, pathToImage, false);
    }

    bool IoVideo::save(Image & image, JSON & data)
    {
        return true;
    }

    void * recordOnFFMPEG(void * self)
    {
        IoVideo * video = (IoVideo *) self;

        double cronoPause = (double)cvGetTickCount();
        double cronoTime = (double) (cv::getTickCount() / cv::getTickFrequency());
        double startedRecording = cronoTime;

        LINFO << "IoVideo (FFMPEG): start writing images";

        pthread_mutex_lock(&video->m_write_lock);

        // Todo write video with FFMPEG..
        // .. (video->m_path);

        LINFO << "IoVideo: locked write thread";

        pthread_mutex_lock(&video->m_time_lock);
        double timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
        pthread_mutex_unlock(&video->m_time_lock);

        try
        {
            while(cronoTime < timeToRecord
                && cronoTime - startedRecording <= video->m_maxDuration) // lower than max recording time (especially for memory)
            {
                // update time to record; (locking)
                pthread_mutex_lock(&video->m_time_lock);
                timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
                pthread_mutex_unlock(&video->m_time_lock);

                cronoPause = (double) cv::getTickCount();
                cronoTime = cronoPause / cv::getTickFrequency();

                usleep(1000); // sleep 1s
            }
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&video->m_lock);
            pthread_mutex_unlock(&video->m_time_lock);
            LERROR << ex.what();
        }

        LINFO << "IoVideo: end writing images";

        pthread_mutex_lock(&video->m_release_lock);

        try
        {
            // Todo stop writing video with FFMPEG..
            // ...
            video->m_recording = false;

            if(video->m_createSymbol)
            {
                std::string link = SYMBOL_DIRECTORY + video->m_fileName;
                std::string pathToVideo = video->m_directory + video->m_fileName;
                symlink(pathToVideo.c_str(), link.c_str());
            }
        }
        catch(cv::Exception & ex)
        {
            LERROR << ex.what();
        }


        LINFO << "IoVideo: remove videowriter";

        pthread_mutex_unlock(&video->m_release_lock);
        pthread_mutex_unlock(&video->m_write_lock);

        LINFO << "IoVideo: unlocking write thread";
    }

    void * recordOnboard(void * self)
    {
        IoVideo * video = (IoVideo *) self;

        double cronoPause = (double)cvGetTickCount();
        double cronoTime = (double) (cv::getTickCount() / cv::getTickFrequency());
        double startedRecording = cronoTime;

        LINFO << "IoVideo (OnBoard): start writing images";

        pthread_mutex_lock(&video->m_write_lock);

        video->m_capture->startRecord(video->m_path);

        LINFO << "IoVideo: locked write thread";

        pthread_mutex_lock(&video->m_time_lock);
        double timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
        pthread_mutex_unlock(&video->m_time_lock);

        try
        {
            while(cronoTime < timeToRecord
                && cronoTime - startedRecording <= video->m_maxDuration) // lower than max recording time (especially for memory)
            {
                // update time to record; (locking)
                pthread_mutex_lock(&video->m_time_lock);
                timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
                pthread_mutex_unlock(&video->m_time_lock);

                cronoPause = (double) cv::getTickCount();
                cronoTime = cronoPause / cv::getTickFrequency();

                usleep(1000); // sleep 1s
            }
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&video->m_lock);
            pthread_mutex_unlock(&video->m_time_lock);
            LERROR << ex.what();
        }

        LINFO << "IoVideo: end writing images";

        pthread_mutex_lock(&video->m_release_lock);

        try
        {
            // create link to start processing to mp4
            std::string link = SYMBOL_DIRECTORY + video->m_fileName + ".h264";
            symlink(video->m_path.c_str(), link.c_str());

            // stop recording thread
            video->m_capture->stopRecord();
            video->m_recording = false;
        }
        catch(cv::Exception & ex)
        {
            LERROR << ex.what();
        }


        LINFO << "IoVideo: remove videowriter";

        pthread_mutex_unlock(&video->m_release_lock);
        pthread_mutex_unlock(&video->m_write_lock);

        LINFO << "IoVideo: unlocking write thread";
    }

    // -------------------------------------------
    // Function ran in a thread, which records for
    // a specific amount of time.

    void * recordContinuously(void * self)
    {
        IoVideo * video = (IoVideo *) self;

        double tickFrequency = cv::getTickFrequency();
        double cronoPause = (double)cvGetTickCount();
        double cronoFPS = cronoPause;
        double cronoTime = (double) (cv::getTickCount() / tickFrequency);
        double timeToSleep = 0;
        double startedRecording = cronoTime;
        double fpsToTime = 1. / video->m_fps;

        LINFO << "IoVideo (OpenCV): start writing images";

        pthread_mutex_lock(&video->m_write_lock);

        LINFO << "IoVideo: locked write thread";

        pthread_mutex_lock(&video->m_time_lock);
        double timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
        pthread_mutex_unlock(&video->m_time_lock);

        video->m_mostRecentImage = video->getImage();
        video->startRetrieveThread();

        try
        {
            while(cronoTime < timeToRecord
                && cronoTime - startedRecording <= video->m_maxDuration) // lower than max recording time (especially for memory)
            {
                cronoFPS = (double) cv::getTickCount();

                // -----------------------------
                // Write the frames to the video

                pthread_mutex_lock(&video->m_lock);
                video->m_writer->write(video->m_mostRecentImage.getImage());
                pthread_mutex_unlock(&video->m_lock);

                BINFO << "IoVideo: writing image";

                // update time to record; (locking)
                pthread_mutex_lock(&video->m_time_lock);
                timeToRecord = video->m_timeStartedRecording + video->m_recordingTimeAfter;
                pthread_mutex_unlock(&video->m_time_lock);

                cronoPause = (double) cv::getTickCount();
                cronoTime = cronoPause / tickFrequency;
                timeToSleep = fpsToTime - ((cronoPause - cronoFPS) / tickFrequency);

                if(timeToSleep > 0)
                {
                    usleep(timeToSleep * 1000 * 1000);
                }
                else
                {
                    LINFO << "IoVideo: framerate is too fast, can't record video at this speed (" << video->m_fps << "/FPS)";
                }
            }
        }
        catch(cv::Exception & ex)
        {
            pthread_mutex_unlock(&video->m_lock);
            pthread_mutex_unlock(&video->m_time_lock);
            LERROR << ex.what();
        }

        LINFO << "IoVideo: end writing images";

        pthread_mutex_lock(&video->m_release_lock);

        try
        {
            if(video->m_writer)
            {
                if(video->m_writer->isOpened())
                {
                    video->m_writer->release();
                }
                delete video->m_writer;
                video->m_writer = 0;
            }
            video->m_recording = false;

            if(video->m_createSymbol)
            {
                std::string link = SYMBOL_DIRECTORY + video->m_fileName;
                std::string pathToVideo = video->m_directory + video->m_fileName;
                symlink(pathToVideo.c_str(), link.c_str());
            }
        }
        catch(cv::Exception & ex)
        {
            LERROR << ex.what();
        }


        LINFO << "IoVideo: remove videowriter";

        pthread_mutex_unlock(&video->m_release_lock);
        pthread_mutex_unlock(&video->m_write_lock);

        LINFO << "IoVideo: unlocking write thread";
    }

    void IoVideo::drawDateOnImage(Image & image, std::string timestamp)
    {
        if(m_drawTimestamp)
        {
            struct tm tstruct;
            char buf[80];

            time_t now = std::atoi(timestamp.c_str());

            char * timeformat = "%d-%m-%Y %X";
            if(m_timezone != "")
            {
                setenv("TZ", m_timezone.c_str(), 1);
                tzset();
            }

            tstruct = *localtime(&now);
            strftime(buf, sizeof(buf), timeformat, &tstruct);

            cv::rectangle(image.getImage(), cv::Point(7,16), cv::Point(200,37), CV_RGB(0,0,0), CV_FILLED);
            cv::putText(image.getImage(), buf, cv::Point(10,30), cv::FONT_HERSHEY_SIMPLEX, 0.5, getTimestampColor());
        }
    }

    // -------------------------------------------
    // Function ran in a thread, which records for
    // a specific amount of time.

    void * retrieveContinuously(void * self)
    {
        IoVideo * video = (IoVideo *) self;

        bool recording = true;


        LINFO << "IoVideo: initializing capture thread";

        while(recording)
        {
            pthread_mutex_lock(&video->m_capture_lock);

            pthread_mutex_lock(&video->m_release_lock);
            recording = video->m_recording;
            pthread_mutex_unlock(&video->m_release_lock);

            if(video->m_capture != 0 && recording)
            {

                BINFO << "IoVideo: grabbing images";

                try
                {
                    Image image = video->getImage();

                    pthread_mutex_lock(&video->m_lock);
                    video->m_mostRecentImage = image;
                    pthread_mutex_unlock(&video->m_lock);

                    usleep((int)(500*1000/video->m_fps)); // Retrieve a little bit faster than the writing frame rate
                }
                catch(cv::Exception & ex)
                {
                    pthread_mutex_unlock(&video->m_lock);
                    LERROR << ex.what();
                }
            }

            pthread_mutex_unlock(&video->m_capture_lock);
            usleep(1000); // sleep 1 ms
        }

        LINFO << "IoVideo: closing capture thread";
    }

    Image IoVideo::getImage()
    {
        // -----------------------------
        // Write the frames to the video

        Image image = m_capture->retrieve();

        if(m_capture->m_angle != 0)
        {
            image.rotate(m_capture->m_angle);
        }

        // ---------------------
        // Apply mask if enabled

        if(m_privacy)
        {
            image.bitwiseAnd(m_mask, image);
        }

        // ------------------
        // Draw date on image

        drawDateOnImage(image, kerberos::helper::getTimestamp());

        return image;
    }

    void IoVideo::startOnboardRecordThread()
    {
        pthread_create(&m_recordOnboardThread, NULL, recordOnboard, this);
        pthread_detach(m_recordOnboardThread);
    }

    void IoVideo::stopOnboardRecordThread()
    {
        pthread_cancel(m_recordOnboardThread);
    }

    void IoVideo::startFFMPEGRecordThread()
    {
        pthread_create(&m_recordOnFFMPEGThread, NULL, recordOnFFMPEG, this);
        pthread_detach(m_recordOnFFMPEGThread);
    }

    void IoVideo::stopFFMPEGRecordThread()
    {
        pthread_cancel(m_recordOnFFMPEGThread);
    }

    void IoVideo::startRecordThread()
    {
        pthread_create(&m_recordThread, NULL, recordContinuously, this);
        pthread_detach(m_recordThread);
    }

    void IoVideo::stopRecordThread()
    {
        pthread_cancel(m_recordThread);
    }

    void IoVideo::startRetrieveThread()
    {
        pthread_create(&m_retrieveThread, NULL, retrieveContinuously, this);
        pthread_detach(m_retrieveThread);
    }

    void IoVideo::stopRetrieveThread()
    {
        pthread_cancel(m_retrieveThread);
    }

    void IoVideo::scan()
    {
        int framerate = m_capture->m_framerate;
        std::string directory = m_directory;
        std::string extension = m_extension;

        while(m_convertThread_running)
        {
            std::vector<std::string> storage;
            helper::getFilesInDirectory(storage, SYMBOL_DIRECTORY); // get all symbol links of directory

            std::vector<std::string>::iterator it = storage.begin();
            while(it != storage.end() && !m_recording) // When videos to process and not recording.
            {
                std::string file = *it;

                std::vector<std::string> fileParts;
                helper::tokenize(file, fileParts, ".");

                if(fileParts[1] != "h264")
                {
                    it++;
                    continue;
                }

                // convert from h264 to mp4 with avconv of ffmpeg
                // (ideally this should be executed in a seperate thread).
                std::string originalFile = helper::returnPathOfLink(file.c_str());

                // Strip filename from path
                fileParts.clear();
                helper::tokenize(originalFile, fileParts, "/");
                std::string name = fileParts[fileParts.size()-1];
                fileParts.clear();
                helper::tokenize(name, fileParts, ".");

                std::string mp4File = directory + fileParts[0] + "." + extension;

                std::string command = m_encodingBinary; // ffmpeg or avconv
                command += " -framerate " + std::to_string(framerate);
                command += " -i " + originalFile;
                command += " -c copy " + mp4File;
                system(command.c_str());

                unlink(file.c_str()); // remove symbol link.
                unlink(originalFile.c_str()); // remove h264 file.

                if(m_createSymbol)
                {
                    std::string link = SYMBOL_DIRECTORY + fileParts[0] + "." + extension;
                    symlink(mp4File.c_str(), link.c_str());
                }

                it++;

                usleep(5000*1000); // wait for 5 seconds.
            }

            usleep(5000*1000); // wait for 5 seconds.
        }
    }

    // --------------
    // Convert thread

    void * convertContinuously(void * self)
    {
        IoVideo * video = (IoVideo *) self;
        video->scan();
    }

    void IoVideo::startConvertThread()
    {
        m_convertThread_running = true;
        pthread_create(&m_convertThread, NULL, convertContinuously, this);
    }

    void IoVideo::stopConvertThread()
    {
        m_convertThread_running = false;
        pthread_cancel(m_convertThread);
        pthread_join(m_convertThread, NULL);
    }
}
