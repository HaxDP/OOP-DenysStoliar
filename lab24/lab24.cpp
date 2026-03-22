#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class INumericOperationStrategy
{
public:
    virtual ~INumericOperationStrategy() = default;
    virtual double Execute(double value) const = 0;
    virtual std::string GetOperationName() const = 0;
};

class SquareOperationStrategy : public INumericOperationStrategy
{
public:
    double Execute(double value) const override
    {
        return value * value;
    }

    std::string GetOperationName() const override
    {
        return "square";
    }
};

class CubeOperationStrategy : public INumericOperationStrategy
{
public:
    double Execute(double value) const override
    {
        return value * value * value;
    }

    std::string GetOperationName() const override
    {
        return "cube";
    }
};

class SquareRootOperationStrategy : public INumericOperationStrategy
{
public:
    double Execute(double value) const override
    {
        if (value < 0.0)
        {
            throw std::invalid_argument("square root strategy expects non-negative input");
        }

        return std::sqrt(value);
    }

    std::string GetOperationName() const override
    {
        return "squareroot";
    }
};

class NumericProcessor
{
private:
    const INumericOperationStrategy* currentStrategy;

public:
    explicit NumericProcessor(const INumericOperationStrategy* strategy)
        : currentStrategy(strategy)
    {
    }

    void SetStrategy(const INumericOperationStrategy* strategy)
    {
        currentStrategy = strategy;
    }

    double Process(double input) const
    {
        return currentStrategy->Execute(input);
    }

    std::string GetCurrentOperationName() const
    {
        return currentStrategy->GetOperationName();
    }
};

class ResultPublisher
{
public:
    using ResultCalculatedHandler = std::function<void(double, const std::string&)>;

private:
    std::vector<ResultCalculatedHandler> subscribers;

public:
    void Subscribe(const ResultCalculatedHandler& handler)
    {
        subscribers.push_back(handler);
    }

    void PublishResult(double result, const std::string& operationName) const
    {
        for (const auto& subscriber : subscribers)
        {
            subscriber(result, operationName);
        }
    }
};

class ConsoleLoggerObserver
{
public:
    void OnResultCalculated(double result, const std::string& operationName) const
    {
        std::cout << "[consoleloggerobserver] operation=" << operationName
                  << ", result=" << result << std::endl;
    }
};

class HistoryLoggerObserver
{
private:
    std::vector<std::string> history;

public:
    void OnResultCalculated(double result, const std::string& operationName)
    {
        std::ostringstream entry;
        entry << operationName << " -> " << result;
        history.push_back(entry.str());
    }

    void PrintHistory() const
    {
        std::cout << "\n[historyloggerobserver] stored history:" << std::endl;

        for (const std::string& entry : history)
        {
            std::cout << "- " << entry << std::endl;
        }
    }
};

class ThresholdNotifierObserver
{
private:
    double threshold;

public:
    explicit ThresholdNotifierObserver(double thresholdValue)
        : threshold(thresholdValue)
    {
    }

    void OnResultCalculated(double result, const std::string& operationName) const
    {
        if (result > threshold)
        {
            std::cout << "[thresholdnotifierobserver] " << operationName
                      << " result " << result
                      << " is above threshold " << threshold << std::endl;
        }
    }
};

void ProcessAndPublishResult(
    NumericProcessor& processor,
    const ResultPublisher& publisher,
    double value)
{
    const double result = processor.Process(value);
    const std::string operationName = processor.GetCurrentOperationName();

    publisher.PublishResult(result, operationName);
}

int main()
{
    SquareOperationStrategy squareStrategy;
    CubeOperationStrategy cubeStrategy;
    SquareRootOperationStrategy squareRootStrategy;

    NumericProcessor processor(&squareStrategy);
    ResultPublisher publisher;

    ConsoleLoggerObserver consoleLogger;
    HistoryLoggerObserver historyLogger;
    ThresholdNotifierObserver thresholdNotifier(50.0);

    publisher.Subscribe(
        [&consoleLogger](double result, const std::string& operationName)
        {
            consoleLogger.OnResultCalculated(result, operationName);
        });

    publisher.Subscribe(
        [&historyLogger](double result, const std::string& operationName)
        {
            historyLogger.OnResultCalculated(result, operationName);
        });

    publisher.Subscribe(
        [&thresholdNotifier](double result, const std::string& operationName)
        {
            thresholdNotifier.OnResultCalculated(result, operationName);
        });

    std::cout << "=== strategy + observer demo ===" << std::endl;

    processor.SetStrategy(&squareStrategy);
    ProcessAndPublishResult(processor, publisher, 4.0);
    ProcessAndPublishResult(processor, publisher, 8.0);

    processor.SetStrategy(&cubeStrategy);
    ProcessAndPublishResult(processor, publisher, 3.0);
    ProcessAndPublishResult(processor, publisher, 5.0);

    processor.SetStrategy(&squareRootStrategy);
    ProcessAndPublishResult(processor, publisher, 81.0);
    ProcessAndPublishResult(processor, publisher, 144.0);

    historyLogger.PrintHistory();
}