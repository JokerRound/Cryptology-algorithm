#pragma once
#ifndef ARRAYMATH_H_
#define ARRAYMATH_H_
#include <vector>
#include "stdafx.h"

void ArrayVectorMultiplication(_In_ BYTE aFirstArray[], 
                               _In_ _Out_ BYTE aSecondArray[]);

BYTE GaloisFieldMultipulication(BYTE byFirstValue,
                                BYTE bySecondValue);



// 矩阵乘矩阵 
template <typename Type, size_t N1, size_t N2>
BOOL ArrayArrayMultiplication(Type aFirstArray[],
                              size_t uiFirstRowNum,
                              Type aSecondArray[],
                              size_t uiSecondRowNum,
                              std::vector<Type> &vctResult)
{
    size_t uiFirstSize = N1 / sizeof(aFirstArray[0]);
    size_t uiFirstColNum = uiFirstSize / uiFirstRowNum;

    size_t uiSecondSize = N2 / sizeof(aSecondArray[0]);
    size_t uiSecondColNum = uiSecondSize / uiSecondRowNum;

    // 当矩阵A的列数等于矩阵B的行数时，两矩阵可相乘
    if (uiFirstColNum != uiSecondRowNum)
    {
        return FALSE;
    }

    for (size_t cntI = 0; cntI < uiFirstRowNum; ++cntI)
    {
        for (size_t cntJ = 0; cntJ < uiSecondColNum; ++cntJ)
        {
            unsigned short usTmp = 0;
            BYTE byTmp = 0;
            BYTE byResultValue = 0;

            

            for (size_t cntK = 0; cntK < uiSecondRowNum; ++cntK)
            {
                BYTE byFirstValue =
                    aFirstArray[cntI * uiFirstColNum + cntK];
                BYTE bySecondValue =
                    aSecondArray[cntK * uiSecondRowNum + cntJ];
                byResultValue ^= GaloisFieldMultipulication(byFirstValue,
                                                           bySecondValue);
            }

            vctResult.push_back(byResultValue);
        }
    }

    return TRUE;
} //! ArrayArrayMultiplication END 

BYTE BitDigitArrayToByte(BYTE aTargetArray[]);

void ByteToBitDigitArray(const BYTE & byTarget, BYTE aResultArray[]);


void ArrayAdd(_In_ BYTE aFirstArray[], _In_ _Out_ BYTE aSecondArray[]);

void ChangeToArray(const std::vector<BYTE> &vctTarget,
                   BYTE aResultArray[]);

void NormalChangeToArray(const std::vector<BYTE> &vctTarget,
                         BYTE aResultArray[]);

#endif // !ARRAYMATH_H_

