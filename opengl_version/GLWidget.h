// ======================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// GLWidget.h - Header file for GLWidget class. Base class of homework solutions.
//
// Written by: George Wolberg, 2015
// ======================================================================

#ifndef GLWidget_H
#define GLWidget_H

#define MXGRAY 256
#define MaxGray 255

#include <QtOpenGL>
#include <QtWidgets>
//#include <QGLWidget>      // This is included in <QtOpenGL>
//#include <QGLFunctions>   // This is included in <QtOpenGL>
//#include <GL/glu.h>

typedef QVector2D vec2;
typedef QVector3D vec3;

enum {
	ATTRIB_VERTEX,
	ATTRIB_COLOR,
	ATTRIB_TEXTURE_POSITION
};

// ----------------------------------------------------------------------
// standard include files
//
class GLWidget : public QGLWidget, protected QGLFunctions {

public:
	GLWidget(QWidget *parent = 0);
	virtual QGroupBox*	controlPanel();		// create control panel
	virtual void		reset();		// reset parameters
};

#endif // GLWidget_H
