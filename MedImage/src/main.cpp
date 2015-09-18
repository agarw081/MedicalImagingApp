//
//  main.cpp
//
//  Created by Umang Agarwal
//  CommandLine by Christian Wohlwend


#include "MedImageGUI.h"
#include "ColorData.h"
#include "PixelBuffer.h"
#include "ImageHandler.h"

#include <iostream>
#include <string>

#include <stdlib.h>
#include <typeinfo>

using std::cout;
using std::endl;
using std::string;

MedImageGUI *app;

void CommandLine(int argCount, char* arguments[]);

int main(int argc, char* argv[])
{
	MedImageGUI *app = new MedImageGUI(argc, argv,800,800, ColorData(1,1,0.95));
	if (argc > 1)
	{
		CommandLine(argc, argv);
	}
	else
	{
	app->runMainLoop();
	}
	exit(0);
}

void HelpMessege()
{
	cout<<"- Help information for command line -"<<endl;
		cout<<"	Valid functions:"<<endl;
		cout<<"		-sharpen <integer>			(sharpens image by integer amount)"<<endl;
		cout<<"		-edgedetect				(detects edges in image)"<<endl;
		cout<<"		-thresh <float> 			(adjusts color threshold by float value)"<<endl;
		cout<<"		-quantize <int> 			(reduces color range by passed in quantize)"<<endl;
		cout<<"		-blur <float>				(adds blur to passed in image by flaot amount)"<<endl;
		cout<<"		-saturate <float> 			(saturates images colors by float amount)"<<endl;
		cout<<"		-multrgb <float> <float> <float> 	(adjusts color channels of passed in image by the 3 float values. R G B respecitvely)"<<endl;
		cout<<"		-stamp <integer> <integer> 		(adds red crosshair to image at passed in x and y values)"<<endl;
		cout<<"		-compare 				(compares 2 images pixel for pixel, returns 0 if different, 1 if same)"<<endl;
		cout<<""<<endl;
		cout<<"	To save to output image, finish command sequence with the name of the output iamge and image type you'd like to save as."<<endl;
		cout<<"		Example:"<<endl;
		cout<<"			./medimage testimage.jpg -thresh .5 -stamp 256 256 testimageout.png"<<endl;
		cout<<""<<endl;
		cout<<"	You can also load in image sequences by loading in the first image in a 3 digit sequence."<<endl;
		cout<<"		Example:"<<endl;
		cout<<"			./medimage testimage001.png -multrgb .5 1.0 0.0 -blur 8 testimageout001.jpg"<<endl;
}

void CommandLine(int argCount, char* arguments[])
{
	bool imageSeq = false;
	bool imageValid;
	PixelBuffer* inputImage;
	std::string imageName;
	if(std::string(arguments[1]) == "-h"){
		HelpMessege();
		return;
	}
	if (app->isValidImageFile(std::string(arguments[1])))
	{
		imageName = std::string(arguments[1]);
		inputImage = ImageHandler::loadImage(std::string(arguments[1]));
		imageValid = true;
	}
	else
	{
		imageValid = false;
		//image not valid
		cout<<"bad input"<<endl;
		HelpMessege();
		return;
	}
	while(imageValid || imageSeq)
	{
	
	for (int i = 2; i < argCount; i++)
	{
		if(std::string(arguments[i]) =="-compare" && app->isValidImageFile(std::string(arguments[argCount-1]))){
			bool isSame = true;
			PixelBuffer* secondImage = ImageHandler::loadImage(std::string(arguments[argCount-1]));
			if( (inputImage->getHeight() == secondImage->getHeight()) && (inputImage->getWidth() == secondImage->getWidth()) )
			{
				for (int i = 0; i < inputImage->getWidth(); i++)
				{
					for (int j = 0; j < inputImage->getHeight(); j++)
					{
						ColorData inputPixel = inputImage->getPixel(i,j);
						ColorData secondPixel = secondImage->getPixel(i,j);
						if (!(inputPixel == secondPixel))
						{
							isSame = false;
						}

					}
				}
				
			}
			else
			{
				isSame = false;
			}
			if(isSame)
			{
				cout<<1<<endl;
				cout<<"comparison passed"<<endl;
				return;
			}
			else
			{
				cout<<0<<endl;
				cout<<"comparision failed"<<endl;
				return;
			}
		}
		if(std::string(arguments[i]) == "-edgedetect") {
			app->FilterEdgeDetect(inputImage);
		}
		else if(std::string(arguments[i]) == "-h") {
			HelpMessege();
		}
		else if((std::string(arguments[i]) == "-sharpen") /*&& ((typeid(arguments[i+1]).name()) =="int")*/) {
			app->FilterSharpen(inputImage, (atoi(arguments[i+1])) ); 
			i++;
		}
		else if( (std::string(arguments[i]) == "-thresh")/* && ((std::string(arguments[i+1])) =="float") */) {
			inputImage = app->FilterThreshold(inputImage, atof(arguments[i+1])); 
			i++;
		}
		else if((std::string(arguments[i]) == "-quantize")/* && ((std::string(arguments[i+1])) =="int")*/) {
			inputImage = app->FilterQuantize(inputImage, (atoi(arguments[i+1]))); 
			i++; 
		}
		else if( (std::string(arguments[i]) == "-blur")/* && ((std::string(arguments[i+1])) =="float")*/ ) {
			inputImage = app->FilterBlur(inputImage, atof(arguments[i+1]));
			i++;
		}
		else if( (std::string(arguments[i]) == "-saturate")/* && ((std::string(arguments[i+1])) =="int")*/ ) {
			inputImage = app->FilterSaturate(inputImage, (atoi(arguments[i+1])));
			i++;
		}
		else if( (std::string(arguments[i]) == "-stamp") /*&& ((std::string(arguments[i+1])) == "int") && ((std::string(arguments[i+2])) =="int") */)  {
		 	PixelBuffer* stamp = (ImageHandler::loadImage("marker.png")); 
		 	inputImage = app->applyStamp(atoi(arguments[i+1]),(atoi(arguments[i+2])), inputImage, stamp); 
		 	i=i+2; 
		}
		else if( (std::string(arguments[i]) == "-multrgb")/* && ((std::string(arguments[i+1])) == "float") && ((std::string(arguments[i+2])) =="float") && ((std::string(arguments[i+3])) =="float")*/) {
			inputImage = app->FilterMultiplyRGB(inputImage, atof(arguments[i+1]), atof(arguments[i+2]), atof(arguments[i+3])); 
			i=i+3; 
		}
		else if(app->isValidImageFileName(std::string(arguments[i])))
		{
			if (imageSeq){
				std::string newName = app->getImageNamePlusSeqOffset(std::string(arguments[i]), 1);
				arguments[i] = &newName[0u];
				if (ImageHandler::saveImage(newName, inputImage)) {
        		cout << "successfully saved image" << endl;
    			} 
    			else {
        		cout << "unsuccessfully saved image..." << endl;
    			}
    		}
    		else
    		{
    			if (ImageHandler::saveImage(std::string(arguments[i]), inputImage)) {
        		cout << "successfully saved image" << endl;
    			} 
    			else {
        		cout << "unsuccessfully saved image..." << endl;
    			}
    		}
		}
		else { 
		cout<<"bad input"<<endl;
		HelpMessege();
		return;
		}
	}
	imageName = app->getImageNamePlusSeqOffset(imageName, 1);
	imageSeq = app->isValidImageFile(imageName);
	if (imageSeq)
	{
		inputImage = ImageHandler::loadImage(imageName);
	}
	else
	{
		imageValid = false;
	}
	}
}
