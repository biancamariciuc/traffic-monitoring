#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include "Accident.h"
#include <stdbool.h>
#include <time.h>
#include <thread>

#define PORT 1248
#define CITY_SIZE 100
#define STREET_SIZE 200
#define CMD_SIZE 50
#define NAME_SIZE 100

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

int socket_des;
Command c;
bool printMsj = true;

bool beginWith(char str[255], char sub_str[100]) {
    return strncmp(str, sub_str, strlen(sub_str)) == 0;
}


void *handle_write(void *arg) {


    while (1) {

        char r[500];

        if (read(socket_des, &r, sizeof(r)) < 0) {
            perror("[client]error read from server.\n");
            return NULL;
        }
        if(printMsj == true) {
            printf("%s\n", r);
        }
    }
}

void advice_speed() {
    int speed = rand() % 81;
    char r[500];

    std :: string speed_char = std::to_string(speed);


    strcpy(r, "Your actual speed is ");
    strcat(r, speed_char.c_str());

    sqlite3 *db;
    if (sqlite3_open("tabels.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(1);
    }

    Accident a;
    string acc_info = a.getAccident(db, c.city, c.street);
    if (acc_info == "nimic") {
        if (speed > 50) {
            strcat(r, " per hour and you are in a city where the limit is 50. We suggest you to slow down.");
        }
        if (speed < 50) {
            strcat(r, " per hour. You are driving so well.");
        }
    } else {
        strcat(r, ". You should slow down because near to you : ");
        strcat(r, acc_info.c_str());
    }

    printf("%s\n", r);
    fflush(stdout);
}

void *give_info_speed(void *arg) {

    srand(time(NULL));
    time_t last_time = time(NULL);
    time_t current_time;
    while (1) {
        current_time = time(NULL);
        if (difftime(current_time, last_time) >= 60) {
            advice_speed();
            last_time = current_time;
        }
    }

}

int main() {

    struct sockaddr_in server;

    if ((socket_des = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("error creating socket");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    if (connect(socket_des, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1) {
        perror("[client]error connecting to server.\n");
        return errno;
    }


    printf("you can start (You can acces <help> if you do not know the sintax)\n");
    fflush(stdout);
    char buf[255];

    memset(&c, 0, sizeof(c));
    char r[500];

    pthread_t write_thread;
    pthread_t speed_thread;

    pthread_create(&write_thread, nullptr, handle_write, NULL);
    pthread_create(&speed_thread, NULL, give_info_speed, NULL);


    while (1) {

        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("error fgets()");
            break;
        }

        memset(r, 0, sizeof(r));

        if (beginWith(buf, "login")) {
            char *p = strtok(buf, ":");
            strcpy(c.cmd, p);
            p = strtok(NULL, "\n");
            strcpy(c.name, p);
        } else if (beginWith(buf, "sign-up")) {
            char *p = strtok(buf, ":");
            strcpy(c.cmd, p);
            p = strtok(NULL, "\n");
            strcpy(c.name, p);
        } else if(beginWith(buf, "yes")){
            c.subscription = 1;
            strcpy(c.cmd, "subscription");
        } else if(beginWith(buf, "no")){
            c.subscription = 0;
            strcpy(c.cmd, "subscription");
        }else if (beginWith(buf, "location")) {
            printMsj = true;
            char *p = strtok(buf, ":");
            strcpy(c.cmd, p);
            p = strtok(NULL, ",");
            strcpy(c.city, p);
            p = strtok(NULL, "\n");
            strcpy(c.street, p);
        } else if (beginWith(buf, "accident")) {
            printMsj = true;
            char *p = strtok(buf, ":");
            strcpy(c.cmd, p);
            p = strtok(NULL, ",");
            strcpy(c.city_acc, p);
            p = strtok(NULL, "\n");
            strcpy(c.street_acc, p);
        } else if (beginWith(buf, "weather info") || beginWith(buf, "fuel info") || beginWith(buf, "events info")) {
            printMsj = true;
            strcpy(c.cmd, buf);
            c.cmd[strcspn(c.cmd, "\n")] = '\0';
        } else if (beginWith(buf, "help")) {
            printMsj = true;
            strcpy(c.cmd, "help");
//            printf("The syntax for commands:\n sign-up:name\n login:name\n location:city,street\n accident:city,street\n weather info\n fuel info\n events info\n exit\n");
//            fflush(stdout);
        } else if (beginWith(buf, "exit")) {
            printMsj = true;
            break;
        } else if (beginWith(buf, "logout")) {
            printMsj = true;
            strcpy(c.cmd, "logout");
        } else {
            printMsj = false;
            printf("You wrote something wrong!\n");
        }

        if (write(socket_des, &c, sizeof(Command)) <= 0) {
            perror("[client]error writing to server.\n");
            return errno;
        }
    }

    shutdown(socket_des, SHUT_RDWR);
    close(socket_des);
    return 0;
}