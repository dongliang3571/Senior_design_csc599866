//
//  Sharpen.h
//  qip
//
//  Created by dong liang on 4/14/16.
//
//

#ifndef Sharpen_h
#define Sharpen_h

#include <stdio.h>
#include "Blur.h"
#include "ImageFilter.h"

class Sharpen: public Blur {
    Q_OBJECT
    
public:
    
    Sharpen	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters
    
    
protected:
    
    void Sharpening(ImagePtr I1, int filterSize, double factor, ImagePtr I2);
    
    protected slots:
        void changeFactor   (int value);
        void startEdging    (bool isChecked);
        void startSharpening(bool isChecked);
    
private:
    QRadioButton*   m_radioButtons[2];
    QSlider*        m_sliderFactor;
    QSpinBox*       m_spinBoxFactor;
    
    
};

#endif /* Sharpen_h */
