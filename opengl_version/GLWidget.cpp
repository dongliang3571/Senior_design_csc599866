// ===============================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// GLWidget.cpp - GLWidget class. Base class of homework solutions.
//
// Written by: George Wolberg, 2015
// ===============================================================

#include "GLWidget.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GLWidget::GLWidget:
//
// GLWidget constructor.
// This is base class for homework solutions that will replace
// the control panel, reset function, and add homework solution. 
//
GLWidget::GLWidget(QWidget *parent)
   : QGLWidget (parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GLWidget::controlPanel:
//
// Create a control panel of widgets for homework solution.
//
QGroupBox*
GLWidget::controlPanel()
{
	return NULL;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// GLWidget::reset:
//
// Reset parameters in control panel.
//
void
GLWidget::reset() {}



// Threshold::setImage
//
// set image
//
void GLWidget::setImage(QImage image)
{
}


void GLWidget::reload()
{
}