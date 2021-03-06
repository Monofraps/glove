#include "glove/threading/Thread.hpp"

namespace BlueDwarf {

Thread::Thread(Runnable::Handle runnable) : runnable(std::move(runnable)) {
    interruptFlag.store(false, std::memory_order_release);
    executionFinished.store(false, std::memory_order_release);
}

Thread::~Thread() {
    DebugAssert(nativeThread.get_id() != std::this_thread::get_id());

    if (nativeThread.joinable()) {
        Interrupt();
        Join();
    }
}

void Thread::Start() {
    // Check whether the thread has already been started. std::thread::joinable() returns true in such cases.
    if (nativeThread.joinable()) {
        // todo throw
        throw 0;
    }

    nativeThread = std::thread([this]() {
        this->Run();
        this->executionFinished.store(true, std::memory_order_release);
    });
}

void Thread::Interrupt() {
    interruptFlag.store(true, std::memory_order_release);
}

void Thread::Join() {
    if (!nativeThread.joinable()) {
        // todo throw
        throw 0;
    }

    nativeThread.join();
}

bool Thread::HasFinishedExecution() const {
    return executionFinished.load(std::memory_order_acquire);
}

bool Thread::InterruptRequested() const {
    return interruptFlag.load(std::memory_order_acquire);
}

void Thread::Run() {
    runnable->Run(*this);
}

Thread::Handle Thread::CreateThread(Runnable::Handle runnable) {
    return Thread::Handle(new Thread(std::move(runnable)));
}
}