#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T18:18:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = svm-app
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    availabilityhandler.cpp \
    filemanager.cpp \
    loadingindicator.cpp \
        mainwindow.cpp \
    scriptqtmanager.cpp \
    svmcontroller.cpp \
    svm.cpp \
    svmscale.cpp \
    outputhandler.cpp

HEADERS  += mainwindow.h \
    availabilityhandler.h \
    filemanager.h \
    loadingindicator.h \
    scriptqtmanager.h \
    svmcontroller.h \
    svm.h \
    svmscale.h \
    outputhandler.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
        checkdata.py
        f_select.py

DISTFILES += \
    plotter.py \
    pointsgen.py \
    svm-train.c \
    svm-predict.c \
    checkdata.py \
    f_select.py \
    grid.py \
    h4c.py \
    h4r.py \
    convert2svm.py

#copy python scripts
copydata.commands = $(COPY_DIR) $$PWD/checkdata.py $$PWD/f_select.py $$PWD/h4c.py $$PWD/h4r.py $$PWD/convert2svm.py $$PWD/grid.py $$PWD/svm-train.c $$PWD/svm-predict.c $$PWD/pointsgen.py $$PWD/$$OUT_PWD
#first.depends = $(first) copydata
#export(first.depends)
#export(copydata.commands)
#QMAKE_EXTRA_TARGETS += first copydata
