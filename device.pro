#-------------------------------------------------
#
# Project created by QtCreator 2018-08-31T15:12:27
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += serialport gui

TEMPLATE = lib

DEFINES += DEVICE_LIBRARY

release: TARGET = device
debug: TARGET = deviced

DESTDIR = $$(DEV_LIBS)

release: BUILDDIR = build/release
debug:   BUILDDIR = build/debug

OBJECTS_DIR = $$BUILDDIR/.obj
MOC_DIR = $$BUILDDIR/.moc
RCC_DIR = $$BUILDDIR/.qrc
UI_DIR = $$BUILDDIR/.ui

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBPATH = $$(DEV_LIBS)

if(debug){
LIBS += -L$$LIBPATH -ltagsystemd
}

INCLUDEPATH += ../../tagsystem


SOURCES += \
        device.cpp \
    serialportsettings.cpp \
    settingsdialog.cpp \
    inputdevicefactory.cpp \
    inputdevicemanager.cpp \
    pidextractdevice.cpp \
    baseinputdevicefactory.cpp \
    serialdevices/bmw712smart.cpp \
    serialdevices/oscilloscope.cpp

HEADERS += \
        device.h \
        device_global.h \ 
    serialportsettings.h \
    settingsdialog.h \
    inputdevicefactory.h \
    inputdevicemanager.h \
    pidextractdevice.h \
    baseinputdevicefactory.h \
    serialdevices/bmw712smart.h \
    serialdevices/oscilloscope.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    settingsdialog.ui
