// 5. User Authentication & Notification (UserAccountManager): 
// • Клас: UserAccountManager для реєстрації, входу, відправки email та SMS. 
// • Залежності: DatabaseConnection, SmtpClient, SmsGateway. 
// • Проблема: Змішування логіки та прямі залежності від служб відправки повідомлень.

#include <iostream>
#include <string>
#include <memory>

/*поганий ISP та DIP*/

class IUserService 
{
public:
    virtual ~IUserService() = default;
    
    virtual void saveUser(const std::string& login) = 0;
    virtual void deleteUser(const std::string& login) = 0;
    virtual void updateUser(const std::string& login) = 0;
    
    virtual void sendEmail(const std::string& to, const std::string& text) = 0;
    virtual void sendSms(const std::string& to, const std::string& text) = 0;
    virtual void pushToTelegram(const std::string& to, const std::string& text) = 0;
    
    virtual void banUser(const std::string& login) = 0;
    virtual void unbanUser(const std::string& login) = 0;
    
    virtual int getTotalUsers() = 0;
    virtual double getAverageAge() = 0;
    virtual void generateReport() = 0;
};

class UserServiceImpl : public IUserService 
{
private:
    class DatabaseConnection 
    {
    public:
        void saveUser(const std::string& login) 
        {
            std::cout << "saved user: " << login << std::endl;
        }
        void deleteUser(const std::string& login) 
        {
            std::cout << "deleted user: " << login << std::endl;
        }
        void updateUser(const std::string& login) 
        {
            std::cout << "updated user: " << login << std::endl;
        }
    };
    
    class NotificationService 
    {
    public:
        void sendEmail(const std::string& to, const std::string& text) 
        {
            std::cout << "email to " << to << ": " << text << std::endl;
        }
        void sendSms(const std::string& to, const std::string& text) 
        {
            std::cout << "sms to " << to << ": " << text << std::endl;
        }
        void pushToTelegram(const std::string& to, const std::string& text) 
        {
            std::cout << "telegram to " << to << ": " << text << std::endl;
        }
    };
    DatabaseConnection db;
    NotificationService notifier;
public:
    void saveUser(const std::string& login) override {db.saveUser(login);}
    void deleteUser(const std::string& login) override {db.deleteUser(login);}
    void updateUser(const std::string& login) override {db.updateUser(login);}
    void sendEmail(const std::string& to, const std::string& text) override {notifier.sendEmail(to, text);}
    void sendSms(const std::string& to, const std::string& text) override {notifier.sendSms(to, text);}
    void pushToTelegram(const std::string& to, const std::string& text) override {notifier.pushToTelegram(to, text);}
    void banUser(const std::string& login) override {std::cout << "banned user: " << login << std::endl;}
    void unbanUser(const std::string& login) override {std::cout << "unbanned user: " << login << std::endl;}
    int getTotalUsers() override {return 42;}
    double getAverageAge() override {return 25.7;}
    void generateReport() override {std::cout << "generating report..." << std::endl;}
};

class UserManager
{
private:
    UserServiceImpl service;

public:
    void registerUser(const std::string& login)
    {
        service.saveUser(login);
        service.sendEmail(login, "welcome");
        service.sendSms(login, "welcome");
        service.pushToTelegram(login, "welcome");
        
        int total = service.getTotalUsers();
        std::cout << "total users: " << total << std::endl;
    }
    
    void manageUser(const std::string& login)
    {
        service.saveUser(login);
        service.banUser(login);
        service.sendEmail("admin@gmail.com", "user " + login + " was banned");
        double avgAge = service.getAverageAge();
        std::cout << "average age: " << avgAge << std::endl;
        service.generateReport();
    }
};

class ServiceFactory
{
public:
    static UserServiceImpl* createUserService() {
        return new UserServiceImpl();
    }
};

/* ----- хороший ISP та DIP----- */

class IUserRepository
{
public:
    virtual void saveUser(const std::string& login) = 0;
    virtual bool userExists(const std::string& login) = 0;
    virtual ~IUserRepository() = default;
};

class IEmailSender
{
public:
    virtual void sendEmail(const std::string& to, const std::string& text) = 0;
    virtual ~IEmailSender() = default;
};

class ISmsSender
{
public:
    virtual void sendSms(const std::string& to, const std::string& text) = 0;
    virtual ~ISmsSender() = default;
};

class mySqlUserRepository : public IUserRepository
{
public:
    void saveUser(const std::string& login) override
    {
        std::cout << "mysql saved user: " << login << std::endl;
    }

    bool userExists(const std::string& login) override
    {
        return false;
    }
};

class smtpEmailSender : public IEmailSender
{
public:
    void sendEmail(const std::string& to, const std::string& text) override
    {
        std::cout << "smtp email to " << to << ": " << text << std::endl;
    }
};

class twilioSmsSender : public ISmsSender
{
public:
    void sendSms(const std::string& to, const std::string& text) override
    {
        std::cout << "twilio sms to " << to << ": " << text << std::endl;
    }
};

class userAccountManager
{
    IUserRepository* repo;
    IEmailSender* email;
    ISmsSender* sms;

public:
    userAccountManager
    (
        IUserRepository* repo,
        IEmailSender* email,
        ISmsSender* sms
    ) : repo(repo), email(email), sms(sms) {}

    void registerUser(const std::string& login)
    {
        if (!repo->userExists(login))
        {
            repo->saveUser(login);
            email->sendEmail(login, "welcome");
        }
    }

    void loginUser(const std::string& login)
    {
        if (repo->userExists(login))
        {
            sms->sendSms(login, "new login detected");
        }
    }
};

int main()
{
    mySqlUserRepository repo;
    smtpEmailSender email;
    twilioSmsSender sms;

    userAccountManager good(&repo, &email, &sms);
    good.registerUser("test@gmail.com");
    good.loginUser("test@gmail.com");
}