#include "machinery/io/IoPushbullet.h"

namespace kerberos
{
    void IoPushbullet::setup(const StringMap & settings)
    {
        Io::setup(settings);

        // --------------------------
        // Get name from instance

        std::string instanceName = settings.at("name");
        setInstanceName(instanceName);

        // -------
        // Get url

        setUrl(settings.at("ios.Pushbullet.url").c_str());

      	//------
      	//set pushbullet token

      	setToken((settings.at("ios.Pushbullet.token").c_str()));

        // -------------
        // Set throttler

        throttle.setRate(std::stoi(settings.at("ios.Pushbullet.throttler")));

        // ----------------------------
        // Initialize connection object

        pushbulletConnection = new RestClient::Connection(m_url);
        pushbulletConnection->SetTimeout(5); // set connection timeout to 5s
        RestClient::HeaderFields headers;
        headers["Content-Type"] = "application/json";
        pushbulletConnection->SetHeaders(headers);
        pushbulletConnection->AppendHeader("Access-Token",m_pbToken);
    }

    bool IoPushbullet::save(Image & image, JSON & data)
    {
        if(throttle.canExecute())
        {
            // ---------------------------------------
            // Attach additional fields to JSON object

            JSON dataCopy,pbResp;
            JSON::AllocatorType& allocator = dataCopy.GetAllocator();
            dataCopy.CopyFrom(data, allocator);

            JSONValue instanceName;
            instanceName.SetString(getInstanceName().c_str(), allocator);
            dataCopy.AddMember("instanceName", instanceName, allocator);

            // -----------------------------
            // Convert JSON object to string

            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            dataCopy.Accept(writer);


      	    std::string upUrl;
      	    std::string fileUrl;
      	    RestClient::Response r;

	    /*
	     * Step 1 : save file to tmp and provide  path in a string var
	     */
	    	std::string tmpFile = "/tmp/detection.jpg";
		if(image.save(tmpFile))
		{

		    /*
		     * Step 2 : create push for file
		     */

		    r = pushbulletConnection->post("/v2/upload-request", "{\"file_name\":\"detection.jpg\",\"file_type\":\"image/jpeg\"}");

		    if(r.code == 200){
			pbResp.Parse(r.body.c_str());
			upUrl = pbResp["upload_url"].GetString();
			fileUrl = pbResp["file_url"].GetString();
			LINFO << "IoPushbullet: response to upload request " + r.body;

			/*
			* Step 3 : upload file to pushbullet
			*/
			if (pbUploadImage(tmpFile, upUrl)) {

			/*
			* Step 4 : create text push for detection
			*/
				r = pushbulletConnection->post("/v2/pushes", "{\"type\":\"file\",\"file_url\":\""+ fileUrl +"\"}");
				if(r.code==200)
					LINFO << "IoPushbullet: response to push file request " + r.body ;
		    	}

	     	   }
		}
            // -------------------
            // Send a message  to pushbullet

            r = pushbulletConnection->post("/v2/pushes", "{\"body\":\"Motion detected\",\"title\":\"Kios\",\"type\":\"note\"}");

            if(r.code == 200)
            {
            	LINFO << "IoPushbullet: response to post to pushbullet " + r.body;
                return true;
            }

            return false;
      }
      return true;
    }

	bool IoPushbullet::pbUploadImage(std::string tmpFile, std::string upUrl) {

		CURL *curl;
		CURLcode res;

		struct curl_httppost *formpost = NULL;
		struct curl_httppost *lastptr = NULL;

		curl_global_init(CURL_GLOBAL_ALL);
		curl_formadd(&formpost, &lastptr,
				CURLFORM_COPYNAME, "file",
				CURLFORM_FILENAME, "detection.jpg",
				CURLFORM_FILE, tmpFile.c_str(),
				CURLFORM_CONTENTTYPE, "image/jpeg",
				CURLFORM_END);

		curl = curl_easy_init();

		if (curl) {

		    curl_easy_setopt(curl, CURLOPT_URL, (upUrl).c_str());
		    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		    res = curl_easy_perform(curl);

		    curl_easy_cleanup(curl);
		    curl_formfree(formpost);

		    if (res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
		    	return false;
		    }
		}
		else
			return false;

		return true;
	}

}
