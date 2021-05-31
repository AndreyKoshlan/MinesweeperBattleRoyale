#include "gametextedit.h"
#include <QFile>

GameTextEdit::GameTextEdit()
{
    this->setFixedWidth(400);
    this->setFixedHeight(64);
    this->setAttribute(Qt::WA_TranslucentBackground); //round borders

    //TextEdit stylesheet
    QFile styleFile(":/styles/line" );
    styleFile.open( QFile::ReadOnly );
    QString style( styleFile.readAll() );
    this->setStyleSheet(style);
}
