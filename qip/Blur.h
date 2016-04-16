//
//  Blur.h
//  qip
//
//  Created by dong liang on 4/7/16.
//
//

#ifndef Blur_h
#define Blur_h

#include <stdio.h>
#include "ImageFilter.h"

class Blur: public ImageFilter {
    Q_OBJECT
    
public:
    Blur	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters
    
    
protected:
    
    void            Blurring(ImagePtr I1, int xsz, int ysz, ImagePtr I2);
    void            copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int kernel, int strike);
    void            settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value);
    int             bufferSize;
    short*          buffer;
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderXrange;
    QSlider*        m_sliderYrange;
    QSpinBox*       m_spinBoxXrange;
    QSpinBox*       m_spinBoxYrange;
    QCheckBox*      m_checkboxLockXY;
    
    protected slots:
        void changeXrange   (int  value);
        void changeYrange   (int  value);
        void lockXY         (int  isChecked);
    
    
private:
    
    
    
    
    
};



#endif /* Blur_h */
