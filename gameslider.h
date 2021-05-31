#ifndef GAMESLIDER_H
#define GAMESLIDER_H

#include <QSlider>
#include <QLabel>
#include "gameobject.h"
#include "gamelabel.h"
#include <QGraphicsScene>

class GameSlider : public QSlider, public GameObject
{
public:
    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return geometry();}
    virtual void gameObjectMove(QPoint p) { move(p); };
    virtual void gameObjectResize(QSize s) { resize(s); };

    void setCounter(GameLabel* label, QString text = "", int *valueToChange = nullptr,
                    bool center = false, QGraphicsScene* scene = nullptr, QVector<QString> list = {});

    GameSlider();
};

#endif // GAMESLIDER_H
