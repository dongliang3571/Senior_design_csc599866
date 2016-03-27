TEMPLATE    = app
TARGET      = qip
QT 	   += widgets printsupport
OBJECTS_DIR = ./obj
MOC_DIR     = ./moc


win32-msvc2013 {
	INCLUDEPATH 	+= ./IP/win/header
	LIBS 		+= -L./IP/win/lib
	LIBS 		+= -lIP_d
	QMAKE_CXXFLAGS  += /MP /Zi
}


macx{
        QMAKE_MAC_SDK = macosx10.11
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
	INCLUDEPATH += ./IP/mac/header
	LIBS        += -L./IP/mac/lib
	LIBS        += -lIP_d
}

unix:!macx {
	CONFIG += C++11
	INCLUDEPATH += ./IP/linux/header
	LIBS        += -L./IP/linux/lib
	LIBS        += -lIP_d
}

# Input
HEADERS += Contrast.h \
           Dummy.h \
           ImageFilter.h \
           MainWindow.h \
           qcustomplot.h \
           Threshold.h \
           Quantize.h  \
           HistogramStretching.h  \
           HistogramMatching.h    \
          

SOURCES += Contrast.cpp \
           Dummy.cpp \
           ImageFilter.cpp \
           main.cpp \
           MainWindow.cpp \
           qcustomplot.cpp \
           Threshold.cpp \
           Quantize.cpp  \
           HistogramStretching.cpp \
           HistogramMatching.cpp   \

DISTFILES += \
    IP/mac/lib/libIP_d.a
