//
// Created by Edward on 20/10/2026.
//

#ifndef CLOUD_CLIENT_EVENTS_H
#define CLOUD_CLIENT_EVENTS_H

#include <nes/event_dispatcher.hpp>
#include <memory>

namespace Events {
    inline std::unique_ptr<nes::event_dispatcher> dispatcher;

    struct RenderEvent {};
}

#endif //CLOUD_CLIENT_EVENTS_H
