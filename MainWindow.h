#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "AppData.h"
#include "socketclient.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // Navigation
    void goToRegister();
    void goToLogin();
    void goToUserProfile();
    void backToMainApp();
    void adminLogout();

    // authentification
    void processLogin();
    void processRegister();

    // search / filter
    void filterAccommodations(const QString &query);
    void filterRooms(const QString &query);
    void filterAdminBookings(const QString &query);

    // accomodation details & booking
    void openAccommodationDetails(const Accommodation &acc);
    void bookRoom(int roomId);

    // backend
    void handleBackendMessage(const QString &message);

private:
    void setupUi();

    // UI update helpers
    void populateAccommodations(const QString &f = "");
    void displayRooms(const QString &filter = "");
    void clearRegisterFields();
    void updateBookingHistoryUi();
    void updateAdminDashboardUi();
    void clientCancelBooking(int bookingIndex);

    QString ip = "10.10.25.219";
    SocketClient *m_socketClient;

    // stacked pages
    QStackedWidget *stackedWidget;

    // login page
    QLineEdit *loginEmailInput, *loginPasswordInput;

    // register page
    QLineEdit *regNameInput, *regEmailInput, *regPasswordInput,
        *regPhoneInput, *regAddressInput;
    QDateEdit *regDobInput;
    QComboBox *regCountryInput, *regGenderInput;

    // main app page
    QLineEdit   *searchBarInput;
    QWidget     *accommodationsContainer;
    QVBoxLayout *accommodationsLayout;

    // profile page
    QLabel *lblNameVal, *lblEmailVal, *lblPhoneVal, *lblDobVal,
        *lblCountryVal, *lblGenderVal, *lblAddressVal, *lblBalanceVal;
    QVBoxLayout *historyLayout;

    // details / rooms page
    QLabel    *detName, *detAddress, *detPromo;
    QLineEdit *roomSearchBar;
    QVBoxLayout *roomsLayout;
    QCheckBox *cbBalcony, *cbFridge, *cbAC, *cbTV, *cbWifi, *cbSofa;
    Accommodation currentAccommodationInDetails;

    // admin dashboard
    QVBoxLayout *adminHistoryLayout = nullptr;
    QLabel *lblAdminLocation = nullptr;
    QLineEdit *adminSearchBar = nullptr;

    // session state
    UserInfo currentUser;
    QList<Accommodation> allAccommodations;
    QList<BookingHistory> userBookings;
};

#endif
