//
//  ErrorDiffusion.cpp
//  qip
//
//  Created by dong liang on 4/3/16.
//
//

#include "ErrorDiffusion.h"
#include "MainWindow.h"
#include <cmath>

extern MainWindow *g_mainWindowP;


ErrorDiffusion::ErrorDiffusion(QWidget *parent) : ImageFilter(parent)
{
    
}



bool
ErrorDiffusion::applyFilter(ImagePtr I1, ImagePtr I2) {
    
    if(I1.isNull()) return 0;
    
    bool isChecked = m_checkbox->isChecked();
    
    ErrorDiffuse(I1, isChecked, I2);
    
    return true;
}



QGroupBox*
ErrorDiffusion::controlPanel()
{
    m_ctrlGrp = new QGroupBox("ErrorDiffusion");
    
    m_checkbox = new QCheckBox("Error Diffusion");
    
    connect(m_checkbox , SIGNAL(stateChanged(int)), this, SLOT(startDiffuse(int)));
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_checkbox, 0, 0);
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;
    
}

void ErrorDiffusion::copyRowToBuffer(ChannelPtr<uchar> &p1, int width, int row) {
    
    int i;
    
    if (row%2 == 0) {
        bufferUp[0] = *p1;
        bufferUp[bufferSize-1] = *(p1+width-1);
        for (i = 1; i < bufferSize-1; i++) {
            bufferUp[i] = *p1++;
        }
    } else {
        bufferDown[0] = *p1;
        bufferDown[bufferSize-1] = *(p1+width-1);
        for (i = 1; i < bufferSize-1; i++) {
            bufferDown[i] = *p1++;
        }
    }
}






void ErrorDiffusion::ErrorDiffuse(ImagePtr I1, bool isChecked, ImagePtr I2) {
    
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, y, x, ch;
    ChannelPtr<uchar> p1, p2, endd;
    
    if(isChecked) {
        short* in1;
        short* in2;
        int thr = MXGRAY/2;
        int error;
        bufferSize = w+2;
        bufferUp   = new short[bufferSize];
        bufferDown = new short[bufferSize];
        
        for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            copyRowToBuffer(p1, w, 0);
            
            for(y = 1; y < h; y++) {
                copyRowToBuffer(p1, w, y);
                if (y%2 == 0) {
                    in2 = bufferUp   + 1;
                    in1 = bufferDown + 1;
                } else {
                    in1 = bufferUp   + 1;
                    in2 = bufferDown + 1;
                }
                for (x = 0; x < w; x++) {
                    *p2 = (*in1 < thr)? 0 : 255;
                    error = *in1 - *p2;
                    *(in1+1)   += (error * 7/16);
                    *(in2-1)   += (error * 3/16);
                    *(in2)     += (error * 5/16);
                    *(in2+1)   += (error * 1/16);
                    in1++;
                    in2++;
                    p2++;
                }
            }
        }
        delete [] bufferUp;
        delete [] bufferDown;
    } else {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
    }
}

void ErrorDiffusion::startDiffuse(int value) {
    
    if (value != 0) {
        m_checkbox->blockSignals(true);
        m_checkbox->setCheckState(Qt::Checked);
        m_checkbox->blockSignals(false);
    } else {
        m_checkbox->blockSignals(true);
        m_checkbox->setCheckState(Qt::Unchecked);
        m_checkbox->blockSignals(false);
    }
    
    
    
    
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    
    // display output
    g_mainWindowP->displayOut();
}


void ErrorDiffusion::reset() {
    
    qDebug() <<MaxGray <<"  "<<MXGRAY;
    
}
