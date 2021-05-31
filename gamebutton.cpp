#include <QPainter>
#include <QDebug>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QTimer>
#include <QPropertyAnimation>
#include <QtMath>

#include "gamebutton.h"

QPixmap GameButton::updateScale(QPixmap &px, size_t width, size_t height) {
    return px.scaled(QSize(width, height), Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
}

void GameButton::updateColor(QPixmap &px, QColor clr) {
    QPainter painter(&px);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(px.rect(), clr);
    painter.end();
}

void GameButton::updateText(QPixmap &px, QString text, QFont* font, QColor fontColor) {
    QPainter painter(&px);
    if (px.width() > 256) {
        m_font->setPixelSize(px.width()/8);
    } else {
        m_font->setPixelSize(px.width()/5);
    }
    painter.setFont(*font);
    painter.setPen(fontColor);
    painter.drawText(QRect(0, 0, px.width(), px.height()), Qt::AlignCenter, text);
}

void GameButton::update() {
    /* parts of buttons */
    struct btnComponents {
        QString fileName;
        QColor color;
    } buttonComponents;

    if (isPressed)
        m_backgroundColorCurrent = m_backgroundColorPressed;
    QVector<btnComponents> components = { {":/textures/button", m_backgroundColorCurrent},
                                          {":/textures/button_border", m_borderColor},
                                          {":/textures/button_glint", m_specularColor},
                                        };
    if (isPressed)
        m_backgroundColorCurrent = m_backgroundColor;

    /* create transperent button pixmap */
    QPixmap pixmap(m_width, m_height);
    pixmap.fill(QColorConstants::Transparent);

    for (auto buttonPart: components) {
        /* open picture */
        QPixmap tempPixmap(buttonPart.fileName);

        /* scale */
        QPixmap tempPixmapScaled = updateScale(tempPixmap, m_width, m_height);

        /* change color */
        updateColor(tempPixmapScaled, buttonPart.color);

        /* combine pixmaps */
        QPainter painter2(&pixmap);
        painter2.drawPixmap(0, 0, tempPixmapScaled);
    }
    /* draw text */
    updateText(pixmap, m_text, m_font, m_fontColor);

    /* draw */
    setPixmap(pixmap);
}

void GameButton::mousePressEvent(QGraphicsSceneMouseEvent*) {
    isPressed = true;
    update();
}

void GameButton::mouseReleaseEvent(QGraphicsSceneMouseEvent*) {
    isPressed = false;
    update();
    if (isUnderMouse())
        emit clicked();
}

void GameButton::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "backgroundColorCurrent");
    animation->setDuration(100);
    animation->setStartValue(m_backgroundColor);
    animation->setEndValue(m_backgroundColorHover);
    animation->start();
    isHover = true;
    update();
}

void GameButton::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
    QPropertyAnimation *animation = new QPropertyAnimation(this, "backgroundColorCurrent");
    animation->setDuration(100);
    animation->setStartValue(m_backgroundColorHover);
    animation->setEndValue(m_backgroundColor);
    animation->start();
    isHover = false;
    update();
}

GameButton::GameButton() {
    m_font = new QFont("Bainsley");
    setAcceptHoverEvents(true);
    update();
}
