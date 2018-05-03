#include "cloud/ForwardStream.h"

namespace kerberos
{
    void ForwardStream::setup(std::string publicKey, std::string deviceKey)
    {
        std::string ip = "37.139.29.235";
        int port = 1883;
        m_publicKey = publicKey;
        m_deviceKey = deviceKey;
        setTopic("kerberos/" + m_publicKey + "/device/" + m_deviceKey + "/live");
        reinitialise(m_publicKey.c_str(), true);
        mosqpp::lib_init();
	      username_pw_set(publicKey.c_str(),nullptr);
        connect_async(ip.c_str(), port);
      	loop_start();

        std::string timestamp = kerberos::helper::getTimestamp();
        m_lastReceived = std::stoi(timestamp) - 10;

        m_encode_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        m_encode_params.push_back(20);
    }

    bool ForwardStream::forward(Image & cleanImage)
    {
        cv::Mat img = cleanImage.getImage();

        std::vector<uchar> buf;
        cv::imencode(".jpg", img, buf, m_encode_params);
        uchar *enc_msg = new uchar[buf.size()];
        for(int i=0; i < buf.size(); i++) enc_msg[i] = buf[i];
        std::string encoded = base64_encode(enc_msg, buf.size());

        int ret = publish(NULL, getTopic(), encoded.size(), encoded.c_str(), 0 ,true);
        return (ret == MOSQ_ERR_SUCCESS);
    }

    bool ForwardStream::isRequestingLiveStream()
    {
        std::string t = kerberos::helper::getTimestamp();
        int timestamp = std::stoi(t);
        return timestamp - m_lastReceived < 10;
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

    void ForwardStream::on_subscribe(int mid, int qos_count, const int *granted_qos)
    {
        //printf("Subscription succeeded.\n");
    }

}
