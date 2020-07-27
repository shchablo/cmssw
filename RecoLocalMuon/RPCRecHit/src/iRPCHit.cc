/*!
\file
\brief Body file with definitions of iRPCHit functions.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/
#include "iRPCHit.h"

iRPCHit::iRPCHit() { _channel = -1; _strip = -1; _time = -1; _isHR = false; _isLR = false; }
iRPCHit::iRPCHit(int channel, int strip, float time) :
                 _channel(channel), _strip(strip), _time(time) { _isHR = false; _isLR = false; }
iRPCHit::iRPCHit(int channel, int strip, float time, int bunchx) :
                 _channel(channel), _strip(strip), _time(time), _bunchx(bunchx) { _isHR = false; _isLR = false; }
iRPCHit::iRPCHit(int channel, int strip, float time, int bunchx, float digiY) :
                 _channel(channel), _strip(strip), _time(time), _bunchx(bunchx), _digiY{digiY} { _isHR = false; _isLR = false; }
iRPCHit::~iRPCHit() {}

int iRPCHit::bx() { return _bunchx; }
int iRPCHit::strip() { return _strip; }
int iRPCHit::channel() { return _channel; }
float iRPCHit::time() { return _time; }
float iRPCHit::digiY() { return _digiY; }

void iRPCHit::setHR(bool is) { _isHR = is; };
void iRPCHit::setLR(bool is) { _isLR = is; };

bool iRPCHit::isHR() { return _isHR; };
bool iRPCHit::isLR() { return _isLR; };
