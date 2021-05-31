#ifndef GAMEBUTTON_H
#define GAMEBUTTON_H

#include <QGraphicsPixmapItem>
#include "gameobject.h"

class GameButton : public QObject, public QGraphicsPixmapItem, public GameObject
{
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColorCurrent READ backgroundColorCurrent WRITE setBackgroundColorCurrent)

public:
    GameButton();
    void setWidth(const size_t width) { m_width = width; update(); }
    size_t width() const { return m_width; }

    void setHeight(const size_t height) { m_height = height; update(); }
    size_t height() const { return m_height; }

    void setBackgroundColor(const QColor clr) { m_backgroundColor = clr;
                                                m_backgroundColorCurrent = m_backgroundColor;
                                                update(); }
    QColor backgroundColor() const { return m_backgroundColor; }

    void setBackgroundColorCurrent(const QColor clr) { m_backgroundColorCurrent = clr; update(); }
    QColor backgroundColorCurrent() const { return m_backgroundColorCurrent; }

    void setBackgroundColorHover(const QColor clr) { m_backgroundColorHover = clr; update(); }
    QColor backgroundColorHover() const { return m_backgroundColorHover; }

    void setBackgroundColorPresed(const QColor clr) { m_backgroundColorPressed = clr; update(); }
    QColor backgroundColorPressed() const { return m_backgroundColorPressed; }

    void setBorderColor(const QColor clr) { m_borderColor = clr; update(); }
    QColor borderColor() const { return m_borderColor; }

    void setSpecularColor(const QColor clr) { m_specularColor = clr; update(); }
    QColor specularColor() const { return m_specularColor; }

    void setFontColor(const QColor clr) { m_fontColor = clr; update(); }
    QColor fontColor() const { return m_fontColor; }

    void setFont(QFont *nfont) { m_font = nfont; update(); }
    QFont* font() const { return m_font; }

    void setText(QString text) { m_text = text; update(); }
    QString text() const { return m_text; }

    /* placement on a graphics scene  */
    virtual QRectF gameObjectRect() const { return sceneBoundingRect();}
    virtual void gameObjectMove(QPoint p) { setX(p.x()); setY(p.y()); };
    virtual void gameObjectResize(QSize s) {setWidth(s.width()); setHeight(s.height());};

    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*);
signals:
    void clicked();
private:
    /* methodes */
    void update();
    QPixmap updateScale(QPixmap &px, size_t width, size_t height);
    void updateColor(QPixmap &px, QColor clr);
    void updateText(QPixmap &px, QString text, QFont* font, QColor fontColor);

    /* parameters */
    size_t m_height = 140;
    size_t m_width = 340;
    QColor m_backgroundColor = {226, 213, 249};
    QColor m_backgroundColorCurrent = m_backgroundColor;
    QColor m_backgroundColorHover = {240, 233, 252};
    QColor m_backgroundColorPressed = {163, 121, 236};
    QColor m_borderColor = {57, 19, 52};
    QColor m_specularColor = {255, 255, 255};
    QColor m_fontColor = {57, 19, 52};
    QFont* m_font;
    QString m_text;
    bool isPressed = false;
    bool isHover = false;
};

#endif // GAMEBUTTON_H
