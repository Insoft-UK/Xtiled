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

#include "xTiled.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <regex>
#include <fstream>
#include <array>

#include <string>

static int* arr;

static std::string removeExtension(const std::string& filename) {
    // Find the last dot in the string
    size_t lastDotPosition = filename.find_last_of('.');

    // If there is no dot, return the original string
    if (lastDotPosition == std::string::npos) {
        return filename;
    }

    // Return the substring from the beginning up to the last dot
    return filename.substr(0, lastDotPosition);
}

static std::string removePath(const std::string& filename) {
    // Find the last dot in the string
    size_t pos = filename.find_last_of('/');

    if (pos == std::string::npos) {
        return filename;
    }

    // Return the substring from the beginning up to the last dot
    return filename.substr(pos + 1, filename.length() - pos - 1);
}

static void extractTileFromImage(const TImage* dstImage, const TImage* srcImage, int x, int y, int w, int h) {
    copyPixmap(dstImage, 0, 0, srcImage, x, y, w, h);
}

static int findTileUID(const TImage* tileset, const TImage* tile) {
    int w = tile->width;
    int h = tile->height;
    
    int uid = 1;
    
    for (int y = 0; y < tileset->height; y += h) {
        for (int x = 0; x < tileset->width; x += w) {
            if (compareSubImage(tileset, x, y, tile)) {
                return uid;
            }
            uid++;
        }
    }
    
    return -1;
}

static void appendTileToTileset(const TImage* tile, const TImage* tileset) {
    int w = tile->width;
    int h = tile->height;
    
    for (int y = 0; y < tileset->height; y += h) {
        for (int x = 0; x < tileset->width; x += w) {
            if (containsImage(tileset, x, y, w, h))
                continue;
            
            copyImageAt(tileset, x, y, tile);
            return;
        }
    }
}


//MARK: - xTiled Method/s

void xTiled::createTMJFile(std::string& filename) {
    std::ofstream outfile;
    
    
    if (_tileset != nullptr) {
        saveImageAsPNGFile(_tileset, removeExtension(filename) + ".png");
    }
    
    std::string tmj = R"({ 
    "compressionlevel":-1,
    "height":@height,
    "infinite":false,
    "layers":[
        {
            "data":[
@layers.data
            ],
            "height":@layers.height,
            "id":1,
            "name":"@name",
            "opacity":1,
            "type":"tilelayer",
            "visible":true,
            "width":@layers.width,
            "x":0,
            "y":0
        }
    ],
    "nextlayerid":2,
    "nextobjectid":1,
    "orientation":"orthogonal",
    "renderorder":"right-down",
    "tiledversion":"1.11.0",
    "tileheight":@tileheight,
    "tilesets":[
        {
            "columns":16,
            "firstgid":1,
            "image":"@image",
            "imageheight":@tilesets.imageheight,
            "imagewidth":@tilesets.imagewidth,
            "margin":0,
            "name":"@name",
            "spacing":0,
            "tilecount":@tilesets.tilecount,
            "tileheight":@tilewidth,
            "tilewidth":@tilewidth,
            "transparentcolor":"#ff00ff"
        }
    ],
    "tilewidth":@tilewidth,
    "type":"map",
    "version":"1.10",
    "width":@width
})";
    
    tmj = regex_replace(tmj, std::regex(R"(@image)"), removePath(removeExtension(filename)) + ".png");
    tmj = regex_replace(tmj, std::regex(R"(@name)"), removePath(removeExtension(filename)));
    
    
    tmj = regex_replace(tmj, std::regex(R"(@width)"), std::to_string(_tiledImage->width / tileWidth));
    tmj = regex_replace(tmj, std::regex(R"(@height)"), std::to_string(_tiledImage->height / tileHeight));
    tmj = regex_replace(tmj, std::regex(R"(@tilewidth)"), std::to_string(tileWidth));
    tmj = regex_replace(tmj, std::regex(R"(@tileheight)"), std::to_string(tileHeight));
    tmj = regex_replace(tmj, std::regex(R"(@tilesets\.imagewidth)"), std::to_string(_tileset->width));
    tmj = regex_replace(tmj, std::regex(R"(@tilesets\.imageheight)"), std::to_string(_tileset->height));
    tmj = regex_replace(tmj, std::regex(R"(@tilesets\.tilecount)"), std::to_string(tileCount));
    
    tmj = regex_replace(tmj, std::regex(R"(@layers\.width)"), std::to_string(_tiledImage->width / tileWidth));
    tmj = regex_replace(tmj, std::regex(R"(@layers\.height)"), std::to_string(_tiledImage->height / tileHeight));
    tmj = regex_replace(tmj, std::regex(R"(@layers\.name)"), "Tile Layer");
    
    std::ostringstream os;
    int col = _tiledImage->width / tileWidth;
    int row = _tiledImage->height / tileHeight;
    for (int r = 0; r < row; r++) {
        os << "\t\t\t\t";
        for (int c = 0; c < col; c++) {
            os << arr[c + r * col] << ", ";
        }
        if (r < row - 1) {
            os << "\n";
        }
    }
    auto data = os.str();
    data.resize(data.length() - 2);
    tmj = regex_replace(tmj, std::regex(R"(@layers\.data)"), data);
    
    
    outfile.open(removeExtension(filename) + ".tmj", std::ios::out | std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(tmj.c_str(), tmj.length());
        outfile.close();
    }
    
    delete [] arr;
}

void xTiled::generateTMJData(void) {
    int columnCount = (int)ceil(sqrt((double)tileCount));
    _tileset = createPixmap(tileWidth * columnCount, tileHeight * columnCount, _tiledImage->bitWidth);
    
    _tileCount=1;
    
    
    if (_tileset == nullptr) {
        std::cout << "ERROR!\n";
        return;
    }
    
    TImage* tile = createPixmap(tileWidth, tileHeight, _tiledImage->bitWidth);
    
    uint32_t* p = (uint32_t *)tile->data;
    for (int i = 0; i < tile->width * tile->height; i++) {
        *p++ = 0xFF000000;
    }
    appendTileToTileset(tile, _tileset);
    
    int col = _tiledImage->width / tileWidth;
    int row = _tiledImage->height / tileHeight;
    arr = new int[col * row];
    
    int i = 0;
    int uid;
    for (int y = 0; y < _tiledImage->height; y += tileHeight) {
        for (int x = 0; x < _tiledImage->width; x += tileWidth) {
            extractTileFromImage(tile, _tiledImage, x, y, tileWidth, tileHeight);
            uid = findTileUID(_tileset, tile);
            if (uid == -1) {
                if (_tileCount < tileCount) {
                    appendTileToTileset(tile, _tileset);
                    uid = ++_tileCount;
                } else {
                    uid = 0;
                }
            }
            arr[i++] = uid;
        }
    }
    
    reset(tile);
}



