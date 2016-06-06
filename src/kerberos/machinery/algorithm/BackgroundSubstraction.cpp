#include "machinery/algorithm/BackgroundSubstraction.h"

namespace kerberos
{
    void BackgroundSubstraction::setup(const StringMap & settings)
    {
        Algorithm::setup(settings);
        int erode = std::atoi(settings.at("algorithms.BackgroundSubstraction.erode").c_str());
        int dilate = std::atoi(settings.at("algorithms.BackgroundSubstraction.dilate").c_str());
        setErodeKernel(erode, erode);
        setDilateKernel(dilate, dilate);

        m_substractor = cv::createBackgroundSubtractorMOG2();
        
        std::string shadows = settings.at("algorithms.BackgroundSubstraction.shadows");
        int history = std::atoi(settings.at("algorithms.BackgroundSubstraction.history").c_str());
        int nmixtures = std::atoi(settings.at("algorithms.BackgroundSubstraction.nmixtures").c_str());
        double ratio = std::atof(settings.at("algorithms.BackgroundSubstraction.ratio").c_str());
        int threshold = std::atoi(settings.at("algorithms.BackgroundSubstraction.threshold").c_str());
        m_substractor->setDetectShadows((shadows == "true"));
        m_substractor->setHistory(history);
        m_substractor->setNMixtures(nmixtures);
        m_substractor->setBackgroundRatio(ratio);
        m_substractor->setVarThreshold(threshold);
        m_substractor->setVarThresholdGen(threshold);
    }
    
    // ---------------------------------------------
    // Convert all images (except last one) to gray

    void BackgroundSubstraction::initialize(ImageVector & images)
    {
        for(int i = 0; i < images.size()-1; i++)
        {
            m_substractor->apply(images[i]->getImage(), m_backgroud.getImage());
        }
    }

    Image BackgroundSubstraction::evaluate(ImageVector & images, JSON & data)
    {
        // -----------
        // Calculate

        m_substractor->apply(images[2]->getImage(), m_backgroud.getImage());
        
        cv::Mat brackgroundmodel;
        m_substractor->getBackgroundImage(brackgroundmodel);
        m_backgroud.erode(m_erodeKernel);
        m_backgroud.dilate(m_dilateKernel);
        
        return m_backgroud;
    }
    
    void BackgroundSubstraction::setErodeKernel(int width, int height)
    {
        m_erodeKernel.setImage(Image::createKernel(width, height));
    }
    
    void BackgroundSubstraction::setDilateKernel(int width, int height)
    {
        m_dilateKernel.setImage(Image::createKernel(width, height));
    }
    
    void BackgroundSubstraction::setThreshold(int threshold)
    {
        m_threshold = threshold;
    }
}