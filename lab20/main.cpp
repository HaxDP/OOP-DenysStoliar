#include <iostream>
#include <string>
#include <vector>

enum class orderStatus 
{
    newOrder,
    pendingValidation,
    processed,
    shipped,
    delivered,
    cancelled
};

class order 
{
public:
    int id;
    std::string customerName;
    double totalAmount;
    orderStatus status;

    order(int orderId, std::string name, double amount) 
    {
        id = orderId;
        customerName = name;
        totalAmount = amount;
        status = orderStatus::newOrder;
    }
};

class iOrderValidator 
{
public:
    virtual bool isValid(const order& ord) = 0;
    virtual ~iOrderValidator() {}
};

class iOrderRepository 
{
public:
    virtual void save(const order& ord) = 0;
    virtual order* getById(int id) = 0;
    virtual ~iOrderRepository() {}
};

class iEmailService 
{
public:
    virtual void sendOrderConfirmation(const order& ord) = 0;
    virtual ~iEmailService() {}
};

class orderValidator : public iOrderValidator 
{
public:
    bool isValid(const order& ord) override 
    {
        return ord.totalAmount > 0;
    }
};

class inMemoryOrderRepository : public iOrderRepository 
{
private:
    std::vector<order> orders;

public:
    void save(const order& ord) override 
    {
        orders.push_back(ord);
        std::cout << "order " << ord.id << " saved to database" << std::endl;
    }

    order* getById(int id) override 
    {
        for (auto& ord : orders) 
        {
            if (ord.id == id) 
            {
                return &ord;
            }
        }
        return nullptr;
    }
};

class consoleEmailService : public iEmailService 
{
public:
    void sendOrderConfirmation(const order& ord) override 
    {
        std::cout << "email sent to customer " << ord.customerName 
                  << " for order " << ord.id << std::endl;
    }
};

class orderService 
{
private:
    iOrderValidator* validator;
    iOrderRepository* repository;
    iEmailService* emailService;

public:
    orderService(iOrderValidator* val, iOrderRepository* repo, iEmailService* email) 
    {
        validator = val;
        repository = repo;
        emailService = email;
    }

    bool processOrder(order& ord) 
    {
        std::cout << "\nprocessing order " << ord.id << "..." << std::endl;

        if (!validator->isValid(ord)) 
        {
            std::cout << "error: order is not valid (amount <= 0)" << std::endl;
            ord.status = orderStatus::cancelled;
            return false;
        }

        ord.status = orderStatus::pendingValidation;
        repository->save(ord);
        ord.status = orderStatus::processed;
        emailService->sendOrderConfirmation(ord);
        ord.status = orderStatus::shipped;

        std::cout << "order processed successfully!" << std::endl;
        return true;
    }
};

class orderProcessor 
{
public:
    void processOrder(order& ord) 
    {
        std::cout << "\nprocessing order with orderProcessor:" << std::endl;
        
        if (ord.totalAmount <= 0) 
        {
            std::cout << "error: order is not valid" << std::endl;
            return;
        }

        std::cout << "order " << ord.id << " saved" << std::endl;
        std::cout << "email sent" << std::endl;
        ord.status = orderStatus::processed;
        std::cout << "status updated" << std::endl;
    }
};

int main() 
{
    std::cout << "\ndemonstrating class that violates SRP:" << std::endl;
    orderProcessor oldProcessor;
    order order1(1, "Yura Kovalchyk", 100.50);
    oldProcessor.processOrder(order1);

    std::cout << "\n----------------------------------------" << std::endl;

    std::cout << "\ndemonstrating refactored version:" << std::endl;

    orderValidator validator;
    inMemoryOrderRepository repository;
    consoleEmailService emailService;

    orderService service(&validator, &repository, &emailService);

    order validOrder(2, "Stoliar Denys", 250.75);
    service.processOrder(validOrder);

    order invalidOrder(3, "Sasha Harchyk", -50.0);
    service.processOrder(invalidOrder);

    std::cout << "\ndemonstrating repository work" << std::endl;
    order* retrievedOrder = repository.getById(2);
    if (retrievedOrder != nullptr) 
    {
        std::cout << "found order: " << retrievedOrder->customerName 
                  << ", amount: " << retrievedOrder->totalAmount << "\n" << std::endl;
    }
}