#include "Accident.h"

Accident ::  Accident(int id, string city, string street, string accident_info) {
    this->id = id;
    this->city = city;
    this->street = street;
    this->accident_info = accident_info;
}

Accident::Accident(string city, string street, string accident_info) {
    this->city = city;
    this->street = street;
    this->accident_info = accident_info;
}

vector<Accident> Accident ::  getAll(sqlite3 *db) {
    vector<Accident> r;

    const char *select_sql = "SELECT * FROM accident;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string city(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            string street(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            string accident_info(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));


            Accident a(id, city, street, accident_info);
            r.push_back(a);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to query the database" << std::endl;
    }
    return r;
}

void Accident ::  save(sqlite3 *db) {
    auto insert = "INSERT INTO accident (city, street, accident) VALUES ('" + this->city + "', '" + this->street + "', '" + this->accident_info + "');";
    auto r  = sqlite3_exec(db, insert.c_str(), 0, 0, NULL);

    if ( r != SQLITE_OK) {
        std::cerr << "SQL error" << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const Accident& obj) {
    os << "id: " << obj.id << ", name: " << obj.city << ", street: " << obj.street << ", accident info :" << obj.accident_info;
    return os;
}

void Accident ::deleteById(sqlite3 *db, int id) {
    string delete_sql = "DELETE FROM accident WHERE ID = " + std::to_string(id) + ";";

    int result = sqlite3_exec(db, delete_sql.c_str(), nullptr, nullptr, NULL);

    if (result != SQLITE_OK) {
        std::cerr << "Error deleting row" << std::endl;
    }
}


void Accident::populateAccident(sqlite3 *db) {
    Accident a1("Iasi", "Copou 12", "an audi jumped off the road");
    Accident a2("Iasi", "Stefan", "a Bucharest resident lost control of the car and ran into a pole");
    Accident a3("Botsani", "Copou 12", "an audi jumped off the road");
    Accident a4("Botosani", "Stefan", "a Bucharest resident lost control of the car and ran into a pole");
    Accident a5("Iasi", "Tatarasi", "is a traffic jam");
    Accident a6("Iasi", "Bucium", "is atraffic jam");

    a1.save(db);
    a2.save(db);
    a3.save(db);
    a4.save(db);
    a5.save(db);
    a6.save(db);
}

string Accident::getAccident(sqlite3 *db, string city, string street) {
    string info_ev;
    bool ok = false;
    vector<Accident> ev = Accident::getAll(db);

    for(auto e : ev) {
        if(e.street == street && e.city == city) {
            info_ev = e.accident_info;
            ok = true;
            break;
        }
    }
    if(ok == false)
        info_ev = "nimic";
    return info_ev;
}
