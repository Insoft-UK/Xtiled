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

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <fstream>
#include <array>

#include "xTiled.hpp"

#include "build.h"

bool verbose = false;

enum class MessageType {
    Warning,
    Error,
    Verbose
};

std::ostream& operator<<(std::ostream& os, MessageType type) {
    switch (type) {
        case MessageType::Error:
            os << R"(\e[1;91mError\e[0m: )";
            break;

        case MessageType::Warning:
            os << R"(\e[1;93mWarning\e[0m: )";
            break;
            
        case MessageType::Verbose:
            os << ": ";
            break;

        default:
            os << ": ";
            break;
    }

    return os;
}

/*
 The decimalToBase24 function converts a given 
 base 10 integer into its base 24 representation using a
 specific set of characters. The character set is
 comprised of the following 24 symbols:

     •    Numbers: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
     •    Letters: C, D, F, H, J, K, M, N, R, U, V, W, X, Y
     
 Character Selection:
 The choice of characters was made to avoid confusion
 with common alphanumeric representations, ensuring
 that each character is visually distinct and easily
 recognizable. This set excludes characters that closely
 resemble each other or numerical digits, promoting
 clarity in representation.
 */
static std::string decimalToBase24(int num) {
    if (num == 0) {
        return "C";
    }

    const std::string base24Chars = "0123456789CDFHJKMNRUVWXY";
    std::string base24;

    while (num > 0) {
        int remainder = num % 24;
        base24 = base24Chars[remainder] + base24; // Prepend character
        num /= 24; // Integer division
    }

    return base24;
}

static std::string getBuildCode(void) {
    std::string str;
    int majorVersionNumber = BUILD_NUMBER / 100000;
    str = std::to_string(majorVersionNumber) + decimalToBase24(BUILD_NUMBER - majorVersionNumber * 100000);
    return str;
}

void help(void)
{
    int rev = BUILD_NUMBER / 1000 % 10;
    
    std::cout << "Copyright (C) 2024 Insoft. All rights reserved.\n";
    std::cout << "Insoft xTiled version, " << BUILD_NUMBER / 100000 << "." << BUILD_NUMBER / 10000 % 10 << (rev ? "." + std::to_string(rev) : "")
    << " (BUILD " << getBuildCode() << "-" << decimalToBase24(BUILD_DATE) << ")\n\n";
    std::cout << "Usage: repix <input-file> [-o <output-file>] [-w <width>] [-h <height>] [-t <tilecount>] [-s <similarity>]\n\n";
    std::cout << "Options:\n";
    std::cout << "    -o  <output-file>        Specify the filename for Tiled file.\n";
    std::cout << "    -w  <width>              Specify the width of the tiles used.\n";
    std::cout << "    -h  <height>             Specify the height of the tiles used.\n";
    std::cout << "    -c  <tilecount>          Specify the number of tiles used.\n";
    std::cout << "    -s  <similarity>         Specify similarity percentage of tiles for matching.\n";
    std::cout << "\n";
    std::cout << "Additional Commands:\n";
    std::cout << "  xtiled {-version | -help}\n";
    std::cout << "    -version                 Display the version information.\n";
    std::cout << "    -help                    Show this help message.\n";
}

void version(void) {
    std::cout << "Copyright (C) 2024 Insoft. All rights reserved.\n";
    std::cout << "Insoft xTiled version, " << BUILD_NUMBER / 100000 << "." << BUILD_NUMBER / 10000 % 10 << "." << BUILD_NUMBER / 1000 % 10
    << " (BUILD " << getBuildCode() << ")\n";
    std::cout << "Built on: " << CURRENT_DATE << "\n";
    std::cout << "Licence: MIT License\n\n";
    std::cout << "For more information, visit: http://www.insoft.uk\n";
}

void error(void)
{
    std::cout << "xtiled: try 'xtiled -help' for more information\n";
    exit(0);
}

void info(void) {
    std::cout << "Copyright (c) 2024 Insoft. All rights reserved.\n";
    int rev = BUILD_NUMBER / 1000 % 10;
    std::cout << "xTiled version, " << BUILD_NUMBER / 100000 << "." << BUILD_NUMBER / 10000 % 10 << (rev ? "." + std::to_string(rev) : "")
    << " (BUILD " << getBuildCode() << "-" << decimalToBase24(BUILD_DATE) << ")\n\n";
}

bool fileExists(const std::string& filename) {
    std::ofstream outfile;
    outfile.open(filename, std::ios::in | std::ios::binary);
    if(!outfile.is_open()) {
        return false;
    }
    outfile.close();
    return true;
}

std::string removeExtension(const std::string& filename) {
    // Find the last dot in the string
    size_t lastDotPosition = filename.find_last_of('.');

    // If there is no dot, return the original string
    if (lastDotPosition == std::string::npos) {
        return filename;
    }

    // Return the substring from the beginning up to the last dot
    return filename.substr(0, lastDotPosition);
}


int main(int argc, const char * argv[])
{
    if ( argc == 1 ) {
        error();
        return 0;
    }
    
    std::string out_filename, in_filename;
    
    
    
    xTiled xtiled = xTiled();
    
    for( int n = 1; n < argc; n++ ) {
        if (*argv[n] == '-') {
            std::string args(argv[n]);
            
            if (args == "-o") {
                if (++n > argc) error();
                out_filename = argv[n];
                continue;
            }
            
            if (args == "-w") {
                if (++n > argc) error();
                xtiled.tileWidth = atoi(argv[n]);
                continue;
            }
            
            if (args == "-h") {
                if (++n > argc) error();
                xtiled.tileHeight = atoi(argv[n]);
                continue;
            }
            
            if (args == "-c") {
                if (++n > argc) error();
                xtiled.tileCount = atoi(argv[n]);
                continue;
            }
            
            if (args == "-s") {
                if (++n > argc) error();
                xtiled.similarityPercentage = atof(argv[n]);
                continue;
            }
            
            
            if (args == "-help") {
                help();
                return 0;
            }
            
            if (args == "-version") {
                version();
                return 0;
            }
            
            error();
            return 0;
        }
        in_filename = argv[n];
    }
    
    info();
    
    if (!fileExists(in_filename)) {
        std::cout << MessageType::Error << "File '" << in_filename << "' not found.\n";
        return -1;
    }
    
    if (out_filename.empty() || out_filename == in_filename) {
        out_filename = removeExtension(in_filename) + ".xml";
    }
    
    xtiled.loadTiledImage(in_filename);
    
    if (!xtiled.isTiledImageLoaded()) {
        std::cout << MessageType::Error << "File '" << in_filename << "' failed to load.\n";
        return -1;
    }
    
    
    xtiled.generateTMJData();
    xtiled.createTMJFile(out_filename);
    
    
    return 0;
}

