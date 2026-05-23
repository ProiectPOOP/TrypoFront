#include "LoginPage.h"
#include "MainWindow.h"
#include "Styles.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

QWidget *LoginPage::createWidget(MainWindow *mw,
                                 QLineEdit *&loginEmailInput,
                                 QLineEdit *&loginPasswordInput)
{
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setAlignment(Qt::AlignCenter);

    QLabel *t = new QLabel("Welcome back to Trypo", w);

    // 1. Alinierea textului pe centru în interiorul widget-ului
    t->setAlignment(Qt::AlignCenter);

    // 2. Aplicarea unui stil modern (font mai gros, culori neon din paleta ta și spațiere între litere)
    t->setStyleSheet(
        "font-family: 'Segoe UI', Arial, sans-serif;"
        "font-size: 32px;"
        "font-weight: 900;"          // Extrem de bold, stil cyberpunk/modern
        "color: #38bdf8;"            // Accentul Cyan Neon folosit în Styles.h
        "letter-spacing: 1px;"       // Spațiere elegantă între litere
        "margin-bottom: 30px;"       // Spațiu generos sub titlu
        );

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

    QObject::connect(lb, &QPushButton::clicked, mw, &MainWindow::processLogin);
    QObject::connect(rb, &QPushButton::clicked, mw, &MainWindow::goToRegister);
    return w;
}
