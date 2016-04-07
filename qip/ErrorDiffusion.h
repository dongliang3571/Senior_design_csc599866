//
//  ErrorDiffusion.h
//  qip
//
//  Created by dong liang on 4/3/16.
//
//

#ifndef ErrorDiffusion_h
#define ErrorDiffusion_h

#include <stdio.h>
#include "ImageFilter.h"

class ErrorDiffusion: public ImageFilter {
    Q_OBJECT
    
public:
    ErrorDiffusion	(QWidget *parent = 0);		// constructor
    QGroupBox*	controlPanel	();		// create control panel
    bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
    void		reset		();		// reset parameters
    
    
    
    
protected:
    //    void MatchingEq(ImagePtr I1, ImagePtr I2);
    void ErrorDiffuse(ImagePtr I1, bool isChecked, ImagePtr I2);
    
    protected slots:
    void startDiffuse(int);
    
    
private:
    
    void            copyRowToBuffer(ChannelPtr<uchar> &p1, int width, int row);
    
    int             bufferSize;
    short*          bufferUp;
    short*          bufferDown;
    
    QGroupBox*      m_ctrlGrp;	// groupbox for panel
    QCheckBox*      m_checkbox;
    
    
    
    
    
};


#endif /* ErrorDiffusion_h */
