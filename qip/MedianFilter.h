//
//  MedianFilter.h
//  qip
//
//  Created by dong liang on 4/8/16.
//
//

#ifndef MedianFilter_h
#define MedianFilter_h

#include <stdio.h>

#include "ImageFilter.h"

class MedianFilter: public ImageFilter {
    Q_OBJECT
    
public:
    MedianFilter	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters
    
    friend void insertion_sort(short* arr, int length);
    
    
protected:
    void Median(ImagePtr I1, int size, int avg_nbrs, ImagePtr I2);
    
    protected slots:
    
private:
    void            copyRowToBuffer(ChannelPtr<uchar> &imagePtr, int width, int kernel, int strike);
    int             bufferSize;
    short*          buffer;
    
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    
    
};


#endif /* MedianFilter_h */
