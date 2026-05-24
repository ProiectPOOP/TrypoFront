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
    QString imageSource;
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
    QString imageSource;
};

struct BookingHistory
{
    int bookingId;
    QString hotelName;
    QString roomType;
    QString dateRange;
    QString status;
    QString userName;
    QString userEmail;
    QString rawCheckIn; // data de check-in în format "yyyy-MM-dd"
    double totalCost;   // pretul total al cazarii
};

struct UserInfo
{
    QString name, email, password, phone, dob, country, gender, address;
    double balance;
};

inline int IdUser=0;
#endif
