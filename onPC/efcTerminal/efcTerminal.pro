#-------------------------------------------------
#
# Project created by QtCreator 2014-01-22T03:54:04
#
#-------------------------------------------------

QT       += core serialport

QT       -= gui

TARGET = efcTerminal
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    terminalhandler.cpp \
    serialhandler.cpp \
    dispatcher.cpp \
    commands/commands.cpp \
    commands/commandmanager.cpp \
    commands/reactions.cpp

HEADERS += \
    terminalhandler.h \
    serialhandler.h \
    dispatcher.h \
    commands/defines.h \
    commands/commandtypes.h \
    commands/commanddata.h \
    commands/reactions.h \
    commands/commands.h \
    commands/commandmanager.h \
    commands/commands_include.h

DEFINES += QT_ENABLED
