#pragma once

#include <coroutine>
#include <future>

namespace CoroutineUtils
{
    struct coroutine_void
    {
        struct promise_type
        {
            coroutine_void get_return_object() { return {}; }

            std::suspend_never initial_suspend() { return {}; }

            std::suspend_never final_suspend() noexcept { return {}; }

            void return_void() {}

            void unhandled_exception() { std::terminate(); }
        };
    };

    template<typename T>
    struct Awaitable
    {
        std::future<T> future;

        bool await_ready() const noexcept
        {
            return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        }

        void await_suspend(std::coroutine_handle<> handle)
        {
            std::thread([this, handle]() mutable {
                future.wait();
                handle.resume();
            }).detach();
        }

        T await_resume() { return future.get(); }
    };
}   // namespace CoroutineUtils