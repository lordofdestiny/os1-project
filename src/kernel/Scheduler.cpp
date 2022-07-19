//
// Created by os on 7/16/22.
//

#include "../../h/kernel/Scheduler.h"
#include "../../h/kernel/MemoryAllocator.h"

namespace kernel {
    Scheduler& Scheduler::getInstance() {
        static Scheduler instance{};
        return instance;
    }

    TCB *Scheduler::get() {
        if(readyHead == nullptr) return getIdleThread();

        auto thread = readyHead;
        readyHead = readyHead->next;
        if(readyHead == nullptr) {
            readyTail = nullptr;
        }
        thread->next= nullptr;
        if(thread->isUserThread()){
            userThreadCount--;
        }

        return thread;
    }

    void Scheduler::put(kernel::TCB *thread) {
        if(thread == idleThread) return;

        thread->status = TCB::ThreadStatus::READY;
        if(readyHead == nullptr) {
            readyHead = thread;
        }else {
            readyTail->next = thread;
        }
        readyTail = thread;

        if(thread->isUserThread()){
            userThreadCount++;
        }
    }

    TCB* Scheduler::getIdleThread() {
        auto& allocator = MemoryAllocator::getInstance();
        auto task = [](void*) {
            while(true);
        };
        if(!idleThread) {
            void* stack = allocator.allocateBytes(DEFAULT_STACK_SIZE);
            idleThread = new TCB(task, nullptr, stack, TCB::ThreadType::SYSTEM);
        }
        return idleThread;
    }

    bool Scheduler::hasUserThreads() const{
        return userThreadCount != 0;
    }
}
