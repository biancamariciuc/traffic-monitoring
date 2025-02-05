#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <mysql/mysql.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include "Person.h"
#include "Location.h"
#include "Event.h"
#include "Fuel.h"
#include "Weather.h"
#include <sys/select.h>
#include "Accident.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sstream>

#define PORT 1248
#define CITY_SIZE 100
#define STREET_SIZE 200
#define CMD_SIZE 50
#define NAME_SIZE 100

typedef struct {
    int thread_id;
    int socket_cl;
} thData;

typedef struct {
    bool subscription;
    char cmd[CMD_SIZE];
    char city[CITY_SIZE];
    char street[STREET_SIZE];
    char city_acc[CITY_SIZE];
    char street_acc[STREET_SIZE];
    char name[NAME_SIZE];
    int socket_id;
} Command;

Command c;
sqlite3 *db;
vector<int> clients;
vector<string> subscription;
vector<string> logged;

char City[CITY_SIZE];
char Street[STREET_SIZE];

void *answer(void *arg);

static void *treat(void *arg) {
    thData tdL = *((thData *) arg);

    printf("[Thread %d] waiting for a message...\n", tdL.thread_id);
    fflush(stdout);

    pthread_detach(pthread_self());

    pthread_t th1, th2;
    answer(arg);

    close(tdL.socket_cl);
    free(arg);
    return NULL;
}

char *read_from_file(const char *path) {
    FILE *file;
    char *buffer;
    long file_size;


    file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    buffer = (char *) malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return nullptr;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    return buffer;
}


void delete_data_from_tabels() {
    const char *sql1 = "DELETE FROM drivers;";
    const char *sql2 = "DELETE FROM location;";
    const char *sql3 = "DELETE FROM accident;";
    const char *sql4 = "DELETE FROM events;";
    const char *sql5 = "DELETE FROM fuel;";
    const char *sql6 = "DELETE FROM weather;";

    sqlite3_exec(db, sql1, 0, 0, NULL);
    sqlite3_exec(db, sql2, 0, 0, NULL);
    sqlite3_exec(db, sql3, 0, 0, NULL);
    sqlite3_exec(db, sql4, 0, 0, NULL);
    sqlite3_exec(db, sql5, 0, 0, NULL);
    sqlite3_exec(db, sql6, 0, 0, NULL);
}

void populate_tabels() {
    Person p;
    p.populateDrivers(db);

    Location l;
    l.populateLocation(db);

    Accident a;
    a.populateAccident(db);

    Event e;
    e.populateEvent(db);

    Fuel f;
    f.populateFuel(db);

    Weather w;
    w.populateWeather(db);
}

void create_tabel() {
    char *err_msg = new char[100];

    if (sqlite3_open("tabels.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(1);
    }

    char *sql = read_from_file("./sql/server_data_creation.sql");

    if (sqlite3_exec(db, sql, nullptr, nullptr, (char **) &err_msg) != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }

    delete_data_from_tabels();
    populate_tabels();

    delete[] sql;
}


void broadcast_accident() {
    char r[500];
    strcpy(r, "Accident reported in: ");
    strcat(r, City);
    strcat(r, ", ");
    strcat(r, Street);

    Accident a(City, Street, "accident with 2 injured");
    a.save(db);

    for (auto it = clients.begin(); it != clients.end();) {
        if (write(*it, r, sizeof(r)) == -1) {
            perror("write");
            close(*it);
            it = clients.erase(it);
        } else {
            ++it;
        }
    }
}

bool find_client_logged(string str) {
    for (int i = 0; i < logged.size(); i++)
        if (logged[i] == str)
            return true;
    return false;
}

bool find_client_subscription(string str) {
    for (int i = 0; i < subscription.size(); i++)
        if (subscription[i] == str)
            return true;
    return false;
}

void Remove(string str) {
    int index = 0;
    for (int i = 0; i < logged.size(); i++) {
        if (logged[i] != str) {
            logged[index++] = logged[i];
        }
    }
    logged.resize(index);
}

void give_help(Command *c, char r[500]) {
    strcpy(r, "The syntax for commands:\n sign-up:name\n login:name\n logout\n location:city,street\n accident:city,street\n weather info\n fuel info\n events info\n exit\n");
}

void sign_in(Command *c, char r[500]) {

    Person p(c->name);
    p.save(db);
    strcpy(r, "You are logged ");
    strcat(r, c->name);
    strcat(r, ".\n Would you like a subscription for more information about weather, events and fuel stations?");
    if (!find_client_logged(c->name))
        logged.push_back(c->name);

}

void login(Command *c, char r[500]) {
    Person p;
    if (p.searchByName(c->name, db) && !find_client_logged(c->name)) {
        strcpy(r, "You are logged ");
        strcat(r, c->name);
        strcat(r, ".\nWould you like a subscription for more information about weather, events and fuel stations?");

        if (!find_client_logged(c->name))
            logged.push_back(c->name);
    } else if (!p.searchByName(c->name, db) && !find_client_logged(c->name)) {
        strcpy(r, "You are not register ");
        strcat(r, c->name);
        strcat(r, ".  You can register with command 'sign-up:name'");
    } else {
        strcpy(r, "You are already logged ");
        strcat(r, c->name);
    }
}

void give_subscription(Command *c, char r[500]) {
    if(c->subscription == 1) {
        strcpy(r, "You have subscription for events, weather and fuel station information.");
        subscription.push_back(c->name);
    } else {
        strcpy(r, "You have no subscription for events, weather and fuel station information.");
    }
}

void give_info_location(Command *c, char r[500]) {
    if (find_client_logged(c->name)) {
        Location l;
        if (l.validateData(db, c->city, c->street)) {
            strcpy(r, "the location is stored");
        } else {
            strcpy(r, "you have written a location that did not exist");
        }
    } else {
        strcpy(r, "you have to sign-up first");
    }
}

void give_info_accident(Command *c, char r[500]) {

    strcpy(City, c->city_acc);
    strcpy(Street, c->street_acc);
    Location l;
    if (l.validateData(db, c->city_acc, c->street_acc)) {
        strcpy(r, "the location is stored");
        broadcast_accident();
    } else {
        strcpy(r, "you have written a location that did not exist");
    }
}

void give_info_events(Command *c, char r[500]) {
    if (find_client_logged(c->name) && find_client_subscription(c->name) ) {
        Event e;
        string info = e.getEvent(db, c->city, c->street);
        if (info == "nimic") {
            strcpy(r, "there is no event near by");
        } else {
            strcpy(r, "event: ");
            strcat(r, info.c_str());
        }

    } else if(!find_client_subscription(c->name)){
        strcpy(r, "you do not have subscription");
    } else {
        strcpy(r, "you have to sign-in first");
    }
}

void give_info_fuel(Command *c, char r[500]) {
    if (find_client_logged(c->name) && find_client_subscription(c->name) ) {
        Fuel e;
        string info = e.getFuel(db, c->city, c->street);
        if (info == "nimic") {
            strcpy(r, "there is no fuel station near by");
        } else {
            strcpy(r, "there is the ");
            strcat(r, info.c_str());
            strcat(r, " station near by you");
        }

    }else if(!find_client_subscription(c->name)){
        strcpy(r, "you do not have subscription");
    } else {
        strcpy(r, "you have to sign-in first");
    }
}

void give_info_weather(Command *c, char r[500]) {
    if (find_client_logged(c->name) && find_client_subscription(c->name) ) {
        Weather w;
        string info = w.getWeather(db, c->city);
        if (info == "nimic") {
            strcpy(r, "there is no info about the weather of this city");
        } else {
            strcpy(r, "weather: ");
            strcat(r, info.c_str());
        }

    } else if(!find_client_subscription(c->name)){
        strcpy(r, "you do not have subscription");
    } else {
        strcpy(r, "you have to sign-in first");
    }
}

void logout(Command *c, char r[500]) {
    strcpy(r, "you are logged out ");
    strcat(r, c->name);
    Remove(c->name);
}

void *answer(void *arg) {


    memset(&c, 0, sizeof(Command));
    char r[500];
    memset(r, 0, sizeof(r));

    thData tdL = *((thData *) arg);

    while (1) {

        if (read(tdL.socket_cl, &c, sizeof(Command)) <= 0) {
            printf("[Thread %d] error reading from client.\n", tdL.thread_id);
            perror("read error");
            return nullptr;
        }
        printf("%d", c.socket_id);
        fflush(stdout);
        if (strcmp(c.cmd, "login") == 0) {
            login(&c, r);
        } else if (strcmp(c.cmd, "subscription") == 0) {
            give_subscription(&c, r);
        }else if (strcmp(c.cmd, "sign-up") == 0) {
            sign_in(&c, r);
        } else if (strcmp(c.cmd, "logout") == 0) {
            logout(&c, r);
        } else if (strcmp(c.cmd, "help") == 0) {
            give_help(&c, r);
        } else if (strcmp(c.cmd, "location") == 0) {
            give_info_location(&c, r);
        } else if (strcmp(c.cmd, "accident") == 0) {
            give_info_accident(&c, r);
        } else if (strcmp(c.cmd, "weather info") == 0) {
            give_info_weather(&c, r);
        } else if (strcmp(c.cmd, "fuel info") == 0) {
            give_info_fuel(&c, r);
        } else if (strcmp(c.cmd, "events info") == 0) {
            give_info_events(&c, r);
        }


        if (write(tdL.socket_cl, &r, sizeof(r)) <= 0) {
            printf("[Thread %d] error writing to client.\n", tdL.thread_id);
            perror("write error");
        } else {
            printf("[Thread %d] message sent successfully.\n", tdL.thread_id);
        }
    }
}


int main() {
    create_tabel();

    struct sockaddr_in server, from;
    int socket_des, client;
    socklen_t length = sizeof(from);
    pthread_t th[100];
    pthread_t speed_thread;
    int i = 0;

    if ((socket_des = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[Server] error creating socket.");
        return errno;
    }


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(socket_des, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
        perror("[Server] error binding socket.");
        return errno;
    }

    if (listen(socket_des, 2) == -1) {
        perror("[Server] error on listen.");
        return errno;
    }


    while (1) {
        if ((client = accept(socket_des, (struct sockaddr *) &from, &length)) < 0) {
            perror("[Server] error accepting client connection.");
            continue;
        }

        clients.push_back(client);


        thData *td = (thData *) malloc(sizeof(thData));
        td->thread_id = i;
        td->socket_cl = client;


        if (pthread_create(&th[i++], NULL, treat, td) != 0) {
            perror("[Server] error creating thread.");
            free(td);
            close(client);
        }
    }

    sqlite3_close(db);
}
