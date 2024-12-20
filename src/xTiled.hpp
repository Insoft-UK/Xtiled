/*
 The MIT License (MIT)
 
 Copyright (c) 2024 Insoft. All rights reserved.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef xTiled_hpp
#define xTiled_hpp

#include "image.hpp"

class xTiled {
public:
    unsigned tileWidth = 0;
    unsigned tileHeight = 0;
    unsigned tileCount = 2048;
    float similarityPercentage = 1.0;
    uint32_t transparentColor = 0;
    
    ~xTiled() {
        reset(_tiledImage);
        reset(_tileset);
    }
    
    bool isTiledImageLoaded(void) {
        return _tiledImage != nullptr;
    }
    
    void loadTiledImage(std::string& imagefile) {
        _tiledImage = loadPNGGraphicFile(imagefile);
    }
    
    void createTMJFile(std::string& filename);
    void generateTMJData(void);
    
private:
    TImage* _tiledImage = nullptr;
    TImage* _tileset = nullptr;
    int _tileCount = 0;
};

#endif /* xTiled_hpp */
