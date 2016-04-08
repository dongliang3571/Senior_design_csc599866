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
    
    Blur(I1, true, 19, 7, I2);
    
    return 1;
}




QGroupBox*
BlurSharpen::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Blur and Sharpen");
    
    
    
    return(m_ctrlGrp);
}

void BlurSharpen::copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int kernel, int strike) {
    
    int i;
    int neighborSize = kernel/2;
//    printf("kernal us %d\n", kernel);
    if (strike == 1) {
        for (i = 0; i < neighborSize; i++ ) {
            buffer[i] = *imagePtr;
        }
        
        for (i = neighborSize+width; i < bufferSize; i++) {
            buffer[i] = *(imagePtr+width-1);
        }
        
        for (i = neighborSize; i < width+neighborSize; i++) {
            buffer[i] = *imagePtr++;
        }
    } else {
        for (i = 0; i < neighborSize; i++ ) {
            buffer[i] = *imagePtr;
        }
        
        for (i = neighborSize+width; i < bufferSize; i++) {
            buffer[i] = *(imagePtr+(width-1)*strike);
        }
        int count = 0;
        for (i = neighborSize; i < width+neighborSize; i++ ) {
            buffer[i] = *(imagePtr+strike*count);
            count++;
        }
        imagePtr++;

        
    }
}

void
BlurSharpen::Blur(ImagePtr I1, bool isChecked, int xsz, int ysz, ImagePtr I2) {
    ImagePtr tempImage;
    IP_copyImageHeader(I1, I2);
    IP_copyImageHeader(I1, tempImage);
    
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, y, x, ch, k, sum;

    ChannelPtr<uchar> p1, p2, endd, tempOut;
    
    if (xsz == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }
    
    short* in;
    int XneighborSize = xsz/2;
    bufferSize = w + XneighborSize*2;
    buffer = new short[bufferSize];
//    printf("buffer width is %d\n", bufferSize);
    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(tempImage, ch, tempOut, type);
        for(y = 0; y < h; y++) {
            sum = 0;
            copyRowToBuffer(p1, w, xsz, 1);
            in = buffer;
            
            for (k = 0; k < xsz; k++) {
                sum += in[k];
            }
            
            for (x = XneighborSize; x < w+XneighborSize; x++) {
                *tempOut = sum/xsz;
                sum += (in[x+XneighborSize+1] - in[x-XneighborSize]);
                tempOut++;
            }
        }
    }
    
    delete[] buffer;
    int YneighborSize = ysz/2;
    bufferSize = w + YneighborSize*2;
    buffer = new short[bufferSize];
    
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I1, ch, p1, type);
        IP_getChannel(I2, ch, p2, type);
//        for(x = 0; x < w; x++) {
            sum = 0;
            copyRowToBuffer(p1, h, ysz, w);
        copyRowToBuffer(p1, h, ysz, w);
        printf("heigh is %d\n", h);
        printf("buffersize is %d\n", bufferSize);
        for (x = 0; x < bufferSize; x++) {
            printf("indxe at %d, valye is %d\n", x*w+1, buffer[x]);
        }
//        }
    }
    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        for (x = 0; x < total; x++) {
            printf("at index %d value is %d\n", x , *p1++);
        }
    }
}


void
BlurSharpen::reset() {}
