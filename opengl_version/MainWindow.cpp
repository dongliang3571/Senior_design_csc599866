// ===============================================================
// Computer Graphics Homework Solutions
// Copyright (C) 2015 by George Wolberg
//
// MainWindow.cpp - MainWindow class
//
// Written by: George Wolberg, 2015
// ===============================================================

#include "MainWindow.h"
#include "Threshold.h"
#include "Contrast.h"
#include "Quantization.h"


enum {
    DUMMY, THRESHOLD, CONTRAST, QUANTIZE,
    HISTOGRAMSTRETCHING, HISTOGRAMMATCHING,
    ERRORDIFFUSION, BLUR, SHARPEN, MEDIANFILTER
};

enum {RGB, R, G, B, GRAY};

QString GroupBoxStyle = "QGroupBox {				\
			border: 2px solid gray;			\
			border-radius: 9px;			\
			margin-top: 0.5em;}";

MainWindow *MainWindowP = NULL;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::MainWindow:
//
// MainWindow constructor.
//
MainWindow::MainWindow(QWidget *parent)
	:  QMainWindow(parent)
{
	setWindowTitle("Senior Design Homework");

	// set global variable for main window pointer
	MainWindowP = this;
    
    createGLWidgets();
    createMainWindow();
    createMenuActions();
    createMenus();
    
    // set stacked widget to first solution
    m_tabWidget->setCurrentIndex(0);
    m_stackWidgetControlPanel->setCurrentIndex(0);
}



// MainWindow::createGLWidgets:
//
// Create user-defined GLwidgets for display
//
void MainWindow::createGLWidgets()
{
    // create list of hw names; m_hwName name will be used for
    // tab name and as key for class in m_hw container
    m_widgetName << "Threshold" << "Contrast" << "Quantization";
    
    // instantiate homework solution classes
    m_glWidgets[m_widgetName[0]] = new Threshold();
    m_glWidgets[m_widgetName[1]] = new Contrast();
    m_glWidgets[m_widgetName[2]] = new Quantization();
}



// MainWindow::createWidgets:
//
// Create user-defined widgets for display and control panel.
// INSERT YOUR LINES HERE TO CREATE A TAB PER HOMEWORK OBJECT.
//
void MainWindow::createMainWindow()
{
    // Create a QHBoxLayout and add image view group box and control panel group box
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(createImageViewGroupBox());
    hbox->setStretch(0, 1);
    hbox->addWidget(createControlPanelGroupBox());
    
    // Create a new QWidget and set it as central widget of MainWindow
    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(hbox);
    setCentralWidget(mainWidget); // set central widget so that it can be displayed
}



// MainWindow::createMenuActions:
//
// Create actions to associate with menu and toolbar selection.
//
void MainWindow::createMenuActions()
{
    
    // File Actions
    m_actionOpen = new QAction("&Open", this);
    m_actionOpen->setShortcut(tr("Ctrl+O"));
//    connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    
    m_actionQuit = new QAction("&Quit", this);
    m_actionQuit->setShortcut(tr("Ctrl+Q"));
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    
    // Point Ops Actions
    m_actionThreshold = new QAction("&Threshold", this);
    m_actionThreshold->setShortcut(tr("Ctrl+T"));
    m_actionThreshold->setData(THRESHOLD);
    
//    connect(menuBar(), SIGNAL(triggered(QAction*)), this, SLOT(execute(QAction*)));
}



// MainWindow::createMenus:
//
// Create menus and install in menubar.
//
void MainWindow::createMenus()
{
    // File menu
    m_menuFile = menuBar()->addMenu("&File");
    m_menuFile->addAction(m_actionOpen);
    m_menuFile->addAction(m_actionQuit);
    
    // Point Ops menu
    m_menuPtOps = menuBar()->addMenu("&Point Ops");
    m_menuPtOps->addAction(m_actionThreshold);
    
    m_menuPtOps->setEnabled(false);
}




// MainWindow::createImageViewGroupBox:
//
// Create preview window groupbox.
//
QGroupBox* MainWindow::createImageViewGroupBox()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setStyleSheet(GroupBoxStyle);
    
    // create a tab widget to handle multiple displays
    m_tabWidget = new QTabWidget;
    
    // add one tab per homework problem
    for(int i = 0; i < (int) m_widgetName.size(); i++)
        m_tabWidget->addTab(m_glWidgets[m_widgetName[i]], m_widgetName[i]);
    
    // assemble stacked widget in vertical layout
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_tabWidget);
    groupBox->setLayout(vbox);
    
    // init signal/slot connections
    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));
    
    return groupBox;
}



// MainWindow::createControlPanelGroupBox:
//
// Create preview window groupbox.
//
QGroupBox* MainWindow::createControlPanelGroupBox()
{
    // init group box
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setMinimumWidth(300);
    
    // create pushbuttons
    QPushButton *buttonReset = new QPushButton("Reset");
    QPushButton *buttonQuit  = new QPushButton("Quit");
    
    // init signal/slot connections
    connect(buttonReset, SIGNAL(clicked()), this, SLOT(reset()));
    connect(buttonQuit , SIGNAL(clicked()), this, SLOT(quit ()));
    
    // add button to layout
    QHBoxLayout *buttonLayout = new QHBoxLayout(); // init a QHBoxLayout
    buttonLayout->addWidget(buttonReset);
    buttonLayout->addWidget(buttonQuit );
    
    // create a stacked widget to hold multiple control panels
    m_stackWidgetControlPanel = new QStackedWidget;
    
    // add control panels to stacked widget
    for(int i = 0; i < (int) m_widgetName.size(); i++)
        m_stackWidgetControlPanel->addWidget(m_glWidgets[m_widgetName[i]]->controlPanel());
    
    // add stacked widget and buttonlayout to vertical box layout
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_stackWidgetControlPanel);
    vbox->addStretch(1);
    vbox->addLayout(buttonLayout);
    groupBox->setLayout(vbox);
    
    return groupBox;
}




// MainWindow::changeTab:
//
// Slot function to change OpenGL canvas and control panel in stacked widget.
//
void MainWindow::changeTab(int index)
{
	m_tabWidget->setCurrentIndex(index);	// change OpenGL widget
	m_stackWidgetControlPanel->setCurrentIndex(index);	// change control panel

	// change keyboard focus to GL widget
	m_glWidgets[m_widgetName[index]]->setFocusPolicy(Qt::StrongFocus);
}



// MainWindow::open:
//
// Open input image.
//
//void MainWindow::open() {
//    QFileDialog dialog(this);
//    
//    // open the last known working directory
//    if(!m_currentDir.isEmpty())
//        dialog.setDirectory(m_currentDir);
//    
//    // display existing files and directories
//    dialog.setFileMode(QFileDialog::ExistingFile);
//    
//    // invoke native file browser to select file
//    m_file = dialog.getOpenFileName(this,
//                                    "Open File", m_currentDir,
//                                    "Images (*.jpg *.png *.ppm *.pgm *.bmp);;All files (*)");
//    
//    // verify that file selection was made
//    if(m_file.isNull()) return;
//    
//    // save current directory
//    QFileInfo f(m_file);
//    m_currentDir = f.absolutePath();
//    
//    
//    
//    
//}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::reset:
//
// Reset application.
//
void
MainWindow::reset()
{
	int index = m_tabWidget->currentIndex();	// current OpenGL widget
	m_glWidgets[ m_widgetName[index] ]->reset();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::quit:
//
// Quit application.
//
void
MainWindow::quit()
{
	// close the dialog window
	close();
}

