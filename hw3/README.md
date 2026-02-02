## принцип isp (interface segregation principle) означає, що клієнти не повинні залежати від методів, які вони не використовують. інтерфейс має бути вузьким і описувати лише ту поведінку, яка реально потрібна конкретному споживачу

### Тут порушено ISP:
```cpp
class UserService {
public:
    virtual void login(const std::string& login) = 0;
    virtual void registerUser(const std::string& login) = 0;
    virtual void sendEmail(const std::string& text) = 0;
    virtual void sendSms(const std::string& text) = 0;
    virtual void exportToPdf() = 0;
};

class SimpleAuth : public UserService {
public:
    void login(const std::string& login) override {
        std::cout << "login: " << login << std::endl;
    }

    void registerUser(const std::string& login) override {
        std::cout << "register: " << login << std::endl;
    }

    void sendEmail(const std::string&) override {}
    void sendSms(const std::string&) override {}
    void exportToPdf() override {}
};
```

### Тут правильно:
```cpp
class AuthService {
public:
    virtual void login(const std::string& login) = 0;
    virtual void registerUser(const std::string& login) = 0;
};

class NotificationService {
public:
    virtual void sendEmail(const std::string& text) = 0;
    virtual void sendSms(const std::string& text) = 0;
};

class ExportService {
public:
    virtual void exportToPdf() = 0;
};

class SimpleAuth : public AuthService {
public:
    void login(const std::string& login) override {
        std::cout << "login: " << login << std::endl;
    }

    void registerUser(const std::string& login) override {
        std::cout << "register: " << login << std::endl;
    }
};
```

## принцип dip говорить, що високорівневі модулі не повинні залежати від низькорівневих, обидва мають залежати від абстракцій. на практиці це означає, що замість створення об'єктів напряму через new, залежності передаються ззовні через конструктор або сеттер. завдяки цьому код стає гнучким, легко розширюється і не ламається при заміні реалізацій. застосування dip через dependency injection дозволяє легко міняти, наприклад, реальну базу даних на тестову або мок-об'єкт без зміни бізнес-логіки. це зменшує зв'язність між компонентами і робить систему простішою в підтримці

## вузькі інтерфейси за isp напряму покращують di та тестування, бо залежності стають мінімальними. клас залежить лише від невеликої абстракції, яку легко підмінити в тестах. у результаті тести стають простішими, швидшими і не потребують складної інфраструктури