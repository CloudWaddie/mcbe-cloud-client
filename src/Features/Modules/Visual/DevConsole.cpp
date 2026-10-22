#include "DevConsole.h"
#include <imgui.h>

#include "Globals.h"
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
    if (ImGui::Button("Uninject", ImVec2(-1, 0))) {
        Client::shouldUninject = true;
    }
    ImGui::End();
}
