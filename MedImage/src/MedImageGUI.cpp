#include "MedImageGUI.h"
#include "ColorData.h"
#include "PixelBuffer.h"
#include <sstream>

#include "ColorData.h"
#include "PixelBuffer.h"
#include "ToolFactory.h"
#include "Tool.h"
#include "Filter.h"
#include "FilterFactory.h"
#include "Kernel.h"
#include "TBlur.h"
#include "TStamp.h"
#include "ImageHandler.h"

#include <png.h>
#include <cstring>
#include <string>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>

#include <cmath>

using std::cout;
using std::endl;

ColorData bg;
int stack_count = -1;
int limit;
int flagg;

MedImageGUI::MedImageGUI(int argc, char* argv[], int width, int height, ColorData backgroundColor) : BaseGfxApp(argc, argv, width, height, 50, 50, GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH, true, width+51, 50)
{
    // Set the name of the window
    setCaption("MedImage");
    
    // Initialize Interface
    initializeBuffers(backgroundColor, width, height);

    m_tools = new Tool* [1];
    m_tools[0] = ToolFactory::createTool(0);
    
    stamp = (ImageHandler::loadImage("marker.png"));
    TStamp* tStamp = new TStamp();
    initializeFilters();


    initGlui();
    initGraphics();
}

void MedImageGUI::initializeFilters()
{
    Filter* quantize = FilterFactory::createFilter("Quantize");
    Filter* threshold = FilterFactory::createFilter("Threshold");
    Filter* saturation = FilterFactory::createFilter("Saturation");
    Filter* adjustRGB = FilterFactory::createFilter("AdjustRGB");
    Filter* detectEdges = FilterFactory::createFilter("DetectEdges");
    Filter* blur = FilterFactory::createFilter("Blur");
    Filter* sharpen = FilterFactory::createFilter("Sharpen");

}

void MedImageGUI::display()
{
    drawPixels(0, 0, m_width, m_height, m_displayBuffer->getData());
}

MedImageGUI::~MedImageGUI()
{
    if (m_displayBuffer) {
        delete m_displayBuffer;
    }
}

PixelBuffer* MedImageGUI::applyStamp(int x, int y, PixelBuffer* inputBuffer, PixelBuffer* inputStamp)
{
    TStamp* tStamp = new TStamp();
    float height = inputBuffer->getHeight();
    tStamp->applyStamp(x, height - y, inputBuffer, inputStamp, ColorData(1, 1, 1));
    return inputBuffer;
}

void MedImageGUI::mouseDragged(int x, int y)
{
       int max_steps = 30;

    // We implimented a smoothing feature by interpolating between
    // mouse events. This is at the expense of processing, though,
    // because we just "stamp" the tool many times between the two
    // even locations. you can reduce max_steps until it runs
    // smoothly on your machine.
    
    // Get the differences between the events
    // in each direction
    int delta_x = x-m_mouseLastX;
    int delta_y = y-m_mouseLastY;
    
    // Calculate the min number of steps necesary to fill
    // completely between the two event locations.
    float pixelsBetween = fmax(abs(delta_x), abs(delta_y));
    int step_count = pixelsBetween;
    int step_size = 1;
    
    // Optimize by maxing out at the max_steps,
    // and fill evenly between
    if (pixelsBetween > max_steps) 
    {
        step_size = pixelsBetween/max_steps;
        step_count = max_steps;
    }
    
    // Iterate between the event locations
    for (int i = 0; i < pixelsBetween; i+=step_size) 
    {
        int x = m_mouseLastX+(i*delta_x/pixelsBetween);
        int y = m_mouseLastY+(i*delta_y/pixelsBetween);
        if (m_curTool == 1)
        {
            cout<<"Do stamp here"<<endl;
        }
        else
        {
          m_tools[m_curTool]->applyToBuffer(x, m_height-y, ColorData(1, 0, 0), m_displayBuffer);
        }
    }
    
    // let the previous point catch up with the current.
    m_mouseLastX = x;
    m_mouseLastY = y;
}

void MedImageGUI::addToUndoStack()
{
    redoEnabled(false);
    PixelBuffer* f1 = new PixelBuffer(m_width, m_height, bg);
    m_displayBuffer->copyPixelBuffer(m_displayBuffer, f1);

    stack_count++;
    limit=stack_count;
    stack.insert(stack.begin()+stack_count, f1);
    
    if(stack.empty()) undoEnabled(false); else undoEnabled(true); 
    cout<<"Entry made\n";
       
}

void MedImageGUI::mouseMoved(int x, int y)
{
    
}

void MedImageGUI::leftMouseDown(int x, int y)
{
    addToUndoStack();

    if (m_curTool == 1)
    {
        m_displayBuffer = applyStamp(x,y, m_displayBuffer, stamp);
    }
    else
    {
       m_tools[m_curTool]->applyToBuffer(x, m_height-y, ColorData(1, 0, 0), m_displayBuffer);
    }

    m_mouseLastX = x;
    m_mouseLastY = y;
}
void MedImageGUI::addToRedoStack()
{
    PixelBuffer* f1 = new PixelBuffer(m_width, m_height, bg);
    m_displayBuffer->copyPixelBuffer(m_displayBuffer, f1);
    
    redostack.insert(redostack.begin(), f1);
}
void MedImageGUI::leftMouseUp(int x, int y)
{
    addToRedoStack();
}

void MedImageGUI::initializeBuffers(ColorData backgroundColor, int width, int height) {
    m_displayBuffer = new PixelBuffer(width, height, backgroundColor);
}

void MedImageGUI::initGlui()
{
    // Select first tool (this activates the first radio button in glui)
    m_curTool = 0;
    
    
    
    new GLUI_Column(m_glui, false);
    GLUI_Panel *toolPanel = new GLUI_Panel(m_glui, "Tool Type");
    {
        GLUI_RadioGroup *radio = new GLUI_RadioGroup(toolPanel, &m_curTool, UI_TOOLTYPE, s_gluicallback);
        // Create interface buttons for different tools:
        new GLUI_RadioButton(radio, "Pen");
        new GLUI_RadioButton(radio, "Arrow");
        
    }
    
    
    GLUI_Panel *filterPanel = new GLUI_Panel(m_glui, "Filters");
    {
        GLUI_Panel *sharpenPanel = new GLUI_Panel(filterPanel, "Sharpen");
        {
            GLUI_Spinner * filterSharpAmount = new GLUI_Spinner(sharpenPanel, "Amount:", &m_filterParameters.sharpen_amount);
            filterSharpAmount->set_int_limits(1, 10);
            filterSharpAmount->set_int_val(1);
            filterSharpAmount->set_speed(0.1);
            
            new GLUI_Button(sharpenPanel, "Apply", UI_APPLY_SHARP, s_gluicallback);
        }
        GLUI_Panel *edgeDetPanel = new GLUI_Panel(filterPanel, "Edge Detect");
        
        {
            new GLUI_Button(edgeDetPanel, "Apply", UI_APPLY_EDGE, s_gluicallback);
        }
        GLUI_Panel *thresPanel = new GLUI_Panel(filterPanel, "Threshold");
        {
            GLUI_Spinner * filterThresholdAmount = new GLUI_Spinner(thresPanel, "Level:", &m_filterParameters.threshold_amount);
            filterThresholdAmount->set_float_limits(0, 1);
            filterThresholdAmount->set_float_val(0.5);
            
            new GLUI_Button(thresPanel, "Apply", UI_APPLY_THRESHOLD, s_gluicallback);
        }
        
        GLUI_Panel *multiplyPanel = new GLUI_Panel(filterPanel, "Multiply RGB");
        {
            GLUI_Spinner * filterChannelRed = new GLUI_Spinner(multiplyPanel, "R:", &m_filterParameters.multiply_colorRed);
            GLUI_Spinner * filterChannelGreen = new GLUI_Spinner(multiplyPanel, "G:", &m_filterParameters.multiply_colorGreen);
            GLUI_Spinner * filterChannelBlue = new GLUI_Spinner(multiplyPanel, "B:", &m_filterParameters.multiply_colorBlue);
            
            filterChannelRed->set_float_limits(0, 10);
            filterChannelRed->set_float_val(1);
            filterChannelGreen->set_float_limits(0, 10);
            filterChannelGreen->set_float_val(1);
            filterChannelBlue->set_float_limits(0, 10);
            filterChannelBlue->set_float_val(1);
            
            new GLUI_Button(multiplyPanel, "Apply", UI_APPLY_MULTIPLY_RGB, s_gluicallback);
        }
        
        GLUI_Panel *quantPanel = new GLUI_Panel(filterPanel, "Quantize");
        {
            GLUI_Spinner * filterQuantizeBins = new GLUI_Spinner(quantPanel, "Bins:", &m_filterParameters.quantize_bins);
            filterQuantizeBins->set_int_limits(2, 256);
            filterQuantizeBins->set_int_val(8);
            filterQuantizeBins->set_speed(0.1);
            
            new GLUI_Button(quantPanel, "Apply", UI_APPLY_QUANTIZE, s_gluicallback);
        }
        
        GLUI_Panel *grayPanel = new GLUI_Panel(filterPanel, "Grayscale");
        {
            
            new GLUI_Button(grayPanel, "Apply", UI_APPLY_GRAYSCALE, s_gluicallback);
        }


        
        // UNDO,REDO,QUIT
        {
            m_gluiControlHooks.undoButton = new GLUI_Button(m_glui, "Undo", UI_UNDO, s_gluicallback);
            undoEnabled(false);
            m_gluiControlHooks.redoButton  = new GLUI_Button(m_glui, "Redo", UI_REDO, s_gluicallback);
            redoEnabled(false);
            
            new GLUI_Separator(m_glui);
            new GLUI_Button(m_glui, "Quit", UI_QUIT, (GLUI_Update_CB)exit);
        }
    }
    
    new GLUI_Column(m_glui, true);

    GLUI_Panel *imagePanel = new GLUI_Panel(m_glui, "Image I/O");
    {
        m_gluiControlHooks.fileBrowser = new GLUI_FileBrowser(imagePanel, "Choose Image", false, UI_FILE_BROWSER, s_gluicallback);
        
        m_gluiControlHooks.fileBrowser->set_h(400);
        
        m_gluiControlHooks.fileNameBox = new     GLUI_EditText( imagePanel , "Image:", m_fileName, UI_FILE_NAME, s_gluicallback );
        m_gluiControlHooks.fileNameBox->set_w(200);

        new GLUI_Separator(imagePanel);
        
        m_gluiControlHooks.currentFileLabel = new GLUI_StaticText(imagePanel, "Will load image: none");
        m_gluiControlHooks.loadCanvasButton = new GLUI_Button(imagePanel, "Load Canvas", UI_LOAD_CANVAS_BUTTON, s_gluicallback);
        
        new GLUI_Separator(imagePanel);

        m_gluiControlHooks.saveFileLabel = new GLUI_StaticText(imagePanel, "Will save image: none");
        
        m_gluiControlHooks.saveCanvasButton = new GLUI_Button(imagePanel, "Save Canvas", UI_SAVE_CANVAS_BUTTON, s_gluicallback);
        
        
        m_gluiControlHooks.previousImageButton = new GLUI_Button(imagePanel, "Previous Image", UI_PREVIOUS_IMAGE_BUTTON, s_gluicallback);
        m_gluiControlHooks.nextImageButton = new GLUI_Button(imagePanel, "Next Image", UI_NEXT_IMAGE_BUTTON, s_gluicallback);
        
        
        loadCanvasEnabled(false);
        saveCanvasEnabled(false);
        nextImageEnabled(false);
        previousImageEnabled(false);
    }
    return;
}

void MedImageGUI::gluiControl(int controlID)
{
    
    switch (controlID) {
        case UI_APPLY_SHARP:
            applyFilterSharpen();
            break;
        case UI_APPLY_EDGE:
            applyFilterEdgeDetect();
            break;
        case UI_APPLY_THRESHOLD:
            applyFilterThreshold();
            break;
        case UI_APPLY_GRAYSCALE:
            applyFilterGrayScale();
            break;
        case UI_APPLY_MULTIPLY_RGB:
            applyFilterMultiplyRGB();
            break;
        case UI_APPLY_QUANTIZE:
            applyFilterQuantize();
            break;
        case UI_FILE_BROWSER:
            setImageFile(m_gluiControlHooks.fileBrowser->get_file());
            break;
        case UI_LOAD_CANVAS_BUTTON:
            loadImageToCanvas();
            break;
        case UI_SAVE_CANVAS_BUTTON:
            saveCanvasToFile();
            // Reload the current directory:
            m_gluiControlHooks.fileBrowser->fbreaddir(".");
            break;
        case UI_NEXT_IMAGE_BUTTON:
            loadNextImage();
            break;
        case UI_PREVIOUS_IMAGE_BUTTON:
            loadPreviousImage();
            break;
        case UI_FILE_NAME:
            setImageFile(m_fileName);
            break;
        case UI_UNDO:
            undoOperation();
            break;
        case UI_REDO:
            redoOperation();
            break;
        default:
            break;
    }
    
    // Forces canvas to update changes made in this function
    m_glui->post_update_main_gfx();
}

// **********************
// *** GLUI CALLBACKS ***

// Edit these functions to provide instructions
// for how MedImageGUI should respond to the
// button presses.

void MedImageGUI::loadImageToCanvas()
{
    cout << "Load Canvas has been clicked for file " << m_fileName << endl;
    // TODO: Perform loading task
    if (m_displayBuffer) { delete m_displayBuffer; }
    m_displayBuffer = ImageHandler::loadImage(m_fileName);
    setWindowDimensions(m_displayBuffer->getWidth(), m_displayBuffer->getHeight());
    
    // Determining whether there are next or previous images
    m_nextFileName = getImageNamePlusSeqOffset(m_fileName, 1);
    m_prevFileName = getImageNamePlusSeqOffset(m_fileName, -1);
    
    nextImageEnabled(isValidImageFile(m_nextFileName));
    previousImageEnabled(isValidImageFile(m_prevFileName));
}

void MedImageGUI::saveCanvasToFile()
{
    if (ImageHandler::saveImage(m_fileName, m_displayBuffer)) {
        std::cout << "successfully saved image" << std::endl;
    } else {
        std::cout << "unsuccessfully saved image..." << std::endl;
    }
    cout << "Save Canvas been clicked for file " << m_fileName << endl;
}

void MedImageGUI::applyFilterThreshold()
{
    addToUndoStack();
    threshold->applyFilter(m_displayBuffer, m_filterParameters.threshold_amount);
    addToRedoStack();
    cout << "Apply has been clicked for Threshold has been clicked with amount =" << m_filterParameters.threshold_amount << endl;
}

void MedImageGUI::applyFilterMultiplyRGB()
{
    addToUndoStack();
    adjustRGB->applyFilter(m_displayBuffer, m_filterParameters.multiply_colorRed, m_filterParameters.multiply_colorGreen, m_filterParameters.multiply_colorBlue);
    addToRedoStack();
    cout << "Apply has been clicked for Multiply RGB with red = " << m_filterParameters.multiply_colorRed
    << ", green = " << m_filterParameters.multiply_colorGreen
    << ", blue = " << m_filterParameters.multiply_colorBlue << endl;
}

void MedImageGUI::applyFilterGrayScale()
{
    addToUndoStack();
    saturation->applyFilter(m_displayBuffer, 0);
    addToRedoStack();
    cout << "Apply has been clicked for Grayscale" << endl;
}


void MedImageGUI::applyFilterSharpen()
{
    addToUndoStack();
    sharpen->buildKernelAndApply(m_displayBuffer, m_filterParameters.sharpen_amount);
    addToRedoStack();
    cout << "Apply has been clicked for Sharpen with amount = " << m_filterParameters.sharpen_amount << endl;
}

void MedImageGUI::applyFilterEdgeDetect() {
    addToUndoStack();
    cout<<"Buidling Kernel for edge Detect"<<endl;
    detectEdges->buildKernel(m_displayBuffer);
    addToRedoStack();
    cout << "Apply has been clicked for Edge Detect" << endl;
}

void MedImageGUI::applyFilterQuantize() {
    addToUndoStack();
    quantize->applyFilter(m_displayBuffer, m_filterParameters.quantize_bins);
    addToRedoStack();
    cout << "Apply has been clicked for Quantize with bins = " << m_filterParameters.quantize_bins << endl;
}

//Command line filters
PixelBuffer* MedImageGUI::FilterThreshold(PixelBuffer* input, float threshold_amount)
{
    Threshold* threshold = new Threshold();
    threshold->applyFilter(input, threshold_amount);
    cout << "Apply has been clicked for Threshold has been clicked with amount =" << threshold_amount << endl;
    return input;
}

PixelBuffer* MedImageGUI::FilterMultiplyRGB(PixelBuffer* input, float multiply_colorRed, float multiply_colorGreen, float multiply_colorBlue)
{
    AdjustRGB* adjustRGB = new AdjustRGB();
    adjustRGB->applyFilter(input, multiply_colorRed, multiply_colorGreen, multiply_colorBlue);
    cout << "Apply has been clicked for Multiply RGB with red = " << multiply_colorRed
    << ", green = " << multiply_colorGreen
    << ", blue = " << multiply_colorBlue << endl;
    return input;
}

PixelBuffer* MedImageGUI::FilterSharpen(PixelBuffer* input, int sharpen_amount)
{
    Sharpen* sharpen = new Sharpen(); 
    sharpen->buildKernelAndApply(input, sharpen_amount);
    cout << "Apply has been clicked for Sharpen with amount = " << sharpen_amount << endl;
    return input;
}

PixelBuffer* MedImageGUI::FilterEdgeDetect(PixelBuffer* input) {
    cout<<"Buidling Kernel for edge Detect"<<endl;
    DetectEdges* detectEdges = new DetectEdges(); 
    detectEdges->buildKernel(input);
    cout << "Apply has been clicked for Edge Detect" << endl;
    return input;
}

PixelBuffer* MedImageGUI::FilterQuantize(PixelBuffer* input, int bins) {
    Quantize* quantize = new Quantize();
    quantize->applyFilter(input, bins);
    cout << "Apply has been clicked for Quantize with bins = " << bins << endl;
    return input;
}
PixelBuffer* MedImageGUI::FilterSaturate(PixelBuffer* input, int saturation_amount) {
    Saturation* saturation = new Saturation();
    saturation->applyFilter(input, saturation_amount);
    cout << "Applied for Saturate with amount = " << saturation_amount << endl;
    return input;
}
PixelBuffer* MedImageGUI::FilterBlur(PixelBuffer* input, int blur_amount)
{
    Blur* blur = new Blur();
    blur->buildKernelAndApply(input, blur_amount);
    return input;
}

void MedImageGUI::undoOperation()
{
    redoEnabled(true);
    PixelBuffer* f1;
    f1 = new PixelBuffer(m_width, m_height, bg);
    f1 = stack[stack_count];
    setWindowDimensions(f1->getWidth(),f1->getHeight());
    m_displayBuffer = new PixelBuffer(f1->getWidth(),f1->getHeight(),bg); 
    for (int x = 0; x < m_width; x ++)
    {
        for (int y = 0; y < m_height; y++)
        {
            ColorData currentColor = f1->getPixel(x,y);
            m_displayBuffer->setPixel(x,y,currentColor);
        }
    } 
    stack_count--;
    if (stack_count < 0) undoEnabled(false);
    cout << "Undoing..." << endl;
    flagg = 0 ;
}

void MedImageGUI::redoOperation()
{
    static int length = redostack.size();
    static int keep =0;
    PixelBuffer* f1;
    f1 = new PixelBuffer(m_width, m_height, bg);
    if (flagg == 0) stack_count = stack_count+2;
    else stack_count++; 
    if (limit >= (stack_count))
    { 
        f1 = stack[stack_count];}
    else {
        stack_count--;
        f1 = redostack[0];
        redoEnabled(false);
        //f1 = redostack.back();
        }
    setWindowDimensions(f1->getWidth(),f1->getHeight());
    m_displayBuffer = new PixelBuffer(f1->getWidth(),f1->getHeight(),bg);    
    for (int x = 0; x < m_width; x ++)
    {
        for (int y = 0; y < m_height; y++)
        {
            ColorData currentColor = f1->getPixel(x,y);
            m_displayBuffer->setPixel(x,y,currentColor);
        }
    }          
    //redostack.pop_back();
    //if(redostack.empty()) redoEnabled(false); else redoEnabled(true);
    cout << "Redoing..." << endl;
    flagg = 1;
}
// ** END OF CALLBACKS **
// **********************


// **********************
// Provided code for managing the
// GLUI interface.

void MedImageGUI::buttonEnabled(GLUI_Button * button, bool enabled) {
    if(enabled) button->enable();
    else button->disable();
    button->redraw();
}

void MedImageGUI::redoEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.redoButton, enabled);
}

void MedImageGUI::undoEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.undoButton, enabled);
}

void MedImageGUI::saveCanvasEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.saveCanvasButton, enabled);
}

void MedImageGUI::loadCanvasEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.loadCanvasButton, enabled);
}

void MedImageGUI::previousImageEnabled(bool enabled) {
    buttonEnabled(m_gluiControlHooks.previousImageButton, enabled);
}

void MedImageGUI::nextImageEnabled(bool enabled) {
    buttonEnabled(m_gluiControlHooks.nextImageButton, enabled);
}

void MedImageGUI::loadNextImage()
{
    setImageFile(m_nextFileName);
    loadImageToCanvas();
}

void MedImageGUI::loadPreviousImage()
{
    setImageFile(m_prevFileName);
    loadImageToCanvas();
}


bool MedImageGUI::hasSuffix(const std::string & str, const std::string & suffix){
    return str.find(suffix,str.length()-suffix.length()) != std::string::npos;
}

bool MedImageGUI::isValidImageFileName(const std::string & name) {
    
    if (hasSuffix(name, ".png")
        || hasSuffix(name, ".jpg")
        || hasSuffix(name, ".jpeg")
        )
        return true;
    else
        return false;
}

bool MedImageGUI::isValidImageFile(const std::string & name) {
    
    FILE *f;
    bool isValid = false;
    if (isValidImageFileName(name)) {
        if ((f = fopen( name.c_str(), "r"))) {
            isValid = true;
            fclose(f);
        }
    }
    
    
    return isValid;
}

std::string MedImageGUI::getImageNamePlusSeqOffset(const std::string & currentFileName, int offset)
{
    
    int digitCount = 3;
    
    std::string extension, name, number;
    size_t dotPos = currentFileName.find_last_of(".");
    if (dotPos ==  std::string::npos || dotPos == 0) {
        return "";
    }
    
    extension = currentFileName.substr(dotPos+1);
    name = currentFileName.substr(0,dotPos);
    if (name.length() < digitCount) {
        return "";
    }
    
    number = name.substr(name.length()-digitCount);
    name = name.substr(0,name.length()-digitCount);
    int num;
    std::istringstream ( number ) >> num;
    int output_num = num +offset;
    if (output_num < 0) {
        return "";
    }
    std::stringstream ss;
    ss << output_num;
    std::string output_number;
    ss >> output_number;
    
    // Append zero chars
    size_t str_length = output_number.length();
    for (int i = 0; i < digitCount - str_length; i++)
        output_number = "0" + output_number;

    
    

    
    return (name + output_number + "." + extension);
}

void MedImageGUI::setImageFile(const std::string & fileName)
{
    // If a directory was selected
    // instead of a file, use the
    // latest file typed or selected.
    std::string imageFile = fileName;
    if (!isValidImageFileName(imageFile)) {
        imageFile = m_fileName;
    }
    
    
    // TOGGLE SAVE FEATURE
    // If no file is selected or typed,
    // don't allow file to be saved. If
    // there is a file name, then allow
    // file to be saved to that name.
    
    if (!isValidImageFileName(imageFile)) {
        m_gluiControlHooks.saveFileLabel->set_text("Will save image: none");
        saveCanvasEnabled(false);
    } else {
        m_gluiControlHooks.saveFileLabel->set_text((std::string("Will save image: ") + imageFile).c_str());
        saveCanvasEnabled(true);
    }
    
    // TOGGLE LOAD FEATURE
    
    // If the file specified cannot be opened,
    // then disable stamp and canvas loading.
    if (isValidImageFile(imageFile)) {
        loadCanvasEnabled(true);
        
        m_gluiControlHooks.currentFileLabel->set_text((std::string("Will load: ") + imageFile).c_str());
        m_gluiControlHooks.fileNameBox->set_text(imageFile);
    } else {
        loadCanvasEnabled(false);
        m_gluiControlHooks.currentFileLabel->set_text("Will load: none");
    }
}
