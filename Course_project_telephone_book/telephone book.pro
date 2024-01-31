QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sources/contactButton.cpp \
    sources/contactdialog.cpp \
    sources/contactedit.cpp \
    sources/contactinfo.cpp \
    sources/contactshow.cpp \
    sources/main.cpp \
    sources/mainwindow.cpp

HEADERS += \
    headers/List.h \
    headers/contactButton.h \
    headers/contactdialog.h \
    headers/contactedit.h \
    headers/contactinfo.h \
    headers/contactshow.h \
    headers/exception.h \
    headers/fileexception.h \
    headers/inputexception.h \
    headers/mainwindow.h \
    headers/inputexception.h \
    headers/mainwindow.h

FORMS += \
    contactdialog.ui \
    contactedit.ui \
    contactshow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resources/down.png \
    resources/edit.png \
    resources/plus.png \
    resources/show.png \
    resources/sprav.png \
    resources/up.png

RESOURCES += \
    resources/images.qrc

win32:RC_ICONS += sprav.ico
