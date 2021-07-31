/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <kernel/filesystem/File.h>

namespace Kernel {

class Inode;

class InodeFile final : public File {
public:
    static KResultOr<NonnullRefPtr<InodeFile>> create(NonnullRefPtr<Inode>&& inode)
    {
        auto file = adopt_ref_if_nonnull(new (nothrow) InodeFile(move(inode)));
        if (!file)
            return ENOMEM;
        return file.release_nonnull();
    }

    virtual ~InodeFile() override;

    const Inode& inode() const { return *m_inode; }
    Inode& inode() { return *m_inode; }

    virtual bool can_read(const FileDescription&, size_t) const override { return true; }
    virtual bool can_write(const FileDescription&, size_t) const override { return true; }

    virtual KResultOr<size_t> read(FileDescription&, u64, UserOrKernelBuffer&, size_t) override;
    virtual KResultOr<size_t> write(FileDescription&, u64, const UserOrKernelBuffer&, size_t) override;
    virtual KResult ioctl(FileDescription&, unsigned request, Userspace<void*> arg) override;
    virtual KResultOr<Region*> mmap(Process&, FileDescription&, const Range&, u64 offset, int prot, bool shared) override;
    virtual KResult stat(::stat& buffer) const override { return inode().metadata().stat(buffer); }

    virtual String absolute_path(const FileDescription&) const override;

    virtual KResult truncate(u64) override;
    virtual KResult chown(FileDescription&, uid_t, gid_t) override;
    virtual KResult chmod(FileDescription&, mode_t) override;

    virtual StringView class_name() const override { return "InodeFile"; }

    virtual bool is_seekable() const override { return true; }
    virtual bool is_inode() const override { return true; }

private:
    explicit InodeFile(NonnullRefPtr<Inode>&&);
    NonnullRefPtr<Inode> m_inode;
};

}