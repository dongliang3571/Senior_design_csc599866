TEMPLATE = app
TARGET = hw
QT += widgets opengl printsupport
OBJECTS_DIR = ./obj
MOC_DIR     = ./moc
RESOURCES   = hw.qrc

win32-msvc2013 {
	LIBS += -lopengl32 -lglu32
	QMAKE_CXXFLAGS += /MP /Zi
}


macx{
        QMAKE_MAC_SDK = macosx10.11
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
}

unix:!macx {
	CONFIG += C++11
}

# Input
HEADERS +=	MainWindow.h  \
		GLWidget.h		  \
        qcustomplot.h     \
        Threshold.h       \
        Contrast.h        \
        Quantization.h    \
        HistogramStretching.h \
        HistogramMatching.h   \
        Blur.h            \
		
SOURCES +=	main.cpp	  \
		MainWindow.cpp 	  \
        qcustomplot.cpp   \
		GLWidget.cpp	  \
        Threshold.cpp     \
        Contrast.cpp      \
        Quantization.cpp  \
        HistogramStretching.cpp \
        HistogramMatching.cpp   \
        Blur.cpp          \