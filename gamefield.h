#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QStringList>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QObject>
#include "gameobject.h"

struct GameCell {
    short id;
    unsigned char mines;
    unsigned char status;
};

struct CellStatus {
    bool mined = false;
    bool flagged = false;
    bool own = false;
    CellStatus(int status) {
        if ((status & 1) == 1)
            own = true;
        if ((status & 10) == 10)
            flagged = true;
        if ((status & 100) ==  100)
            mined = true;
    }
};

typedef QVector<QVector<GameCell>> GameFieldData;

class GameField : public QObject, public QGraphicsPixmapItem, public GameObject
{
    Q_OBJECT
public:
    void LoadFromRawData(char* ndata, qint64 maxlength);

    bool get(int x, int y, GameCell& rdata) const;

    bool isCell(QPointF pos);
    QPoint cellAt(QPointF pos);
    QPointF posAt(QPoint cell);

    void setWidth(const size_t width) { m_width = width; update(); }
    size_t width() const { return m_width; }

    void setHeight(const size_t height) { m_height = height; update(); }
    size_t height() const { return m_height; }

    void suspend(int time);
    void fail();
    void setMineCount(int mines);
    bool suspended = false;
    bool failed = false;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return sceneBoundingRect();}
    virtual void gameObjectMove(QPoint p) { setX(p.x()); setY(p.y()); };
    virtual void gameObjectResize(QSize s) {setWidth(s.width()); setHeight(s.height());};

    GameField(QGraphicsScene* scene);
private:
    /* properties */
    const size_t cellwidth = 52;
    const size_t cellheight = 52;
    const size_t cellmargin = 2;
    const QColor grayColor = QColor(179, 172, 185);
    size_t m_height = 1000;
    size_t m_width = 2000;
    int mines = 0;
    size_t reservedCells = 245;
    bool isPressed = false;
    bool isMoving = false;
    bool isLeftButtonPressed = false;
    bool isRightButtonPressed = false;
    QPointF offset = {0, 0};
    QPointF mousePos = {0, 0};
    GameFieldData data;
    QVector<QColor> colorMap;
    QVector<QColor> fontColorMap;
    QGraphicsScene* scene;

    /* methodes */
    void drawStatusBar(QPainter& p, QPixmap& px);
    void drawFailText(QPainter& p, QPixmap& px);
    void drawSuspendText(QPainter& p, QPixmap& px);
    void drawCell(QPainter& p, QColor color);
    void drawText(QPainter& p, QPixmap& px, QColor color, QString text);
    void drawTexture(QPainter& p, QPixmap& px, bool mine, bool flag, bool side = false);
    QPixmap getCellPixmap(int id, int mines, CellStatus status);
    void createColorMap();
    void createFontColorMap();
    void paintCells();
    void update();
signals:
    void leftClick(QPoint cell);
    void rightClick(QPoint cell);
    void signalUpdate();
};

#endif // GAMEFIELD_H
