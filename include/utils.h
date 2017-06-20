/*
 *  Copyright (c) 2014 Álan Crístoffer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#ifndef UTILS_H
#define UTILS_H

typedef uint8_t bool;
typedef uint8_t boolean;
typedef uint8_t byte;

#define true  0x1
#define false 0x0

#define _bv(num)             (1 << num)                                // Integer to binary shift
#define sbi(reg, bit)        ( reg |= _bv(bit) )                       // Set bit
#define cbi(reg, bit)        ( reg &= ~_bv(bit) )                      // Clear bit
#define tbi(reg, bit)        ( reg ^= _bv(bit) )                       // Toggle bit
#define wbi(reg, bit, value) ( value ? sbi(reg, bit) : cbi(reg, bit) ) // Write bit
#define rbi(reg, bit)        ( (reg >> bit) & 0x1 )                    // Read bit

#define PI         3.1415926535897932384626433832795028841971693993751
#define DEG_TO_RAD 0.0174532925199432957692369076848861271344287188854
#define RAD_TO_DEG 57.295779513082320876798154814105170332405472466564

#ifdef abs
 #undef abs
#endif

#define min(a, b)                                    ( (a) < (b) ? (a) : (b) )
#define max(a, b)                                    ( (a) > (b) ? (a) : (b) )
#define abs(x)                                       ( (x) > 0 ? (x) : -(x) )
#define constrain(amt, low, high)                    ( (amt) < (low) ? (low) : ( (amt) > (high) ? (high) : (amt) ) )
#define round(x)                                     ( (x) >= 0 ? (long) ( (x) + 0.5 ) : (long) ( (x) - 0.5 ) )
#define radians(deg)                                 ( (deg) * DEG_TO_RAD )
#define degrees(rad)                                 ( (rad) * RAD_TO_DEG )
#define map(value, in_min, in_max, out_min, out_max) ( (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min )
#endif
