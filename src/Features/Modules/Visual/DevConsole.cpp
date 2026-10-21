#include "DevConsole.h"
#include <imgui.h>

#include "Rendering/Notifications.h"

void DevConsole::onRender(Events::RenderEvent&) {
    if (!ImGui::Begin("Dev Console")) { ImGui::End(); return; }

    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %.0f (%.2f ms)", io.Framerate, 1000.0f / io.Framerate);
    ImGui::Text("Trigger notification:");
    bool enter = ImGui::InputText("##notification", notificationText, IM_ARRAYSIZE(notificationText), ImGuiInputTextFlags_EnterReturnsTrue);
    if (ImGui::Button("Trigger", ImVec2(-1, 0)) || enter) {
        Rendering::Notifications::notify(std::string(notificationText));
    }
    static float frameTimes[120] = {};
    static int offset = 0;
    frameTimes[offset] = 1000.0f / (io.Framerate > 0.f ? io.Framerate : 1.f);
    offset = (offset + 1) % IM_ARRAYSIZE(frameTimes);
    ImGui::PlotLines("##ft", frameTimes, IM_ARRAYSIZE(frameTimes), offset,
                     "frame ms", 0.f, 33.f, ImVec2(-1, 60));
    ImGui::End();
}
