#ifndef CLIENT_H
#define CLIENT_H

#define ENABLE_PYTHON_DEBUG
#ifdef ENABLE_PYTHON_DEBUG
#include <QThread>
#endif

#include <QTcpSocket>
#include <QRandomGenerator>
#include <QDateTime>
#include <QCryptographicHash>
#include <QVector>
#include <QObject>

struct Player {
    int id;
    bool failed;
    int opened;
    int mined;
    QString player;
};

typedef QVector<Player> PlayerList;

class Client: public QObject
{
    Q_OBJECT
public:
    /* read methodes */
    PlayerList extractPlayerList(QByteArray buffer);

    /* messages */
    bool handshake(QString player);
    void sendMessage(QByteArray data);
    void sendMessageStart(int bots, int diff);
    void sendMessageClick(int clickType, QPoint cell);

    /* server */
    void listen(bool enable);
    bool connect(QString host, int port);
    void disconnect();
    void createServer(QString host, int port, int players);
    void closeServer();

    /* constructor */
    Client();

    bool admin = false;
private:
    void generateKey();

    QTcpSocket* socket;
    QString secretKey;
    bool listened = false;
    const QString serverPath = "server.py";
signals:
    void msgUserList(PlayerList pl);
    void msgDisconnect();
    void msgMap(QByteArray buff); //start game
    void msgSuspend(int time);
    void msgFail();
    void msgOver(PlayerList pl);
    void msgGetMines(int mines);
private slots:
    void onReceiveData();
    void processData(QByteArray buffer);
    void onDisconnect();
};

#endif // CLIENT_H
