#include "Event.h"

Event ::  Event(int id, string city, string street, string event_info) {
    this->id = id;
    this->city = city;
    this->street = street;
    this->event_info = event_info;
}

Event::Event(string city, string street, string event_info) {
    this->city = city;
    this->street = street;
    this->event_info = event_info;
}

vector<Event> Event ::  getAll(sqlite3 *db) {
    vector<Event> r;

    const char *select_sql = "SELECT * FROM events;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string city(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string street(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            string event(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));


            Event a(id, city, street, event);
            r.push_back(a);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to query the database" << std::endl;
    }
    return r;
}

void Event::save(sqlite3 *db) {
    auto insert = "INSERT INTO events (city, street, event) VALUES ('" + this->city + "', '" + this->street + "', '" + this->event_info + "');";
    auto r  = sqlite3_exec(db, insert.c_str(), 0, 0, NULL);

    if (r != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    }
}


void Event::deleteById(sqlite3 *db, int id) {
    string delete_sql = "DELETE FROM events WHERE ID = " + std::to_string(id) + ";";

    int result = sqlite3_exec(db, delete_sql.c_str(), nullptr, nullptr, NULL);

    if (result != SQLITE_OK) {
        std::cerr << "Error deleting row" << std::endl;
    }
}

std::ostream &operator<<(ostream &os, const Event &obj) {
    os << "id: " << obj.id << ", city: " << obj.city << ", street: " << obj.street << ", event: " << obj.event_info;
    return os;
}

void Event::populateEvent(sqlite3 *db) {
    Event e1("Iasi", "Copou 12", "group of runners");
    Event e2("Iasi", "Targusor Copou", "football match at the stadium");
    Event e3("Iasi", "Stefan", "Sf. Parascheva days");
    Event e4("Iasi", "Alexandru Ioan Cuza", "university days");
    Event e5("Iasi", "Stefan 20", "students days");

    e1.save(db);
    e2.save(db);
    e3.save(db);
    e4.save(db);
    e5.save(db);
}

string Event::getEvent(sqlite3 *db, string city, string street) {
    string info_ev;
    bool ok = false;
    vector<Event> ev = Event::getAll(db);

    for(auto e : ev) {
        if(e.street == street && e.city == city) {
            info_ev = e.event_info;
            ok = true;
            break;
        }
    }
    if(ok == false)
        info_ev = "nimic";
    return info_ev;
}

bool Event::equal(string str1, string str2) {
    return str1 == city && str2 == street;
}

string Event::getEvent() {
    return event_info;
}
