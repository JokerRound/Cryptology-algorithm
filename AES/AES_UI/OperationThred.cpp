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
        // ��Դ�ļ�
        bRet = fSrcFile.Open(pstOperationInfo->csSrcFile_.GetString(),
                             CFile::modeRead | CFile::typeBinary);
        if (!bRet)
        {
            AfxMessageBox(_T("Դ�ļ���ʧ��"));
            break;
        }

        // ��Ŀ���ļ�
        bRet = fDstFile.Open(pstOperationInfo->csDstFile_.GetString(),
                             CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
        if (!bRet)
        {
            AfxMessageBox(_T("Ŀ���ļ���ʧ��"));
            break;
        }

        // ��ȡ�ļ�����
        ULONGLONG ullFileSize = 0;
        ullFileSize = fSrcFile.GetLength();

        // �����ȡ����
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
        

        // ��¼��ǰ��ȡ��������
        ULONGLONG ullCurReadActiveNum = 0;
        // �Ѷ�ȡ���ֽ���
        ULONGLONG ullcntReadByte = 0;

        // ��ȡ������
        BYTE *abyReadBuffer = NULL;
        abyReadBuffer = new BYTE[BUFFER_BYTE_NUM + REVERSE_BYTE_NUM];
        if (abyReadBuffer == NULL)
        {
            AfxMessageBox(_T("��ȡ�������ڴ����ʧ��"));
            break;
        }

        // д�뻺����
        BYTE *abyWriteBuffer = NULL;
        abyWriteBuffer = new BYTE[BUFFER_BYTE_NUM + REVERSE_BYTE_NUM];
        if (abyWriteBuffer == NULL)
        {
            AfxMessageBox(_T("д�뻺�����ڴ����ʧ��"));
        }
        
        // ��ʼ���̲���
        while (ullCurReadActiveNum < ullReadActiveNum &&
               fSrcFile.GetLength() != fSrcFile.GetPosition())
        {
            // ����д��������0
            memset(abyReadBuffer, 0, BUFFER_BYTE_NUM + REVERSE_BYTE_NUM);
            memset(abyWriteBuffer, 0, BUFFER_BYTE_NUM + REVERSE_BYTE_NUM);
            // ���������С
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

            // ��ʼ��ȡ 
            // ����Ҫ���жϣ�����ǽ���, ����CBC������Ҫ���16���ֽ�
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
            // ��ʼ���м���/���ܲ���
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

            // ��д���ļ�
            // ע�⣬����Ҫ����
            fDstFile.Write((char *)abyWriteBuffer, dwSize);

            // ���½�����
            DWORD dwRate = 
                (DWORD)((double)ullcntReadByte / (double)ullFileSize * 1000);

            HWND hAESWnd = FindWindow(NULL, _T("AES-16λ"));
            if (hAESWnd != NULL)
            {
                ::SendMessage(hAESWnd,
                              WM_UPDATEPROCESSCTRL,
                              (WPARAM)&dwRate,
                              NULL);
            }

            // ��������
            ++ullCurReadActiveNum;
        } //! while �������� END

        // �ͷ��ڴ���Դ
        if (abyReadBuffer != NULL)
        {
            delete[] abyReadBuffer;
        }
        if (abyWriteBuffer != NULL)
        {
            delete[] abyWriteBuffer;
        }
    } while (FALSE);

    // �ļ��ر�
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
