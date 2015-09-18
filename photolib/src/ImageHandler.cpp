//
//  ImageHandler.cpp
//  PhotoShop
//
//  Created by Sarit Ghildayal on 4/2/15.
//  Copyright (c) 2015 Sarit Ghildayal. All rights reserved.
//

#include "ImageHandler.h"

#include "PixelBuffer.h"
#include "ColorData.h"

#include "IPNGHandler.h"
#include "IJPGHandler.h"

#include <iostream>

using std::cout;
using std::endl;


PixelBuffer* ImageHandler::loadImage(const std::string & fileName) {
    
    IMAGE_TYPE imageType = getImageType(fileName);
    PixelBuffer* loadedImage = NULL;
    if (imageType == PNG_IMAGE) {
        loadedImage = IPNGHandler::loadImage(fileName);
    } else if (imageType == JPG_IMAGE) {
        loadedImage = IJPGHandler::loadImage(fileName);
    }  else {
        // how to error out?
    }
    return loadedImage;
}

bool ImageHandler::saveImage(const std::string & fileName, const PixelBuffer* bufferToSave) {
    IMAGE_TYPE imageType = getImageType(fileName);
    bool success = false;
    if (imageType == PNG_IMAGE) {
        success = IPNGHandler::saveImage(fileName, bufferToSave);
    } else if (imageType == JPG_IMAGE) {
        success = IJPGHandler::saveImage(fileName, bufferToSave);
    }  else {
        // how to error out?
    }
    return success;
}

ImageHandler::IMAGE_TYPE ImageHandler::getImageType(const std::string & fileName) {
    if (fileName.find(".png",fileName.size()-5) != std::string::npos) {
        return PNG_IMAGE;
    } else if (fileName.find(".jpg",fileName.size()-5) != std::string::npos || fileName.find(".jpeg",fileName.size()-6) != std::string::npos) {
        return JPG_IMAGE;
    } else {
        return UNKNOWN_IMAGE;
    }
}