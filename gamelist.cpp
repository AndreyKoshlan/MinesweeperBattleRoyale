#include "gamelist.h"
#include <QFile>
#include <QScrollBar>

GameList::GameList()
{
    this->resize(600, 300);
    //this->setAttribute(Qt::WA_TranslucentBackground);
    setAlternatingRowColors(true);

    //List stylesheet
    QFile styleFile(":/styles/list" );
    styleFile.open( QFile::ReadOnly );
    QString style( styleFile.readAll() );
    this->setStyleSheet(style);

    //ScrollBar stylesheet
    this->verticalScrollBar()->setStyleSheet(style);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    //this->horizontalScrollBar()->setStyleSheet(style);
}
