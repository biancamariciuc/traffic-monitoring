CREATE TABLE IF NOT EXISTS drivers
(
    ID
    INTEGER
    PRIMARY
    KEY
    AUTOINCREMENT,
    Name
    TEXT
);
CREATE TABLE IF NOT EXISTS location
(
    ID
    INTEGER
    PRIMARY
    KEY
    AUTOINCREMENT,
    City
    TEXT,
    Street
    TEXT
);
CREATE TABLE IF NOT EXISTS events
(
    ID
    INTEGER
    PRIMARY
    KEY
    AUTOINCREMENT,
    City
    TEXT,
    Street
    TEXT,
    Event
    TEXT
);
CREATE TABLE IF NOT EXISTS weather
(
    ID
    INTEGER
    PRIMARY
    KEY
    AUTOINCREMENT,
    City
    TEXT,
    Event
    TEXT
);
CREATE TABLE IF NOT EXISTS speed
(
    ID
    INTEGER
    PRIMARY
    KEY
    AUTOINCREMENT,
    City
    TEXT,
    Street
    TEXT,
    Speed
    INTEGER
);
CREATE TABLE IF NOT EXISTS fuel
(
    ID
    INTEGER
    PRIMARY
    KEY
    AUTOINCREMENT,
    City
    TEXT,
    Street
    TEXT,
    Fuel
    INTEGER
);
CREATE TABLE IF NOT EXISTS accident
(
    ID
    INTEGER
    PRIMARY
    KEY
    AUTOINCREMENT,
    City
    TEXT,
    Street
    TEXT,
    Accident
    INTEGER
);

create table location_dg_tmp
(
    ID        INTEGER
        primary key autoincrement,
    City      TEXT,
    Street    TEXT,
    id_driver integer
        constraint location_drivers_ID_fk
            references drivers
);

insert into location_dg_tmp(ID, City, Street)
select ID, City, Street
from location;

drop table location;

alter table location_dg_tmp
    rename to location;