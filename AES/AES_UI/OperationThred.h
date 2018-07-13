#pragma once
#ifndef OPERATIONTHREAD_H_
#define OPERATIONTHREAD_H_
#include "AssistCore.h"
#include "StructShare.h"



class COperationThred : public CThread
{
private:
    HANDLE m_hSrcFile = INVALID_HANDLE_VALUE;
    HANDLE m_hDstFile = INVALID_HANDLE_VALUE;
public:
    COperationThred();
    virtual ~COperationThred();

    virtual bool OnThreadEventRun(LPVOID lpParameter);
};

#endif // !OPERATIONTHREAD_H_
