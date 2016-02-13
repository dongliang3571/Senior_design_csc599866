#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_myqwidget = new QWidget();
    setCentralWidget(m_myqwidget);
    m_mainlayout = new QVBoxLayout();
    m_myqwidget->setLayout(m_mainlayout);
    createActions();
    createMenu();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createMenu() {
    m_fileMenu = menuBar()->addMenu(tr("File"));
    m_fileMenu->addAction(m_newAct);
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_editMenu = menuBar()->addMenu(tr("Edit"));
    m_editMenu->addAction(m_changeAct);
    
    
}

void MainWindow::createActions() {
    m_newAct = new QAction(tr("New"), this);
    m_newAct->setShortcut(tr("Ctrl+N"));
    m_openAct = new QAction(tr("Open"), this);
    m_saveAct = new QAction(tr("Save"), this);
    m_changeAct = new QAction(tr("change"), this);
}
