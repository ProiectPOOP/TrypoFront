#include "MainWindow.h"
#include <QCalendarWidget>
#include <QCryptographicHash>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QStatusBar>
#include <QVBoxLayout>
#include "Styles.h"
#include "socketclient.h"
#include <QDialog>
#include <QTextCharFormat>
#include <QFormLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Trypo");
    resize(1000, 800);
    this->setStyleSheet("QMainWindow { background-color: #0f172a; } " + globalLabelStyle);
    userBookings = {{"Retezat Cabin", "Jan 12 - Jan 15, 2026", "Finished"},
                    {"Grand Hotel Trypo", "April 10 - April 12, 2026", "Finished"}};
    cbBalcony = cbFridge = cbAC = cbTV = cbWifi = cbSofa = nullptr;
    m_socketClient = new SocketClient(this);
    m_socketClient->connectToBackend(ip, 12345);
    connect(m_socketClient, &SocketClient::dataReceived, this, &MainWindow::handleBackendMessage);
    setupUi();
}
MainWindow::~MainWindow() {}
void MainWindow::setupUi()
{
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(createLoginWidget());       // 0
    stackedWidget->addWidget(createRegisterWidget());    // 1
    stackedWidget->addWidget(createMainAppWidget());     // 2
    stackedWidget->addWidget(createUserProfileWidget()); // 3
    stackedWidget->addWidget(createDetailsWidget());     // 4
    setCentralWidget(stackedWidget);
}
QString hashPassword(const QString &password)
{
    // Transformăm parola din QString în QByteArray
    QByteArray passwordData = password.toUtf8();

    // Generăm hash-ul folosind algoritmul SHA-256
    QByteArray hashedData = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256);

    // Returnăm hash-ul sub formă de string Hexadecimal (litere și cifre)
    return hashedData.toHex();
}
void MainWindow::processRegister()
{
    QString em = regEmailInput->text().trimmed();
    QString ps = regPasswordInput->text();

    if (regNameInput->text().isEmpty() || em.isEmpty() || ps.isEmpty()) {
        QMessageBox::warning(this, "Error", "Fill all fields.");
        return;
    }
    if (!em.contains("@")) {
        QMessageBox::warning(this, "Error", "Invalid email.");
        return;
    }
    if (ps.length() < 6){
        QMessageBox::warning(this, "Error", "Weak password, it must have minimum 6 characters.");
        return;
    }
    if (!QRegularExpression("[A-Z]").match(ps).hasMatch()){
        QMessageBox::warning(this, "Error", "Weak password, it must have minimum 1 capital letter.");
        return;
    }
    if (!QRegularExpression("[^a-zA-Z0-9]").match(ps).hasMatch()) {
        QMessageBox::warning(this, "Error", "Weak password, it must have minimum one special character.");
        return;
    }
    QString hashedPassword = hashPassword(ps);
    QJsonObject userObj;
    userObj["type"] = "REGISTER_USER";
    userObj["name"] = regNameInput->text().trimmed();
    userObj["email"] = em;
    userObj["password"] = hashedPassword;
    userObj["phone"] = regPhoneInput->text().trimmed();
    userObj["dob"] = regDobInput->date().toString("yyyy-MM-dd"); // Format standard SQL
    userObj["country"] = regCountryInput->currentText();
    userObj["gender"] = regGenderInput->currentText();
    userObj["address"] = regAddressInput->text().trimmed();
    QJsonDocument doc(userObj);
    m_socketClient->sendMessage(doc.toJson(QJsonDocument::Compact));

    qDebug() << "Cerere de înregistrare trimisă pentru:" << em;
    goToLogin();
}

void MainWindow::processLogin()
{
    QString em = loginEmailInput->text().trimmed();
    QString ps = loginPasswordInput->text();
    QString hashedPassword = hashPassword(ps);
    QJsonObject userObj;
    userObj["type"] = "LOGIN_USER";
    userObj["email"] = em;
    userObj["password"] = hashedPassword;
    QJsonDocument doc(userObj);
    m_socketClient->sendMessage(doc.toJson(QJsonDocument::Compact));

    qDebug() << "Cerere de logare trimisă pentru:" << em;
}
QWidget *MainWindow::createLoginWidget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setAlignment(Qt::AlignCenter);
    QLabel *t = new QLabel("Welcome back to Trypo", w);
    t->setStyleSheet("font-size: 28px; font-weight: bold; color: #f8fafc; margin-bottom: 25px;");
    loginEmailInput = new QLineEdit(w);
    loginEmailInput->setPlaceholderText("Email");
    loginEmailInput->setStyleSheet(lineEditStyle);
    loginPasswordInput = new QLineEdit(w);
    loginPasswordInput->setPlaceholderText("Password");
    loginPasswordInput->setEchoMode(QLineEdit::Password);
    loginPasswordInput->setStyleSheet(lineEditStyle);
    QPushButton *lb = new QPushButton("Login", w);
    lb->setStyleSheet(primaryBtnStyle);
    lb->setCursor(Qt::PointingHandCursor);
    QPushButton *rb = new QPushButton("Create account", w);
    rb->setStyleSheet(secondaryBtnStyle);
    rb->setCursor(Qt::PointingHandCursor);
    l->addWidget(t);
    l->addWidget(loginEmailInput);
    l->addWidget(loginPasswordInput);
    l->addSpacing(15);
    l->addWidget(lb);
    l->addWidget(rb);
    connect(lb, &QPushButton::clicked, this, &MainWindow::processLogin);
    connect(rb, &QPushButton::clicked, this, &MainWindow::goToRegister);
    return w;
}

QWidget *MainWindow::createRegisterWidget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(50, 20, 50, 20);
    QLabel *t = new QLabel("Register to Trypo", w);
    t->setStyleSheet("font-size: 26px; font-weight: bold; color: #f8fafc; margin-bottom: 20px;");
    QFormLayout *fl = new QFormLayout();
    fl->setSpacing(12);
    regNameInput = new QLineEdit();
    regNameInput->setStyleSheet(lineEditStyle);
    regEmailInput = new QLineEdit();
    regEmailInput->setStyleSheet(lineEditStyle);
    regPasswordInput = new QLineEdit();
    regPasswordInput->setEchoMode(QLineEdit::Password);
    regPasswordInput->setStyleSheet(lineEditStyle);
    regPhoneInput = new QLineEdit();
    regPhoneInput->setStyleSheet(lineEditStyle);
    regAddressInput = new QLineEdit();
    regAddressInput->setStyleSheet(lineEditStyle);
    regDobInput = new QDateEdit(QDate(2000, 1, 1));
    regDobInput->setCalendarPopup(true);
    regDobInput->setStyleSheet(dropDownStyle);
    // set echo mode -> ascundem parola cand o scriem de la tastatura

    QCalendarWidget *calendar = regDobInput->calendarWidget();
    calendar->setStyleSheet(
        "QCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #0f172a; padding: "
        "4px; }"
        "QCalendarWidget QToolButton { color: white; background-color: transparent; border: none; "
        "font-weight: bold; font-size: 14px; padding: 4px 8px; margin: 0px 2px; }"
        "QCalendarWidget QToolButton:hover { background-color: #2563eb; border-radius: 4px; }"
        "QCalendarWidget QMenu { background-color: #1e293b; color: white; border: 1px solid "
        "#475569; }"
        "QCalendarWidget QSpinBox { background-color: #1e293b; color: white; border: 1px solid "
        "#475569; border-radius: 3px; padding: 2px 4px; min-width: 65px; margin-left: 10px; }"
        "QCalendarWidget QSpinBox::up-button, QCalendarWidget QSpinBox::down-button { width: 16px; "
        "}"
        "QCalendarWidget QAbstractItemView:enabled { background-color: #1e293b; color: white; "
        "selection-background-color: #2563eb; selection-color: white; border: none; outline: none; "
        "}"
        "QCalendarWidget QAbstractItemView:disabled { color: #475569; }");

    regCountryInput = new QComboBox();
    regCountryInput->addItems({"Romania", "Moldova", "UK", "Germany"});
    regCountryInput->setStyleSheet(dropDownStyle);
    regGenderInput = new QComboBox();
    regGenderInput->addItems({"Male", "Female", "Unspecified"});
    regGenderInput->setStyleSheet(dropDownStyle);
    QString ls = "font-weight: bold; color: #94a3b8; font-size: 14px;";
    auto addR = [&](QString txt, QWidget *in) {
        QLabel *l = new QLabel(txt);
        l->setStyleSheet(ls);
        fl->addRow(l, in);
    };
    addR("Full Name:", regNameInput);
    addR("Email:", regEmailInput);
    addR("Password:", regPasswordInput);
    addR("Phone:", regPhoneInput);
    addR("Date of Birth:", regDobInput);
    addR("Country:", regCountryInput);
    addR("Gender:", regGenderInput);
    addR("Address:", regAddressInput);
    ml->addWidget(t);
    ml->addLayout(fl);
    QPushButton *rb = new QPushButton("Register Now", w);
    rb->setStyleSheet(primaryBtnStyle);
    QPushButton *bb = new QPushButton("Back to Login", w);
    bb->setStyleSheet(secondaryBtnStyle);
    ml->addSpacing(20);
    ml->addWidget(rb);
    ml->addWidget(bb);
    connect(rb, &QPushButton::clicked, this, &MainWindow::processRegister);
    connect(bb, &QPushButton::clicked, this, &MainWindow::goToLogin);
    return w;
}

QWidget *MainWindow::createMainAppWidget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(20, 20, 20, 20);
    QHBoxLayout *hl = new QHBoxLayout();
    QLabel *t = new QLabel("Recommended for you");
    t->setStyleSheet("font-size: 22px; font-weight: bold; color: #f8fafc;");
    searchBarInput = new QLineEdit();
    searchBarInput->setPlaceholderText("Where do you want to go?");
    searchBarInput->setStyleSheet(lineEditStyle);
    connect(searchBarInput, &QLineEdit::textChanged, this, &MainWindow::filterAccommodations);
    QPushButton *pb = new QPushButton("👤 My Profile");
    pb->setStyleSheet(secondaryBtnStyle);
    pb->setCursor(Qt::PointingHandCursor);
    QPushButton *lb = new QPushButton("Logout");
    lb->setStyleSheet(dangerBtnStyle);
    hl->addWidget(t);
    hl->addStretch();
    hl->addWidget(searchBarInput);
    hl->addWidget(pb);
    hl->addWidget(lb);
    ml->addLayout(hl);

    QScrollArea *sa = new QScrollArea();
    sa->setWidgetResizable(true);
    sa->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    accommodationsContainer = new QWidget();
    accommodationsLayout = new QVBoxLayout(accommodationsContainer);
    accommodationsLayout->setAlignment(Qt::AlignTop);
    sa->setWidget(accommodationsContainer);
    ml->addWidget(sa);
    QJsonObject obj;
    obj["type"] = "GET_ACCOMMODATIONS";
    m_socketClient->sendMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    // legatura intre butoane si sloturi (functii)
    connect(pb, &QPushButton::clicked, this, &MainWindow::goToUserProfile);
    connect(lb, &QPushButton::clicked, this, &MainWindow::goToLogin);
    return w;
}

QWidget *MainWindow::createUserProfileWidget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(40, 30, 40, 30);
    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *bb = new QPushButton("← Back to Explore");
    bb->setStyleSheet(secondaryBtnStyle);
    connect(bb, &QPushButton::clicked, this, &MainWindow::backToMainApp);
    QLabel *pt = new QLabel("Account Settings");
    pt->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    h->addWidget(bb);
    h->addSpacing(20);
    h->addWidget(pt);
    h->addStretch();
    ml->addLayout(h);
    ml->addSpacing(30);
    QLabel *sub1 = new QLabel("Customer Information");
    sub1->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    ml->addWidget(sub1);
    QWidget *detW = new QWidget();
    QFormLayout *f = new QFormLayout(detW);
    f->setContentsMargins(0, 15, 0, 30);
    f->setVerticalSpacing(15);
    QString vs = "color: white; font-size: 15px;";
    lblNameVal = new QLabel("N/A");
    lblNameVal->setStyleSheet(vs);
    lblEmailVal = new QLabel("N/A");
    lblEmailVal->setStyleSheet(vs);
    lblPhoneVal = new QLabel("N/A");
    lblPhoneVal->setStyleSheet(vs);
    lblDobVal = new QLabel("N/A");
    lblDobVal->setStyleSheet(vs);
    lblCountryVal = new QLabel("N/A");
    lblCountryVal->setStyleSheet(vs);
    lblGenderVal = new QLabel("N/A");
    lblGenderVal->setStyleSheet(vs);
    lblAddressVal = new QLabel("N/A");
    lblAddressVal->setStyleSheet(vs);
    lblBalanceVal = new QLabel("0.00 €");
    lblBalanceVal->setStyleSheet("color: #22c55e; font-size: 16px; font-weight: bold;");

    f->addRow(new QLabel("Name:"), lblNameVal);
    f->addRow(new QLabel("Email:"), lblEmailVal);
    f->addRow(new QLabel("Phone:"), lblPhoneVal);
    f->addRow(new QLabel("Birth Date:"), lblDobVal);
    f->addRow(new QLabel("Country:"), lblCountryVal);
    f->addRow(new QLabel("Gender:"), lblGenderVal);
    f->addRow(new QLabel("Address:"), lblAddressVal);
    f->addRow(new QLabel("Current Balance:"), lblBalanceVal);
    ml->addWidget(detW);
    QLabel *sub2 = new QLabel("Your Booking History");
    sub2->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    ml->addWidget(sub2);

    QScrollArea *hsa = new QScrollArea();
    hsa->setWidgetResizable(true);
    hsa->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget *hc = new QWidget();
    historyLayout = new QVBoxLayout(hc); // <--- MODIFICARE: Alocăm pointerul din clasa de bază
    historyLayout->setAlignment(Qt::AlignTop);

    // --- NOTĂ: Bucla 'for' veche a fost mutată în funcția de mai jos ---
    updateBookingHistoryUi(); // Apel inițial pentru a încărca rezervările hardcodate
    // ------------------------------------------------------------------

    hsa->setWidget(hc);
    ml->addWidget(hsa);
    return w;
}

QWidget *MainWindow::createDetailsWidget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(30, 30, 30, 30);
    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *bb = new QPushButton("← Back");
    bb->setStyleSheet(secondaryBtnStyle);
    connect(bb, &QPushButton::clicked, this, &MainWindow::backToMainApp);
    detName = new QLabel("Hotel Details");
    detName->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    h->addWidget(bb);
    h->addSpacing(20);
    h->addWidget(detName);
    h->addStretch();
    ml->addLayout(h);
    detAddress = new QLabel("");
    detAddress->setStyleSheet("color: #94a3b8; font-size: 14px; margin-left: 85px;");
    ml->addWidget(detAddress);
    detPromo = new QLabel("");
    ml->addWidget(detPromo);

    QHBoxLayout *filtersLayout = new QHBoxLayout();
    QLabel *filterLbl = new QLabel("Facilities:");
    filterLbl->setStyleSheet("color: #94a3b8; font-weight: bold; font-size: 14px;");
    filtersLayout->addWidget(filterLbl);

    cbBalcony = new QCheckBox("Balcony");
    cbBalcony->setStyleSheet(checkBoxStyle);
    cbFridge = new QCheckBox("Fridge");
    cbFridge->setStyleSheet(checkBoxStyle);
    cbAC = new QCheckBox("AC");
    cbAC->setStyleSheet(checkBoxStyle);
    cbTV = new QCheckBox("TV");
    cbTV->setStyleSheet(checkBoxStyle);
    cbWifi = new QCheckBox("WiFi");
    cbWifi->setStyleSheet(checkBoxStyle);
    cbSofa = new QCheckBox("Sofa");
    cbSofa->setStyleSheet(checkBoxStyle);

    filtersLayout->addWidget(cbBalcony);
    filtersLayout->addWidget(cbFridge);
    filtersLayout->addWidget(cbAC);
    filtersLayout->addWidget(cbTV);
    filtersLayout->addWidget(cbWifi);
    filtersLayout->addWidget(cbSofa);
    filtersLayout->addStretch();

    ml->addSpacing(10);
    ml->addLayout(filtersLayout);

    auto updateRoomFilters = [this]() { displayRooms(roomSearchBar->text()); };
    connect(cbBalcony, &QCheckBox::checkStateChanged, this, updateRoomFilters);
    connect(cbFridge, &QCheckBox::checkStateChanged, this, updateRoomFilters);
    connect(cbAC, &QCheckBox::checkStateChanged, this, updateRoomFilters);
    connect(cbTV, &QCheckBox::checkStateChanged, this, updateRoomFilters);
    connect(cbWifi, &QCheckBox::checkStateChanged, this, updateRoomFilters);
    connect(cbSofa, &QCheckBox::checkStateChanged, this, updateRoomFilters);

    roomSearchBar = new QLineEdit();
    roomSearchBar->setPlaceholderText("Filter rooms by text...");
    roomSearchBar->setStyleSheet(lineEditStyle);
    connect(roomSearchBar, &QLineEdit::textChanged, this, &MainWindow::filterRooms);
    ml->addSpacing(10);
    ml->addWidget(roomSearchBar);
    ml->addSpacing(20);
    QLabel *roomT = new QLabel("Available Rooms");
    roomT->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    ml->addWidget(roomT);
    QScrollArea *sa = new QScrollArea();
    sa->setWidgetResizable(true);
    sa->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    QWidget *rc = new QWidget();
    roomsLayout = new QVBoxLayout(rc);
    roomsLayout->setAlignment(Qt::AlignTop);
    sa->setWidget(rc);
    ml->addWidget(sa);
    return w;
}

void MainWindow::openAccommodationDetails(const Accommodation &acc)
{
    currentAccommodationInDetails = acc;
    detName->setText(acc.name);
    detAddress->setText("📍 " + acc.address + ", " + acc.location);
    if (roomSearchBar)
        roomSearchBar->clear();
    if (cbBalcony)
        cbBalcony->setChecked(false);
    if (cbFridge)
        cbFridge->setChecked(false);
    if (cbAC)
        cbAC->setChecked(false);
    if (cbTV)
        cbTV->setChecked(false);
    if (cbWifi)
        cbWifi->setChecked(false);
    if (cbSofa)
        cbSofa->setChecked(false);

    if (acc.discountPercent > 0) {
        detPromo->setText("🔥 Special Offer: " + acc.promoName + " ("
                          + QString::number(acc.discountPercent * 100) + "% OFF!)");
        detPromo->setStyleSheet(
            "color: #f59e0b; font-weight: bold; margin-left: 85px; font-size: 14px;");
        detPromo->show();
    } else {
        detPromo->hide();
    }
    displayRooms("");
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::filterRooms(const QString &query)
{
    displayRooms(query);
}

void MainWindow::displayRooms(const QString &f)
{
    QLayoutItem *child;
    while ((child = roomsLayout->takeAt(0)) != nullptr) {
        if (child->widget())
            delete child->widget();
        delete child;
    }

    bool reqBalcony = cbBalcony && cbBalcony->isChecked();
    bool reqFridge = cbFridge && cbFridge->isChecked();
    bool reqAC = cbAC && cbAC->isChecked();
    bool reqTV = cbTV && cbTV->isChecked();
    bool reqWifi = cbWifi && cbWifi->isChecked();
    bool reqSofa = cbSofa && cbSofa->isChecked();

    for (const auto &room : currentAccommodationInDetails.rooms) {
        bool matchesType = room.type.contains(f, Qt::CaseInsensitive);
        bool matchesFac = false;
        bool rBalcony = false, rFridge = false, rAC = false, rTV = false, rWifi = false;

        for (const auto &fac : room.facilities) {
            if (fac.name.contains(f, Qt::CaseInsensitive)) {
                matchesFac = true;
            }
            if (fac.name == "Balcony")
                rBalcony = true;
            if (fac.name == "Fridge")
                rFridge = true;
            if (fac.name == "AC")
                rAC = true;
            if (fac.name == "TV")
                rTV = true;
            if (fac.name == "WiFi")
                rWifi = true;
        }

        if (!f.isEmpty() && !matchesType && !matchesFac)
            continue;
        if (reqBalcony && !rBalcony)
            continue;
        if (reqFridge && !rFridge)
            continue;
        if (reqAC && !rAC)
            continue;
        if (reqTV && !rTV)
            continue;
        if (reqWifi && !rWifi)
            continue;
        if (reqSofa && !room.hasSofa)
            continue;

        QFrame *fr = new QFrame();
        fr->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 10px; border: 1px "
                          "solid #334155; }");
        QHBoxLayout *l = new QHBoxLayout(fr);
        l->setContentsMargins(15, 15, 15, 15);
        QVBoxLayout *inf = new QVBoxLayout();
        QLabel *typeL = new QLabel(room.type);
        typeL->setStyleSheet("font-size: 18px; font-weight: bold; color: white; border: none; "
                             "background: transparent;");
        int totalCap = room.beds + (room.hasSofa ? 1 : 0);
        QLabel *capL = new QLabel(
            QString("🛏️ %1 Beds | 👥 Max Capacity: %2 people").arg(room.beds).arg(totalCap));
        capL->setStyleSheet("color: #94a3b8; border: none; background: transparent;");
        QString flist = "Facilities: ";
        for (const auto &fac : room.facilities) {
            flist += fac.name + ", ";
        }
        QLabel *facL = new QLabel(flist.left(flist.length() - 2));
        facL->setStyleSheet(
            "color: #64748b; font-size: 12px; border: none; background: transparent;");
        inf->addWidget(typeL);
        inf->addWidget(capL);
        inf->addWidget(facL);
        double finalPrice = room.basePrice * (1.0 - currentAccommodationInDetails.discountPercent);
        QLabel *pL = new QLabel(QString::number(finalPrice) + "€ / night");
        pL->setStyleSheet("color: #22c55e; font-size: 18px; font-weight: bold; border: none; "
                          "background: transparent;");
        l->addLayout(inf);
        l->addStretch();
        l->addWidget(pL);

        // --- COLOANA DIN DREAPTA: PREȚ ȘI BUTON REZERVARE (HARDCODAT) ---
        QVBoxLayout *rightLayout = new QVBoxLayout();
        rightLayout->setAlignment(Qt::AlignCenter);

        rightLayout->addWidget(pL); // Adăugăm eticheta de preț existentă

        QPushButton *btnBook = new QPushButton("Book Now");
        btnBook->setStyleSheet(primaryBtnStyle);
        btnBook->setCursor(Qt::PointingHandCursor);

        // Conectăm butonul la funcția bookRoom transmițând ID-ul camerei
        connect(btnBook, &QPushButton::clicked, this, [this, room]() { this->bookRoom(room.id); });

        rightLayout->addWidget(btnBook);
        l->addLayout(rightLayout);
        // ----------------------------------------------------------------


        roomsLayout->addWidget(fr);

    }
}

void MainWindow::populateAccommodations(const QString &f)
{
    qDebug() << "Populating UI. Total accommodations in vector:" << allAccommodations.size();
    QLayoutItem *c;

    while ((c = accommodationsLayout->takeAt(0)) != nullptr) {
        if (c->widget())
            delete c->widget();
        delete c;
    }
    for (const auto &acc : allAccommodations) {
        if (!f.isEmpty() && !acc.name.contains(f, Qt::CaseInsensitive)
            && !acc.location.contains(f, Qt::CaseInsensitive))
            continue;
        QFrame *card = new QFrame();
        card->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 12px; }");
        QHBoxLayout *l = new QHBoxLayout(card);
        l->setContentsMargins(20, 20, 20, 20);
        QVBoxLayout *inf = new QVBoxLayout();
        QLabel *nL = new QLabel(acc.name);
        nL->setStyleSheet("color: white; font-size: 18px; font-weight: bold; border: none; "
                          "background: transparent;");
        QLabel *lL = new QLabel("📍 " + acc.location);
        lL->setStyleSheet(
            "color: #94a3b8; font-size: 14px; border: none; background: transparent;");
        inf->addWidget(nL);
        inf->addWidget(lL);
        QPushButton *btn = new QPushButton("View Details");
        btn->setStyleSheet(primaryBtnStyle);
        connect(btn, &QPushButton::clicked, this, [this, acc]() { openAccommodationDetails(acc); });
        l->addLayout(inf);
        l->addStretch();
        l->addWidget(btn);
        accommodationsLayout->addWidget(card);
    }
}

void MainWindow::goToRegister()
{
    stackedWidget->setCurrentIndex(1);
}
void MainWindow::goToLogin()
{
    // Trimitem FORCE_LOGOUT doar dacă utilizatorul vine din aplicația principală (pagina 2) sau profil (pagina 3)
    // Dacă vine de la Register (pagina 1), ignorăm trimiterea pachetului către server
    if (stackedWidget->currentIndex() == 2 || stackedWidget->currentIndex() == 3) {
        QJsonObject req;
        req["type"] = "FORCE_LOGOUT";
        req["email"] = currentUser.email;
        m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
    }

    // Golește câmpurile de la Register (nume, email, telefon etc.)
    clearRegisterFields();

    // Golește câmpurile de la LOGIN
    if (loginEmailInput)
        loginEmailInput->clear();
    if (loginPasswordInput)
        loginPasswordInput->clear();

    // Golește bara de căutare din aplicația principală
    if (searchBarInput)
        searchBarInput->clear();

    // Curăță lista de hoteluri din memorie și de pe ecran
    allAccommodations.clear();
    populateAccommodations("");

    // Ne întoarcem în siguranță la ecranul de Login (pagina 0) cu toate câmpurile curate
    stackedWidget->setCurrentIndex(0);
}
void MainWindow::goToUserProfile()
{
    updateBookingHistoryUi();
    stackedWidget->setCurrentIndex(3);
}
void MainWindow::backToMainApp()
{
    stackedWidget->setCurrentIndex(2);
}
void MainWindow::filterAccommodations(const QString &q)
{
    populateAccommodations(q);
}

void MainWindow::clearRegisterFields()
{
    regNameInput->clear();
    regEmailInput->clear();
    regPasswordInput->clear();
    regPhoneInput->clear();
    regAddressInput->clear();
    regDobInput->setDate(QDate(2000, 1, 1));
    regCountryInput->setCurrentIndex(0);
    regGenderInput->setCurrentIndex(0);
}
void MainWindow::handleBackendMessage(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isNull())
        return;
    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();
    if (type == "REGISTER_RESPONSE") {
        QString status = obj["status"].toString();
        QString serverMsg = obj["message"].toString();
        if (status == "success") {
            QMessageBox::information(this, "Succes", serverMsg);
            stackedWidget->setCurrentIndex(0); // Mergi la Login
        } else {
            QMessageBox::critical(this, "Eroare Inregistrare", serverMsg);
        }
    }
    if (type == "LOGIN_RESPONSE") {
        QString status = obj["status"].toString();

        if (status == "success") {
            QJsonObject userData = obj["data"].toObject();

            currentUser.name = userData["name"].toString();
            currentUser.email = userData["email"].toString();
            currentUser.phone = userData["phone"].toString();
            currentUser.address = userData["address"].toString();
            currentUser.country = userData["country"].toString();
            currentUser.dob = userData["dob"].toString();
            currentUser.gender = userData["gender"].toString();
            currentUser.balance = userData["balance"].toDouble(0.0);

            lblNameVal->setText(currentUser.name);
            lblEmailVal->setText(currentUser.email);
            lblPhoneVal->setText(currentUser.phone);
            lblDobVal->setText(currentUser.dob);
            lblCountryVal->setText(currentUser.country);
            lblGenderVal->setText(currentUser.gender);
            lblAddressVal->setText(currentUser.address);
            lblBalanceVal->setText(QString::number(currentUser.balance, 'f', 2) + " €");

            QJsonObject req;
            req["type"] = "GET_ACCOMMODATIONS";
            m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
            stackedWidget->setCurrentIndex(2);
        } else {
            QString serverMsg = obj["message"].toString();
            QMessageBox::warning(this, "Eroare Login", serverMsg);
        }
    }
    if (type == "GET_ACCOMMODATIONS") {
        QJsonArray dataArray = obj["data"].toArray();
        qDebug() << "Update: QList<Accommodation> are acum" << dataArray;

        allAccommodations.clear();

        for (const QJsonValue &value : dataArray) {
            QJsonObject accObj = value.toObject();

            Accommodation acc;
            acc.id = accObj["id"].toInt();
            acc.name = accObj["name"].toString();
            acc.location = accObj["location"].toString();
            acc.address = accObj["address"].toString();

            acc.discountPercent = accObj["discount"].toDouble(0.0);
            acc.promoName = accObj["promo_name"].toString("");

            if (accObj.contains("rooms") && accObj["rooms"].isArray()) {
                QJsonArray roomsArray = accObj["rooms"].toArray();
                for (const QJsonValue &rVal : roomsArray) {
                    QJsonObject rObj = rVal.toObject();

                    Room r;
                    r.id = rObj["id"].toInt();
                    r.type = rObj["type"].toString();
                    r.basePrice = rObj["price"].toDouble();
                    r.beds = rObj["beds"].toInt();
                    r.hasSofa = rObj["hasSofa"].toBool(false);
                    if (rObj.contains("facilities") && rObj["facilities"].isArray()) {
                        QJsonArray facArray = rObj["facilities"].toArray();

                        for (const QJsonValue &fValue : facArray) {
                            Facility fac;

                            if (fValue.isString()) {
                                fac.name = fValue.toString();
                            }

                            else if (fValue.isObject()) {
                                fac.name = fValue.toObject()["name"].toString();
                            }

                            r.facilities.append(fac);
                        }
                    }
                    acc.rooms.append(r);
                }
            }

            allAccommodations.append(acc);
        }

        qDebug() << "Update: QList<Accommodation> are acum" << allAccommodations.size()
                 << "elemente.";
        populateAccommodations("");
    }
    if (message == "FORCE_LOGOUT") {
        allAccommodations.clear();
        goToLogin();
    }
}

void MainWindow::bookRoom(int roomId)
{
    // 1. Găsim camera în memorie
    Room *selectedRoom = nullptr;
    for (auto &r : currentAccommodationInDetails.rooms) {
        if (r.id == roomId) { selectedRoom = &r; break; }
    }
    if (!selectedRoom) return;

    // Simulare date ocupate (Demo hardcodat la final de lună pentru test cross-month)
    if (selectedRoom->bookedDates.isEmpty()) {
        // Simulăm că zilele de 28 și 29 ale lunii curente sunt deja ocupate
        QDate target = QDate::currentDate();
        selectedRoom->bookedDates.append(QDate(target.year(), target.month(), 28));
        selectedRoom->bookedDates.append(QDate(target.year(), target.month(), 29));
    }

    // 2. Construim fereastra dialog pop-up
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Book Room - " + selectedRoom->type);
    dialog->resize(470, 580);
    dialog->setStyleSheet("QDialog { background-color: #0f172a; }");
    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QLabel *title = new QLabel("Select Booking Period", dialog);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: white; margin-bottom: 5px;");
    layout->addWidget(title);

    QLabel *statusLbl = new QLabel("Next action: Select CHECK-IN date", dialog);
    statusLbl->setStyleSheet("color: #3b82f6; font-weight: bold; font-size: 13px; margin-bottom: 5px;");
    layout->addWidget(statusLbl);

    // 3. Inițializare Calendar
    QCalendarWidget *calendar = new QCalendarWidget(dialog);
    calendar->setGridVisible(true);
    calendar->setStyleSheet(
        "QCalendarWidget QWidget { color: #f8fafc; background-color: #1e293b; }"
        "QCalendarWidget QAbstractItemView:enabled { background-color: #1e293b; selection-background-color: #2563eb; }"
        );
    layout->addWidget(calendar);

    // Formate de culori
    QTextCharFormat availableFormat, unavailableFormat, previewFormat;
    availableFormat.setBackground(QColor("#16a34a")); availableFormat.setForeground(Qt::white);   // Verde
    unavailableFormat.setBackground(QColor("#dc2626")); unavailableFormat.setForeground(Qt::white); // Roșu
    previewFormat.setBackground(QColor("#f59e0b"));     previewFormat.setForeground(Qt::white);   // Portocaliu (Selecție curentă)

    QFormLayout *formLayout = new QFormLayout();
    QDateEdit *checkInEdit = new QDateEdit(QDate::currentDate());
    checkInEdit->setCalendarPopup(true); checkInEdit->setStyleSheet(dropDownStyle);
    QDateEdit *checkOutEdit = new QDateEdit(QDate::currentDate().addDays(1));
    checkOutEdit->setCalendarPopup(true); checkOutEdit->setStyleSheet(dropDownStyle);

    formLayout->addRow(new QLabel("Check-in Date:"), checkInEdit);
    formLayout->addRow(new QLabel("Check-out Date:"), checkOutEdit);
    layout->addLayout(formLayout);

    // Starea selecției (true = se așteaptă check-in, false = se așteaptă checkout)
    auto isSelectingCheckIn = std::make_shared<bool>(true);
    auto hasFinalSelection = std::make_shared<bool>(false);

    // --- FUNCTIA LAMBDA DE REFRESH GRAFIC AL CALENDARULUI ---
    // Curăță și redesenează totul corect, indiferent de lună sau an
    auto refreshCalendarColors = [=]() {
        QDate base = QDate::currentDate();
        // Resetăm preventiv un interval larg de 365 de zile cu Verde (Disponibil)
        for (int i = -30; i < 335; ++i) {
            calendar->setDateTextFormat(base.addDays(i), availableFormat);
        }

        // Aplicăm Roșu strict pentru zilele salvate în vectorul de rezervări ocupate
        for (const QDate &bookedDate : selectedRoom->bookedDates) {
            calendar->setDateTextFormat(bookedDate, unavailableFormat);
        }

        // Dacă avem o selecție activă în curs, colorăm intervalul cu Portocaliu (Live Preview cross-month)
        if (!(*isSelectingCheckIn) || *hasFinalSelection) {
            QDate startSel = checkInEdit->date();
            QDate endSel = checkOutEdit->date();
            for (QDate d = startSel; d <= endSel; d = d.addDays(1)) {
                calendar->setDateTextFormat(d, previewFormat);
            }
        }
    };

    // Apelăm refresh-ul inițial ca să vedem zonele roșii din start
    refreshCalendarColors();

    // Forțăm reîmprospătarea culorilor când utilizatorul schimbă luna din săgeți
    connect(calendar, &QCalendarWidget::currentPageChanged, dialog, [=](int, int) {
        refreshCalendarColors();
    });

    // --- LOGICĂ CLICK INTELIGENTĂ (AICI SE REZOLVĂ BUG-UL) ---
    connect(calendar, &QCalendarWidget::clicked, this, [=](const QDate &date) {
        if (date < QDate::currentDate()) {
            QMessageBox::warning(dialog, "Error", "Cannot select past dates.");
            return;
        }

        if (selectedRoom->bookedDates.contains(date)) {
            QMessageBox::warning(dialog, "Date Unavailable", "This day is already booked! Please select a green day.");
            *isSelectingCheckIn = true;
            *hasFinalSelection = false;
            statusLbl->setText("Next action: Select CHECK-IN date");
            statusLbl->setStyleSheet("color: #3b82f6; font-weight: bold;");
            refreshCalendarColors();
            return;
        }

        if (*isSelectingCheckIn) {
            checkInEdit->setDate(date);
            checkOutEdit->setDate(date.addDays(1));
            statusLbl->setText("Next action: Select CHECK-OUT date");
            statusLbl->setStyleSheet("color: #f59e0b; font-weight: bold;");
            *isSelectingCheckIn = false;
            *hasFinalSelection = false;
        } else {
            if (date > checkInEdit->date()) {
                // Verificăm dacă între Check-in-ul curent și noul click există nopți ocupate (roșii)
                bool hasBlockedNight = false;
                for (QDate d = checkInEdit->date(); d < date; d = d.addDays(1)) {
                    if (selectedRoom->bookedDates.contains(d)) {
                        hasBlockedNight = true;
                        break;
                    }
                }

                if (hasBlockedNight) {
                    // Dacă a sărit peste o zonă roșie, mutăm noul Check-In aici
                    checkInEdit->setDate(date);
                    checkOutEdit->setDate(date.addDays(1));
                    statusLbl->setText("Jumped over a blocked zone. New CHECK-IN set here!");
                    statusLbl->setStyleSheet("color: #3b82f6; font-weight: bold;");
                    *isSelectingCheckIn = false;
                    *hasFinalSelection = false;
                } else {
                    // Perioada este validă (chiar dacă trece în luna următoare!)
                    checkOutEdit->setDate(date);
                    statusLbl->setText("Period Selected! Click again to reset Check-in.");
                    statusLbl->setStyleSheet("color: #22c55e; font-weight: bold;");
                    *isSelectingCheckIn = true;
                    *hasFinalSelection = true; // Confirmă că avem un interval complet definit vizual
                }
            } else {
                checkInEdit->setDate(date);
                checkOutEdit->setDate(date.addDays(1));
                statusLbl->setText("Next action: Select CHECK-OUT date");
                statusLbl->setStyleSheet("color: #f59e0b; font-weight: bold;");
                *isSelectingCheckIn = false;
                *hasFinalSelection = false;
            }
        }

        // Rulăm refresh-ul grafic după fiecare click pentru a redesena Preview-ul portocaliu cross-month
        refreshCalendarColors();
    });

    QPushButton *btnConfirm = new QPushButton("Confirm Reservation", dialog);
    btnConfirm->setStyleSheet(primaryBtnStyle);
    layout->addWidget(btnConfirm);

    // --- SALVARE REZERVARE ---
    connect(btnConfirm, &QPushButton::clicked, this, [=]() {
        QDate start = checkInEdit->date();
        QDate end = checkOutEdit->date();

        if (start >= end) {
            QMessageBox::warning(dialog, "Error", "Check-out date must be after Check-in date.");
            return;
        }

        // Adăugăm nopțile rezervate în baza locală (până la ziua de checkout, excluzând-o, conform standardului hotelier)
        for (QDate d = start; d < end; d = d.addDays(1)) {
            selectedRoom->bookedDates.append(d);
        }

        // Sincronizare în structura globală
        for (auto &acc : allAccommodations) {
            for (auto &r : acc.rooms) {
                if (r.id == roomId) {
                    for (QDate d = start; d < end; d = d.addDays(1)) r.bookedDates.append(d);
                }
            }
        }

        // Adăugare în istoric profil
        BookingHistory newBooking;
        newBooking.hotelName = currentAccommodationInDetails.name + " (" + selectedRoom->type + ")";
        newBooking.dateRange = start.toString("MMM dd") + " - " + end.toString("MMM dd, yyyy");
        newBooking.status = "Upcoming";
        userBookings.append(newBooking);

        QMessageBox::information(dialog, "Success", "Reservation completed successfully!");
        dialog->accept();

        displayRooms(roomSearchBar ? roomSearchBar->text() : "");
    });

    dialog->exec();
}

void MainWindow::updateBookingHistoryUi()
{
    if (!historyLayout) return;

    // Curățăm elementele vizuale vechi din layout ca să nu se suprapună
    QLayoutItem *child;
    while ((child = historyLayout->takeAt(0)) != nullptr) {
        if (child->widget())
            delete child->widget();
        delete child;
    }

    // Desenăm cardurile din vectorul actualizat userBookings
    for (const auto &b : userBookings) {
        QFrame *fr = new QFrame();
        fr->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 8px; border: 1px "
                          "solid #334155; padding: 5px; }");
        QHBoxLayout *cl = new QHBoxLayout(fr);
        QVBoxLayout *inf = new QVBoxLayout();

        QLabel *hN = new QLabel(b.hotelName);
        hN->setStyleSheet("font-weight: bold; color: white; border: none; background: transparent;");
        QLabel *hD = new QLabel(b.dateRange);
        hD->setStyleSheet("color: #94a3b8; font-size: 12px; border: none; background: transparent;");

        inf->addWidget(hN);
        inf->addWidget(hD);

        QLabel *st = new QLabel(b.status);
        st->setStyleSheet(b.status == "Finished"
                              ? "color:#22c55e; border: none; background: transparent;"
                              : "color:#3b82f6; border: none; background: transparent;");
        cl->addLayout(inf);
        cl->addStretch();
        cl->addWidget(st);
        historyLayout->addWidget(fr);
    }
}