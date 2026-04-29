#pragma once
#include "../core/CentralRegistry.h"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

/**
 * ConfigRepository - CSV Persistence for CentralRegistry
 */
class ConfigRepository
{
    string filePath;

public:
    explicit ConfigRepository(const string &path) : filePath(path) {}

    void load(CentralRegistry *registry) const
    {
        if (registry == nullptr)
            return;

        ifstream input(filePath);
        if (!input.is_open())
            return;

        string line;
        getline(input, line); // header
        while (getline(input, line))
        {
            if (line.empty())
                continue;

            stringstream ss(line);
            string key;
            string value;
            getline(ss, key, ',');
            getline(ss, value);
            if (!key.empty())
                registry->setConfig(key, value);
        }
    }

    void save(CentralRegistry *registry) const
    {
        if (registry == nullptr)
            return;

        ofstream output(filePath);
        output << "key,value\n";
        const map<string, string> &entries = registry->getAllConfig();
        for (const auto &entry : entries)
            output << entry.first << "," << entry.second << "\n";
    }
};
