// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve:
//
// Apply image convolution on I1. Output is in I2.
//

void
HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2) {
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w*h;
    int type, y, x, ch, i, j;

    // Getting kernel information
    int kw = Ikernel->width();
    int kh = Ikernel->height();
    ChannelPtr<float> kernelPtr;
    IP_getChannel(Ikernel, 0, kernelPtr, type);

    int paddingW = kw/2;
    int paddingH = kh/2;
    int bufferSize = w + paddingW*2;
    int filterSize = kw*kh;

    // allocate memory for buffer
    short** buffer = new short*[kh];
    for(i = 0; i < kh; i++) {
        buffer[i] = new short[bufferSize];
    }

    ChannelPtr<uchar> p1, p2, endd, cursor;
    endd = p1 + total;

    // When kernel size is one, copy input to output directly
    if (kw == 1 && kh == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }

    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        // save pointer position pointed to first pixel of image
        cursor = p1;
        
        // copy all top padding buffer
        for(i = 0; i < paddingH; i++) {
            // copy left padding buffer
            for(j = 0; j < paddingW; j++) {
                buffer[i][j] = *cursor;
            }

            // copy buffer between padding
            for(j = paddingW; j < w+paddingW; j++) {
                buffer[i][j] = *cursor++;
            }

            //copy right padding buffer
            cursor--;
            for(j = w+paddingW; j < bufferSize; j++) {
                buffer[i][j] = *cursor;
            }
        }

        // copy rest of buffer
        cursor = p1;    // move cursor back to first pixel
        for(i = paddingH; i < kh; i++) {
            for(j = 0; j < paddingW; j++) {
                buffer[i][j] = *cursor;
            }

            // copy buffer between padding
            for(j = paddingW; j < w+paddingW; j++) {
                buffer[i][j] = *cursor++;
            }

            //copy right padding buffer
            cursor--;
            for(j = w+paddingW; j < bufferSize; j++) {
                buffer[i][j] = *cursor;
            }
            cursor++;
        }

        //visit one row at a time
        for(y = 0; y < h; y++) {

            // doing convolution
            for (x = 0; x < w; x++) {
                float sum = 0;
                for (i = 0; i < kh; i++) {
                    for (j = 0; j < kw; j++) {
                        sum += buffer[i][j+x] * (*kernelPtr++);
                    }
                }

                // assign convoluted value to output
                *p2++ = CLIP(sum, 0, MaxGray);

                // move kernelPtr back to first value
                kernelPtr -= filterSize;
            }

            // move all row buffer one index up
            for (i = 0; i<kh-1; i++) {
                for (j = 0; j<bufferSize; j++) {
                    buffer[i][j] = buffer[i+1][j];
                }
            }

            // copy next row buffer
            // copy left padding buffer
            for(j = 0; j < paddingW; j++) {
                buffer[kh-1][j] = *cursor;
            }

            // copy buffer between padding
            for(j = paddingW; j < w+paddingW; j++) {
                buffer[kh-1][j] = *cursor++;
            }

            //copy right padding buffer
            cursor--;
            for(j = w+paddingW; j < bufferSize; j++) {
                buffer[kh-1][j] = *cursor;
            }
            cursor++;

            if(y >= (h - paddingH - 2)) {
                cursor -= w;
            }
        }
    }
    for (i = 0; i < kh; i++) {
        delete[] buffer[i];
    }
    delete[] buffer;
}
