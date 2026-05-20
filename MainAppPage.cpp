#include "MainAppPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

QWidget *MainAppPage::createWidget(MainWindow *mw,
                                   QLineEdit *&searchBarInput,
                                   QWidget *&accommodationsContainer,
                                   QVBoxLayout *&accommodationsLayout)
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
    QObject::connect(searchBarInput, &QLineEdit::textChanged, mw, &MainWindow::filterAccommodations);

    QPushButton *pb = new QPushButton("👤 My Profile");
    pb->setStyleSheet(secondaryBtnStyle);
    pb->setCursor(Qt::PointingHandCursor);

    QPushButton *lb = new QPushButton("Logout");
    lb->setStyleSheet(dangerBtnStyle);
    lb->setCursor(Qt::PointingHandCursor);

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

    // Do NOT request accommodations here at construction time.
    // The user is not logged in yet when this widget is built, so a request
    // sent now would be unauthenticated. We send GET_ACCOMMODATIONS inside
    // handleBackendMessage after a successful LOGIN_RESPONSE instead.

    QObject::connect(pb, &QPushButton::clicked, mw, &MainWindow::goToUserProfile);
    QObject::connect(lb, &QPushButton::clicked, mw, &MainWindow::goToLogin);
    return w;
}
