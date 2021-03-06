
// AES_UIDlg.h: 头文件
//

#pragma once
#include "afxwin.h"
#include "AssistCore.h"
#include "StructShare.h"
#include "OperationThred.h"
#include "afxcmn.h"


// CAESUIDlg 对话框
class CAESUIDlg : public CDialog
{
private:

// 构造
public:
	CAESUIDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AES_UI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
    tagOperationInfo m_stOperationInfo = { 0 };
    // 工作线程
    COperationThred *m_pthdOperator = NULL;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    // 操作模式
    int m_iOperationType;
    // 密钥
    CEdit m_edtKey;
    // 源文件路径编辑框
    CEdit m_edtSrcFilePath;
    // 目标文件路径编辑框
    CEdit m_edtDstFilePath;
    afx_msg void OnBnClickedBtnBegin();
    afx_msg void OnBnClickedBtnSrcfileBrowse();
    afx_msg void OnBnClickedBtnDstfileBrowse();
    afx_msg void OnBnClickedRadModeEncrypt();
    afx_msg void OnBnClickedRadModeDecrypt();
    // 分组模式
    int m_iPacketMode;
    afx_msg void OnBnClickedRadPacketmodeCbc();
    LRESULT OnUpdateProcessCtrl(WPARAM wParam, LPARAM lParam);
    CProgressCtrl m_prgOperateRate;
    afx_msg void OnDestroy();
    CButton m_btnBegin;
};
