#include "Location.h"

Location::Location(int id, string city, string street) {
    this->id = id;
    this->city=city;
    this->street=street;
}

Location::Location(string city, string street) {
    this->city=city;
    this->street=street;
}

vector<Location> Location::getAll(sqlite3 *db) {
    vector<Location> r;

    const char *select_sql = "SELECT * FROM location;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string city(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string street(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

            Location l(id, city, street);
            r.push_back(l);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to query the database" << std::endl;
    }
    return r;
}

void Location::save(sqlite3 *db) {
    auto insert = "INSERT INTO location (city, street) VALUES ('" + this->city + "', '" + this->street +"');";
    auto r  = sqlite3_exec(db, insert.c_str(), 0, 0, NULL);

    if ( r != SQLITE_OK) {
        std::cerr << "SQL error" << std::endl;
    }
}

void Location::deleteById(sqlite3 *db, int id) {
    string delete_sql = "DELETE FROM location WHERE ID = " + std::to_string(id) + ";";

    int result = sqlite3_exec(db, delete_sql.c_str(), nullptr, nullptr, NULL);

    if (result != SQLITE_OK) {
        std::cerr << "Error deleting row" << std::endl;
    }
}

std::ostream &operator<<(ostream &os, const Location &obj) {
    os << "id: " << obj.id << ", city: " << obj.city << ", street: " << obj.street;
    return os;
}

void Location::populateLocation(sqlite3 *db) {
    Location l1("Iasi", "Copou 12");
    Location l2("Iasi", "Targusor Copou");
    Location l3("Iasi", "Stefan");
    Location l4("Iasi", "Alexandru Ioan Cuza");
    Location l5("Iasi", "Mihai 12");
    Location l6("Iasi", "Stefan 20");
    Location l7("Iasi", "Corbu 30");
    Location l8("Iasi", "Palatul Culturii");
    Location l9("Iasi", "Tatarasi");
    Location l10("Iasi", "Bucium");
    Location l11("Botosani", "Copou 12");
    Location l12("Botosani", "Targusor Copou");
    Location l13("Botosani", "Stefan");
    Location l14("Botosani", "Alexandru Ioan Cuza");
    Location l15("Botosani", "Mihai 12");
    Location l16("Botosani", "Stefan 20");
    Location l17("Botosani", "Corbu 30");
    Location l18("Botosani", "Palatul Culturii");
    Location l19("Botosani", "Tatarasi");
    Location l20("Botosani", "Bucium");

    l1.save(db);
    l2.save(db);
    l3.save(db);
    l4.save(db);
    l5.save(db);
    l6.save(db);
    l7.save(db);
    l8.save(db);
    l9.save(db);
    l10.save(db);
    l11.save(db);
    l12.save(db);
    l13.save(db);
    l14.save(db);
    l15.save(db);
    l16.save(db);
    l17.save(db);
    l18.save(db);
    l19.save(db);
    l20.save(db);
}

bool Location::validateData(sqlite3 *db, string city, string street) {
    vector<Location> loc = Location::getAll(db);

    for(auto l : loc) {
        if(l.street == street && l.city == city)
            return true;
    }
    return false;
}
