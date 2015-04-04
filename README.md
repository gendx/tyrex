# Tyrex - the versatile file decoder

An hexadecimal file reader with extensions to decode lots of file formats. [![Build Status](https://travis-ci.org/gendx/tyrex.svg?branch=master)](https://travis-ci.org/gendx/tyrex)

## Installation

Tyrex is written in C++11 with Qt4 graphical interface. You can build it with QtCreator or follow instructions written for Travic-CI.

## Supported file formats

Work is still in progress and new decoders are coming soon !

### Already available

Type | Format | Status
---|---|---
compression | **deflate, gzip, zlib** | beta
compression | **lzma, lzma2** | beta
compression | **bzip2** | beta
compression | **lzw** | beta
compression | **xz** | lzma filter only
archive | **zip** | encryption and some compression algorithms are missing
archive | **tar** | basic features
image | **png** | some interlaced and bit depth are missing
font | **freetype** | basic features only
program | **java** | extraction of symbols
program | **elf** | basic features

### Coming soon

* document : **pdf**
* sound : **midi**

## Other tools

Tool | Methods
---|---
hash function | **CRC (32, 64), SHA (256)**

## Contribute

Feel free to fork this project on Github, report bugs and make pull requests !

Here is an overview of the code.

```
icons/                  : icons
src/                    : source code
    data/               : representation of decoded files
    external/           : external resources
    graphic/            : graphical user interface
        view/           : graphical representation of data
    misc/               : miscellaneous
    parse/              : parsers to decode file formats
    platform-specific/  : code specific to each platform
```

To implement a new file format, you need to provide three parts.
* An abstract model of the data represented by this format. Write a new class that inherits `tyrex::data::Data` in the folder `src/data/`.
* A graphical view to show the data. Subclass `tyrex::graphic::View` in `src/graphic/view`.
* A parser to convert encoded bytes into the data model. Subclass `tyrex::parse::Parser` in `src/parse/`.
Note that you can reuse models and views for several parsers. For example, image files will be converted into matrices of pixels.

Then, you will need to bind them together into the program. Add entries in class `tyrex::parse::Document` and `tyrex::graphic::TypeSelector` (in `src/graphic/dialog/`). You can also specify the *magic* of your format in `tyrex::parse::Document` to quickly determine the type of unknown files.

## License

*Copyright (C) 2014 - 2015  G. Endignoux*

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see [www.gnu.org/licenses/gpl-3.0.txt](http://www.gnu.org/licenses/gpl-3.0.txt).

