#pragma once

#include <iostream>
#include <vector>
#include <sqlite3.h>

using namespace std;

class Person {
    int id;
    string name;
public:
    Person(int id, string name);

    Person(string name);
    Person() {};


    void save(sqlite3 *db);
    bool searchByName(string name, sqlite3 *db);

    void populateDrivers(sqlite3 *db);

    static void deleteById(sqlite3 *db, int id);

    static vector<Person> getAll(sqlite3 *db);

    friend std::ostream &operator<<(std::ostream &os, const Person &obj);
};