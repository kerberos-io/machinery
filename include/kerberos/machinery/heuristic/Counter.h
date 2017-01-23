//
//  Class: Counter
//  Description: The counter heuristic will check if a valid in-out (out-in) 
//               happened. The detections in and out are kept in memory.
//               E.g. total people in the building.
//  Created:     28/04/2015
//  Author:      Cédric Verstraeten
//  Mail:        cedric@verstraeten.io
//  Website:     www.verstraeten.io
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied under 
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#ifndef __Counter_H_INCLUDED__   // if Counter.h hasn't been included yet...
#define __Counter_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "machinery/heuristic/Heuristic.h"

namespace kerberos
{
    char CounterName[] = "Counter";
    
    class Feature
    {
        private:
            int x;
            int y;
            int area;
            int appearance;

        public:
            Feature(int x, int y, int area, int appearance):x(x), y(y), area(area), appearance(appearance){};
            int getX(){return x;}
            int getY(){return y;}
            int getArea(){return area;}
            int getAppearance(){return appearance;}

            double distance(Feature f)
            {
                return sqrt((f.x - x) * (f.x - x) + (f.y - y) * (f.y - y));
            }

            double areaDistance(Feature f)
            {
                return sqrt((f.area - area) * (f.area - area));
            }

            void decreaseAppearance()
            {
                appearance--;
            }
    };

    enum Direction{left, right, top, bottom, parallell};

    class Counter : public HeuristicCreator<CounterName, Counter>
    {
        private:
            std::vector<cv::Point> m_in;
            std::vector<cv::Point> m_out;
            int m_minimumChanges;
            int m_noMotionDelayTime;
            int m_appearance;
            int m_maxDistance;
            int m_minArea;
            bool m_onlyTrueWhenCounted;

            std::vector<std::vector<Feature> > m_features;
            cv::Mat m_status;
        
        public:
            Counter(){}
            void setup(const StringMap & settings);
            void setMinimumChanges(int changes){m_minimumChanges=changes;};
            void setNoMotionDelayTime(int delay){m_noMotionDelayTime=delay;};
            void setAppearance(int appearance){m_appearance=appearance;};
            void setMaxDistance(int maxDistance){m_maxDistance=maxDistance;};
            void setMinArea(int minArea){m_minArea=minArea;};
            void setOnlyTrueWhenCounted(bool onlyTrueWhenCounted){m_onlyTrueWhenCounted=onlyTrueWhenCounted;};
            bool intersection(cv::Point2f o1, cv::Point2f p1, cv::Point2f o2, cv::Point2f p2, cv::Point2f &r);
            bool isValid(const Image & evaluation, const ImageVector & images, JSON & data);
    };
}
#endif