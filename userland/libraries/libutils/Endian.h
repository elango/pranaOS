/*
 * Copyright (c) 2021, nuke123-sudo
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

#include <libutils/Prelude.h>

namespace Utils
{

template <typename TValue>
constexpr TValue swap_little_endian(TValue value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return value;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    if constexpr (sizeof(TValue) == 8)
        return __builtin_bswap64(value);
    if constexpr (sizeof(TValue) == 4)
        return __builtin_bswap32(value);
    if constexpr (sizeof(TValue) == 2)
        return __builtin_bswap16(value);
    if constexpr (sizeof(TValue) == 1)
        return value;
#endif
}
