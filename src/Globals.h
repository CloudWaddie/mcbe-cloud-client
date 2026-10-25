//
// Created by Edward on 22/10/2026.
//

#ifndef CLOUD_CLIENT_GLOBALS_H
#define CLOUD_CLIENT_GLOBALS_H
#include <atomic>

namespace Client {
    inline std::atomic<bool> shouldUninject{false};
    inline std::atomic<bool> clickGuiOpen{false};
}

#endif //CLOUD_CLIENT_GLOBALS_H
