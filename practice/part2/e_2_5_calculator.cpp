#include <iostream>
#include "stack.h"
#include <string.h>


// todo 数据溢出
int my_atoi(const char* buf, int length = 0)
{
    int ret_val = 0;
    if (length == 0)
        length = strlen(buf);

    for (int i = 0; i < length; i++)
    {
        if (buf[i] > '9' || buf[i] < '0')
        {
            return 0;
        }

        ret_val*= 10;
        ret_val += (buf[i] - '0');
    }
    return ret_val;
}

enum class ValType
{
    OPERATOR = 0,
    INT = 1
};

struct StackInfo
{
    StackInfo(int val) : val_(val), type_(ValType::INT) {}
    StackInfo(char op) : operator_(op), type_(ValType::OPERATOR) {}
    union
    {
        int val_;
        char operator_;
    };
    ValType type_;
};

void calculator_three_size_operator(tools::Stack<StackInfo>& stack)
{
    int temp_val = stack.top().val_;
    stack.pop();
    auto info = stack.top();
    switch(info.operator_)
    {
        case '+':
        {
            stack.pop();
            stack.top().val_ += temp_val;
            break;
        }
        case '-':
        {
            stack.pop();
            stack.top().val_ -= temp_val;
            break;
        }
        case '*':
        {
            stack.pop();
            stack.top().val_ *= temp_val;
            break;
        }
        case '/':
        {
            stack.pop();
            assert(temp_val != 0);
            stack.top().val_ /= temp_val;
            break;
        }
        default:
            assert(false);
    }
}

// 栈过程：
// 1. 5+4*2+3=;  
// 2. 5*3*2+1=;  
// 3. 5+3+2=;
// 4. 5-3+2=;
// 栈结构：
uint32_t calculator(const char* buf, uint32_t size)
{
    tools::Stack<StackInfo> stack;
    int num_begin_index = -1;  // 数字字符起始位置
    int val = 0;
    for (uint32_t i = 0; i < size; i ++)
    {
        if (buf[i] >= '0' && buf[i] <= '9')
        {
            if (num_begin_index == -1)
                num_begin_index = i;
        }
        else
        {
            if (num_begin_index != -1)
            {
                val = my_atoi(buf + num_begin_index, i - num_begin_index);
                num_begin_index = -1;

                if (!stack.empty())
                {
                    auto info = stack.top();
                    if (info.operator_ == '*')
                    {
                        stack.pop();
                        stack.top().val_ *= val;
                    }
                    else if (info.operator_ == '/')
                    {
                        stack.pop();
                        assert(val != 0);
                        stack.top().val_ /= val;
                    }
                    else
                        stack.push(val);
                }
                else
                    stack.push(val);
            }
            switch(buf[i])
            {
                case ' ':
                    break;
                case '=':
                {
                    if (stack.size() > 1)
                    {
                        assert(stack.size() == 3);
                        calculator_three_size_operator(stack);
                        assert(stack.size() == 1);
                    }
                    break;
                }
                case '+':
                case '-':
                {
                    // 直接计算栈中的值
                    if (stack.size() > 1)
                    {
                        assert(stack.size() == 3);
                        calculator_three_size_operator(stack);
                        assert(stack.size() == 1);
                    }
                    assert(stack.size() == 1);
                    stack.push(buf[i]);
                    break;
                }
                case '*':
                case '/':
                {
                    // 入栈一个操作符
                    stack.push(buf[i]);
                    break;
                }
                default:
                {
                    assert(false);
                    return 0; // error
                }
            }
        }
    }
    assert(stack.size() == 1);
    return stack.top().val_;
}


void calculator_add_sub_operator(tools::Stack<StackInfo>& stack)
{
    int temp_val = stack.top().val_;
    stack.pop();
    auto info = stack.top();
    switch(info.operator_)
    {
        case '+':
        {
            stack.pop();
            stack.top().val_ += temp_val;
            break;
        }
        case '-':
        {
            stack.pop();
            stack.top().val_ -= temp_val;
            break;
        }
        default:
            assert(false);
    }
}

void calculator_mul_div_operator(tools::Stack<StackInfo>& stack)
{
    int temp_val = stack.top().val_;
    stack.pop();
    auto info = stack.top();
    switch(info.operator_)
    {
        case '*':
        {
            stack.pop();
            stack.top().val_ *= temp_val;
            break;
        }
        case '/':
        {
            stack.pop();
            assert(temp_val != 0);
            stack.top().val_ /= temp_val;
            break;
        }
        default:
            assert(false);
    }
}

/*
    使用另外一种计算方式：将最后的运算符保存在临时量中，只有新增加的运算符和最后的运算符比较之后，
    才能开始开始做运算。这样让整个逻辑清晰了很多
*/
uint32_t calculator2(const char* buf, uint32_t size)
{
    // 使用一个变量保存栈顶运算符
    tools::Stack<StackInfo> stack;
    int num_begin_index = -1;  // 数字字符起始位置
    int val = 0;
    char last_operator = 0;
    for (uint32_t i = 0; i < size; i ++)
    {
        if (buf[i] >='0' && buf[i] <= '9')
        {
            if (num_begin_index == -1)
                num_begin_index = i;
        }
        else
        {
            if (num_begin_index != -1)
            {
                val = my_atoi(buf + num_begin_index, i - num_begin_index);
                num_begin_index = -1;

                stack.push(val);
            }
            switch(buf[i])
            {
                case ' ':
                    break;
                case '=':
                {
                    assert(stack.size() == 3);
                    calculator_three_size_operator(stack);
                    assert(stack.size() == 1);
                    break;
                }
                case '+':
                case '-':
                {
                    if (last_operator == 0)
                    {
                        last_operator = buf[i];
                        stack.push(buf[i]);
                    }
                    else
                    {
                        // 循环特别重要，因为遇到+-运算符的话，就必须将栈内部待运算的全部计算完毕
                        while(stack.size() > 1)
                        {
                            calculator_three_size_operator(stack);
                        }
                        stack.push(buf[i]);
                        last_operator = buf[i];
                    }

                    break;
                }
                case '*':
                case '/':
                {
                    if (last_operator == 0)
                    {
                        last_operator = buf[i];
                        stack.push(buf[i]);
                    }
                    else
                    {
                        if (last_operator == '*' || last_operator == '/')
                        {
                            calculator_three_size_operator(stack);
                        }
                        stack.push(buf[i]);
                        last_operator = buf[i];
                    }
                    
                    break;
                }
                default:
                    assert(false);
            }
        }
    }
    assert(stack.size() == 1);
    return stack.top().val_;
}