#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ILogger
{
public:
    virtual ~ILogger() = default;
    virtual void Log(const std::string& message) = 0;
};

class ConsoleLogger : public ILogger
{
public:
    void Log(const std::string& message) override
    {
        std::cout << "[consolelogger] " << message << std::endl;
    }
};

class FileLogger : public ILogger
{
private:
    std::string filePath;

public:
    explicit FileLogger(const std::string& path)
        : filePath(path)
    {
    }

    void Log(const std::string& message) override
    {
        std::ofstream output(filePath, std::ios::app);

        if (output.is_open())
        {
            output << "[filelogger] " << message << std::endl;
        }
    }
};

class LoggerFactory
{
public:
    virtual ~LoggerFactory() = default;
    virtual std::unique_ptr<ILogger> CreateLogger() const = 0;
};

class ConsoleLoggerFactory : public LoggerFactory
{
public:
    std::unique_ptr<ILogger> CreateLogger() const override
    {
        return std::make_unique<ConsoleLogger>();
    }
};

class FileLoggerFactory : public LoggerFactory
{
public:
    std::unique_ptr<ILogger> CreateLogger() const override
    {
        return std::make_unique<FileLogger>("lab25_log.txt");
    }
};

class LoggerManager
{
private:
    static LoggerManager* instance;
    std::unique_ptr<LoggerFactory> factory;

    LoggerManager()
        : factory(std::make_unique<ConsoleLoggerFactory>())
    {
    }

public:
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;

    static LoggerManager& GetInstance()
    {
        if (instance == nullptr)
        {
            instance = new LoggerManager();
        }

        return *instance;
    }

    void SetFactory(std::unique_ptr<LoggerFactory> newFactory)
    {
        factory = std::move(newFactory);
    }

    void Log(const std::string& message)
    {
        std::unique_ptr<ILogger> logger = factory->CreateLogger();
        logger->Log(message);
    }
};

LoggerManager* LoggerManager::instance = nullptr;

class IDataProcessorStrategy
{
public:
    virtual ~IDataProcessorStrategy() = default;
    virtual std::string Process(const std::string& input) const = 0;
    virtual std::string GetName() const = 0;
};

class EncryptDataStrategy : public IDataProcessorStrategy
{
public:
    std::string Process(const std::string& input) const override
    {
        std::string result = input;

        for (char& symbol : result)
        {
            symbol = static_cast<char>(symbol + 1);
        }

        return result;
    }

    std::string GetName() const override
    {
        return "encryptdatastrategy";
    }
};

class CompressDataStrategy : public IDataProcessorStrategy
{
public:
    std::string Process(const std::string& input) const override
    {
        std::string result;

        for (char symbol : input)
        {
            if (result.empty() || result.back() != symbol)
            {
                result.push_back(symbol);
            }
        }

        return result;
    }

    std::string GetName() const override
    {
        return "compressdatastrategy";
    }
};

class DataContext
{
private:
    const IDataProcessorStrategy* strategy;

public:
    explicit DataContext(const IDataProcessorStrategy* initialStrategy)
        : strategy(initialStrategy)
    {
    }

    void SetStrategy(const IDataProcessorStrategy* newStrategy)
    {
        strategy = newStrategy;
    }

    std::string ProcessData(const std::string& data) const
    {
        return strategy->Process(data);
    }

    std::string GetStrategyName() const
    {
        return strategy->GetName();
    }
};

class DataPublisher
{
public:
    using DataProcessedHandler = std::function<void(const std::string&, const std::string&)>;

private:
    std::vector<DataProcessedHandler> subscribers;

public:
    void Subscribe(const DataProcessedHandler& handler)
    {
        subscribers.push_back(handler);
    }

    void PublishDataProcessed(const std::string& processedData, const std::string& strategyName) const
    {
        for (const auto& subscriber : subscribers)
        {
            subscriber(processedData, strategyName);
        }
    }
};

class ProcessingLoggerObserver
{
public:
    void OnDataProcessed(const std::string& processedData, const std::string& strategyName) const
    {
        LoggerManager::GetInstance().Log(
            "observer received processed data, strategy=" + strategyName + ", data=" + processedData);
    }
};

void RunProcessingScenario(
    const std::string& scenarioTitle,
    DataContext& context,
    const DataPublisher& publisher,
    const std::string& input)
{
    std::cout << "\n========== " << scenarioTitle << " ==========" << std::endl;

    const std::string processed = context.ProcessData(input);
    LoggerManager::GetInstance().Log("data processed in context, input=" + input + ", output=" + processed);
    publisher.PublishDataProcessed(processed, context.GetStrategyName());
}

int main()
{
    EncryptDataStrategy encryptStrategy;
    CompressDataStrategy compressStrategy;

    DataContext context(&encryptStrategy);
    DataPublisher publisher;
    ProcessingLoggerObserver observer;

    publisher.Subscribe(
        [&observer](const std::string& processedData, const std::string& strategyName)
        {
            observer.OnDataProcessed(processedData, strategyName);
        });

    LoggerManager::GetInstance().SetFactory(std::make_unique<ConsoleLoggerFactory>());
    RunProcessingScenario("scenario 1: full integration", context, publisher, "hellooo_worlld");

    RunProcessingScenario("scenario 2: dynamic logger change - before switch", context, publisher, "datta_111");
    LoggerManager::GetInstance().SetFactory(std::make_unique<FileLoggerFactory>());
    RunProcessingScenario("scenario 2: dynamic logger change - after switch", context, publisher, "datta_222");

    LoggerManager::GetInstance().SetFactory(std::make_unique<ConsoleLoggerFactory>());
    RunProcessingScenario("scenario 3: dynamic strategy change - before switch", context, publisher, "aaabbbcccc");
    context.SetStrategy(&compressStrategy);
    RunProcessingScenario("scenario 3: dynamic strategy change - after switch", context, publisher, "aaabbbcccc");

    std::cout << "\ndone, check console and lab25_log.txt for logger behavior changes" << std::endl;
}