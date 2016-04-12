//
//  BlurSharpen.hpp
//  qip
//
//  Created by dong liang on 4/7/16.
//
//

#ifndef BlurSharpen_h
#define BlurSharpen_h

#include <stdio.h>
#include "ImageFilter.h"

class BlurSharpen: public ImageFilter {
    Q_OBJECT
    
public:
    BlurSharpen	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters
    
    
protected:
    
    void Blur(ImagePtr I1, int xsz, int ysz, ImagePtr I2);
    void Sharpen(ImagePtr I1, int filterSize, double factor, ImagePtr I2);
    
    protected slots:
        void startBlurring  (bool clicked);
        void startSharpening(bool clicked);
        void changeXrange   (int  value);
        void changeYrange   (int  value);
        void changeFactor   (int  value);
        void lockXY         (int  isChecked);
    
    
private:
    
    void            copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int kernel, int strike);
    void            settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value);
    int             bufferSize;
    short*          buffer;
    
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QRadioButton*   m_radioButtons[2];
    QSlider*        m_sliderXrange;
    QSlider*        m_sliderYrange;
    QSpinBox*       m_spinBoxXrange;
    QSpinBox*       m_spinBoxYrange;
    
    QSlider*        m_sliderFactor;
    QSpinBox*       m_spinBoxFactor;
    
    QCheckBox*      m_checkboxLockXY;
    
};



#endif /* BlurSharpen_h */
