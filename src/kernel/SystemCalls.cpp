//
// Created by os on 7/16/22.
//
#include "../../h/kernel/SystemCalls.h"
#include "../../h/kernel/MemoryAllocator.h"
#include "../../h/kernel/Thread.h"
#include "../../h/kernel/Scheduler.h"
#include "../../h/syscall_c.h"
#include "../../h/kernel/Semaphore.h"
#include "../../h/kernel/Console.h"

#define RETURN_IF(test, value)      \
    do{                             \
        if((test)) {                \
            RETURN(value);          \
            return;                 \
        }                           \
    }while(0)                       \

namespace kernel {
    namespace SystemCalls {
        void mem_alloc() {
            auto blockCount = ACCEPT(size_t, 1);
            auto memory = ALLOCATOR.allocateBlocks(blockCount);
            RETURN(memory);
        }

        void mem_free() {
            auto memory = ACCEPT(void*, 1);
            int code = ALLOCATOR.deallocateBlocks(memory);
            RETURN(code);
        }

        void thread_create() {
            auto handle = ACCEPT(thread_t*, 1);
            auto task = ACCEPT(Thread::Task, 2);
            auto argument = ACCEPT(void*, 3);
            auto stack = ACCEPT(void*, 4);
            auto thread = new Thread(task, argument,
                                     stack, Thread::Mode::USER);
            if(thread == nullptr) {
                ALLOCATOR.deallocateBlocks(stack);
            }else {
                *handle = (thread_t) thread;
                SCHEDULER.put(thread);
            }
            RETURN(-(thread == nullptr));
        }

        void thread_exit() { // Handle if attempting to exit main
            auto runningThread = Thread::getRunning();
            RETURN_IF(runningThread == Thread::getMainThread(), -0x01);
            delete runningThread;
            Thread::dispatch();
            RETURN(0);
        }

        void sem_open() {
            auto handle = ACCEPT(sem_t*, 1);
            auto init = ACCEPT(unsigned , 2);
            auto semaphore = new Semaphore((int) init);
            RETURN_IF(semaphore == nullptr, -0x01);
            *handle = (sem_t) semaphore;
            RETURN(0);
        }

        void sem_close() {
            auto handle = ACCEPT(Semaphore*, 1);
            RETURN_IF(handle == nullptr, -0x01);
            delete handle;
            RETURN(0);
        }

        void sem_wait() {
            auto id = ACCEPT(Semaphore*, 1);
            RETURN_IF(id == nullptr, -0x01);
            RETURN(0);
            id->wait();
        }

        void sem_signal() {
            auto id = ACCEPT(Semaphore*, 1);
            RETURN_IF(id == nullptr, -0x01);
            RETURN(0);
            id->signal();
        }

        void time_sleep() {
            auto thread = Thread::getRunning();
            auto ticks = ACCEPT(time_t, 1);
            RETURN_IF(ticks < 0, -0x01);
            RETURN(0);
            SCHEDULER.entrance(thread, ticks);
        }

        void getc() {
            auto c = CONSOLE.readChar();
            RETURN(c);
        }

        void putc() {
            auto c = ACCEPT(char, 1);
            CONSOLE.writeChar(c);
        }

        void enter_user_mode() {
            auto thread = Thread::getRunning();
            thread->enterUserMode();
        }
    }
}