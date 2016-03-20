// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Contrast.cpp - Brightness/Contrast widget.
//
// Written by: George Wolberg, 2016
// ======================================================================

#include "MainWindow.h"
#include "Contrast.h"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::Contrast:
//
// Constructor.
//
Contrast::Contrast(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Contrast::applyFilter(ImagePtr I1, ImagePtr I2)
{
    // INSERT YOUR CODE HERE


    if(I1.isNull()) return 0;
    // get threshold value
    double brigt, contr;	// brightness, contrast parameters
    brigt = (double) m_sliderB->value();
    contr  = (double) m_sliderC->value();

    // error checking
    if(brigt < -256 || brigt > 256 || contr < -100 || contr >100) return 0;

    // apply filter
    contrast(I1, brigt, contr, I2);

    return 1;
}





// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::createGroupBox:
//
// Create group box for control panel.
//
QGroupBox*
Contrast::controlPanel()
{
    m_ctrlGrp = new QGroupBox("Contrast");
    // init group box
    QLabel *label_brightness = new QLabel;
    label_brightness->setText(QString("Brightness"));

    //slider for brightness
    m_sliderB = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderB->setTickPosition(QSlider::TicksBelow);
    m_sliderB->setTickInterval(25);
    m_sliderB->setMinimum(-256);
    m_sliderB->setMaximum(256);
    m_sliderB->setValue  (0);

    //spinbox for brightness
    m_spinBoxB = new QSpinBox(m_ctrlGrp);
    m_spinBoxB->setMinimum   (-256);
    m_spinBoxB->setMaximum   (256);
    m_spinBoxB->setValue     (0);
    m_spinBoxB->setSingleStep(10);

    QLabel *label_contrast = new QLabel;
    label_contrast->setText(QString("Contrast"));

    //slider for contrast
    m_sliderC = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderC->setTickPosition(QSlider::TicksBelow);
    m_sliderC->setTickInterval(25);
    m_sliderC->setRange(-100, 100);
    m_sliderC->setValue  (0);


    // spinbox for contrast
    m_spinBoxC = new QDoubleSpinBox(m_ctrlGrp);
    m_spinBoxC->setRange     (0.25, 5.0);
    m_spinBoxC->setValue     (0.0);
    m_spinBoxC->setDecimals  (1);
    m_spinBoxC->setSingleStep(0.5);


    // init signal/slot connections for Threshold
    connect(m_sliderB , SIGNAL(valueChanged(int)),    this, SLOT(changeBrightness (int)));
    connect(m_spinBoxB, SIGNAL(valueChanged(int)),    this, SLOT(changeBrightness (int)));
    connect(m_sliderC,  SIGNAL(valueChanged(int)),    this, SLOT(changeContr_slide(int)));
    connect(m_spinBoxC, SIGNAL(valueChanged(double)), this, SLOT(changeContr_spinB(double)));

    // INSERT YOUR CODE HERE

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label_brightness, 0, 0);
    layout->addWidget(m_sliderB,        0, 1);
    layout->addWidget(m_spinBoxB,       0, 2);

    layout->addWidget(label_contrast,   1, 0);
    layout->addWidget(m_sliderC,        1, 1);
    layout->addWidget(m_spinBoxC,       1, 2);

    m_ctrlGrp->setLayout(layout);

    return(m_ctrlGrp);
}


double calculateContr(double contrast) {

    double contr;

    if (contrast >= 0) {
        contr = contrast / 25.0 + 1.0;
    }
    else {
        contr = contrast / 133.0 + 1.0;
    }

    return contr;

}

int calculateContr_int(double contrast) {
    int contr;

    if (contrast >= 1) {
        contr = (contrast - 1) * 25;
    }
    else {
        contr = (contrast - 1) * 133;
    }

    return contr;
}



// gray = .3R +.59G + .11B
void
Contrast::contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{

    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    double contr;

    //set contrast range to 0.25 to 5
    contr = calculateContr(contrast);

    // compute lut[]
    int reference = 128;
    int i, lut[MXGRAY];
    for(i=0; i<MXGRAY; ++i)
        lut[i] = (int)CLIP((i - reference)*contr + reference + brightness, 0, 255);


    int type;
    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
}


void Contrast::changeBrightness(int brightness)
{

    m_sliderB ->blockSignals(true );
    m_sliderB ->setValue    (brightness);
    m_sliderB ->blockSignals(false);
    m_spinBoxB->blockSignals(true );
    m_spinBoxB->setValue    (brightness);
    m_spinBoxB->blockSignals(false);


    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();


}

void Contrast::changeContr_slide(int contrast)
{

        m_sliderC ->blockSignals(true );
        m_sliderC ->setValue    (contrast);
        m_sliderC ->blockSignals(false);

        double temp = calculateContr((double)contrast);
        m_spinBoxC->blockSignals(true );
        m_spinBoxC->setValue    (temp);
        m_spinBoxC->blockSignals(false);


    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}


void Contrast::changeContr_spinB(double contrast)
{

    double temp = calculateContr_int(contrast);
    m_sliderC ->blockSignals(true );
    m_sliderC ->setValue    (temp);
    m_sliderC ->blockSignals(false);

    m_spinBoxC->blockSignals(true );
    m_spinBoxC->setValue    (contrast);
    m_spinBoxC->blockSignals(false);


    // apply filter to source image; save result in destination image
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::reset:
//
// Reset parameters.
//
void
Contrast::reset() {
    
    m_sliderB -> setValue(0);
    m_sliderC -> setValue(0);
}
