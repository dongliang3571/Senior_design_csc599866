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
    
    bool BlurIsChecked    = m_radioButtons[0]->isChecked();
    bool SharpenIsChecked = m_radioButtons[1]->isChecked();
    
    int xsz    = m_sliderXrange  ->value();
    int ysz    = m_spinBoxYrange ->value();
    int factor = m_sliderFactor  ->value();
    
    if (BlurIsChecked) {
        Blur(I1, xsz, ysz, I2);
    } else if(SharpenIsChecked){
        Sharpen(I1, xsz, factor, I2);
    } else {
        return 0;
    }
    
    return 1;
}




QGroupBox*
BlurSharpen::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Blur and Sharpen");
    QLabel* xrange = new QLabel(QString("X range"));
    QLabel* yrange = new QLabel(QString("Y range"));
    QLabel* factor = new QLabel(QString("Factor"));
    
    m_radioButtons[0] = new QRadioButton("Blur", m_ctrlGrp);
    m_radioButtons[0]->setChecked(true);
    m_radioButtons[1] = new QRadioButton("Sharpen", m_ctrlGrp);
    m_radioButtons[1]->setChecked(false);
    
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
    
    m_sliderFactor = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderFactor->setTickPosition(QSlider::TicksBelow);
    m_sliderFactor->setTickInterval(5);
    m_sliderFactor->setMinimum(1);
    m_sliderFactor->setMaximum(50);
    m_sliderFactor->setValue  (1);
    m_sliderFactor->setEnabled(false);
    
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
    
    m_spinBoxFactor = new QSpinBox(m_ctrlGrp);
    m_spinBoxFactor->setMinimum   (1);
    m_spinBoxFactor->setMaximum   (100);
    m_spinBoxFactor->setValue     (1);
    m_spinBoxFactor->setSingleStep(1);
    m_spinBoxFactor->setEnabled(false);
    
    m_checkboxLockXY = new QCheckBox(QString("Synchronize X Y"));
    
    connect(m_radioButtons[0], SIGNAL(clicked(bool)),     this, SLOT(startBlurring(bool)));
    connect(m_radioButtons[1], SIGNAL(clicked(bool)),     this, SLOT(startSharpening(bool)));
    connect(m_sliderXrange,    SIGNAL(valueChanged(int)), this, SLOT(changeXrange(int)));
    connect(m_spinBoxXrange,   SIGNAL(valueChanged(int)), this, SLOT(changeXrange(int)));
    connect(m_sliderYrange,    SIGNAL(valueChanged(int)), this, SLOT(changeYrange(int)));
    connect(m_spinBoxYrange,   SIGNAL(valueChanged(int)), this, SLOT(changeYrange(int)));
    connect(m_sliderFactor,    SIGNAL(valueChanged(int)), this, SLOT(changeFactor(int)));
    connect(m_spinBoxFactor,   SIGNAL(valueChanged(int)), this, SLOT(changeFactor(int)));
    connect(m_checkboxLockXY,  SIGNAL(stateChanged(int)), this, SLOT(lockXY(int)));
    
    QGridLayout *layout = new QGridLayout;
    
    layout->addWidget(xrange,            0, 0);
    layout->addWidget(m_sliderXrange,    0, 1);
    layout->addWidget(m_spinBoxXrange,   0, 2);
    
    layout->addWidget(yrange,            1, 0);
    layout->addWidget(m_sliderYrange,    1, 1);
    layout->addWidget(m_spinBoxYrange,   1, 2);
    
    layout->addWidget(factor,            2, 0);
    layout->addWidget(m_sliderFactor,    2, 1);
    layout->addWidget(m_spinBoxFactor,   2, 2);
    
    layout->addWidget(m_radioButtons[0], 4, 0);
    layout->addWidget(m_checkboxLockXY);
    layout->addWidget(m_radioButtons[1], 5, 0);
    
    
    m_ctrlGrp->setLayout(layout);
    
    return(m_ctrlGrp);
}

void BlurSharpen::copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int kernel, int stride) {
    
    int i;
    int neighborSize = kernel/2;
    
    if (stride == 1) {
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
        int k = *(imagePtr+(width-1)*stride);
        for (i = neighborSize+width; i < bufferSize; i++) {
            buffer[i] = k;
        }
//        int count = 0;
        ChannelPtr<uchar> p = imagePtr;
        for (i = neighborSize; i < width+neighborSize; i++ ) {
            buffer[i] = *p;
            p += stride;
        }

    }
}

void
BlurSharpen::Blur(ImagePtr I1, int xsz, int ysz, ImagePtr I2) {
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
    
    //blurring the columns
    delete[] buffer;
    int YneighborSize = ysz/2;
    bufferSize = h + YneighborSize*2;
    buffer = new short[bufferSize];
    
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I2, ch, p2, type);

        for(x = 0; x < w; x++) {
            sum = 0;
            copyRowToBuffer(tempOut, h, ysz, w);
            in = buffer;

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


void BlurSharpen::Sharpen(ImagePtr I1, int filterSize, double factor, ImagePtr I2) {
    ImagePtr tempImage;
    IP_copyImageHeader(I1, tempImage);
    IP_copyImageHeader(I1, I2);
    
    
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, ch;
    ChannelPtr<uchar> p1, p2, endd, tempOut;
    
    if (filterSize == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }
    
    Blur(I1, filterSize, filterSize, tempImage);
    
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I1, ch, p1, type);
        IP_getChannel(I2, ch, p2, type);
        for(endd = tempOut + total; tempOut<endd; p2++, p1++, tempOut++)
            *p2 = CLIP(factor*(*p1-*tempOut), 0, 255);
    }
}

void BlurSharpen::startBlurring(bool clicked) {
    
    if(clicked == true) {
        m_sliderFactor ->setEnabled(false);
        m_spinBoxFactor->setEnabled(false);
        applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
        g_mainWindowP->displayOut();
    }
    
}

void BlurSharpen::startSharpening(bool clicked) {
    
    if(clicked == true) {
        m_sliderFactor ->setEnabled(true);
        m_spinBoxFactor->setEnabled(true);
        applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
        g_mainWindowP->displayOut();
    }
}

void BlurSharpen::changeXrange(int value) {
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

void BlurSharpen::changeYrange(int value) {
    settingSliderAndSpinBox(m_sliderYrange, m_spinBoxYrange, value);
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}

void BlurSharpen::changeFactor(int value) {
    settingSliderAndSpinBox(m_sliderFactor, m_spinBoxFactor, value);
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}

void BlurSharpen::lockXY(int isChecked) {
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

void BlurSharpen::settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value) {
    value += !(value%2);
    slider->blockSignals(true );
    slider->setValue    (value);
    slider->blockSignals(false);
    
    spinbox->blockSignals(true );
    spinbox->setValue    (value);
    spinbox->blockSignals(false);
}

void
BlurSharpen::reset() {}
