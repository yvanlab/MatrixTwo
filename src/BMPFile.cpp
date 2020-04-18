/*
 * Copyright (c) 2014, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <BMPFile.h>
#include "main.h"


uint8_t BMPFile::loadBitmap(String fileName) {

    DEBUGLOGF("loadBitmap[%s]\n", fileName.c_str());
    if (!SPIFFS.exists(fileName.c_str()))   {
        DEBUGLOGF("loadBitmap[%s] does not exist\n", fileName.c_str());
        return 1;
    }

    _file = SPIFFS.open(fileName.c_str(), "r");
    uint8_t res = loadImageHeader();
    if (res != 0 ) {
        _file.close();
        return res;
    }
    _buffer = new uint16_t[_width*_height];
    loadRGB565();
    DEBUGLOGF("loadBitmap[%s][%d]\n", _file.name(), _file.size());
    return 0;
}


uint8_t BMPFile::loadImageHeader() {
    _file.seek(0);
    if (_file.read((uint8_t *)&_header, sizeof(struct BitmapFileHeader)) != sizeof(struct BitmapFileHeader)) {
        _width = 0;
        _height = 0;
        return 1;
    }
    DEBUGLOGF("Read loadImageHeader size [%d]\n", _header.bfSize);
    
    if (_file.read((uint8_t *)&_info, sizeof(struct BitmapInfoHeader)) != sizeof(struct BitmapInfoHeader)) {
        _width = 0;
        _height = 0;
        return 2;
    }
    DEBUGLOGF("Read loadImageHeader size [%d]\n", _info.biSizeImage);

    DEBUGLOGF("Read biBitCount size [%d]\n", _info.biBitCount);
   /* if (_info.biBitCount < 16) {
        _file.seek(sizeof(struct BitmapFileHeader) + _info.biSize);
        int _paletteSize = _info.biClrUsed;
        if (_paletteSize == 0) {
            _paletteSize = 1<<_info.biBitCount;
        }

        for (int i = 0; i < _paletteSize; i++) {
            _file.read((uint8_t *)&_palette[i], sizeof(struct BitmapPixel32));
        }
    }*/
    
    _file.seek(_header.bfBitmapOffset);
    _spos = _header.bfBitmapOffset;

    _width = _info.biWidth;
    _height = _info.biHeight;
    DEBUGLOGF("BMP sie X;Y [%d,%d]\n", _width, _height);
    
    switch (_info.biBitCount) {
        case 8:
            _chunkSize = _width;
            break;
        case 16:
            _chunkSize = _width * 2;
            break;
        case 24:
            _chunkSize = _width * 3;
            break;
        case 32:
            _chunkSize = _width * 4;
    }
    _chunkSize = (_chunkSize + 3) & (~3);
    return 0;
}

void BMPFile::getScanLine(int line, uint8_t *data) {
    uint32_t eof = _header.bfBitmapOffset + (getHeight() * _chunkSize);
    uint32_t pos = eof - ((line + 1) * _chunkSize);
    if (_spos != pos) {
        _file.seek(pos);
    }
    _file.read(data, _chunkSize);
    _spos += _chunkSize;
}


void BMPFile::loadRGB565() {
    uint16_t cpt =0;
    uint8_t data[_chunkSize];
    for (int iy = 0; iy < getHeight(); iy++) {
        getScanLine(iy, data);
        for (int ix = 0; ix < getWidth(); ix++) {
            uint32_t offset = ix * 2;
            uint16_t p = (data[offset+1] << 8) | data[offset];
            _buffer[cpt] = p;
            cpt++;
        }
    }
}

/*
void BMPFile::loadRGB(DisplayCore *dev, int x, int y, int32_t trans) {
    uint8_t data[_chunkSize];
    if (trans < 0) {
        dev->openWindow(x, y, getWidth(), getHeight());
    }
    for (int iy = 0; iy < getHeight(); iy++) {
        getScanLine(iy, data);
        for (int ix = 0; ix < getWidth(); ix++) {
            uint32_t offset = ix * 3;
            struct BitmapPixel24 *p = (struct BitmapPixel24 *)(data + offset);
            color_t col = rgb(p->r, p->g, p->b);
            if (trans < 0) {
                dev->windowData(col);
            } else {
                if (col != trans) {
                    dev->setPixel(x + ix, y + iy, col);
                }
            }
        }
    }
    if (trans < 0) {
        dev->closeWindow();
    }
}
*/


int BMPFile::getWidth() {
    if (_width == 0) {
        loadImageHeader();
    }
    return _width;
}

int BMPFile::getHeight() {
    if (_height == 0) {
        loadImageHeader();
    }
    return _height;
}


