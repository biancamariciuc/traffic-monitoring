#include "Person.h"

Person ::  Person(int id, string name) {
    this->id = id;
    this->name = name;
}

Person::Person(string name) {
    this->name = name;
}

vector<Person> Person ::  getAll(sqlite3 *db) {
    vector<Person> r;

    const char *select_sql = "SELECT * FROM drivers;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));

            Person p(id, name);
            r.push_back(p);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to query the database" << std::endl;
    }
    return r;
}

void Person ::  save(sqlite3 *db) {
    auto insert = "INSERT INTO drivers (Name) VALUES ('" + this->name + "');";
    auto r  = sqlite3_exec(db, insert.c_str(), 0, 0, NULL);

    if ( r != SQLITE_OK) {
        std::cerr << "SQL error" << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const Person& obj) {
    os << "id: " << obj.id << ", name: " << obj.name;
    return os;
}

void Person ::deleteById(sqlite3 *db, int id) {
    string delete_sql = "DELETE FROM drivers WHERE ID = " + std::to_string(id) + ";";

    int result = sqlite3_exec(db, delete_sql.c_str(), nullptr, nullptr, NULL);

    if (result != SQLITE_OK) {
        std::cerr << "Error deleting row" << std::endl;
    }
}


void Person::populateDrivers(sqlite3 *db) {
    Person p1("Maria");
    Person p2("Ionel");
    Person p3("Andrei");
    Person p4("Ariadna");
    Person p5("Magda");
    Person p6("Bianca");
    Person p7("Erica");
    Person p8("Victoria");

    p1.save(db);
    p2.save(db);
    p3.save(db);
    p4.save(db);
    p5.save(db);
    p6.save(db);
    p7.save(db);
    p8.save(db);
}

bool Person::searchByName(string name, sqlite3 *db) {
    sqlite3_stmt* stmt;

    std::string sql = "SELECT COUNT(*) FROM drivers WHERE name = ?;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        found = (count > 0);
    }

    sqlite3_finalize(stmt);

    return found;
}
