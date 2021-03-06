#include "stream.h"
#include "stack.h"

#include <stack>

using namespace tools;

int main()
{
    {
        stream << "sizeof stack : " << sizeof(std::stack<char>) << std::endl;
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
    
        stack_int_temp.clear();
        stream << stack_int_temp.size() << " " << stack_int_temp.capacity() << std::endl;
    }

    return 0;
}