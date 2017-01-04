// ======================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2016 by George Wolberg
//
// MainWindow.h - Header file for MainWindow class
//
// Written by: George Wolberg, 2016
// ======================================================================

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// ----------------------------------------------------------------------
// standard include files
//
#include <QtWidgets>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <vector>
#include <map>
#include <algorithm>
#include "IP.h"
#include "IPtoUI.h"
#include "ImageFilter.h"
#include "qcustomplot.h"

#define MAXFILTERS	50

using namespace IP;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	// constructor
	MainWindow	(QWidget *parent = 0);
	ImagePtr	imageSrc	() const;
	ImagePtr	imageDst	() const;
	QCustomPlot*	histogram()	{return m_histogram;}

public slots:
	void		open		();
	void		displayIn	();
	void		displayOut	();
	void		modeRGB		();
	void		modeGray	();
	void		preview		();
	void		reset		();
	void		quit		();
	void		execute		(QAction*);

protected slots:
	void		setHisto	(int);

protected:
	void		createActions	();
	void		createMenus	();
	void		createWidgets	();
	QGroupBox*	createGroupPanel();
	QGroupBox*	createGroupDisplay  ();
	QGroupBox*	createDisplayButtons();
	QGroupBox*	createModeButtons();
	QHBoxLayout*	createExitButtons();
	void		displayHistogram(ImagePtr);
	void		display		(int);
	void		mode		(int);


private:
	QMenu*			m_menuFile;
	QMenu*			m_menuPtOps;
    QMenu*          m_menuNeighborhoodOps;
	QAction*		m_actionOpen;
	QAction*		m_actionQuit;
    
    //Point operations
	QAction*		m_actionThreshold;
	QAction*		m_actionContrast ;
    QAction*        m_actionQuantize ;
    QAction*        m_actionHistogramStretching;
    QAction*        m_actionHistogramMatching;
    
    //Neighborhood operations
    QAction*        m_actionErrorDiffusion;
    QAction*        m_actionBlur;
    QAction*        m_actionSharpen;
    QAction*        m_actionMedianFilter;
	// homework objects
	ImageFilter*		m_imageFilterType[MAXFILTERS];

	// widgets
	QStackedWidget*		m_stackWidgetImages;
	QStackedWidget*		m_stackWidgetPanels;

	// widgets for image display groupbox
	QRadioButton*		m_radioDisplay[2];	// radio buttons for input/output
	QRadioButton*		m_radioMode   [2];	// radio buttons for RGB/Gray modes
	QCheckBox*          m_checkboxHisto;	// checkbox: histogram display
	QWidget*            m_extension;		// extension widget for histogram
	QCustomPlot*		m_histogram;		// histogram plot
    
    
    //groupbox
    QGroupBox*      m_groupBox_mode;
    QGroupBox*      m_groupBox_display;
    

	int             m_width;
	int             m_height;
	int             m_code;
	QString			m_file;
	QString			m_currentDir;
	ImagePtr		m_imageIn;
	ImagePtr		m_imageSrc;
	ImagePtr		m_imageDst;

	// histogram variables
	int			m_histoColor;	// histogram color id: 0=RGB, 1=R, 2=G, 3=B, 4=gray
	double			m_histoXmin[4];	// xmin for all histogram channels
	double			m_histoXmax[4];	// xmax for all histogram channels
	double			m_histoYmin[4];	// ymin for all histogram channels
	double			m_histoYmax[4];	// ymax for all histogram channels
};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::imageSrc:
//
// Source image.
//
inline ImagePtr
MainWindow::imageSrc() const
{
	return m_imageSrc;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::imageDst:
//
// Destination image.
//
inline ImagePtr
MainWindow::imageDst() const
{
	return m_imageDst;
}

#endif // MAINWINDOW_H
