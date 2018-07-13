#pragma once
#ifndef MACROSHARE_H_
#define MACROSHARE_H_

// 说明性宏
#define NUM_AES_BYTE 16u
#define NUM_STATE_COL 4u
#define NUM_STATE_ROW 4u
#define NUM_BYTE_DIGIT 8u
#define NUM_ROUND 10u

// 功能性宏
#define MAKE_BYTE(x, y) (((x) << 4) | (y))
#define HISHORT(x) ((x) >> 8)
#define LOSHORT(y) ((y) & 0x00ff)
#define MAKE_WORD(i, j, k, x) (((i) << 24) | ((j) << 16) | ((k) << 8) | (x))
#define SWAP(x, y, t) ((t) = (x), (x) = (y), (y) = (t))
#endif // !MACROSHARE_H_
