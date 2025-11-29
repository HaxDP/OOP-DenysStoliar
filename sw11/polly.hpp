// бібліотека для retry, circuit breaker, timeout
#pragma once
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <stdexcept>
#include <thread>

class TimeoutException : public std::runtime_error 
{
public:
    TimeoutException(const char* msg) : std::runtime_error(msg) {}
};

class RetryPolicy 
{
public:
    RetryPolicy(int maxRetries = 3, std::chrono::milliseconds delay = std::chrono::milliseconds(100))
        : maxRetries_(maxRetries), delay_(delay) {}

    template <typename F>
    auto Execute(F&& func) -> decltype(func()) 
    {
        int attempt = 0;
        while (true) 
        {
            try 
            {
                return func();
            } catch (...) 
            {
                if (++attempt > maxRetries_) throw;
                std::this_thread::sleep_for(delay_);
                delay_ *= 2;
            }
        }
    }

private:
    int maxRetries_;
    std::chrono::milliseconds delay_;
};

class CircuitBreaker 
{
public:
    CircuitBreaker(int failureThreshold = 3, std::chrono::milliseconds openDuration = std::chrono::milliseconds(2000))
        : threshold_(failureThreshold), openDuration_(openDuration), failures_(0), state_(State::CLOSED) {}

    template <typename F>
    auto Execute(F&& func) -> decltype(func()) 
    {
        std::unique_lock<std::mutex> lk(mutex_);
        if (state_ == State::OPEN) 
        {
            auto now = std::chrono::steady_clock::now();
            if (now < openUntil_) 
            {
                throw std::runtime_error("Circuit is open");
            } else 
            {
                state_ = State::HALF_OPEN;
            }
        }
        lk.unlock();

        try 
        {
            auto result = func();
            std::unique_lock<std::mutex> lk2(mutex_);
            failures_ = 0;
            state_ = State::CLOSED;
            return result;
        } catch (...) 
        {
            std::unique_lock<std::mutex> lk3(mutex_);
            ++failures_;
            if (failures_ >= threshold_) 
            {
                state_ = State::OPEN;
                openUntil_ = std::chrono::steady_clock::now() + openDuration_;
            }
            throw;
        }
    }

private:
    enum class State { CLOSED, OPEN, HALF_OPEN };
    int threshold_;
    std::chrono::milliseconds openDuration_;
    int failures_;
    State state_;
    std::chrono::steady_clock::time_point openUntil_;
    std::mutex mutex_;
};

template <typename F, typename R = std::invoke_result_t<F>>
R ExecuteWithTimeout(F&& func, std::chrono::milliseconds timeout) 
{
    auto future = std::async(std::launch::async, std::forward<F>(func));
    if (future.wait_for(timeout) == std::future_status::ready) 
    {
        return future.get();
    } else 
    {
        throw TimeoutException("Operation timed out");
    }
}