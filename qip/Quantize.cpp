

#include "Quantize.h"
#include "MainWindow.h"


extern MainWindow *g_mainWindowP;




Quantize::Quantize(QWidget *parent) : ImageFilter(parent)
{

}



bool
Quantize::applyFilter(ImagePtr I1, ImagePtr I2) {
    
    if(I1.isNull()) return 0;
    
    int level;
    bool isChecked;
    isChecked = m_checkBox_dither->isChecked();
    level = m_sliderLevel->value();
    
    quantize(I1, level, isChecked, I2);
    return true;
}



QGroupBox*
Quantize::controlPanel()
{
    m_ctrlGrp = new QGroupBox("Quantization");
    
    QLabel* label_quantize = new QLabel(QString("Quantize"));

    m_sliderLevel = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderLevel->setTickPosition(QSlider::TicksBelow);
    m_sliderLevel->setTickInterval(50);
    m_sliderLevel->setMinimum(1);
    m_sliderLevel->setMaximum(256);
    m_sliderLevel->setValue  (1);
    
    //spinbox for brightness
    m_spinboxLevel = new QSpinBox(m_ctrlGrp);
    m_spinboxLevel->setMinimum   (1);
    m_spinboxLevel->setMaximum   (256);
    m_spinboxLevel->setValue     (1);
    m_spinboxLevel->setSingleStep(1);
    
    m_checkBox_dither = new QCheckBox("Dither");
    m_checkBox_dither->setCheckState(Qt::Unchecked);
    
    
    connect(m_sliderLevel ,    SIGNAL(valueChanged(int)), this, SLOT(changeLevel(int)));
    connect(m_spinboxLevel,    SIGNAL(valueChanged(int)), this, SLOT(changeLevel(int)));
    connect(m_checkBox_dither, SIGNAL(stateChanged(int)), this, SLOT(changeDither(int)));
    
    
    
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label_quantize,    0, 0);
    layout->addWidget(m_sliderLevel,     0, 1);
    layout->addWidget(m_spinboxLevel,    0, 2);
    layout->addWidget(m_checkBox_dither, 1, 0);


    
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;

}


// gray = .3R +.59G + .11B
void Quantize::quantize(ImagePtr I1, int level, bool isChecked, ImagePtr I2) {
    
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    
    // compute lut[]
    int scale = MXGRAY/level;
    
    
    int i, lut[MXGRAY];
    for(i=0; i<MXGRAY; ++i)
        lut[i] = CLIP(scale * (int)(i/scale) + scale/2, 0, 255);
    
    
//    if (isChecked) {
//        qDebug() <<"inside dithering";
//        for (i=0; i<MXGRAY; ++i) {
//            //create noise
//            int noise = (char)(((rand()/(float)0x7fff)-0.5)*255);
//
//            if (i%2 != 0) {
//                qDebug() <<"I'm before addition"<<lut[i];
//                lut[i] = CLIP(lut[i] + noise, 0, 255);
//                qDebug() <<"I'm in after addition"<<lut[i];
//            }
//            else {
//                lut[i] = CLIP(lut[i] + noise, 0, 255);
//            }
//        }
//        
//    }
    
    int type;
    ChannelPtr<uchar> p1, p2, endd;

    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
    
    if (isChecked) {

        int bias = ((MXGRAY / level) * 0.5);
        
        int point = 0;
        int randomized_bias;
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) {
                
                randomized_bias = rand() % bias;
                
//                qDebug() <<randomized_bias;
                if (point%2 == 0) {
                    *p2 = *p1 + randomized_bias;
                    p2++;
                    p1++;
                    point += 1;
                }
                else {
                    *p2 = *p1 - randomized_bias;
                    p2++;
                    p1++;
                    point += 1;
                }
            }
        }
    }
    
    
    
    
    


//    if (isChecked) {
//        int error = 0;
//        int nextValue = 0;
//        
//        for(i=0; i<128 && i<MXGRAY; ++i) lut[i] = 0;
//        for(   ; i <= MaxGray;      ++i) lut[i] = MaxGray;
//        
//        
//        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
//            IP_getChannel(I2, ch, p2, type);
//            for(endd = p1 + total; p1<endd;) {
//                
//                nextValue = CLIP(*p1++ + error, 0, 255);
//                qDebug() <<"nextblaue is "<<nextValue;
//                error = nextValue - lut[nextValue];
//                qDebug() <<"error is "<<error;
//                *p2++ = lut[nextValue];
//                qDebug() <<"lut[nextVlue] is "<<nextValue;
//                
//                
//                
//            }
//        }
//    }

}



void Quantize::changeLevel(int level) {
    
    m_sliderLevel ->blockSignals(true );
    m_sliderLevel ->setValue    (level);
    m_sliderLevel ->blockSignals(false);
    
    m_spinboxLevel->blockSignals(true );
    m_spinboxLevel->setValue    (level);
    m_spinboxLevel->blockSignals(false);
    
    
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    
    // display output
    g_mainWindowP->displayOut();
}

void Quantize::changeDither(int flag) {
    
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
    
}


void Quantize::reset() {

}

