/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Entity
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <cstdint>
#include <bitset>

#define MAX_ENTITIES 2048
#define MAX_COMPONENTS 64

using Entity = std::uint32_t;

using ComponentType = std::uint16_t;

using Signature = std::bitset<MAX_COMPONENTS>;

#endif /* !ENTITY_HPP_ */
