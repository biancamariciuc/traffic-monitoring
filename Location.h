#pragma once

#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <string>

using namespace std;

class Location {
    int id;
    string city;
    string street;
public:
    Location(int id, string city, string street);

    Location(string city, string street);

    Location() {};

    void save(sqlite3 *db);
    bool validateData(sqlite3 *db, string city, string street);

    static void deleteById(sqlite3 *db, int id);

    static vector<Location> getAll(sqlite3 *db);

    friend std::ostream &operator<<(std::ostream &os, const Location &obj);

    void populateLocation(sqlite3 *db);
};