/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

// includes
#include <base/HashMap.h>
#include <base/Singleton.h>
#include <base/StringView.h>
#include <kernel/arch/x86/InterruptDisabler.h>
#include <kernel/filesystem/FileSystem.h>
#include <kernel/filesystem/Inode.h>
#include <kernel/net/LocalSocket.h>
#include <kernel/vm/MemoryManager.h>

namespace Kernel {

static u32 s_lastFileSystemID;
static Base::Singleton<HashMap<u32, FileSystem*>> s_file_system_map;

static HashMap<u32, FileSystem*>& all_file_systems()
{
    return *s_file_system_map;
}

FileSystem::FileSystem()
    : m_fsid(++s_lastFileSystemID)
{
    s_file_system_map->set(m_fsid, this);
}

FileSystem::~FileSystem()
{
    s_file_system_map->remove(m_fsid);
}

FileSystem* FileSystem::from_fsid(u32 id)
{
    auto it = all_file_systems().find(id);
    if (it != all_file_systems().end())
        return (*it).value;
    return nullptr;
}

FileSystem::DirectoryEntryView::DirectoryEntryView(const StringView& n, InodeIdentifier i, u8 ft)
    : name(n)
    , inode(i)
    , file_type(ft)
{
}

void FileSystem::sync()
{
    Inode::sync();

    NonnullRefPtrVector<FileSystem, 32> file_systems;
    {
        InterruptDisabler disabler;
        for (auto& it : all_file_systems())
            file_systems.append(*it.value);
    }

    for (auto& fs : file_systems)
        fs.flush_writes();
}

void FileSystem::lock_all()
{
    for (auto& it : all_file_systems()) {
        it.value->m_lock.lock();
    }
}

}