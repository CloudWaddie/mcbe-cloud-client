//
// Created by Edward on 22/10/2026.
//
#include "Features/Modules/Module.h"
#include "ClickGUI.h"
#include <imgui.h>

#include "Features/Modules/ModuleManager.h"
#include "Utils/RenderUtils.h"
#include <string>

namespace {
    struct Layout {
        float panelWidth;
        float panelMargin;
        float panelGap;
        float headerHeight;
        float rowHeight;
        float textPadding;
    };
}

static void DrawCategory(Category category, const std::vector<Module*>& modules, ImVec2 pos, const Layout& layout) {
    ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver);
    const std::string categoryName = getCategoryName(category);
    std::string windowName = categoryName + "##clickgui";
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    auto drawlist = ImGui::GetWindowDrawList();
    float totalHeight = layout.headerHeight + static_cast<int>(modules.size()) * layout.rowHeight;
    if (categoryName == "Visual") {
        totalHeight -= layout.rowHeight;
    }
    ImVec2 min = ImGui::GetCursorScreenPos();
    ImVec2 max = ImVec2(min.x + layout.panelWidth, min.y + totalHeight);
    drawlist->AddRectFilled(min, max, IM_COL32(10, 10, 12, 235), 0);
    ImGui::Dummy(ImVec2(layout.panelWidth, layout.headerHeight));
    drawlist->AddRectFilled(min, ImVec2(max.x, min.y + layout.headerHeight), ImColor(20, 200, 255), 0);
    drawlist->AddText(ImVec2(min.x + layout.textPadding, min.y + (layout.headerHeight - ImGui::CalcTextSize(categoryName.c_str()).y) / 2), IM_COL32(255,255,255,255), categoryName.c_str());
    for (Module* currentModule : modules) {
        std::string moduleName = currentModule->getName();
        if (moduleName == "ClickGUI") {
            continue;
        }

        ImVec2 rowMin = ImGui::GetCursorScreenPos();
        if (ImGui::InvisibleButton(moduleName.c_str(), ImVec2(layout.panelWidth, layout.rowHeight))) {
            ModuleManager::modulesWantToggle.push_back(currentModule);
        }
        ImVec2 rowMax = ImGui::GetItemRectMax();
        const bool isHovered = ImGui::IsItemHovered();
        const bool isEnabled = currentModule->isEnabled();

        if (isEnabled) {
            drawlist->AddRectFilled(rowMin, rowMax, IM_COL32(20, 200, 255, 40));
        }
        if (isHovered) {
            drawlist->AddRectFilled(rowMin, rowMax, IM_COL32(255, 255, 255, 20));
        }
        const ImU32 textColor = isEnabled ? IM_COL32(20, 200, 255, 255) : isHovered ? IM_COL32(255, 255, 255, 255) : IM_COL32(170, 170, 175, 255);
        const float textY = rowMin.y + (layout.rowHeight - ImGui::CalcTextSize(moduleName.c_str()).y) / 2;
        drawlist->AddText(ImVec2(rowMin.x + layout.textPadding, textY), textColor, moduleName.c_str());
    }
    ImGui::PopStyleVar(); // pops item spacing
    ImGui::End();
    ImGui::PopStyleVar(); // pops window padding
}

void ClickGUI::onRender(Events::RenderEvent&) {
    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    if (displaySize.x <= 0.f || displaySize.y <= 0.f) {
        return;
    }

    const Layout layout{
        Utils::Render::scaleToScreenWidth(220.f, displaySize.x),
        Utils::Render::scaleToScreenWidth(40.f, displaySize.x),
        Utils::Render::scaleToScreenWidth(16.f, displaySize.x),
        Utils::Render::scaleToScreenHeight(38.f, displaySize.y),
        Utils::Render::scaleToScreenHeight(30.f, displaySize.y),
        Utils::Render::scaleToScreenWidth(12.f, displaySize.x)
    };

    for (int i = 0; i < static_cast<int>(Category::Count); i++) {
        const auto category = static_cast<Category>(i);
        auto modules = ModuleManager::getModulesByCategory(category);
        DrawCategory(category, modules, ImVec2(layout.panelMargin + i * (layout.panelGap + layout.panelWidth), 100.f), layout);
    }
}