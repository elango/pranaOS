/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/DoublyLinkedList.h>
#include <base/Function.h>
#include <base/HashMap.h>
#include <kernel/devices/AsyncDeviceRequest.h>
#include <kernel/filesystem/File.h>
#include <kernel/Mutex.h>
#include <kernel/UnixTypes.h>

namespace Kernel {

class Device : public File {
public:
    virtual ~Device() override;

    unsigned major() const { return m_major; }
    unsigned minor() const { return m_minor; }

    virtual String absolute_path(const FileDescription&) const override;
    virtual String absolute_path() const;

    uid_t uid() const { return m_uid; }
    uid_t gid() const { return m_gid; }

    virtual mode_t required_mode() const = 0;
    virtual String device_name() const = 0;

    virtual bool is_device() const override { return true; }
    virtual bool is_disk_device() const { return false; }

    static void for_each(Function<void(Device&)>);
    static Device* get_device(unsigned major, unsigned minor);

    void process_next_queued_request(Badge<AsyncDeviceRequest>, const AsyncDeviceRequest&);

    template<typename AsyncRequestType, typename... Args>
    NonnullRefPtr<AsyncRequestType> make_request(Args&&... args)
    {
        auto request = adopt_ref(*new AsyncRequestType(*this, forward<Args>(args)...));
        ScopedSpinLock lock(m_requests_lock);
        bool was_empty = m_requests.is_empty();
        m_requests.append(request);
        if (was_empty)
            request->do_start(move(lock));
        return request;
    }

protected:
    Device(unsigned major, unsigned minor);
    void set_uid(uid_t uid) { m_uid = uid; }
    void set_gid(gid_t gid) { m_gid = gid; }

    static HashMap<u32, Device*>& all_devices();

private:
    unsigned m_major { 0 };
    unsigned m_minor { 0 };
    uid_t m_uid { 0 };
    gid_t m_gid { 0 };

    SpinLock<u8> m_requests_lock;
    DoublyLinkedList<RefPtr<AsyncDeviceRequest>> m_requests;
};

}