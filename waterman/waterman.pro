QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
INCLUDEPATH+=qh3d gl
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gl/opengl.cpp \
    gl/renderer.cpp \
    main.cpp \
    mainwindow.cpp \
    qh3d/Face.cpp \
    qh3d/FaceList.cpp \
    qh3d/HalfEdge.cpp \
    qh3d/Point3d.cpp \
    qh3d/QuickHull3D.cpp \
    qh3d/Vector3d.cpp \
    qh3d/Vertex.cpp \
    qh3d/VertexList.cpp \
    qh3d/Waterman.cpp \
    watermanrender.cpp


HEADERS += \
    gl/opengl.h \
    gl/renderer.h \
    mainwindow.h \
    qh3d/Face.h \
    qh3d/FaceList.h \
    qh3d/HalfEdge.h \
    qh3d/Point3d.h \
    qh3d/QuickHull3D.h \
    qh3d/Vector3d.h \
    qh3d/Vertex.h \
    qh3d/VertexList.h \
    qh3d/Waterman.h \
    watermanrender.h


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
