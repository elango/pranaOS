/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/


// includes
#include <base/Array.h>
#include <base/Base64.h>
#include <base/ByteBuffer.h>
#include <base/String.h>
#include <base/StringBuilder.h>
#include <base/StringView.h>
#include <base/Types.h>
#include <base/Vector.h>

namespace Base 
{

static constexpr auto make_alphabet()
{
    Array alphabet = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };
    return alphabet;
}

static constexpr auto make_lookup_table()
{
    constexpr auto alphabet = make_alphabet();
    Array<u8, 256> table {};
    for (size_t i = 0; i < alphabet.size(); ++i) {
        table[alphabet[i]] = i;
    }
    return table;
}

size_t calculate_base64_decoded_length(const StringView& input)
{
    return input.length() * 3 / 4;
}

size_t calculate_base64_decoded_length(ReadonlyBytes input)
{
    return ((4 * input.size() / 3) + 3) & ~3;
}

ByteBuffer decode_base64(const StringView& input)
{
    auto get = [&](const size_t offset, bool* is_padding = nullptr) -> u8 {
        constexpr auto table = make_lookup_table();
        if (offset >= input.length())
            return 0;
        if (input[offset] == '=') {
            if (is_padding)
                *is_padding = true;
            return 0;
        }
        return table[input[offset]];
    };

    Vector<u8> output;
    output.ensure_capacity(calculate_base64_decoded_length(input));

    for (size_t i = 0; i < input.length(); i += 4) {
        bool in2_is_padding = false;
        bool in3_is_padding = false;

        const u8 in0 = get(i);
        const u8 in1 = get(i + 1);
        const u8 in2 = get(i + 2, &in2_is_padding);
        const u8 in3 = get(i + 3, &in3_is_padding);

        const u8 out0 = (in0 << 2) | ((in1 >> 4) & 3);
        const u8 out1 = ((in1 & 0xf) << 4) | ((in2 >> 2) & 0xf);
        const u8 out2 = ((in2 & 0x3) << 6) | in3;

        output.append(out0);
        if (!in2_is_padding)
            output.append(out1);
        if (!in3_is_padding)
            output.append(out2);
    }

    return ByteBuffer::copy(output.data(), output.size());
}

}