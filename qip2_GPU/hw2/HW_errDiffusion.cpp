void gammaCorrect(ImagePtr, double, ImagePtr);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//
void
HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{
    ImagePtr tempImage;
    IP_copyImageHeader(I1, I2);
    IP_copyImageHeader(I1, tempImage);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, y, x, ch, bufferSize, i, j;
    ChannelPtr<uchar> p1, p2, endd;

    gammaCorrect(I1, gamma, tempImage);

    // if method is Floyd-Steinberg
    if(method == 0) {
        short* in1;
        short* in2;
        int thr = MXGRAY/2;
        int error;
        bufferSize = w+2;
        short* bufferUp   = new short[bufferSize];
        short* bufferDown = new short[bufferSize];

        for(ch = 0; IP_getChannel(tempImage, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);

            // Copy the first row into buffer
            bufferUp[0] = *p1;
            bufferUp[bufferSize-1] = *(p1+w-1);
            for (i = 1; i < bufferSize-1; i++) {
                bufferUp[i] = *p1++;
            }

            // Copy rest of the rows one at a time depending on row number
            for(y = 1; y < h; y++) {
                if (y%2 == 0) {
                    bufferUp[0] = *p1;
                    bufferUp[bufferSize-1] = *(p1+w-1);
                    for (i = 1; i < bufferSize-1; i++) {
                        bufferUp[i] = *p1++;
                    }
                } else {
                    bufferDown[0] = *p1;
                    bufferDown[bufferSize-1] = *(p1+w-1);
                    for (i = 1; i < bufferSize-1; i++) {
                        bufferDown[i] = *p1++;
                    }
                }

                // Check if serpentine is on
                if(serpentine) {
                    // if serpentine is on and it's even row we scan the row from right to left
                    if (y%2 == 0) {
                        // Move output pointer to the last element of the row
                        p2 = p2 + w - 1;
                        in2 = bufferUp   + bufferSize - 2;
                        in1 = bufferDown + bufferSize - 2;
                        for (x = 0; x < w; x++) {
                            *p2 = (*in1 < thr)? 0 : 255;
                            error = *in1 - *p2;
                            *(in1-1)   += (error * 7/16);
                            *(in2+1)   += (error * 3/16);
                            *(in2)     += (error * 5/16);
                            *(in2-1)   += (error * 1/16);
                            in1--;
                            in2--;
                            p2--;
                        }
                        // Move output pointer to the first element of next row
                        p2 = p2 + w + 1;

                    // otherwise we scan the row from left to right
                    } else {
                        in1 = bufferUp   + 1;
                        in2 = bufferDown + 1;
                        for (x = 0; x < w; x++) {
                            *p2 = (*in1 < thr)? 0 : 255;
                            error = *in1 - *p2;
                            *(in1+1)   += (error * 7/16);
                            *(in2-1)   += (error * 3/16);
                            *(in2)     += (error * 5/16);
                            *(in2+1)   += (error * 1/16);
                            in1++;
                            in2++;
                            p2++;
                        }
                    }
                // When serpentine is off
                } else {
                    // depending on row number, we use two pointers to point to each buffer
                    if (y%2 == 0) {
                        in2 = bufferUp   + 1;
                        in1 = bufferDown + 1;
                    } else {
                        in1 = bufferUp   + 1;
                        in2 = bufferDown + 1;
                    }
                    for (x = 0; x < w; x++) {
                        *p2 = (*in1 < thr)? 0 : 255;
                        error = *in1 - *p2;
                        *(in1+1)   += (error * 7/16);
                        *(in2-1)   += (error * 3/16);
                        *(in2)     += (error * 5/16);
                        *(in2+1)   += (error * 1/16);
                        in1++;
                        in2++;
                        p2++;
                    }
                }
            }
        }
        delete [] bufferUp;
        delete [] bufferDown;

    // if method is Jarvis-Judice-Ninke
    } else if(method == 1) {
        bufferSize = w+4;
        short** in = new short*[3];
        int thr = MXGRAY/2;
        int error;
        short** buffer = new short*[3];

        for(int i = 0; i < 3; i++) {
            buffer[i] = new short[bufferSize];
        }

        for(ch = 0; IP_getChannel(tempImage, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            int restLen = bufferSize - 2;

            // Copy first two rows into buffer
            for (i = 0; i < 2; i++) {
                buffer[i][0] = *p1;
                buffer[i][1] = *p1;
                buffer[i][bufferSize-1] = *(p1+w-1);
                buffer[i][bufferSize-2] = *(p1+w-1);
                for (j = 2; j < restLen; j++) {
                    buffer[i][j] = *p1++;
                }
            }

            // depending on row number, we start copying a row into buffer one at a time
            for(y = 2; y < h; y++) {
                if(y == 2) {
                    buffer[2][0] = *p1;
                    buffer[2][1] = *p1;
                    buffer[2][bufferSize-1] = *(p1+w-1);
                    buffer[2][bufferSize-2] = *(p1+w-1);
                    for (i = 2; i < restLen; i++) {
                        buffer[2][i] = *p1++;
                    }
                } else if (y%3 == 0) {
                    buffer[0][0] = *p1;
                    buffer[0][1] = *p1;
                    buffer[0][bufferSize-1] = *(p1+w-1);
                    buffer[0][bufferSize-2] = *(p1+w-1);
                    for (i = 2; i < restLen; i++) {
                        buffer[0][i] = *p1++;
                    }
                } else if(y%3 == 1) {
                    buffer[1][0] = *p1;
                    buffer[1][1] = *p1;
                    buffer[1][bufferSize-1] = *(p1+w-1);
                    buffer[1][bufferSize-2] = *(p1+w-1);
                    for (i = 2; i < restLen; i++) {
                        buffer[1][i] = *p1++;
                    }
                } else {
                    buffer[2][0] = *p1;
                    buffer[2][1] = *p1;
                    buffer[2][bufferSize-1] = *(p1+w-1);
                    buffer[2][bufferSize-2] = *(p1+w-1);
                    for (i = 2; i < restLen; i++) {
                        buffer[2][i] = *p1++;
                    }
                }

                // Check if serpentine is on
                if(serpentine) {
                    // if serpentine is on and it's even row we scan the row from right to left
                    // Even row number
                    if (y%2 == 0) {
                        // Move output pointer to the last element of the row
                        p2 = p2 + w - 1;

                        // depending on row number, we use three pointers to point to each buffer
                        if(y == 2) {
                            in[0] = buffer[0] + bufferSize - 3;
                            in[1] = buffer[1] + bufferSize - 3;
                            in[2] = buffer[2] + bufferSize - 3;
                        } else if (y%3 == 0) {
                            in[0] = buffer[1] + bufferSize - 3;
                            in[1] = buffer[2] + bufferSize - 3;
                            in[2] = buffer[0] + bufferSize - 3;
                        } else if(y%3 == 1) {
                            in[0] = buffer[2] + bufferSize - 3;
                            in[1] = buffer[0] + bufferSize - 3;
                            in[2] = buffer[1] + bufferSize - 3;
                        } else {
                            in[0] = buffer[0] + bufferSize - 3;
                            in[1] = buffer[1] + bufferSize - 3;
                            in[2] = buffer[2] + bufferSize - 3;
                        }
                        for (x = 0; x < w; x++) {
                            *p2 = (*in[0] < thr)? 0 : 255;
                            error = *in[0] - *p2;
                            *(in[0]-1) += (error * 7/48);
                            *(in[0]-2) += (error * 5/48);
                            *(in[1]+1) += (error * 5/48);
                            *(in[1]+2) += (error * 3/48);
                            *(in[1]-1) += (error * 5/48);
                            *(in[1]-2) += (error * 3/48);
                            *(in[2]+1) += (error * 3/48);
                            *(in[2]+2) += (error * 1/48);
                            *(in[2]-1) += (error * 3/48);
                            *(in[2]-2) += (error * 1/48);
                            in[0]--;
                            in[1]--;
                            in[2]--;
                            p2--;
                        }
                        // Move output pointer to the first element of next row
                        p2 = p2 + w + 1;

                    // otherwise we scan the row from left to right
                    // Odd row number
                    } else {
                        // depending on row number, we use three pointers to point to each buffer
                        if(y == 2) {
                            in[0] = buffer[0] + 2;
                            in[1] = buffer[1] + 2;
                            in[2] = buffer[2] + 2;
                        } else if (y%3 == 0) {
                            in[0] = buffer[1] + 2;
                            in[1] = buffer[2] + 2;
                            in[2] = buffer[0] + 2;
                        } else if(y%3 == 1) {
                            in[0] = buffer[2] + 2;
                            in[1] = buffer[0] + 2;
                            in[2] = buffer[1] + 2;
                        } else {
                            in[0] = buffer[0] + 2;
                            in[1] = buffer[1] + 2;
                            in[2] = buffer[2] + 2;
                        }
                        for (x = 0; x < w; x++) {
                            *p2 = (*in[0] < thr)? 0 : 255;
                            error = *in[0] - *p2;
                            *(in[0]+1) += (error * 7/48);
                            *(in[0]+2) += (error * 5/48);
                            *(in[1]+1) += (error * 5/48);
                            *(in[1]+2) += (error * 3/48);
                            *(in[1]-1) += (error * 5/48);
                            *(in[1]-2) += (error * 3/48);
                            *(in[2]+1) += (error * 3/48);
                            *(in[2]+2) += (error * 1/48);
                            *(in[2]-1) += (error * 3/48);
                            *(in[2]-2) += (error * 1/48);
                            in[0]++;
                            in[1]++;
                            in[2]++;
                            p2++;
                        }
                    }
                // When serpentine is off
                } else {
                    // depending on row number, we use three pointers to point to each buffer
                    if(y == 2) {
                        in[0] = buffer[0] + 2;
                        in[1] = buffer[1] + 2;
                        in[2] = buffer[2] + 2;
                    } else if (y%3 == 0) {
                        in[0] = buffer[1] + 2;
                        in[1] = buffer[2] + 2;
                        in[2] = buffer[0] + 2;
                    } else if(y%3 == 1) {
                        in[0] = buffer[2] + 2;
                        in[1] = buffer[0] + 2;
                        in[2] = buffer[1] + 2;
                    } else {
                        in[0] = buffer[0] + 2;
                        in[1] = buffer[1] + 2;
                        in[2] = buffer[2] + 2;
                    }
                    for (x = 0; x < w; x++) {
                        *p2 = (*in[0] < thr)? 0 : 255;
                        error = *in[0] - *p2;
                        *(in[0]+1) += (error * 7/48);
                        *(in[0]+2) += (error * 5/48);
                        *(in[1]+1) += (error * 5/48);
                        *(in[1]+2) += (error * 3/48);
                        *(in[1]-1) += (error * 5/48);
                        *(in[1]-2) += (error * 3/48);
                        *(in[2]+1) += (error * 3/48);
                        *(in[2]+2) += (error * 1/48);
                        *(in[2]-1) += (error * 3/48);
                        *(in[2]-2) += (error * 1/48);
                        in[0]++;
                        in[1]++;
                        in[2]++;
                        p2++;
                    }
                }
            }
        }
        for(int i = 0; i < 3; i++) {
            delete[] buffer[i];
        }
        delete[] buffer;

    // neither of the choices, just copy input into output
    } else {
        for(int ch = 0; IP_getChannel(tempImage, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// gammaCorrect:
//
// Apply gamma correction to image I1.
// Save result in I2.
//
void
gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;

	// init lookup table
	int i, lut[MXGRAY];
	for(i=0; i<MXGRAY; ++i) {
        lut[i] = CLIP(pow((double)i/MaxGray, 1/gamma) * MaxGray, 0, MaxGray);
    }

	// evaluate output: each input pixel indexes into lut[] to eval output
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);
		for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
	}
}
