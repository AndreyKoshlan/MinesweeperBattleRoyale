#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QRectF>

class GameObject
{
public:
    virtual QRectF gameObjectRect() const { return QRectF(0, 0, 0, 0); }
    virtual void gameObjectMove(QPoint){};
    virtual void gameObjectResize(QSize){};

    double gameObjectLeft() const;
    double gameObjectTop() const;
    void gameObjectSetLeft(int left, QPoint offset = {0, 0});
    void gameObjectSetTop(int top, QPoint offset = {0, 0});



    GameObject();
};

#endif // GAMEOBJECT_H
