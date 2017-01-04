// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_contrast:
//
// Apply contrast enhancement to I1. Output is in I2.
// Stretch intensity difference from reference value (128) by multiplying
// difference by "contrast" and adding it back to 128. Shift result by
// adding "brightness" value.
//
// Written by: Dong Liang, 2016
//

double calculateContr(double contrast) {
    double contr;
    if (contrast >= 0) {
        contr = contrast / 25.0 + 1.0;
    }
    else {
        contr = contrast / 133.0 + 1.0;
    }
    return contr;
}

void
HW_contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;
    double contr = calculateContr(contrast);

	// init lookup table
    int reference = 128;
    int i, lut[MXGRAY];
    for(i=0; i<MXGRAY; ++i) {
        lut[i] = (int)CLIP((i - reference)*contr + reference + brightness, 0, MaxGray);
    }

	// evaluate output: each input pixel indexes into lut[] to eval output
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);
		for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
	}
}
