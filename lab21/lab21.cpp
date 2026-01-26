#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>

class igymStrategy
{
public:
    virtual ~igymStrategy() = default;
    virtual double calculateCost(double hours, bool hasExtraServices) = 0;
};

class morningPassStrategy : public igymStrategy
{
private:
    double hourRate = 80.0;
    double extraServiceRate = 150.0;
    double morningDiscount = 0.7;
    
public:
    double calculateCost(double hours, bool hasExtraServices) override
    {
        double baseCost = hours * hourRate * morningDiscount;
        if (hasExtraServices)
        {
            baseCost += extraServiceRate * morningDiscount;
        }
        return baseCost;
    }
};

class dayPassStrategy : public igymStrategy
{
private:
    double hourRate = 100.0;
    double extraServiceRate = 200.0;
    
public:
    double calculateCost(double hours, bool hasExtraServices) override
    {
        double baseCost = hours * hourRate;
        if (hasExtraServices)
        {
            baseCost += extraServiceRate;
        }
        return baseCost;
    }
};

class fullPassStrategy : public igymStrategy
{
private:
    double hourRate = 120.0;
    double extraServiceRate = 250.0;
    double fullPassDiscount = 0.8;
    
public:
    double calculateCost(double hours, bool hasExtraServices) override
    {
        double baseCost = hours * hourRate * fullPassDiscount;
        if (hasExtraServices)
        {
            baseCost += extraServiceRate * fullPassDiscount;
        }
        return baseCost;
    }
};

class studentPassStrategy : public igymStrategy
{
private:
    double hourRate = 60.0;
    double extraServiceRate = 100.0;
    double studentDiscount = 0.5;
    
public:
    double calculateCost(double hours, bool hasExtraServices) override
    {
        double baseCost = hours * hourRate * studentDiscount;
        if (hasExtraServices)
        {
            baseCost += extraServiceRate * studentDiscount;
        }
        return baseCost;
    }
};

class gymStrategyFactory 
{
public:
    static igymStrategy* createStrategy(const std::string& pass_type) 
    {
        return pass_type == "morning" ? static_cast<igymStrategy*>(new morningPassStrategy()) :
               pass_type == "day" ? static_cast<igymStrategy*>(new dayPassStrategy()) :
               pass_type == "full" ? static_cast<igymStrategy*>(new fullPassStrategy()) :
               pass_type == "student" ? static_cast<igymStrategy*>(new studentPassStrategy()) :
               throw std::invalid_argument("unknown pass type");
    }
};

class gymService
{
public:
    double calculateCost(double hours, bool extraServices, igymStrategy* strategy)
    {
        return strategy->calculateCost(hours, extraServices);
    }
};

int main()
{
    gymService gymService;
    
    std::string passType;
    double hours;
    std::string extraServicesInput;
    
    std::cout << "Available pass types: morning, day, full, student\n";
    while (true)
    {
        std::cout << "Enter pass type: ";
        std::cin >> passType;
        
        if (passType == "morning" || passType == "day" || passType == "full" || passType == "student")
        {
            break;
        }
        std::cout << "Invalid pass type! Please enter: morning, day, full, or student\n";
    }
    while (true)
    {
        std::cout << "Enter number of hours: ";
        std::cin >> hours;
        
        if (std::cin.fail() || hours <= 0)
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input! Please enter a positive number\n";
        }
        else
        {
            break;
        }
    }
    while (true)
    {
        std::cout << "Need extra services (sauna/pool)? (yes/no): ";
        std::cin >> extraServicesInput;
        
        if (extraServicesInput == "yes" || extraServicesInput == "no")
        {
            break;
        }
        std::cout << "Invalid input! Please enter 'yes' or 'no'\n";
    }
    
    bool extraServices = (extraServicesInput == "yes");
    
    try
    {
        std::unique_ptr<igymStrategy> strategy(gymStrategyFactory::createStrategy(passType));
        double cost = gymService.calculateCost(hours, extraServices, strategy.get());
        
        std::cout << "\nGym pass calculation:\n";
        std::cout << "Pass type: " << passType << "\n";
        std::cout << "Hours: " << hours << "\n";
        std::cout << "Extra services: " << (extraServices ? "yes" : "no") << "\n";
        std::cout << "Total cost: " << cost << " UAH\n";
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
    
    return 0;
}