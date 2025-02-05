#include "Fuel.h"

Fuel ::  Fuel(int id, string city, string street, string fuel_info) {
    this->id = id;
    this->city = city;
    this->street = street;
    this->fuel_info = fuel_info;
}

Fuel::Fuel(string city, string street, string fuel_info) {
    this->city = city;
    this->street = street;
    this->fuel_info = fuel_info;
}

vector<Fuel> Fuel ::  getAll(sqlite3 *db) {
    vector<Fuel> r;

    const char *select_sql = "SELECT * FROM fuel;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string city(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string street(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            string fuel(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));


            Fuel a(id, city, street, fuel);
            r.push_back(a);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to query the database" << std::endl;
    }
    return r;
}

void Fuel::save(sqlite3 *db) {
    auto insert = "INSERT INTO fuel (city, street, fuel) VALUES ('" + this->city + "', '" + this->street + "', '" + this->fuel_info + "');";
    auto r  = sqlite3_exec(db, insert.c_str(), 0, 0, NULL);

    if (r != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    }
}


void Fuel::deleteById(sqlite3 *db, int id) {
    string delete_sql = "DELETE FROM fuel WHERE ID = " + std::to_string(id) + ";";

    int result = sqlite3_exec(db, delete_sql.c_str(), nullptr, nullptr, NULL);

    if (result != SQLITE_OK) {
        std::cerr << "Error deleting row" << std::endl;
    }
}

std::ostream &operator<<(ostream &os, const Fuel &obj) {
    os << "id: " << obj.id << ", city: " << obj.city << ", street: " << obj.street << ", event: " << obj.fuel_info;
    return os;
}

void Fuel::populateFuel(sqlite3 *db) {
    Fuel f1("Iasi", "Copou 12", "OMV (Disel: 7.48 Lei, Benzine:7.31 Lei)");
    Fuel f2("Iasi", "Targusor Copou", "SOCAR (Disel: 7.49 Lei, Benzine:7.39 Lei)");
    Fuel f3("Iasi", "Stefan", "ROMPETROL (Disel: 7.50 Lei, Benzine:7.30 Lei)");
    Fuel f4("Iasi", "Alexandru Ioan Cuza", "Petrom (Disel: 7.48 Lei, Benzine:7.40 Lei)");
    Fuel f5("Iasi", "Tatarasi", "Cristal (Disel: 7.30 Lei, Benzine:7.31 Lei)");
    Fuel f6("Iasi", "Stefan 20", "LUK-OIL (Disel: 7.48 Lei, Benzine:7.31 Lei)");
    Fuel f7("Iasi", "Bucium", "Petrom (Disel: 7.50 Lei, Benzine:7.34 Lei)");
    Fuel f8("Botosani", "Stefan 20", "LUK-OIL (Disel: 7.48 Lei, Benzine:7.31 Lei)");
    Fuel f9("Botosani", "Bucium", "Petrom (Disel: 7.50 Lei, Benzine:7.34 Lei)");

    f1.save(db);
    f2.save(db);
    f3.save(db);
    f4.save(db);
    f5.save(db);
    f6.save(db);
    f7.save(db);
}

string Fuel::getFuel(sqlite3 *db, string city, string street) {
    string info_ev;
    bool ok = false;
    vector<Fuel> ev = Fuel::getAll(db);

    for(auto e : ev) {
        if(e.street == street && e.city == city) {
            info_ev = e.fuel_info;
            ok = true;
            break;
        }
    }
    if(ok == false)
        info_ev = "nimic";
    return info_ev;
}

bool Fuel::equal(string str1, string str2) {
    return str1 == city && str2 == street;
}

string Fuel::getFuel() {
    return fuel_info;
}
