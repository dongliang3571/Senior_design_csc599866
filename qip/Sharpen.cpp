//
//  Sharpen.cpp
//  qip
//
//  Created by dong liang on 4/14/16.
//
//

#include "Sharpen.h"
#include "MainWindow.h"


extern MainWindow *g_mainWindowP;
Sharpen::Sharpen(QWidget *parent) : Blur(parent)
{}


bool
Sharpen::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // error checking
    if(I1.isNull()) return 0;
    
    
    int xsz    = m_sliderXrange  ->value();
    int ysz    = m_spinBoxYrange ->value();
    int factor = m_sliderFactor  ->value();
    
    
    Sharpening(I1, xsz, factor, I2);
    
    
    return 1;
}

QGroupBox*
Sharpen::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("Sharpen");
    QLabel* xrange = new QLabel(QString("Filter Width"));
    QLabel* yrange = new QLabel(QString("Filter Height"));
    QLabel* factor = new QLabel(QString("Factor"));
    
    m_radioButtons[0] = new QRadioButton("Edging", m_ctrlGrp);
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
    
    m_checkboxLockXY = new QCheckBox(QString("Synchronize X Y"));
    m_checkboxLockXY->setCheckState(Qt::Checked);
    
    connect(m_radioButtons[0], SIGNAL(clicked(bool)),     this, SLOT(startEdging(bool)));
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



void Sharpen::Sharpening(ImagePtr I1, int filterSize, double factor, ImagePtr I2) {
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
    
    Blurring(I1, filterSize, filterSize, tempImage);
    
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I1, ch, p1, type);
        IP_getChannel(I2, ch, p2, type);
        for(endd = tempOut + total; tempOut<endd; p2++, p1++, tempOut++)
            if (m_radioButtons[0]->isChecked()) {
                *p2 = CLIP(factor*(*p1-*tempOut), 0, 255);
            } else {
                *p2 = CLIP(*p1 + factor*(*p1-*tempOut), 0, 255);
            }
        
    }
}

void Sharpen::changeFactor(int value) {
    settingSliderAndSpinBox(m_sliderFactor, m_spinBoxFactor, value);
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}

void Sharpen::startEdging(bool isChecked) {
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}

void Sharpen::startSharpening(bool isChecked) {
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}

void
Sharpen::reset() {}
