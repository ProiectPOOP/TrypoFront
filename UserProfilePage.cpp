#include "UserProfilePage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

QWidget *UserProfilePage::createWidget(MainWindow *mw,
                                       QLabel *&lblNameVal,
                                       QLabel *&lblEmailVal,
                                       QLabel *&lblPhoneVal,
                                       QLabel *&lblDobVal,
                                       QLabel *&lblCountryVal,
                                       QLabel *&lblGenderVal,
                                       QLabel *&lblAddressVal,
                                       QLabel *&lblBalanceVal,
                                       QVBoxLayout *&historyLayout)
{
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(40, 30, 40, 30);

    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *bb = new QPushButton("← Back to Explore");
    bb->setStyleSheet(secondaryBtnStyle);
    QObject::connect(bb, &QPushButton::clicked, mw, &MainWindow::backToMainApp);

    QLabel *pt = new QLabel("Account Settings");
    pt->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    h->addWidget(bb);
    h->addSpacing(10);
    h->addWidget(pt);
    h->addStretch();
    ml->addLayout(h);
    ml->addSpacing(15);

    QHBoxLayout *splitLayout = new QHBoxLayout();
    splitLayout->setSpacing(50);

    // Left column: customer info
    QVBoxLayout *leftColumn = new QVBoxLayout();
    leftColumn->setAlignment(Qt::AlignTop);

    QLabel *sub1 = new QLabel("Customer Information");
    sub1->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    leftColumn->addWidget(sub1);

    QWidget *detW = new QWidget();
    QFormLayout *f = new QFormLayout(detW);
    f->setContentsMargins(0, 5, 0, 30);
    f->setVerticalSpacing(15);

    QString vs = "color: white; font-size: 15px;";
    lblNameVal    = new QLabel("N/A"); lblNameVal->setStyleSheet(vs);
    lblEmailVal   = new QLabel("N/A"); lblEmailVal->setStyleSheet(vs);
    lblPhoneVal   = new QLabel("N/A"); lblPhoneVal->setStyleSheet(vs);
    lblDobVal     = new QLabel("N/A"); lblDobVal->setStyleSheet(vs);
    lblCountryVal = new QLabel("N/A"); lblCountryVal->setStyleSheet(vs);
    lblGenderVal  = new QLabel("N/A"); lblGenderVal->setStyleSheet(vs);
    lblAddressVal = new QLabel("N/A"); lblAddressVal->setStyleSheet(vs);
    lblBalanceVal = new QLabel("0.00 €");
    lblBalanceVal->setStyleSheet("color: #22c55e; font-size: 16px; font-weight: bold;");

    QString ls = "font-weight: bold; color: #94a3b8; font-size: 14px;";
    auto addRow = [&](const QString &lbl, QLabel *val) {
        QLabel *l = new QLabel(lbl); l->setStyleSheet(ls);
        f->addRow(l, val);
    };
    addRow("Name:",            lblNameVal);
    addRow("Email:",           lblEmailVal);
    addRow("Phone:",           lblPhoneVal);
    addRow("Birth Date:",      lblDobVal);
    addRow("Country:",         lblCountryVal);
    addRow("Gender:",          lblGenderVal);
    addRow("Address:",         lblAddressVal);
    addRow("Current Balance:", lblBalanceVal);
    leftColumn->addWidget(detW);

    // Right column: booking history
    QVBoxLayout *rightColumn = new QVBoxLayout();
    rightColumn->setAlignment(Qt::AlignTop);

    QLabel *sub2 = new QLabel("Your Booking History");
    sub2->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    rightColumn->addWidget(sub2);
    rightColumn->addSpacing(5);

    QScrollArea *hsa = new QScrollArea();
    hsa->setWidgetResizable(true);
    hsa->setStyleSheet("QScrollArea, QScrollArea > QWidget { background: transparent; border: none; }");

    QWidget *hc = new QWidget();
    hc->setStyleSheet("background: transparent;");
    historyLayout = new QVBoxLayout(hc);
    historyLayout->setAlignment(Qt::AlignTop);

    hsa->setWidget(hc);
    rightColumn->addWidget(hsa);

    splitLayout->addLayout(leftColumn, 1);
    splitLayout->addLayout(rightColumn, 1);
    ml->addLayout(splitLayout);
    return w;
}
