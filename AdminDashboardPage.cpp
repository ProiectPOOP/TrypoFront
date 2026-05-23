#include "AdminDashboardPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

QWidget *AdminDashboardPage::createWidget(MainWindow *mw, QVBoxLayout *&adminHistoryLayout, QLabel *&adminLocationLabel)
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

    // --- BUCATA NOUĂ: Inițializăm label-ul pe care îl vom modifica din MainWindow ---
    adminLocationLabel = new QLabel("Loading location...", w);
    adminLocationLabel->setStyleSheet("font-size: 15px; color: #64748b; font-style: italic; margin-top: -5px;");
    ml->addWidget(adminLocationLabel);
    // -------------------------------------------------------------------------------

    ml->addSpacing(20);

    QLabel *sub = new QLabel("Booking History");
    sub->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6; margin-bottom: 10px;");
    ml->addWidget(sub);

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