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
    stackedWidget->addWidget(createAdminDashboardWidget()); // 5 <--- ADAUGARE PAGINA ADMIN
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

    // --- ADAUGARE: VERIFICARE CONT ADMIN HARDCODAT ---
    if (em == "admin@trypo.com" && ps == "Admin123!") {
        qDebug() << "Admin logged in successfully!";
        updateAdminDashboardUi(); // Încărcăm istoricul proaspăt
        stackedWidget->setCurrentIndex(5); // Mergem la pagina de Admin
        return; // Oprim funcția aici, nu mai trimitem pachetul la server
    }
    // -------------------------------------------------

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

    // --- HEADER-UL DE SUS ---
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

    // --- SPLIT-SCREEN LAYOUT ---
    QHBoxLayout *splitLayout = new QHBoxLayout();
    splitLayout->setSpacing(50);

    // 1. COLOANA STÂNGĂ: Informații Client
    QVBoxLayout *leftColumn = new QVBoxLayout();
    leftColumn->setAlignment(Qt::AlignTop);

    QLabel *sub1 = new QLabel("Customer Information");
    sub1->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    leftColumn->addWidget(sub1);

    QWidget *detW = new QWidget();
    QFormLayout *f = new QFormLayout(detW);
    // MODIFICARE: Am redus marginea de sus de la 15 la 5 ca să aducem textul mai aproape de titlu
    f->setContentsMargins(0, 5, 0, 30);
    f->setVerticalSpacing(15);

    QString vs = "color: white; font-size: 15px;";
    lblNameVal = new QLabel("N/A");    lblNameVal->setStyleSheet(vs);
    lblEmailVal = new QLabel("N/A");   lblEmailVal->setStyleSheet(vs);
    lblPhoneVal = new QLabel("N/A");   lblPhoneVal->setStyleSheet(vs);
    lblDobVal = new QLabel("N/A");     lblDobVal->setStyleSheet(vs);
    lblCountryVal = new QLabel("N/A"); lblCountryVal->setStyleSheet(vs);
    lblGenderVal = new QLabel("N/A");  lblGenderVal->setStyleSheet(vs);
    lblAddressVal = new QLabel("N/A"); lblAddressVal->setStyleSheet(vs);

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

    leftColumn->addWidget(detW);

    // 2. COLOANA DREAPTĂ: Istoric Rezervări
    QVBoxLayout *rightColumn = new QVBoxLayout();
    rightColumn->setAlignment(Qt::AlignTop);

    QLabel *sub2 = new QLabel("Your Booking History");
    sub2->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    rightColumn->addWidget(sub2);
    // MODIFICARE: Am micșorat spațiul dintre titlu și listă de la 15 la 5
    rightColumn->addSpacing(5);

    QScrollArea *hsa = new QScrollArea();
    hsa->setWidgetResizable(true);
    hsa->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget *hc = new QWidget();
    historyLayout = new QVBoxLayout(hc);
    historyLayout->setAlignment(Qt::AlignTop);

    updateBookingHistoryUi();

    hsa->setWidget(hc);
    rightColumn->addWidget(hsa);

    // Asamblare coloane (50% / 50%)
    splitLayout->addLayout(leftColumn, 1);
    splitLayout->addLayout(rightColumn, 1);

    ml->addLayout(splitLayout);

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

        // --- REZOLVARE BUG: Capturăm doar ID-ul, nu întreaga structură prin valoare ---
        int targetId = acc.id;
        connect(btn, &QPushButton::clicked, this, [this, targetId]() {
            // Căutăm în vectorul global varianta proaspătă a cazării (care conține noile nopți roșii)
            for (const auto &globalAcc : allAccommodations) {
                if (globalAcc.id == targetId) {
                    openAccommodationDetails(globalAcc);
                    break;
                }
            }
        });
        // -----------------------------------------------------------------------------

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
            //currentUser.balance = userData["balance"].toDouble(0.0);
            currentUser.balance = 99999.0; // balanta hardcodata pt teste

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

    // Simulare date ocupate (Demo)
    if (selectedRoom->bookedDates.isEmpty()) {
        QDate target = QDate::currentDate();
        selectedRoom->bookedDates.append(QDate(target.year(), target.month(), 28));
        selectedRoom->bookedDates.append(QDate(target.year(), target.month(), 29));
    }

    // 2. Construim fereastra dialog pop-up principală a calendarului
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
    QTextCharFormat availableFormat, unavailableFormat, previewFormat, pastFormat;
    availableFormat.setBackground(QColor("#16a34a")); availableFormat.setForeground(Qt::white);   // Verde
    unavailableFormat.setBackground(QColor("#dc2626")); unavailableFormat.setForeground(Qt::white); // Roșu
    previewFormat.setBackground(QColor("#f59e0b"));     previewFormat.setForeground(Qt::white);   // Portocaliu
    pastFormat.setBackground(QColor("#334155"));        pastFormat.setForeground(QColor("#64748b")); // Gri

    QDateEdit *checkInEdit = new QDateEdit(QDate::currentDate());
    checkInEdit->setCalendarPopup(true); checkInEdit->setStyleSheet(dropDownStyle);
    QDateEdit *checkOutEdit = new QDateEdit(QDate::currentDate().addDays(1));
    checkOutEdit->setCalendarPopup(true); checkOutEdit->setStyleSheet(dropDownStyle);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow(new QLabel("Check-in Date:"), checkInEdit);
    formLayout->addRow(new QLabel("Check-out Date:"), checkOutEdit);
    layout->addLayout(formLayout);

    auto isSelectingCheckIn = std::make_shared<bool>(true);
    auto hasFinalSelection = std::make_shared<bool>(false);

    // Funcția lambda de refresh culori calendar
    auto refreshCalendarColors = [=]() {
        QDate today = QDate::currentDate();
        for (int i = -365; i < 365; ++i) {
            QDate d = today.addDays(i);
            if (d < today) { calendar->setDateTextFormat(d, pastFormat); }
            else { calendar->setDateTextFormat(d, availableFormat); }
        }
        for (const QDate &bookedDate : selectedRoom->bookedDates) {
            if (bookedDate >= today) { calendar->setDateTextFormat(bookedDate, unavailableFormat); }
        }
        if (!(*isSelectingCheckIn) || *hasFinalSelection) {
            QDate startSel = checkInEdit->date();
            QDate endSel = checkOutEdit->date();
            for (QDate d = startSel; d <= endSel; d = d.addDays(1)) {
                if (d >= today) { calendar->setDateTextFormat(d, previewFormat); }
            }
        }
    };

    refreshCalendarColors();

    connect(calendar, &QCalendarWidget::currentPageChanged, dialog, [=](int, int) {
        refreshCalendarColors();
    });

    // Logică Click Calendar
    connect(calendar, &QCalendarWidget::clicked, this, [=](const QDate &date) {
        if (date < QDate::currentDate()) {
            QMessageBox::warning(dialog, "Error", "Cannot select past dates.");
            return;
        }
        if (selectedRoom->bookedDates.contains(date)) {
            QMessageBox::warning(dialog, "Date Unavailable", "This day is already booked!");
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
                bool hasBlockedNight = false;
                for (QDate d = checkInEdit->date(); d < date; d = d.addDays(1)) {
                    if (selectedRoom->bookedDates.contains(d)) { hasBlockedNight = true; break; }
                }
                if (hasBlockedNight) {
                    checkInEdit->setDate(date);
                    checkOutEdit->setDate(date.addDays(1));
                    statusLbl->setText("Jumped over a blocked zone. New CHECK-IN set here!");
                    statusLbl->setStyleSheet("color: #3b82f6; font-weight: bold;");
                    *isSelectingCheckIn = false;
                    *hasFinalSelection = false;
                } else {
                    checkOutEdit->setDate(date);
                    statusLbl->setText("Period Selected! Click again to reset Check-in.");
                    statusLbl->setStyleSheet("color: #22c55e; font-weight: bold;");
                    *isSelectingCheckIn = true;
                    *hasFinalSelection = true;
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
        refreshCalendarColors();
    });

    QPushButton *btnConfirm = new QPushButton("Confirm Reservation", dialog);
    btnConfirm->setStyleSheet(primaryBtnStyle);
    layout->addWidget(btnConfirm);

    // --- LOGICĂ CONFIRMARE ȘI VERIFICARE BALANȚĂ ---
    connect(btnConfirm, &QPushButton::clicked, this, [=]() {
        QDate start = checkInEdit->date();
        QDate end = checkOutEdit->date();

        if (start >= end) {
            QMessageBox::warning(dialog, "Error", "Check-out date must be after Check-in date.");
            return;
        }

        // 1. CALCULUL COSTULUI TOTAL
        int nights = start.daysTo(end);
        double pricePerNight = selectedRoom->basePrice * (1.0 - currentAccommodationInDetails.discountPercent);
        double totalCost = pricePerNight * nights;

        // 2. VERIFICARE FONDURI DISPONIBILE
        if (currentUser.balance < totalCost) {
            // Creăm o mini-interfață customizată de eroare în stilul aplicației
            QDialog *failDialog = new QDialog(dialog);
            failDialog->setWindowTitle("Insufficient Funds");
            failDialog->setFixedSize(380, 200);
            failDialog->setStyleSheet("QDialog { background-color: #0f172a; border: 1px solid #ef4444; border-radius: 8px; }");

            QVBoxLayout *failLayout = new QVBoxLayout(failDialog);
            failLayout->setContentsMargins(25, 25, 25, 25);

            QLabel *failMsg = new QLabel(failDialog);
            failMsg->setText(QString("Booking failed! You do not have enough funds.\n\nTotal Cost: %1 €\nYour Balance: %2 €")
                                 .arg(QString::number(totalCost, 'f', 2))
                                 .arg(QString::number(currentUser.balance, 'f', 2)));
            failMsg->setStyleSheet("color: #f8fafc; font-size: 14px; font-weight: bold;");
            failMsg->setWordWrap(true);
            failMsg->setAlignment(Qt::AlignCenter);
            failLayout->addWidget(failMsg);
            failLayout->addSpacing(15);

            QPushButton *btnOk = new QPushButton("Ok, I understand", failDialog);
            btnOk->setStyleSheet(dangerBtnStyle + " padding: 8px 15px; font-size: 13px;");
            btnOk->setCursor(Qt::PointingHandCursor);
            connect(btnOk, &QPushButton::clicked, failDialog, &QDialog::accept);
            failLayout->addWidget(btnOk);

            failDialog->exec();
            failDialog->deleteLater();
            return; // Oprim execuția rezervării aici!
        }

        // 3. ACTUALIZARE ȘI DDUCERE BALANȚĂ (Dacă are destui bani)
        currentUser.balance -= totalCost;
        if (lblBalanceVal) {
            lblBalanceVal->setText(QString::number(currentUser.balance, 'f', 2) + " €");
        }

        // Salvăm nopțile rezervate
        for (QDate d = start; d < end; d = d.addDays(1)) {
            selectedRoom->bookedDates.append(d);
        }
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
        newBooking.userName = currentUser.name.isEmpty() ? "Tester Local" : currentUser.name;
        newBooking.userEmail = currentUser.email;
        userBookings.append(newBooking);

        // Pop-up succes în stil clasic
        QMessageBox::information(dialog, "Success", QString("Reservation completed! Deducted %1 € from your balance.")
                                                        .arg(QString::number(totalCost, 'f', 2)));
        dialog->accept();

        displayRooms(roomSearchBar ? roomSearchBar->text() : "");
    });

    dialog->exec();
}

void MainWindow::updateBookingHistoryUi()
{
    if (!historyLayout) return;

    QLayoutItem *child;
    while ((child = historyLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    for (const auto &b : userBookings) {
        if (b.userEmail != currentUser.email) {
            continue; // Daca rezervarea nu este a utilizatorului curent, nu o afisam!
        }
        QFrame *fr = new QFrame();
        fr->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 8px; border: 1px solid #334155; padding: 5px; }");
        QHBoxLayout *cl = new QHBoxLayout(fr);
        QVBoxLayout *inf = new QVBoxLayout();

        QLabel *hN = new QLabel(b.hotelName);
        hN->setStyleSheet("font-weight: bold; color: white; border: none; background: transparent;");
        QLabel *hD = new QLabel(b.dateRange);
        hD->setStyleSheet("color: #94a3b8; font-size: 12px; border: none; background: transparent;");

        inf->addWidget(hN);
        inf->addWidget(hD);

        QLabel *st = new QLabel(b.status);
        // --- MODIFICARE STILIZARE STATUS CLIENT ---
        if (b.status == "Finished") {
            st->setStyleSheet("color:#22c55e; border: none; background: transparent; font-weight: bold;");
        } else if (b.status == "Cancelled") {
            st->setStyleSheet("color:#ef4444; border: none; background: transparent; font-weight: bold;"); // Roșu pentru anulat
        } else {
            st->setStyleSheet("color:#3b82f6; border: none; background: transparent; font-weight: bold;");
        }
        // ------------------------------------------

        cl->addLayout(inf);
        cl->addStretch();
        cl->addWidget(st);
        historyLayout->addWidget(fr);
    }
}

QWidget *MainWindow::createAdminDashboardWidget()
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(40, 30, 40, 30);

    // Header Admin
    QHBoxLayout *h = new QHBoxLayout();
    QLabel *pt = new QLabel("🔑 Admin Control Panel");
    pt->setStyleSheet("font-size: 24px; font-weight: bold; color: #ef4444;"); // Roșu pentru admin

    QPushButton *btnLogout = new QPushButton("Logout Admin");
    btnLogout->setStyleSheet(dangerBtnStyle);
    btnLogout->setCursor(Qt::PointingHandCursor);
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::goToLogin);

    h->addWidget(pt);
    h->addStretch();
    h->addWidget(btnLogout);
    ml->addLayout(h);
    ml->addSpacing(20);

    QLabel *sub = new QLabel("Global Booking History (All Users)");
    sub->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6; margin-bottom: 10px;");
    ml->addWidget(sub);

    // Zona de Scroll pentru rezervări
    QScrollArea *sa = new QScrollArea();
    sa->setWidgetResizable(true);
    sa->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget *container = new QWidget();
    adminHistoryLayout = new QVBoxLayout(container);
    adminHistoryLayout->setAlignment(Qt::AlignTop);

    sa->setWidget(container);
    ml->addWidget(sa);

    return w;
}

void MainWindow::updateAdminDashboardUi()
{
    if (!adminHistoryLayout) return;

    // Curățăm elementele grafice vechi din layout-ul de admin
    QLayoutItem *child;
    while ((child = adminHistoryLayout->takeAt(0)) != nullptr) {
        if (child->widget())
            delete child->widget();
        delete child;
    }

    // 1. Rezervări simulate fixe (baza de date generală)
    struct GlobalBooking { QString user; QString hotel; QString dates; QString status; };
    QList<GlobalBooking> databaseBookings = {
        {"George Popescu", "Transylvania Castle (Deluxe Suite)", "May 10 - May 14, 2026", "Finished"},
        {"Elena Ionescu", "Retezat Cabin (Double Room)", "June 01 - June 05, 2026", "Upcoming"}
    };

    // 2. Funcția lambda ajutătoare pentru randarea unui card în Admin
    auto addCard = [this](QString user, QString hotel, QString dates, QString status, bool canCancel, int bookingIndex = -1) {
        QFrame *fr = new QFrame();
        fr->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 10px; border: 1px solid #334155; padding: 12px; margin-bottom: 5px; }");
        QHBoxLayout *cl = new QHBoxLayout(fr);
        QVBoxLayout *inf = new QVBoxLayout();

        QLabel *uL = new QLabel("👤 Customer: " + user);
        uL->setStyleSheet("font-weight: bold; color: #94a3b8; font-size: 13px; border: none; background: transparent;");
        QLabel *hN = new QLabel(hotel);
        hN->setStyleSheet("font-weight: bold; color: white; font-size: 16px; border: none; background: transparent;");
        QLabel *hD = new QLabel("📅 " + dates);
        hD->setStyleSheet("color: #64748b; font-size: 13px; border: none; background: transparent;");

        inf->addWidget(uL);
        inf->addWidget(hN);
        inf->addWidget(hD);
        cl->addLayout(inf);
        cl->addStretch();

        QLabel *st = new QLabel(status);
        if (status == "Finished") {
            st->setStyleSheet("color:#22c55e; font-weight: bold; border: none; background: transparent; margin-right: 15px;");
        } else if (status == "Cancelled") {
            st->setStyleSheet("color:#ef4444; font-weight: bold; border: none; background: transparent; margin-right: 15px;");
        } else {
            st->setStyleSheet("color:#3b82f6; font-weight: bold; border: none; background: transparent; margin-right: 15px;");
        }
        cl->addWidget(st);

        // Buton anulare pentru Admin
        if (canCancel && status != "Cancelled" && status != "Finished") {
            QPushButton *btnCancel = new QPushButton("Cancel Reservation");
            btnCancel->setStyleSheet(dangerBtnStyle + " padding: 5px 10px; font-size: 12px;");
            btnCancel->setCursor(Qt::PointingHandCursor);

            // --- NOU: LOGICĂ MINI-INTERFAȚĂ CUSTOMIZATĂ DE CONFIRMARE ---
            connect(btnCancel, &QPushButton::clicked, this, [this, bookingIndex]() {
                // Creăm un dialog pop-up personalizat
                QDialog *confDialog = new QDialog(this);
                confDialog->setWindowTitle("Confirm Action");
                confDialog->setFixedSize(380, 180);
                confDialog->setStyleSheet("QDialog { background-color: #0f172a; border: 1px solid #334155; border-radius: 8px; }");

                QVBoxLayout *layout = new QVBoxLayout(confDialog);
                layout->setContentsMargins(25, 25, 25, 25);

                // Textul întrebării stilizat ca în aplicație
                QLabel *msgLabel = new QLabel("Are you sure you want to cancel this reservation?", confDialog);
                msgLabel->setStyleSheet("color: #f8fafc; font-size: 15px; font-weight: bold;");
                msgLabel->setWordWrap(true);
                msgLabel->setAlignment(Qt::AlignCenter);
                layout->addWidget(msgLabel);
                layout->addSpacing(15);

                // Layout orizontal pentru butoane
                QHBoxLayout *btnLayout = new QHBoxLayout();

                QPushButton *btnNo = new QPushButton("No, Keep It", confDialog);
                btnNo->setStyleSheet(secondaryBtnStyle + " padding: 8px 15px; font-size: 13px;");
                btnNo->setCursor(Qt::PointingHandCursor);

                QPushButton *btnYes = new QPushButton("Yes, Cancel", confDialog);
                btnYes->setStyleSheet(dangerBtnStyle + " padding: 8px 15px; font-size: 13px;");
                btnYes->setCursor(Qt::PointingHandCursor);

                btnLayout->addWidget(btnNo);
                btnLayout->addWidget(btnYes);
                layout->addLayout(btnLayout);

                // Conectăm butoanele la acțiunile dialogului (Accept / Reject)
                connect(btnYes, &QPushButton::clicked, confDialog, &QDialog::accept);
                connect(btnNo, &QPushButton::clicked, confDialog, &QDialog::reject);

                // Executăm mini-interfața și verificăm dacă s-a apăsat "Yes"
                if (confDialog->exec() == QDialog::Accepted) {
                    userBookings[bookingIndex].status = "Cancelled";
                    updateAdminDashboardUi(); // Refresh la listă
                }

                // Curățăm automat memoria pop-up-ului după închidere
                confDialog->deleteLater();
            });
            // -------------------------------------------------------------

            cl->addWidget(btnCancel);
        }

        adminHistoryLayout->addWidget(fr);
    };


    for (const auto &b : databaseBookings) {
        addCard(b.user, b.hotel, b.dates, b.status, false);
    }

    for (int i = 0; i < userBookings.size(); ++i) {
        const auto &b = userBookings[i];
        addCard(b.userName, b.hotelName, b.dateRange, b.status, true, i);
    }
}