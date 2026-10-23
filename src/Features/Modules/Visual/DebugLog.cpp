//
// Created by Edward on 23/10/2026.
//

#include "DebugLog.h"

#include <imgui.h>

#include "Rendering/Notifications.h"

void DebugLog::onRender(Events::RenderEvent&) {
    ImGui::ShowDebugLogWindow();
}
