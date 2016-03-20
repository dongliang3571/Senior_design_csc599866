#ifndef QUANTIZE_H
#define QUANTIZE_H

#include "ImageFilter.h"

class Quantize: public ImageFilter {
    Q_OBJECT

public:
    Quantize	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters


protected:
    void quantize(ImagePtr I1, int level, bool isChecked, ImagePtr I2);
    
protected slots:
    void changeLevel (int level);
    void changeDither();

private:
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QSlider*        m_sliderLevel;
    QSpinBox*       m_spinboxLevel;
    QCheckBox*      m_checkBox_dither;
    
};

#endif // QUANTIZE_H
