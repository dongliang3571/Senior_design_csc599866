//
//  HistogramProcessing.cpp
//  qip
//
//  Created by dong liang on 3/19/16.
//
//

#include "HistogramProcessing.h"
#include "MainWindow.h"


extern MainWindow *g_mainWindowP;




HistogramProcessing::HistogramProcessing(QWidget *parent) : ImageFilter(parent)
{
    
}



bool
HistogramProcessing::applyFilter(ImagePtr I1, ImagePtr I2) {

    if(I1.isNull()) return 0;
    
    int max = m_sliderMax->value();
    int min = m_sliderMin->value();
    
    if (max<1 || max > MaxGray) return 0;
    if (min<0 || min > MaxGray-1) return 0;
    
    if (min == MaxGray || max == 0 || min >= max) return 0;
    
    Stretching(I1, max, min, I2);
    
    return true;
}



QGroupBox*
HistogramProcessing::controlPanel()
{
    m_ctrlGrp = new QGroupBox("HistogramStretching");

    QLabel* label_Max = new QLabel(QString("Max"));
    QLabel* label_Min = new QLabel(QString("Min"));
    
    m_sliderMax = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderMax->setTickPosition(QSlider::TicksBelow);
    m_sliderMax->setTickInterval(50);
    m_sliderMax->setMinimum(1);
    m_sliderMax->setMaximum(MaxGray);
    m_sliderMax->setValue  (MaxGray);
    
    m_sliderMin = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderMin->setTickPosition(QSlider::TicksBelow);
    m_sliderMin->setTickInterval(50);
    m_sliderMin->setMinimum(0);
    m_sliderMin->setMaximum(MaxGray-1);
    m_sliderMin->setValue  (0);
    
    m_spinboxMax = new QSpinBox(m_ctrlGrp);
    m_spinboxMax->setMinimum   (1);
    m_spinboxMax->setMaximum   (MaxGray);
    m_spinboxMax->setValue     (MaxGray);
    m_spinboxMax->setSingleStep(1);
    
    m_spinboxMin = new QSpinBox(m_ctrlGrp);
    m_spinboxMin->setMinimum   (0);
    m_spinboxMin->setMaximum   (MaxGray-1);
    m_spinboxMin->setValue     (0);
    m_spinboxMin->setSingleStep(1);
    
    m_checkBoxAutoMax = new QCheckBox("AutoMax");
    m_checkBoxAutoMax->setCheckState(Qt::Unchecked);

    m_checkBoxAutoMin = new QCheckBox("AutoMin");
    m_checkBoxAutoMin->setCheckState(Qt::Unchecked);
    
    connect(m_sliderMax ,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMax(int)));
    connect(m_spinboxMax,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMax(int)));
    connect(m_checkBoxAutoMax, SIGNAL(stateChanged(int)), this, SLOT(autoStretch(int)));
    
    connect(m_sliderMin ,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMin(int)));
    connect(m_spinboxMin,    SIGNAL(valueChanged(int)), this, SLOT(changeStretchMin(int)));
    connect(m_checkBoxAutoMin, SIGNAL(stateChanged(int)), this, SLOT(autoStretch(int)));
    
    
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label_Max,    0, 0);
    layout->addWidget(m_sliderMax,     0, 1);
    layout->addWidget(m_spinboxMax,    0, 2);
    layout->addWidget(m_checkBoxAutoMax, 0, 3);
    
    layout->addWidget(label_Min,    1, 0);
    layout->addWidget(m_sliderMin,     1, 1);
    layout->addWidget(m_spinboxMin,    1, 2);
    layout->addWidget(m_checkBoxAutoMin, 1, 3);
    
    
    
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;
    
}


// gray = .3R +.59G + .11B
void HistogramProcessing::Stretching(ImagePtr I1, int max, int min, ImagePtr I2) {
    
    
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int tempMax = max;
    int tempMin = min;
    
    // compute lut[]
    int i, lut[MXGRAY];
    int Histogram_Array[MaxGray];
    
    int type;
    ChannelPtr<uchar> p1, p2, endd;
    
    if (m_checkBoxAutoMax->isChecked() || m_checkBoxAutoMin->isChecked()) {
        // Initialize Histogram array with all 0s
        for(i = 0; i < MaxGray; i++) Histogram_Array[i] = 0;
        
        //Count frequency for all pixels from tje input image
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++)
            for(endd = p1 + total; p1<endd; p1++)
                Histogram_Array[*p1] += 1;
    }
    
    if (m_checkBoxAutoMax->isChecked()) {
        //find max
        for(i = MaxGray-1; i >= 0; i--) {
            if (!Histogram_Array[i]);
            else {
                tempMax = i;
                settingSliderAndSpinBox(m_sliderMax, m_spinboxMax, tempMax);
                break;
            }
        }
    }
    
    if (m_checkBoxAutoMin->isChecked()) {
        //find min
        for(i = 0; i < MaxGray; i++) {
            if (!Histogram_Array[i]);
            else {
                tempMin = i;
                settingSliderAndSpinBox(m_sliderMin, m_spinboxMin, tempMin);
                break;
            }
        }
    }
    
    for (i = 0; i < MXGRAY; i++) {
        lut[i] = CLIP(MXGRAY * (i - tempMin) / (tempMax - tempMin), 0, 255);
    }
    
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
}


void HistogramProcessing::settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value) {
    slider->blockSignals(true );
    slider->setValue    (value);
    slider->blockSignals(false);
    
    spinbox->blockSignals(true );
    spinbox->setValue    (value);
    spinbox->blockSignals(false);
}


void HistogramProcessing::changeStretchMax(int value) {
    
    m_checkBoxAutoMax->blockSignals(true);
    m_checkBoxAutoMax->setCheckState(Qt::Unchecked);
    m_checkBoxAutoMax->blockSignals(false);
    
    settingSliderAndSpinBox(m_sliderMax, m_spinboxMax, value);
    if (m_sliderMax->value() <= m_sliderMin->value()) {
        settingSliderAndSpinBox(m_sliderMax, m_spinboxMax, m_sliderMin->value() + 1);
    }
    
    
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    
    // display output
    g_mainWindowP->displayOut();
}

void HistogramProcessing::changeStretchMin(int value) {
    
    m_checkBoxAutoMin->blockSignals(true);
    m_checkBoxAutoMin->setCheckState(Qt::Unchecked);
    m_checkBoxAutoMin->blockSignals(false);
    
    settingSliderAndSpinBox(m_sliderMin, m_spinboxMin, value);
    if (m_sliderMin->value() >= m_sliderMax->value()) {
        settingSliderAndSpinBox(m_sliderMin, m_spinboxMin, m_sliderMax->value() - 1);
    }
    
    
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    
    // display output
    g_mainWindowP->displayOut();
}

void HistogramProcessing::autoStretch(int isAuto) {
    
    
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
    

}


void HistogramProcessing::reset() {
    
}


