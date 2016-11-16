// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// ObjectMatch.cpp - ObjectMatch widget.
//
// Written by: George Wolberg, 2016
// ======================================================================

#include "MainWindow.h"
#include "ObjectMatch.h"
#include "hw2/HW_objectMatch.cpp"

extern MainWindow *g_mainWindowP;
enum { WSIZE, HSIZE, STEPX, STEPY, KERNEL, SAMPLER };
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectMatch::ObjectMatch:
//
// Constructor.
//
ObjectMatch::ObjectMatch(QWidget *parent) : ImageFilter(parent)
{
    m_kernel = NULL;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectMatch::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
ObjectMatch::controlPanel()
{
    // init group box
    m_ctrlGrp = new QGroupBox("ObjectMatch");
    
    // layout for assembling filter widget
    QVBoxLayout *vbox = new QVBoxLayout;
    
    // create file pushbutton
    m_button = new QPushButton("File");
    m_label = new QLabel("x: , y: , corr: ");
    m_label_template = new QLabel;
    
    // assemble dialog
    vbox->addWidget(m_button);
    vbox->addWidget(m_label);
    vbox->addWidget(m_label_template);
    m_ctrlGrp->setLayout(vbox);
    
    // init signal/slot connections
    connect(m_button, SIGNAL(clicked()), this, SLOT(load()));
    
    return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectMatch::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
ObjectMatch::applyFilter(ImagePtr I1, bool gpuFlag, ImagePtr I2)
{
    // error checking
    if(I1.isNull())		return 0;
    if(m_kernel.isNull())	return 0;
    m_width  = I1->width();
    m_height = I1->height();
    // ObjectMatch image
    if(!(gpuFlag && m_shaderFlag)) objMatch(I1, m_kernel, I2);
    else    g_mainWindowP->glw()->applyFilterGPU(m_nPasses);
    
    return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectMatch::ObjectMatch:
//
// ObjectMatch image I1 with convolution filter in kernel.
// Output is in I2.
//
void
ObjectMatch::objMatch(ImagePtr I1, ImagePtr kernel, ImagePtr I2)
{
    char buf[100];
    float *corr;
    corr = HW_objectMatch(I1, kernel, I2);
    sprintf(buf, "x: %.2f, y: %.2f, corr: %.5f", corr[0], corr[1], corr[2]);
    m_label->setText(QString(buf));
    delete[] corr;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectMatch::load:
//
// Slot to load filter kernel from file.
//
int
ObjectMatch::load()
{
    QFileDialog dialog(this);
    
    // open the last known working directory
    if(!m_currentDir.isEmpty())
        dialog.setDirectory(m_currentDir);
    
    // display existing files and directories
    dialog.setFileMode(QFileDialog::ExistingFile);
    
    // invoke native file browser to select file
    m_file =  dialog.getOpenFileName(this,
                                     "Open File", m_currentDir,
                                     "Images (*.AF *.png);;All files (*)");
    
    // verify that file selection was made
    if(m_file.isNull()) return 0;
    
    // save current directory
    QFileInfo f(m_file);
    m_currentDir = f.absolutePath();
    
    // read kernel
    m_kernel = IP_readImage(qPrintable(m_file));
    
//    IP_castImage(m_kernel,  BW_IMAGE, m_kernel);
    // init vars
    int w = m_kernel->width ();
    int h = m_kernel->height();
    
    // update button with filename (without path)
    m_button->setText(f.fileName());
    m_button->update();
    
    QImage q;
    IP_IPtoQImage(m_kernel, q);
    QPixmap p;
    p = QPixmap::fromImage(q);
    m_label_template->setPixmap(p);
    
    // apply filter to source image and display result
    g_mainWindowP->preview();
    
    return 1;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectMatch::initShader:
//
// init shader program and parameters.
//
void
ObjectMatch::initShader()
{
    m_nPasses = 1;
    
    // initialize GL function resolution for current context
    initializeGLFunctions();
    
    UniformMap uniforms;
    
    // init uniform hash table based on uniform variable names and location IDs
    uniforms["u_SizeW"   ] = WSIZE;
    uniforms["u_SizeH"   ] = HSIZE;
    uniforms["u_StepX"  ] = STEPX;
    uniforms["u_StepY"  ] = STEPY;
    uniforms["u_Kernel" ] = KERNEL;
    uniforms["u_Sampler"] = SAMPLER;
    
    QString v_name = ":/vshader_passthrough";
    QString f_name = ":/hw2/fshader_objectMatch";
    
#ifdef __APPLE__
    v_name += "_Mac";
    f_name += "_Mac";
#endif
    
    // compile shader, bind attribute vars, link shader, and initialize uniform var table
    g_mainWindowP->glw()->initShader(m_program[PASS1],
                                     v_name + ".glsl",
                                     f_name + ".glsl",
                                     uniforms,
                                     m_uniform[PASS1]);
    
    m_shaderFlag = true;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ObjectMatch::gpuProgram:
//
// Active gpu program
//
void
ObjectMatch::gpuProgram(int pass)
{
    int w   = m_kernel->width();
    int h   = m_kernel->height();
    int kernelSize = w*h;
    
    int type;
    float* kernel = new float[kernelSize];
    ChannelPtr<uchar> kernelPtr;
    IP_getChannel(m_kernel, 0, kernelPtr, type);
    
    for(int i = 0; i < kernelSize; i++) {
        kernel[i] = (*kernelPtr / 255.0);
        kernelPtr++;
    }
    
    glUseProgram(m_program[pass].programId());
    glUniform1i (m_uniform[pass][WSIZE],  w);
    glUniform1i (m_uniform[pass][HSIZE],  h);
    glUniform1f (m_uniform[pass][STEPX], (GLfloat) 1.0f / m_width);
    glUniform1f (m_uniform[pass][STEPY], (GLfloat) 1.0f / m_height);
    glUniform1fv(m_uniform[pass][KERNEL], kernelSize, kernel);
    glUniform1i (m_uniform[pass][SAMPLER], 0);
}
