#include "machinery/expositor/RectangleExpositor.h"

namespace kerberos
{
    void RectangleExpositor::setup(const StringMap & settings)
    {
        Expositor::setup(settings);
        int x1 = std::atoi(settings.at("expositors.Rectangle.region.x1").c_str());
        int y1 = std::atoi(settings.at("expositors.Rectangle.region.y1").c_str());
        int x2 = std::atoi(settings.at("expositors.Rectangle.region.x2").c_str());
        int y2 = std::atoi(settings.at("expositors.Rectangle.region.y2").c_str());
        setCoordinates(x1, y1, x2, y2);
    }

    void RectangleExpositor::setCoordinates(const int x1, const int y1, const int x2, const int y2)
    {
        m_x1 = x1;
        m_y1 = y1;
        m_x2 = x2;
        m_y2 = y2;
    }
    
    void RectangleExpositor::calculate(Image & evaluation, JSON & data)
    {
        int numberOfChanges = 0;
        Rectangle rectangle(evaluation.getColumns(), evaluation.getRows(), 0, 0);

        // -----------------------------------
        // loop over image and detect changes

        for(int i = m_x1; i < m_x2; i++)
        {
            for(int j = m_y1; j < m_y2; j++)
            {
                if(static_cast<int>(evaluation.get(j,i)) == 255)
                {
                    numberOfChanges++;
                    if(rectangle.m_x1>i) rectangle.m_x1 = i;
                    if(rectangle.m_x2<i) rectangle.m_x2 = i;
                    if(rectangle.m_y1>j) rectangle.m_y1 = j;
                    if(rectangle.m_y2<j) rectangle.m_y2 = j;
                }
            }
        }
        
        // -------------------------
        //check if not out of bounds
        
        if(rectangle.m_x1-10 > 0) rectangle.m_x1 -= 10;
        if(rectangle.m_y1-10 > 0) rectangle.m_y1 -= 10;
        if(rectangle.m_x2+10 < evaluation.getColumns()-1) rectangle.m_x2 += 10;
        if(rectangle.m_y2+10 < evaluation.getRows()-1) rectangle.m_y2 += 10;
        
        // --------------------------
        // Add coordinates to object
        
        JSON::AllocatorType& allocator = data.GetAllocator();
        
        JSONValue region;
        region.SetArray();
        region.PushBack(rectangle.m_x1, allocator);
        region.PushBack(rectangle.m_y1, allocator);
        region.PushBack(rectangle.m_x2, allocator);
        region.PushBack(rectangle.m_y2, allocator);

        data.AddMember("regionCoordinates", region, allocator);
        
        // --------------------------
        // Add number of changes

        if(numberOfChanges)
        {
            BINFO << "RectangleExpositor: activity detected from (" +  helper::to_string(rectangle.m_x1) + "," + helper::to_string(rectangle.m_y1) + ") to (" +  helper::to_string(rectangle.m_x2) + "," + helper::to_string(rectangle.m_y2) + ")";
        }

        data.AddMember("numberOfChanges", numberOfChanges, allocator);
    }
}