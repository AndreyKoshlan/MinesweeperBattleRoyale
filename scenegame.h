#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include "client.h"
#include "gamefield.h"

class SceneGame : public QGraphicsScene
{
    Q_OBJECT
public:
    GameField* field;
    SceneGame(Client* client);
private:
    Client* client;
};

#endif // GAMESCENE_H
