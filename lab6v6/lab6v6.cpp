#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

class Order {
public:
    int id;
    double sum;
    std::string status;

    Order(int id, double sum, const std::string& status)
        : id(id), sum(sum), status(status) {}
};

int main() 
{
    std::vector<Order> orders = 
    {
        {1, 1500.0, "Completed"},
        {2, 800.0,  "Pending"},
        {3, 2300.0, "Completed"},
        {4, 500.0,  "Cancelled"},
        {5, 1200.0, "Pending"},
        {6, 3400.0, "Completed"}
    };

    // Загальна сума виконаних замовлень
    auto totalCompleted = std::accumulate
    (
        orders.begin(), orders.end(), 0.0,
        [](double acc, const Order& o) 
        {
            return o.status == "Completed" ? acc + o.sum : acc;
        }
    );

    // Кількість Pending
    auto countPending = std::count_if
    (
        orders.begin(), orders.end(),
        [](const Order& o) { return o.status == "Pending"; }
    );

    // Сортування за сумою
    std::sort
    (
        orders.begin(), orders.end(),
        [](const Order& a, const Order& b) { return a.sum < b.sum; }
    );

    std::cout << "Total Completed Sum: " << totalCompleted << "\n";
    std::cout << "Pending Count: " << countPending << "\n\n";

    std::cout << "Sorted Orders by Sum:\n";
    for (const auto& o : orders) 
    {
        std::cout << "#" << o.id << " : " << o.sum 
                  << " (" << o.status << ")\n";
    }
}
