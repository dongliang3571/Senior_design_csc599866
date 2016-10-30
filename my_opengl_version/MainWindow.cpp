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
#include "HistogramStretching.h"
#include "HistogramMatching.h"
#include "Blur.h"
#include "Sharpen.h"
#include "MedianFilter.h"


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
    m_widgetName << "Threshold" << "Contrast" << "Quantization" << "Histogram Stretching"
                 << "Histogram Matching" << "Blur" << "Sharpen" << "Median Filter";
    
    // instantiate homework solution classes
    m_glWidgets[m_widgetName[0]] = new Threshold();
    m_glWidgets[m_widgetName[1]] = new Contrast();
    m_glWidgets[m_widgetName[2]] = new Quantization();
    m_glWidgets[m_widgetName[3]] = new HistogramStretching();
    m_glWidgets[m_widgetName[4]] = new HistogramMatching();
    m_glWidgets[m_widgetName[5]] = new Blur();
    m_glWidgets[m_widgetName[6]] = new Sharpen();
    m_glWidgets[m_widgetName[7]] = new MedianFilter();
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
    connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    
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
    groupBox->setMinimumWidth(400);
    
    // create a stacked widget to hold multiple control panels
    m_stackWidgetControlPanel = new QStackedWidget;
    
    // add control panels to stacked widget
    for(int i = 0; i < (int) m_widgetName.size(); i++)
        m_stackWidgetControlPanel->addWidget(m_glWidgets[m_widgetName[i]]->controlPanel());
    
    // A HBOX to hold display radio buttons and mode radio buttons
    QHBoxLayout *layoutDisplayAndMode = new QHBoxLayout();
    layoutDisplayAndMode->addWidget(createDisplayButtons());
    layoutDisplayAndMode->addWidget(createModeButtons());
    
    // add stacked widget and buttonlayout to vertical box layout
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addLayout(layoutDisplayAndMode);
    vbox->addWidget(m_stackWidgetControlPanel);
    vbox->addStretch(1);
    vbox->addWidget(createHistogram());
    vbox->addLayout(createExitButtons());
    groupBox->setLayout(vbox);
    
    return groupBox;
}



QHBoxLayout* MainWindow::createExitButtons()
{
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
    
    return buttonLayout;
}



QCustomPlot* MainWindow::createHistogram()
{
    // create histogram plot
    m_histogram = new QCustomPlot;
    
    // set histogram title
    m_histogram->plotLayout()->insertRow(0);
    m_histogram->plotLayout()->addElement(0, 0, new QCPPlotTitle(m_histogram,"Histogram"));
    
    // assign label axes
    m_histogram->xAxis->setLabel("Intensity");
    m_histogram->yAxis->setLabel("Frequency");
    m_histogram->xAxis->setAutoTickStep(0);
    m_histogram->xAxis->setTickStep(32.);
    
    // set axes ranges, so we see all the data
    m_histogram->xAxis->setRange(0, MXGRAY);
    m_histogram->yAxis->setRange(0, MXGRAY);
    m_histogram->setMinimumHeight(400);
    m_histogram->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    
    return m_histogram;

}



// MainWindow::createDisplayButtons
//
// Create preview window groupbox.
//
QGroupBox* MainWindow::createDisplayButtons()
{
    // init group box
    m_groupBox_display = new QGroupBox("Display");
    
    // create radio buttons
    m_radioDisplay[0] = new QRadioButton("Input");
    m_radioDisplay[1] = new QRadioButton("Output");
    
    // create button group and add radio buttons to it
    QButtonGroup *bGroup = new QButtonGroup;
    for(int i = 0; i<2; ++i)
        bGroup->addButton(m_radioDisplay[i]);
    
    // set input radio button to be default
    m_radioDisplay[1]->setChecked(true);
    
    // assemble radio buttons into vertical widget
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_radioDisplay[0]);
    vbox->addWidget(m_radioDisplay[1]);
    m_groupBox_display->setLayout(vbox);
    
    // init signal/slot connections
    connect(m_radioDisplay[0], SIGNAL(clicked()), this, SLOT(displayIn()));
    connect(m_radioDisplay[1], SIGNAL(clicked()), this, SLOT(displayOut()));
    
//    m_groupBox_display->setEnabled(false);
    
    return m_groupBox_display;
}



// MainWindow::createModeButtons:
//
// Create preview window groupbox.
//
QGroupBox*
MainWindow::createModeButtons()
{
    // init group box
    m_groupBox_mode = new QGroupBox("Mode");
    
    // create radio buttons
    m_radioMode[0] = new QRadioButton("RGB");
    m_radioMode[1] = new QRadioButton("Gray");
    
    // create button group and add radio buttons to it
    QButtonGroup *bGroup = new QButtonGroup;
    for(int i = 0; i<2; ++i)
        bGroup->addButton(m_radioMode[i]);
    
    // set RGB radio button to be default
    m_radioMode[0]->setChecked(true);
    
    // assemble radio buttons into vertical widget
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_radioMode[0]);
    //	vbox->addWidget(m_radioMode[1]);
    vbox->addWidget(m_radioMode[1]);	// redundant radiobutton won't display; placeholder
    m_groupBox_mode->setLayout(vbox);
    
    // init signal/slot connections
    connect(m_radioMode[0], SIGNAL(clicked()), this, SLOT(modeRGB ()));
    connect(m_radioMode[1], SIGNAL(clicked()), this, SLOT(modeGray()));
    
//    m_groupBox_mode->setEnabled(false);
    return m_groupBox_mode;
}




// MainWindow::changeTab:
//
// Slot function to change OpenGL canvas and control panel in stacked widget.
//
void MainWindow::changeTab(int index)
{
    if(!m_image.isNull()) {
        m_glWidgets[m_widgetName[index]]->setImage(m_image);
    }
    
	m_tabWidget->setCurrentIndex(index);	// change OpenGL widget
	m_stackWidgetControlPanel->setCurrentIndex(index);	// change control panel
    
	// change keyboard focus to GL widget
	m_glWidgets[m_widgetName[index]]->setFocusPolicy(Qt::StrongFocus);
}



// MainWindow::open:
//
// Open input image.
//
void MainWindow::open() {
    QFileDialog dialog(this);
    
    // open the last known working directory
    if(!m_currentDir.isEmpty())
        dialog.setDirectory(m_currentDir);
    
    // display existing files and directories
    dialog.setFileMode(QFileDialog::ExistingFile);
    
    // invoke native file browser to select file
    m_file = dialog.getOpenFileName(this,
                                    "Open File", m_currentDir,
                                    "Images (*.jpg *.png *.ppm *.pgm *.bmp);;All files (*)");
    
    // verify that file selection was made
    if(m_file.isNull()) return;
    
    // save current directory
    QFileInfo f(m_file);
    m_currentDir = f.absoluteFilePath();
    m_image.load(m_currentDir);
    
    int index = m_tabWidget->currentIndex();	// current OpenGL widget
    m_glWidgets[ m_widgetName[index] ]->setImage(m_image);
}



// MainWindow::displayIn:
//
// Slot functions to display input images.
//
void MainWindow::displayIn ()
{
    int index = m_tabWidget->currentIndex();	// current OpenGL widget
    m_glWidgets[ m_widgetName[index] ]->setDisplay(true);
    m_glWidgets[ m_widgetName[index] ]->reload();
}



// MainWindow::displayOut:
//
// Slot functions to display output images.
//
void MainWindow::displayOut()
{
    int index = m_tabWidget->currentIndex();	// current OpenGL widget
    m_glWidgets[ m_widgetName[index] ]->setDisplay(false);
    m_glWidgets[ m_widgetName[index] ]->reload();
}



// MainWindow::displayHistogram:
//
// Display image histogram in control panel
//
void MainWindow::displayHistogram(QImage image, bool isRGB)
{
    int width  = image.width();
    int height = image.height();
    QVector<double> x;
    
    for (int i = 0; i < MXGRAY; i++) {
        x.push_back(i);
    }
    
    // clear any previous histogram plots
    m_histogram->clearGraphs();
    m_histogram->legend->setVisible(true);
    
    if(isRGB) {
        QVector<double> red(MXGRAY, 0);
        QVector<double> green(MXGRAY, 0);
        QVector<double> blue(MXGRAY, 0);
        
        
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                QRgb pixel = image.pixel(x, y);
                int r = qRed(pixel);
                int g = qGreen(pixel);
                int b = qBlue(pixel);
                red[r]++;
                green[g]++;
                blue[b]++;
            }
        }
        
        // add new graph for histogram channel
        m_histogram->addGraph();
        m_histogram->graph(0)->setData(x, red);
        m_histogram->graph(0)->setPen (QPen(Qt::red));
        m_histogram->graph(0)->setName("R");
        
        m_histogram->addGraph();
        m_histogram->graph(1)->setData(x, green);
        m_histogram->graph(1)->setPen (QPen(Qt::green));
        m_histogram->graph(1)->setName("G");
        
        m_histogram->addGraph();
        m_histogram->graph(2)->setData(x, blue);
        m_histogram->graph(2)->setPen (QPen(Qt::blue));
        m_histogram->graph(2)->setName("B");
        
        
    } else {
        QVector<double> gray(MXGRAY, 0);
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                QRgb pixel = image.pixel(x, y);
                int gr = qGray(pixel);
                gray[gr]++;
            }
        }
        m_histogram->addGraph();
        m_histogram->graph(0)->setData(x, gray);
        m_histogram->graph(0)->setPen (QPen(Qt::gray));
        m_histogram->graph(0)->setName("G");
    }
    m_histogram->rescaleAxes();
    m_histogram->replot();
}



// MainWindow::modeRGB:
//
// Slot functions to display RGB and grayscale images.
//
void MainWindow::modeRGB()
{
    int index = m_tabWidget->currentIndex();	// current OpenGL widget
    m_glWidgets[ m_widgetName[index] ]->setMode(true);
    m_glWidgets[ m_widgetName[index] ]->reload();
    m_glWidgets[ m_widgetName[index] ]->displayHisto();
}



// MainWindow::modeGray:
//
// Slot functions to display RGB and grayscale images.
//
void MainWindow::modeGray()
{
    int index = m_tabWidget->currentIndex();	// current OpenGL widget
    m_glWidgets[ m_widgetName[index] ]->setMode(false);
    m_glWidgets[ m_widgetName[index] ]->reload();
    m_glWidgets[ m_widgetName[index] ]->displayHisto();
    
}



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

