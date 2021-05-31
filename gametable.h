#ifndef GAMETABLE_H
#define GAMETABLE_H

#include <QTableWidget>
#include "gameobject.h"

class GameTable : public QTableWidget, public GameObject
{
public:
    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return geometry();}
    virtual void gameObjectMove(QPoint p) { move(p); };
    virtual void gameObjectResize(QSize s) { resize(s); };

    GameTable();
};

#endif // GAMETABLE_H
