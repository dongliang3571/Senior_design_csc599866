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
    
    void Blur(ImagePtr I1, bool isChecked, int xsz, int ysz, ImagePtr I2);
    
    protected slots:
//    void startBlur(int);
    
    
private:
    
    void            copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int kernel, int strike);
    
    int             bufferSize;
    short*          buffer;
//    short*          bufferDown;
    
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QCheckBox*      m_checkbox;
    
};



#endif /* BlurSharpen_h */
