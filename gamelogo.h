#ifndef GAMELOGO_H
#define GAMELOGO_H

#include <QGraphicsPixmapItem>
#include "gameobject.h"

class GameLogo: public QGraphicsPixmapItem, public GameObject {
public:
    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return sceneBoundingRect();}
    virtual void gameObjectMove(QPoint p) { setX(p.x()); setY(p.y()); };
    virtual void gameObjectResize(QSize s) { pixmap().scaled(s); };

    /* offset of menu items relative to the
     *  position and size of the logo */
    int topOffset() const {return y()+pixmap().height()*1.12;};

    /* constructor */
    GameLogo();
};

#endif // GAMELOGO_H
