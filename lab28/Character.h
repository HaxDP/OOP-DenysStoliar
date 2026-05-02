#pragma once

#include "Item.h"
#include <vector>
#include <memory>

class Character
{
private:
    int id;
    std::string name;
    int level;
    std::vector<Item> inventory;

public:
    Character() : id(0), name(""), level(1) {}

    Character(int id, const std::string& name, int level)
        : id(id), name(name), level(level)
    {
    }

    int GetId() const { return id; }
    std::string GetName() const { return name; }
    int GetLevel() const { return level; }
    const std::vector<Item>& GetInventory() const { return inventory; }

    void SetId(int newId) { id = newId; }
    void SetName(const std::string& newName) { name = newName; }
    void SetLevel(int newLevel) { level = newLevel; }

    void AddItem(const Item& item)
    {
        inventory.push_back(item);
    }

    void ClearInventory()
    {
        inventory.clear();
    }

    void SetInventory(const std::vector<Item>& newInventory)
    {
        inventory = newInventory;
    }

    virtual ~Character() = default;
};