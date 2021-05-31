#include "gametable.h"
#include "game.h"
#include "gamefield.h"

void Game::createServer(QString host, int port, QString name, int players, int bots, int diff, bool lobby) {
    client->createServer(host, port, players);
    if (connectToServer(host, port, name, true, lobby)) {
        if (!lobby) {
            startGame(bots, diff);
        }
    }
}

bool Game::connectToServer(QString host, int port, QString name, bool admin, bool lobby) {
    client->admin = admin;
    if (client->connect(host, port)) {
        if (client->handshake(name)) {
            client->listen(true);
            if (lobby) {
                menu->slides->lobby = new SlideLobby(menu, admin);
            }
            return true;
        }
    }
    return false;
}

/* message 0x00 from server
 * refreshing lobby user list */
void Game::refreshUserList(PlayerList pl) {
    if (menu->slides->lobby) {
        if (menu->slides->lobby->list) {
            menu->slides->lobby->list->clear();
            for (auto x : pl) {
                menu->slides->lobby->list->addItem(x.player);
            }
        }
    }
}

/* user wants to disconnect */
void Game::disconnectFromServer() {
    client->disconnect();
}

/* the connection was broken by user or server */
void Game::disconnected() {
    view->setScene(menu);
    menu->slides->MainMenu = new SlideMainMenu(menu);
}

/* user clicked start button */
void Game::startGame(int bots, int diff) {
    client->sendMessageStart(bots, diff);
}

/* message 0x01 from server
 * start the game and init map or refresh map */
void Game::refreshMap(QByteArray buff) {
    if (view->scene() != gamescene) {
        qDebug() << "Game started";
        view->setScene(gamescene);
        gamescene->field->suspended = false;
        gamescene->field->failed = false;
    }
    gamescene->field->LoadFromRawData(buff.data()+1, buff.size());
}

/* message 0x02 from server
 * show message about suspending user */
void Game::suspend(int time) {
    gamescene->field->suspend(time);
}

/* message 0x03 from server
 * Called when a player fails the game and stops his actions */
void Game::fail() {
    gamescene->field->fail();
}

/* message 0x04 from server
 * Finish game */
void Game::gameOver(PlayerList pl) {
    client->disconnect();
    view->setScene(menu);
    menu->slides->gameover = new SlideGameOver(menu);

    /* prepare table results */
    GameTable* table = menu->slides->gameover->table;
    table->setRowCount(pl.size());
    table->setColumnCount(4);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Игрок"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Открыто"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Мины"));
    for (int i = 0; i < pl.size(); i++) {
        Player player = pl[i];
        if (!pl[i].failed) {
            QIcon icon(":/textures/crown");
            QTableWidgetItem* iconItem = new QTableWidgetItem;
            iconItem->setIcon(icon);
            table->setIconSize({32, 32});
            table->setItem(i, 0, iconItem);
        }
        table->setItem(i, 1, new QTableWidgetItem(player.player));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(player.opened)));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(player.mined)));
    }
    table->setColumnWidth(0, 36);
    table->setColumnWidth(1, 164);
    table->setColumnWidth(2, 152);
    table->setColumnWidth(3, 152);
}

/* message 0x05 from server
 * Refresh mine count */
void Game::refreshMineCounter(int mines) {
    gamescene->field->setMineCount(mines);
}

/* user clicked left button on the field */
void Game::lfield(QPoint cell) {
    client->sendMessageClick(0, cell);
}

/* user clicked right button on the field */
void Game::rfield(QPoint cell) {
    client->sendMessageClick(1, cell);
}

Game::Game()
    :   view(new QCustomGraphicsView())
    ,   menu(new SceneMenu())
    ,   client(new Client())
{
    gamescene = new SceneGame(client);
    connect(menu, &SceneMenu::signalCreate, this, &Game::createServer);
    connect(menu, &SceneMenu::signalConnect, this, &Game::connectToServer);
    connect(menu, &SceneMenu::signalDisconnect, this, &Game::disconnectFromServer);
    connect(menu, &SceneMenu::signalStart, this, &Game::startGame);
    connect(client, &Client::msgUserList, this, &Game::refreshUserList);
    connect(client, &Client::msgMap, this, &Game::refreshMap);
    connect(client, &Client::msgSuspend, this, &Game::suspend);
    connect(client, &Client::msgFail, this, &Game::fail);
    connect(client, &Client::msgOver, this, &Game::gameOver);
    connect(client, &Client::msgDisconnect, this, &Game::disconnected);
    connect(client, &Client::msgGetMines, this, &Game::refreshMineCounter);
    connect(gamescene->field, &GameField::leftClick, this, &Game::lfield);
    connect(gamescene->field, &GameField::rightClick, this, &Game::rfield);
    view->setScene(menu);
    view->show();
}
