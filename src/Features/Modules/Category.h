//
// Created by Edward on 22/10/2026.
//

#ifndef CLOUD_CLIENT_CATEGORY_H
#define CLOUD_CLIENT_CATEGORY_H

enum class Category { Combat, Movement, Visual, Player, Misc, Count }; // count will always be the amount of cats
inline const char* getCategoryName(Category category) {
    switch (category) {
        case Category::Combat: return "Combat";
        case Category::Movement: return "Movement";
        case Category::Visual: return "Visual";
        case Category::Player: return "Player";
        case Category::Misc: return "Misc";
        case Category::Count: return "DEVELOPER STUB. YOU SHOULD NOT SEE THIS";
    }
    return "idiot you forgot a category or got it wrong";
}

#endif //CLOUD_CLIENT_CATEGORY_H
