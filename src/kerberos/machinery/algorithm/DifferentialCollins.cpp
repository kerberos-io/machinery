#include "machinery/algorithm/DifferentialCollins.h"

namespace kerberos
{
    void DifferentialCollins::setup(const StringMap & settings)
    {
        Algorithm::setup(settings);
        int erode = std::atoi(settings.at("algorithms.DifferentialCollins.erode").c_str());
        int threshold = std::atoi(settings.at("algorithms.DifferentialCollins.threshold").c_str());
        setErodeKernel(erode, erode);
        setThreshold(threshold);
    }
    
    // ---------------------------------------------
    // Convert all images (except last one) to gray

    void DifferentialCollins::initialize(ImageVector & images)
    {
        for(int i = 0; i < images.size()-1; i++)
        {
            images[i]->convert2Gray();
        }
    }

    Image DifferentialCollins::evaluate(ImageVector & images, JSON & data)
    {
        // -------------
        // Make gray

        images[2]->convert2Gray();

        // -----------
        // Calculate

        images[0]->difference(*images[2], h_d1);
        images[1]->difference(*images[2], h_d2);
        Image evaluation;
        h_d1.bitwiseAnd(h_d2, evaluation);
        evaluation.threshold(m_threshold);
        evaluation.erode(m_erodeKernel);

        return evaluation;
    }
    
    void DifferentialCollins::setErodeKernel(int width, int height)
    {
        m_erodeKernel.setImage(Image::createKernel(width, height));
    }
    
    void DifferentialCollins::setThreshold(int threshold)
    {
        m_threshold = threshold;
    }
}