#ifndef APPDATA_H
#define APPDATA_H

#include <QDate>
#include <QList>
#include <QString>

struct Facility
{
    QString name;
};

struct Room
{
    int id;
    QString type; // single room, double room, triple room
    int beds;
    QList<Facility> facilities;
    double basePrice;
    bool hasSofa;           // increases capacity by 1
    QList<QDate> bookedDates;
};

struct Accommodation
{
    int id;
    QString name;
    QString location;
    QString address;
    double discountPercent;
    QString promoName;
    int capacity;
    QList<Room> rooms;
};

struct BookingHistory
{
    QString hotelName;
    QString dateRange;
    QString status;
    QString userName;
    QString userEmail;
};

struct UserInfo
{
    QString name, email, password, phone, dob, country, gender, address;
    double balance = 10000.0;
};

#endif // APPDATA_H
