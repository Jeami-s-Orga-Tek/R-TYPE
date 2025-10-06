/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** EnemyInfo
*/

#ifndef ENEMYINFO_HPP_
#define ENEMYINFO_HPP_

enum class ENEMY_TYPES {
    SIMPLE,
    SINE_WAVE,
};

namespace Engine {
    namespace Components {
        typedef struct EnemyInfo_s {
            int health;
            int maxHealth;
            int type;
            int scoreValue;
            float speed;
            bool isActive;
        } EnemyInfo;
    }
}

#endif /* !ENEMYINFO_HPP_ */
