//
//  IPNGHandler.cpp
//  PhotoShop
//
//  Created by Sarit Ghildayal on 4/2/15.
//  Copyright (c) 2015 Sarit Ghildayal. All rights reserved.
//

#include "IPNGHandler.h"

#include "PixelBuffer.h"
#include "ColorData.h"
#include <png.h>
#include <cstring>

#include <iostream>

using std::cout;
using std::endl;

PixelBuffer* IPNGHandler::loadImage(const std::string fileName)
{
    PixelBuffer* loadedImageBuffer = NULL;
    
    png_image image;
    memset(&image, 0, (sizeof image));
    image.version = PNG_IMAGE_VERSION;
    
    if (png_image_begin_read_from_file(&image, fileName.c_str())) {
        
        loadedImageBuffer = new PixelBuffer(image.width, image.height, ColorData(0.0,0.0,0.0));
        
        png_bytep buffer;
        image.format = PNG_FORMAT_RGBA;
        buffer = new png_byte[PNG_IMAGE_SIZE(image)];
        
        if (buffer && png_image_finish_read(&image, NULL, buffer, 0, NULL)) {
            
            for (int y = 0; y < image.height; y++) {
                for (int x = 0; x < image.width; x++) {
                    int r, g, b, a = 0;
                    r = (int)buffer[(y*image.width*4)+(x*4)];
                    g = (int)buffer[(y*image.width*4)+(x*4)+1];
                    b = (int)buffer[(y*image.width*4)+(x*4)+2];
                    a = (int)buffer[(y*image.width*4)+(x*4)+3];
                    loadedImageBuffer->setPixel(x, image.height-(y+1), ColorData(r/255.0f,g/255.0f,b/255.0f,a/255.0f));
                }
            }
            
        }
        
        delete[] buffer;
    }
    
    return loadedImageBuffer;
}

bool IPNGHandler::saveImage(const std::string fileName, const PixelBuffer* bufferToSave)
{
    bool success = false;
    
    png_image image;
    
    memset(&image, 0, (sizeof image));
    image.height = bufferToSave->getHeight();
    image.width = bufferToSave->getWidth();
    image.version = PNG_IMAGE_VERSION;
    image.opaque = NULL;
    image.format = PNG_FORMAT_RGBA;
    
    png_bytep buffer = new png_byte[PNG_IMAGE_SIZE(image)];
    
    for (int y = image.height-1; y >= 0; y--) {
        for (int x = 0; x < image.width; x++) {
            ColorData currentPixel = bufferToSave->getPixel(x, y);
            buffer[((image.height-(y+1))*image.width+x)*4] = (png_byte) (currentPixel.getRed()*255.0);
            buffer[((image.height-(y+1))*image.width+x)*4+1] = (png_byte) (currentPixel.getGreen()*255.0);
            buffer[((image.height-(y+1))*image.width+x)*4+2] = (png_byte) (currentPixel.getBlue()*255.0);
            buffer[((image.height-(y+1))*image.width+x)*4+3] = (png_byte) (currentPixel.getAlpha()*255.0);
        }
    }
    
    if (png_image_write_to_file(&image, fileName.c_str(), 0/*convert_to_8bit*/,
                                buffer, 0/*row_stride*/, NULL/*colormap*/) != 0) {
        success = true;
    } else {
        success = false;
    }
    
    delete[] buffer;
    return success;
}
