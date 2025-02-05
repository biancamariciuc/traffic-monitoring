#include "Weather.h"

Weather ::  Weather(int id, string city, string weather_info) {
    this->id = id;
    this->city = city;
    this->weather_info = weather_info;
}

Weather::Weather(string city, string weather_info) {
    this->city = city;
    this->weather_info = weather_info;
}

vector<Weather> Weather ::  getAll(sqlite3 *db) {
    vector<Weather> r;

    const char *select_sql = "SELECT * FROM weather;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string city(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string event(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));


            Weather w(id, city, event);
            r.push_back(w);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to query the database" << std::endl;
    }
    return r;
}

void Weather::save(sqlite3 *db) {
    auto insert = "INSERT INTO weather (city, event) VALUES ('" + this->city + "', '" + this->weather_info + "');";
    auto r  = sqlite3_exec(db, insert.c_str(), 0, 0, NULL);

    if (r != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    }
}


void Weather::deleteById(sqlite3 *db, int id) {
    string delete_sql = "DELETE FROM weather WHERE ID = " + std::to_string(id) + ";";

    int result = sqlite3_exec(db, delete_sql.c_str(), nullptr, nullptr, NULL);

    if (result != SQLITE_OK) {
        std::cerr << "Error deleting row" << std::endl;
    }
}

std::ostream &operator<<(ostream &os, const Weather &obj) {
    os << "id: " << obj.id << ", city: " << obj.city << ", event: " << obj.weather_info;
    return os;
}

void Weather::populateWeather(sqlite3 *db) {
    Weather w1("Botosani", "snow fall");
    Weather w2("Iasi", "heavy rain");

    w1.save(db);
    w2.save(db);
}

string Weather::getWeather(sqlite3 *db, string city) {
    string info_ev;
    bool ok = false;
    vector<Weather> ev = Weather::getAll(db);

    for(auto e : ev) {
        if(e.city == city) {
            info_ev = e.weather_info;
            ok = true;
            break;
        }
    }
    if(ok == false)
        info_ev = "nimic";
    return info_ev;
}

bool Weather::equal(string str1) {
    return str1 == city;
}

string Weather::getWeather() {
    return weather_info;
}
