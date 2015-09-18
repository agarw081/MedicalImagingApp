//
//  IPNGHandler.h
//  PhotoShop
//
//  Created by Sarit Ghildayal on 4/2/15.
//  Copyright (c) 2015 Sarit Ghildayal. All rights reserved.
//
//  For educational purposes only.  Please do not post online.

#ifndef __PhotoShop__IPNGHandler__
#define __PhotoShop__IPNGHandler__

#include <string>

#include "ImageHandler.h"

class PixelBuffer;

class IPNGHandler : public ImageHandler {
public:
    static PixelBuffer* loadImage(const std::string fileName);
    static bool saveImage(const std::string fileName, const PixelBuffer* bufferToSave);
};

#endif
