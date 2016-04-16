//
//  Blur.cpp
//  qip
//
//  Created by dong liang on 4/7/16.
//
//

#include "Blur.h"
#include "MainWindow.h"


extern MainWindow *g_mainWindowP;
Blur::Blur(QWidget *parent) : ImageFilter(parent)
{}


bool
Blur::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // error checking
    if(I1.isNull()) return 0;
    
    int xsz    = m_sliderXrange  ->value();
    int ysz    = m_spinBoxYrange ->value();
    
    Blurring(I1, xsz, ysz, I2);

    return 1;
}




QGroupBox*
Blur::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Blur");
    QLabel* xrange = new QLabel(QString("Filter Width"));
    QLabel* yrange = new QLabel(QString("Filter Length"));

    m_sliderXrange = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderXrange->setTickPosition(QSlider::TicksBelow);
    m_sliderXrange->setTickInterval(5);
    m_sliderXrange->setMinimum(1);
    m_sliderXrange->setMaximum(100);
    m_sliderXrange->setValue  (1);
    
    m_sliderYrange = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderYrange->setTickPosition(QSlider::TicksBelow);
    m_sliderYrange->setTickInterval(5);
    m_sliderYrange->setMinimum(1);
    m_sliderYrange->setMaximum(100);
    m_sliderYrange->setValue  (1);
    
    m_spinBoxXrange = new QSpinBox(m_ctrlGrp);
    m_spinBoxXrange->setMinimum   (1);
    m_spinBoxXrange->setMaximum   (100);
    m_spinBoxXrange->setValue     (1);
    m_spinBoxXrange->setSingleStep(2);
    
    m_spinBoxYrange = new QSpinBox(m_ctrlGrp);
    m_spinBoxYrange->setMinimum   (1);
    m_spinBoxYrange->setMaximum   (100);
    m_spinBoxYrange->setValue     (1);
    m_spinBoxYrange->setSingleStep(2);
    
    m_checkboxLockXY = new QCheckBox(QString("Synchronize X Y"));
    
    connect(m_sliderXrange,    SIGNAL(valueChanged(int)), this, SLOT(changeXrange(int)));
    connect(m_spinBoxXrange,   SIGNAL(valueChanged(int)), this, SLOT(changeXrange(int)));
    connect(m_sliderYrange,    SIGNAL(valueChanged(int)), this, SLOT(changeYrange(int)));
    connect(m_spinBoxYrange,   SIGNAL(valueChanged(int)), this, SLOT(changeYrange(int)));
    connect(m_checkboxLockXY,  SIGNAL(stateChanged(int)), this, SLOT(lockXY(int)));
    
    QGridLayout *layout = new QGridLayout;
    
    layout->addWidget(xrange,            0, 0);
    layout->addWidget(m_sliderXrange,    0, 1);
    layout->addWidget(m_spinBoxXrange,   0, 2);
    
    layout->addWidget(yrange,            1, 0);
    layout->addWidget(m_sliderYrange,    1, 1);
    layout->addWidget(m_spinBoxYrange,   1, 2);
    
    
    
    layout->addWidget(m_checkboxLockXY,  2, 0);
    
    
    
    m_ctrlGrp->setLayout(layout);
    
    return(m_ctrlGrp);
}

void Blur::copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int kernel, int stride) {
    
    int i;
    int neighborSize = kernel/2;
    
    //when stride is one
    if (stride == 1) {
        //create left padding buffer
        for (i = 0; i < neighborSize; i++ ) {
            buffer[i] = *imagePtr;
        }
        
        //create buffer between left and right padding
        for (i = neighborSize; i < width+neighborSize; i++) {
            buffer[i] = *imagePtr++;
        }
        
        //create right padding buffer
        for (i = neighborSize+width; i < bufferSize; i++) {
            buffer[i] = *(imagePtr+width-1);
        }
        
    //When stride is width of the image
    } else {
        //create top padding buffer
        for (i = 0; i < neighborSize; i++ ) {
            buffer[i] = *imagePtr;
        }
        
        //create buffer between top and bottom padding
        int k = *(imagePtr+(width-1)*stride);
        for (i = neighborSize+width; i < bufferSize; i++) {
            buffer[i] = k;
        }
        
        //create bottom padding buffer
        ChannelPtr<uchar> p = imagePtr;
        for (i = neighborSize; i < width+neighborSize; i++ ) {
            buffer[i] = *p;
            p += stride;
        }

    }
}

void
Blur::Blurring(ImagePtr I1, int xsz, int ysz, ImagePtr I2) {
    //create a temporary image and copy header from input image
    ImagePtr tempImage;
    IP_copyImageHeader(I1, I2);
    IP_copyImageHeader(I1, tempImage);
    
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, y, x, ch, k, sum;

    ChannelPtr<uchar> p1, p2, endd, tempOut;
    
    if (xsz == 1 && ysz == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }
    
    // blurring the rows
    short* in;
    int XneighborSize = xsz/2;
    bufferSize = w + XneighborSize*2;
    buffer = new short[bufferSize];

    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(tempImage, ch, tempOut, type);
        
        //Loop through each row
        for(y = 0; y < h; y++) {
            sum = 0;
            copyRowToBuffer(p1, w, xsz, 1);
            in = buffer;
            
            //added up first xsz pixel in buffer
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
    
    //blurring the columns
    delete[] buffer;
    int YneighborSize = ysz/2;
    bufferSize = h + YneighborSize*2;
    buffer = new short[bufferSize];
    
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        
        //Loop through each column
        for(x = 0; x < w; x++) {
            sum = 0;
            copyRowToBuffer(tempOut, h, ysz, w);
            in = buffer;

            //added up first ysz pixel in buffer
            for (k = 0; k < ysz; k++) {
                sum += in[k];
            }

            int count = 0;
            for (y = YneighborSize; y < h+YneighborSize; y++) {
                p2[count*w] = sum/ysz;
                sum += (in[y+YneighborSize+1] - in[y-YneighborSize]);
                count++;
            }
            p2++;
            tempOut++;
        }
    }
    delete[] buffer;
}




void Blur::changeXrange(int value) {
    bool isChecked = m_checkboxLockXY->isChecked();
    if (isChecked) {
        settingSliderAndSpinBox(m_sliderXrange, m_spinBoxXrange, value);
        settingSliderAndSpinBox(m_sliderYrange, m_spinBoxYrange, value);
    } else {
        settingSliderAndSpinBox(m_sliderXrange, m_spinBoxXrange, value);
    }
        
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}

void Blur::changeYrange(int value) {
    settingSliderAndSpinBox(m_sliderYrange, m_spinBoxYrange, value);
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}



void Blur::lockXY(int isChecked) {
    if (isChecked == Qt::Checked) {
        int xValue = m_sliderXrange->value();
        settingSliderAndSpinBox(m_sliderYrange, m_spinBoxYrange, xValue);
        m_sliderYrange ->setEnabled(false);
        m_spinBoxYrange->setEnabled(false);
        
        applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
        g_mainWindowP->displayOut();
    } else {
        m_sliderYrange ->setEnabled(true);
        m_spinBoxYrange->setEnabled(true);
        
        applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
        g_mainWindowP->displayOut();
    }
    
}

void Blur::settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value) {
    value += !(value%2);
    slider->blockSignals(true );
    slider->setValue    (value);
    slider->blockSignals(false);
    
    spinbox->blockSignals(true );
    spinbox->setValue    (value);
    spinbox->blockSignals(false);
}

void
Blur::reset() {}
