//
// Created by os on 7/16/22.
//

#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H
#include "../../lib/hw.h"


namespace kernel {
    class Thread {
    public:
        friend class Scheduler;
        friend class Semaphore;

        using TTask = void(*)(void*);

        enum class Owner {
            USER, SYSTEM
        };

        enum class Status {
            CREATED, READY, RUNNING, BLOCKED
        };

        class Context {
        public:
            friend Thread;
            struct Registers{
                uint64 zero, ra, sp, gp, tp, t0, t1, t2,
                        s0, s1, a0, a1, a2, a3, a4, a5,
                        a6, a7, s2, s3, s4, s5, s6, s7,
                        s8, s9, s10, s11, t3, t4, t5, t6;
                Registers();
            };
        private:
            uint64 pc = 0x00;
            uint64 sstatus = 0x00;
            Registers registers;
        public:
            explicit Context(uint64 pc = 0x00, uint64 status = 0);
            Registers& getRegisters() { return registers; }
        };
    public:
        static void* operator new(size_t size) noexcept;
        static void operator delete(void* ptr) noexcept;

        static void initialize();

        static void tick();
        static void dispatch();

        static Thread* getMainThread();
        static Thread* getRunning();
    private:
        static void taskWrapper();

        static uint64 sstatusGetInitial();
        static uint64 pcGetInitial(TTask function);
        static Owner runningThreadOwner();

        static Thread* mainThread;
        static Thread* runningThread asm("__runningThread");
        static time_t runningTimeLeft;

        static uint64 threadIdSource;
    private:
        Context context;
        TTask task;
        void* arg;
        uint64* stack;
        Owner owner;
        Thread* next = nullptr;
        uint64 id = threadIdSource++;
        Status status = Status::READY;
    public:
        Thread(TTask function, void* argument, void* stack);
        Thread(TTask function, void* argument, void* stack, Owner type);
        Thread(Thread const&)=delete;
        Thread& operator=(Thread const&)=delete;
        ~Thread();

        Context& getConetxt() { return context; }
        uint64 getThreadId() const { return id; }
        uint64 getPC() const { return context.pc; }
        void setPC(uint64 value) { context.pc = value; }
        uint64 getsstatus() const{ return context.sstatus; }
        void setStatus(Status value) { status = value; };
        Status getStatus() { return status; };
        bool isUserThread() const { return owner == Owner::USER; }
    };
} // kernel

#endif //PROJECT_THREAD_H
