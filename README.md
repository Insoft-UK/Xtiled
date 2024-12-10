<img src="../../../Insoft-UK/blob/main/assets/logo.svg" width="80" height="80" />

# TMJ File Generator

A command-line tool that takes an image composed of tiles and generates a TMJ (Tiled Map Editor JSON) file. This utility automates the process of creating a tileset and map data from a tile-based image, making it easy to integrate into Tiled-based workflows for game development and level design.

Features
    •    Automated Tileset Creation: Extracts unique tiles from the input image.
    •    Map Layout Generation: Builds a map structure representing the arrangement of tiles in the input image.
    •    TMJ File Output: Outputs a ready-to-use TMJ file compatible with the Tiled Map Editor.

Usage

Run the tool via the command line, specifying the input image and output file:

xtiled input-image.png output-file.tmj

Example

xtiled level.png level.tmj

This command processes the level.png image, generates a tileset and map data, and saves them as a level.tmj file.

Requirements
    •    A tile-based input image (e.g., PNG).
    •    Compatible with standard TMJ file specifications.

Why Use This Tool?

This tool streamlines the workflow of creating TMJ files directly from graphical assets, eliminating manual tileset creation and enabling faster iteration for developers and designers.

> [!NOTE]
The only image file format currently supported by this utility tool is the Portable Network Graphic (PNG) format.
