#include "gameslider.h"
#include <QFile>

void GameSlider::setCounter(GameLabel* label, QString text, int *valueToChange,
    bool center, QGraphicsScene* scene, QVector<QString> list)
{
    connect(this, &GameSlider::valueChanged, this,
           [this, label, text, valueToChange, center, scene, list](int value) {
                if (valueToChange != nullptr)
                    *valueToChange = value;
                if (list.size() > 0)
                    label->setPlainText(text+list[value]);
                else
                    label->setPlainText(text+QString::number(value));
                if (center) {
                    label->setX(scene->width() / 2 - label->boundingRect().width() / 2);
                    this->move(scene->width() / 2 - this->geometry().width() / 2, this->y());
                }
    });
    emit(valueChanged(this->value()));
}

GameSlider::GameSlider() {
    setOrientation(Qt::Horizontal);
    //this->setFixedWidth(384);
    this->resize(384, 68);

    //Slider stylesheet
    QFile styleFile(":/styles/slider" );
    styleFile.open( QFile::ReadOnly );
    QString style( styleFile.readAll() );
    this->setStyleSheet(style);
}
