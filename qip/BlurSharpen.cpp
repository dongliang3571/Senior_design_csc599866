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
    
    Blur(I1, true, 3, 1, I2);
    
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
    int neighborSize = kernel/2;
    
    for (i = 0; i < neighborSize; i++ ) {
        buffer[i] = *p1;
    }
    
    for (i = neighborSize+width; i < bufferSize; i++) {
        buffer[i] = *(p1+width-1);
    }
    
    for (i = neighborSize; i < width+neighborSize; i++) {
        buffer[i] = *p1++;
    }
}

void
BlurSharpen::Blur(ImagePtr I1, bool isChecked, int kernel, int strike, ImagePtr I2) {
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, y, x, ch;
    ChannelPtr<uchar> p1, p2, endd;
    
    if (kernel == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }
    
    int neighborSize = kernel/2;
    short* in1;
    bufferSize = w + neighborSize*2;
    buffer = new short[bufferSize];
    printf("buffer width is %d\n", bufferSize);
    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        
        IP_getChannel(I2, ch, p2, type);
        
//        for (x = 0; x<w; x++) {
//            printf("value at %d is %d\n", x, *p1++);
//        }
        copyRowToBuffer(p1, w, kernel, strike);
        
        for (x = 0; x < bufferSize; x++) {
            printf("value at index %d is %d\n", x,buffer[x]);
        }
        copyRowToBuffer(p1, w, kernel, strike);
        
        for (x = 0; x < bufferSize; x++) {
            printf("value22 at index %d is %d\n", x,buffer[x]);
        }
//        for(y = 1; y < h; y++) {
//            
//            copyRowToBuffer(p1, w, kernel, strike);
//            in1 = buffer + 1;
//            
//            for (x = 0; x < w; x++) {
//                
//                *p2 = (*in1 < thr)? 0 : 255;
//                error = *in1 - *p2;
//                
//                *(in1+1)   += (error * 3/16);
//                *(in2-1)   += (error * 5/16);
//                *(in2)     += (error * 1/16);
//                *(in2+1)   += (error * 7/16);
//                
//                in1++;
//                in2++;
//                p2++;
//            }
//        }
    }
}


void
BlurSharpen::reset() {}
