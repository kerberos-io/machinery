#include "machinery/expositor/HullExpositor.h"

namespace kerberos
{
    void HullExpositor::setup(const StringMap &settings)
    {
        Expositor::setup(settings);
        
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
        // Get width and height of camera
        
        int captureWidth = std::atoi(settings.at("capture.width").c_str());
        int captureHeight = std::atoi(settings.at("capture.height").c_str());
        
        // --------------------------------
        // Calculate points in hull
        m_points.clear();
        for(int j = 0; j < captureHeight; j++)
        {
            for(int i = 0; i < captureWidth; i++)
            {
                cv::Point2f p(i,j);
                if(cv::pointPolygonTest(coor, p, false) >= 0)
                {
                    m_points.push_back(p);
                }
            }
        }
    }
    
    void HullExpositor::calculate(Image & evaluation, JSON & data)
    {
        int x, y, size = int(m_points.size());
        
        int numberOfChanges = 0;
        Rectangle rectangle(evaluation.getColumns(), evaluation.getRows(), 0, 0);
        
        for(int i = 0; i < size; i++)
        {
            x = m_points[i].x;
            y = m_points[i].y;
            
            if(static_cast<int>(evaluation.get(y,x)) == 255)
            {
                numberOfChanges++;
                if(rectangle.m_x1>x) rectangle.m_x1 = x;
                if(rectangle.m_x2<x) rectangle.m_x2 = x;
                if(rectangle.m_y1>y) rectangle.m_y1 = y;
                if(rectangle.m_y2<y) rectangle.m_y2 = y;
            }
        }
        
        // --------------------------
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
            BINFO << "HullExpositor: activity detected from (" +  helper::to_string(rectangle.m_x1) + "," + helper::to_string(rectangle.m_y1) + ") to (" +  helper::to_string(rectangle.m_x2) + "," + helper::to_string(rectangle.m_y2) + ")";
        }

        data.AddMember("numberOfChanges", numberOfChanges, allocator);
    }
}
