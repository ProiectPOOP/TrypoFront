#ifndef ADMINDASHBOARDPAGE_H
#define ADMINDASHBOARDPAGE_H

#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

class MainWindow;

class AdminDashboardPage
{
public:
    static QWidget *createWidget(MainWindow *mw, QVBoxLayout *&adminHistoryLayout, QLabel *&adminLocationLabel);
};

#endif // ADMINDASHBOARDPAGE_H
