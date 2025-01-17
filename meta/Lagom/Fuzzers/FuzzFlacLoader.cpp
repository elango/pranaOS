/*
 * Copyright (c) 2021, nuke123-sudo
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <libaudio/FlacLoader.h>
#include <stddef.h>
#include <stdint.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    auto flac_data = ByteBuffer::copy(data, size);
    auto flac = make<Audio::FlacLoaderPlugin>(flac_data);

    if (!flac->sniff())
        return 1;

    while (flac->get_more_samples())
        ;

    return 0;
}
