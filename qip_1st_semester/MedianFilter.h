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
#include <ctime>
#include "ImageFilter.h"
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>

using namespace std;

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
    void MedianHistogramBase(ImagePtr I1, int size, int avg_nbrs, ImagePtr I2);
    
    protected slots:
        void changeFilterSize(int value);
        void changeAvgK(int value);
        void applyHistoBased(int state);
    
private:
    void            copyRowsToBuffer(ChannelPtr<uchar> &Ptr, short* firstRow, short* lastRow, int row, int width, int height, int kernel);
    void            settingSliderAndSpinBox(QSlider* slider, QSpinBox* spinbox, int value, bool oddOnly);
    int             bufferSize;
    short**         buffer;
    vector<string>  m_logs;
    
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderFilterSize;
    QSlider*        m_sliderAvgK;
    QSpinBox*       m_spinBoxFilterSize;
    QSpinBox*       m_spinBoxAvgK;
    QCheckBox*      m_checkBoxHistoBase;
    QLabel*         m_labelLogging;
    QScrollArea*    m_scrollArea;
    
    
    
};


#endif /* MedianFilter_h */
