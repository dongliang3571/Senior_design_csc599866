// ======================================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// MainWindow.h - Header file for MainWindow class
//
// Written by: George Wolberg, 2015
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
#include "GLWidget.h"

typedef std::map<QString, GLWidget*> glWidget;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	// constructor
	MainWindow	(QWidget *parent = 0);


public slots:
	void		changeTab	(int);
    void		open		();
//    void		execute		(QAction*);
    void		displayIn	();
    void		displayOut	();
    void		modeRGB		();
    void		modeGray	();
    void		reset		();
    void		quit		();

protected:
    void createGLWidgets();
    void createMainWindow();
    void createMenuActions();
    void createMenus();
    
    // Two main conponents on the MainWindow
    QGroupBox*      createImageViewGroupBox();
    QGroupBox*      createControlPanelGroupBox();
    
    // These three methods are called in createControlPanelGroupBox()
    QGroupBox*      createDisplayButtons();
    QGroupBox*      createModeButtons();
    QHBoxLayout*	createExitButtons();

private:
	// homework objects
	QStringList	 m_widgetName;
	glWidget	 m_glWidgets;

	// widgets
	QTabWidget	*m_tabWidget;
	QStackedWidget	*m_stackWidgetControlPanel;
    
    // GUI components
    QString			m_file;
    QString			m_currentDir;
    QImage          m_image;
    QGroupBox*      m_groupBox_mode;    //groupbox
    QGroupBox*      m_groupBox_display; //groupbox
    
    // widgets for image display groupbox
    QRadioButton*	m_radioDisplay[2];	// radio buttons for input/output
    QRadioButton*	m_radioMode   [2];	// radio buttons for RGB/Gray modes
    QMenu*			m_menuFile;
    QMenu*			m_menuPtOps;
    QMenu*          m_menuNeighborhoodOps;
    QAction*		m_actionOpen;
    QAction*		m_actionQuit;
    QAction*		m_actionThreshold;
};

extern MainWindow *MainWindowP;

#endif // MAINWINDOW_H
