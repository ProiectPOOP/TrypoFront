#ifndef MAINAPPPAGE_H
#define MAINAPPPAGE_H

#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow;

class MainAppPage
{
public:
    static QWidget *createWidget(MainWindow *mw,
                                 QLineEdit *&searchBarInput,
                                 QWidget *&accommodationsContainer,
                                 QVBoxLayout *&accommodationsLayout);
};

#endif
