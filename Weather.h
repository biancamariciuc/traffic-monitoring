#pragma once

#include <iostream>
#include <vector>
#include <sqlite3.h>

using namespace std;

class Weather {
    int id;
    string city;
    string weather_info;
public:
    Weather(int id, string city, string weather_info);

    Weather(string city, string weather_info);

    Weather() {};

    string getWeather(sqlite3 *db, string city);
    bool equal(string str1);
    string getWeather();

    void save(sqlite3 *db);

    void populateWeather(sqlite3 *db);

    static void deleteById(sqlite3 *db, int id);

    static vector<Weather> getAll(sqlite3 *db);

    friend std::ostream &operator<<(std::ostream &os, const Weather &obj);
};