#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "socketclient.h"

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

// FIX: Renamed from User to UserInfo to match the .cpp usage.
// Added default initialisation for balance so that resetting via
// currentUser = UserInfo() always produces a clean zero state.
struct UserInfo
{
    QString name, email, password, phone, dob, country, gender, address;
    double balance = 10000.0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void goToRegister();
    void goToLogin();
    void processLogin();
    void processRegister();
    void goToUserProfile();
    void backToMainApp();
    void filterAccommodations(const QString &query);
    void filterRooms(const QString &query);
    void openAccommodationDetails(const Accommodation &acc);
    void bookRoom(int roomId);
    // FIX: moved here from private — must be a slot because it is
    // connected via connect() in the constructor.
    void handleBackendMessage(const QString &message);

private:
    void setupUi();
    QWidget *createLoginWidget();
    QWidget *createRegisterWidget();
    QWidget *createMainAppWidget();
    QWidget *createUserProfileWidget();
    QWidget *createDetailsWidget();
    QWidget *createAdminDashboardWidget();
    void updateAdminDashboardUi();
    void updateBookingHistoryUi();
    void populateAccommodations(const QString &f = "");
    void displayRooms(const QString &filter = "");
    void clearRegisterFields();

    QString ip = "127.0.0.1";

    SocketClient *m_socketClient;

    // Stacked pages
    QStackedWidget *stackedWidget;

    // Login page
    QLineEdit *loginEmailInput, *loginPasswordInput;

    // Register page
    QLineEdit *regNameInput, *regEmailInput, *regPasswordInput,
        *regPhoneInput, *regAddressInput;
    QDateEdit *regDobInput;
    QComboBox *regCountryInput, *regGenderInput;

    // Main app page
    QLineEdit *searchBarInput;
    QWidget   *accommodationsContainer;
    // FIX: removed the unused/duplicate QJsonArray *allAccomodations pointer.
    // The authoritative list is QList<Accommodation> allAccommodations below.
    QVBoxLayout *accommodationsLayout;

    // Profile page
    QLabel *lblNameVal, *lblEmailVal, *lblPhoneVal, *lblDobVal,
        *lblCountryVal, *lblGenderVal, *lblAddressVal, *lblBalanceVal;
    QVBoxLayout *historyLayout;

    // Details / rooms page
    QLabel    *detName, *detAddress, *detPromo;
    QLineEdit *roomSearchBar;
    QVBoxLayout *roomsLayout;
    QCheckBox *cbBalcony, *cbFridge, *cbAC, *cbTV, *cbWifi, *cbSofa;
    Accommodation currentAccommodationInDetails;

    // Admin dashboard
    QVBoxLayout *adminHistoryLayout = nullptr;

    // Session state
    // FIX: type changed from User to UserInfo
    UserInfo currentUser;
    QList<Accommodation> allAccommodations;
    QList<BookingHistory> userBookings;
};

#endif // MAINWINDOW_H