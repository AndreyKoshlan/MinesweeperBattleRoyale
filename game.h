#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>

#include "scenemenu.h"
#include "scenegame.h"
#include "client.h"

class QCustomGraphicsView: public QGraphicsView {
public:
    QCustomGraphicsView() {
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setMinimumSize(640, 480);
        //setStyleSheet("background-image : url(\":/textures/background\"); background-position: center;");
        setStyleSheet("background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 #FF4F8E, stop:1 #834d9b);");
    }

private:
    void resizeEvent(QResizeEvent *) {
        fitInView(0, 0, 1000, 1000, Qt::KeepAspectRatio);
        centerOn(0, 0);
    }
};

class Game: public QObject {
public:
    Game();
    QCustomGraphicsView* view;
    SceneMenu* menu;
    SceneGame* gamescene;
    Client* client;
public slots:
    void createServer(QString host, int port, QString name, int players, int bots, int diff, bool lobby);
    bool connectToServer(QString host, int port, QString name, bool admin = false, bool lobby = false);
    void startGame(int bots, int diff);
    void disconnectFromServer();
    void disconnected();
    void refreshUserList(PlayerList pl);
    void refreshMineCounter(int mines);
    void refreshMap(QByteArray buff);
    void suspend(int time);
    void fail();
    void gameOver(PlayerList pl);
    void lfield(QPoint cell);
    void rfield(QPoint cell);
};

#endif // GAME_H
