#-------------------------------------------------
#
# Project created by QtCreator 2013-07-23T18:35:50
#
#-------------------------------------------------

TEMPLATE = app
QT       += core gui widgets
TARGET = CustomWindow

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(CustomWindow.pri)

copydata.commands = $(COPY_DIR) \"$$shell_path($$PWD\\data)\" \"$$shell_path($$OUT_PWD\\data)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

SOURCES += \
    main.cpp
