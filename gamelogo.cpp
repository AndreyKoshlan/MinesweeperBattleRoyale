#include "gamelogo.h"

GameLogo::GameLogo() {
    QPixmap logo_pixmap(":/textures/logo");
    this->setPixmap(logo_pixmap);
}
