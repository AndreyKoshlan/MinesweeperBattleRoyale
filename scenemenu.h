#ifndef MAINMENU_H
#define MAINMENU_H

#include <QGraphicsScene>
#include <QVector>
#include <QDebug>
#include <QGraphicsItem>
#include <QVector>
#include "gameobject.h"
#include "gamelogo.h"
#include "gamebutton.h"
#include "gameslider.h"
#include "gamelabel.h"
#include "gametextedit.h"
#include "gamelist.h"
#include "gametable.h"

const QVector<QString> difficulties = {
    "Легкий", "Нормальный", "Сложный"
};
const int player_limits = 16;
const int bot_limits = 7;
const QString default_host = "localhost";
const QString default_port = "29965";
const QString local_host = "localhost";
const QString local_port = "29965";

class SceneMenu;

class Slide: public QObject {
public:
    Slide(SceneMenu* scene) {
        activeScene = scene;
        static Slide* currentSlide;
        if (currentSlide) {
            delete currentSlide;
        }
        currentSlide = this;
    }
    ~Slide() {
        ((QGraphicsScene*)activeScene)->clear();
    }
protected:
    SceneMenu* activeScene;
};

class SlideGameOver: public Slide {
    Q_OBJECT
public:
    GameLogo* logo;
    GameLabel* label;

    GameTable* table;

    GameButton* button_ok;

    SlideGameOver(SceneMenu* scene);
public slots:
    void next();
};

class SlideLobby: public Slide {
    Q_OBJECT
public:
    GameLogo* logo;
    GameLabel* label;

    GameList* list;

    GameButton* button_start;
    GameButton* button_cancel;

    SlideLobby(SceneMenu* scene, bool admin);
    ~SlideLobby();
public slots:
    void start();
    void cancel();
};

class SlideCreate: public Slide {
    Q_OBJECT
public:
    GameLabel* label;

    GameLabel* label_host;
    GameTextEdit* edit_host;
    GameLabel* label_port;
    GameTextEdit* edit_port;
    GameLabel* label_name;
    GameTextEdit* edit_name;
    GameLabel* label_players;
    GameSlider* slider_players;
    GameLabel* label_bots;
    GameSlider* slider_bots;

    GameButton* button_create;
    GameButton* button_cancel;

    int players;

    SlideCreate(SceneMenu* scene);
public slots:
    void create();
    void cancel();
};

class SlideConnection: public Slide {
    Q_OBJECT
public:
    GameLabel* label;
    GameLabel* label_host;
    GameTextEdit* edit_host;
    GameLabel* label_port;
    GameTextEdit* edit_port;
    GameLabel* label_name;
    GameTextEdit* edit_name;
    GameLogo* logo;

    GameButton* button_connect;
    GameButton* button_cancel;

    SlideConnection(SceneMenu* scene);
public slots:
    void connectToServer();
    void cancel();
};

class SlideMultiplayer: public Slide {
    Q_OBJECT
public:
    GameLabel* label;
    GameButton* button_create;
    GameButton* button_connect;
    GameButton* button_cancel;
    GameLogo* logo;

    SlideMultiplayer(SceneMenu* scene);
public slots:
    void createServer();
    void connectToServer();
    void cancel();
};

class SlideOffline: public Slide {
    Q_OBJECT
public:
    GameLabel* label_players;
    GameSlider* slider_players;
    GameLabel* label_diff;
    GameSlider* slider_diff;
    GameButton* button_start;
    GameButton* button_cancel;
    GameLogo* logo;

    int players;
    int diff;

    SlideOffline(SceneMenu* scene);
public slots:
    void begin();
    void cancel();
};

class SlideMainMenu: public Slide {
    Q_OBJECT
public:
    GameButton* button_start;
    GameButton* button_online;
    GameButton* button_exit;
    GameLogo* logo;
    SlideMainMenu(SceneMenu* scene);
public slots:
    void startGame();
    void startMulti();
    void exit();
};

struct Slides {
    SlideMainMenu* MainMenu;
    SlideOffline* offline;
    SlideMultiplayer* multiplayer;
    SlideConnection* connection;
    SlideCreate* creation;
    SlideLobby* lobby;
    SlideGameOver* gameover;
};

class SceneMenu : public QGraphicsScene {
    Q_OBJECT
public:
    Slides* slides;

    /* layout methodes */
    double getItemsOffset() const {return 1.16;};
    int getCenterX(int width);
    int getCenterY(int height);
    QPoint getCenter(int width, int height);
    QPoint verticalLayout(QRectF back_rect, int width);
    QPoint verticalLayout(GameLogo* logo, int width);
    QPair<QPoint, QPoint> horizontalLayout(int top, int width1, int width2);

    /* item constructor */
    void placeGameObject(GameObject* a, GameObject* b, QSize size = {0, 0}, QPoint offset = {0, 0});
    GameButton* placeGameButton(GameButton* btn, QString text, GameObject* gameobj,
                                 QSize size = {0, 0}, QPoint offset = {0, 0});
    GameLabel* placeGameLabel(GameLabel* label, QString text, GameObject* gameobj, QPoint offset = {0, 0});
    GameSlider* placeGameSlider(GameSlider* slider, int min, int max, GameObject* gameobj,
                                QSize size = {0, 0}, QPoint offset = {0, 0});
    GameTextEdit* placeGameTextEdit(GameTextEdit* edit, QString text, GameObject* gameobj,
                                    QSize size = {0, 0}, QPoint offset = {0, 0});
    GameList* placeGameList(GameList* list, QStringList items, GameObject* gameobj,
                            QSize size = {0, 0}, QPoint offset = {0, 0});
    GameTable* placeGameTable(GameTable* table, GameObject* gameobj,
                            QSize size = {0, 0}, QPoint offset = {0, 0});

    /* item layout */
    void alignToWidth(GameObject* a, GameObject* b, int distance = 15, QPoint offset1 = {0, 0}, QPoint offset2 = {0, 0}, GameObject* adjustobj = nullptr);

    /* constructors */
    SceneMenu();
signals:
    void signalCreate(QString host, int port, QString name, int players, int bots, int diff, bool lobby);
    bool signalConnect(QString host, int port, QString name, bool admin = false, bool lobby = false);
    void signalDisconnect();
    void signalStart(int bots, int diff);
};

#endif // MAINMENU_H
