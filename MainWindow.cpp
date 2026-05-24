#include "MainWindow.h"
#include "AdminDashboardPage.h"
#include "DetailsPage.h"
#include "LoginPage.h"
#include "MainAppPage.h"
#include "RegisterPage.h"
#include "Styles.h"
#include "UserProfilePage.h"

#include <QCalendarWidget>
#include <QCryptographicHash>
#include <QDialog>
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
#include <QTextCharFormat>
#include <QVBoxLayout>


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
    stackedWidget->addWidget(LoginPage::createWidget(this, loginEmailInput, loginPasswordInput));                                                                                             // 0
    stackedWidget->addWidget(RegisterPage::createWidget(this, regNameInput, regEmailInput, regPasswordInput, regPhoneInput, regAddressInput, regDobInput, regCountryInput, regGenderInput));  // 1
    stackedWidget->addWidget(MainAppPage::createWidget(this, searchBarInput, accommodationsContainer, accommodationsLayout));                                                                 // 2
    stackedWidget->addWidget(UserProfilePage::createWidget(this, lblNameVal, lblEmailVal, lblPhoneVal, lblDobVal, lblCountryVal, lblGenderVal, lblAddressVal, lblBalanceVal, historyLayout)); // 3
    stackedWidget->addWidget(DetailsPage::createWidget(this, detName, detAddress, detPromo, roomSearchBar, roomsLayout, cbBalcony, cbFridge, cbAC, cbTV, cbWifi, cbSofa, detImage)); // 4
    stackedWidget->addWidget(AdminDashboardPage::createWidget(this, adminHistoryLayout, lblAdminLocation, adminSearchBar));                                                                   // 5
    setCentralWidget(stackedWidget);
}


static QString hashPassword(const QString &password)
{
    QByteArray passwordData = password.toUtf8();
    QByteArray hashedData = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256);
    return hashedData.toHex();
}


void MainWindow::goToRegister()
{
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::goToLogin()
{
    int idx = stackedWidget->currentIndex();
    if ((idx == 2 || idx == 3) && !currentUser.email.isEmpty()) {
        QJsonObject req;
        req["type"]  = "FORCE_LOGOUT";
        req["email"] = currentUser.email;
        m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
    }

    currentUser = UserInfo();

    clearRegisterFields();
    if (loginEmailInput)    loginEmailInput->clear();
    if (loginPasswordInput) loginPasswordInput->clear();
    if (searchBarInput)     searchBarInput->clear();

    allAccommodations.clear();
    populateAccommodations("");

    stackedWidget->setCurrentIndex(0);
}

void MainWindow::goToUserProfile()
{
    QJsonObject req;
    req["type"]  = "GET_CLIENT_BOOKINGS";
    req["id"] = IdUser;
    m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
    stackedWidget->setCurrentIndex(3);
}

void MainWindow::backToMainApp()
{
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::adminLogout()
{
    if (loginEmailInput)    loginEmailInput->clear();
    if (loginPasswordInput) loginPasswordInput->clear();
    QJsonObject req;
    req["type"]  = "FORCE_LOGOUT";
    req["email"] = currentUser.email;
    m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
    stackedWidget->setCurrentIndex(0);
}

// register
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
    if (ps.length() < 6) {
        QMessageBox::warning(this, "Error", "Weak password – minimum 6 characters.");
        return;
    }
    if (!QRegularExpression("[A-Z]").match(ps).hasMatch()) {
        QMessageBox::warning(this, "Error", "Weak password – must contain at least 1 capital letter.");
        return;
    }
    if (!QRegularExpression("[^a-zA-Z0-9]").match(ps).hasMatch()) {
        QMessageBox::warning(this, "Error", "Weak password – must contain at least one special character.");
        return;
    }
    if (regCountryInput->findText(regCountryInput->currentText()) == -1) {
        QMessageBox::warning(this, "Error", "Please select a valid country from the list.");
        return;
    }

    QJsonObject userObj;
    userObj["type"]     = "REGISTER_USER";
    userObj["name"]     = regNameInput->text().trimmed();
    userObj["email"]    = em;
    userObj["password"] = hashPassword(ps);
    userObj["phone"]    = regPhoneInput->text().trimmed();
    userObj["dob"]      = regDobInput->date().toString("yyyy-MM-dd");
    userObj["country"]  = regCountryInput->currentText();
    userObj["gender"]   = regGenderInput->currentText();
    userObj["address"]  = regAddressInput->text().trimmed();

    m_socketClient->sendMessage(QJsonDocument(userObj).toJson(QJsonDocument::Compact));
    qDebug() << "Register request sent for:" << em;
}

// login
void MainWindow::processLogin()
{
    QString em = loginEmailInput->text().trimmed();
    QString ps = loginPasswordInput->text();

    QJsonObject userObj;
    userObj["type"]     = "LOGIN_USER";
    userObj["email"]    = em;
    userObj["password"] = hashPassword(ps);

    m_socketClient->sendMessage(QJsonDocument(userObj).toJson(QJsonDocument::Compact));
    qDebug() << "Login request sent for:" << em;
}

// filtrare sloturi
void MainWindow::filterAccommodations(const QString &q)
{
    populateAccommodations(q);
}

void MainWindow::filterRooms(const QString &query)
{
    displayRooms(query);
}

// stergere campuri
void MainWindow::clearRegisterFields()
{
    if (regNameInput)     regNameInput->clear();
    if (regEmailInput)    regEmailInput->clear();
    if (regPasswordInput) regPasswordInput->clear();
    if (regPhoneInput)    regPhoneInput->clear();
    if (regAddressInput)  regAddressInput->clear();
    if (regDobInput)      regDobInput->setDate(QDate(2000, 1, 1));
    if (regCountryInput)  regCountryInput->setCurrentIndex(0);
    if (regGenderInput)   regGenderInput->setCurrentIndex(0);
}

void MainWindow::openAccommodationDetails(const Accommodation &acc)
{
    currentAccommodationInDetails = acc;
    detName->setText(acc.name);
    detAddress->setText("📍 " + acc.address + ", " + acc.location);

    QPixmap pixmap;
    QByteArray imageData = QByteArray::fromBase64(acc.imageSource.toUtf8());
    if (!pixmap.loadFromData(imageData)) {
        pixmap.load(acc.imageSource);
    }
    if (pixmap.isNull()) {
        pixmap.load(":/img/destination.png");
    }
    QPixmap scaledPixmap = pixmap.scaled(160, 90, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap croppedPixmap = scaledPixmap.copy(0, 0, 160, 90);
    detImage->setPixmap(croppedPixmap);

    if (roomSearchBar)  roomSearchBar->clear();
    if (cbBalcony) cbBalcony->setChecked(false);
    if (cbFridge)  cbFridge->setChecked(false);
    if (cbAC)      cbAC->setChecked(false);
    if (cbTV)      cbTV->setChecked(false);
    if (cbWifi)    cbWifi->setChecked(false);
    if (cbSofa)    cbSofa->setChecked(false);

    if (acc.discountPercent > 0) {
        detPromo->setText("🔥 Special Offer: " + acc.promoName + " ("
                          + QString::number(acc.discountPercent * 100, 'f', 0) + "% OFF!)");
        detPromo->setStyleSheet("color: #f59e0b; font-weight: bold; margin-left: 85px; font-size: 14px;");
        detPromo->show();
    } else {
        detPromo->hide();
    }
    QJsonObject reqImages;
    reqImages["type"] = "GET_RENTAL_IMAGES";
    reqImages["hotel_id"] = acc.id;
    m_socketClient->sendMessage(QJsonDocument(reqImages).toJson(QJsonDocument::Compact));

    QJsonObject req;
    req["type"] = "GET_LOCATION_BOOKINGS";
    req["location_id"] = acc.id;
    m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::displayRooms(const QString &f)
{
    QLayoutItem *child;
    while ((child = roomsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    bool reqBalcony = cbBalcony && cbBalcony->isChecked();
    bool reqFridge  = cbFridge  && cbFridge->isChecked();
    bool reqAC      = cbAC      && cbAC->isChecked();
    bool reqTV      = cbTV      && cbTV->isChecked();
    bool reqWifi    = cbWifi    && cbWifi->isChecked();
    bool reqSofa    = cbSofa    && cbSofa->isChecked();

    for (const auto &room : currentAccommodationInDetails.rooms) {
        bool matchesType = room.type.contains(f, Qt::CaseInsensitive);
        bool matchesFac  = false;
        bool rBalcony = false, rFridge = false, rAC = false, rTV = false, rWifi = false;

        for (const auto &fac : room.facilities) {
            if (fac.name.contains(f, Qt::CaseInsensitive)) matchesFac = true;
            if (fac.name == "Balcony") rBalcony = true;
            if (fac.name == "Fridge")  rFridge  = true;
            if (fac.name == "AC")      rAC      = true;
            if (fac.name == "TV")      rTV      = true;
            if (fac.name == "WiFi")    rWifi    = true;
        }

        if (!f.isEmpty() && !matchesType && !matchesFac) continue;
        if (reqBalcony && !rBalcony)     continue;
        if (reqFridge  && !rFridge)      continue;
        if (reqAC      && !rAC)          continue;
        if (reqTV      && !rTV)          continue;
        if (reqWifi    && !rWifi)        continue;
        if (reqSofa    && !room.hasSofa) continue;

        QFrame *fr = new QFrame();
        fr->setStyleSheet("QFrame { background-color: transparent; border-radius: 10px; border: 1px solid rgba(255, 255, 255, 0.1); }");

        QHBoxLayout *l = new QHBoxLayout(fr);
        l->setContentsMargins(15, 15, 15, 15);

        QLabel *roomImgLabel = new QLabel();
        QPixmap roomPixmap;
        QByteArray roomImageData = QByteArray::fromBase64(room.imageSource.toUtf8());
        if (!roomPixmap.loadFromData(roomImageData)) {
            roomPixmap.load(room.imageSource);
        }
        if (roomPixmap.isNull()) {
            roomPixmap.load(":/img/destination.png"); // placeholder dacă nu există poză
        }

        // Dimensiuni pentru poza camerei din listă
        int rWidth = 160;
        int rHeight = 90;
        QPixmap scaledRoomPixmap = roomPixmap.scaled(rWidth, rHeight, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPixmap croppedRoomPixmap = scaledRoomPixmap.copy(0, 0, rWidth, rHeight);

        roomImgLabel->setPixmap(croppedRoomPixmap);
        roomImgLabel->setFixedSize(rWidth, rHeight);
        roomImgLabel->setStyleSheet("border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; background: #0f172a;");

        // Adăugăm imaginea în stânga cardului camerei
        l->addWidget(roomImgLabel);
        l->addSpacing(15);

        QVBoxLayout *inf = new QVBoxLayout();

        QLabel *typeL = new QLabel(room.type);
        typeL->setStyleSheet("font-size: 18px; font-weight: bold; color: white; border: none; background: transparent;");

        int totalCap = room.beds + (room.hasSofa ? 1 : 0);
        QLabel *capL = new QLabel(
            QString("🛏️ %1 Beds | 👥 Max Capacity: %2 people").arg(room.beds).arg(totalCap));
        capL->setStyleSheet("color: #94a3b8; border: none; background: transparent;");

        QString flist;
        for (const auto &fac : room.facilities) flist += fac.name + ", ";
        if (flist.endsWith(", ")) flist.chop(2);
        QLabel *facL = new QLabel("Facilities: " + flist);
        facL->setStyleSheet("color: #64748b; font-size: 12px; border: none; background: transparent;");

        inf->addWidget(typeL);
        inf->addWidget(capL);
        inf->addWidget(facL);

        double finalPrice = room.basePrice * (1.0 - currentAccommodationInDetails.discountPercent);
        QLabel *pL = new QLabel(QString::number(finalPrice, 'f', 2) + " € / night");
        pL->setStyleSheet("color: #22c55e; font-size: 18px; font-weight: bold; border: none; background: transparent;");

        QVBoxLayout *rightLayout = new QVBoxLayout();
        rightLayout->setAlignment(Qt::AlignCenter);
        rightLayout->addWidget(pL);

        QPushButton *btnBook = new QPushButton("Book Now");
        btnBook->setStyleSheet(primaryBtnStyle);
        btnBook->setCursor(Qt::PointingHandCursor);
        connect(btnBook, &QPushButton::clicked, this, [this, room]() { bookRoom(room.id); });
        rightLayout->addWidget(btnBook);

        l->addLayout(inf);
        l->addStretch();
        l->addLayout(rightLayout);

        roomsLayout->addWidget(fr);
    }
}

void MainWindow::populateAccommodations(const QString &f)
{
    // Ștergem elementele vechi din layout pentru a evita suprapunerile la filtrare
    QLayoutItem *c;
    while ((c = accommodationsLayout->takeAt(0)) != nullptr) {
        if (c->widget()) delete c->widget();
        delete c;
    }

    // Parcurgem toate unitățile de cazare primite de pe backend
    for (const auto &acc : allAccommodations) {
        if (!f.isEmpty()
            && !acc.name.contains(f, Qt::CaseInsensitive)
            && !acc.location.contains(f, Qt::CaseInsensitive))
            continue;

        // Creăm cardul pentru unitatea de cazare
        QFrame *card = new QFrame();
        card->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 12px; }");

        QHBoxLayout *l = new QHBoxLayout(card);
        l->setContentsMargins(20, 20, 20, 20);

        // --- SECTIUNE IMAGINE (Claritate îmbunătățită și scalare inteligentă) ---
        QLabel *imgLabel = new QLabel();
        QPixmap pixmap;

        // Încercăm mai întâi interpretarea textului ca string Base64 primit din JSON
        QByteArray imageData = QByteArray::fromBase64(acc.imageSource.toUtf8());
        if (!pixmap.loadFromData(imageData)) {
            // Dacă nu e Base64, încercăm încărcarea ca o cale locală directă de pe disc
            pixmap.load(acc.imageSource);
        }

        // Dacă imaginea este nulă (eroare de încărcare/lipsă date), aplicăm un placeholder implicit
        if (pixmap.isNull()) {
            pixmap.load(":/img/destination.png");
        }

        // Setează dimensiunile ideale tip landscape (format ~16:9)
        int imgWidth = 240;
        int imgHeight = 135;

        // Pasul 1: Scalare fină (SmoothTransformation elimină pixelarea și marginile zimțate)
        // Utilizăm KeepAspectRatioByExpanding pentru ca imaginea să umple tot spațiul alocat
        QPixmap scaledPixmap = pixmap.scaled(imgWidth, imgHeight,
                                             Qt::KeepAspectRatioByExpanding,
                                             Qt::SmoothTransformation);

        // Pasul 2: Decupăm exact surplusul pentru a păstra dimensiunile perfecte, fără distorsionare
        QPixmap croppedPixmap = scaledPixmap.copy(0, 0, imgWidth, imgHeight);

        imgLabel->setPixmap(croppedPixmap);
        imgLabel->setFixedSize(imgWidth, imgHeight);

        // Stil pentru imagine: colțuri rotunjite subtile și o bordură fină adaptată la tema dark
        imgLabel->setStyleSheet("border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; background: #0f172a;");

        // Adăugăm imaginea în partea stângă a cardului
        l->addWidget(imgLabel);
        l->addSpacing(15); // Spațiu între imagine și descrieri
        // ------------------------------------------------------------------------

        // Coloana cu texte informative (Nume și Locație)
        QVBoxLayout *inf = new QVBoxLayout();
        QLabel *nL = new QLabel(acc.name);
        nL->setStyleSheet("color: white; font-size: 18px; font-weight: bold; border: none; background: transparent;");
        QLabel *lL = new QLabel("📍 " + acc.location);
        lL->setStyleSheet("color: #94a3b8; font-size: 14px; border: none; background: transparent;");
        inf->addWidget(nL);
        inf->addWidget(lL);

        // Butonul de vizualizare detalii
        QPushButton *btn = new QPushButton("View Details");
        btn->setStyleSheet(primaryBtnStyle);
        btn->setCursor(Qt::PointingHandCursor);

        int targetId = acc.id;
        connect(btn, &QPushButton::clicked, this, [this, targetId]() {
            for (const auto &globalAcc : allAccommodations) {
                if (globalAcc.id == targetId) {
                    openAccommodationDetails(globalAcc);
                    break;
                }
            }
        });

        // Asamblăm structura cardului
        l->addLayout(inf);
        l->addStretch();
        l->addWidget(btn);

        // Adăugăm cardul în containerul principal din interfață
        accommodationsLayout->addWidget(card);
    }
}

void MainWindow::bookRoom(int roomId)
{
    Room *selectedRoom = nullptr;
    for (auto &r : currentAccommodationInDetails.rooms) {
        if (r.id == roomId) { selectedRoom = &r; break; }
    }
    if (!selectedRoom) return;

    QDialog *dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle("Book Room – " + selectedRoom->type);
    dialog->resize(470, 480);
    dialog->setStyleSheet("QDialog { background-color: #0f172a; }");

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QLabel *title = new QLabel("Select Booking Period", dialog);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: white; margin-bottom: 5px;");
    layout->addWidget(title);

    QLabel *statusLbl = new QLabel("Next action: Select CHECK-IN date", dialog);
    statusLbl->setStyleSheet("color: #3b82f6; font-weight: bold; font-size: 13px; margin-bottom: 10px;");
    layout->addWidget(statusLbl);

    QString customCalendarStyle =
        "QCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #0f172a; padding: 4px; }"
        "QCalendarWidget QToolButton { color: white; background-color: transparent; border: none; font-weight: bold; font-size: 14px; padding: 4px 8px; margin: 0px 2px; }"
        "QCalendarWidget QToolButton:hover { background-color: #2563eb; border-radius: 4px; }"
        "QCalendarWidget QMenu { background-color: #1e293b; color: white; border: 1px solid #475569; }"
        "QCalendarWidget QSpinBox { background-color: #1e293b; color: white; border: 1px solid #475569; border-radius: 3px; padding: 2px 4px; min-width: 65px; margin-left: 10px; }"
        "QCalendarWidget QSpinBox::up-button, QCalendarWidget QSpinBox::down-button { width: 16px; }"
        "QCalendarWidget QAbstractItemView:enabled { background-color: #1e293b; color: white; selection-background-color: #2563eb; selection-color: white; border: none; outline: none; }"
        "QCalendarWidget QAbstractItemView:disabled { color: #475569; }";

    QCalendarWidget *calendar = new QCalendarWidget(dialog);
    calendar->setGridVisible(true);
    calendar->setStyleSheet(customCalendarStyle);
    layout->addWidget(calendar);

    QTextCharFormat availableFormat, unavailableFormat, previewFormat, pastFormat;
    availableFormat.setBackground(QColor("#16a34a"));   availableFormat.setForeground(Qt::white);
    unavailableFormat.setBackground(QColor("#dc2626")); unavailableFormat.setForeground(Qt::white);
    previewFormat.setBackground(QColor("#f59e0b"));     previewFormat.setForeground(Qt::white);
    pastFormat.setBackground(QColor("#334155"));        pastFormat.setForeground(QColor("#64748b"));

    auto checkInDate  = std::make_shared<QDate>(QDate::currentDate());
    auto checkOutDate = std::make_shared<QDate>(QDate::currentDate().addDays(1));
    auto isSelectingCheckIn = std::make_shared<bool>(true);
    auto hasFinalSelection  = std::make_shared<bool>(false);

    auto refreshCalendarColors = [=]() {
        QDate today = QDate::currentDate();
        for (int i = -365; i < 365; ++i) {
            QDate d = today.addDays(i);
            calendar->setDateTextFormat(d, d < today ? pastFormat : availableFormat);
        }
        for (const QDate &bookedDate : selectedRoom->bookedDates) {
            if (bookedDate >= today) calendar->setDateTextFormat(bookedDate, unavailableFormat);
        }
        if (!(*isSelectingCheckIn) || *hasFinalSelection) {
            for (QDate d = *checkInDate; d <= *checkOutDate; d = d.addDays(1)) {
                if (d >= today) calendar->setDateTextFormat(d, previewFormat);
            }
        }
    };
    refreshCalendarColors();

    connect(calendar, &QCalendarWidget::currentPageChanged, dialog, [=](int, int) { refreshCalendarColors(); });

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
            *checkInDate = date;
            *checkOutDate = date.addDays(1);
            statusLbl->setText(QString("📍 Check-in set: %1 | Now select CHECK-OUT date...").arg(date.toString("yyyy-MM-dd")));
            statusLbl->setStyleSheet("color: #f59e0b; font-weight: bold;");
            *isSelectingCheckIn = false;
            *hasFinalSelection  = false;
        } else {
            if (date > *checkInDate) {
                bool hasBlockedNight = false;
                for (QDate d = *checkInDate; d < date; d = d.addDays(1)) {
                    if (selectedRoom->bookedDates.contains(d)) { hasBlockedNight = true; break; }
                }
                if (hasBlockedNight) {
                    *checkInDate = date;
                    *checkOutDate = date.addDays(1);
                    statusLbl->setText(QString("You selected an occupied period. New Check-in: %1").arg(date.toString("yyyy-MM-dd")));
                    statusLbl->setStyleSheet("color: #3b82f6; font-weight: bold;");
                    *isSelectingCheckIn = false;
                    *hasFinalSelection  = false;
                } else {
                    *checkOutDate = date;
                    statusLbl->setText(QString("✅ Period: %1 to %2 (Click again to reset)").arg(checkInDate->toString("yyyy-MM-dd")).arg(date.toString("yyyy-MM-dd")));
                    statusLbl->setStyleSheet("color: #22c55e; font-weight: bold;");
                    *isSelectingCheckIn = true;
                    *hasFinalSelection  = true;
                }
            } else {
                *checkInDate = date;
                *checkOutDate = date.addDays(1);
                statusLbl->setText(QString("📍 Check-in set: %1 | Now select CHECK-OUT date...").arg(date.toString("yyyy-MM-dd")));
                statusLbl->setStyleSheet("color: #f59e0b; font-weight: bold;");
                *isSelectingCheckIn = false;
                *hasFinalSelection  = false;
            }
        }
        refreshCalendarColors();
    });

    layout->addSpacing(15);

    QPushButton *btnConfirm = new QPushButton("Confirm Reservation", dialog);
    btnConfirm->setStyleSheet(primaryBtnStyle);
    btnConfirm->setCursor(Qt::PointingHandCursor);
    layout->addWidget(btnConfirm);

    connect(btnConfirm, &QPushButton::clicked, this, [=]() {
        if (!(*hasFinalSelection)) {
            QMessageBox::warning(dialog, "Selection Incomplete", "Please select a complete Check-in and Check-out period on the calendar first.");
            return;
        }

        QDate start = *checkInDate;
        QDate end   = *checkOutDate;

        int nights = start.daysTo(end);
        double discountPercent = currentAccommodationInDetails.discountPercent;
        double basePricePerNight = selectedRoom->basePrice;

        double originalTotal = basePricePerNight * nights;
        double discountedPricePerNight = basePricePerNight * (1.0 - discountPercent);
        double finalTotal = discountedPricePerNight * nights;
        double savedAmount = originalTotal - finalTotal;

        if (currentUser.balance < finalTotal) {
            QDialog *failDialog = new QDialog(dialog);
            failDialog->setAttribute(Qt::WA_DeleteOnClose);
            failDialog->setWindowTitle("Insufficient Funds");
            failDialog->setFixedSize(400, 260);
            failDialog->setStyleSheet("QDialog { background-color: #0f172a; border: 1px solid #ef4444; border-radius: 12px; }");

            QVBoxLayout *failLayout = new QVBoxLayout(failDialog);
            failLayout->setContentsMargins(25, 25, 25, 25);

            QLabel *failTitle = new QLabel("⚠️ Transaction Declined", failDialog);
            failTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #ef4444; border: none; background: transparent;");
            failTitle->setAlignment(Qt::AlignCenter);
            failLayout->addWidget(failTitle);
            failLayout->addSpacing(10);

            QLabel *failBody = new QLabel(failDialog);
            failBody->setTextFormat(Qt::RichText);
            failBody->setWordWrap(true);
            failBody->setAlignment(Qt::AlignCenter);
            failBody->setStyleSheet("color: #94a3b8; font-size: 14px; border: none; background: transparent;");

            failBody->setText(QString(
                                  "You do not have enough funds to complete this booking.<br><br>"
                                  "<table align='center' style='color: #94a3b8; font-size: 14px;'>"
                                  "<tr><td align='right'>Required Amount:</td><td><b style='color: white;'>&nbsp;%1 €</b></td></tr>"
                                  "<tr><td align='right'>Your Balance:</td><td><b style='color: #ef4444;'>&nbsp;%2 €</b></td></tr>"
                                  "</table>"
                                  ).arg(QString::number(finalTotal, 'f', 2)).arg(QString::number(currentUser.balance, 'f', 2)));

            failLayout->addWidget(failBody);
            failLayout->addStretch();

            QPushButton *btnOk = new QPushButton("Got it, thanks", failDialog);
            btnOk->setStyleSheet(dangerBtnStyle + " padding: 10px; font-size: 13px; font-weight: bold;");
            btnOk->setCursor(Qt::PointingHandCursor);
            connect(btnOk, &QPushButton::clicked, failDialog, &QDialog::accept);
            failLayout->addWidget(btnOk);

            failDialog->exec();
            return;
        }

        QDialog *payDialog = new QDialog(dialog);
        payDialog->setWindowTitle("Confirm Booking & Payment");
        payDialog->setFixedSize(420, 320);
        payDialog->setStyleSheet("QDialog { background-color: #0f172a; border: 1px solid #334155; border-radius: 12px; }");

        QVBoxLayout *payVBox = new QVBoxLayout(payDialog);
        payVBox->setContentsMargins(25, 25, 25, 25);

        QLabel *payTitle = new QLabel("Reservation Summary", payDialog);
        payTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: white; border: none; background: transparent;");
        payTitle->setAlignment(Qt::AlignCenter);
        payVBox->addWidget(payTitle);
        payVBox->addSpacing(15);

        QLabel *payBody = new QLabel(payDialog);
        payBody->setTextFormat(Qt::RichText);
        payBody->setAlignment(Qt::AlignCenter);
        payBody->setStyleSheet("color: #94a3b8; font-size: 14px; border: none; background: transparent;");

        QString htmlContent = "";
        if (discountPercent > 0.0) {
            htmlContent = QString(
                              "<span style='color: #94a3b8;'>Initial Total: <span style='text-decoration: line-through; color: #ef4444;'>%1 €</span></span><br>"
                              "<span style='color: #f59e0b; font-weight: bold;'>Discount Applied: %2% OFF</span><br>"
                              "<span style='color: #22c55e; font-size: 12px;'>You save: %3 €</span><br><br>"
                              "<span style='color: white;'>Period: %4 night(s) | Rate: %5 € / night</span><br><br>"
                              "<span style='font-size: 15px; color: white;'>Total to Pay:</span><br>"
                              "<span style='font-size: 24px; color: #22c55e; font-weight: bold;'>%6 €</span>"
                              )
                              .arg(QString::number(originalTotal, 'f', 2))
                              .arg(QString::number(discountPercent * 100, 'f', 0))
                              .arg(QString::number(savedAmount, 'f', 2))
                              .arg(nights)
                              .arg(QString::number(discountedPricePerNight, 'f', 2))
                              .arg(QString::number(finalTotal, 'f', 2));
        } else {
            htmlContent = QString(
                              "<span style='color: white;'>Period: %1 night(s)</span><br>"
                              "<span style='color: white;'>Standard Rate: %2 € / night</span><br><br><br>"
                              "<span style='font-size: 15px; color: white;'>Total to Pay:</span><br>"
                              "<span style='font-size: 24px; color: #22c55e; font-weight: bold;'>%3 €</span>"
                              )
                              .arg(nights)
                              .arg(QString::number(basePricePerNight, 'f', 2))
                              .arg(QString::number(finalTotal, 'f', 2));
        }

        payBody->setText(htmlContent);
        payVBox->addWidget(payBody);
        payVBox->addStretch();

        QHBoxLayout *btnBox = new QHBoxLayout();
        QPushButton *btnNo = new QPushButton("Cancel", payDialog);
        btnNo->setStyleSheet(secondaryBtnStyle + " padding: 8px; font-size: 13px;");
        btnNo->setCursor(Qt::PointingHandCursor);

        QPushButton *btnYes = new QPushButton("Confirm & Pay", payDialog);
        btnYes->setStyleSheet(primaryBtnStyle + " padding: 8px; font-size: 13px;");
        btnYes->setCursor(Qt::PointingHandCursor);

        btnBox->addWidget(btnNo);
        btnBox->addWidget(btnYes);
        payVBox->addLayout(btnBox);

        connect(btnNo, &QPushButton::clicked, payDialog, &QDialog::reject);
        connect(btnYes, &QPushButton::clicked, payDialog, &QDialog::accept);

        if (payDialog->exec() == QDialog::Accepted) {
            QJsonObject req;
            req["type"] = "CREATE_RESERVATION";
            req["client_id"] = IdUser;
            req["room_id"] = selectedRoom->id;
            req["check_in"] = start.toString("yyyy-MM-dd");
            req["check_out"] = end.toString("yyyy-MM-dd");
            req["total_cost"] = finalTotal;

            m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
            dialog->accept();
        }
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

    bool any = false;
    for (int i = 0; i < userBookings.size(); ++i) {
        const auto &b = userBookings[i];
        if (b.userEmail != currentUser.email) continue;
        any = true;

        QFrame *fr = new QFrame();
        fr->setStyleSheet("QFrame { background-color: transparent; border-radius: 8px; border: 1px solid rgba(255, 255, 255, 0.1); padding: 5px; }");
        QHBoxLayout *cl = new QHBoxLayout(fr);
        QVBoxLayout *inf = new QVBoxLayout();

        QString titluAfisat = b.hotelName;
        if (!b.roomType.isEmpty()) {
            titluAfisat += " — " + b.roomType;
        }

        QLabel *hN = new QLabel(titluAfisat);
        hN->setStyleSheet("font-weight: bold; color: white; border: none; background: transparent;");

        QLabel *hD = new QLabel(b.dateRange);
        hD->setStyleSheet("color: #94a3b8; font-size: 12px; border: none; background: transparent;");

        inf->addWidget(hN);
        inf->addWidget(hD);

        QLabel *st = new QLabel(b.status);
        if (b.status == "finished")
            st->setStyleSheet("color:#22c55e; border: none; background: transparent; font-weight: bold;");
        else if (b.status == "cancelled")
            st->setStyleSheet("color:#ef4444; border: none; background: transparent; font-weight: bold;");
        else
            st->setStyleSheet("color:#3b82f6; border: none; background: transparent; font-weight: bold;");

        cl->addLayout(inf);
        cl->addStretch();
        cl->addWidget(st);

        if (b.status != "cancelled" && b.status != "finished") {
            QPushButton *btnCancel = new QPushButton("Cancel");
            btnCancel->setStyleSheet(dangerBtnStyle + " padding: 5px 10px; font-size: 12px; margin-left: 10px;");
            btnCancel->setCursor(Qt::PointingHandCursor);

            connect(btnCancel, &QPushButton::clicked, this, [this, i]() {
                clientCancelBooking(i);
            });
            cl->addWidget(btnCancel);
        }

        historyLayout->addWidget(fr);
    }

    if (!any) {
        QLabel *empty = new QLabel("No bookings yet.");
        empty->setStyleSheet("color: #475569; font-size: 14px;");
        empty->setAlignment(Qt::AlignCenter);
        historyLayout->addWidget(empty);
    }
}

void MainWindow::clientCancelBooking(int bookingIndex)
{
    if (bookingIndex < 0 || bookingIndex >= userBookings.size()) return;
    const auto &booking = userBookings[bookingIndex];

    QDate checkInDate = QDate::fromString(booking.rawCheckIn, "yyyy-MM-dd");
    QDate today = QDate::currentDate();


    // calculam numarul de zile si taxa de penalizare
    int daysToCheckIn = today.daysTo(checkInDate);
    double fee = 0.0;
    bool areTaxa = false;

    if (daysToCheckIn < 3) {
        fee = booking.totalCost * 0.30; // 30% taxa de penalizare
        areTaxa = true;
    }

    // suma returnata si noua balanta a clientului
    double refundAmount = booking.totalCost - fee;
    double updatedBalance = currentUser.balance + refundAmount;

    QDialog *confDialog = new QDialog(this);
    confDialog->setAttribute(Qt::WA_DeleteOnClose);
    confDialog->setWindowTitle("Cancellation confirmation");
    confDialog->setFixedSize(480, 260);
    confDialog->setStyleSheet("QDialog { background-color: #0f172a; border: 1px solid #334155; border-radius: 12px; }");

    QVBoxLayout *vl = new QVBoxLayout(confDialog);
    vl->setContentsMargins(25, 25, 25, 25);

    QLabel *titleLabel = new QLabel("Are you sure you want to cancel this reservation?", confDialog);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    titleLabel->setAlignment(Qt::AlignCenter);
    vl->addWidget(titleLabel);
    vl->addSpacing(10);

    QLabel *msgLabel = new QLabel(confDialog);
    msgLabel->setWordWrap(true);
    msgLabel->setAlignment(Qt::AlignCenter);
    msgLabel->setStyleSheet("color: #94a3b8; font-size: 14px;");

    if (areTaxa) {
        msgLabel->setText(QString(
                              "Warning! You are cancelling less than 3 days before check-in.<br>"
                              "A penalty fee of <b style='color: #ef4444;'>30%</b> will apply.<br><br>"
                              "Amount refunded to your account: <b style='color: #22c55e;'>%1 €</b><br>"
                              "Retained penalty: <b style='color: #ef4444;'>%2 €</b>"
                              ).arg(QString::number(refundAmount, 'f', 2)).arg(QString::number(fee, 'f', 2)));
    } else {
        msgLabel->setText(QString(
                              "Cancellation is made at least 3 days before check-in.<br>"
                              "This operation is <b style='color: #22c55e;'>FREE</b>.<br><br>"
                              "Full amount refunded: <b style='color: #22c55e;'>%1 €</b>"
                              ).arg(QString::number(refundAmount, 'f', 2)));
    }
    vl->addWidget(msgLabel);
    vl->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnNo  = new QPushButton("Keep Reservation", confDialog);
    btnNo->setStyleSheet(secondaryBtnStyle + " padding: 8px 15px; font-size: 13px;");
    btnNo->setCursor(Qt::PointingHandCursor);

    QPushButton *btnYes = new QPushButton("Cancel Reservation", confDialog);
    btnYes->setStyleSheet(dangerBtnStyle + " padding: 8px 15px; font-size: 13px;");
    btnYes->setCursor(Qt::PointingHandCursor);

    btnLayout->addWidget(btnNo);
    btnLayout->addWidget(btnYes);
    vl->addLayout(btnLayout);

    connect(btnNo,  &QPushButton::clicked, confDialog, &QDialog::reject);
    connect(btnYes, &QPushButton::clicked, confDialog, &QDialog::accept);

    if (confDialog->exec() == QDialog::Accepted) {
        QJsonObject req;
        req["type"] = "CLIENT_CANCEL_BOOKING";
        req["b_id"] = booking.bookingId;
        req["client_mail"] = currentUser.email;
        req["new_balance"] = updatedBalance;
        currentUser.balance=updatedBalance;

        m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
    }
}

void MainWindow::updateAdminDashboardUi()
{
    if (!adminHistoryLayout) return;

    QLayoutItem *child;
    while ((child = adminHistoryLayout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    struct GlobalBooking { QString user, hotel, dates, status; };
    QString filterText = adminSearchBar ? adminSearchBar->text().trimmed() : "";

    auto addCard = [this](const QString &user, const QString &hotel,
                          const QString &dates, const QString &status,
                          bool canCancel, int bookingIndex = -1)
    {
        QFrame *fr = new QFrame();
        fr->setStyleSheet("QFrame { background-color: transparent; border-radius: 10px; border: 1px solid rgba(255, 255, 255, 0.1); padding: 12px; margin-bottom: 5px; }");
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
        if (status == "finished")
            st->setStyleSheet("color:#22c55e; font-weight: bold; border: none; background: transparent; margin-right: 15px;");
        else if (status == "cancelled")
            st->setStyleSheet("color:#ef4444; font-weight: bold; border: none; background: transparent; margin-right: 15px;");
        else
            st->setStyleSheet("color:#3b82f6; font-weight: bold; border: none; background: transparent; margin-right: 15px;");
        cl->addWidget(st);

        if (canCancel && status != "cancelled" && status != "finished") {
            QPushButton *btnCancel = new QPushButton("Cancel Reservation");
            btnCancel->setStyleSheet(dangerBtnStyle + " padding: 5px 10px; font-size: 12px;");
            btnCancel->setCursor(Qt::PointingHandCursor);

            connect(btnCancel, &QPushButton::clicked, this, [this, bookingIndex]() {
                QDialog *confDialog = new QDialog(this);
                confDialog->setAttribute(Qt::WA_DeleteOnClose);
                confDialog->setWindowTitle("Confirm Action");
                confDialog->setFixedSize(380, 180);
                confDialog->setStyleSheet("QDialog { background-color: #0f172a; border: 1px solid #334155; border-radius: 8px; }");

                QVBoxLayout *vl = new QVBoxLayout(confDialog);
                vl->setContentsMargins(25, 25, 25, 25);

                QLabel *msgLabel = new QLabel("Are you sure you want to cancel this reservation?", confDialog);
                msgLabel->setStyleSheet("color: #f8fafc; font-size: 15px; font-weight: bold;");
                msgLabel->setWordWrap(true);
                msgLabel->setAlignment(Qt::AlignCenter);
                vl->addWidget(msgLabel);
                vl->addSpacing(15);

                QHBoxLayout *btnLayout = new QHBoxLayout();
                QPushButton *btnNo  = new QPushButton("No, Keep It",  confDialog);
                btnNo->setStyleSheet(secondaryBtnStyle + " padding: 8px 15px; font-size: 13px;");
                btnNo->setCursor(Qt::PointingHandCursor);
                QPushButton *btnYes = new QPushButton("Yes, Cancel", confDialog);
                btnYes->setStyleSheet(dangerBtnStyle   + " padding: 8px 15px; font-size: 13px;");
                btnYes->setCursor(Qt::PointingHandCursor);
                btnLayout->addWidget(btnNo);
                btnLayout->addWidget(btnYes);
                vl->addLayout(btnLayout);

                connect(btnYes, &QPushButton::clicked, confDialog, &QDialog::accept);
                connect(btnNo,  &QPushButton::clicked, confDialog, &QDialog::reject);

                if (confDialog->exec() == QDialog::Accepted) {
                    if (bookingIndex >= 0 && bookingIndex < userBookings.size()) {
                        QJsonObject userObj;
                        userObj["type"]     = "ADMIN_CANCEL_BOOKING";
                        userObj["id"]   = userBookings[bookingIndex].bookingId;
                        m_socketClient->sendMessage(QJsonDocument(userObj).toJson(QJsonDocument::Compact));
                    }
                }
            });
            cl->addWidget(btnCancel);
        }

        adminHistoryLayout->addWidget(fr);
    };

    for (int i = 0; i < userBookings.size(); ++i) {
        if (!filterText.isEmpty() && !userBookings[i].userName.contains(filterText, Qt::CaseInsensitive)) {
            continue;
        }

        addCard(userBookings[i].userName,
                userBookings[i].hotelName,
                userBookings[i].dateRange,
                userBookings[i].status,
                true,
                i);
    }
}

// backend messages
void MainWindow::handleBackendMessage(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (doc.isNull()) return;

    QJsonObject obj  = doc.object();
    QString type     = obj["type"].toString();

    // register
    if (type == "REGISTER_RESPONSE") {
        QString status    = obj["status"].toString();
        QString serverMsg = obj["message"].toString();
        if (status == "success") {
            QMessageBox::information(this, "Success", serverMsg);
            stackedWidget->setCurrentIndex(0);
        } else {
            QMessageBox::critical(this, "Registration Error", serverMsg);
        }
        return;
    }

    // login
    if (type == "LOGIN_RESPONSE") {
        QString status = obj["status"].toString();
        if (status == "success") {
            QJsonObject userData = obj["data"].toObject();
            if(userData["role"]==0){
                IdUser=userData["id"].toInt();
                currentUser.name    = userData["name"].toString();
                currentUser.email   = userData["email"].toString();
                currentUser.phone   = userData["phone"].toString();
                currentUser.address = userData["address"].toString();
                currentUser.country = userData["country"].toString();
                currentUser.dob     = userData["dob"].toString();
                currentUser.gender  = userData["gender"].toString();
                currentUser.balance = userData["balance"].toDouble();

                lblNameVal->setText(currentUser.name);
                lblEmailVal->setText(currentUser.email);
                lblPhoneVal->setText(currentUser.phone);
                lblDobVal->setText(currentUser.dob);
                lblCountryVal->setText(currentUser.country);
                lblGenderVal->setText(currentUser.gender);
                lblAddressVal->setText(currentUser.address);
                lblBalanceVal->setText(QString::number(currentUser.balance, 'f', 2) + " €");

                QJsonObject req;
                req["type"] = "GET_RENTALS";
                m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));

                stackedWidget->setCurrentIndex(2);
            }
            else
            {
                QString numeLocatie = userData["location_name"].toString();
                int idLocatie = userData["location_id"].toInt();
                currentUser.email = loginEmailInput->text().trimmed();
                currentUser.address = QString::number(idLocatie);
                if (lblAdminLocation) {
                    lblAdminLocation->setText("🏢 Managing Estate: " + numeLocatie);
                }

                QJsonObject req;
                req["type"] = "GET_LOCATION_BOOKINGS";
                req["location_id"] = idLocatie;
                m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));

                stackedWidget->setCurrentIndex(5);
            }
        } else {
            QMessageBox::warning(this, "Login Error", obj["message"].toString());
        }
        return;
    }

    // accomodations
    if (type == "GET_RENTALS") {
        QJsonArray dataArray = obj["data"].toArray();
        allAccommodations.clear();

        for (const QJsonValue &value : dataArray) {
            QJsonObject accObj = value.toObject();

            Accommodation acc;
            acc.id              = accObj["id"].toInt();
            acc.name            = accObj["name"].toString();
            acc.location        = accObj["location"].toString();
            acc.address         = accObj["address"].toString();
            acc.discountPercent = accObj["discount"].toDouble(0.0);
            acc.promoName       = accObj["promo_name"].toString();
            acc.imageSource     = accObj["image"].toString();

            if (accObj["rooms"].isArray()) {
                for (const QJsonValue &rVal : accObj["rooms"].toArray()) {
                    QJsonObject rObj = rVal.toObject();

                    Room r;
                    r.id        = rObj["id"].toInt();
                    r.type      = rObj["type"].toString();
                    r.basePrice = rObj["price"].toDouble();
                    r.beds      = rObj["beds"].toInt();
                    r.hasSofa   = rObj["hasSofa"].toBool();
                    r.imageSource = rObj["image"].toString();

                    if (rObj["facilities"].isArray()) {
                        for (const QJsonValue &fValue : rObj["facilities"].toArray()) {
                            Facility fac;
                            if (fValue.isString())       fac.name = fValue.toString();
                            else if (fValue.isObject())  fac.name = fValue.toObject()["name"].toString();
                            r.facilities.append(fac);
                        }
                    }
                    acc.rooms.append(r);
                }
            }
            allAccommodations.append(acc);
        }

        qDebug() << "Accommodations loaded:" << allAccommodations.size();
        populateAccommodations(searchBarInput ? searchBarInput->text() : "");
        return;
    }

    if (type == "FORCE_LOGOUT") {
        allAccommodations.clear();
        currentUser = UserInfo();
        goToLogin();
        return;
    }
    if (type == "GET_LOCATION_BOOKINGS_RESPONSE") {
        QJsonArray bookingsArray = obj["data"].toArray();
        userBookings.clear();

        for (auto &room : currentAccommodationInDetails.rooms) {
            room.bookedDates.clear();
        }

        for (const QJsonValue &value : bookingsArray) {
            QJsonObject bObj = value.toObject();
            BookingHistory b;
            b.bookingId  = bObj["id"].toInt();
            b.userName   = bObj["client_name"].toString();
            b.hotelName  = bObj["room_type"].toString();
            b.dateRange  = bObj["date_range"].toString();
            b.status     = bObj["status"].toString();
            userBookings.append(b);
            if (b.status != "cancelled") {
                int rId = bObj["room_id"].toInt();
                QDate start = QDate::fromString(bObj["raw_check_in"].toString(), "yyyy-MM-dd");
                QDate end = QDate::fromString(bObj["raw_check_out"].toString(), "yyyy-MM-dd");

                for (auto &room : currentAccommodationInDetails.rooms) {
                    if (room.id == rId) {
                        for (QDate d = start; d < end; d = d.addDays(1)) {
                            room.bookedDates.append(d);
                        }
                        break;
                    }
                }
            }
        }

        if (stackedWidget->currentIndex() == 4) {
            displayRooms(roomSearchBar ? roomSearchBar->text() : "");
        }
        else if (stackedWidget->currentIndex() == 5) {
            updateAdminDashboardUi();
        }

        return;
    }
    if (type == "GET_CLIENT_BOOKINGS_RESPONSE") {
        QJsonArray bookingsArray = obj["data"].toArray();
        userBookings.clear();
        for (const QJsonValue &value : bookingsArray) {
            QJsonObject bObj = value.toObject();
            BookingHistory b;
            b.userEmail  = currentUser.email;
            b.bookingId  = bObj["id"].toInt();
            b.hotelName  = bObj["hotel_name"].toString();
            b.roomType   = bObj["room_type"].toString();
            b.dateRange  = bObj["date_range"].toString();
            b.status     = bObj["status"].toString();
            b.rawCheckIn = bObj["raw_check_in"].toString();
            b.totalCost  = bObj["total_cost"].toDouble(0.0);

            userBookings.append(b);
        }
        updateBookingHistoryUi();
        return;
    }
    if (type == "ADMIN_CANCEL_BOOKING_RESPONSE") {
        QString status = obj["status"].toString();

        if (status == "success") {
            QMessageBox::information(this, "Success", "Reservation has been successfully cancelled!");

            QJsonObject req;
            req["type"] = "GET_LOCATION_BOOKINGS";
            req["location_id"] = currentUser.address.toInt();

            m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
        }
        else {
            QString serverMsg = obj["message"].toString();
            QMessageBox::critical(this, "Error", "Failed to cancel reservation: " + serverMsg);
        }
        return;
    }
    if (type == "CREATE_RESERVATION_RESPONSE") {
        QString status = obj["status"].toString();
        QString serverMsg = obj["message"].toString();

        if (status == "success") {
            currentUser.balance = obj["new_balance"].toDouble();
            if (lblBalanceVal) {
                lblBalanceVal->setText(QString::number(currentUser.balance, 'f', 2) + " €");
            }

            QMessageBox::information(this, "Success", serverMsg);
            QJsonObject req;
            req["type"] = "GET_LOCATION_BOOKINGS";
            req["location_id"] = currentAccommodationInDetails.id;
            m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
        }
        else {
            QMessageBox::critical(this, "Booking Error", serverMsg);
        }
        return;
    }

    if (type == "CLIENT_CANCEL_BOOKING_RESPONSE") {
        QString status = obj["status"].toString();
        QString serverMsg = obj["message"].toString();

        if (status == "success") {
            QMessageBox::information(this, "Success", "Reservation cancelled successfully!");
            if (lblBalanceVal) {
                lblBalanceVal->setText(QString::number(currentUser.balance, 'f', 2) + " €");
            }
            QJsonObject req;
            req["type"]  = "GET_CLIENT_BOOKINGS";
            req["id"] = IdUser;
            m_socketClient->sendMessage(QJsonDocument(req).toJson(QJsonDocument::Compact));
        } else {
            QMessageBox::critical(this, "Eroare la Anulare", serverMsg);
        }
        return;}

    if (type == "GET_RENTAL_IMAGES_RESPONSE") {
        int hotelId = obj["hotel_id"].toInt();
        QJsonArray roomsImages = obj["rooms_images"].toArray();

        // Actualizăm pozele direct în instanța curentă deschisă pe ecranul de detalii
        if (currentAccommodationInDetails.id == hotelId) {
            for (auto &room : currentAccommodationInDetails.rooms) {
                for (const QJsonValue &imgVal : roomsImages) {
                    QJsonObject imgObj = imgVal.toObject();
                    if (imgObj["room_id"].toInt() == room.id) {
                        room.imageSource = imgObj["image"].toString(); // Salvăm stringul Base64 sosit în cameră
                        break;
                    }
                }
            }
        }

        // Actualizăm și cache-ul global (allAccommodations) pentru consistența viitoare a datelor
        for (auto &acc : allAccommodations) {
            if (acc.id == hotelId) {
                for (auto &room : acc.rooms) {
                    for (const QJsonValue &imgVal : roomsImages) {
                        QJsonObject imgObj = imgVal.toObject();
                        if (imgObj["room_id"].toInt() == room.id) {
                            room.imageSource = imgObj["image"].toString();
                            break;
                        }
                    }
                }
                break;
            }
        }

        // Redesenăm interfața cu camere pe firul principal de execuție, acum având pozele salvate local
        QMetaObject::invokeMethod(this, [this]() {
            displayRooms(roomSearchBar ? roomSearchBar->text() : "");
        }, Qt::QueuedConnection);
        return;
    }
}
void MainWindow::filterAdminBookings(const QString &query)
{
    Q_UNUSED(query);
    updateAdminDashboardUi();
}