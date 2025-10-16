// The MIT License (MIT)
//
// Copyright (c) 2024-2025 Insoft.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <fstream>
#include <array>

#include "xtiled.hpp"


#include "../version_code.h"

#define NAME "Xtiled"
#define COMMAND_NAME "xtiled"

bool verbose = false;

enum class MessageType {
    Warning,
    Error,
    Verbose
};

std::ostream& operator<<(std::ostream& os, MessageType type) {
    switch (type) {
        case MessageType::Error:
            os << "❌ ";
            break;

        case MessageType::Warning:
            os << "⚠️ ";
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


bool fileExists(const std::string& filename) {
    std::ofstream outfile;
    outfile.open(filename, std::ios::in | std::ios::binary);
    if(!outfile.is_open()) {
        return false;
    }
    outfile.close();
    return true;
}


// MARK: - Extensions

namespace std::filesystem {
    std::string expand_tilde(const std::string& path) {
        if (!path.empty() && path.starts_with("~")) {
#ifdef _WIN32
            const char* home = std::getenv("USERPROFILE");
#else
            const char* home = std::getenv("HOME");
#endif
            
            if (home) {
                return std::string(home) + path.substr(1);  // Replace '~' with $HOME
            }
        }
        return path;  // return as-is if no tilde or no HOME
    }
}

// MARK: - Command Line
void version(void) {
    using namespace std;
    std::cout
    << "Copyright (C) 2024-" << YEAR << " Insoft.\n"
    << "Insoft "<< NAME << " version, " << VERSION_NUMBER << " (BUILD " << VERSION_CODE << ")\n"
    << "Built on: " << DATE << "\n"
    << "Licence: MIT License\n\n"
    << "For more information, visit: http://www.insoft.uk\n";
}

void error(void) {
    std::cout << COMMAND_NAME << ": try '" << COMMAND_NAME << " --help' for more information\n";
    exit(0);
}

void info(void) {
    using namespace std;
    std::cout
    << "          ***********     \n"
    << "        ************      \n"
    << "      ************        \n"
    << "    ************  **      \n"
    << "  ************  ******    \n"
    << "************  **********  \n"
    << "**********    ************\n"
    << "************    **********\n"
    << "  **********  ************\n"
    << "    ******  ************  \n"
    << "      **  ************    \n"
    << "        ************      \n"
    << "      ************        \n"
    << "    ************          \n\n"
    << "Copyright (C) 2024-" << YEAR << " Insoft.\n"
    << "Insoft " << NAME << "\n\n";
}

void help(void) {
    using namespace std;
    std::cout
    << "Copyright (C) 2024-" << YEAR << " Insoft.\n"
    << "Insoft "<< NAME << " version, " << VERSION_NUMBER << " (BUILD " << VERSION_CODE << ")\n"
    << "\n"
    << "Usage: " << COMMAND_NAME << " <input-file> [-o <output-file>] [-w <width>] [-h <height>] [-t <tilecount>] [-s <similarity>]\n"
    << "\n"
    << "Options:\n"
    << "  -o <output-file>        Specify the filename for generated tmj code.\n"
    << "  -w  <width>             Specify the width of the tiles used.\n"
    << "  -h  <height>            Specify the height of the tiles used.\n"
    << "  -c  <tilecount>         Specify the number of tiles used.\n"
    << "  -s  <similarity>        Specify similarity percentage of tiles for matching.\n"
    << "\n"
    << "Additional Commands:\n"
    << "  " << COMMAND_NAME << " {--version | --help }\n"
    << "    --version              Display the version information.\n"
    << "    --help                 Show this help message.\n";
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
    
        in_filename = std::filesystem::expand_tilde(argv[n]);
    }
    
    if (std::filesystem::path(in_filename).parent_path().empty()) {
        in_filename.insert(0, "./");
    }
    
    info();
    
    if (!fileExists(in_filename)) {
        std::cout << MessageType::Error << "File '" << in_filename << "' not found.\n";
        return -1;
    }
    
    if (out_filename.empty() || out_filename == in_filename) {
        out_filename = std::filesystem::path(out_filename).replace_extension("");
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

