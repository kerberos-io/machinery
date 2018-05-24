#include "cloud/ForwardStream.h"

namespace kerberos
{
    void ForwardStream::setup(std::string publicKey, std::string deviceKey)
    {
        std::string ip = "159.65.215.225";
        int port = 1883;
        m_publicKey = publicKey;
        m_deviceKey = deviceKey;
        throttle.setRate(10);
        setTopic("kerberos/" + m_publicKey + "/device/" + m_deviceKey + "/live");
        setMotionTopic("kerberos/" + m_publicKey + "/device/" + m_deviceKey + "/motion");

        std::string clientId = publicKey + "-" + kerberos::helper::getTimestamp() + "-" + kerberos::helper::getMicroseconds();
        mosqpp::lib_init();
        reinitialise(clientId.c_str(), true);
	      username_pw_set(publicKey.c_str(),nullptr);
        connect_async(ip.c_str(), port);
      	loop_start();

        std::string timestamp = kerberos::helper::getTimestamp();
        m_lastReceived = std::stoi(timestamp) - 5;

        m_encode_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        m_encode_params.push_back(75);
    }

    cv::Mat ForwardStream::GetSquareImage(const cv::Mat& img, int target_width)
    {
        int width = img.cols,
           height = img.rows;

        cv::Mat square = cv::Mat::zeros( target_width, target_width, img.type() );

        int max_dim = ( width >= height ) ? width : height;
        float scale = ( ( float ) target_width ) / max_dim;
        cv::Rect roi;
        if ( width >= height )
        {
            roi.width = target_width;
            roi.x = 0;
            roi.height = height * scale;
            roi.y = ( target_width - roi.height ) / 2;
        }
        else
        {
            roi.y = 0;
            roi.height = target_width;
            roi.width = width * scale;
            roi.x = ( target_width - roi.width ) / 2;
        }

        cv::resize( img, square( roi ), roi.size() );

        return square;
    }

    bool ForwardStream::forward(Image & cleanImage)
    {
        cv::Mat & img = cleanImage.getImage();

        int maxWidth = 300;
        int newWidth = img.cols;
        int newHeight = img.rows;

        if(newWidth > maxWidth) {
          double ratio = newWidth / maxWidth;
          newWidth = newWidth / ratio;
          newHeight = newHeight / ratio;
        }

        cv::resize(img, img, cv::Size(newWidth,newHeight));
        //cv::Mat grayImg(img.cols, img.rows, CV_8UC1);
        //cvtColor(img, grayImg, cv::COLOR_RGB2GRAY);

        std::vector<uchar> buf;
        cv::imencode(".jpg", img, buf, m_encode_params);
        uchar *enc_msg = new uchar[buf.size()];
        for(int i=0; i < buf.size(); i++) enc_msg[i] = buf[i];
        std::string encoded = base64_encode(enc_msg, buf.size());

        int ret = publish(NULL, getTopic(), encoded.size(), encoded.c_str(), 2 ,false);
        return (ret == MOSQ_ERR_SUCCESS);
    }

    bool ForwardStream::forwardRAW(uint8_t * data, int32_t length)
    {
        std::string encoded = base64_encode((uchar *) data, length);
        int ret = publish(NULL, getTopic(), encoded.size(), encoded.c_str(), 2 ,false);
        return (ret == MOSQ_ERR_SUCCESS);
    }

    bool ForwardStream::isRequestingLiveStream()
    {
        std::string t = kerberos::helper::getTimestamp();
        int timestamp = std::stoi(t);
        return timestamp - m_lastReceived < 5;
    }

    void ForwardStream::on_connect(int rc)
    {
      	//printf("Connected with code %d.\n", rc);
      	if(rc == 0)
        {
            std::string topic = "kerberos/" + m_publicKey + "/device/" + m_deviceKey + "/request-live";
        		subscribe(NULL, topic.c_str());
      	}
    }

    void ForwardStream::on_message(const struct mosquitto_message *message)
    {
        //printf("Receive succeeded %s.\n", message->topic);
        std::string timestamp = kerberos::helper::getTimestamp();
        m_lastReceived = std::stoi(timestamp);
    }

    bool ForwardStream::triggerMotion()
    {
        if(throttle.canExecute())
        {
            std::string motion = "motion";
            int ret = publish(NULL, getMotionTopic(), motion.size(), motion.c_str(), 2 ,false);
            return (ret == MOSQ_ERR_SUCCESS);
        }

        return false;
    }

    void ForwardStream::on_subscribe(int mid, int qos_count, const int *granted_qos)
    {
        //printf("Subscription succeeded.\n");
    }

}
