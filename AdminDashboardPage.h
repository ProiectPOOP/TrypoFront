#ifndef ADMINDASHBOARDPAGE_H
#define ADMINDASHBOARDPAGE_H

#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>

class MainWindow;

class AdminDashboardPage
{
public:
    static QWidget *createWidget(MainWindow *mw, QVBoxLayout *&adminHistoryLayout, QLabel *&adminLocationLabel, QLineEdit *&adminSearchBar);
};

#endif
