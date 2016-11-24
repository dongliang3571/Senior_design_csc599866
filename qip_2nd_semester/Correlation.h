// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Correlation.h - Correlation widget
//
// Written by: George Wolberg, 2016
// ======================================================================

#ifndef Correlation_H
#define Correlation_H

#include "ImageFilter.h"

class Correlation : public ImageFilter {
    Q_OBJECT
    
public:
    Correlation			(QWidget *parent = 0);	// constructor
    QGroupBox*	controlPanel	();			// create control panel
    bool		applyFilter	(ImagePtr, bool, ImagePtr);	// apply filter to input
    void		correlate	(ImagePtr, ImagePtr, ImagePtr);
    void		initShader();
    void		gpuProgram(int pass);	// use GPU program to apply filter
    
    protected slots:
    int		load		();
    void    match       ();
    
private:
    // widgets
    QPushButton*	m_button;	// Correlation pushbutton
    QPushButton*	m_button2;	// Correlation pushbutton
    QLabel*         m_label;
    QLabel*         m_label_template;
    QGroupBox*	m_ctrlGrp;	// groupbox for panel
    
    // variables
    QString		m_file;
    QString		m_currentDir;
    ImagePtr	m_template;
    int		m_width;	// input image width
    int		m_height;	// input image height
};

#endif	// Correlation_H
