//
// Created by Edward on 21/10/2026.
//

#include "Watermark.h"
#include <imgui.h>
#include <format>

#include "SDK/game/ClientInstance.h"
#include "Utils/RenderUtils.h"

void Watermark::onRender(Events::RenderEvent&) {
    const ImGuiIO& io = ImGui::GetIO();
    constexpr std::string text = "Cloud Client";
    const float h = io.DisplaySize.y;
    const float margin = Utils::Render::scaleToScreenHeight(h * 0.01f, h);
    const float titleH = ImGui::GetFont()->CalcTextSizeA(50.0f, FLT_MAX, 0.0f, text.data(), text.data() + text.size()).y;
    const ImVec2 pos(margin, margin); // ik this might seem weird but its just so margin is same on both sides
    const auto time = static_cast<float>(ImGui::GetTime());

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    Utils::Render::drawAnimatedGradientText(dl, 50.0f, pos, text, ImColor(255, 255, 255), ImColor(20, 200, 255), time * 0.7f);

    auto clientinstance = SDK::ClientInstance::get();

    if (!clientinstance) {
        return;
    }
    auto screenName = clientinstance->getScreenName();
    if (screenName.empty()) {
        return;
    }
    Utils::Render::drawAnimatedGradientText(dl, 30.0f, ImVec2(pos.x, pos.y + titleH + 5.f), screenName, ImColor(255, 255, 255), ImColor(20, 200, 255), time * 0.7f);
}
