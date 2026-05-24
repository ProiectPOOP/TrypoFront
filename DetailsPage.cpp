#include "DetailsPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

QWidget *DetailsPage::createWidget(MainWindow *mw,
                                   QLabel *&detName,
                                   QLabel *&detAddress,
                                   QLabel *&detPromo,
                                   QLineEdit *&roomSearchBar,
                                   QVBoxLayout *&roomsLayout,
                                   QCheckBox *&cbBalcony,
                                   QCheckBox *&cbFridge,
                                   QCheckBox *&cbAC,
                                   QCheckBox *&cbTV,
                                   QCheckBox *&cbWifi,
                                   QCheckBox *&cbSofa,
                                   QLabel *&detImage)
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(30, 30, 30, 30);

    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *bb = new QPushButton("← Back");
    bb->setStyleSheet(secondaryBtnStyle);
    QObject::connect(bb, &QPushButton::clicked, mw, &MainWindow::backToMainApp);

    detName = new QLabel("Hotel Details");
    detName->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    h->addWidget(bb);
    h->addSpacing(20);
    h->addWidget(detName);
    h->addStretch();

    detImage = new QLabel();
    detImage->setFixedSize(160, 90); // format landscape pentru colt
    detImage->setStyleSheet("border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 8px; background: #0f172a;");
    h->addWidget(detImage);

    ml->addLayout(h);

    detAddress = new QLabel("");
    detAddress->setStyleSheet("color: #94a3b8; font-size: 14px; margin-left: 85px;");
    ml->addWidget(detAddress);

    detPromo = new QLabel("");
    ml->addWidget(detPromo);

    // Facility filters
    QHBoxLayout *filtersLayout = new QHBoxLayout();
    QLabel *filterLbl = new QLabel("Facilities:");
    filterLbl->setStyleSheet("color: #94a3b8; font-weight: bold; font-size: 14px;");
    filtersLayout->addWidget(filterLbl);

    cbBalcony = new QCheckBox("Balcony"); cbBalcony->setStyleSheet(checkBoxStyle);
    cbFridge  = new QCheckBox("Fridge");  cbFridge->setStyleSheet(checkBoxStyle);
    cbAC      = new QCheckBox("AC");      cbAC->setStyleSheet(checkBoxStyle);
    cbTV      = new QCheckBox("TV");      cbTV->setStyleSheet(checkBoxStyle);
    cbWifi    = new QCheckBox("WiFi");    cbWifi->setStyleSheet(checkBoxStyle);
    cbSofa    = new QCheckBox("Sofa");    cbSofa->setStyleSheet(checkBoxStyle);

    filtersLayout->addWidget(cbBalcony);
    filtersLayout->addWidget(cbFridge);
    filtersLayout->addWidget(cbAC);
    filtersLayout->addWidget(cbTV);
    filtersLayout->addWidget(cbWifi);
    filtersLayout->addWidget(cbSofa);
    filtersLayout->addStretch();

    ml->addSpacing(10);
    ml->addLayout(filtersLayout);

    // functie lambda ce actualizeaza lista camerei ori de cate ori bifam/debifam un filtru
    auto updateRoomFilters = [mw, &roomSearchBar]() {
        mw->filterRooms(roomSearchBar ? roomSearchBar->text() : "");
    };
    QObject::connect(cbBalcony, &QCheckBox::checkStateChanged, mw, updateRoomFilters);
    QObject::connect(cbFridge,  &QCheckBox::checkStateChanged, mw, updateRoomFilters);
    QObject::connect(cbAC,      &QCheckBox::checkStateChanged, mw, updateRoomFilters);
    QObject::connect(cbTV,      &QCheckBox::checkStateChanged, mw, updateRoomFilters);
    QObject::connect(cbWifi,    &QCheckBox::checkStateChanged, mw, updateRoomFilters);
    QObject::connect(cbSofa,    &QCheckBox::checkStateChanged, mw, updateRoomFilters);

    roomSearchBar = new QLineEdit();
    roomSearchBar->setPlaceholderText("Filter rooms by text...");
    roomSearchBar->setStyleSheet(lineEditStyle);
    QObject::connect(roomSearchBar, &QLineEdit::textChanged, mw, &MainWindow::filterRooms);
    ml->addSpacing(10);
    ml->addWidget(roomSearchBar);
    ml->addSpacing(20);

    QLabel *roomT = new QLabel("Available Rooms");
    roomT->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    ml->addWidget(roomT);

    QScrollArea *sa = new QScrollArea();
    sa->setWidgetResizable(true);
    sa->setStyleSheet("QScrollArea, QScrollArea > QWidget { background: transparent; border: none; }");

    QWidget *rc = new QWidget();
    rc->setStyleSheet("background: transparent;");
    roomsLayout = new QVBoxLayout(rc);
    roomsLayout->setAlignment(Qt::AlignTop);
    sa->setWidget(rc);
    ml->addWidget(sa);
    return w;
}
