//
//  HistogramMatching.h
//  hw
//
//  Created by dong liang on 8/27/16.
//
//

#ifndef HistogramMatching_h
#define HistogramMatching_h

#include "GLWidget.h"


class HistogramMatching : public GLWidget {
    Q_OBJECT
public:
    HistogramMatching  (QWidget *parent = 0);		// constructor
    QGroupBox*  controlPanel();		// create control panel
    void        reset();            // reset parameters
    void        setImage(QImage image);  // set image
    void        reload();
    void        displayHisto();
    
    void        initVertexBuffer(); // initialize vertices
    void        initShaders();      // initialize shaders
    void        initTexture();      // initialize texture
    
    
    public slots:
    void changeThr(int value);      // slot when m_slider changes
    
protected:
    void		initializeGL	();		// init GL state
    void		resizeGL	(int, int);	// resize GL widget
    void		paintGL		();         // render GL scene
    
private:
    int         m_winW;			// window width
    int         m_winH;			// window height
    int         m_numberVertices;   // number of vertices
    bool        m_isInitialized;    // Bool to save state of initialization
    
    
    QGLShaderProgram    m_program;  // GLSL program
    QImage		  m_image;          // texture image
    QImage        m_GLImage;
    GLsizei       m_width_GLImage;
    GLsizei       m_height_GLImage;
    
    GLuint		  m_texture;        // shader index to texture unit
    GLuint		  m_uniform[16];	// uniform var for a shader and <16 vars
    
    // shader variables
    QMatrix4x4    m_u_mvpMatrix;	// View Model Porjection matrix
    GLfloat       m_u_reference;    // HistogramMatching reference value
    
    // GUI variables. widgets
    QSlider		*m_slider ;	// HistogramMatching sliders
    QSpinBox	*m_spinBox;	// HistogramMatching spin boxes
    QLabel		*m_label;	// Label for printing Otsu thresholds
    QGroupBox	*m_ctrlGrp;	// Groupbox for panel
};


#endif /* HistogramMatching_h */
