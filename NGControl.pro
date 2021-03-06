greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TEMPLATE = app
win32:RC_FILE = NGControl.rc
#CONFIG += console
CONFIG(debug, debug|release) {
    unix:DESTDIR = ../debug/unix
    unix:OBJECTS_DIR = ../debug/unix/objects
    unix:MOC_DIR = ../debug/unix/moc
    unix:RCC_DIR = ../debug/unix/moc

    win32:DESTDIR = ../debug/win32
    win32:OBJECTS_DIR = ../debug/win32/objects
    win32:MOC_DIR = ../debug/win32/moc
    win32:RCC_DIR = ../debug/win32/moc

    TARGET = NGControlProd

    DEFINES += DEBUG
}
else {
    unix:DESTDIR = ../release/unix
    unix:OBJECTS_DIR = ../release/unix/objects
    unix:MOC_DIR = ../release/unix/moc
    unix:RCC_DIR = ../release/unix/moc

    win32:DESTDIR = ../release/win32
    win32:OBJECTS_DIR = ../release/win32/objects
    win32:MOC_DIR = ../release/win32/moc
    win32:RCC_DIR = ../release/win32/moc

    TARGET = NGControlPro
}

HEADERS += \
    ComPort.h \
    IProtocol.h \
    Dialog.h \
    NGProtocol.h \
    RewindButton.h \
    SpinBox.h \
    ISpinBox.h \
    LCDSpinBox.h \
    SpinBoxButton.h

SOURCES += \
    ComPort.cpp \
    Dialog.cpp \
    main.cpp \
    NGProtocol.cpp \
    RewindButton.cpp \
    SpinBox.cpp \
    ISpinBox.cpp \
    LCDSpinBox.cpp \
    SpinBoxButton.cpp

RESOURCES += \
    NGControl.qrc
