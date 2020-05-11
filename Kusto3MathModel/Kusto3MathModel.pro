QT += core gui network widgets webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    KX_pult_src/configdata.cpp \
    KX_pult_src/kx_protocol.cpp \
    KX_pult_src/qkx_coeffs.cpp \
    KX_pult_src/qpiconfig.cpp \
    main.cpp \
    senderwidget.cpp \
    su_rov.cpp

HEADERS += \
    KX_pult_src/configdata.h \
    KX_pult_src/kx_protocol.h \
    KX_pult_src/qkx_coeffs.h \
    KX_pult_src/qpiconfig.h \
    senderwidget.h \
    su_rov.h

FORMS += \
    senderwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
