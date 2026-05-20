#include "RegisterPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QCalendarWidget>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

QWidget *RegisterPage::createWidget(MainWindow *mw,
                                    QLineEdit *&regNameInput,
                                    QLineEdit *&regEmailInput,
                                    QLineEdit *&regPasswordInput,
                                    QLineEdit *&regPhoneInput,
                                    QLineEdit *&regAddressInput,
                                    QDateEdit *&regDobInput,
                                    QComboBox *&regCountryInput,
                                    QComboBox *&regGenderInput)
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(50, 20, 50, 20);

    QLabel *t = new QLabel("Register to Trypo", w);
    t->setStyleSheet("font-size: 26px; font-weight: bold; color: #f8fafc; margin-bottom: 20px;");

    QFormLayout *fl = new QFormLayout();
    fl->setSpacing(12);

    regNameInput     = new QLineEdit(); regNameInput->setStyleSheet(lineEditStyle);
    regEmailInput    = new QLineEdit(); regEmailInput->setStyleSheet(lineEditStyle);
    regPasswordInput = new QLineEdit(); regPasswordInput->setEchoMode(QLineEdit::Password);
    regPasswordInput->setStyleSheet(lineEditStyle);
    regPhoneInput    = new QLineEdit(); regPhoneInput->setStyleSheet(lineEditStyle);
    regAddressInput  = new QLineEdit(); regAddressInput->setStyleSheet(lineEditStyle);

    regDobInput = new QDateEdit(QDate(2000, 1, 1));
    regDobInput->setCalendarPopup(true);
    regDobInput->setStyleSheet(dropDownStyle);

    QCalendarWidget *calendar = regDobInput->calendarWidget();
    calendar->setStyleSheet(
        "QCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #0f172a; padding: 4px; }"
        "QCalendarWidget QToolButton { color: white; background-color: transparent; border: none; font-weight: bold; font-size: 14px; padding: 4px 8px; margin: 0px 2px; }"
        "QCalendarWidget QToolButton:hover { background-color: #2563eb; border-radius: 4px; }"
        "QCalendarWidget QMenu { background-color: #1e293b; color: white; border: 1px solid #475569; }"
        "QCalendarWidget QSpinBox { background-color: #1e293b; color: white; border: 1px solid #475569; border-radius: 3px; padding: 2px 4px; min-width: 65px; margin-left: 10px; }"
        "QCalendarWidget QSpinBox::up-button, QCalendarWidget QSpinBox::down-button { width: 16px; }"
        "QCalendarWidget QAbstractItemView:enabled { background-color: #1e293b; color: white; selection-background-color: #2563eb; selection-color: white; border: none; outline: none; }"
        "QCalendarWidget QAbstractItemView:disabled { color: #475569; }");

    regCountryInput = new QComboBox();
    regCountryInput->addItems({"Romania", "Moldova", "UK", "Germany"});
    regCountryInput->setStyleSheet(dropDownStyle);

    regGenderInput = new QComboBox();
    regGenderInput->addItems({"Male", "Female", "Unspecified"});
    regGenderInput->setStyleSheet(dropDownStyle);

    QString ls = "font-weight: bold; color: #94a3b8; font-size: 14px;";
    auto addR = [&](const QString &txt, QWidget *in) {
        QLabel *lbl = new QLabel(txt);
        lbl->setStyleSheet(ls);
        fl->addRow(lbl, in);
    };
    addR("Full Name:",     regNameInput);
    addR("Email:",         regEmailInput);
    addR("Password:",      regPasswordInput);
    addR("Phone:",         regPhoneInput);
    addR("Date of Birth:", regDobInput);
    addR("Country:",       regCountryInput);
    addR("Gender:",        regGenderInput);
    addR("Address:",       regAddressInput);

    ml->addWidget(t);
    ml->addLayout(fl);

    QPushButton *rb = new QPushButton("Register Now", w);
    rb->setStyleSheet(primaryBtnStyle);
    QPushButton *bb = new QPushButton("Back to Login", w);
    bb->setStyleSheet(secondaryBtnStyle);

    ml->addSpacing(20);
    ml->addWidget(rb);
    ml->addWidget(bb);

    QObject::connect(rb, &QPushButton::clicked, mw, &MainWindow::processRegister);
    QObject::connect(bb, &QPushButton::clicked, mw, &MainWindow::goToLogin);
    return w;
}
