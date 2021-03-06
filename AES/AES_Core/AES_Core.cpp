// AES_Core.cpp: 定义 DLL 应用程序的导出函数。
//

#include <vector>

#include "stdafx.h"
#include "AES_Core.h"
#include "ArrayMath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 更改错误代码以符合您的需要
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此处为应用程序的行为编写代码。
        }
    }
    else
    {
        // TODO: 更改错误代码以符合您的需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}




typedef BYTE(*PA_DATA)[NUM_AES_BYTE];

// 字节替换
void CMyAes::SubBytes(BYTE abyTarget[NUM_AES_BYTE], tagActiveType eType)
{
    PASBOX apSBox[2] = { &m_abyEncryptSbox, &m_abyDecryptSbox };
    PASBOX pSBoxUsed = apSBox[eType];

    for (DWORD cntI = 0; cntI < NUM_AES_BYTE; cntI++)
    {
        // 获取行和列。高位为行，低位为列
        BYTE uiRow = HIBYTE(abyTarget[cntI]);
        BYTE uiCol = LOBYTE(abyTarget[cntI]); 

        // 使用S盒中对应值做替换
        abyTarget[cntI] = (*pSBoxUsed)[uiRow][uiCol];
    }
}

// 移位
void CMyAes::ShiftRow(BYTE abyTarget[NUM_AES_BYTE],  tagActiveType eType)
{
    BYTE abyResult[NUM_AES_BYTE] = { 0 };
    for (int cntI = 0; cntI < NUM_AES_BYTE / 4; cntI++)
    {
        
        int iOffset = cntI;
        int iMoveNum = iOffset;
        if (iMoveNum > 2)
        {
            iMoveNum -= 4;
        }
        
        // 加密向左移，解密向右移
        eType ? iOffset = -iOffset : iOffset;

        for (int cntJ = 0; cntJ < (NUM_AES_BYTE / 4) ; cntJ++)
        {
            if (cntI == 0)
            {
                abyResult[cntI * 4 + cntJ] = abyTarget[cntI * 4 + cntJ];
            }

            int cntNewJ = (cntJ - iOffset) % 4;
            cntNewJ < 0 ? cntNewJ += 4 : cntNewJ;
            abyResult[cntI * 4 + cntNewJ] = abyTarget[cntI * 4 + cntJ];
        }
    }

    for (size_t cntI = 0; cntI < NUM_AES_BYTE; cntI++)
    {
        abyTarget[cntI] = abyResult[cntI];
    }
}

// 列混淆
void CMyAes::MixColumn(BYTE abyTarget[NUM_AES_BYTE], tagActiveType eType)
{
    // 定义加密常量矩阵
    BYTE abyEnConstant[NUM_AES_BYTE] = {
        0x02, 0x03, 0x01, 0x01,
        0x01, 0x02, 0x03, 0x01,
        0x01, 0x01, 0x02, 0x03,
        0x03, 0x01, 0x01, 0x02,
    };

    // 定义解密常量矩阵
    BYTE abyDeConstant[NUM_AES_BYTE] = {
        0x0e, 0x0b, 0x0d, 0x09,
        0x09, 0x0e, 0x0b, 0x0d,
        0x0d, 0x09, 0x0e, 0x0b,
        0x0b, 0x0d, 0x09, 0x0e,
    };

    // 根据操作类型决定使用哪个矩阵
    PACONSTANT abyConstant[2] = { &abyEnConstant, &abyDeConstant };
    PACONSTANT abyCurrentConstant = abyConstant[eType];

    // 进行矩阵与矩阵的乘法运算
    std::vector<BYTE> vctResult;
    ArrayArrayMultiplication<BYTE, NUM_AES_BYTE, NUM_AES_BYTE>(
        (BYTE *)(*abyCurrentConstant),
        NUM_AES_BYTE / 4,
        abyTarget,
        NUM_AES_BYTE / 4,
        vctResult);

    size_t uiIndex = 0;
    for (auto itorI = vctResult.begin();
         itorI != vctResult.end();
         ++itorI)
    {
        abyTarget[uiIndex] = *itorI;
        ++uiIndex;
    }
} //! CMyAes::MixColumn END

// 轮密钥加，与子密钥做模二加法
void CMyAes::AddRoundKey(BYTE abyTarget[NUM_AES_BYTE], DWORD dwRoundNum)
{
    // 获取轮密钥
    DWORD *pbyKeyElement = (DWORD *)m_adwSubKey + dwRoundNum * NUM_STATE_ROW;

    for (size_t cntI = 0; cntI < NUM_AES_BYTE / NUM_STATE_ROW; ++cntI)
    {
        // 密钥字转字节
        BYTE abyRoundKey[4] = { 0 };
        short dwHigthWord = HIWORD(pbyKeyElement[cntI]);
        short dwLowerWrod = LOWORD(pbyKeyElement[cntI]);
        // 0 为高位, 3为低位。将最高位移到最低位
        abyRoundKey[0] = HISHORT(dwHigthWord); 
        abyRoundKey[1] = LOSHORT(dwHigthWord);
        abyRoundKey[2] = HISHORT(dwLowerWrod);
        abyRoundKey[3] = LOSHORT(dwLowerWrod);
        for (size_t cntJ = 0; cntJ < NUM_AES_BYTE / NUM_STATE_ROW; ++cntJ)
        {
            abyTarget[cntI + NUM_STATE_ROW * cntJ] ^= abyRoundKey[cntJ];
        }
    }
}

// 密钥扩展 （正确性已验证）
void CMyAes::KeyExpansion(BYTE abyKey[NUM_AES_BYTE])
{
    // 将密钥先放到扩展密钥组的前4个字节
    for (size_t cntI = 0; cntI < 4; ++cntI)
    {
        m_adwSubKey[cntI] = MAKE_WORD(abyKey[cntI ],
                                      abyKey[cntI + 1*4],
                                      abyKey[cntI + 2*4],
                                      abyKey[cntI + 3*4]);
    }

    for (size_t cntI = 4; cntI < 44; ++cntI)
    {
        DWORD dwTmpWord = m_adwSubKey[cntI - 1];
        if (cntI % 4 == 0)
        {
            dwTmpWord = OperationG(dwTmpWord, AT_ENCRYPT, cntI / 4);
        }

        m_adwSubKey[cntI] = m_adwSubKey[cntI - 4] ^ dwTmpWord;
        printf("%08x\r\n", m_adwSubKey[cntI]);
    }
}

DWORD CMyAes::OperationG(DWORD dwTarget, 
                         tagActiveType eType,
                         DWORD dwRoundNum)
{
    DWORD dwResult = 0;
    // 转换成字节, 同时进行移位
    BYTE abyTarget[4] = { 0 };
    short dwHigthWord = HIWORD(dwTarget);
    short dwLowerWrod = LOWORD(dwTarget);
    // 0 为高位, 3为低位。将最高位移到最低位
    abyTarget[0] = LOSHORT(dwHigthWord);
    abyTarget[1] = HISHORT(dwLowerWrod);
    abyTarget[2] = LOSHORT(dwLowerWrod);
    abyTarget[3] = HISHORT(dwHigthWord);

    // S盒替换
    PASBOX apSBox[2] = { &m_abyEncryptSbox, &m_abyDecryptSbox };
    PASBOX pSBoxUsed = apSBox[eType];
    for (size_t cntI = 0; cntI < 4; cntI++)
    {
        // 获取行和列。高位为行，低位为列
        BYTE uiRow = HIBYTE(abyTarget[cntI]);
        BYTE uiCol = LOBYTE(abyTarget[cntI]); 

        // 使用S盒中对应值做替换
        abyTarget[cntI] = (*pSBoxUsed)[uiRow][uiCol]; 
    }

    // 轮常量：
    BYTE aabyRoundConstant[NUM_ROUND][4] = {
        {0x01, 0x00, 0x00, 0x00},
        {0x02, 0x00, 0x00, 0x00},
        {0x04, 0x00, 0x00, 0x00},
        {0x08, 0x00, 0x00, 0x00},
        {0x10, 0x00, 0x00, 0x00},
        {0x20, 0x00, 0x00, 0x00},
        {0x40, 0x00, 0x00, 0x00},
        {0x80, 0x00, 0x00, 0x00},
        {0x1B, 0x00, 0x00, 0x00},
        {0x36, 0x00, 0x00, 0x00},
    };

    // 与轮常量异或
    for (size_t cntI = 0; cntI < 4; ++cntI)
    {
        abyTarget[cntI] ^= aabyRoundConstant[dwRoundNum - 1][cntI];
    }

    // 得到结果, 转回字节
    dwResult = MAKE_WORD(abyTarget[0], 
                         abyTarget[1],
                         abyTarget[2], 
                         abyTarget[3]);

    return dwResult;
}


CMyAes::CMyAes()
{
}


CMyAes::~CMyAes()
{
}

void CMyAes::Encrypt(LPBYTE lpTargetData, 
                     LPBYTE lpBuffer,
                     DWORD &dwSize,
                     BYTE abyMainkey[NUM_AES_BYTE])
{
    // 分隔存储区
    std::vector<std::vector<BYTE> > vctData;

    // 缓冲区写入位置
    DWORD dwWritePos = 0;
    // 分隔字符串
    CBCSeparateData(lpTargetData, dwSize, vctData);
    

    // 密钥扩展
    KeyExpansion(abyMainkey);

    // 遍历总vector容器，实现分段加密
    for (auto itorI = vctData.begin();
         itorI != vctData.end();
         ++itorI)
    {
        // 先把vector中的数据拷贝到临时数组中
        // *注意* 该数组在11轮循环后存储的是密文
        BYTE abyClearData[NUM_AES_BYTE] = { 0 };
        ChangeToArray(*itorI, abyClearData);

        // 总循环为11轮
        for (size_t cntI = 0; cntI < NUM_ROUND + 1; ++cntI)
        {
            // 先密钥轮加 
            if (cntI == 0)
            {
                AddRoundKey(abyClearData, cntI);
                continue;
            }

            SubBytes(abyClearData, AT_ENCRYPT);
            ShiftRow(abyClearData, AT_ENCRYPT);

            // 最后一轮不再进行列混淆
            if (cntI < NUM_ROUND)
            {
                MixColumn(abyClearData, AT_ENCRYPT);
            }

            AddRoundKey(abyClearData, cntI);
        } //! for 11轮循环 END 

        // 将加密完的内容复制到输出缓冲区
        for (size_t cntI = 0; 
             cntI < itorI->size() && dwWritePos < dwSize; 
             ++cntI, ++dwWritePos)
        {
            lpBuffer[dwWritePos] = abyClearData[cntI];
        }
    } //! for 遍历总vector容器 END
} //! Encrypt END

// 解密
void CMyAes::Decrypt(LPBYTE lpTargetData,
                     LPBYTE lpBuffer,
                     DWORD &dwSize,
                     BYTE abyMainKey[NUM_AES_BYTE])
{
    // 分隔存储区
    std::vector<std::vector<BYTE> > vctData;

    // 缓冲区写入位置
    DWORD dwWritePos = 0;
    // 分隔字符串
    SeparateData(lpTargetData, dwSize, vctData);

    // 密钥扩展
    KeyExpansion(abyMainKey);

    // 遍历总vector容器，实现分段解密
    for (auto itorI = vctData.begin();
         itorI != vctData.end();
         ++itorI)
    {
        // 先把vector中的数据拷贝到临时数组中
        // *注意* 该数组在11轮循环后存储的是密文
        BYTE abyTmpCipherData[NUM_AES_BYTE] = { 0 };
        NormalChangeToArray(*itorI, abyTmpCipherData);

        // 总循环为11轮
        for (size_t cntI = 0; cntI < NUM_ROUND + 1; ++cntI)
        {
            // 第0轮只密钥轮加 
            if (cntI == 0)
            {
                AddRoundKey(abyTmpCipherData, NUM_ROUND - cntI);
                continue;
            }

            ShiftRow(abyTmpCipherData, AT_DECRYPT);
            SubBytes(abyTmpCipherData, AT_DECRYPT);
            AddRoundKey(abyTmpCipherData, NUM_ROUND - cntI);
            // 最后一个轮不再进行列混淆
            if (cntI < NUM_ROUND)
            {
                MixColumn(abyTmpCipherData, AT_DECRYPT);
            }
        } //! for 11轮循环 END 

        // *注意* 此时解密完的数组中内容是从上到下按列排序的
        // 需要恢复到原先的排列顺序
        BYTE abyCipherData[NUM_AES_BYTE] = { 0 };
        for (size_t cntI = 0; cntI < NUM_STATE_ROW; ++cntI)
        {
            for (size_t cntJ = 0; cntJ < NUM_STATE_COL; ++cntJ)
            {
                abyCipherData[cntI * NUM_STATE_ROW + cntJ] =
                    abyTmpCipherData[cntI + cntJ * NUM_STATE_COL];
            }
        }

        // 将解密密完的内容复制到输出缓冲区
        for (size_t cntI = 0;
             cntI < itorI->size() && dwWritePos < dwSize;
             ++cntI, ++dwWritePos)
        {
            // 如果是最后一个段，则计算解密后的数据的实际长度
            if (itorI == (vctData.end() - 1))
            {
                dwSize -= (abyCipherData[cntI] + NUM_AES_BYTE);
                break;
            }

            lpBuffer[dwWritePos] = abyCipherData[cntI];
        }
    } //! for 遍历总vector容器 END
}

// CBC电子密码本分隔模式
BOOL CMyAes::CBCSeparateData(LPBYTE lpTargetData,
                          DWORD &dwSize,
                          std::vector<std::vector<BYTE> > &vctTargetData)
{
    SeparateData(lpTargetData, dwSize, vctTargetData);

    // 判断最后一个段是否是16位, 是的话进行补齐
    DWORD dwPendNum = 0;
    auto itorEnd = vctTargetData.end() - 1;
    if (itorEnd->size() < NUM_AES_BYTE)
    {
        // 计算差异
        dwPendNum = NUM_AES_BYTE - itorEnd->size();
        for (size_t cntI = 1; cntI <= dwPendNum; ++cntI)
        {
            itorEnd->push_back((BYTE)cntI);
        }
    }

    // 再增加一个段，记录实际最后一个段的填充数量
    // *注意*加密时增加了一个段，则解密时要多读一个段
    std::vector<BYTE> vctTmp;
    vctTmp.clear();
    vctTmp.push_back((BYTE)dwPendNum);
    for (size_t cntI = 0; cntI < NUM_AES_BYTE - 1; ++cntI)
    {
        vctTmp.push_back((BYTE)cntI);
    }
    vctTargetData.push_back(vctTmp);

    // 计算总长度数量
    dwSize += (dwPendNum + NUM_AES_BYTE);

    return TRUE;
}

BOOL CMyAes::SeparateData(LPBYTE lpTargetData,
                          DWORD &dwSize,
                          std::vector<std::vector<BYTE> > &vctTargetData)
{
    LPBYTE lpTmpTargetData = lpTargetData;
    std::vector<BYTE> vctTmp;

    // 分隔
    int cntJ = 0;
    for (int cntI = 0; 
         cntI < (int)dwSize;
         ++cntI, ++cntJ)
    {
        vctTmp.push_back(*lpTmpTargetData);

        if (cntJ == NUM_AES_BYTE - 1)
        {
            // 满NUM_AES_BYTE个元素后，加入到总vector中
            vctTargetData.push_back(vctTmp);
            // 临时vector清空
            vctTmp.clear();
            cntJ = -1;
        }

        ++lpTmpTargetData;
    }
    // 最后一个段加到总vector中
    if (cntJ != 0)
    {
        vctTargetData.push_back(vctTmp);
    }

    return TRUE;
}
