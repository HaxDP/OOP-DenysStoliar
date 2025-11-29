#include "polly.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <thread>

class FlakyService 
{
public:
    FlakyService(int failTimes) : failTimes_(failTimes), attempts_(0) {}

    std::string Call() 
    {
        ++attempts_;
        std::cout << "[INFO] Attempt " << attempts_ << " to call external service\n";
        if (attempts_ <= failTimes_) 
        {
            throw std::runtime_error("Simulated transient error");
        }
        return "Success from external service";
    }

    std::string SlowCall() 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return "Slow success";
    }

private:
    int failTimes_;
    int attempts_;
};

int main() 
{
    std::cout << "=== Start ===\n\n";

    {
        FlakyService svc(2);
        RetryPolicy retry(4, std::chrono::milliseconds(200));
        try 
        {
            std::string res = retry.Execute([&]() 
            {
                return svc.Call();
            });
            std::cout << "[RESULT] " << res << "\n\n";
        } catch (const std::exception& ex) 
        {
            std::cout << "[ERROR] Operation failed after retries: " << ex.what() << "\n\n";
        }
    }

    {
        FlakyService svc(5);
        CircuitBreaker cb(3, std::chrono::milliseconds(2000));

        for (int i = 0; i < 6; ++i) 
        {
            try 
            {
                std::string res = cb.Execute([&]() 
                {
                    return svc.Call();
                });
                std::cout << "[RESULT] " << res << "\n";
            } catch (const std::exception& ex) 
            {
                std::cout << "[CB] Call failed: " << ex.what() << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
        std::cout << "\n";
    }

    {
        FlakyService svc(0);
        try {
            auto res = ExecuteWithTimeout([&]() { return svc.SlowCall(); }, std::chrono::milliseconds(200));
            std::cout << "[RESULT] " << res << "\n";
        } catch (const TimeoutException& ex) 
        {
            std::cout << "[TIMEOUT] " << ex.what() << "\n";
        }
    }

    {
        FlakyService svc(2);
        RetryPolicy retry(3, std::chrono::milliseconds(100));
        try 
        {
            auto res = retry.Execute([&]() 
            {
                return ExecuteWithTimeout([&]() { return svc.Call(); }, std::chrono::milliseconds(300));
            });
            std::cout << "[RESULT] Combined: " << res << "\n";
        } catch (const std::exception& ex) 
        {
            std::cout << "[ERROR] Combined operation failed: " << ex.what() << "\n";
        }
    }
    std::cout << "\n=== End ===\n";
}