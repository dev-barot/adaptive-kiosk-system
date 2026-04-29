#pragma once
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
using namespace std;

/**
 * TransactionRepository - CSV Audit Log
 */
class TransactionRepository
{
    string filePath;
    inline static mutex fileMutex;

    static string escapeCsv(const string &value)
    {
        string escaped = value;
        for (char &ch : escaped)
        {
            if (ch == ',')
                ch = ';';
            if (ch == '\n' || ch == '\r')
                ch = ' ';
        }
        return escaped;
    }

public:
    explicit TransactionRepository(const string &path) : filePath(path) {}

    void initialize() const
    {
        ifstream input(filePath);
        if (input.good())
            return;

        ofstream output(filePath);
        output << "timestamp,transaction_id,operation,status,item_id,quantity,amount,payment_method,phase,reason,details\n";
    }

    void append(const string &operation,
                const string &status,
                const string &details) const
    {
        appendAudit("N/A", operation, status, "", 0, 0.0f, "", "", "", details);
    }

    void appendAudit(const string &transactionId,
                     const string &operation,
                     const string &status,
                     const string &itemId,
                     int qty,
                     float amount,
                     const string &paymentMethod,
                     const string &phase,
                     const string &reason,
                     const string &details) const
    {
        initialize();
        lock_guard<mutex> lock(fileMutex);
        ofstream output(filePath, ios::app);

        const auto now = chrono::system_clock::now();
        const time_t raw = chrono::system_clock::to_time_t(now);
        tm localTime {};
#ifdef _MSC_VER
        localtime_s(&localTime, &raw);
#elif defined(_WIN32)
        tm* timeinfo = localtime(&raw);
        if (timeinfo) localTime = *timeinfo;
#else
        localtime_r(&raw, &localTime);
#endif

        ostringstream stamp;
        stamp << put_time(&localTime, "%Y-%m-%d %H:%M:%S");

        output << stamp.str() << ","
               << escapeCsv(transactionId) << ","
               << escapeCsv(operation) << ","
               << escapeCsv(status) << ","
               << escapeCsv(itemId) << ","
               << qty << ","
               << fixed << setprecision(2) << amount << ","
               << escapeCsv(paymentMethod) << ","
               << escapeCsv(phase) << ","
               << escapeCsv(reason) << ","
               << escapeCsv(details) << "\n";
    }

    vector<string> loadAll() const
    {
        vector<string> rows;
        ifstream input(filePath);
        if (!input.is_open())
            return rows;

        string line;
        getline(input, line); // header
        while (getline(input, line))
        {
            if (!line.empty())
                rows.push_back(line);
        }
        return rows;
    }
};
