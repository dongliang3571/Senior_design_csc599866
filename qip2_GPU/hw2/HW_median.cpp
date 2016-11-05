#include <algorithm>
#include <vector>
using std::vector;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size sz x sz to I1.
// Clamp sz to 9.
// Output is in I2.
//
void
HW_median(ImagePtr I1, int sz, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w*h;
    int type, y, x, ch, k, i, j, l;
    short* firstRowBuffer;
    short* lastRowBuffer;
    short* newList;
    short* tempFornewList;
    short** tempForBuffer;
    ChannelPtr<uchar> p1, p2, endd, tempOut;

    if (sz%2 == 0) sz++;

    // When kernel size is one, copy input to output directly
    if (sz == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }

    int neighborSize = sz/2;
    int kernel = sz;
    int filterSize = kernel*kernel;
    int halfFilterSize = filterSize/2;
    int bufferSize = w + neighborSize*2;
    short** buffer = new short*[kernel];
    tempForBuffer = buffer;
    firstRowBuffer = new short[w];
    lastRowBuffer = new short[w];
    newList = new short[filterSize];
    tempFornewList = newList;

    // initializing buffer
    for(k = 0; k < kernel; k++) {
        buffer[k] = new short[bufferSize];
        tempForBuffer[k] = buffer[k];
    }

    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        tempOut = p1;

        //copy first row to firstRowbuffer
        for (k = 0; k < w; k++) {
            firstRowBuffer[k] = p1[k];
        }

        //copy last row to lastRowbuffer
        int lastrow = (h-1)*w;
        for (k = 0; k < w; k++) {
            lastRowBuffer[k] = p1[k+lastrow];
        }

        //visit one row at a time
        for(y = 0; y < h; y++) {

            // Copy row to buffer
            int lastElement = w-1;
            int extraTopRow = neighborSize - y;
            int extraBottomRow = y + neighborSize - (h - 1);
            if (extraTopRow < 0) {
                extraTopRow = 0;
            }
            if (extraBottomRow < 0) {
                extraBottomRow = 0;
            }

            int extraTopIndex = w + neighborSize;

            // create buffer for upper rows out of boundary
            for (i = 0; i < extraTopRow; i++) {
                for (j = 0; j < neighborSize; j++) {
                    buffer[i][j] = firstRowBuffer[0];
                }
                for (k = 0; k < w; k++) {
                    buffer[i][k+neighborSize] = firstRowBuffer[k];
                }
                for (l = 0; l < neighborSize; l++) {
                    buffer[i][l+extraTopIndex] = firstRowBuffer[lastElement];
                }
            }

            // create buffer for all rows higher than middle row but inside boundary
            for (i = 0; i < neighborSize-extraTopRow; i++) {
                for (j = 0; j < neighborSize; j++) {
                    buffer[i+extraTopRow][j] = *(tempOut-(i+1)*w);
                }
                for (k = 0; k < w; k++) {
                    buffer[i+extraTopRow][k+neighborSize] = *(tempOut-(i+1)*w+k);
                }
                for (l = 0; l < neighborSize; l++) {
                    buffer[i+extraTopRow][l+extraTopIndex] = *(tempOut-(i+2)*w-1);
                }
            }

            // create buffer for middle row
            for (i = 0; i < 1; i++) {
                for (j = 0; j < neighborSize; j++) {
                    buffer[i+neighborSize][j] = *(tempOut);
                }
                for (k = 0; k < w; k++) {
                    buffer[i+neighborSize][k+neighborSize] = *(tempOut+k);
                }
                for (l = 0; l < neighborSize; l++) {
                    buffer[i+neighborSize][l+extraTopIndex] = *(tempOut+lastElement);
                }
            }

            // create buffer for all rows lower than middle row but inside boundary
            for (i = 0; i < neighborSize-extraBottomRow; i++) {
                for (j = 0; j < neighborSize; j++) {
                    buffer[i+neighborSize+1][j] = *(tempOut+(i+1)*w);
                }
                for (k = 0; k < w; k++) {
                    buffer[i+neighborSize+1][k+neighborSize] = *(tempOut+(i+1)*w+k);
                }
                for (l = 0; l < neighborSize; l++) {
                    buffer[i+neighborSize+1][l+extraTopIndex] = *(tempOut+(i+2)*w-1);
                }
            }

            // create buffer for lower rows out of boundary
            int extraBottomIndex = kernel - extraBottomRow;
            for (i = 0; i < extraBottomRow; i++) {
                for (j = 0; j < neighborSize; j++) {
                    buffer[i+extraBottomIndex][j] = lastRowBuffer[0];
                }
                for (k = 0; k < w; k++) {
                    buffer[i+extraBottomIndex][k+neighborSize] = lastRowBuffer[k];
                }
                for (l = 0; l < neighborSize; l++) {
                    buffer[i+extraBottomIndex][l+extraTopIndex] = lastRowBuffer[lastElement];
                }
            }
            tempOut += w;
            // Finish first round of copying rows to buffer

            // Collect all pixels inside kernel
            for (x = 0; x < w; x++) {
                for (i = 0; i < kernel; i++) {
                    for (j = 0; j < kernel; j++) {
                        *newList++ = tempForBuffer[i][j+x];
                    }
                }

                // Move newList pointer back to begining so that next look can use it
                newList -= filterSize;

                // Take median
                std::sort(tempFornewList, tempFornewList+filterSize);
                *p2 = tempFornewList[halfFilterSize];
                p2++;
            }
        }
    }

    //take care of memory leakage
    delete[] firstRowBuffer;
    delete[] lastRowBuffer;
    for (k = 0; k < kernel; k++) {
        delete[] buffer[k];
    }
    delete[] buffer;
    delete[] tempFornewList;
}
