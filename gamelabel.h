#ifndef GAMELABEL_H
#define GAMELABEL_H

#include <QGraphicsTextItem>
#include "gameobject.h"

class GameLabel : public QGraphicsTextItem, public GameObject
{
public:
    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return sceneBoundingRect();}
    virtual void gameObjectMove(QPoint p) { setX(p.x()); setY(p.y()); };
    virtual void gameObjectResize(QSize) {};

    GameLabel();
};

#endif // GAMELABEL_H
