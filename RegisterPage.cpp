#include "RegisterPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QCalendarWidget>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCompleter>
#include <QAbstractItemView>

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

    // centrare text
    t->setAlignment(Qt::AlignCenter);

    t->setStyleSheet(
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "font-size: 32px;"
        "font-weight: 900;"
        "color: #38bdf8;"
        "letter-spacing: 1px;"
        "margin-bottom: 30px;"
        );

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

    QStringList countries = {
        "Afghanistan", "Albania", "Algeria", "Andorra", "Angola", "Argentina", "Armenia", "Australia",
        "Austria", "Azerbaijan", "Bahamas", "Bahrain", "Bangladesh", "Barbados", "Belarus", "Belgium",
        "Belize", "Benin", "Bhutan", "Bolivia", "Bosnia and Herzegovina", "Botswana", "Brazil", "Brunei",
        "Bulgaria", "Burkina Faso", "Burundi", "Cambodia", "Cameroon", "Canada", "Cape Verde",
        "Central African Republic", "Chad", "Chile", "China", "Colombia", "Comoros", "Costa Rica",
        "Croatia", "Cuba", "Cyprus", "Czech Republic", "Denmark", "Djibouti", "Dominican Republic",
        "Ecuador", "Egypt", "El Salvador", "Equatorial Guinea", "Eritrea", "Estonia", "Eswatini",
        "Ethiopia", "Fiji", "Finland", "France", "Gabon", "Gambia", "Georgia", "Germany", "Ghana",
        "Greece", "Guatemala", "Guinea", "Haiti", "Honduras", "Hungary", "Iceland", "India", "Indonesia",
        "Iran", "Iraq", "Ireland", "Israel", "Italy", "Jamaica", "Japan", "Jordan", "Kazakhstan",
        "Kenya", "Kuwait", "Kyrgyzstan", "Laos", "Latvia", "Lebanon", "Lesotho", "Liberia", "Libya",
        "Liechtenstein", "Lithuania", "Luxembourg", "Madagascar", "Malawi", "Malaysia", "Maldives",
        "Mali", "Malta", "Mauritania", "Mauritius", "Mexico", "Moldova", "Monaco", "Mongolia",
        "Montenegro", "Morocco", "Mozambique", "Myanmar", "Namibia", "Nepal", "Netherlands",
        "New Zealand", "Nicaragua", "Niger", "Nigeria", "North Korea", "North Macedonia", "Norway",
        "Oman", "Pakistan", "Palestine", "Panama", "Paraguay", "Peru", "Philippines", "Poland",
        "Portugal", "Qatar", "Romania", "Russia", "Rwanda", "San Marino", "Saudi Arabia", "Senegal",
        "Serbia", "Seychelles", "Sierra Leone", "Singapore", "Slovakia", "Slovenia", "Somalia",
        "South Africa", "South Korea", "Spain", "Sri Lanka", "Sudan", "Sweden", "Switzerland", "Syria",
        "Taiwan", "Tajikistan", "Tanzania", "Thailand", "Togo", "Tunisia", "Turkey", "Turkmenistan",
        "Uganda", "Ukraine", "United Arab Emirates", "United Kingdom", "United States", "Uruguay",
        "Uzbekistan", "Vatican City", "Venezuela", "Vietnam", "Yemen", "Zambia", "Zimbabwe"
    };

    regCountryInput->addItems(countries);
    regCountryInput->setStyleSheet(dropDownStyle);

    regCountryInput->setEditable(true); // permitem utilizatorului sa scrie text pentru a cauta
    regCountryInput->setInsertPolicy(QComboBox::NoInsert); // Interzicem adaugarea de tari inventate de user

    // completer -> sugestii inteligente
    QCompleter *completer = new QCompleter(countries, w);
    completer->setCaseSensitivity(Qt::CaseInsensitive); // cauta indiferent de litere mari/mici
    completer->setFilterMode(Qt::MatchContains);        // gasirea textului oriunde in cuvant

    completer->popup()->setStyleSheet(
        "QAbstractItemView { "
        "   background-color: #1e293b; "
        "   color: #f1f5f9; "
        "   selection-background-color: #0ea5e9; "
        "   selection-color: white; "
        "   border: 1px solid #38bdf8; "
        "   border-radius: 8px; "
        "   padding: 5px; "
        "}"
        );

    regCountryInput->setCompleter(completer);

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
