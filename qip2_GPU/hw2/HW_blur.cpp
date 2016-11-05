void blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
    //create a temporary image and copy header from input image

    ImagePtr tempImage;
    IP_copyImageHeader(I1, I2);
    IP_copyImageHeader(I1, tempImage);

    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, ch;
    ChannelPtr<uchar> p1, p2, endd, tempOut;

    if (filterW%2 == 0) filterW++;
    if (filterH%2 == 0) filterH++;

    if (filterW == 1 && filterH == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }

    // blurring the rows
    for(ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(tempImage, ch, tempOut, type);
        blur1D(p1, w, 1, filterW, tempOut);
    }

    //blurring the columns
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        blur1D(tempOut, h, w, filterH, p2);
    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// blur1D:
//
// Blur src with 1D box filter of width ww.
//
void
blur1D(ChannelPtr<uchar> src, int len, int stride, int ww, ChannelPtr<uchar> dst)
{
    int i, x, y, k, count, sum;
    short* in;
    int neighborSize = ww/2;
    int bufferSize = len + ww - 1;
    short* buffer = new short[bufferSize];

    // Loop through each row
    for(count = 0; count < len; count++) {
        sum = 0;

        // when stride is one
        if (stride == 1) {

            //create left padding buffer
            for (i = 0; i < neighborSize; i++ ) {
                buffer[i] = *src;
            }

            //create right padding buffer
            for (i = neighborSize+len; i < bufferSize; i++) {
                buffer[i] = *(src+len-1);
            }

            //create buffer between left and right padding
            for (i = neighborSize; i < len+neighborSize; i++) {
                buffer[i] = *src++;
            }

            in = buffer;
            //added up first ww pixel in buffer
            for (k = 0; k < ww; k++) {
                sum += in[k];
            }

            for (x = neighborSize; x < len+neighborSize; x++) {
                *dst = sum/ww;
                sum += (in[x+neighborSize+1] - in[x-neighborSize]);
                dst++;
            }

        //When stride is width of the image
        } else {
            //create top padding buffer
            for (i = 0; i < neighborSize; i++ ) {
                buffer[i] = *src;
            }

            //create bottom padding buffer
            int k = *(src+(len-1)*stride);
            for (i = neighborSize+len; i < bufferSize; i++) {
                buffer[i] = k;
            }

            //create buffer between top and bottom padding
            ChannelPtr<uchar> p = src;
            for (i = neighborSize; i < len+neighborSize; i++ ) {
                buffer[i] = *p;
                p += stride;
            }

            //added up first ysz pixel in buffer
            for (k = 0; k < ww; k++) {
                sum += in[k];
            }

            int count = 0;
            for (y = neighborSize; y < len+neighborSize; y++) {
                dst[count*len] = sum/ww;
                sum += (in[y+neighborSize+1] - in[y-neighborSize]);
                count++;
            }
            dst++;
            src++;
        }
    }
    delete[] buffer;
}
