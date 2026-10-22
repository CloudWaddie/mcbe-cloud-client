//
// Created by Edward on 18/10/2026.
//

#ifndef CLOUD_CLIENT_LOG_H
#define CLOUD_CLIENT_LOG_H

#include <cstdio>

// if we are debugging then we log otherwise we dont

#ifndef NDEBUG
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...) ((void)0)
#endif

#endif //CLOUD_CLIENT_LOG_H
