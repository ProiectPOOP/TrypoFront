#ifndef USERPROFILEPAGE_H
#define USERPROFILEPAGE_H

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow;

class UserProfilePage
{
public:
    static QWidget *createWidget(MainWindow *mw,
                                 QLabel *&lblNameVal,
                                 QLabel *&lblEmailVal,
                                 QLabel *&lblPhoneVal,
                                 QLabel *&lblDobVal,
                                 QLabel *&lblCountryVal,
                                 QLabel *&lblGenderVal,
                                 QLabel *&lblAddressVal,
                                 QLabel *&lblBalanceVal,
                                 QVBoxLayout *&historyLayout);
};

#endif // USERPROFILEPAGE_H
