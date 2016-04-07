//
//  BlurSharpen.cpp
//  qip
//
//  Created by dong liang on 4/7/16.
//
//

#include "BlurSharpen.h"
#include "MainWindow.h"


extern MainWindow *g_mainWindowP;
BlurSharpen::BlurSharpen(QWidget *parent) : ImageFilter(parent)
{}


bool
BlurSharpen::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // error checking
    if(I1.isNull()) return 0;
    
    int isChecked = 0;
    
    Blur(I1, isChecked, I2);
    
    return 1;
}




QGroupBox*
BlurSharpen::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Blur and Sharpen");
    
    
    
    return(m_ctrlGrp);
}

void BlurSharpen::copyRowToBuffer(ChannelPtr<uchar> &p1, int width, int kernel, int strike) {
    
    int i;
    
    buffer[0] = *p1;
    buffer[bufferSize-1] = *(p1+width-1);
    for (i = 1; i < bufferSize-1; i++) {
        buffer[i] = *p1++;
    }
}

void
BlurSharpen::Blur(ImagePtr I1, bool isChecked, int kernelImagePtr I2) {
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    
    bufferSize = w + kernel/2;
    
    
}


void
BlurSharpen::reset() {}
