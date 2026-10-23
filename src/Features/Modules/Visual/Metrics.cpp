//
// Created by Edward on 23/10/2026.
//

#include "Metrics.h"
#include <imgui.h>

void Metrics::onRender(Events::RenderEvent&) {
    ImGui::ShowMetricsWindow();
}

