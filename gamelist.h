#ifndef GAMELIST_H
#define GAMELIST_H

#include <QListWidget>
#include <gameobject.h>

class GameList : public QListWidget, public GameObject
{
public:
    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return geometry();}
    virtual void gameObjectMove(QPoint p) { move(p); };
    virtual void gameObjectResize(QSize s) { resize(s); };

    GameList();
};

#endif // GAMELIST_H
