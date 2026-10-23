//
// Created by Edward on 19/10/2026.
//

#ifndef CLOUD_CLIENT_RENDERUTILS_H
#define CLOUD_CLIENT_RENDERUTILS_H
#include <cmath>
#include <imgui.h>
#include <string>

namespace Utils {
    namespace Render {
        inline float getLerpValue(const float a, const float b, const float t) {
            return a + (b - a) * t;
        }
        inline ImU32 rgbToCol(const int r, const int g, const int b, const float a = 1.f) {
            return IM_COL32(r, g, b, static_cast<int>(a * 255.f));
        }

        // converts a value designed at my dev screen height (1080) to the end users screen height
        inline float scaleToScreenHeight(float designPixels, float screenHeight) {
            return designPixels * (screenHeight / 1080.f);
        }
        inline float scaleToScreenWidth(float designPixels, float screenWdith) {
            return designPixels * (screenWdith / 1920.f);
        }
        inline void drawAnimatedGradientText(ImDrawList* dl, const float fontSize, const ImVec2 pos, const std::string& text, const ImColor from, const ImColor to, const float shift) {
            ImGui::PushFont(nullptr, fontSize);
            const float totalWidth = ImGui::CalcTextSize(text.c_str()).x;
            const int startVecs = dl->VtxBuffer.Size;
            dl->AddText(pos, IM_COL32(255,255,255, 255), text.c_str());
            const int endVecs = dl->VtxBuffer.Size;
            for (int i = startVecs; i < endVecs; i++) {
                const float x = dl->VtxBuffer[i].pos.x - pos.x; // get relative pos from start of text
                const float hueInitialOffset = std::fmodf((x / totalWidth) + shift, 1.0f);
                // if < 0.5 double to make it 0 to 1. if greater do the same but take from 2 to swap the order so 1 to 0.
                const float wave = hueInitialOffset < 0.5f ? hueInitialOffset * 2.f : 2.f - hueInitialOffset * 2.f;
                const ImVec4 c(
                    getLerpValue(from.Value.x, to.Value.x, wave),
                    getLerpValue(from.Value.y, to.Value.y, wave),
                    getLerpValue(from.Value.z, to.Value.z, wave),
                    1.f // we dont need to animate alpha idk if that would look cool
                );
                dl->VtxBuffer[i].col = ImGui::ColorConvertFloat4ToU32(c);
            }
            ImGui::PopFont();
        }
    }
}

#endif //CLOUD_CLIENT_RENDERUTILS_H
