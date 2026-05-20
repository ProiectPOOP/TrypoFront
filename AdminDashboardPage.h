#ifndef ADMINDASHBOARDPAGE_H
#define ADMINDASHBOARDPAGE_H

#include <QVBoxLayout>
#include <QWidget>

class MainWindow;

class AdminDashboardPage
{
public:
    static QWidget *createWidget(MainWindow *mw, QVBoxLayout *&adminHistoryLayout);
};

#endif // ADMINDASHBOARDPAGE_H
