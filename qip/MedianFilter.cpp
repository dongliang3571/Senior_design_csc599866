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
    
    Median(I1, 1, 0, I2);
    
    return true;
}



QGroupBox*
MedianFilter::controlPanel()
{
    m_ctrlGrp = new QGroupBox("Median Filter");
    
    
    QGridLayout *layout = new QGridLayout;
    
    
    m_ctrlGrp->setLayout(layout);
    
    return m_ctrlGrp;
    
}

void MedianFilter::copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int size, int strike) {
    
    int i;
    int neighborSize = size;
    
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
        
    }
}


// gray = .3R +.59G + .11B
void MedianFilter::Median(ImagePtr I1, int size, int avg_nbrs, ImagePtr I2) {
    ImagePtr tempImage;
    IP_copyImageHeader(I1, I2);
    IP_copyImageHeader(I1, tempImage);
    
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, y, x, ch, k;
    
    ChannelPtr<uchar> p1, p2, endd, tempOut;
    
    if (size == 0) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }
    
    // blurring the rows
    short* in;
    int XneighborSize = size;
    int kernel = XneighborSize*2 + 1;
    short shorterList[kernel];
    short sortedList[kernel];
    bufferSize = w + XneighborSize*2;
    buffer = new short[bufferSize];
    
    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(tempImage, ch, tempOut, type);
        for(y = 0; y < h; y++) {
            copyRowToBuffer(p1, w, XneighborSize, 1);
            in = buffer;
            
            for (x = XneighborSize; x < w+XneighborSize; x++) {

                for (k = 0; k <= XneighborSize; k++) {
                    shorterList[k] = in[x-k];
                }
                for (k = 1+XneighborSize; k <= XneighborSize+XneighborSize; k++) {
                    shorterList[k] = in[x+k];
                }
                for (k = 0; k < kernel; k++) {
                    sortedList[k] = shorterList[k];
                }
                insertion_sort(sortedList, kernel);
                *tempOut = sortedList[XneighborSize];
                tempOut++;
            }
        }
    }
    
    //blurring the columns
    delete[] buffer;
    bufferSize = h + XneighborSize*2;
    buffer = new short[bufferSize];
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        
        for(x = 0; x < w; x++) {
            copyRowToBuffer(tempOut, h, XneighborSize, w);
            in = buffer;
            
            int count = 0;
            for (y = XneighborSize; y < h+XneighborSize; y++) {
                for (k = 0; k <= XneighborSize; k++) {
                    shorterList[k] = in[y-k];
                }
                for (k = 1+XneighborSize; k < XneighborSize+XneighborSize+1; k++) {
                    shorterList[k] = in[y+k];
                }
                for (k = 0; k < kernel; k++) {
                    sortedList[k] = shorterList[k];
                }
                insertion_sort(sortedList, kernel);
                p2[count*w] = sortedList[XneighborSize];
                count++;
            }
            p2++;
            tempOut++;
        }
    }
}




void MedianFilter::reset() {
    
}

