/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

// includes
#include <libabi/Process.h>
#include <libabi/Task.h>
#include <libio/Path.h>
#include <libutils/List.h>
#include "system/memory/Memory.h"
#include "system/scheduling/Blocker.h"
#include "system/tasking/Domain.h"
#include "system/tasking/Handles.h"

typedef void (*TaskEntryPoint)();

struct MemoryMapping;

struct Task
{
    int id;
    char name[PROCESS_NAME_SIZE];
    TaskFlags _flags;

    Syscall _current_syscall;
    bool _is_doing_syscall = false;
    bool _is_canceled = false;
    bool _is_interrupted = false;

    TaskState _state;
    Blocker *_blocker;

    uintptr_t user_stack_pointer;
    void *user_stack;

    uintptr_t kernel_stack_pointer;
    void *kernel_stack;

    TaskEntryPoint entry_point;
    char fpu_registers[512];

    List<MemoryMapping *> *memory_mapping;
    Arch::AddressSpace *address_space;

    int exit_value = 0;

    Handles _handles;
    Domain _domain;

    Handles &handles() { return _handles; }
    Domain &domain() { return _domain; }

    TaskState state();

    void state(TaskState state);

    JResult cancel(int exit_value);

    void try_unblock()
    {
        if (_blocker->can_unblock(*this))
        {
            _blocker->unblock(*this);
            state(TASK_STATE_RUNNING);
        }
        else if (_blocker->has_timeout())
        {
            _blocker->timeout(*this);
            state(TASK_STATE_RUNNING);
        }
        else if (_blocker->is_interrupted())
        {
            state(TASK_STATE_RUNNING);
        }
    }

    void begin_syscall(Syscall current)
    {
        _is_doing_syscall = true;
        _current_syscall = current;
    }

    void end_syscall()
    {
        _current_syscall = (Syscall)-1;
        _is_doing_syscall = false;
    }

    void interrupt();

    void kill_me_if_you_dare();
};