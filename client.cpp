#include "client.h"
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QWidget>

PlayerList Client::extractPlayerList(QByteArray buffer) {
    PlayerList list;
    int p1 = 1;
    int pcount = buffer.mid(p1, 4).toHex().toInt(nullptr, 16);
    p1 += 4;
    for (int i = 0; i < pcount; i++) {
        int id = buffer.mid(p1, 4).toHex().toInt(nullptr, 16);
        p1 += 4;
        bool failed = buffer.mid(p1, 1).toHex().toInt(nullptr, 16);
        p1 += 1;
        int opened = buffer.mid(p1, 4).toHex().toInt(nullptr, 16);
        p1 += 4;
        int mined = buffer.mid(p1, 4).toHex().toInt(nullptr, 16);
        p1 += 4;
        int psize = buffer.mid(p1, 4).toHex().toInt(nullptr, 16);
        p1 += 4;
        QString name = QString::fromUtf8(buffer.mid(p1, psize));
        list.append({id, failed, opened, mined, name});
        p1 += psize;
    }
    return list;
}

void Client::processData(QByteArray buffer) {
    if (!listened)
        return;
    if (buffer.at(0) == 0x00) {
        PlayerList pl = extractPlayerList(buffer);
        qDebug() << "Message 0x00: player list (" << pl.count() << ")";
        emit this->msgUserList(pl);
    }
    if (buffer.at(0) == 0x01) {
        qDebug() << "Message 0x01: map (init)";
        emit this->msgMap(buffer);
    }
    if (buffer.at(0) == 0x02) {
        qDebug() << "Message 0x02: suspend";
        int time = buffer.mid(1, 4).toHex().toInt(nullptr, 16);
        emit this->msgSuspend(time);
    }
    if (buffer.at(0) == 0x03) {
        qDebug() << "Message 0x03: failed";
        emit this->msgFail();
    }
    if (buffer.at(0) == 0x04) {
        PlayerList pl = extractPlayerList(buffer);
        qDebug() << "Message 0x04: game over (" << pl.count() << ")";
        emit this->msgOver(pl);
    }
    if (buffer.at(0) == 0x05) {
        qDebug() << "Message 0x05: mine count";
        int mines = buffer.mid(1, 4).toHex().toInt(nullptr, 16);
        emit this->msgGetMines(mines);
    }
}

void Client::onReceiveData() {
    QByteArray buffer;
    buffer = socket->readAll();
    int p = 0;
    while (p < buffer.size()) {
        int pkgSize = buffer.mid(p, 4).toHex().toInt(nullptr, 16);
        processData(buffer.mid(p+=4, pkgSize));
        p += pkgSize;
    }
}

void Client::onDisconnect() {
    qDebug() << "Message: disconnected";
    listen(false);
    emit msgDisconnect();
}

void Client::listen(bool enable) {
    listened = enable;
    if (enable) {
        QObject::connect(socket, &QTcpSocket::readyRead, this, &Client::onReceiveData);
        QObject::connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnect);
    } else {
        QObject::disconnect(socket, &QTcpSocket::readyRead, this, &Client::onReceiveData);
        QObject::disconnect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnect);
    }
}

void Client::sendMessage(QByteArray data) {
    int data_size = data.size();
    data.push_front(QByteArray((char*)&data_size, 4));
    socket->write(data);
}

void Client::sendMessageStart(int bots, int diff) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    char msg_type[] = {char(0x01)};
    stream.writeRawData(msg_type, 1);
    stream << bots << diff;
    sendMessage(data);
}

void Client::sendMessageClick(int clickType, QPoint cell) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    char msg_type[] = {char(0x02)};
    stream.writeRawData(msg_type, 1);
    stream << clickType << cell.x() << cell.y();
    sendMessage(data);
}

bool Client::handshake(QString player) {
    if (socket->state() == QTcpSocket::ConnectedState) {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        /* magic bytes */
        char magic[] = { char(0xC1), char(0xD0), char(0xDF), char(0xD5), char(0xE0) };
        stream.writeRawData(magic, 5);
        /* player name */
        stream << player.toUtf8();
        /* token */
        if (admin) {
            stream << secretKey.toUtf8();
        } else {
            stream << QString("").toUtf8();
        }
        sendMessage(data);
        /* read and finish handshake */
        char* readData = new char[4096];
        if (!socket->waitForReadyRead()) return false;
        socket->read(readData, 4096);
        QString msg = QString::fromUtf8(readData+4);
        return msg == "OK";
    }
    return false;
}

bool Client::connect(QString host, int port) {
    listen(false);
    socket->disconnect();
    socket->connectToHost(host, port);
    if (socket->waitForConnected()) {
        return (socket->state() == QTcpSocket::ConnectedState);
    }
    return false;
}

void Client::disconnect() {
    listen(false);
    socket->disconnect();
    socket->disconnectFromHost();
    emit msgDisconnect();
}

void Client::generateKey() {
    QRandomGenerator random;
    random.seed(QDateTime::currentMSecsSinceEpoch());
    QString str = "";
    for (int i = 0; i < 128; i++) {
        char rnd = random.bounded(255);
        str += rnd;
    }
    secretKey = QCryptographicHash::hash(str.toLatin1(), QCryptographicHash::Sha256).toHex();
}

void Client::createServer(QString host, int port, int players) {
    generateKey();
    QString dirPath = QDir::toNativeSeparators(QApplication::applicationDirPath());
    QStringList argumentString = QStringList() << serverPath <<
            "-s" << host <<
            "-p" << QString::number(port) <<
            "-c" << QString::number(players) <<
            "-k" << secretKey <<
            "--autoshutdown";
#ifdef ENABLE_PYTHON_DEBUG
    QThread *thread = QThread::create([argumentString, dirPath]{
        system(("py "+dirPath+"\\"+argumentString.join(" ")+"&").toStdString().c_str());
    });
    thread->start();
#else
    QProcess python;
    python.startDetached("py", argumentString, dirPath);
#endif
}

void Client::closeServer() {

}

Client::Client()
    : socket(new QTcpSocket()) {}
