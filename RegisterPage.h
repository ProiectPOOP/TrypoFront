#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QComboBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QWidget>

class MainWindow;

class RegisterPage
{
public:
    static QWidget *createWidget(MainWindow *mw,
                                 QLineEdit *&regNameInput,
                                 QLineEdit *&regEmailInput,
                                 QLineEdit *&regPasswordInput,
                                 QLineEdit *&regPhoneInput,
                                 QLineEdit *&regAddressInput,
                                 QDateEdit *&regDobInput,
                                 QComboBox *&regCountryInput,
                                 QComboBox *&regGenderInput);
};

#endif // REGISTERPAGE_H
