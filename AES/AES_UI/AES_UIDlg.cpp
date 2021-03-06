
// AES_UIDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "AES_UI.h"
#include "AES_UIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAESUIDlg 对话框



CAESUIDlg::CAESUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_AES_UI_DIALOG, pParent)
    , m_iOperationType(0)
    , m_iPacketMode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAESUIDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDT_KEY, m_edtKey);
    DDX_Control(pDX, IDC_EDT_SRCFILE_PATH, m_edtSrcFilePath);
    DDX_Control(pDX, IDC_EDT_DSTFILE_PATH, m_edtDstFilePath);
    DDX_Control(pDX, IDC_PRG_OPERATE_RATE, m_prgOperateRate);
    DDX_Control(pDX, IDC_BTN_BEGIN, m_btnBegin);
}

BEGIN_MESSAGE_MAP(CAESUIDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_BEGIN, &CAESUIDlg::OnBnClickedBtnBegin)
    ON_BN_CLICKED(IDC_BTN_SRCFILE_BROWSE, &CAESUIDlg::OnBnClickedBtnSrcfileBrowse)
    ON_BN_CLICKED(IDC_BTN_DSTFILE_BROWSE, &CAESUIDlg::OnBnClickedBtnDstfileBrowse)
    ON_BN_CLICKED(IDC_RAD_TYPE_ENCRYPT, &CAESUIDlg::OnBnClickedRadModeEncrypt)
    ON_BN_CLICKED(IDC_RAD_TYPE_DECRYPT, &CAESUIDlg::OnBnClickedRadModeDecrypt)
    ON_BN_CLICKED(IDC_RAD_PACKETMODE_CBC, &CAESUIDlg::OnBnClickedRadPacketmodeCbc)
    ON_MESSAGE(WM_UPDATEPROCESSCTRL, &CAESUIDlg::OnUpdateProcessCtrl)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAESUIDlg 消息处理程序

BOOL CAESUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    SetDlgItemText(IDC_STC_OPERATION_RESULT, _T(""));
    m_prgOperateRate.SetRange(0, 1000);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAESUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAESUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAESUIDlg::OnBnClickedBtnBegin()
{
    // TODO: 在此添加控件通知处理程序代码
    // 将开始按钮设置为不可用
    m_btnBegin.EnableWindow(0);
    // 将进度条置空
    m_prgOperateRate.SetPos(0);

    // 获取操作类型
    m_stOperationInfo.eType_ = (tagActiveType)m_iOperationType;
    // 获取分组模式
    m_stOperationInfo.eMode_ = (tagPacketMode)m_iPacketMode;
    // 获取源目文件
    m_edtSrcFilePath.GetWindowText(m_stOperationInfo.csSrcFile_);
    m_edtDstFilePath.GetWindowText(m_stOperationInfo.csDstFile_);

    GetDlgItemTextA(this->m_hWnd, 
                    IDC_EDT_KEY,
                    (LPSTR)m_stOperationInfo.abyMainKey_,
                    NUM_AES_BYTE + 1);

    SetDlgItemText(IDC_STC_OPERATION_RESULT, _T("开始操作.."));
 
    // 开启工作线程
    m_pthdOperator = new COperationThred();
    if (m_pthdOperator != NULL)
    {
        m_pthdOperator->StartThread(&m_stOperationInfo);
    }
}


void CAESUIDlg::OnBnClickedBtnSrcfileBrowse()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog dlgFile(TRUE);
    CString csFileName;
    // 设置缓冲区大小
    // 单个文件名长度
    const int kcntMaxFiles = 100;
    // 应对多个文件选中的情况
    const int kcntBuffSize = (kcntMaxFiles * (MAX_PATH + 1)) + 1;
    dlgFile.GetOFN().lpstrFile = csFileName.GetBuffer(kcntBuffSize);
    dlgFile.GetOFN().nMaxFile = kcntMaxFiles;

    // 弹出界面
    DWORD dwRet = dlgFile.DoModal();
    csFileName.ReleaseBuffer();

    if (dwRet == IDOK)
    {
        // 设置文件名到源文件路径编辑框
        SetDlgItemText(IDC_EDT_SRCFILE_PATH, csFileName.GetString());
    }  
}


void CAESUIDlg::OnBnClickedBtnDstfileBrowse()
{
    // TODO: 在此添加控件通知处理程序代码
    CFileDialog dlgFile(FALSE, NULL);
    // 获取单独的文件名
    CString csFileName = m_stOperationInfo.csSrcFile_;
    int iPos = csFileName.ReverseFind(_T('\\'));
    csFileName =
        csFileName.Right(csFileName.GetLength() - iPos - 1);

     // 单个文件名长度
    const int kcntMaxFiles = 100;
    // 应对多个文件选中的情况
    const int kcntBuffSize = (kcntMaxFiles * (MAX_PATH + 1)) + 1;
    dlgFile.GetOFN().lpstrFile = csFileName.GetBuffer(kcntBuffSize);
    dlgFile.GetOFN().nMaxFile = kcntMaxFiles;

    // 弹出界面
    DWORD dwRet = dlgFile.DoModal();
    // 将没用到的空间释放
    csFileName.ReleaseBuffer();

    if (dwRet == IDOK)
    {
        // 设置文件名到源文件路径编辑框
        SetDlgItemText(IDC_EDT_DSTFILE_PATH, csFileName.GetString());

    }
}




void CAESUIDlg::OnBnClickedRadModeEncrypt()
{
    // TODO: 在此添加控件通知处理程序代码
    m_iOperationType = AT_ENCRYPT;
}


void CAESUIDlg::OnBnClickedRadModeDecrypt()
{
    // TODO: 在此添加控件通知处理程序代码
    m_iOperationType = AT_DECRYPT;
}


void CAESUIDlg::OnBnClickedRadPacketmodeCbc()
{
    // TODO: 在此添加控件通知处理程序代码
    m_iPacketMode = 0;
}


LRESULT CAESUIDlg::OnUpdateProcessCtrl(WPARAM wParam, LPARAM lParam)
{
    DWORD dwCopyRate = *(DWORD *)wParam;
    m_prgOperateRate.SetPos(dwCopyRate);
    if (dwCopyRate == 1000)
    {
        SetDlgItemText(IDC_STC_OPERATION_RESULT, _T("操作成功!"));
        m_btnBegin.EnableWindow(1);
    }

    return 0;
}

void CAESUIDlg::OnDestroy()
{
    CDialog::OnDestroy();

    if (m_pthdOperator != NULL)
    {
        delete m_pthdOperator;
        m_pthdOperator = NULL;
    }
    // TODO: 在此处添加消息处理程序代码
}
