#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class DiscountCalculator
{
public:
    double CalculateDiscount(double originalPrice, double discountPercent) const
    {
        if (originalPrice < 0.0)
        {
            throw std::out_of_range("original price cannot be negative");
        }

        if (discountPercent < 0.0 || discountPercent > 100.0)
        {
            throw std::out_of_range("discount percent must be in range 0..100");
        }

        return originalPrice - (originalPrice * discountPercent / 100.0);
    }

    double ApplyCoupon(double originalPrice, const std::string& couponCode) const
    {
        if (originalPrice < 0.0)
        {
            throw std::out_of_range("original price cannot be negative");
        }

        if (couponCode.empty())
        {
            throw std::invalid_argument("coupon code cannot be empty");
        }

        if (couponCode == "save10")
        {
            return CalculateDiscount(originalPrice, 10.0);
        }

        if (couponCode == "save20")
        {
            return CalculateDiscount(originalPrice, 20.0);
        }

        if (couponCode == "fixed50")
        {
            double result = originalPrice - 50.0;
            return result < 0.0 ? 0.0 : result;
        }

        throw std::invalid_argument("unsupported coupon code");
    }
};

bool AreEqual(double left, double right, double epsilon = 1e-9)
{
    return std::fabs(left - right) < epsilon;
}

void AssertTrue(bool condition, const std::string& testName)
{
    if (!condition)
    {
        throw std::runtime_error(testName + " failed");
    }
}

void AssertThrows(const std::function<void()>& action, const std::string& testName)
{
    bool thrown = false;

    try
    {
        action();
    }
    catch (...)
    {
        thrown = true;
    }

    if (!thrown)
    {
        throw std::runtime_error(testName + " failed");
    }
}

int main()
{
    DiscountCalculator calculator;
    int passed = 0;
    int failed = 0;

    auto RunTest = [&](const std::string& testName, const std::function<void()>& testAction)
    {
        try
        {
            testAction();
            ++passed;
            std::cout << "pass: " << testName << '\n';
        }
        catch (const std::exception& exception)
        {
            ++failed;
            std::cout << "fail: " << testName << " -> " << exception.what() << '\n';
        }
    };

    RunTest("calculatediscount normal case",
        [&]()
        {
            AssertTrue(AreEqual(calculator.CalculateDiscount(100.0, 10.0), 90.0), "calculatediscount normal case");
        });

    RunTest("calculatediscount with zero percent",
        [&]()
        {
            AssertTrue(AreEqual(calculator.CalculateDiscount(123.0, 0.0), 123.0), "calculatediscount with zero percent");
        });

    RunTest("calculatediscount with hundred percent",
        [&]()
        {
            AssertTrue(AreEqual(calculator.CalculateDiscount(123.0, 100.0), 0.0), "calculatediscount with hundred percent");
        });

    RunTest("calculatediscount negative price throws",
        [&]()
        {
            AssertThrows([&]() { calculator.CalculateDiscount(-1.0, 10.0); }, "calculatediscount negative price throws");
        });

    RunTest("calculatediscount invalid percent throws",
        [&]()
        {
            AssertThrows([&]() { calculator.CalculateDiscount(100.0, 101.0); }, "calculatediscount invalid percent throws");
        });

    RunTest("applycoupon save10",
        [&]()
        {
            AssertTrue(AreEqual(calculator.ApplyCoupon(200.0, "save10"), 180.0), "applycoupon save10");
        });

    RunTest("applycoupon save20",
        [&]()
        {
            AssertTrue(AreEqual(calculator.ApplyCoupon(200.0, "save20"), 160.0), "applycoupon save20");
        });

    RunTest("applycoupon fixed50",
        [&]()
        {
            AssertTrue(AreEqual(calculator.ApplyCoupon(200.0, "fixed50"), 150.0), "applycoupon fixed50");
        });

    RunTest("applycoupon fixed50 floor to zero",
        [&]()
        {
            AssertTrue(AreEqual(calculator.ApplyCoupon(20.0, "fixed50"), 0.0), "applycoupon fixed50 floor to zero");
        });

    RunTest("applycoupon unknown code throws",
        [&]()
        {
            AssertThrows([&]() { calculator.ApplyCoupon(100.0, "bad"); }, "applycoupon unknown code throws");
        });

    RunTest("applycoupon empty code throws",
        [&]()
        {
            AssertThrows([&]() { calculator.ApplyCoupon(100.0, ""); }, "applycoupon empty code throws");
        });

    RunTest("applycoupon negative price throws",
        [&]()
        {
            AssertThrows([&]() { calculator.ApplyCoupon(-10.0, "save10"); }, "applycoupon negative price throws");
        });

    std::cout << "summary: passed=" << passed << " failed=" << failed << '\n';

    if (failed == 0)
    {
        std::cout << "all tests passed" << '\n';
        return 0;
    }

    std::cout << "some tests failed" << '\n';
    return 1;
}