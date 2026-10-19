//
// Created by Edward on 19/10/2026.
//

#ifndef DLL_TEST_RENDERUTILS_H
#define DLL_TEST_RENDERUTILS_H
#include <cmath>
#include <imgui.h>

namespace Utils {
    namespace Render {
        inline float getLerpValue(const float a, const float b, const float t) {
            const float v = a - (b - a) * t;
            return std::ceilf(v * 1000.f) / 1000.f;
        }
        inline ImU32 rgbToCol(const int r, const int g, const int b, const float a = 1.f) {
            return IM_COL32(r, g, b, static_cast<int>(a * 255.f));
        }

        // converts a value designed at my dev screen height (1920) to the end users screen height
        inline float scaleToScreenHeight(float designPixels, float screenHeight) {
            return designPixels * (screenHeight / 1920.f);
        }
    }
}

#endif //DLL_TEST_RENDERUTILS_H
