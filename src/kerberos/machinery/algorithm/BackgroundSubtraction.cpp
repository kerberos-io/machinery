#include "machinery/algorithm/BackgroundSubtraction.h"

namespace kerberos
{
    void BackgroundSubtraction::setup(const StringMap & settings)
    {
        Algorithm::setup(settings);
        int erode = std::atoi(settings.at("algorithms.BackgroundSubtraction.erode").c_str());
        int dilate = std::atoi(settings.at("algorithms.BackgroundSubtraction.dilate").c_str());
        setErodeKernel(erode, erode);
        setDilateKernel(dilate, dilate);

        m_subtractor = cv::createBackgroundSubtractorMOG2();
        
        std::string shadows = settings.at("algorithms.BackgroundSubtraction.shadows");
        int history = std::atoi(settings.at("algorithms.BackgroundSubtraction.history").c_str());
        int nmixtures = std::atoi(settings.at("algorithms.BackgroundSubtraction.nmixtures").c_str());
        double ratio = std::atof(settings.at("algorithms.BackgroundSubtraction.ratio").c_str());
        int threshold = std::atoi(settings.at("algorithms.BackgroundSubtraction.threshold").c_str());
        m_subtractor->setDetectShadows((shadows == "true"));
        m_subtractor->setHistory(history);
        m_subtractor->setNMixtures(nmixtures);
        m_subtractor->setBackgroundRatio(ratio);
        m_subtractor->setVarThreshold(threshold);
        m_subtractor->setVarThresholdGen(threshold);
    }
    
    // ---------------------------------------------
    // Convert all images (except last one) to gray

    void BackgroundSubtraction::initialize(ImageVector & images)
    {
        for(int i = 0; i < images.size()-1; i++)
        {
            m_subtractor->apply(images[i]->getImage(), m_backgroud.getImage());
        }
    }

    Image BackgroundSubtraction::evaluate(ImageVector & images, JSON & data)
    {
        // -----------
        // Calculate

        m_subtractor->apply(images[2]->getImage(), m_backgroud.getImage());
        
        cv::Mat brackgroundmodel;
        m_subtractor->getBackgroundImage(brackgroundmodel);
        m_backgroud.erode(m_erodeKernel);
        m_backgroud.dilate(m_dilateKernel);
        
        return m_backgroud;
    }
    
    void BackgroundSubtraction::setErodeKernel(int width, int height)
    {
        m_erodeKernel.setImage(Image::createKernel(width, height));
    }
    
    void BackgroundSubtraction::setDilateKernel(int width, int height)
    {
        m_dilateKernel.setImage(Image::createKernel(width, height));
    }
    
    void BackgroundSubtraction::setThreshold(int threshold)
    {
        m_threshold = threshold;
    }
}