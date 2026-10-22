//
// Created by Edward on 19/10/2026.
//

#ifndef CLOUD_CLIENT_NOTIFICATIONS_H
#define CLOUD_CLIENT_NOTIFICATIONS_H

#include <string>

namespace Rendering {
    namespace Notifications {
        void notify(const std::string& message);
        void render();
        void clear();
    }
}

#endif //CLOUD_CLIENT_NOTIFICATIONS_H
