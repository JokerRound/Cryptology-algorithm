#include <cmath>
#include <vector>

#include "stdafx.h"
#include "ArrayMath.h"
#include "MacroShare.h"

// ��������� 
// *ע��*����������ģ2�ӷ�
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

// λת�ֽ�
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

// ������GF(2^8)�ϵĳ˷�
BYTE GaloisFieldMultipulication(BYTE byFirstValue,
                                BYTE bySecondValue)
{
    BYTE byResult = 0;
    for (size_t cntK = 0; cntK < 8; ++cntK)
    {
        BYTE byTmp = 0;
        // �������Ϊ1����ֱ��ģ2��
        if (byFirstValue & 0x01)
        {
            byResult ^= bySecondValue;
        }

        // �ȱ���ԭ����ֵ��Ȼ���������
        byTmp = bySecondValue;
        bySecondValue <<= 1;
        // ���ԭ�ȵ�ֵ��λΪ1�� ��������
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
    // ���ϵ���˳�����
    for (auto itorI = vctTarget.begin();
         itorI != vctTarget.end() && cntJ < NUM_STATE_COL;
         ++itorI)
    {
        aResultArray[cntI * NUM_STATE_COL + cntJ] = *itorI; 
        ++cntI;

        // ת����һ��
        if (cntI == NUM_STATE_ROW)
        {
            cntI = 0;
            ++cntJ;
        }
    }
}