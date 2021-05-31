QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    game.cpp \
    gamebutton.cpp \
    gamefield.cpp \
    gamelabel.cpp \
    gamelist.cpp \
    gamelogo.cpp \
    gameobject.cpp \
    gameslider.cpp \
    gametable.cpp \
    gametextedit.cpp \
    main.cpp \
    scenegame.cpp \
    scenemenu.cpp

HEADERS += \
    client.h \
    game.h \
    gamebutton.h \
    gamefield.h \
    gamelabel.h \
    gamelist.h \
    gamelogo.h \
    gameobject.h \
    gameslider.h \
    gametable.h \
    gametextedit.h \
    scenegame.h \
    scenemenu.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
