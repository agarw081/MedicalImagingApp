
#include "PhotoShop.h"
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

#define PNG_CHECK 4
#define ALPHA_YES 1
#define ALPHA_NO 0


ColorData bg;
int stack_count = -1;
int limit;
int flagg;

PhotoShop::PhotoShop(int argc, char* argv[], int width, int height, ColorData backgroundColor) : BaseGfxApp(argc, argv, width, height, 50, 50, GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH, true, width+51, 50)
{
	// Set the name of the window
	setCaption("PhotoShop");
	
	// Initialize Interface
	initializeBuffers(backgroundColor, width, height);
	
	// Create array of tools and populate
	m_tools = new Tool* [ToolFactory::getNumTools()];
	for (int i = 0; i < ToolFactory::getNumTools(); i++) {
		m_tools[i] = ToolFactory::createTool(i);
	}
	
	initGlui();
	initGraphics();
    initializeFilters();
}

void PhotoShop::display()
{
	drawPixels(0, 0, m_width, m_height, m_displayBuffer->getData());
}


PhotoShop::~PhotoShop()
{
	if (m_displayBuffer)
	{
		delete m_displayBuffer;
	}
	
	// Delete each of the tools before deleting the list of tool pointers.
	if (m_tools) {
		Tool ** toolsEnd =  m_tools + ToolFactory::getNumTools();
		for (Tool ** tool_i = m_tools; tool_i < toolsEnd; tool_i++) {
			Tool* tool = *tool_i;
			if (tool) {
				delete tool;
			}
		}
		
		delete [] m_tools;
	}
}

void PhotoShop::initializeFilters()
{
    Filter* quantize = FilterFactory::createFilter("Quantize");
    Filter* threshold = FilterFactory::createFilter("Threshold");
    Filter* saturation = FilterFactory::createFilter("Saturation");
    Filter* adjustRGB = FilterFactory::createFilter("AdjustRGB");
    Filter* detectEdges = FilterFactory::createFilter("DetectEdges");
    Filter* blur = FilterFactory::createFilter("Blur");
    Filter* motionBlur = FilterFactory::createFilter("MotionBlur");
    Filter* sharpen = FilterFactory::createFilter("Sharpen");
    Filter* specialFilter = FilterFactory::createFilter("SpecialFilter");
    TBlur* tBlur = new TBlur();
    TStamp* tStamp = new TStamp();

}

void PhotoShop::mouseDragged(int x, int y)
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
        if (m_curTool == 6)
        {
            cout<<"Do stamp here"<<endl;
        }
		else if (m_curTool == 7)
        {
            tBlur.applyKernelToBuffer(x, m_height-y, m_displayBuffer);
        }
        else
        {
		  m_tools[m_curTool]->applyToBuffer(x, m_height-y, ColorData(m_curColorRed, m_curColorGreen, m_curColorBlue), m_displayBuffer);
        }
	}
	
	// let the previous point catch up with the current.
	m_mouseLastX = x;
	m_mouseLastY = y;
}

void PhotoShop::mouseMoved(int x, int y)
{
	
}



void PhotoShop::addToUndoStack()
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
void PhotoShop::leftMouseDown(int x, int y)
{
    
    addToUndoStack();

    if (m_curTool == 6)
    {
        tStamp.applyStamp(x, m_height-y, m_displayBuffer, stamp, ColorData(m_curColorRed, m_curColorGreen, m_curColorBlue));
    }
    else if (m_curTool == 7)
    {
        tBlur.applyKernelToBuffer(x, m_height-y, m_displayBuffer);
    }
    else
    {
        cout<<"Before tool draw"<<endl;
	   m_tools[m_curTool]->applyToBuffer(x, m_height-y, ColorData(m_curColorRed, m_curColorGreen, m_curColorBlue), m_displayBuffer);
    }

	m_mouseLastX = x;
	m_mouseLastY = y;
}


void PhotoShop::addToRedoStack()
{
    PixelBuffer* f1 = new PixelBuffer(m_width, m_height, bg);
    m_displayBuffer->copyPixelBuffer(m_displayBuffer, f1);
    
    redostack.insert(redostack.begin(), f1);
}
void PhotoShop::leftMouseUp(int x, int y)
{
    addToRedoStack();
}

void PhotoShop::initializeBuffers(ColorData backgroundColor, int width, int height) 
{
	m_displayBuffer = new PixelBuffer(width, height, backgroundColor);
}


void PhotoShop::initGlui()
{
    // Select first tool (this activates the first radio button in glui)
    m_curTool = 0;
    
    GLUI_Panel *toolPanel = new GLUI_Panel(m_glui, "Tool Type");
    {
        GLUI_RadioGroup *radio = new GLUI_RadioGroup(toolPanel, &m_curTool, UI_TOOLTYPE, s_gluicallback);
        // Create interface buttons for different tools:
        new GLUI_RadioButton(radio, "Pen");
        new GLUI_RadioButton(radio, "Eraser");
        new GLUI_RadioButton(radio, "Spray Can");
        new GLUI_RadioButton(radio, "Caligraphy Pen");
        new GLUI_RadioButton(radio, "Highlighter");
        new GLUI_RadioButton(radio, "Chalk");
        new GLUI_RadioButton(radio, "Stamp");
        new GLUI_RadioButton(radio, "Blur");
    }
    
    GLUI_Panel *colorPanel = new GLUI_Panel(m_glui, "Tool Color");
    {
        m_curColorRed = 0;
        m_gluiControlHooks.spinnerRed  = new GLUI_Spinner(colorPanel, "Red:", &m_curColorRed, UI_COLOR_R, s_gluicallback);
        m_gluiControlHooks.spinnerRed->set_float_limits(0, 1.0);
        
        m_curColorGreen = 0;
        m_gluiControlHooks.spinnerGreen  = new GLUI_Spinner(colorPanel, "Green:", &m_curColorGreen, UI_COLOR_G, s_gluicallback);
        m_gluiControlHooks.spinnerGreen->set_float_limits(0, 1.0);
        
        m_curColorBlue = 0;
        m_gluiControlHooks.spinnerBlue  = new GLUI_Spinner(colorPanel, "Blue:", &m_curColorBlue, UI_COLOR_B, s_gluicallback);
        m_gluiControlHooks.spinnerBlue->set_float_limits(0, 1.0);
        
        new GLUI_Button(colorPanel, "Red", UI_PRESET_RED, s_gluicallback);
        new GLUI_Button(colorPanel, "Orange", UI_PRESET_ORANGE, s_gluicallback);
        new GLUI_Button(colorPanel, "Yellow", UI_PRESET_YELLOW, s_gluicallback);
        new GLUI_Button(colorPanel, "Green", UI_PRESET_GREEN, s_gluicallback);
        new GLUI_Button(colorPanel, "Blue", UI_PRESET_BLUE, s_gluicallback);
        new GLUI_Button(colorPanel, "Purple", UI_PRESET_PURPLE, s_gluicallback);
        new GLUI_Button(colorPanel, "White", UI_PRESET_WHITE, s_gluicallback);
        new GLUI_Button(colorPanel, "Black", UI_PRESET_BLACK, s_gluicallback);
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
    
    new GLUI_Column(m_glui, true);
    GLUI_Panel *filterPanel = new GLUI_Panel(m_glui, "Filters");
    {
        GLUI_Panel *blurPanel = new GLUI_Panel(filterPanel, "Blur");
        {
            GLUI_Spinner * filterBlurAmount = new GLUI_Spinner(blurPanel, "Amount:", &m_filterParameters.blur_amount);
            filterBlurAmount->set_int_limits(0, 20);
            
            filterBlurAmount->set_int_val(5);
            
            new GLUI_Button(blurPanel, "Apply", UI_APPLY_BLUR, s_gluicallback);
        }
        
        GLUI_Panel *motionBlurPanel = new GLUI_Panel(filterPanel, "MotionBlur");
        {
            GLUI_Spinner * filterMotionBlurAmount = new GLUI_Spinner(motionBlurPanel, "Amount:", &m_filterParameters.motionBlur_amount);
            filterMotionBlurAmount->set_int_limits(0, 100);
            
            filterMotionBlurAmount->set_int_val(5);
            
            m_filterParameters.motionBlur_direction = 0;
            GLUI_RadioGroup *dirBlur = new GLUI_RadioGroup(motionBlurPanel, &m_filterParameters.motionBlur_direction);
            new GLUI_RadioButton(dirBlur, "North/South");
            new GLUI_RadioButton(dirBlur, "East/West");
            new GLUI_RadioButton(dirBlur, "NorthEast/SouthWest");
            new GLUI_RadioButton(dirBlur, "NorthWest/SouthEast");
            
            new GLUI_Button(motionBlurPanel, "Apply", UI_APPLY_MOTION_BLUR, s_gluicallback);
        }
        GLUI_Panel *sharpenPanel = new GLUI_Panel(filterPanel, "Sharpen");
        {
            GLUI_Spinner * filterSharpAmount = new GLUI_Spinner(sharpenPanel, "Amount:", &m_filterParameters.sharpen_amount);
            filterSharpAmount->set_int_limits(0, 100);
            
            filterSharpAmount->set_int_val(5);
            
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
        
        GLUI_Panel *saturPanel = new GLUI_Panel(filterPanel, "Saturation");
        {
            GLUI_Spinner * filterSaturationAmount = new GLUI_Spinner(saturPanel, "Amount:", &m_filterParameters.saturation_amount);
            filterSaturationAmount->set_float_limits(-10, 10);
            filterSaturationAmount->set_float_val(1);
            
            new GLUI_Button(saturPanel, "Apply", UI_APPLY_SATURATE, s_gluicallback);
        }
        
        GLUI_Panel *channelPanel = new GLUI_Panel(filterPanel, "Channels");
        {
            GLUI_Spinner * filterChannelRed = new GLUI_Spinner(channelPanel, "Red:", &m_filterParameters.channel_colorRed);
            GLUI_Spinner * filterChannelGreen = new GLUI_Spinner(channelPanel, "Green:", &m_filterParameters.channel_colorGreen);
            GLUI_Spinner * filterChannelBlue = new GLUI_Spinner(channelPanel, "Blue:", &m_filterParameters.channel_colorBlue);
            
            filterChannelRed->set_float_limits(0, 10);
            filterChannelRed->set_float_val(1);
            filterChannelGreen->set_float_limits(0, 10);
            filterChannelGreen->set_float_val(1);
            filterChannelBlue->set_float_limits(0, 10);
            filterChannelBlue->set_float_val(1);
            
            new GLUI_Button(channelPanel, "Apply", UI_APPLY_CHANNEL, s_gluicallback);
        }
        
        GLUI_Panel *quantPanel = new GLUI_Panel(filterPanel, "Quantize");
        {
            GLUI_Spinner * filterQuantizeBins = new GLUI_Spinner(quantPanel, "Bins:", &m_filterParameters.quantize_bins);
            filterQuantizeBins->set_int_limits(2, 256);
            filterQuantizeBins->set_int_val(8);
            filterQuantizeBins->set_speed(0.1);
            
            new GLUI_Button(quantPanel, "Apply", UI_APPLY_QUANTIZE, s_gluicallback);
        }
        
        GLUI_Panel *specialFilterPanel = new GLUI_Panel(filterPanel, "Special Filter"); // YOUR SPECIAL FILTER PANEL
        {
            new GLUI_Button(specialFilterPanel, "Apply", UI_APPLY_SPECIAL_FILTER, s_gluicallback);
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
        m_gluiControlHooks.loadStampButton = new GLUI_Button(imagePanel, "Load Stamp", UI_LOAD_STAMP_BUTTON, s_gluicallback);
        
        new GLUI_Separator(imagePanel);

        m_gluiControlHooks.saveFileLabel = new GLUI_StaticText(imagePanel, "Will save image: none");
        
        m_gluiControlHooks.saveCanvasButton = new GLUI_Button(imagePanel, "Save Canvas", UI_SAVE_CANVAS_BUTTON, s_gluicallback);
        
        loadCanvasEnabled(false);
        loadStampEnabled(false);
        saveCanvasEnabled(false);
    }
    return;
}

void PhotoShop::gluiControl(int controlID)
{
    
    switch (controlID) {
        case UI_PRESET_RED:
            m_curColorRed = 1;
            m_curColorGreen = 0;
            m_curColorBlue = 0;
            updateColors();
            break;
        case UI_PRESET_ORANGE:
            m_curColorRed = 1;
            m_curColorGreen = 0.5;
            m_curColorBlue = 0;
            updateColors();
            break;
        case UI_PRESET_YELLOW:
            m_curColorRed = 1;
            m_curColorGreen = 1;
            m_curColorBlue = 0;
            updateColors();
            break;
        case UI_PRESET_GREEN:
            m_curColorRed = 0;
            m_curColorGreen = 1;
            m_curColorBlue = 0;
            updateColors();
            break;
        case UI_PRESET_BLUE:
            m_curColorRed = 0;
            m_curColorGreen = 0;
            m_curColorBlue = 1;
            updateColors();
            break;
        case UI_PRESET_PURPLE:
            m_curColorRed = 0.5;
            m_curColorGreen = 0;
            m_curColorBlue = 1;
            updateColors();
            break;
        case UI_PRESET_WHITE:
            m_curColorRed = 1;
            m_curColorGreen = 1;
            m_curColorBlue = 1;
            updateColors();
            break;
        case UI_PRESET_BLACK:
            m_curColorRed = 0;
            m_curColorGreen = 0;
            m_curColorBlue = 0;
            updateColors();
            break;
        case UI_APPLY_BLUR:
            applyFilterBlur();
            break;
        case UI_APPLY_SHARP:
            applyFilterSharpen();
            break;
        case UI_APPLY_MOTION_BLUR:
            applyFilterMotionBlur();
            break;
        case UI_APPLY_EDGE:
            applyFilterEdgeDetect();
            break;
        case UI_APPLY_THRESHOLD:
            applyFilterThreshold();
            break;
        case UI_APPLY_DITHER:
            applyFilterThreshold();
            break;
        case UI_APPLY_SATURATE:
            applyFilterSaturate();
            break;
        case UI_APPLY_CHANNEL:
            applyFilterChannel();
            break;
        case UI_APPLY_QUANTIZE:
            applyFilterQuantize();
            break;
        case UI_APPLY_SPECIAL_FILTER:
            applyFilterSpecial();
            break;
        case UI_FILE_BROWSER:
            setImageFile(m_gluiControlHooks.fileBrowser->get_file());
            break;
        case UI_LOAD_CANVAS_BUTTON:
            loadImageToCanvas();
            break;
        case UI_LOAD_STAMP_BUTTON:
            loadImageToStamp();
            break;
        case UI_SAVE_CANVAS_BUTTON:
            saveCanvasToFile();
            // Reload the current directory:
            m_gluiControlHooks.fileBrowser->fbreaddir(".");
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
    m_glui->post_update_main_gfx();
}

// **********************
// *** GLUI CALLBACKS ***

// Edit these functions to provide instructions
// for how PhotoShop should respond to the
// button presses.

void PhotoShop::applyFilterThreshold()
{
    addToUndoStack();
    threshold->applyFilter(m_displayBuffer, m_filterParameters.threshold_amount);
    addToRedoStack();
    cout << "Apply has been clicked for Threshold has been clicked with amount =" << m_filterParameters.threshold_amount << endl;
}

void PhotoShop::applyFilterChannel()
{
    addToUndoStack();
    adjustRGB->applyFilter(m_displayBuffer, m_filterParameters.channel_colorRed, m_filterParameters.channel_colorGreen, m_filterParameters.channel_colorBlue);
    addToRedoStack();
    cout << "Apply has been clicked for Channels with red = " << m_filterParameters.channel_colorRed
    << ", green = " << m_filterParameters.channel_colorGreen
    << ", blue = " << m_filterParameters.channel_colorBlue << endl;
}

void PhotoShop::applyFilterSaturate()
{
    addToUndoStack();
    saturation->applyFilter(m_displayBuffer, m_filterParameters.saturation_amount);
    addToRedoStack();
    cout << "Apply has been clicked for Saturate with amount = " << m_filterParameters.saturation_amount << endl;
}

void PhotoShop::applyFilterBlur()
{
    addToUndoStack();
    blur->buildKernelAndApply(m_displayBuffer, m_filterParameters.blur_amount);
    addToRedoStack();
    cout << "Apply has been clicked for Blur with amount = " << m_filterParameters.blur_amount << endl;
}

void PhotoShop::applyFilterSharpen()
{
    addToUndoStack();
    sharpen->buildKernelAndApply(m_displayBuffer, m_filterParameters.sharpen_amount);
    addToRedoStack();
    cout << "Apply has been clicked for Sharpen with amount = " << m_filterParameters.sharpen_amount << endl;
}

void PhotoShop::applyFilterMotionBlur()
{
    addToUndoStack();
    motionBlur->buildKernelAndApply(m_displayBuffer, m_filterParameters.motionBlur_amount, m_filterParameters.motionBlur_direction);
    addToRedoStack();
    cout << "Apply has been clicked for Sharpen with amount = " << m_filterParameters.motionBlur_amount
    << " and direction " << m_filterParameters.motionBlur_direction << endl;
}

void PhotoShop::applyFilterEdgeDetect() 
{
    addToUndoStack();
    cout<<"Buidling Kernel for edge Detect"<<endl;
    detectEdges->buildKernel(m_displayBuffer);
    addToRedoStack();
    
    //detectEdges->buildFilter(m_displayBuffer);
    cout << "Apply has been clicked for Edge Detect" << endl;
}

void PhotoShop::applyFilterQuantize() 
{
    addToUndoStack();
    cout<<"does this print before the filter applies?"<<endl;

    quantize->applyFilter(m_displayBuffer, m_filterParameters.quantize_bins);
    addToRedoStack();
    cout << "Apply has been clicked for Quantize with bins = " << m_filterParameters.quantize_bins << endl;
}

void PhotoShop::applyFilterSpecial() 
{   
    addToUndoStack();
    specialFilter->buildKernel(m_displayBuffer);
    addToRedoStack();
    cout << "Apply has been clicked for Special" << endl;
}

void PhotoShop::undoOperation()
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

void PhotoShop::redoOperation()
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
void PhotoShop::loadImageToCanvas()
{
    if (m_displayBuffer) { delete m_displayBuffer; }
    m_displayBuffer = ImageHandler::loadImage(m_fileName);
    setWindowDimensions(m_displayBuffer->getWidth(), m_displayBuffer->getHeight());
}

void PhotoShop::loadImageToStamp()
{
    stamp = (ImageHandler::loadImage(m_fileName));
    
    //setWindowDimensions(m_displayBuffer->getWidth(), m_displayBuffer->getHeight());
}

void PhotoShop::saveCanvasToFile()
{
    if (ImageHandler::saveImage(m_fileName, m_displayBuffer)) {
        std::cout << "successfully saved image" << std::endl;
    } else {
        std::cout << "unsuccessfully saved image..." << std::endl;
    }
}

void PhotoShop::buttonEnabled(GLUI_Button * button, bool enabled) {
    if(enabled) button->enable();
    else button->disable();
    button->redraw();
}

void PhotoShop::redoEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.redoButton, enabled);
}

void PhotoShop::undoEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.undoButton, enabled);
}

void PhotoShop::saveCanvasEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.saveCanvasButton, enabled);
}

void PhotoShop::loadStampEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.loadStampButton, enabled);
}

void PhotoShop::loadCanvasEnabled(bool enabled)
{
    buttonEnabled(m_gluiControlHooks.loadCanvasButton, enabled);
}

void PhotoShop::updateColors() {
    m_gluiControlHooks.spinnerBlue->set_float_val(m_curColorBlue);
    m_gluiControlHooks.spinnerGreen->set_float_val(m_curColorGreen);
    m_gluiControlHooks.spinnerRed->set_float_val(m_curColorRed);
}

bool PhotoShop::hasSuffix(const std::string & str, const std::string & suffix){
    return str.find(suffix,str.length()-suffix.length()) != std::string::npos;
}


bool PhotoShop::isValidImageFileName(const std::string & name) {
    
    if (hasSuffix(name, ".png")
        || hasSuffix(name, ".jpg")
        || hasSuffix(name, ".jpeg")
        )
        return true;
    else
        return false;
}

bool PhotoShop::isValidImageFile(const std::string & name) {
    
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

void PhotoShop::setImageFile(const std::string & fileName)
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
        loadStampEnabled(true);
        loadCanvasEnabled(true);
        
        m_gluiControlHooks.currentFileLabel->set_text((std::string("Will load: ") + imageFile).c_str());
        m_gluiControlHooks.fileNameBox->set_text(imageFile);
    } else {
        loadStampEnabled(false);
        loadCanvasEnabled(false);
        m_gluiControlHooks.currentFileLabel->set_text("Will load: none");
    }
	
	//spinner3->set_float_val(m_curColorBlue);
	//spinner2->set_float_val(m_curColorGreen);
	//spinner1->set_float_val(m_curColorRed);	
}

