//
//  HistogramMatching.cpp
//  qip
//
//  Created by dong liang on 3/19/16.
//
//

#include "HistogramMatching.h"
#include "MainWindow.h"
#include <cmath>

extern MainWindow *g_mainWindowP;




HistogramMatching::HistogramMatching(QWidget *parent) : ImageFilter(parent)
{

}



bool
HistogramMatching::applyFilter(ImagePtr I1, ImagePtr I2) {

    if(I1.isNull()) return 0;

    int value = m_slider->value();

    Matching(I1, value, I2);

    return true;
}



QGroupBox*
HistogramMatching::controlPanel()
{
    m_ctrlGrp = new QGroupBox("HistogramMatching");

    QLabel* label = new QLabel(QString("Exponent"));


    m_slider = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_slider->setTickInterval(50);
    m_slider->setMinimum(-100);
    m_slider->setMaximum(100);
    m_slider->setValue  (0);

    m_spinbox = new QSpinBox(m_ctrlGrp);
    m_spinbox->setMinimum   (-100);
    m_spinbox->setMaximum   (100);
    m_spinbox->setValue     (0);
    m_spinbox->setSingleStep(1);

    connect(m_slider , SIGNAL(valueChanged(int)), this, SLOT(generateHistogram(int)));
    connect(m_spinbox, SIGNAL(valueChanged(int)), this, SLOT(generateHistogram(int)));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(label,    0, 0);
    layout->addWidget(m_slider,     0, 1);
    layout->addWidget(m_spinbox,    0, 2);


    m_ctrlGrp->setLayout(layout);

    return m_ctrlGrp;

}



void HistogramMatching::Matching(ImagePtr I1, int value, ImagePtr I2) {

    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;

    int left[MXGRAY], right[MXGRAY], histo[MXGRAY], target[MXGRAY], lim[MXGRAY], index[MXGRAY];;
    double Histogram_ref[MXGRAY];
    double Havg = 0, scale;
    int i, p, R = 0, Hsum = 0;

    int type;
    ChannelPtr<uchar> p1, p2, endd;

    for(i=0; i<MXGRAY; i++) {
        histo[i] = 0; /* clear histogram */
        lim[i] = 0;
        index[i] = 0;
    }

    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++)
        for(endd = p1 + total; p1<endd; p1++)
            histo[*p1]++;

    for (i = 0; i < MXGRAY; i++) {

        //check slider is positive or negative
        if (value >= 0) {
            Histogram_ref[i] = pow(i/(double)MaxGray, value);
        } else {
            Histogram_ref[i] = 1 - pow(i/(double)MaxGray, value*(-1));
        }
        Havg += Histogram_ref[i];
    }

    /* normalize h2 to conform with dimensions of I1 */
    scale = (double) total / Havg;
    if(scale != 1)
        for(i=0; i<MXGRAY; i++)
            Histogram_ref[i] *= scale;

    for (i=0; i<MXGRAY; i++) {
        target[i] = (int)Histogram_ref[i];
    }

    for(i=0; i<MXGRAY; i++) {
        left[i] = R; /* left end of interval */
        lim[i] = target[R] - Hsum;
        Hsum += histo[i];

        while(Hsum > target[R] && R < MXGRAY - 1) {
            Hsum -= target[R];
            R++;
        }
        if(left[i] != R) {
            index[i] = left[i];
        }
        right[i] = R;
    }

    for(i=0; i<MXGRAY; i++) histo[i] = 0;

    /* visit all input pixels */
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd; p1++, p2++) {
            p = left[*p1];
            if(histo[p] < target[p]) {
                if(left[*p1] != right[*p1]) {
                    if(left[*p1] == index[*p1]) {
                        if(lim[*p1] > 0) {
                            *p2 = p;
                            lim[*p1] -= 1;
                        } else {
                            *p2 = p = left[*p1] = MIN(p+1, right[*p1]);
                        }
                    } else {
                        *p2 = p;
                    }
                } else {
                    *p2 = p;
                }
            } else {
                *p2 = p = left[*p1] = MIN(p+1, right[*p1]);
            }
            histo[p]++;
        }
    }
}

void HistogramMatching::generateHistogram(int value) {

    m_slider -> blockSignals(true );
    m_slider -> setValue    (value);
    m_slider -> blockSignals(false);

    m_spinbox -> blockSignals(true );
    m_spinbox -> setValue    (value);
    m_spinbox -> blockSignals(false);


    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

    // display output
    g_mainWindowP->displayOut();
}


void HistogramMatching::reset() {

}
