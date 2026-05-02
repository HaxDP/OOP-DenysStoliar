#pragma once

#include <string>

class Item
{
private:
    int id;
    std::string name;
    std::string type;
    int power;

public:
    Item() : id(0), name(""), type(""), power(0) {}

    Item(int id, const std::string& name, const std::string& type, int power)
        : id(id), name(name), type(type), power(power)
    {
    }

    int GetId() const { return id; }
    std::string GetName() const { return name; }
    std::string GetType() const { return type; }
    int GetPower() const { return power; }

    void SetId(int newId) { id = newId; }
    void SetName(const std::string& newName) { name = newName; }
    void SetType(const std::string& newType) { type = newType; }
    void SetPower(int newPower) { power = newPower; }

    virtual ~Item() = default;
};