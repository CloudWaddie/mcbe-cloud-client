//
// Created by Edward on 19/10/2026.
//

#include "Notifications.h"

#include <chrono>
#include <string>
#include "Utils/RenderUtils.h"
#include <vector>
#include <imgui.h>
#include <cmath>

struct Notification {
    float currentXPos = 0;
    float currentYPos = 0;
    std::string msg;
    bool finished = false;
    bool arrived = false;
    float width = 0;
    bool firstTime = true;
    float textWidth = 0.f;
    float textHeight = 0.f;
    std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
};

static std::vector<Notification> g_notifications;

namespace Rendering {
    namespace Notifications {
        void notify(const std::string& message) {
            Notification n;
            n.msg = message;
            n.currentYPos = ImGui::GetIO().DisplaySize.y; // set to the bottom of the screen
            g_notifications.push_back(n);
        }

        void clear() {
            g_notifications.clear();
        }

        void render() {
            auto& io = ImGui::GetIO();
            float screenHeight = io.DisplaySize.y;
            float screenWidth = io.DisplaySize.x;

            float delta = io.DeltaTime > 0 ? io.DeltaTime : 0.016f; // 1/60 = 0.016 recurring
            float frameFactor = delta * 60.0f; // so its the same speed on any fps screen
            if (frameFactor > 1.f) frameFactor = 1.f; // the lower the fps the higher the delta time the higher the frame factor.
            frameFactor = std::floor(frameFactor * 100.f) / 100.f; // makes it 2 decimal places

            float roundRadiusX = Utils::Render::scaleToScreenHeight(20, screenHeight);
            float height = screenHeight * 0.035f;
            float fontSize = screenHeight * 0.128f; // TODO: Use a calc font size render util
            float imguiFontSize = fontSize * 0.18f; // TODO: Above
            ImU32 accentCol = Utils::Render::rgbToCol(20, 200, 255);
            ImDrawList* drawList = ImGui::GetForegroundDrawList(); // lots of ppl use background draw list but i want arraylist, watermark and notifs to be ontop

            int i = 0;
            for (auto& notification : g_notifications) {
                if (notification.finished) continue;

                float posyModif = -((height + screenHeight * 0.01f) * i); // for stacking notifs. its the height plus a small gap

                if (notification.firstTime) {
                    ImVec2 ts = ImGui::CalcTextSize(notification.msg.c_str());
                    notification.textWidth = ts.x;
                    notification.textHeight = ts.y;
                    float scale = imguiFontSize / ImGui::GetFontSize();
                    float textW = ts.x * scale;
                    notification.width = textW + screenHeight * 0.0345f;
                    notification.currentXPos = (screenWidth - notification.width) / 2.f; // center
                    notification.firstTime = false;
                }

                auto drawNotif = [&]() {
                    drawList->AddRectFilled(
                        ImVec2(notification.currentXPos, notification.currentYPos + posyModif),
                        ImVec2(notification.currentXPos + notification.width, notification.currentYPos + posyModif + height),
                        accentCol, roundRadiusX);

                    float sc = imguiFontSize / ImGui::GetFontSize();
                    float tx = notification.currentXPos + (notification.width - notification.textWidth * sc) / 2.f;
                    float ty = notification.currentYPos + posyModif + (height - notification.textHeight * sc) / 2.f;
                    drawList->AddText(ImGui::GetFont(), imguiFontSize, ImVec2(tx, ty),
                        Utils::Render::rgbToCol(255, 255, 255), notification.msg.c_str());
                };

                if (!notification.arrived) {
                    // Sliding in
                    drawNotif();
                    notification.currentYPos = Utils::Render::getLerpValue(notification.currentYPos, screenHeight * 0.9f, frameFactor * 0.067f);
                    if (notification.currentYPos <= screenHeight * 0.92f) {
                        notification.arrived = true;
                        notification.time = std::chrono::steady_clock::now();
                    }
                } else {
                    // Holding, then sliding out
                    drawNotif();
                    auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - notification.time);
                    if (timeDiff.count() > 5000) {
                        notification.currentYPos = Utils::Render::getLerpValue(notification.currentYPos, screenHeight + 500, frameFactor * 0.052f);
                        if (notification.currentYPos >= screenHeight) {
                            notification.finished = true;
                        }
                    }
                }

                i++;
            }

            std::erase_if(g_notifications, [](const Notification& n) { return n.finished; });
        }
    }
}