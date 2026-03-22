#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

class Printer
{
public:
    virtual ~Printer() = default;

    virtual void PrintColorImage(const std::string& imageName)
    {
        std::cout << "printing color image: " << imageName << std::endl;
    }
};

class BlackAndWhitePrinter : public Printer
{
public:
    void PrintColorImage(const std::string& imageName) override
    {
        throw std::logic_error("blackandwhiteprinter cannot print color image: " + imageName);
    }
};

void PrintMarketingBrochure(Printer& printer)
{
    std::cout << "client sends brochure to printer" << std::endl;
    printer.PrintColorImage("spring_sale_brochure.png");
    std::cout << "brochure printed successfully" << std::endl;
}

class ITextPrinter
{
public:
    virtual ~ITextPrinter() = default;
    virtual void PrintTextPage(const std::string& text) = 0;
};

class IColorPrinter
{
public:
    virtual ~IColorPrinter() = default;
    virtual void PrintColorImage(const std::string& imageName) = 0;
};

class OfficeColorPrinter : public ITextPrinter, public IColorPrinter
{
public:
    void PrintTextPage(const std::string& text) override
    {
        std::cout << "[officecolorprinter] text page: " << text << std::endl;
    }

    void PrintColorImage(const std::string& imageName) override
    {
        std::cout << "[officecolorprinter] color image: " << imageName << std::endl;
    }
};

class OfficeBlackAndWhitePrinter : public ITextPrinter
{
public:
    void PrintTextPage(const std::string& text) override
    {
        std::cout << "[officeblackandwhiteprinter] text page: " << text << std::endl;
    }
};

void PrintDailyReport(ITextPrinter& printer)
{
    printer.PrintTextPage("sales report for current day");
}

void PrintColorPoster(IColorPrinter& printer)
{
    printer.PrintColorImage("summer_event_poster.png");
}

int main()
{
    std::cout << "=== lsp violation ===" << std::endl;

    Printer regularPrinter;
    PrintMarketingBrochure(regularPrinter);

    BlackAndWhitePrinter blackAndWhitePrinter;

    try
    {
        PrintMarketingBrochure(blackAndWhitePrinter);
    }
    catch (const std::exception& exception)
    {
        std::cout << "lsp violation detected: " << exception.what() << std::endl;
    }

    std::cout << "\n=== lsp solution ===" << std::endl;

    OfficeColorPrinter colorPrinter;
    OfficeBlackAndWhitePrinter textOnlyPrinter;

    PrintDailyReport(colorPrinter);
    PrintDailyReport(textOnlyPrinter);
    PrintColorPoster(colorPrinter);

    std::cout << "all operations completed without broken contracts" << std::endl;
}