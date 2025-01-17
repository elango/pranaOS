/*
 * Copyright (c) 2021, nuke123-sudo
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
 
// includes
#include <kernel/filesystem/AnonymousFile.h>
#include <kernel/Process.h>
#include <kernel/vm/AnonymousVMObject.h>

namespace Kernel {

AnonymousFile::AnonymousFile(NonnullRefPtr<AnonymousVMObject> vmobject)
    : m_vmobject(move(vmobject))
{
}

AnonymousFile::~AnonymousFile()
{
}

KResultOr<Region*> AnonymousFile::mmap(Process& process, FileDescription&, const Range& range, u64 offset, int prot, bool shared)
{
    if (offset != 0)
        return EINVAL;

    if (range.size() != m_vmobject->size())
        return EINVAL;

    return process.space().allocate_region_with_vmobject(range, m_vmobject, offset, {}, prot, shared);
}

}
