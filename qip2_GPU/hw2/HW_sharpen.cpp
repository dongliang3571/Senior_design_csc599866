extern void HW_blur  (ImagePtr, int, int, ImagePtr);
extern void HW_blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//
void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
    ImagePtr tempImage;
    IP_copyImageHeader(I1, tempImage);
    IP_copyImageHeader(I1, I2);


    int w = I1->width();
    int h = I1->height();
    int total = w * h;
    int type, ch;
    ChannelPtr<uchar> p1, p2, endd, tempOut;

    if (size%2 == 0) size++;
    
    if (size == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
        return;
    }

    HW_blur(I1, size, size, tempImage);
    for(ch = 0; IP_getChannel(tempImage, ch, tempOut, type); ch++) {
        IP_getChannel(I1, ch, p1, type);
        IP_getChannel(I2, ch, p2, type);
        for(endd = tempOut + total; tempOut<endd; p2++, p1++, tempOut++) {
            *p2 = CLIP(*p1 + factor*(*p1-*tempOut), 0, 255);
        }
    }
}
