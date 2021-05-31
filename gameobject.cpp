#include "gameobject.h"

double GameObject::gameObjectLeft() const {
    return gameObjectRect().x();
}

double GameObject::gameObjectTop() const {
    return gameObjectRect().y();
}

void GameObject::gameObjectSetLeft(int left, QPoint offset) {
    QRectF r = gameObjectRect();
    gameObjectMove(QPoint(left, r.top())+offset);
}
void GameObject::gameObjectSetTop(int top, QPoint offset) {
    QRectF r = gameObjectRect();
    gameObjectMove(QPoint(r.left(), top)+offset);
}

GameObject::GameObject() {}
