QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES+="WITH_TBB=ON"
CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mousepainter.cpp \
    qimageshow.cpp

HEADERS += \
    mainwindow.h \
    mnist.h \
    mousepainter.h \
    nn.h \
    qimageshow.h

FORMS += \
    mainwindow.ui

OBJECTS += mnistmodel.o

# opencv
INCLUDEPATH += /usr/include/opencv4
LIBS += -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_dnn

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
