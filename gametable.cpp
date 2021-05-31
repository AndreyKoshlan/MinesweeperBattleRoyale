#include "gametable.h"
#include <QScrollBar>
#include <QHeaderView>
#include <QFile>

GameTable::GameTable()
{
    this->resize(600, 300);
    //this->setAttribute(Qt::WA_TranslucentBackground);
    setAlternatingRowColors(true);

    //Table stylesheet
    QFile styleFile(":/styles/list" );
    styleFile.open( QFile::ReadOnly );
    QString style( styleFile.readAll() );
    this->setStyleSheet(style);

    //ScrollBar stylesheet
    this->verticalScrollBar()->setStyleSheet(style);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->horizontalHeader()->setStyleSheet("background-color: #7D7483;");
    this->verticalHeader()->setStyleSheet("background-color: #7D7483;");
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->verticalHeader()->setDefaultSectionSize(92);
}
