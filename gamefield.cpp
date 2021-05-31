#include "gamefield.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QDateTime>
#include <QThread>
#include <QGraphicsView>

/********************************* drawing features *********************************/

void GameField::drawCell(QPainter& p, QColor color) {
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, cellwidth, cellheight), 5, 5);
    QPen pen(Qt::black, 1);
    p.setPen(pen);
    p.fillPath(path, color);
    p.drawPath(path);
}

void GameField::drawText(QPainter& p, QPixmap& px, QColor color, QString text) {
    p.setFont(QFont("Impact", px.width()/2, QFont::Bold));
    p.setPen(color);
    p.drawText(QRect(0, 0, px.width(), px.height()), Qt::AlignCenter, text);
}

void GameField::drawTexture(QPainter& p, QPixmap& px, bool mine, bool flag, bool side) {
    if (mine) {
        QPixmap minepx(":/textures/mine");
        int minewidth = side ? cellwidth / 3: cellwidth / 1.38;
        int mineheight = side ? cellheight / 3: cellheight / 1.38;
        QPixmap minepxScaled = minepx.scaled(QSize(minewidth, mineheight),
                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        if (side) {
            p.drawPixmap(2, 2, minepxScaled);
        } else {
            p.drawPixmap((px.rect().bottomRight() - minepxScaled.rect().bottomRight())/2, minepxScaled);
        }
    }
    if (flag) {
        QPixmap flagpx(":/textures/flag");
        QPixmap flagpxScaled = flagpx.scaled(QSize(cellwidth/1.38, cellwidth/1.38),
                                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap((px.rect().bottomRight() - flagpxScaled.rect().bottomRight())/2, flagpxScaled);
    }
}

void GameField::drawFailText(QPainter& p, QPixmap& px) {
    p.fillRect(px.rect(), QBrush(QColor(0, 0, 0, 0x7F)));
    p.setFont(QFont("Bainsley", px.width()/42, QFont::Bold));
    p.setPen(grayColor);
    if (scene->views().size() > 0) {
        QGraphicsView* view = scene->views()[0];
        QRectF viewRect = view->mapToScene(view->viewport()->geometry()).boundingRect();
        p.drawText(QRect(0, 0, viewRect.width(), viewRect.height()), Qt::AlignCenter, "Все ваши ячейки были раскрыты");
    }
}

void GameField::drawSuspendText(QPainter& p, QPixmap& px) {
    p.fillRect(px.rect(), QBrush(QColor(0, 0, 0, 0x7F)));
    p.setFont(QFont("Bainsley", px.width()/32, QFont::Bold));
    p.setPen(grayColor);
    if (scene->views().size() > 0) {
        QGraphicsView* view = scene->views()[0];
        QRectF viewRect = view->mapToScene(view->viewport()->geometry()).boundingRect();
        p.drawText(QRect(0, 0, viewRect.width(), viewRect.height()), Qt::AlignCenter, "Вы открыли ячейку с миной");
        p.drawText(QRect(0, 90, viewRect.width(), viewRect.height()), Qt::AlignCenter, "Ожидайте");
    }
}

void GameField::drawStatusBar(QPainter& p, QPixmap& px) {
    QPixmap pxline(px.width(), 32);
    QPainter pline(&pxline);
    pline.fillRect(pxline.rect(), QBrush(QColor("#74536B")));
    QPixmap minepx(":/textures/mine");
    QPixmap minepxScaled = minepx.scaled(QSize(24, 24),
                                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pline.drawPixmap(8, 4, minepxScaled);
    pline.setFont(QFont("Bainsley", 18, QFont::Bold));
    pline.setPen(grayColor);
    pline.drawText(38, 19, QString::number(this->mines));
    p.drawPixmap(QRect(0, 0, pxline.width(),pxline.height()), pxline);
}

QPixmap GameField::getCellPixmap(int id, int mines, CellStatus status) {
    QPixmap px(cellwidth, cellheight);

    /* fill background */
    px.fill(QColorConstants::Transparent);

    /* painting cell */
    QPainter p(&px);
    drawCell(p, status.own ? grayColor : colorMap[id]);

    /* text out */
    if (mines != 0) {
        drawText(p, px, fontColorMap[mines-1], QString::number(mines));
    }

    /* texture */
    drawTexture(p, px, status.mined, status.flagged, mines != 0);

    return px;
}

void GameField::update() {
    QPixmap pixmap(m_width, m_height);
    pixmap.fill(QColor(0, 0, 0, 0x7F));
    QPainter painter(&pixmap);
    QPoint paintFrom = cellAt(offset);
    QPoint paintTo = cellAt(QPointF(offset.x()-m_width-cellwidth, offset.y()-m_height-cellheight));
    for (int y = paintFrom.y(); y <= paintTo.y(); y++) {
        for (int x = paintFrom.x(); x <= paintTo.x(); x++) {
            GameCell cell;
            if (!get(x, y, cell))
                continue;
            QPixmap cellpx;
            cellpx = getCellPixmap(cell.id, cell.mines, CellStatus(cell.status));
            painter.drawPixmap(posAt(QPoint(x, y)), cellpx);
        }
    }
    if (failed) {
        drawFailText(painter, pixmap);
    } else {
        if (suspended)
            drawSuspendText(painter, pixmap);
    }
    drawStatusBar(painter, pixmap);
    setPixmap(pixmap);
}

/********************************* other *********************************/

void GameField::LoadFromRawData(char* ndata, qint64 maxlength) {
    GameFieldData list;
    QByteArray array(ndata, maxlength);
    QDataStream stream(&array, QIODevice::ReadOnly);
    int x = 0;
    while (x < maxlength) {
        int cells_count = 0;
        stream >> cells_count;
        if (cells_count == 0)
            break;
        list.resize(list.size()+1);
        for (int i = 0; i < cells_count; i++) {
            GameCell cell = {};
            stream >> cell.id >> cell.mines >> cell.status;
            list[list.size()-1].append(cell);
            x += 4; //cell size
        }
        x += 4; //row length field
    }
    data = list;
    update();
}

bool GameField::get(int x, int y, GameCell& rdata) const {
    if (x >= 0 && y >= 0) {
        if (y < data.size() && x < data[y].size()) {
            rdata = data[y][x];
            return true;
        }
    }
    return false;
}

QPointF GameField::posAt(QPoint cell) {
    return QPointF(cell.x()*(cellwidth+cellmargin)+cellmargin+offset.x(),
                   cell.y()*(cellheight+cellmargin)+cellmargin+offset.y());
}

bool GameField::isCell(QPointF pos) {
    int cellx = (-pos.x() - cellmargin) / (cellwidth + cellmargin);
    int celly = (-pos.y() - cellmargin) / (cellwidth + cellmargin);
    if (celly >= 0 && cellx >= 0) {
        if (celly < data.size()-1 && cellx < data[celly].size()-1) {
            return true;
        }
    }
    return false;
}

QPoint GameField::cellAt(QPointF pos) {
    int cellx = (-pos.x() - cellmargin) / (cellwidth + cellmargin);
    int celly = (-pos.y() - cellmargin) / (cellwidth + cellmargin);
    cellx = fmax(cellx, 0);
    celly = fmax(celly, 0);
    celly = fmin(celly, data.size()-1);
    cellx = fmin(cellx, data[0].size()-1);
    return QPoint(cellx, celly);
}

void GameField::suspend(int time) {
    this->suspended = true;
    QThread *thread = QThread::create([this, time] {
        for (int i = 0; i < time; i++) {
            QThread::sleep(1);
            emit this->signalUpdate();
        }
        this->suspended = false;
    });
    thread->start();
    this->update();
}

void GameField::fail() {
    this->failed = true;
    this->update();
}

void GameField::setMineCount(int mines) {
    this->mines = mines;
    this->update();
}

/********************************* mouse events *********************************/

void GameField::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    mousePos = event->pos();
    if (event->button() & Qt::LeftButton)
        isLeftButtonPressed = true;
    if (event->button() & Qt::RightButton)
        isRightButtonPressed = true;
    isPressed = true;
}

void GameField::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    mousePos = event->pos();
    if (isMoving == false) {
        QPointF mousePosDisplaced;
        mousePosDisplaced.setX(mousePos.x()-cellwidth);
        mousePosDisplaced.setY(mousePos.y()-cellheight);
        mousePosDisplaced = -mousePosDisplaced+offset;
        if (isCell(mousePosDisplaced)) {
            if (isLeftButtonPressed) {
                /*
                QPoint cell_pos = cellAt(-mousePos+offset);
                GameCell cell = data[cell_pos.y()][cell_pos.x()];
                if (CellStatus(cell.status).own && !CellStatus(cell.status).mined)
                    mines--;
                if (mines < 0)
                    mines = 0;
                emit leftClick(cell_pos);
                */
                emit leftClick(cellAt(-mousePos+offset));
            } else if (isRightButtonPressed) {
                emit rightClick(cellAt(-mousePos+offset));
            }
        }
    }
    isPressed = false;
    isLeftButtonPressed = false;
    isRightButtonPressed = false;
    isMoving = false;
}

void GameField::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (isPressed) {
        isMoving = true;
        offset += event->pos()-mousePos;
        mousePos = event->pos();
        update();
    }
}

/********************************* constructor *********************************/

void GameField::createColorMap() {
    colorMap = {};
    QRandomGenerator random;
    for (int i = 0; i < 256; i++) {
        random.seed(i);
        colorMap.append(QColor::fromHslF(random.bounded(1.0), 0.5, 0.5));
    }
    colorMap[255] = QColor(255, 255, 255); //free cell
}

void GameField::createFontColorMap() {
    /*
    fontColorMap = {
      QColor(72, 133, 239),
      QColor(0, 135, 68),
      QColor(219, 50, 54),
      QColor(182, 72, 242),
      QColor(244, 132, 13),
      QColor(72, 240, 241),
      QColor(15, 15, 15),
      QColor(190, 190, 190)
    };
    */
    fontColorMap = {
      QColor(0, 0, 224),
      QColor(0, 127, 0),
      QColor(224, 0, 0),
      QColor(0, 0, 127),
      QColor(127, 0, 0),
      QColor(0, 127, 127),
      QColor(0, 0, 0),
      QColor(96, 96, 96)
    };
}

GameField::GameField(QGraphicsScene* scene) {
    this->scene = scene;
    connect(this, &GameField::signalUpdate, this, &GameField::update);
    setAcceptHoverEvents(true);
    createColorMap();
    createFontColorMap();
}
