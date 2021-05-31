#include "scenegame.h"

SceneGame::SceneGame(Client* client) {
    field = new GameField(this);
    addItem(field);
    this->client = client;
}
