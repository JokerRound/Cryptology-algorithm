#pragma once
#ifndef STRUCTSHARE_H_
#define STRUCTSHARE_H_
#include "AES_Core.h"


struct tagOperationInfo
{
    CString csSrcFile_;
    CString csDstFile_;
    tagActiveType eType_;
    tagPacketMode eMode_;
    BYTE abyMainKey_[NUM_AES_BYTE];
};

#endif // !1
