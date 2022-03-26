QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++latest


QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qmandelbrot.cpp

HEADERS += \
    cl/mandelbrotCL.h \
    cl/opencl.hpp \
    cl/qcl.hpp \
    cl/qcl_array.hpp \
    cl/qcl_boost_compat.hpp \
    cl/qcl_module.hpp \
    common.h \
    mainwindow.h \
    mandel.h \
    qmandelbrot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resurces.qrc

unix:!macx: LIBS += -lOpenCL -lmpfr
