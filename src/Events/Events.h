//
// Created by Edward on 20/10/2026.
//

#ifndef DLL_TEST_EVENTS_H
#define DLL_TEST_EVENTS_H

#include <nes/event_dispatcher.hpp>

namespace Events {
    inline std::unique_ptr<nes::event_dispatcher> dispatcher;

    struct RenderEvent {};
}

#endif //DLL_TEST_EVENTS_H
