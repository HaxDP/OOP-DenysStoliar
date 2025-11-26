#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <functional>

using namespace std;

// --- fileProcessor ---
class FileProcessor 
{
    int failCount = 0; // перші 2 рази імітуємо помилку
public:
    vector<string> LoadOrderIds(const string& path)
    {
        if (failCount < 2)
        {
            failCount++;
            throw runtime_error("IOException: unable to read file");
        }
        return { "ORD-001", "ORD-002", "ORD-003" };
    }
};

// --- networkClient ---
class NetworkClient 
{
    int failCount = 0; // пперші 3 рази імітуємо HttpRequestException
public:
    vector<string> GetOrdersFromApi(const string& url) 
    {
        if (failCount < 3) 
        {
            failCount++;
            throw runtime_error("httpRequestException: network unavailable");
        }
        return { "API-100", "API-200", "API-300" };
    }
};

// --- retryHelper ---
class RetryHelper 
{
public:
    template<typename T>
    static T ExecuteWithRetry(
        function<T()> operation,
        int retryCount = 3,
        chrono::milliseconds initialDelay = chrono::milliseconds(500),
        function<bool(const exception&)> shouldRetry = nullptr
    ) 
    {
        int attempt = 0;

        while (true) 
        {
            try 
            {
                return operation();
            }
            catch (const exception& ex) 
            {
                cout << "attempt #" << attempt + 1 << " ended with error: "
                     << ex.what() << endl;

                if (attempt >= retryCount - 1) 
                {
                    cout << "to many attempts. go now and newer return.\n";
                    throw;
                }

                if (shouldRetry && !shouldRetry(ex)) 
                {
                    cout << "shouldRetry declined next attempt.\n";
                    throw;
                }

                // експоненційна затримка
                auto delay = initialDelay * (1 << attempt);
                cout << "waiting " << delay.count() << " ms...\n\n";
                this_thread::sleep_for(delay);
            }
            attempt++;
        }
    }
};

int main() 
{
    FileProcessor fp;
    NetworkClient nc;

    auto shouldRetry = [](const exception& ex) 
    {
        string msg = ex.what();
        return msg.find("IOException") != string::npos ||
               msg.find("HttpRequestException") != string::npos;
    };

    cout << "\n=== READING ORDERS FROM FILE ===\n";
    try 
    {
        auto fileOrders = RetryHelper::ExecuteWithRetry<vector<string>>(
            [&]() { return fp.LoadOrderIds("orders.txt"); },
            5, chrono::milliseconds(400), shouldRetry
        );
        cout << "success, data from file:\n";
        for (auto& id : fileOrders) cout << id << endl;
    }
    catch (...) 
    {
        cout << "error reading file.\n";
    }

    cout << "\n=== ORDER API REQUEST ===\n";
    try 
    {
        auto apiOrders = RetryHelper::ExecuteWithRetry<vector<string>>(
            [&]() { return nc.GetOrdersFromApi("https://api/orders"); },
            5, chrono::milliseconds(400), shouldRetry
        );
        cout << "success, data from API:\n";
        for (auto& id : apiOrders) cout << id << endl;
    }
    catch (...) 
    {
        cout << "error network request.\n";
    }

    return 0;
}