// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
// Written by: Dong Liang, 2016
//
void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;

    int scale = MXGRAY/levels;
    int i, lut[MXGRAY];
    for(i=0; i<MXGRAY; ++i)
        lut[i] = CLIP(scale * (int)(i/scale) + scale/2, 0, MaxGray);

    int type;
	ChannelPtr<uchar> p1, p2, endd;
    if (!dither) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
        }
    } else {
        int bias = scale * 0.5;
        int noise, sign;
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(int x = 0; x < h; x++) {
                // if it is odd row, sign equals negative else it's positive
                sign = (x%2) ? -1 : 1;
                for(int y = 0; y < w; y++, p1++) {
                    noise = (double)rand() / RAND_MAX * bias;
                    *p2++ = lut[CLIP(*p1+noise*sign, 0, MaxGray)];
                    sign *= -1;
                }
            }
        }
    }
}
