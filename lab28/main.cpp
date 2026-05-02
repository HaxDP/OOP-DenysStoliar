#include <iostream>
#include "CharacterRepository.h"

void PrintCharacter(const Character& character)
{
    std::cout << "  ID: " << character.GetId()
              << " | Name: " << character.GetName()
              << " | Level: " << character.GetLevel()
              << " | Items: " << character.GetInventory().size() << std::endl;

    for (const auto& item : character.GetInventory())
    {
        std::cout << "    - " << item.GetName()
                  << " (" << item.GetType() << ", Power: " << item.GetPower() << ")" << std::endl;
    }
}

int main()
{
    CharacterRepository repo;

    std::cout << "Game Character Repository Demo" << std::endl << std::endl;

    std::cout << "1. Creating characters with items..." << std::endl;
    Character archer(1, "Archer", 5);
    archer.AddItem(Item(101, "Bow", "weapon", 15));
    archer.AddItem(Item(102, "Arrow Pack", "ammo", 5));

    Character warrior(2, "Warrior", 8);
    warrior.AddItem(Item(103, "Sword", "weapon", 25));
    warrior.AddItem(Item(104, "Shield", "armor", 20));
    warrior.AddItem(Item(105, "Health Potion", "consumable", 0));

    Character mage(3, "Mage", 6);
    mage.AddItem(Item(106, "Staff", "weapon", 18));
    mage.AddItem(Item(107, "Mana Potion", "consumable", 0));

    repo.Add(archer);
    repo.Add(warrior);
    repo.Add(mage);

    std::cout << "Added 3 characters to repository." << std::endl << std::endl;

    std::cout << "2. Characters before saving:" << std::endl;
    for (const auto& character : repo.GetAll())
    {
        PrintCharacter(character);
    }
    std::cout << std::endl;

    std::cout << "3. Saving to JSON file..." << std::endl;
    if (!repo.SaveToFile("characters.json"))
    {
        std::cerr << "Failed to save characters." << std::endl;
        return 1;
    }
    std::cout << std::endl;

    std::cout << "4. Clearing repository..." << std::endl;
    repo.Clear();
    std::cout << "Repository now contains " << repo.GetAll().size() << " characters." << std::endl << std::endl;

    std::cout << "5. Loading from JSON file..." << std::endl;
    if (!repo.LoadFromFile("characters.json"))
    {
        std::cerr << "Failed to load characters." << std::endl;
        return 1;
    }
    std::cout << std::endl;

    std::cout << "6. Characters after loading:" << std::endl;
    for (const auto& character : repo.GetAll())
    {
        PrintCharacter(character);
    }
    std::cout << std::endl;

    std::cout << "7. Finding character by ID (2)..." << std::endl;
    Character* foundChar = repo.GetById(2);
    if (foundChar)
    {
        std::cout << "Found: " << foundChar->GetName() << std::endl;
    }
    else
    {
        std::cout << "Character not found." << std::endl;
    }

    std::cout << "\nDemo Complete" << std::endl;
    return 0;
}