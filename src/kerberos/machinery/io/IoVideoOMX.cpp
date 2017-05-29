#include "machinery/io/IoVideoOMX.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

omxcam_video_settings_t omx_settings;

int fd;
uint32_t current = 0;

int log_error (){
  omxcam_perror ();
  return 1;
}

void on_data_length (omxcam_buffer_t buffer){
  
  //Append the buffer to the file
  if (pwrite (fd, buffer.data, buffer.length, 0) == -1){
    fprintf (stderr, "error: pwrite\n");
    if (omxcam_video_stop ()) log_error ();
  }
}


int save_length (char* filename, omxcam_video_settings_t* settings){
  printf ("capturing %s\n", filename);
  
  fd = open (filename, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0666);
  if (fd == -1){
    fprintf (stderr, "error: open\n");
    return 1;
  }
  
  //Capture indefinitely
  if (omxcam_video_start (settings, 10000)){
    return log_error ();
  }
  
  //Close the file
  if (close (fd)){
    fprintf (stderr, "error: close\n");
    return 1;
  }
  
  return 0;
}

namespace kerberos
{
    void IoVideoOMX::setup(const StringMap & settings)
    {
        Io::setup(settings);
        
        // --------------------------
        // Get name from instance
        
        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);

        std::string timezone = settings.at("timezone");
        std::replace(timezone.begin(), timezone.end(), '-', '/');
        std::replace(timezone.begin(), timezone.end(), '$', '_');
        setTimezone(timezone);
        
        m_publicKey = settings.at("clouds.S3.publicKey");
        m_privateKey = settings.at("clouds.S3.privateKey");
        
        m_createSymbol = false;
        if(m_privateKey != "" && m_publicKey != "")
        {
            m_createSymbol = true;
        }
        
        // -------------------------------------------------------------
        // Filemanager is mapped to a directory and is used by an image
        // to save to the correct directory.
        
	// -------------------------------------------------------------
        // Filemanager is mapped to a directory and is used by an image
        // to save to the correct directory.

        setFileFormat(settings.at("ios.Video.fileFormat"));
        m_directory = settings.at("ios.Video.directory");
	
	        //1920x1080 @30fps by default
  //Capture a video of ~2000ms, 640x480 @90fps
  //Camera modes: http://www.raspberrypi.org/new-camera-mode-released
  //Note: Encode the file at 30fps
  omxcam_video_init (&omx_settings);
  omx_settings.on_data = on_data_length;
  omx_settings.camera.width = 1280;
  omx_settings.camera.height = 720;
  omx_settings.camera.framerate = 25;
  omx_settings.camera.exposure = OMXCAM_EXPOSURE_NIGHT; 
 
    }
    
    std::string IoVideoOMX::buildPath(std::string pathToVideo, JSON & data)
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
     
    void IoVideoOMX::fire(JSON & data)
    {
	LINFO << "yolo";

	  std::string pathToVideo = getFileFormat();
            m_fileName = buildPath(pathToVideo, data) + ".mp4";

	std::string videoPath = m_directory + m_fileName;

	save_length ((char*)videoPath.c_str(), &omx_settings);
    }

    bool IoVideoOMX::save(Image & image)
    {
        return true;
    }

    bool IoVideoOMX::save(Image & image, JSON & data)
    {
        return true;
    }
    
    void IoVideoOMX::disableCapture()
    {

    }
}

