#include <QApplication>
#include <QFontDatabase>

#include "game.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/fonts/Bainsley");
    Game *game = new Game();
    return a.exec();
}
