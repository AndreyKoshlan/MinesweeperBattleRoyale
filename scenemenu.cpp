#include <QGraphicsPixmapItem>
#include <QApplication>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QGraphicsView>
#include <QDateTime>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

#include "scenemenu.h"
#include "gamebutton.h"
#include "gamelogo.h"

/*************************** SLIDE GAMEOVER ********************************/

void SlideGameOver::next() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->MainMenu = new SlideMainMenu(activeScene);
}

SlideGameOver::SlideGameOver(SceneMenu* scene): Slide(scene) {
    logo = new GameLogo();
    scene->addItem(logo);

    label = scene->placeGameLabel(label, "Результаты игры", logo, {0, -40});

    table = scene->placeGameTable(table, logo, {0, 0}, {0, 40});

    button_ok = scene->placeGameButton(button_ok, "ОК", table);
    connect(button_ok, &GameButton::clicked, this, &SlideGameOver::next);
}

/***************************** SLIDE LOBBY *********************************/

void SlideLobby::start() {
    //todo: make bots in online
    emit activeScene->signalStart(0, 0);
}

void SlideLobby::cancel() {
    emit activeScene->signalDisconnect();
}

SlideLobby::SlideLobby(SceneMenu* scene, bool admin): Slide(scene) {
    logo = new GameLogo();
    scene->addItem(logo);

    label = scene->placeGameLabel(label, "Ожидание готовности игроков", logo, {0, -40});

    list = scene->placeGameList(list, {}, label, {0, 0}, {0, 40});

    button_cancel = scene->placeGameButton(button_cancel, "Выход", list);
    connect(button_cancel, &GameButton::clicked, this, &SlideLobby::cancel);
    if (admin) {
        button_start = scene->placeGameButton(button_start, "Начать", list);
        connect(button_start, &GameButton::clicked, this, &SlideLobby::start);
        scene->alignToWidth(button_start, button_cancel, 15, {0, 20}, {0, 20}, list);
    }
}

SlideLobby::~SlideLobby() {
   activeScene->slides->lobby = nullptr;
}

/***************************** SLIDE CREATE ********************************/

void SlideCreate::create() {
    QString host = edit_host->text();
    int port = edit_port->text().toInt();
    QString name = edit_name->text();
    emit activeScene->signalCreate(host, port, name, players, 0, 0, true);
}

void SlideCreate::cancel() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->multiplayer = new SlideMultiplayer(activeScene);
}

SlideCreate::SlideCreate(SceneMenu* scene): Slide(scene) {
    label = scene->placeGameLabel(label, "Создание комнаты", nullptr);
    QFont font = label->font();
    font.setPointSize(42);
    label->setFont(font);
    label->gameObjectMove(scene->verticalLayout(label->gameObjectRect(), label->gameObjectRect().width()));

    label_host = scene->placeGameLabel(label_host, "Хост:", label);
    edit_host = scene->placeGameTextEdit(edit_host, default_host, label);
    scene->alignToWidth(label_host, edit_host, 10, {0, 25}, {0, 30});

    label_port = scene->placeGameLabel(label_port, "Порт:", label_host);
    edit_port = scene->placeGameTextEdit(edit_port, default_port, edit_host);
    label_port->gameObjectSetLeft(label_host->gameObjectLeft(), {0, 15});
    edit_port->gameObjectSetLeft(edit_host->gameObjectLeft(), {0, 20});

    label_name = scene->placeGameLabel(label_name, "Имя:", label_port);
    edit_name = scene->placeGameTextEdit(edit_name, "Player", edit_port);
    label_name->gameObjectSetLeft(label_host->gameObjectLeft(), {0, 15});
    edit_name->gameObjectSetLeft(edit_host->gameObjectLeft(), {0, 20});

    label_players = scene->placeGameLabel(label_players, "Количество игроков: X", edit_name, {0, 20});
    slider_players = scene->placeGameSlider(slider_players, 2, player_limits, label_players, {500, 68});
    slider_players->setCounter(label_players, "Количество игроков: ", &players, true, scene);

    button_create = scene->placeGameButton(button_create, "Войти", slider_players);
    connect(button_create, &GameButton::clicked, this, [this](){
        this->create();
    });

    button_cancel = scene->placeGameButton(button_cancel, "Отмена", slider_players);
    connect(button_cancel, &GameButton::clicked, this, &SlideCreate::cancel);
    scene->alignToWidth(button_create, button_cancel, 15, {0, 40}, {0, 40}, slider_players);
}

/***************************** SLIDE CONNECTION ****************************/

void SlideConnection::connectToServer() {
    QString host = edit_host->text();
    int port = edit_port->text().toInt();
    QString name = edit_name->text();
    emit activeScene->signalConnect(host, port, name, false, true);
}

void SlideConnection::cancel() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->multiplayer = new SlideMultiplayer(activeScene);
}

SlideConnection::SlideConnection(SceneMenu* scene): Slide(scene) {
    logo = new GameLogo();
    scene->addItem(logo);

    label = scene->placeGameLabel(label, "Подключение", logo, {0, -40});

    label_host = scene->placeGameLabel(label_host, "Хост:", logo);
    edit_host = scene->placeGameTextEdit(edit_host, default_host, logo);
    scene->alignToWidth(label_host, edit_host, 10, {0, 75}, {0, 80});

    label_port = scene->placeGameLabel(label_port, "Порт:", label_host);
    edit_port = scene->placeGameTextEdit(edit_port, default_port, edit_host);
    label_port->gameObjectSetLeft(label_host->gameObjectLeft(), {0, 15});
    edit_port->gameObjectSetLeft(edit_host->gameObjectLeft(), {0, 20});

    label_name = scene->placeGameLabel(label_name, "Имя:", label_port);
    edit_name = scene->placeGameTextEdit(edit_name, "Player", edit_port);
    label_name->gameObjectSetLeft(label_host->gameObjectLeft(), {0, 15});
    edit_name->gameObjectSetLeft(edit_host->gameObjectLeft(), {0, 20});

    button_connect = scene->placeGameButton(button_connect, "Войти", edit_name);
    connect(button_connect, &GameButton::clicked, this, &SlideConnection::connectToServer);

    button_cancel = scene->placeGameButton(button_cancel, "Отмена", edit_name);
    connect(button_cancel, &GameButton::clicked, this, &SlideConnection::cancel);
    scene->alignToWidth(button_connect, button_cancel, 15, {0, 40}, {0, 40}, edit_name);
}

/***************************** SLIDE MULTI *********************************/

void SlideMultiplayer::createServer() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->creation = new SlideCreate(activeScene);
}

void SlideMultiplayer::connectToServer() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->connection = new SlideConnection(activeScene);
}

void SlideMultiplayer::cancel() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->MainMenu = new SlideMainMenu(activeScene);
}

SlideMultiplayer::SlideMultiplayer(SceneMenu* scene): Slide(scene) {
    logo = new GameLogo();
    scene->addItem(logo);

    label = scene->placeGameLabel(label, "Мультиплеер", logo, {0, -40});

    button_create = scene->placeGameButton(button_create, "Создать", logo, {0, 0}, {0, 80});
    connect(button_create, &GameButton::clicked, this, &SlideMultiplayer::createServer);

    button_connect = scene->placeGameButton(button_connect, "Подключиться", button_create);
    connect(button_connect, &GameButton::clicked, this, &SlideMultiplayer::connectToServer);

    button_cancel = scene->placeGameButton(button_cancel, "Отмена", button_connect);
    connect(button_cancel, &GameButton::clicked, this, &SlideMultiplayer::cancel);
}

/***************************** SLIDE OFFLINE *******************************/

void SlideOffline::begin() {
    emit activeScene->signalCreate(local_host, local_port.toInt(),
                                   "Player", 1, players, diff, false);
    //QMessageBox::information(0, "", "Оффлайн режим не доступен в бета-версии программы. Используйте мультиплеер");
}

void SlideOffline::cancel() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->MainMenu = new SlideMainMenu(activeScene);
}

SlideOffline::SlideOffline(SceneMenu* scene): Slide(scene) {
    logo = new GameLogo();
    scene->addItem(logo);

    label_players = scene->placeGameLabel(label_players, "Количество соперников: X", logo);
    slider_players = scene->placeGameSlider(slider_players, 1, bot_limits, label_players);
    slider_players->setCounter(label_players, "Количество соперников: ", &players, true, scene);

    label_diff = scene->placeGameLabel(label_diff, "Сложность: Легкий", slider_players);
    slider_diff = scene->placeGameSlider(slider_diff, 0, difficulties.size()-1, label_diff);
    slider_diff->setCounter(label_diff, "Сложность: ", &diff, true, scene, difficulties);

    button_start = scene->placeGameButton(button_start, "Начать", slider_diff);
    connect(button_start, &GameButton::clicked, this, &SlideOffline::begin);

    button_cancel = scene->placeGameButton(button_cancel, "Отмена", slider_diff);
    connect(button_cancel, &GameButton::clicked, this, &SlideOffline::cancel);
    scene->alignToWidth(button_start, button_cancel, 15, {0, 40}, {0, 40}, slider_diff);
}

/***************************** SLIDE MAIN MENU *****************************/

void SlideMainMenu::exit() {
    QApplication::quit();
}

void SlideMainMenu::startGame() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->offline = new SlideOffline(activeScene);
}

void SlideMainMenu::startMulti() {
    SceneMenu* menuptr = activeScene; //safe free
    menuptr->slides->multiplayer = new SlideMultiplayer(activeScene);
}

SlideMainMenu::SlideMainMenu(SceneMenu* scene): Slide(scene) {
    logo = new GameLogo();
    scene->addItem(logo);

    button_start = scene->placeGameButton(button_start, "Начать", logo);
    connect(button_start, &GameButton::clicked, this, &SlideMainMenu::startGame);

    button_online = scene->placeGameButton(button_online, "Мультиплеер", button_start);
    connect(button_online, &GameButton::clicked, this, &SlideMainMenu::startMulti);

    button_exit = scene->placeGameButton(button_exit, "Выход", button_online);
    connect(button_exit, &GameButton::clicked, this, &SlideMainMenu::exit);
}

/***************************** SCENE MENU *********************************/

int SceneMenu::getCenterX(int width) {
    return this->width() / 2 - width / 2;
}

int SceneMenu::getCenterY(int height) {
    return this->height() / 2 - height / 2;
}

QPoint SceneMenu::getCenter(int width, int height) {
    return QPoint(getCenterX(width), getCenterY(height));
}

QPoint SceneMenu::verticalLayout(QRectF back_rect, int width) {
    return QPoint(getCenterX(width),
                  back_rect.y()+back_rect.height()*getItemsOffset());
}

QPoint SceneMenu::verticalLayout(GameLogo* logo, int width) {
    return QPoint(getCenterX(width), logo->topOffset());
}

QPair<QPoint, QPoint> SceneMenu::horizontalLayout(int top, int width1, int width2) {
    int center = getCenterX(width1+width2+15);
    return {QPoint(center, top),
            QPoint(center+width2+15, top)};
}

void SceneMenu::placeGameObject(GameObject* a, GameObject* b, QSize size, QPoint offset) {
    if (size.width() > 0) {
        a->gameObjectResize(size);
    }
    QPoint pos = {0, 0};
    if (b != nullptr) {
        pos = verticalLayout(b->gameObjectRect(), a->gameObjectRect().width());
    }
    pos.setX(pos.x()+offset.x());
    pos.setY(pos.y()+offset.y());
    a->gameObjectMove(pos);
}

GameButton* SceneMenu::placeGameButton(GameButton* btn, QString text, GameObject* gameobj,
                            QSize size, QPoint offset)
{
    btn = new GameButton();
    btn->setText(text);
    placeGameObject(btn, gameobj, size, offset);
    addItem(btn);
    return btn;
}

GameLabel* SceneMenu::placeGameLabel(GameLabel* label, QString text, GameObject* gameobj, QPoint offset) {
    label = new GameLabel();
    label->setPlainText(text);
    placeGameObject(label, gameobj, QSize(0, 0), offset);
    addItem(label);
    return label;
}

GameSlider* SceneMenu::placeGameSlider(GameSlider* slider, int min, int max, GameObject* gameobj,
                             QSize size, QPoint offset)
{
    slider = new GameSlider();
    slider->setMinimum(min);
    slider->setMaximum(max);
    placeGameObject(slider, gameobj, size, offset);
    addWidget(slider);
    return slider;
}

GameTextEdit* SceneMenu::placeGameTextEdit(GameTextEdit* edit, QString text, GameObject* gameobj,
                                QSize size, QPoint offset)
{
    edit = new GameTextEdit();
    edit->setText(text);
    placeGameObject(edit, gameobj, size, offset);
    addWidget(edit);
    return edit;
}

GameList* SceneMenu::placeGameList(GameList* list, QStringList items, GameObject* gameobj,
                        QSize size, QPoint offset)
{
    list = new GameList();
    list->addItems(items);
    placeGameObject(list, gameobj, size, offset);
    addWidget(list);
    return list;
}

GameTable* SceneMenu::placeGameTable(GameTable* table, GameObject* gameobj, QSize size, QPoint offset)
{
    table = new GameTable();
    placeGameObject(table, gameobj, size, offset);
    addWidget(table);
    return table;
}

void SceneMenu::alignToWidth(GameObject* a, GameObject* b, int distance,
    QPoint offset1, QPoint offset2, GameObject* adjustobj)
{
    if (adjustobj) {
        int sizex = adjustobj->gameObjectRect().width()/2;
        double scale = sizex / a->gameObjectRect().width();
        int sizey = (a->gameObjectRect().height() * scale) + /*magic*/20;
        a->gameObjectResize(QSize(sizex, sizey));
        b->gameObjectResize(QSize(sizex, sizey));
    }
    int width = a->gameObjectRect().width() + b->gameObjectRect().width();
    int center = getCenterX(width);
    int x1 = center - distance;
    int x2 = center + a->gameObjectRect().width() + distance;
    QPoint p1 = QPoint(x1, a->gameObjectRect().y())+offset1;
    QPoint p2 = QPoint(x2, b->gameObjectRect().y())+offset2;
    a->gameObjectMove(p1);
    b->gameObjectMove(p2);
}

SceneMenu::SceneMenu()
    : slides(new Slides())
{
    slides->MainMenu = new SlideMainMenu(this);
}
