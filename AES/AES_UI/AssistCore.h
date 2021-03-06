#ifdef ASSISTCORE_EXPORTS
#define ASSISTCORE_API __declspec(dllexport)
#else
#define ASSISTCORE_API __declspec(dllimport)
#endif
#ifndef ASSISTCORE_H_
#define ASSISTCORE_H_

class ASSISTCORE_API CLock
{
private:
    CRITICAL_SECTION &m_stCs;
    CCritSecLock *m_pCs = NULL;
public:
    CLock(CRITICAL_SECTION &stCs);
    ~CLock();
};

// 事件
class ASSISTCORE_API CMyEvent
{
private:
    HANDLE m_hEvent = NULL;
public:

    CMyEvent(_In_ BOOL bMaualReset = FALSE,
           _In_ BOOL bInitialState = FALSE,
           _In_ LPCTSTR lpszName = NULL,
           _In_ LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
    ~CMyEvent();

    virtual BOOL ResetEvent();
    virtual BOOL SetEvent();
    virtual DWORD Lock();
};

// 互斥体
class ASSISTCORE_API CMyMutex
{
private:
    HANDLE m_hMutex;
public:
    CMyMutex(_In_ BOOL bInitiallyOwn = FALSE,
           _In_ LPCTSTR lpszName = NULL,
           _In_ LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);
    ~CMyMutex();

    virtual DWORD Lock();
    virtual BOOL ReleaseMutex();
};

// 信号量
class ASSISTCORE_API CMySemaphore
{
private:
    HANDLE m_hSemaphore;
public:
    CMySemaphore(_In_ LONG lInitialCount = 1,
               _In_ LONG lMaxCount = 1,
               _In_ LPCTSTR pstrName = NULL,
               _In_ LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
    ~CMySemaphore();

    virtual DWORD Lock();
    virtual BOOL ReleaseSemaphore(_In_ LONG lReleaseCount = 1,
                                  _Out_ LPLONG lpPreviousCount = NULL);
};

struct tagThreadProcParameter
{
    LPVOID pClass_;
    LPVOID pParameter_;
    BOOL bStartSuccess_;
};


class ASSISTCORE_API CThread
{
private:
    // 线程句柄
    HANDLE m_hThread = INVALID_HANDLE_VALUE;
    // 线程ID
    DWORD m_dwThreadId = 0;
    
    // 线程回调函数参数
    tagThreadProcParameter m_stThreadProcParameter = { 0 };
    
    // 成功初始化事件
    CMyEvent *m_pevtThreadStartSuccess = NULL;

public:
    CThread();
    virtual ~CThread();
    
    virtual BOOL StartThread(LPVOID pParameter = NULL,
                             BOOL bInheritHandle = TRUE,
                             BOOL bIsSuspended = FALSE);

    //结束线程
    virtual BOOL ConcludeThread(DWORD dwWaitTime = INFINITE);

    //线程执行主体
    virtual bool OnThreadEventStart()
    {
        return true;
    };

    virtual bool OnThreadEventRun(LPVOID lpParam)
    {
        return true;
    };

    virtual bool OnThreadEventEnd()
    {
        return true;
    }

    static DWORD ThreadFunc(LPVOID lpParam);
};

#endif // !ASSISTCORE_H_
