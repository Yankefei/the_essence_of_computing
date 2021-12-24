#include "stream.h"

#include "ring_queue.h"
#include "queue.h"


using namespace tools;

int main()
{
    {
        NQueue<int>  r_queue;

        for (int i = 0; i < 110; i ++)
        {
            r_queue.push_back(i);
        }

        stream <<"num: " <<r_queue.size() <<std::endl;

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;

        NQueue<int> r_queue_t = r_queue;
        stream << "compare1..." << std::endl;
        for (int i = 100; i < 120; i ++)
        {
            r_queue.push_back(i);
        }

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;


        stream << "compare2..." << std::endl;
        
        r_queue_t = r_queue;
        stream <<"num: " <<r_queue_t.size() <<", front: "<< r_queue_t.front() <<", back: "<< r_queue_t.back() <<std::endl;
    }

    {
        Queue<int>  r_queue;

        for (int i = 0; i < 110; i ++)
        {
            r_queue.push_back(i);
        }

        stream <<"num: " <<r_queue.size() <<std::endl;

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;

        Queue<int> r_queue_t = r_queue;
        stream << "compare1..." << std::endl;
        for (int i = 100; i < 120; i ++)
        {
            r_queue.push_back(i);
        }

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;


        stream << "compare2..." << std::endl;
        
        r_queue_t = r_queue;
        stream <<"num: " <<r_queue_t.size() <<", front: "<< r_queue_t.front() <<", back: "<< r_queue_t.back() <<std::endl;
    }

    {
        stream << "RQueue" << std::endl;
        RQueue<int>  r_queue(100);

        for (int i = 0; i < 110; i ++)
        {
            if (!r_queue.push_back(i))
                stream <<" push_back failed: "<< i << std::endl;
        }

        stream <<"num: " <<r_queue.size() <<std::endl;

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;

        RQueue<int> r_queue_t = r_queue;
        stream << "compare1..." << std::endl;
        for (int i = 100; i < 120; i ++)
        {
            if (!r_queue.push_back(i))
                stream <<" push_back failed: "<< i << std::endl;
        }

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;


        stream << "compare2..." << std::endl;
        
        r_queue_t = r_queue;
        stream <<"num: " <<r_queue_t.size() <<", front: "<< r_queue_t.front() <<", back: "<< r_queue_t.back() <<std::endl;
    }

    {
        stream << "RQueue2" << std::endl;
        RQueue2<int>  r_queue(100);

        for (int i = 0; i < 110; i ++)
        {
            if (!r_queue.push_back(i))
                stream <<" push_back failed: "<< i << std::endl;
        }

        stream <<"num: " <<r_queue.size() <<std::endl;

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;

        RQueue2<int> r_queue_t = r_queue;
        stream << "compare1..." << std::endl;
        for (int i = 100; i < 120; i ++)
        {
            if (!r_queue.push_back(i))
                stream <<" push_back failed: "<< i << std::endl;
        }

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;


        stream << "compare2..." << std::endl;
        
        r_queue_t = r_queue;
        stream <<"num: " <<r_queue_t.size() <<", front: "<< r_queue_t.front() <<", back: "<< r_queue_t.back() <<std::endl;
    }




    return 0;
}