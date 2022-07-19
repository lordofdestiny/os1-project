//
// Created by os on 7/16/22.
//

#ifndef PROJECT_TCB_H
#define PROJECT_TCB_H
#include "../../lib/hw.h"

namespace kernel {
    class TCB {
    public:
        using ThreadTask = void(*)(void*);

        enum class ThreadType {
            USER, SYSTEM
        };

        enum class ThreadStatus {
            CREATED, READY, RUNNING, BLOCKED
        };

        friend class Scheduler;
        friend class SystemCalls;
        friend class Semaphore;

        struct Registers{
            uint64 zero, ra, sp, gp, tp, t0, t1, t2,
                    s0, s1, a0, a1, a2, a3, a4, a5,
                    a6, a7, s2, s3, s4, s5, s6, s7,
                    s8, s9, s10, s11, t3, t4, t5, t6;
            Registers();
        };
    public:
        static void* operator new(size_t size) noexcept;
        static void operator delete(void* ptr) noexcept;

        static void initialize();

        static void tick();
        static void dispatch();

        static TCB* getMainThread();
        static TCB* getRunningThread();
    private:
        static void taskWrapper();

        static uint64 sstatusGetInitial();
        static uint64* pcGetInitial(ThreadTask function);
        static ThreadType runningThreadType();

        static TCB* mainThread;
        static TCB* runningThread;
        static time_t runningTimeLeft;

        static uint64 threadIdSource;
    private:
        struct ThreadContext {
            uint64* pc = nullptr;
            uint64 sstatus;
            Registers registers;
            explicit ThreadContext(uint64 status = 0, uint64* pc = nullptr);
        } context;
        ThreadTask task;
        void* arg;
        uint64* stack;
        ThreadType type;
        TCB* next = nullptr;
        uint64 id = threadIdSource++;
        ThreadStatus status = ThreadStatus::READY;
    public:
        TCB(ThreadTask function, void* argument, void* stack);
        TCB(ThreadTask function, void* argument, void* stack, ThreadType type);
        TCB(TCB const&)=delete;
        TCB& operator=(TCB const&)=delete;
        ~TCB();

        uint64 getThreadId() const { return id; }
        Registers& getRegisters() { return context.registers; }
        uint64* getPC() const { return context.pc; }
        void setPC(uint64* value) { context.pc = value; }
        uint64 getsstatus() const{ return context.sstatus; }
        void setStatus(ThreadStatus value) { status = value; };
        ThreadStatus getStatus() { return status; };
        bool isUserThread() const { return type == ThreadType::USER; }
    };
} // kernel

#endif //PROJECT_TCB_H
