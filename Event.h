#pragma once

#include <iostream>
#include <vector>
#include <sqlite3.h>

using namespace std;

class Event {
    int id;
    string city;
    string street;
    string event_info;
public:
    Event(int id, string city, string street, string event_info);

    Event(string city, string street, string event_info);

    Event() {};

    static vector<Event> getAll(sqlite3 *db);

    string getEvent(sqlite3 *db, string city, string street);
    bool equal(string str1, string str2);
    string getEvent();

    void save(sqlite3 *db);
//    void searchByName(string name);

    void populateEvent(sqlite3 *db);

    static void deleteById(sqlite3 *db, int id);

    friend std::ostream &operator<<(std::ostream &os, const Event &obj);
};