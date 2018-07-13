#include <cmath>
#include <vector>

#include "stdafx.h"
#include "ArrayMath.h"
#include "MacroShare.h"

// 矩阵乘向量 
// *注意*这里做的是模2加法
void ArrayVectorMultiplication(BYTE abyFirstArray[], 
                               BYTE abySecondVector[])
{
    BYTE abyResult[8] = { 0 };
    for (size_t cntI = 0; cntI < 8; ++cntI)
    {
        BYTE byTmpBitDigit = 0;
        for (size_t cntJ = 0; cntJ < 8; ++cntJ)
        {
            byTmpBitDigit ^=
                abyFirstArray[cntI * 8 + cntJ] * abySecondVector[cntJ];
        }

        abyResult[cntI] = byTmpBitDigit;
    }

    for (size_t cntI = 0; cntI < 8; cntI++)
    {
        abySecondVector[cntI] = abyResult[cntI];
    }
}

// 位转字节
BYTE BitDigitArrayToByte(BYTE aTargetArray[])
{
    BYTE byResult = 0;
    for (size_t cntI = 0; cntI < 8; ++cntI)
    {
        byResult += (BYTE)(aTargetArray[cntI] * (BYTE)pow(2, cntI));
    }

    return byResult;
}

void ByteToBitDigitArray(const BYTE &byTarget, BYTE aResultArray[])
{
    BYTE byMask = 0x80;
    for (int cntK = 7; cntK >= 0; --cntK, byMask >>= 1)
    {
        if (byTarget & byMask)
        {
            aResultArray[cntK] = 1;
        }
        else
        {
            aResultArray[cntK] = 0;
        }
    }
}


void ArrayAdd(BYTE aFirstArray[],
              BYTE aSecondArray[])
{
    for (size_t cntI = 0; cntI < 8; ++cntI)
    {
        aSecondArray[cntI] ^= aFirstArray[cntI];
    }
}

// 有限域GF(2^8)上的乘法
BYTE GaloisFieldMultipulication(BYTE byFirstValue,
                                BYTE bySecondValue)
{
    BYTE byResult = 0;
    for (size_t cntK = 0; cntK < 8; ++cntK)
    {
        BYTE byTmp = 0;
        // 如果乘数为1，则直接模2加
        if (byFirstValue & 0x01)
        {
            byResult ^= bySecondValue;
        }

        // 先保存原来的值，然后进行左移
        byTmp = bySecondValue;
        bySecondValue <<= 1;
        // 如果原先的值高位为1， 则进行异或
        if ((byTmp & 0x80) != 0)
        {
            bySecondValue ^= 0x1b;
        }

        byFirstValue >>= 1;
    }

    return byResult;
}

void NormalChangeToArray(const std::vector<BYTE> &vctTarget,
                         BYTE aResultArray[])
{
    size_t cntI = 0;
    for (auto itorI = vctTarget.begin();
         itorI != vctTarget.end();
         ++itorI, ++cntI)
    {
        aResultArray[cntI] = *itorI;
    }
}

void ChangeToArray(const std::vector<BYTE> &vctTarget,
                  BYTE aResultArray[])
{
    size_t cntI = 0;
    size_t cntJ = 0;
    // 从上到下顺序放置
    for (auto itorI = vctTarget.begin();
         itorI != vctTarget.end() && cntJ < NUM_STATE_COL;
         ++itorI)
    {
        aResultArray[cntI * NUM_STATE_COL + cntJ] = *itorI; 
        ++cntI;

        // 转向下一列
        if (cntI == NUM_STATE_ROW)
        {
            cntI = 0;
            ++cntJ;
        }
    }
}