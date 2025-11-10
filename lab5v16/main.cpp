#include <iostream>
#include <vector> // для темплейтів
#include <string>
#include <numeric> 
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <windows.h>
#include <clocale>
#include <limits>

/**
 * @brief власний виняток
 * використовується, коли передано некоректні параметри для аналізу
 */
class InvalidConfigurationException : public std::runtime_error
{
public:
    // конструктор, що приймає повідомлення про помилку
    InvalidConfigurationException(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @brief власний виняток.
 * використовується, коли сенсор за іменем не знайдено у хабі
 */
class SensorNotFoundException : public std::runtime_error
{
public:
    SensorNotFoundException(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @brief узагальнений клас Sensor
 */
template <typename T>
class Sensor
{
private:
    std::string name;
    std::vector<T> readings; // колекція показників

public:
    Sensor(const std::string &n) : name(n) {}

    void addReading(T value)
    {
        readings.push_back(value);
    }

    const std::string &getName() const
    {
        return name;
    }

    const std::vector<T> &getReadings() const
    {
        return readings;
    }

    /**
     * @brief отримує мінімальне значення
     */
    T getMin() const
    {
        if (readings.empty())
        {
            return T();
        }
        // std::min_element - це аналог LINQ .Min()
        return *std::min_element(readings.begin(), readings.end());
    }

    /**
     * @brief отримує максимальне значення
     */
    T getMax() const
    {
        if (readings.empty())
        {
            return T();
        }
        // std::max_element - це аналог LINQ .Max()
        return *std::max_element(readings.begin(), readings.end());
    }

    /**
     * @brief розрахунок ковзного середнього з вікном k
     */
    std::vector<double> getSlidingAverage(int k) const
    {
        if (k <= 0)
        {
            throw InvalidConfigurationException("розмір вікна 'k' має бути > 0.");
        }

        std::vector<double> averages;
        if (k > readings.size())
        {
            return averages; // повертаємо порожній вектор, якщо даних замало
        }

        // ідемо по вектору, доки в нас є повне "вікно" розміром k
        for (size_t i = 0; i <= readings.size() - k; ++i)
        {
            /*
            використовуємо std::accumulate (аналог LINQ .Sum()) для суми поточного вікна
            початковий ітератор: readings.begin() + i
            кінцевий ітератор: readings.begin() + i + k
            */
            double sum = std::accumulate(readings.begin() + i, readings.begin() + i + k, 0.0);
            averages.push_back(sum / k);
        }
        return averages;
    }

    /**
     * @brief виявлення значень, вищих за поріг
     */
    std::vector<T> detectSpikes(T threshold) const
    {
        if (threshold <= 0)
        {
            throw InvalidConfigurationException("поріг 'threshold' має бути > 0.");
        }

        std::vector<T> spikes;
        /*
        std::copy_if - це аналог LINQ .Where()
        ми копіюємо елементи з 'readings' в 'spikes'
        якщо вони задовольняють умову
        */
        std::copy_if(readings.begin(), readings.end(),
        std::back_inserter(spikes),
            [threshold](T value)
            {
                return value > threshold;
            });
        return spikes;
    }
};

/**
 * @brief клас SensorHub.
 */
class SensorHub
{
private:
    std::vector<Sensor<double>> sensors;

public:
    void addSensor(const Sensor<double> &sensor)
    {
        sensors.push_back(sensor);
    }

    /**
     * @brief знаходить сенсор за іменем.
     * кидає виняток SensorNotFoundException, якщо не знайдено.
     */
    Sensor<double> &getSensorByName(const std::string &name)
    {
        // std::find_if - аналог LINQ .FirstOrDefault()
        auto it = std::find_if(sensors.begin(), sensors.end(),
            [&name](const Sensor<double> &s)
            {
                return s.getName() == name;
            });

        if (it != sensors.end())
        {
            return *it;
        }
        else
        {
            throw SensorNotFoundException("сенсор не знайдено: " + name);
        }
    }
};

// допоміжна функція для друку векторів
template <typename T>
void printVector(const std::string &title, const std::vector<T> &vec)
{
    std::cout << title << " [ ";
    for (const T &val : vec)
    {
        std::cout << val << " ";
    }
    std::cout << "]" << std::endl;
}

// допоміжна функція для очищення буфера вводу
void clearInputBuffer()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main()
{
    // налаштування кодування
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "C.UTF-8");

    SensorHub hub;
    bool running = true;

    std::cout << "Система Моніторингу Сенсорів" << std::endl; // СМС хехе, звучить прикольно

    // менюшка
    while (running)
    {
        std::cout << "\nОберіть дію:\n";
        std::cout << "1.Створити новий сенсор" << std::endl;
        std::cout << "2.Додати показники до сенсора" << std::endl;
        std::cout << "3.Провести аналіз сенсора" << std::endl;
        std::cout << "4.Вийти" << std::endl;
        std::cout << "Ваш вибір: ";

        int choice;
        std::cin >> choice;

        // перевірка, чи користувач ввів число
        if (std::cin.fail())
        {
            std::cin.clear();
            clearInputBuffer(); // очищуємо буфер
            std::cerr << "[ПОМИЛКА] Будь ласка, введіть число (1-4).\n";
            continue;
        }

        // очищуємо буфер вводу після зчитування числа
        clearInputBuffer(); 

        std::string name; // змінна для імені сенсора

        switch (choice)
        {
        case 1:
        {
            std::cout << "Введіть назву для нового сенсора: ";
            // std::ws - прибирає пробіли/переноси рядків перед зчитуванням
            std::getline(std::cin >> std::ws, name); 
            
            Sensor<double> newSensor(name);
            hub.addSensor(newSensor);
            std::cout << "Сенсор '" << name << "' успішно додано до хабу.\n";
            break;
        }

        case 2:
        {
            std::cout << "Введіть назву сенсора, до якого додати дані: ";
            std::getline(std::cin >> std::ws, name);

            try
            {
                Sensor<double> &sensor = hub.getSensorByName(name);

                std::cout << "Вводьте показники. Введіть 'q' або будь-яку літеру для завершення:\n";
                double value;
                while (std::cin >> value) 
                {
                    sensor.addReading(value);
                    std::cout << "Додано: " << value << std::endl;
                }
                std::cout << "Введення даних завершено.\n";
                std::cin.clear();
                clearInputBuffer();
            }
            catch (const SensorNotFoundException &e)
            {
                std::cerr << "[ПОМИЛКА ПОШУКУ] " << e.what() << std::endl;
            }
            break;
        }

        case 3:
        {
            std::cout << "Введіть назву сенсора для аналізу: ";
            std::getline(std::cin >> std::ws, name);

            try
            {
                Sensor<double> &sensor = hub.getSensorByName(name);

                // 1. мін/макс
                std::cout << "--- Аналіз для '" << name << "' ---\n";
                std::cout << "Мін. температура: " << sensor.getMin() << std::endl;
                std::cout << "Макс. температура: " << sensor.getMax() << std::endl;

                // 2. ковзне середнє
                std::cout << "\nВведіть розмір вікна 'k' для ковзного середнього: ";
                int k;
                std::cin >> k;
                if (std::cin.fail()) throw std::runtime_error("Некоректне введення для 'k'.");
                clearInputBuffer(); // чисть чисть

                std::vector<double> averages = sensor.getSlidingAverage(k); // можливо кине InvalidConfigurationException, але це не точно
                printVector("Ковзне середнє:", averages);

                // 3. сплески
                std::cout << "\nВведіть поріг 'threshold' для виявлення сплесків: ";
                double threshold;
                std::cin >> threshold;
                if (std::cin.fail()) throw std::runtime_error("Некоректне введення для 'threshold'.");
                clearInputBuffer(); // ще чисть чисть

                std::vector<double> spikes = sensor.detectSpikes(threshold); // може послати і кинути InvalidConfigurationException
                printVector("Сплески (вище порогу):", spikes);
            }
            catch (const SensorNotFoundException &e)
            {
                std::cerr << "[ПОМИЛКА ПОШУКУ] " << e.what() << std::endl;
                clearInputBuffer();
            }
            catch (const InvalidConfigurationException &e)
            {
                std::cerr << "[ПОМИЛКА КОНФІГУРАЦІЇ] " << e.what() << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cerr << "[ЗАГАЛЬНА ПОМИЛКА] " << e.what() << std::endl;
                std::cin.clear();
                clearInputBuffer();
            }
            break;
        }
        case 4:
        {
            running = false;
            std::cout << "Завершення роботи. гуд бай!" << std::endl;
            break;
        }
        default:
        {
            std::cerr << "[ПОМИЛКА] Невірний вибір. Спробуйте ще раз (1-4).\n";
            break;
        }
        }
    }
}