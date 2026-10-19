//
// Created by Edward on 19/10/2026.
//

#ifndef DLL_TEST_NOTIFICATIONS_H
#define DLL_TEST_NOTIFICATIONS_H

#include <string>

namespace Rendering {
    namespace Notifications {
        void notify(const std::string& message);
        void render();
        void clear();
    }
}

#endif //DLL_TEST_NOTIFICATIONS_H
