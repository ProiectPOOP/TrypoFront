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

    // Auth
    void processLogin();
    void processRegister();

    // Search / filter
    void filterAccommodations(const QString &query);
    void filterRooms(const QString &query);

    // Accommodation details & booking
    void openAccommodationDetails(const Accommodation &acc);
    void bookRoom(int roomId);

    // Backend
    void handleBackendMessage(const QString &message);

private:
    void setupUi();

    // UI update helpers
    void populateAccommodations(const QString &f = "");
    void displayRooms(const QString &filter = "");
    void clearRegisterFields();
    void updateBookingHistoryUi();
    void updateAdminDashboardUi();

    QString ip = "10.10.25.219";
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
    QLineEdit   *searchBarInput;
    QWidget     *accommodationsContainer;
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
    UserInfo currentUser;
    QList<Accommodation> allAccommodations;
    QList<BookingHistory> userBookings;
};

#endif // MAINWINDOW_H
