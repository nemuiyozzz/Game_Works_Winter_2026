#pragma once
#include <cstdint>

/// @brief エンティティを表現する型
using Entity = uint32_t;

/// @brief 無効なエンティティを示す定数
constexpr Entity NULL_ENTITY = 0xFFFFFFFF;