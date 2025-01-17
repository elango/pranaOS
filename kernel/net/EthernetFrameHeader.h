/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/Endian.h>
#include <base/MACAddress.h>

#pragma GCC diagnostic ignored "-Warray-bounds"

class [[gnu::packed]] EthernetFrameHeader {
public:
    EthernetFrameHeader() = default;
    ~EthernetFrameHeader() = default;

    MACAddress destination() const { return m_destination; }
    void set_destination(const MACAddress& address) { m_destination = address; }

    MACAddress source() const { return m_source; }
    void set_source(const MACAddress& address) { m_source = address; }

    u16 ether_type() const { return m_ether_type; }
    void set_ether_type(u16 ether_type) { m_ether_type = ether_type; }

    const void* payload() const { return &m_payload[0]; }
    void* payload() { return &m_payload[0]; }

private:
    MACAddress m_destination;
    MACAddress m_source;
    NetworkOrdered<u16> m_ether_type;
    u32 m_payload[0];
};

static_assert(sizeof(EthernetFrameHeader) == 14);