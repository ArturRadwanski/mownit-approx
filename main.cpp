#include <QApplication>
#include <QPushButton>
#include "window/main_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    main_window *w= new main_window();
    w->show();
    return app.exec();
}