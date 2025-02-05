#pragma once

#include <iostream>
#include <vector>
#include <sqlite3.h>

using namespace std;

class Fuel {
    int id;
    string city;
    string street;
    string fuel_info;
public:
    Fuel(int id, string city, string street, string fuel_info);

    Fuel(string city, string street, string fuel_info);

    Fuel() {};

    static vector<Fuel> getAll(sqlite3 *db);

    string getFuel(sqlite3 *db, string city, string street);
    bool equal(string str1, string str2);
    string getFuel();

    void save(sqlite3 *db);

    void populateFuel(sqlite3 *db);

    static void deleteById(sqlite3 *db, int id);

    friend std::ostream &operator<<(std::ostream &os, const Fuel &obj);
};