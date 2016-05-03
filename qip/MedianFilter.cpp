//
//  MedianFilter.cpp
//  qip
//
//  Created by dong liang on 4/8/16.
//
//

#include "MedianFilter.h"
#include "MainWindow.h"


extern MainWindow *g_mainWindowP;


void insertion_sort(short* arr, int length){
    int j, temp;
    
    for (int i = 0; i < length; i++){
        j = i;
        
        while (j > 0 && arr[j] < arr[j-1]){
            temp = arr[j];
            arr[j] = arr[j-1];
            arr[j-1] = temp;
            j--;
        }
    }
}


MedianFilter::MedianFilter(QWidget *parent) : ImageFilter(parent)
{
    
}



bool
MedianFilter::applyFilter(ImagePtr I1, ImagePtr I2) {
    
    if(I1.isNull()) return 0;
    
    int filterSize = m_spinBoxFilterSize->value();
    int AvgK = m_sliderAvgK->value();
    
    Median(I1, filterSize, AvgK, I2);
    
    return true;
}



QGroupBox*
MedianFilter::controlPanel()
{
    m_ctrlGrp = new QGroupBox("Median Filter");
    
    QLabel* labelFilterSize = new QLabel(QString("Filter Size"));
    QLabel* labelAvgK = new QLabel(QString("Avg K"));
    
    m_sliderFilterSize = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderFilterSize->setTickPosition(QSlider::TicksBelow);
    m_sliderFilterSize->setTickInterval(5);
    m_sliderFilterSize->setMinimum(1);
    m_sliderFilterSize->setMaximum(99);
    m_sliderFilterSize->setValue  (1);
    
    m_sliderAvgK = new QSlider(Qt::Horizontal, m_ctrlGrp);
    m_sliderAvgK->setTickPosition(QSlider::TicksBelow);
    m_sliderAvgK->setTickInterval(5);
    m_sliderAvgK->setMinimum(0);
    m_sliderAvgK->setMaximum(49);
    m_sliderAvgK->setValue  (0);
    
    m_spinBoxFilterSize = new QSpinBox(m_ctrlGrp);
    m_spinBoxFilterSize->setMinimum   (1);
    m_spinBoxFilterSize->setMaximum   (99);
    m_spinBoxFilterSize->setValue     (1);
    m_spinBoxFilterSize->setSingleStep(2);
    
    m_spinBoxAvgK = new QSpinBox(m_ctrlGrp);
    m_spinBoxAvgK->setMinimum   (0);
    m_spinBoxAvgK->setMaximum   (49);
    m_spinBoxAvgK->setValue     (0);
    m_spinBoxAvgK->setSingleStep(1);
    
    connect(m_sliderFilterSize,    SIGNAL(valueChanged(int)), this, SLOT(changeFilterSize(int)));
    connect(m_spinBoxFilterSize,   SIGNAL(valueChanged(int)), this, SLOT(changeFilterSize(int)));
    connect(m_sliderAvgK,    SIGNAL(valueChanged(int)), this, SLOT(changeAvgK(int)));
    connect(m_spinBoxAvgK,   SIGNAL(valueChanged(int)), this, SLOT(changeAvgK(int)));
    
    QGridLayout *layout = new QGridLayout;
    
    layout->addWidget(labelFilterSize, 0, 0);
    layout->addWidget(m_sliderFilterSize, 0, 1);
    layout->addWidget(m_spinBoxFilterSize, 0, 2);
    
    layout->addWidget(labelAvgK, 1, 0);
    layout->addWidget(m_sliderAvgK, 1, 1);
    layout->addWidget(m_spinBoxAvgK, 1, 2);
    
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;
    
}

void MedianFilter::copyRowsToBuffer(ChannelPtr<uchar> &Ptr, short* firstRow, short* lastRow, int row, int width, int height, int kernel) {
    int i, j, k, l;
    int lastElement = width-1;
    int neighborSize = kernel/2;
    int extraTopRow = neighborSize - row;
    int extraBottomRow = row + neighborSize - (height - 1);
    if (extraTopRow < 0) {
        extraTopRow = 0;
    }
    if (extraBottomRow < 0) {
        extraBottomRow = 0;
    }
    
    int extraTopIndex = width + neighborSize;
    // create buffer for rows out of boundary
    for (i = 0; i < extraTopRow; i++) {
        for (j = 0; j < neighborSize; j++) {
            buffer[i][j] = firstRow[0];
        }
        for (k = 0; k < width; k++) {
            buffer[i][k+neighborSize] = firstRow[k];
        }
        for (l = 0; l < neighborSize; l++) {
            buffer[i][l+extraTopIndex] = firstRow[lastElement];
        }
    }
    
    for (i = 0; i < neighborSize-extraTopRow; i++) {
        for (j = 0; j < neighborSize; j++) {
            buffer[i+extraTopRow][j] = *(Ptr-(i+1)*width);
        }
        for (k = 0; k < width; k++) {
            buffer[i+extraTopRow][k+neighborSize] = *(Ptr-(i+1)*width+k);
        }
        for (l = 0; l < neighborSize; l++) {
            buffer[i+extraTopRow][l+extraTopIndex] = *(Ptr-(i+2)*width-1);
        }
    }
    
    for (i = 0; i < 1; i++) {
        for (j = 0; j < neighborSize; j++) {
            buffer[i+neighborSize][j] = *(Ptr);
        }
        for (k = 0; k < width; k++) {
            buffer[i+neighborSize][k+neighborSize] = *(Ptr+k);
        }
        for (l = 0; l < neighborSize; l++) {
            buffer[i+neighborSize][l+extraTopIndex] = *(Ptr+lastElement);
        }
    }
    
    for (i = 0; i < neighborSize-extraBottomRow; i++) {
        for (j = 0; j < neighborSize; j++) {
            buffer[i+neighborSize+1][j] = *(Ptr+(i+1)*width);
        }
        for (k = 0; k < width; k++) {
            buffer[i+neighborSize+1][k+neighborSize] = *(Ptr+(i+1)*width+k);
        }
        for (l = 0; l < neighborSize; l++) {
            buffer[i+neighborSize+1][l+extraTopIndex] = *(Ptr+(i+2)*width-1);
        }
    }
    
    int extraBottomIndex = kernel - extraBottomRow;
    for (i = 0; i < extraBottomRow; i++) {
        for (j = 0; j < neighborSize; j++) {
            buffer[i+extraBottomIndex][j] = lastRow[0];
        }
        for (k = 0; k < width; k++) {
            buffer[i+extraBottomIndex][k+neighborSize] = lastRow[k];
        }
        for (l = 0; l < neighborSize; l++) {
            buffer[i+extraBottomIndex][l+extraTopIndex] = lastRow[lastElement];
        }
    }
}

bool myfunction (int i,int j) { return (i<j); }

void MedianFilter::Median(ImagePtr I1, int size, int avg_nbrs, ImagePtr I2) {
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w*h;
    int type, y, x, ch, k, i, j, l, sum=0;
    short* firstRowBuffer;
    short* lastRowBuffer;
    short* newList;
    short* tempFornewList;
    short** tempForBuffer;
    ChannelPtr<uchar> p1, p2, endd, tempOut;
    

    if (size == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }
    
    int neighborSize = size/2;
    int kernel = size;
    int filterSize = kernel*kernel;
    int halfFilterSize = filterSize/2;
    int avgKLength = avg_nbrs*2+1;
    bufferSize = w + neighborSize*2;
    buffer = new short*[kernel];
    tempForBuffer = buffer;
    firstRowBuffer = new short[w];
    lastRowBuffer = new short[w];
    newList = new short[filterSize];
    tempFornewList = newList;
    for(k = 0; k < kernel; k++) {
        buffer[k] = new short[bufferSize];
        tempForBuffer[k] = buffer[k];
    }

    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        tempOut = p1;
        
        //copy first row to buffer
        for (k = 0; k < w; k++) {
            firstRowBuffer[k] = p1[k];
        }
        
        //copy first row to buffer
        int lastrow = (h-1)*w;
        for (k = 0; k < w; k++) {
            lastRowBuffer[k] = p1[k+lastrow];
        }
        
        //visit one row at a time
        for(y = 0; y < h; y++) {
            copyRowsToBuffer(tempOut, firstRowBuffer, lastRowBuffer, y, w, h, kernel); // copy rows to buffer
            tempOut += w;

            for (x = 0; x < w; x++) {
                for (i = 0; i < kernel; i++) {
                    for (j = 0; j < kernel; j++) {
                        *newList++ = tempForBuffer[i][j+x];
                    }
                }
                newList -= filterSize;
                insertion_sort(tempFornewList, filterSize);
                if (avg_nbrs > 0) {
                    for (l = 1; l <= avg_nbrs; l++) {
                        sum += tempFornewList[halfFilterSize-l] + tempFornewList[halfFilterSize+l];
                    }
                    sum += tempFornewList[halfFilterSize];
                    *p2 = sum/(avgKLength);
                    sum = 0;
                } else {
                    *p2 = tempFornewList[halfFilterSize];
                }
                p2++;
            }
        }
        
    }
    delete[] firstRowBuffer;
    delete[] lastRowBuffer;
    for (k = 0; k < kernel; k++) {
        delete[] buffer[k];
    }
    delete[] buffer;
}

void MedianFilter::changeFilterSize(int value) {
    value += !(value%2);
    settingSliderAndSpinBox(m_sliderFilterSize, m_spinBoxFilterSize, value, true);
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
    
}

void MedianFilter::changeAvgK(int value) {
    settingSliderAndSpinBox(m_sliderAvgK, m_spinBoxAvgK, value, false);
    applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());
    g_mainWindowP->displayOut();
}

void MedianFilter::settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value, bool oddOnly) {
    if (oddOnly) {
        value += !(value%2);
    }
    slider->blockSignals(true );
    slider->setValue    (value);
    slider->blockSignals(false);
    
    spinbox->blockSignals(true );
    spinbox->setValue    (value);
    spinbox->blockSignals(false);
}

void MedianFilter::reset() {
    
}

