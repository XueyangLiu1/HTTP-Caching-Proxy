#ifndef _THREAD_INFO_H__
#define _THREAD_INFO_H__
#include "cache.hpp"
#include <pthread.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

class Thread_info {
public:
    int thread_id;
    int browser_fd;
    std::string ip_addr;
    Cache *cache;
};

#endif
