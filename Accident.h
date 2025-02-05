#pragma once

#include <iostream>
#include <vector>
#include <sqlite3.h>

using namespace std;

class Accident {
    int id;
    string city;
    string street;
    string accident_info;
public:
    Accident(int id, string city, string street, string accident_info);

    Accident(string city, string street, string accident_info);

    Accident() {};

    vector<Accident> getAll(sqlite3 *db);

    string getAccident(sqlite3 *db, string city, string street);

    void save(sqlite3 *db);
//    void searchByName(string name);

    void populateAccident(sqlite3 *db);

    static void deleteById(sqlite3 *db, int id);

    friend std::ostream &operator<<(std::ostream &os, const Accident &obj);
};