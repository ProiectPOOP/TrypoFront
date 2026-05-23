#include "AdminDashboardPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

QWidget *AdminDashboardPage::createWidget(MainWindow *mw, QVBoxLayout *&adminHistoryLayout, QLabel *&adminLocationLabel, QLineEdit *&adminSearchBar)
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(40, 30, 40, 30);

    QHBoxLayout *h = new QHBoxLayout();
    QLabel *pt = new QLabel("🔑 Admin Control Panel");
    pt->setStyleSheet("font-size: 24px; font-weight: bold; color: #ef4444;");

    QPushButton *btnLogout = new QPushButton("Logout Admin");
    btnLogout->setStyleSheet(dangerBtnStyle);
    btnLogout->setCursor(Qt::PointingHandCursor);
    QObject::connect(btnLogout, &QPushButton::clicked, mw, &MainWindow::adminLogout);

    h->addWidget(pt);
    h->addStretch();
    h->addWidget(btnLogout);
    ml->addLayout(h);

    // label-ul pentru managementul locatiei
    adminLocationLabel = new QLabel("Loading location...", w);
    adminLocationLabel->setStyleSheet("font-size: 15px; color: #64748b; font-style: italic; margin-top: -5px;");
    ml->addWidget(adminLocationLabel);

    ml->addSpacing(20);

    // bara de cautare pentru filtrare locala dupa client
    adminSearchBar = new QLineEdit();
    adminSearchBar->setPlaceholderText("🔍 Search reservations by customer name...");
    adminSearchBar->setStyleSheet(lineEditStyle);
    QObject::connect(adminSearchBar, &QLineEdit::textChanged, mw, &MainWindow::filterAdminBookings);
    ml->addWidget(adminSearchBar);

    ml->addSpacing(15);

    QLabel *sub = new QLabel("Booking History");
    sub->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6; margin-bottom: 10px;");
    ml->addWidget(sub);

    // zona de scroll complet transparenta (fara blocul negru)
    QScrollArea *sa = new QScrollArea();
    sa->setWidgetResizable(true);
    sa->setStyleSheet("QScrollArea, QScrollArea > QWidget { background: transparent; border: none; }");

    QWidget *container = new QWidget();
    container->setStyleSheet("background: transparent;");
    adminHistoryLayout = new QVBoxLayout(container);
    adminHistoryLayout->setAlignment(Qt::AlignTop);

    sa->setWidget(container);
    ml->addWidget(sa);

    return w;
}