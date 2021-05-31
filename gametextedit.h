#ifndef GAMETEXTEDIT_H
#define GAMETEXTEDIT_H

#include <QLineEdit>
#include "gameobject.h"

class GameTextEdit : public QLineEdit, public GameObject
{
public:
    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return geometry();}
    virtual void gameObjectMove(QPoint p) { move(p); };
    virtual void gameObjectResize(QSize s) { resize(s); };

    GameTextEdit();
};

#endif // GAMETEXTEDIT_H
