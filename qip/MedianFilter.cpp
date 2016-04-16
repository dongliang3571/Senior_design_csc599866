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
    
    Median(I1, 3, 0, I2);
    
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

//void MedianFilter::copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int size, int strike) {
//    
//    int i;
//    int neighborSize = size;
//    
//    if (strike == 1) {
//        for (i = 0; i < neighborSize; i++ ) {
//            buffer[i] = *imagePtr;
//        }
//        
//        for (i = neighborSize+width; i < bufferSize; i++) {
//            buffer[i] = *(imagePtr+width-1);
//        }
//        
//        for (i = neighborSize; i < width+neighborSize; i++) {
//            buffer[i] = *imagePtr++;
//        }
//    } else {
//        for (i = 0; i < neighborSize; i++ ) {
//            buffer[i] = *imagePtr;
//        }
//        
//        for (i = neighborSize+width; i < bufferSize; i++) {
//            buffer[i] = *(imagePtr+(width-1)*strike);
//        }
//        int count = 0;
//        for (i = neighborSize; i < width+neighborSize; i++ ) {
//            buffer[i] = *(imagePtr+strike*count);
//            count++;
//        }
//        
//    }
//}

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
    
//    printf("top row are %d\n", extraTopRow);
    for (i = 0; i < extraTopRow; i++) {
        for (j = 0; j < neighborSize; j++) {
            buffer[i][j] = firstRow[0];
        }
        for (k = 0; k < width; k++) {
            buffer[i][k+neighborSize] = firstRow[k];
        }
        for (l = 0; l < neighborSize; l++) {
            buffer[i][l+width+neighborSize] = firstRow[lastElement];
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
            buffer[i+extraTopRow][l+width+neighborSize] = *(Ptr-(i+1)*width+width-1);
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
            buffer[i+neighborSize][l+width+neighborSize] = *(Ptr+width-1);
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
            buffer[i+neighborSize+1][l+width+neighborSize] = *(Ptr+(i+1)*width+width-1);
        }
    }

    for (i = 0; i < extraBottomRow; i++) {
        for (j = 0; j < neighborSize; j++) {
            buffer[i+kernel-extraBottomRow][j] = lastRow[0];
        }
        for (k = 0; k < width; k++) {
            buffer[i+kernel-extraBottomRow][k+neighborSize] = lastRow[k];
        }
        for (l = 0; l < neighborSize; l++) {
            buffer[i+kernel-extraBottomRow][l+width+neighborSize] = lastRow[lastElement];
        }
    }
}

bool myfunction (int i,int j) { return (i<j); }

void MedianFilter::Median(ImagePtr I1, int size, int avg_nbrs, ImagePtr I2) {
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w*h;
    int type, y, x, ch, k, i, j;
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
        
        
        for(y = 0; y < h; y++) {


            copyRowsToBuffer(tempOut, firstRowBuffer, lastRowBuffer, y, w, h, kernel);
            
            tempOut += w;

            for (x = 0; x < w; x++) {
                for (i = 0; i < kernel; i++) {
                    for (j = 0; j < kernel; j++) {
                        *newList++ = tempForBuffer[i][j+x];
                    }
                }
                newList -= filterSize;
                insertion_sort(tempFornewList, filterSize);
                *p2 = tempFornewList[filterSize/2];
                p2++;
            }
        }
        
    }

    printf("im here\n");
    delete[] firstRowBuffer;
    delete[] lastRowBuffer;
    for (k = 0; k < kernel; k++) {
        delete[] buffer[k];
    }
    delete[] buffer;

}
// gray = .3R +.59G + .11B
//void MedianFilter::Median(ImagePtr I1, int size, int avg_nbrs, ImagePtr I2) {
//    ImagePtr tempImage;
//    IP_copyImageHeader(I1, I2);
//    IP_copyImageHeader(I1, tempImage);
//    
//    int w = I1->width();
//    int h = I1->height();
//    int total = w * h;
//    int type, y, x, ch, k;
//    
//    ChannelPtr<uchar> p1, p2, endd, tempOut;
//    
//    if (size == 0) {
//        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
//            IP_getChannel(I2, ch, p2, type);
//            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
//        }
//        return;
//    }
//    
//    short* in;
//    int neighborSize = size;
//    int kernel = neighborSize*2 + 1;
//    short shorterList[kernel];
//    short sortedList[kernel];
//    bufferSize = w + neighborSize*2;
//    buffer = new short[bufferSize];
//    
//    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
//        IP_getChannel(tempImage, ch, tempOut, type);
//        for(y = 0; y < h; y++) {
//            copyRowToBuffer(p1, w, neighborSize, 1);
//            in = buffer;
//            
//            for (x = neighborSize; x < w+neighborSize; x++) {
//
//                for (k = 0; k <= neighborSize; k++) {
//                    shorterList[k] = in[x-k];
//                }
//                for (k = 1+neighborSize; k <= neighborSize+neighborSize; k++) {
//                    shorterList[k] = in[x+k];
//                }
//                for (k = 0; k < kernel; k++) {
//                    sortedList[k] = shorterList[k];
//                }
//                insertion_sort(sortedList, kernel);
//                *tempOut = sortedList[neighborSize];
//                tempOut++;
//            }
//        }
//    }
//    
//    delete[] buffer;
//    bufferSize = h + neighborSize*2;
//    buffer = new short[bufferSize];
//    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
//        IP_getChannel(I2, ch, p2, type);
//        
//        for(x = 0; x < w; x++) {
//            copyRowToBuffer(tempOut, h, neighborSize, w);
//            in = buffer;
//            
//            int count = 0;
//            for (y = neighborSize; y < h+neighborSize; y++) {
//                for (k = 0; k <= neighborSize; k++) {
//                    shorterList[k] = in[y-k];
//                }
//                for (k = 1+neighborSize; k < neighborSize+neighborSize+1; k++) {
//                    shorterList[k] = in[y+k];
//                }
//                for (k = 0; k < kernel; k++) {
//                    sortedList[k] = shorterList[k];
//                }
//                insertion_sort(sortedList, kernel);
//                p2[count*w] = sortedList[neighborSize];
//                count++;
//            }
//            p2++;
//            tempOut++;
//        }
//    }
//    
//    delete[] buffer;
//}




void MedianFilter::reset() {
    
}

