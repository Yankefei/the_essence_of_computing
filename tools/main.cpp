#include "stream.h"
#include "stack.h"

#include "ring_queue.h"

using namespace tools;

int main()
{
    {
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

        for (int i = 100; i < 120; i ++)
        {
            if (!r_queue.push_back(i))
                stream <<" push_back failed: "<< i << std::endl;
        }

        for (int i = 0; i < 30; i++)
            r_queue.pop_front();
        
        stream <<"num: " <<r_queue.size() <<", front: "<< r_queue.front() <<", back: "<< r_queue.back() <<std::endl;
    }

#if 0
    {
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
#endif
    return 0;
}