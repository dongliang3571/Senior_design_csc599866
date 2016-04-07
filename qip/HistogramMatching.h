//
//  HistogramMatching.h
//  qip
//
//  Created by dong liang on 3/25/16.
//
//

#ifndef HistogramMatching_h
#define HistogramMatching_h

#include <stdio.h>
#include "ImageFilter.h"

class HistogramMatching: public ImageFilter {
    Q_OBJECT
    
public:
    HistogramMatching	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters
    
    
    
    
protected:
//    void MatchingEq(ImagePtr I1, ImagePtr I2);
    void Matching  (ImagePtr I1, int value, ImagePtr I2);
    
protected slots:
    void generateHistogram(int);
    
    
private:
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_slider;
    QSpinBox*       m_spinbox;
    
    
    
    
};



#endif /* HistogramMatching.h */
