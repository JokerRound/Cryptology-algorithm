#include <fstream>

#include "stdafx.h"
#include "OperationThred.h"
#include "AES_Core.h"
#include "MacroShare.h"

COperationThred::COperationThred()
{
}


COperationThred::~COperationThred()
{
}

bool COperationThred::OnThreadEventRun(LPVOID lpParameter)
{
    tagOperationInfo *pstOperationInfo = (tagOperationInfo *)lpParameter;
    CMyAes AES;

    CFile fSrcFile;
    CFile fDstFile;
    BOOL bRet = FALSE;
    do
    {
        // 打开源文件
        bRet = fSrcFile.Open(pstOperationInfo->csSrcFile_.GetString(),
                             CFile::modeRead | CFile::typeBinary);
        if (!bRet)
        {
            AfxMessageBox(_T("源文件打开失败"));
            break;
        }

        // 打开目标文件
        bRet = fDstFile.Open(pstOperationInfo->csDstFile_.GetString(),
                             CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
        if (!bRet)
        {
            AfxMessageBox(_T("目标文件打开失败"));
            break;
        }

        // 获取文件长度
        ULONGLONG ullFileSize = 0;
        ullFileSize = fSrcFile.GetLength();

        // 计算读取次数
        ULONGLONG ullReadActiveNum = ullFileSize / BUFFER_BYTE_NUM;
        DWORD dwRemander = (DWORD)(ullFileSize % BUFFER_BYTE_NUM);

        if (dwRemander > 0 &&
            ullReadActiveNum != 0)
        {
            ullReadActiveNum += 1;
        }

        if (ullReadActiveNum == 0)
        {
            ullReadActiveNum = 1;
        }
        

        // 记录当前读取操作轮数
        ULONGLONG ullCurReadActiveNum = 0;
        // 已读取的字节数
        ULONGLONG ullcntReadByte = 0;

        // 读取缓冲区
        BYTE *abyReadBuffer = NULL;
        abyReadBuffer = new BYTE[BUFFER_BYTE_NUM + REVERSE_BYTE_NUM];
        if (abyReadBuffer == NULL)
        {
            AfxMessageBox(_T("读取缓冲区内存分配失败"));
            break;
        }

        // 写入缓冲区
        BYTE *abyWriteBuffer = NULL;
        abyWriteBuffer = new BYTE[BUFFER_BYTE_NUM + REVERSE_BYTE_NUM];
        if (abyWriteBuffer == NULL)
        {
            AfxMessageBox(_T("写入缓冲区内存分配失败"));
        }
        
        // 开始进程操作
        while (ullCurReadActiveNum < ullReadActiveNum &&
               fSrcFile.GetLength() != fSrcFile.GetPosition())
        {
            // 将读写缓冲区置0
            memset(abyReadBuffer, 0, BUFFER_BYTE_NUM + REVERSE_BYTE_NUM);
            memset(abyWriteBuffer, 0, BUFFER_BYTE_NUM + REVERSE_BYTE_NUM);
            // 操作结果大小
            DWORD dwSize = 0;
            DWORD dwReadByteNum = 0;

            if (dwRemander > 0 && 
                ullCurReadActiveNum == ullReadActiveNum - 1)
            {
                dwReadByteNum = dwRemander;
                if (pstOperationInfo->eType_ == AT_DECRYPT)
                {
                    dwReadByteNum -=
                        (DWORD)ullCurReadActiveNum * 16u;
                }
            }
            else
            {
                dwReadByteNum = BUFFER_BYTE_NUM;
            }

            // 开始读取 
            // 这里要做判断，如果是解密, 且是CBC则这里要多读16个字节
            if (pstOperationInfo->eType_ == AT_DECRYPT &&
                pstOperationInfo->eMode_ == PM_CBC)
            {
                if (ullCurReadActiveNum != ullReadActiveNum - 1)
                {
                    dwReadByteNum += NUM_AES_BYTE;
                }
            }

            fSrcFile.Read((char *)abyReadBuffer, dwReadByteNum);
            ullcntReadByte += dwReadByteNum;

            dwSize = dwReadByteNum;
            // 开始进行加密/解密操作
            if (pstOperationInfo->eType_ == AT_ENCRYPT)
            {
                AES.Encrypt(abyReadBuffer,
                            abyWriteBuffer,
                            dwSize,
                            pstOperationInfo->abyMainKey_);
            }
            else if (pstOperationInfo->eType_ == AT_DECRYPT)
            {
                AES.Decrypt(abyReadBuffer,
                            abyWriteBuffer,
                            dwSize,
                            pstOperationInfo->abyMainKey_);
            }

            // 回写到文件
            // 注意，这里要处理
            fDstFile.Write((char *)abyWriteBuffer, dwSize);

            // 更新进度条
            DWORD dwRate = 
                (DWORD)((double)ullcntReadByte / (double)ullFileSize * 1000);

            HWND hAESWnd = FindWindow(NULL, _T("AES-16位"));
            if (hAESWnd != NULL)
            {
                ::SendMessage(hAESWnd,
                              WM_UPDATEPROCESSCTRL,
                              (WPARAM)&dwRate,
                              NULL);
            }

            // 轮数增加
            ++ullCurReadActiveNum;
        } //! while 操作结束 END

        // 释放内存资源
        if (abyReadBuffer != NULL)
        {
            delete[] abyReadBuffer;
        }
        if (abyWriteBuffer != NULL)
        {
            delete[] abyWriteBuffer;
        }
    } while (FALSE);

    // 文件关闭
    if (fSrcFile.m_hFile != CFile::hFileNull)
    {
        fSrcFile.Close();
    }

    if (fDstFile.m_hFile != CFile::hFileNull)
    {
        fDstFile.Close();
    }
    return true;
}
