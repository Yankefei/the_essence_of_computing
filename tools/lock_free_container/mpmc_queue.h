#ifndef _TOOLS_LOCK_FREE_CONTAINER_MPMC_QUEUE_H_
#define _TOOLS_LOCK_FREE_CONTAINER_MPMC_QUEUE_H_

#include <memory>
#include <cassert>
#include <cstring>

#include "general.h"

#include "queue_base.h"

namespace tools
{

// mpmc_queue
template<typename T, typename Alloc = std::allocator<T>>
class MPMCQueue : protected QueueBase<T, Alloc>
{
    typedef QueueBase<T, Alloc> Queue_Base;

    using Queue_Base::buy_array;
    using Queue_Base::free_array;
public:

private:

};

}

#endif