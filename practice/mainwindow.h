#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QMenuBar>
//#include <QAction>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    
//    private methods
    void createActions();
    void createMenu();
    
    
    
//    private members
    QWidget *m_myqwidget;
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QVBoxLayout *m_mainlayout;
    QActionGroup *m_alignmentGroup;
    QAction *m_newAct;
    QAction *m_openAct;
    QAction *m_saveAct;
    QAction *m_changeAct;
};

#endif // MAINWINDOW_H
