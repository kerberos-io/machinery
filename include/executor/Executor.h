//
//  Class: Executor
//  Description: Executor is a class that executes a function at specific times/intervals
//               by using human language and a function pointer.
//
//  Usage:
//               Executor execute;
//               execute.setAction(this, &ClassName::memberFunction);
//               execute.setInterval("twice in 4 times");
//
//               -- call the funcor methode
//               execute(); // do nothing
//               execute(); // execute function..
//               execute(); // do nothing
//               execute(); // execute function ..
//
//               -- but you can also specify time intervals
//               execute.setInterval("four times a day");
//               execute.setInterval("once a day");
//               execute.setInterval("twice in 10 minutes");
//               execute.setInterval("thrice in 120 functions calls");
//
//
//  Created:     28/11/2014
//  Author:      Cédric Verstraeten
//  Mail:        cedric@verstraeten.io
//  Website:     www.verstraeten.io
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied.
//
/////////////////////////////////////////////////////

#ifndef __Executor_H_INCLUDED__   // if Executor.h hasn't been included yet...
#define __Executor_H_INCLUDED__   // #define this so the compiler knows it has been included

#include <iostream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <sstream>
#include <map>
#include <vector>

namespace kerberos
{
    enum IntervalType {
        SEQUENCE_INTERVAL,
        TIME_INTERVAL
    };
    
    /**********************
    *   ExecutorType
    *
    *           The executor type contains a function that will
    *           determine if a function can be executed or not.
    *           We can add different ExecutorTypes you just have to inherit
    *           from the ExecutorType class.
    */

    class ExecutorType
    {
        protected:
            std::vector<std::pair<int, IntervalType> > integers;
        public:
            typedef std::vector<std::pair<int, IntervalType> > IntegerTypeArray;
            virtual bool operator()() = 0;
    };
    
            class SequenceInterval : public ExecutorType
            {
                private:
                    int m_count;
                    int m_times;
                    int m_boundery;
                    int m_increase;
            
                public:
                    SequenceInterval(IntegerTypeArray & integers);
                    bool operator()();
            };
    
            class TimeInterval : public ExecutorType
            {
                public:
                    TimeInterval(IntegerTypeArray & integers);
                    bool operator()();
            };

    /**********************
    *   ExecutorHelper
    *
    *       This is a helper class that contains the functions
    *       that don't require templating. We need a parent class
    *       because we will have different behaviour when using non-static
    *       and static member functions.
    */
    class ExecutorHelper
    {
        public:
            ExecutorType * m_execute;
            std::map<std::string, std::pair<int, IntervalType> > m_humanTime;

            void initialize()
            {
                // mapping from human language to integers
                m_humanTime["once"] = std::make_pair(1, SEQUENCE_INTERVAL);
                m_humanTime["twice"] = std::make_pair(2, SEQUENCE_INTERVAL);
                m_humanTime["thrice"] = std::make_pair(3, SEQUENCE_INTERVAL);

                m_humanTime["one"] = std::make_pair(1, SEQUENCE_INTERVAL);
                m_humanTime["two"] = std::make_pair(2, SEQUENCE_INTERVAL);
                m_humanTime["three"] = std::make_pair(3, SEQUENCE_INTERVAL);
                m_humanTime["four"] = std::make_pair(4, SEQUENCE_INTERVAL);
                m_humanTime["five"] = std::make_pair(5, SEQUENCE_INTERVAL);
                m_humanTime["six"] = std::make_pair(6, SEQUENCE_INTERVAL);
                m_humanTime["seven"] = std::make_pair(7, SEQUENCE_INTERVAL);
                m_humanTime["eight"] = std::make_pair(8, SEQUENCE_INTERVAL);
                m_humanTime["nine"] = std::make_pair(9, SEQUENCE_INTERVAL);
                m_humanTime["ten"] = std::make_pair(10, SEQUENCE_INTERVAL);

                m_humanTime["day"] = std::make_pair(60 * 60 * 24, TIME_INTERVAL); // seconds in a day
                m_humanTime["week"] = std::make_pair(60 * 60 * 24 * 7, TIME_INTERVAL); // seconds in a week
                m_humanTime["month"] = std::make_pair(60 * 60 * 24 * 7 * 4, TIME_INTERVAL); // seconds in a month
                m_humanTime["year"] = std::make_pair(60 * 60 * 24 * 7 * 4 * 21, TIME_INTERVAL); // seconds in a year
            }

            void setInterval(std::string & sentence)
            {
                parse(sentence);
            }
            void setInterval(char * sentence)
            {
                parse(sentence);
            }

            void parse(char * s)
            {
                std::string sentence(s);
                parse(sentence);
            }

            void parse(std::string & sentence)
            {
                // Call initialize method
                initialize();

                std::istringstream stream(sentence);
                std::string sub;

                IntervalType type = SEQUENCE_INTERVAL;
                typedef std::map<std::string, std::pair<int, IntervalType> > iterator;
                std::vector<std::pair<int, IntervalType> > integers;

                while (stream >> sub)
                {
                    iterator::const_iterator it = m_humanTime.find(sub);
                    if (it != m_humanTime.end())
                    {
                        std::pair<int, IntervalType> temp = it->second;
                        integers.push_back(temp);

                        if (temp.second > type)
                        {
                            type = temp.second;
                        }
                    }
                    else
                    {
                        bool isNumeric = true;
                        for (int i = 0; i < sub.length(); i++)
                        {
                            if (!std::isdigit(sub[i]))
                            {
                                isNumeric = false;
                                break;
                            }
                        }

                        if (isNumeric)
                        {
                            int number = std::atoi(sub.c_str());
                            integers.push_back(std::make_pair(number, SEQUENCE_INTERVAL));
                        }
                    }
                }

                // Static Factory, is enough for this kind of class
                if (type == SEQUENCE_INTERVAL)
                {
                    m_execute = new SequenceInterval(integers);
                }
                else if (type == TIME_INTERVAL)
                {
                    m_execute = new TimeInterval(integers);
                }
            }
    };
    
    /**********************
    *   Executor<T>
    *
    *       class that will be used for non-static member functions.
    */

    template<class T = void>
    class Executor : public ExecutorHelper
    {
        private:
            T * m_obj;
            void (T::*m_obj_func)();

        public:
            Executor();
            ~Executor();

            Executor(std::string & sentence, T * obj, void (T::*func)());
            Executor(char * sentence, T * obj, void (T::*func)());

            void setAction(T * obj, void (T::*func)());
            bool operator()();
    };

            template<class T>
            Executor<T>::Executor(){}

            template<class T>
            Executor<T>::~Executor()
            {
                if (m_execute != 0)
                {
                    delete m_execute;
                }
            }

            template<class T>
            Executor<T>::Executor(std::string & sentence, T * obj, void (T::*func)())
            {
                parse(sentence);
                m_obj = obj;
                m_obj_func = func;
            }

            template<class T>
            Executor<T>::Executor(char * sentence, T * obj, void (T::*func)())
            {
                parse(sentence);
                m_obj = obj;
                m_obj_func = func;
            }

            template<class T>
            void Executor<T>::setAction(T * obj, void (T::*func)())
            {
                m_obj = obj;
                m_obj_func = func;
            }

            template<class T>
            bool Executor<T>::operator()()
            {
                if ((*m_execute)())
                {
                    ((m_obj)->*(m_obj_func))();
                    return true;
                }
                return false;
            }

    /**********************
    *   Executor<void>
    *
    *       class that will be used for function pointers and 
    *       static member function pointers.
    */
    
    template<>
    class Executor<void> : public ExecutorHelper
    {
        private:
            void (*m_func)();

        public:
            Executor(){}
            ~Executor()
            {
                if (m_execute != 0)
                {
                    delete m_execute;
                }
            }

            Executor(std::string & sentence, void (*func)())
            {
                parse(sentence);
                m_func = func;
            }
            Executor(char * sentence, void (*func)())
            {
                parse(sentence);
                m_func = func;
            }

            void setAction(void (*func)())
            {
                m_func = func;
            }
            bool operator()()
            {
                if ((*m_execute)())
                {
                    (*m_func)();
                    return true;
                }
                return false;
            }
    };
}

#endif