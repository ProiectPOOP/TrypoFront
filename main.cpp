#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window; // crearea ferestrei principale
    window.resize(500, 600); // dimensiunea implicita a ferestrei
    window.show();

    return app.exec(); // bucla infinita, interactiunea cu user-ul
}