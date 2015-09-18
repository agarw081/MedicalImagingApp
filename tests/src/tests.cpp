//Tests.cpp
// (c) 2015 Christian Wohlwend


#include "tests.h"
#include "ColorData.h"
#include "PixelBuffer.h"
#include "ToolFactory.h"
#include "Tool.h"
#include "ImageHandler.h"
#include "TBlur.h"
#include "TStamp.h"

#include <png.h>
#include <cstring>
#include <stddef.h>
#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <iostream>
#include <stdlib.h>


using std::cout;
using std::endl;


tests::tests()
{
	m_tools = new Tool* [ToolFactory::getNumTools()];
	for (int i = 0; i < ToolFactory::getNumTools(); i++) {
		m_tools[i] = ToolFactory::createTool(i);
	}
	//generate_tests();
	runTests();
}

void tests::runTests()
{
	test_pen();
	test_eraser();
	test_spraycan();
	test_caligraphypen();
	test_highlighter();
	//test_chalk();
	test_blurtool();
}

void tests::test_pen()
{
	PixelBuffer* imageToPaint = ImageHandler::loadImage("testImages/grumpyTest.png");
	m_tools[0]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[0]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[0]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/grumpyTestTemp.png", imageToPaint);
	imageToPaint = ImageHandler::loadImage("testImages/grumpyTestTemp.png");
	PixelBuffer* imageToCompare = ImageHandler::loadImage("testImages/preComputedImages/grumpyTest_pen.png");
	bool success = comparePixelBuffers(imageToPaint, imageToCompare);
	if(success)
	{
		cout<<"Pen - VALID"<<endl;
	}
	else
	{
		cout<<"Pen - FAILURE"<<endl;
	}
}

void tests::test_eraser()
{
	PixelBuffer* imageToPaint = ImageHandler::loadImage("testImages/grumpyTest.png");
	m_tools[1]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[1]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[1]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/grumpyTestTemp.png", imageToPaint);
	imageToPaint = ImageHandler::loadImage("testImages/grumpyTestTemp.png");
	PixelBuffer* imageToCompare = ImageHandler::loadImage("testImages/preComputedImages/grumpyTest_eraser.png");
	bool success = comparePixelBuffers(imageToPaint, imageToCompare);
	if(success)
	{
		cout<<"Eraser - VALID"<<endl;
	}
	else
	{
		cout<<"Eraser - FAILURE"<<endl;
	}
}

void tests::test_spraycan()
{
	PixelBuffer* imageToPaint = ImageHandler::loadImage("testImages/grumpyTest.png");
	m_tools[2]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[2]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[2]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/grumpyTestTemp.png", imageToPaint);
	imageToPaint = ImageHandler::loadImage("testImages/grumpyTestTemp.png");
	PixelBuffer* imageToCompare = ImageHandler::loadImage("testImages/preComputedImages/grumpyTest_spraycan.png");
	bool success = comparePixelBuffers(imageToPaint, imageToCompare);
	if(success)
	{
		cout<<"SprayCan - VALID"<<endl;
	}
	else
	{
		cout<<"SprayCan - FAILURE"<<endl;
	}
}

void tests::test_caligraphypen()
{
	PixelBuffer* imageToPaint = ImageHandler::loadImage("testImages/grumpyTest.png");
	m_tools[3]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[3]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[3]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/grumpyTestTemp.png", imageToPaint);
	imageToPaint = ImageHandler::loadImage("testImages/grumpyTestTemp.png");
	PixelBuffer* imageToCompare = ImageHandler::loadImage("testImages/preComputedImages/grumpyTest_caligraphypen.png");
	bool success = comparePixelBuffers(imageToPaint, imageToCompare);
	if(success)
	{
		cout<<"CaligraphyPen - VALID"<<endl;
	}
	else
	{
		cout<<"CaligraphyPen - FAILURE"<<endl;
	}
}
void tests::test_highlighter()
{
	PixelBuffer* imageToPaint = ImageHandler::loadImage("testImages/grumpyTest.png");
	m_tools[4]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[4]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[4]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/grumpyTestTemp.png", imageToPaint);
	imageToPaint = ImageHandler::loadImage("testImages/grumpyTestTemp.png");
	PixelBuffer* imageToCompare = ImageHandler::loadImage("testImages/preComputedImages/grumpyTest_highlighter.png");
	bool success = comparePixelBuffers(imageToPaint, imageToCompare);
	if(success)
	{
		cout<<"Highlighter - VALID"<<endl;
	}
	else
	{
		cout<<"Highlighter - FAILURE"<<endl;
	}
}

void tests::test_chalk()
{
	PixelBuffer* imageToPaint = ImageHandler::loadImage("testImages/grumpyTest.png");
	m_tools[5]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[5]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[5]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/grumpyTestTemp.png", imageToPaint);
	imageToPaint = ImageHandler::loadImage("testImages/grumpyTestTemp.png");
	PixelBuffer* imageToCompare = ImageHandler::loadImage("testImages/preComputedImages/grumpyTest_chalk.png");
	bool success = comparePixelBuffers(imageToPaint, imageToCompare);
	if(success)
	{
		cout<<"Chalk - VALID"<<endl;
	}
	else
	{
		cout<<"Chalk - FAILURE"<<endl;
	}
}

void tests::test_blurtool()
{
	PixelBuffer* imageToPaint = ImageHandler::loadImage("testImages/grumpyTest.png");
	TBlur* tBlur = new TBlur();
	tBlur->applyKernelToBuffer(256, 256, imageToPaint);
    tBlur->applyKernelToBuffer(400, 400, imageToPaint);
    tBlur->applyKernelToBuffer(350, 50, imageToPaint);
    ImageHandler::saveImage("testImages/grumpyTestTemp.png", imageToPaint);
	imageToPaint = ImageHandler::loadImage("testImages/grumpyTestTemp.png");
	PixelBuffer* imageToCompare = ImageHandler::loadImage("testImages/preComputedImages/grumpyTest_blurtool.png");
	bool success = comparePixelBuffers(imageToPaint, imageToCompare);
	if(success)
	{
		cout<<"BlurTool - VALID"<<endl;
	}
	else
	{
		cout<<"BlurTool - FAILURE"<<endl;
	}
}






void tests::generate_tests()
{
	generate_test_pen();
	generate_test_eraser();
	generate_test_spraycan();
	generate_test_caligraphypen();
	generate_test_highlighter();
	generate_test_chalk();
	generate_test_blurtool();
}

void tests::generate_test_pen()
{
	std::string imagePath = "testImages/grumpyTest.png";
	PixelBuffer* imageToPaint = ImageHandler::loadImage(imagePath);
	m_tools[0]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[0]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[0]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/preComputedImages/grumpyTest_pen.png", imageToPaint);
}

void tests::generate_test_eraser()
{
	std::string imagePath = "testImages/grumpyTest.png";
	PixelBuffer* imageToPaint = ImageHandler::loadImage(imagePath);
	m_tools[1]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[1]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[1]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/preComputedImages/grumpyTest_eraser.png", imageToPaint);
}

void tests::generate_test_spraycan()
{
	std::string imagePath = "testImages/grumpyTest.png";
	PixelBuffer* imageToPaint = ImageHandler::loadImage(imagePath);
	m_tools[2]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[2]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[2]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/preComputedImages/grumpyTest_spraycan.png", imageToPaint);
}

void tests::generate_test_caligraphypen()
{
	std::string imagePath = "testImages/grumpyTest.png";
	PixelBuffer* imageToPaint = ImageHandler::loadImage(imagePath);
	m_tools[3]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[3]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[3]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/preComputedImages/grumpyTest_caligraphypen.png", imageToPaint);
}

void tests::generate_test_highlighter()
{
	std::string imagePath = "testImages/grumpyTest.png";
	PixelBuffer* imageToPaint = ImageHandler::loadImage(imagePath);
	m_tools[4]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[4]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[4]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/preComputedImages/grumpyTest_highlighter.png", imageToPaint);
}
void tests::generate_test_chalk()
{
	std::string imagePath = "testImages/grumpyTest.png";
	PixelBuffer* imageToPaint = ImageHandler::loadImage(imagePath);
	m_tools[5]->applyToBuffer(256, 256, ColorData(1, 0, 0), imageToPaint);
	m_tools[5]->applyToBuffer(400, 400, ColorData(1, 0, 0), imageToPaint);
	m_tools[5]->applyToBuffer(350, 50, ColorData(1, 0, 0), imageToPaint);
	ImageHandler::saveImage("testImages/preComputedImages/grumpyTest_chalk.png", imageToPaint);
}

void tests::generate_test_blurtool()
{
	std::string imagePath = "testImages/grumpyTest.png";
	PixelBuffer* imageToPaint = ImageHandler::loadImage(imagePath);
    TBlur* tBlur = new TBlur();
    tBlur->applyKernelToBuffer(256, 256, imageToPaint);
    tBlur->applyKernelToBuffer(400, 400, imageToPaint);
    tBlur->applyKernelToBuffer(350, 50, imageToPaint);
	ImageHandler::saveImage("testImages/preComputedImages/grumpyTest_blurtool.png", imageToPaint);
}

bool tests::comparePixelBuffers(PixelBuffer* firstBuffer, PixelBuffer* secondBuffer)
{
	bool isSame = true;
	if( (firstBuffer->getHeight() == secondBuffer->getHeight()) && (firstBuffer->getWidth() == secondBuffer->getWidth()) )
	{
		for (int i = 0; i < firstBuffer->getWidth(); i++)
		{
			for (int j = 0; j < firstBuffer->getHeight(); j++)
			{
				ColorData inputPixel = firstBuffer->getPixel(i,j);
				ColorData secondPixel = secondBuffer->getPixel(i,j);
				if (!(inputPixel == secondPixel))
				{
					cout<<i<<endl;
					cout<<j<<endl;
					return(false);
				}
			}
		}		
	}
	else
	{
		return(false);
	}
	return true;
}

bool tests::hasSuffix(const std::string & str, const std::string & suffix){
    return str.find(suffix,str.length()-suffix.length()) != std::string::npos;
}

bool tests::isValidImageFileName(const std::string & name) {
    
    if (hasSuffix(name, ".png")
        || hasSuffix(name, ".jpg")
        || hasSuffix(name, ".jpeg")
        )
        return true;
    else
        return false;
}

bool tests::isValidImageFile(const std::string & name) {
    
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



