#include "stream.h"
#include "stack.h"

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

    {
        stream << "Stack" << std::endl;
        Stack<int>  stack_int;
        for (int i = 0; i < 1000; i ++)
            stack_int.push(i);

        stream << stack_int.size() << " " << stack_int.capacity()<<  " " << stack_int.top() << std::endl;

        for (int i = 0; i < 500; i ++)
            stack_int.pop();

        stream << stack_int.size() << " " << stack_int.capacity() << std::endl;

        for (int i = 0; i < 500; i ++)
            stack_int.push(i);

        stream << stack_int.size() << " " << stack_int.capacity()<<  " " << stack_int.top() << std::endl;

        Stack<int> stack_int_temp = stack_int;
        stream << stack_int_temp.size() << " " << stack_int_temp.capacity()<<  " " << stack_int_temp.top() << std::endl;
    }

    return 0;
}