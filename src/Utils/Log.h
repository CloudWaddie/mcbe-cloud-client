//
// Created by Edward on 18/10/2026.
//

#ifndef DLL_TEST_LOG_H
#define DLL_TEST_LOG_H

#include <cstdio>

// if we are debugging then we log otherwise we dont

#ifndef NDEBUG
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...) ((void)0)
#endif

#endif //DLL_TEST_LOG_H
