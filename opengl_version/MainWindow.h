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
//    void		open		();
//    void		execute		(QAction*);
    void		reset		();
    void		quit		();

protected:
	QGroupBox*      createGroupView	  ();
	QGroupBox*      createGroupInput  ();
	QHBoxLayout*	createExitButtons ();
    
    void createGLWidgets();
    void createMainWindow();
    void createMenuActions();
    void createMenus();
    QGroupBox* createImageViewGroupBox();
    QGroupBox* createControlPanelGroupBox();
    
    

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
    
    QMenu*			m_menuFile;
    QMenu*			m_menuPtOps;
    QMenu*          m_menuNeighborhoodOps;
    QAction*		m_actionOpen;
    QAction*		m_actionQuit;
    QAction*		m_actionThreshold;
};

extern MainWindow *MainWindowP;

#endif // MAINWINDOW_H
