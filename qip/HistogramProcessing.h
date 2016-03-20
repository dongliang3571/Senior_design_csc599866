//
//  HistogramProcessing.h
//  qip
//
//  Created by dong liang on 3/19/16.
//
//

#ifndef HISTOGRAMPROCESSING_H
#define HISTOGRAMPROCESSING_H

#include <stdio.h>


#include "ImageFilter.h"

class HistogramProcessing: public ImageFilter {
    Q_OBJECT
    
public:
    HistogramProcessing	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters
    
    void        settingSliderAndSpinBox(QSlider*, QSpinBox*, int);


protected:
    void Stretching(ImagePtr I1, int max, int min, ImagePtr I2);
    
    protected slots:
    void changeStretchMax (int stretchValue);
    void changeStretchMin (int stretchValue);
    void autoStretch(int isAuto);
//    void autoMinStretch(int min);

    
private:
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderMax;
    QSlider*        m_sliderMin;
    QSpinBox*       m_spinboxMax;
    QSpinBox*       m_spinboxMin;
    QCheckBox*      m_checkBoxAutoMax;
    QCheckBox*      m_checkBoxAutoMin;
    
};



#endif /* HistogramProcessing.h */
