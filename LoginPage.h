#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QLineEdit>
#include <QWidget>

class MainWindow;

class LoginPage
{
public:
    static QWidget *createWidget(MainWindow *mw,
                                 QLineEdit *&loginEmailInput,
                                 QLineEdit *&loginPasswordInput);
};

#endif
