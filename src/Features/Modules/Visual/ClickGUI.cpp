//
// Created by Edward on 22/10/2026.
//
#include "Features/Modules/Module.h"
#include "ClickGUI.h"
#include <imgui.h>

#include "Features/Modules/ModuleManager.h"
#include "Utils/RenderUtils.h"
#include <string>

constexpr float panelWidth = 220.f;
constexpr float panelMargin = 40.f;
constexpr float panelGap = 16.f;
constexpr float headerHeight = 38;
constexpr float rowHeight = 30;

static void DrawCategory(Category category, const std::vector<Module*>& modules, ImVec2 pos) {
    ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
    std::string windowName = getCategoryName(category);
    windowName += "##clickgui";
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    auto drawlist = ImGui::GetWindowDrawList();
    float totalHeight = headerHeight + static_cast<int>(modules.size()) * rowHeight;
    ImVec2 min = ImGui::GetCursorScreenPos();
    ImVec2 max = ImVec2(min.x + panelWidth, min.y + totalHeight);
    drawlist->AddRectFilled(min, max, IM_COL32(10, 10, 12, 235), 10);
    ImGui::Dummy(ImVec2(panelWidth, headerHeight));
    drawlist->AddRectFilled(min, ImVec2(max.x, min.y + headerHeight), IM_COL32(255, 255, 255, 235), 10, ImDrawFlags_RoundCornersTop);
    ImGui::PopStyleVar(); // pops item spacing
    ImGui::End();
    ImGui::PopStyleVar(); // pops window padding
}

void ClickGUI::onRender(Events::RenderEvent&) {
    for (int i = 0; i < static_cast<int>(Category::Count); i++) {
        const auto category = static_cast<Category>(i);
        auto modules = ModuleManager::getModulesByCategory(category);
        DrawCategory(category, modules, ImVec2(panelMargin + i * (panelGap + panelWidth), 100.f));
    }
}