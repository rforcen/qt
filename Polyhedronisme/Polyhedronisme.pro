QT       += core gui
#CONFIG+=sdk_no_version_check
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-conversion  \
-Wno-implicit-float-conversion -Wno-shorten-64-to-32 -Wno-sign-compare \
-Wno-multichar -Wno-four-char-constants -Wno-unused-function -Wno-gnu-anonymous-struct \
-Wno-nested-anon-types -Wno-double-promotion

CONFIG += c++2z


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
    gl01_widget.cpp \
    gl_widget.cpp \
    glpnt_widget.cpp \
    main.cpp \
    mainwindow.cpp \
    poly/common.cpp \
    poly/johnson.cpp \
    renderer.cpp

HEADERS += \
    Timer.h \
    gl01_widget.h \
    gl_widget.h \
    glpnt_widget.h \
    mainwindow.h \
    mesh.h \
    poly/Thread.h \
    poly/color.hpp \
    poly/common.hpp \
    poly/fastflags.h \
    poly/johnson.hpp \
    poly/parser.hpp \
    poly/point3.h \
    poly/poly_operations_mt.hpp \
    poly/polyhedron.hpp \
    poly/scanner.h \
    poly/seeds.hpp \
    renderer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
