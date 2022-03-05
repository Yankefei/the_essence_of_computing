#include "stream.h"

#include "singleton.h"
#include <cassert>

using namespace tools;

struct CTest : public Singleton<CTest>
{
    CTest(int _a, double _b) : a(_a), b(_b) {}
    int a;
    double b;
};



int main()
{
    CTest* p1 = CTest::GetInstance(1, 2.1);
    CTest* p2 = CTest::GetInstance(1, 2.2);

    CTest* p11 = CTest::GetInstance(1, 2.1);

    assert(p1 != p2);

    assert(p1 == p11);

    return 0;
}