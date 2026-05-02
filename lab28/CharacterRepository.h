#pragma once

#include "Character.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

class CharacterRepository
{
private:
    std::vector<Character> characters;

    std::string ItemToJson(const Item& item) const
    {
        std::ostringstream oss;
        oss << "{\"id\":" << item.GetId()
            << ",\"name\":\"" << item.GetName()
            << "\",\"type\":\"" << item.GetType()
            << "\",\"power\":" << item.GetPower() << "}";
        return oss.str();
    }

    Item JsonToItem(const std::string& json) const
    {
        Item item;
        int id = 0, power = 0;
        std::string name = "", type = "";

        size_t idPos = json.find("\"id\":");
        if (idPos != std::string::npos)
        {
            id = std::stoi(json.substr(idPos + 5));
        }

        size_t nameStart = json.find("\"name\":\"");
        if (nameStart != std::string::npos)
        {
            nameStart += 8;
            size_t nameEnd = json.find("\"", nameStart);
            name = json.substr(nameStart, nameEnd - nameStart);
        }

        size_t typeStart = json.find("\"type\":\"");
        if (typeStart != std::string::npos)
        {
            typeStart += 8;
            size_t typeEnd = json.find("\"", typeStart);
            type = json.substr(typeStart, typeEnd - typeStart);
        }

        size_t powerPos = json.find("\"power\":");
        if (powerPos != std::string::npos)
        {
            power = std::stoi(json.substr(powerPos + 8));
        }

        item.SetId(id);
        item.SetName(name);
        item.SetType(type);
        item.SetPower(power);
        return item;
    }

    std::string CharacterToJson(const Character& character) const
    {
        std::ostringstream oss;
        oss << "{\"id\":" << character.GetId()
            << ",\"name\":\"" << character.GetName()
            << "\",\"level\":" << character.GetLevel()
            << ",\"inventory\":[";

        const auto& inventory = character.GetInventory();
        for (size_t i = 0; i < inventory.size(); ++i)
        {
            oss << ItemToJson(inventory[i]);
            if (i < inventory.size() - 1) oss << ",";
        }

        oss << "]}";
        return oss.str();
    }

    Character JsonToCharacter(const std::string& json) const
    {
        Character character;
        int id = 0, level = 1;
        std::string name = "";

        size_t idPos = json.find("\"id\":");
        if (idPos != std::string::npos)
        {
            id = std::stoi(json.substr(idPos + 5));
        }

        size_t nameStart = json.find("\"name\":\"");
        if (nameStart != std::string::npos)
        {
            nameStart += 8;
            size_t nameEnd = json.find("\"", nameStart);
            name = json.substr(nameStart, nameEnd - nameStart);
        }

        size_t levelPos = json.find("\"level\":");
        if (levelPos != std::string::npos)
        {
            level = std::stoi(json.substr(levelPos + 8));
        }

        character.SetId(id);
        character.SetName(name);
        character.SetLevel(level);

        size_t invStart = json.find("\"inventory\":[");
        if (invStart != std::string::npos)
        {
            invStart += 13;
            size_t invEnd = json.find("]", invStart);
            std::string invJson = json.substr(invStart, invEnd - invStart);

            size_t pos = 0;
            while (pos < invJson.length())
            {
                size_t itemStart = invJson.find("{", pos);
                if (itemStart == std::string::npos) break;

                size_t itemEnd = invJson.find("}", itemStart);
                if (itemEnd == std::string::npos) break;

                std::string itemStr = invJson.substr(itemStart, itemEnd - itemStart + 1);
                character.AddItem(JsonToItem(itemStr));
                pos = itemEnd + 1;
            }
        }

        return character;
    }

public:
    void Add(const Character& character)
    {
        characters.push_back(character);
    }

    const std::vector<Character>& GetAll() const
    {
        return characters;
    }

    Character* GetById(int id)
    {
        auto it = std::find_if(characters.begin(), characters.end(),
                               [id](const Character& c) { return c.GetId() == id; });

        if (it != characters.end())
            return &(*it);

        return nullptr;
    }

    void Clear()
    {
        characters.clear();
    }

    bool SaveToFile(const std::string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
            return false;
        }

        file << "[";
        for (size_t i = 0; i < characters.size(); ++i)
        {
            file << CharacterToJson(characters[i]);
            if (i < characters.size() - 1) file << ",";
        }
        file << "]";

        file.close();
        std::cout << "Characters saved to " << filename << std::endl;
        return true;
    }

    bool LoadFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open file " << filename << " for reading." << std::endl;
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        characters.clear();

        size_t pos = 0;
        while (pos < content.length())
        {
            size_t charStart = content.find("{", pos);
            if (charStart == std::string::npos) break;

            size_t braceCount = 1;
            size_t charEnd = charStart + 1;
            while (charEnd < content.length() && braceCount > 0)
            {
                if (content[charEnd] == '{') braceCount++;
                if (content[charEnd] == '}') braceCount--;
                charEnd++;
            }

            std::string charStr = content.substr(charStart, charEnd - charStart);
            characters.push_back(JsonToCharacter(charStr));
            pos = charEnd;
        }

        std::cout << "Loaded " << characters.size() << " characters from " << filename << std::endl;
        return true;
    }

    virtual ~CharacterRepository() = default;
};