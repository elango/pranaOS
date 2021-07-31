/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <base/Function.h>
#include <base/RefCounted.h>
#include <base/RefPtr.h>
#include <base/StringView.h>
#include <base/Types.h>
#include <kernel/filesystem/File.h>
#include <kernel/filesystem/FileSystem.h>
#include <kernel/Forward.h>
#include <kernel/KResult.h>

namespace Kernel {

class SysFSComponent : public RefCounted<SysFSComponent> {
public:
    virtual StringView name() const { return m_name->view(); }
    virtual KResultOr<size_t> read_bytes(off_t, size_t, UserOrKernelBuffer&, FileDescription*) const { VERIFY_NOT_REACHED(); }
    virtual KResult traverse_as_directory(unsigned, Function<bool(FileSystem::DirectoryEntryView const&)>) const { VERIFY_NOT_REACHED(); }
    virtual RefPtr<SysFSComponent> lookup(StringView) { VERIFY_NOT_REACHED(); };
    virtual KResultOr<size_t> write_bytes(off_t, size_t, UserOrKernelBuffer const&, FileDescription*) { return -EROFS; }
    virtual size_t size() const { return 0; }

    virtual NonnullRefPtr<Inode> to_inode(SysFS const&) const;

    InodeIndex component_index() const { return m_component_index; };

    virtual ~SysFSComponent() = default;

protected:
    explicit SysFSComponent(StringView name);

private:
    NonnullOwnPtr<KString> m_name;
    InodeIndex m_component_index {};
};

class SysFSDirectory : public SysFSComponent {
public:
    virtual KResult traverse_as_directory(unsigned, Function<bool(FileSystem::DirectoryEntryView const&)>) const override;
    virtual RefPtr<SysFSComponent> lookup(StringView name) override;

    virtual NonnullRefPtr<Inode> to_inode(SysFS const& sysfs_instance) const override final;

protected:
    explicit SysFSDirectory(StringView name);
    SysFSDirectory(StringView name, SysFSDirectory const& parent_directory);
    NonnullRefPtrVector<SysFSComponent> m_components;
    RefPtr<SysFSDirectory> m_parent_directory;
};

}