#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class IParser 
{
    public:
        virtual bool parse(const std::string& data) = 0;
        virtual std::string getName() const = 0;
        virtual ~IParser() {}
};

class AbstractParser : public IParser 
{
    protected:
        int successCount = 0;
    public:
        int getSuccessCount() const 
        {
            return successCount;
        }
        virtual ~AbstractParser() {} 
};

class IntegerParser : public AbstractParser 
{
    public:
        bool parse(const std::string& data) override 
        {
            std::cout << "  -> IntegerParser trying '" << data << "': ";
        
            std::istringstream iss(data);
            int value;
            char check_char;

        if (iss >> value && !(iss >> check_char)) 
        {
            successCount++;
            std::cout << "SUCCESS! Value: " << value << std::endl;
            return true;
        } 
        else 
        {
            std::cout << "FAIL (not a clean integer)." << std::endl;
            return false;
        }
    }

    std::string getName() const override 
    {
        return "IntegerParser";
    }
};

class DoubleParser : public AbstractParser 
{
    public:
        bool parse(const std::string& data) override {
            std::cout << "  -> DoubleParser trying '" << data << "': ";
        
            std::istringstream iss(data);
            double value;
            char check_char;

            if (iss >> value && !(iss >> check_char)) {
                successCount++;
                std::cout << "SUCCESS! Value: " << value << std::endl;
                return true;
            } 
            else 
            {
                std::cout << "FAIL (not a clean double)." << std::endl;
                return false;
            }
        }

    std::string getName() const override 
    {
        return "DoubleParser";
    }
};

class ParserManager 
{
    private:
        std::vector<IParser*> parsers;

    public:
        ParserManager() = default;

        ~ParserManager() {}

        void addParser(IParser* parser) 
        {
            if (parser != nullptr) 
            {
                parsers.push_back(parser);
            }
        }

        void processData(const std::vector<std::string>& dataList) 
        {
            std::cout << "\n--- Starting Data Processing ---\n";
            for (const std::string& data : dataList) 
            {
                std::cout << "\nProcessing Input: '" << data << "'\n";
                for (IParser* parser : parsers) 
                {
                    parser->parse(data);
                }
            }
        }

    int getTotalSuccessCount() const 
    {
        int total = 0;
        std::cout << "\n--- Final Recognition Report ---\n";
        
        for (IParser* parser : parsers) 
        {
            AbstractParser* ap = static_cast<AbstractParser*>(parser);
            
            if (ap) 
            {
                int count = ap->getSuccessCount();
                std::cout << "-> " << parser->getName() << " successfully parsed: " << count << " numbers.\n";
                total += count;
            }
        }
        return total;
    }
};

int main() 
{
    IntegerParser* intParser = new IntegerParser();
    DoubleParser* doubleParser = new DoubleParser();
    
    ParserManager manager;

    manager.addParser(intParser);
    manager.addParser(doubleParser);

    std::vector<std::string> testData = {
        "42",
        "10.55",
        "hello",
        "123a",
        "89",
        "7.12345"
    };

    manager.processData(testData);

    int overallSuccess = manager.getTotalSuccessCount();
    std::cout << "\nOVERALL TOTAL SUCCESSFUL RECOGNITIONS: " << overallSuccess << std::endl;

    delete intParser;
    delete doubleParser;

    return 0;
}