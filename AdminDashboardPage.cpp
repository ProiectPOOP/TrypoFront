#include "AdminDashboardPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

QWidget *AdminDashboardPage::createWidget(MainWindow *mw, QVBoxLayout *&adminHistoryLayout)
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
    // Admin logout goes straight to login without sending FORCE_LOGOUT
    // (admin never authenticated against the C++ server).
    QObject::connect(btnLogout, &QPushButton::clicked, mw, &MainWindow::adminLogout);

    h->addWidget(pt);
    h->addStretch();
    h->addWidget(btnLogout);
    ml->addLayout(h);
    ml->addSpacing(20);

    QLabel *sub = new QLabel("Global Booking History (All Users)");
    sub->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6; margin-bottom: 10px;");
    ml->addWidget(sub);

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
