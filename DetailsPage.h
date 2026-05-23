#ifndef DETAILSPAGE_H
#define DETAILSPAGE_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow;

class DetailsPage
{
public:
    static QWidget *createWidget(MainWindow *mw,
                                 QLabel *&detName,
                                 QLabel *&detAddress,
                                 QLabel *&detPromo,
                                 QLineEdit *&roomSearchBar,
                                 QVBoxLayout *&roomsLayout,
                                 QCheckBox *&cbBalcony,
                                 QCheckBox *&cbFridge,
                                 QCheckBox *&cbAC,
                                 QCheckBox *&cbTV,
                                 QCheckBox *&cbWifi,
                                 QCheckBox *&cbSofa);
};

#endif
