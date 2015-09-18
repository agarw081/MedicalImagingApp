//
//  MedImageGUI.h
//  PaintShop
//
//  Created by CSci3081 Guru on 4/6/15.
//  Copyright (c) 2015 CSci3081 Guru. All rights reserved.
//
//  The MedImage GUI class.  This class sets up the GUI and the display pixelBuffer.
//
//  For educational purposes only.  Please do not post online.


#ifndef MEDIMAGEGUI_H
#define MEDIMAGEGUI_H

#include "BaseGfxApp.h"

#include "Threshold.h"
#include "Saturation.h"
#include "AdjustRGB.h"
#include "Quantize.h"
#include "DetectEdges.h"
#include "Blur.h"
#include "MotionBlur.h"
#include "Sharpen.h"
#include "SpecialFilter.h"
#include "TStamp.h"

class ColorData;
class PixelBuffer;
class Tool;
class Filter;

class MedImageGUI : public BaseGfxApp {
public:
    
    MedImageGUI(int argc, char* argv[], int width, int height, ColorData backgroundColor);
    virtual ~MedImageGUI();
    
    // Glut overrided function
    void mouseDragged(int x, int y) ;
    void mouseMoved(int x, int y);
    void leftMouseDown(int x, int y);
    void leftMouseUp(int x, int y);
    void display();
    void gluiControl(int controlID);

    PixelBuffer* FilterSharpen(PixelBuffer* input, int sharpen_amount);
    PixelBuffer* FilterEdgeDetect(PixelBuffer* input);
    PixelBuffer* FilterThreshold(PixelBuffer* input, float threshold_amount);
    PixelBuffer* FilterMultiplyRGB(PixelBuffer* input, float multiply_colorRed, float multiply_colorGreen, float multiply_colorBlue);
    PixelBuffer* FilterSaturate(PixelBuffer* input, int saturation_amount);
    PixelBuffer* FilterQuantize(PixelBuffer* input, int bins);
    PixelBuffer* FilterBlur(PixelBuffer* input, int blur_amount);
    PixelBuffer* applyStamp(int x, int y, PixelBuffer* inputBuffer, PixelBuffer* stamp);

    void setImageFile(const std::string & filepath);
    bool isValidImageFileName(const std::string & name);
    bool isValidImageFile(const std::string & name);
    bool hasSuffix(const std::string & str, const std::string & suffix);
    std::string getImageNamePlusSeqOffset(const std::string & currentFileName, int offset);
    
private:
    
    // GLUI INTERFACE ELEMENTS
    enum UIControlType {
        UI_TOOLTYPE,
        UI_FILE_BROWSER,
        UI_LOAD_CANVAS_BUTTON,
        UI_SAVE_CANVAS_BUTTON,
        UI_NEXT_IMAGE_BUTTON,
        UI_PREVIOUS_IMAGE_BUTTON,
        UI_FILE_NAME,
        UI_APPLY_SHARP,
        UI_APPLY_EDGE,
        UI_APPLY_THRESHOLD,
        UI_APPLY_GRAYSCALE,
        UI_APPLY_MULTIPLY_RGB,
        UI_APPLY_QUANTIZE,
        UI_UNDO,
        UI_REDO,
        UI_QUIT
    };
    
    void buttonEnabled(GLUI_Button * button, bool enabled);
    void undoEnabled(bool enabled);
    void redoEnabled(bool enabled);
    void saveCanvasEnabled(bool enabled);
    void loadCanvasEnabled(bool enabled);
    void previousImageEnabled(bool enabled);
    void nextImageEnabled(bool enabled);
    void addToUndoStack();
    void addToRedoStack();
    void initializeFilters();
    
    
    void loadImageToCanvas();
    void saveCanvasToFile();
    void loadNextImage();
    void loadPreviousImage();
    
    void applyFilterSharpen();
    void applyFilterEdgeDetect();
    void applyFilterThreshold();
    void applyFilterMultiplyRGB();
    void applyFilterGrayScale();
    void applyFilterQuantize();

    
    
    void undoOperation();
    void redoOperation();
    
    
    // PRIVATE MEMBER TASKS
    // PaintShop specific functions
    void initGlui();
    void initializeBuffers(ColorData initialColor, int width, int height);

    
    struct {
        float multiply_colorRed;
        float multiply_colorGreen;
        float multiply_colorBlue;
        float threshold_amount;
        float sharpen_amount;
        int quantize_bins;
    } m_filterParameters;
    
    struct {
        GLUI_FileBrowser* fileBrowser;
        GLUI_Button *loadCanvasButton;
        GLUI_Button *saveCanvasButton;
        GLUI_Button* nextImageButton;
        GLUI_Button* previousImageButton;
        GLUI_Button *redoButton;
        GLUI_Button *undoButton;
        
        GLUI_StaticText * currentFileLabel;
        GLUI_EditText* fileNameBox;
        GLUI_StaticText * saveFileLabel;

        
        
        
    } m_gluiControlHooks;
    
    
    //  MEMBER VARAIBLES
    
    // This is the pointer to the buffer where the display PixelBuffer is stored
    PixelBuffer* m_displayBuffer;
    PixelBuffer* stamp;



    //Filters
    Threshold* threshold;
    Saturation* saturation;
    AdjustRGB* adjustRGB;
    Quantize* quantize;
    DetectEdges* detectEdges;
    Blur* blur;
    MotionBlur* motionBlur;
    Sharpen* sharpen;
    SpecialFilter* specialFilter;
    
    // These are used to store the selections from the GLUI user interface
    int m_curTool;
    Tool **m_tools;
    std::string m_fileName;
    std::string m_nextFileName;
    std::string m_prevFileName;

    int m_mouseLastX, m_mouseLastY;

    TStamp tStamp;

    float m_curColorRed, m_curColorGreen, m_curColorBlue;


    std::vector<PixelBuffer*> stack;
    std::vector<PixelBuffer*> redostack;

};


#endif
