// ControlSocket.cpp : implementation file
//

#include "stdafx.h"
#include "FTPserver.h"
#include "FTPserverDlg.h"
#include "ControlSocket.h"
#include "ConnectThread.h"
#include "DataSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CControlSocket

CControlSocket::CControlSocket()
{
	m_nStatus = STATUS_LOGIN;
	m_pDataSocket = NULL;
	m_strRemoteHost = "";
	m_nRemotePort = -1;
	m_bPassiveMode = FALSE;
}

CControlSocket::~CControlSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CControlSocket, CSocket)
	//{{AFX_MSG_MAP(CControlSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CControlSocket member functions

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::SendResponse							*/
/* 作用     : 返回信息给客户端 										*/
/*																	*/
/********************************************************************/
BOOL CControlSocket::SendResponse(LPCTSTR pstrFormat, ...)
{
	CString str;
	
	// 格式化返回信息
	va_list args;
	va_start(args, pstrFormat);
	str.FormatV(pstrFormat, args);
	
	// 发送信息
	int nBytes = CSocket::Send(str + "\r\n", str.GetLength()+2);
	// 发送失败
	if (nBytes == SOCKET_ERROR)
	{
		Close();	
		
		// 通知线程关闭
		m_pThread->PostThreadMessage(WM_QUIT, 1, 0);
		
		return FALSE;
	}

	// 更新活动时间
	((CConnectThread *)AfxGetThread())->UpdateTime();

	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::OnClose								*/
/* 作用     : 当套接字被关闭时销毁线程								*/
/*																	*/
/********************************************************************/
void CControlSocket::OnClose(int nErrorCode) 
{
	Close();
	// 销毁线程
	m_pThread->PostThreadMessage(WM_QUIT, 0, 0);
	CSocket::OnClose(nErrorCode);
}

#define BUFFERSIZE 4096

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::OnReceive								*/
/* 作用     : 当有数据进入缓冲区时调用								*/
/*																	*/
/********************************************************************/
void CControlSocket::OnReceive(int nErrorCode) 
{

	TCHAR buff[BUFFERSIZE+1];
	
	int nRead = Receive(buff, BUFFERSIZE);
	switch (nRead)
	{
	case 0:
		Close();
		break;
		
	case SOCKET_ERROR:
		if (GetLastError() != WSAEWOULDBLOCK) 
		{
			TCHAR szError[256];
			wsprintf(szError, "OnReceive error: %d", GetLastError());
			//AfxMessageBox (szError);
		}
		break;
		
	default:
		if (nRead != SOCKET_ERROR && nRead != 0)
		{		
			buff[nRead] = 0;
			m_RxBuffer += CString(buff);
			
			GetCommandLine();
		}
		break;
	}
	CSocket::OnReceive(nErrorCode);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::GetCommandLine						*/
/* 作用     : 获得完整的命令行信息									*/
/*																	*/
/********************************************************************/
void CControlSocket::GetCommandLine()
{
	CString strTemp;
	int nIndex;
	
	while(!m_RxBuffer.IsEmpty())
	{
		nIndex = m_RxBuffer.Find("\r\n");
		if (nIndex != -1)
		{
			strTemp = m_RxBuffer.Left(nIndex);
			m_RxBuffer = m_RxBuffer.Mid(nIndex + 2);
			if (!strTemp.IsEmpty())
			{
				m_strCommands.AddTail(strTemp);
				// 处理命令
				ProcessCommand();
			}
		}
		else
		{
			break;
		}
	}
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::ProcessCommand						*/
/* 作用     : 处理命令行及参数										*/
/*																	*/
/********************************************************************/
void CControlSocket::ProcessCommand()
{
	CString strCommand, strArguments;

	// 获取命令部分
	CString strBuff = m_strCommands.RemoveHead();
	int nIndex = strBuff.Find(" ");
	if (nIndex == -1)
	{
		strCommand = strBuff;
	}
	else
	{
		strCommand = strBuff.Left(nIndex);
		strArguments = strBuff.Mid(nIndex+1);
	}
	strCommand.MakeUpper();

	// 处理中断命令溢出
	if (strCommand.Right(4) == "ABOR")
	{
		strCommand = "ABOR";
	}

	// 登陆没输入账号及密码
	if ((m_nStatus == STATUS_LOGIN) && (strCommand != "USER" && strCommand != "PASS"))
	{
		SendResponse("530 Please login with USER and PASS.");
		return;
	}
	
	// 输入账号
	if (strCommand == "USER")
	{
		m_nStatus = STATUS_LOGIN;
		m_strUserName = strArguments;

		CString strPeerAddress;
		UINT nPeerPort;
		GetPeerName(strPeerAddress, nPeerPort);

		SendResponse("331 Please specify the password.");
	}
	else
	// 输入密码
	if (strCommand == "PASS")
	{
		// 账号不能为空
		if (m_strUserName.IsEmpty())
		{
			SendResponse("503 Login with USER first.");
		}
		else
		{
			// 检查用户名
			if (!m_strUserName.CompareNoCase(pWnd->m_strName))
			{
				// 检查密码
				if (!strArguments.Compare(pWnd->m_strPassword) || strArguments.IsEmpty() || !m_strUserName.CompareNoCase("anonymous"))
				{
					// 设置主目录
					m_strCurrentDir = "/";

					// 返回成功登陆
					m_nStatus = STATUS_IDLE;
					SendResponse("230 Login successful.");
					return;
				}
			}
			SendResponse("530 Not logged in, user or password incorrect!");
		}
	}		
	else
	// 关闭连接
	if ((strCommand == "QUIT") || (strCommand == "BYE"))
	{
		// 返回关闭
		CConnectThread *pThread = (CConnectThread *)m_pThread;
		SendResponse("220 %s", ((CFTPserverDlg *)pThread->m_pWndServer)->GetGoodbyeMessage());

		Close();

		// 关闭连接线程
		m_pThread->PostThreadMessage(WM_QUIT, 1, 0);
	}
	else
	// 改变传输类型
	if (strCommand == "TYPE")
	{
		// 返回传输类型
		SendResponse("200 Type set to %s", strArguments);
	}
	else
	// 返回当前目录
	if (strCommand == "PWD")
	{
		SendResponse("257 \"%s\" is current directory.", m_strCurrentDir);
	}
	else
	// 返回上级目录
	if (strCommand == "CDUP")
	{
		DoChangeDirectory("..");
	}
	else
	// 改变工作目录
	if (strCommand == "CWD")
	{
		DoChangeDirectory(strArguments);
	}
	else
	// PORT模式，转换IP和2字节的端口地址为标准地址
	// (PORT a1,a2,a3,a4,p1,p2) -> a1.a2.a3.a4, p1*256+p2
	if (strCommand == "PORT")
	{
		CString strSub;
		int nCount=0;

		while (AfxExtractSubString(strSub, strArguments, nCount++, ','))
		{
			switch(nCount)
			{
				case 1:	// a1
					m_strRemoteHost = strSub;
					m_strRemoteHost += ".";
					break;
				case 2:	// a2
					m_strRemoteHost += strSub;
					m_strRemoteHost += ".";
					break;
				case 3:	// a3
					m_strRemoteHost += strSub;
					m_strRemoteHost += ".";
					break;
				case 4:	// a4
					m_strRemoteHost += strSub;
					break;
				case 5:	// p1
					m_nRemotePort = 256*atoi(strSub);
					break;
				case 6:	// p2
					m_nRemotePort += atoi(strSub);
					break;
			}
		}
		SendResponse("200 Port command successful.");
	}
	else
	// PASV模式
	if (strCommand == "PASV")
	{	
		// 删除已经存在的数据连接
		DestroyDataConnection();

		// 创建新的数据SOCKET
		m_pDataSocket = new CDataSocket(this);

		if (!m_pDataSocket->Create())
		{
			DestroyDataConnection();	
			SendResponse("421 Failed to create socket.");
			return;
		}
		// 开始监听
		m_pDataSocket->Listen();
		m_pDataSocket->AsyncSelect();
		
		CString strIPAddress, strTmp;
		UINT nPort;
		
		// 获取本地IP
		GetSockName(strIPAddress, nPort);
		// 获取监听端口
		m_pDataSocket->GetSockName(strTmp, nPort);
		// 将'.'换成','
		strIPAddress.Replace(".", ",");
		// 返回PASV连接信息
		SendResponse("227 Entering Passive Mode (%s,%d,%d).", strIPAddress, nPort/256, nPort%256);
		
		m_bPassiveMode = TRUE;
	} 
	else
	// 列出当前目录文件
	if (strCommand == "LIST")
	{
		CString strListing;
		if (!GetDirectoryList(strArguments, strListing))
		{
			// something went wrong
			return;
		}

		SendResponse("150 Opening ASCII mode data connection for directory list."); 

		// 创建数据连接
		if (CreateDataConnection())
		{
			if (strListing.IsEmpty())
			{
				// 关闭数据连接
				DestroyDataConnection();

				SendResponse("226 Transfer complete."); 
				m_nStatus = STATUS_IDLE;
				return;
			}
		}
		else
		{
			// 关闭数据连接
			DestroyDataConnection();
			return;
		}

		m_nStatus = STATUS_LIST;
		
		// 发送目录信息
		m_pDataSocket->SendListing(strListing);
	} 
	else
	// 下载文件
	if (strCommand == "RETR")
	{
		CString strResult;
		int nResult = CheckFileName(strArguments, FTP_DOWNLOAD, strResult);
		m_nStatus = STATUS_DOWNLOAD;
		switch(nResult)
		{
			case ERROR_ACCESS_DENIED:
				SendResponse("550 Permission denied.");
				break;
			case ERROR_FILE_NOT_FOUND:
				SendResponse("550 File not found.");
				break;
			default:
				SendResponse("150 Opening BINARY mode data connection for file transfer.");

				// 创建新的数据连接用于文件传输
				if (!CreateDataConnection())
				{
					m_nStatus = STATUS_IDLE;
					// 出错关闭数据连接
					DestroyDataConnection();
				}
				else
				{
					//m_nStatus = STATUS_DOWNLOAD;
					// 发送文件
					m_pDataSocket->SendFile(strResult);
				}
				break;
		}
	}
	else
	// 上传文件
	if (strCommand == "STOR")
	{
		CString strResult;
		int nResult = CheckFileName(strArguments, FTP_UPLOAD, strResult);
		m_nStatus = STATUS_UPLOAD;
		switch(nResult)
		{
			case ERROR_ACCESS_DENIED:
				SendResponse("550 Permission denied.");
				break;
			case ERROR_FILE_NOT_FOUND:
				SendResponse("550 Filename invalid.");
				break;
			default:
				SendResponse("150 Opening BINARY mode data connection for file transfer.");

				// 创建新的数据连接用于文件传输
				if (!CreateDataConnection())
				{
					m_nStatus = STATUS_IDLE;
					// 出错关闭数据连接
					DestroyDataConnection();
				}
				else
				{
					//m_nStatus = STATUS_UPLOAD;
					// 接收文件
					m_pDataSocket->RetrieveFile(strResult);
				}
				break;
		}
	}
	else
	// 得到文件大小
	if (strCommand == "SIZE")
	{
		CString strResult;
		int nResult = CheckFileName(strArguments, FTP_DOWNLOAD, strResult);
		switch(nResult)
		{
			case ERROR_ACCESS_DENIED:
				SendResponse("550 Permission denied.");
				break;
			case ERROR_FILE_NOT_FOUND:
				SendResponse("550 File not found.");
				break;
			default:
			{
				CFileStatus status;
				CFile::GetStatus(strResult, status);
				SendResponse("213 %d", status.m_size);
				break;
			}
		}
	}
	else
	// 删除文件
	if (strCommand == "DELE")
	{
		CString strResult;
		int nResult = CheckFileName(strArguments, FTP_DELETE, strResult);
		switch(nResult)
		{
			case ERROR_ACCESS_DENIED:
				SendResponse("550 Permission denied.");
				break;
			case ERROR_FILE_NOT_FOUND:
				SendResponse("550 File not found.");
				break;
			default:
				CString strRelativePath;
				GetRelativePath(strResult, strRelativePath);

				// 删除文件
				if (!DeleteFile(strResult))
				{
					SendResponse("450 Internal error deleting the file: \"%s\".",  strRelativePath);
				}
				else
				{
					SendResponse("250 File \"%s\" was deleted successfully.", strRelativePath);
				}
				break;
		}
	}
	else
	// 文件改名from
	if (strCommand == "RNFR")
	{
		int nResult = 0; 
		// 检查文件
		nResult = CheckFileName(strArguments, FTP_RENAME, strReName);
		if (nResult != ERROR_SUCCESS)
		{
			// 检查目录
			nResult = CheckDirectory(strArguments,FTP_RENAME,strReName);
			if (nResult != ERROR_SUCCESS)
			{
				switch(nResult)
				{
				case ERROR_ACCESS_DENIED:
					SendResponse("550 Permission denied.");
					break;
				case ERROR_PATH_NOT_FOUND:
					SendResponse("550 Directory not found.");
					break;
				case ERROR_FILE_NOT_FOUND:
					SendResponse("550 File not found.");
					break;
				}
			}
			else
			{
				SendResponse("350 Directory exists, ready for destination name.");
			}	
		}
		else
		{
			SendResponse("350 File exists, ready for destination name.");
		}
	}
	else
	// 文件改名to
	if (strCommand == "RNTO")
	{
		CString strResult;
		CheckFileName(strArguments, FTP_RENAME, strResult);
		// 移动文件
		if (!MoveFile(strReName,strResult))
		{
			SendResponse("450 Internal error renamed the file.");
		}
		else
		{
			SendResponse("250 renamed successfully.");
		}
	}
	else
	// 删除目录
	if (strCommand == "RMD")
	{		
		CString strResult;
		int nResult = CheckDirectory(strArguments, FTP_DELETE, strResult);
		switch(nResult)
		{
			case ERROR_ACCESS_DENIED:
				SendResponse("550 Permission denied.");
				break;
			case ERROR_PATH_NOT_FOUND:
				SendResponse("550 Directory not found.");
				break;
			default:
				// 删除目录
				if (!RemoveDirectory(strResult))
				{
					if (GetLastError() == ERROR_DIR_NOT_EMPTY)
					{
						SendResponse("550 Directory not empty.");
					}
					else
					{
						SendResponse("450 Internal error deleting the directory.");
					}
				}
				else
				{
					SendResponse("250 Directory deleted successfully.");
				}
				break;
		}
	}
	else
	// 创建目录
	if (strCommand == "MKD")
	{		
		CString strResult;
		int nResult = CheckDirectory(strArguments, FTP_CREATE_DIR, strResult);
		switch(nResult)
		{
			case ERROR_SUCCESS:
				SendResponse("550 Directory already exists.");
				break;
			case ERROR_ACCESS_DENIED:
				SendResponse("550 Can't create directory. Permission denied.");
				break;
			default:
				// 创建目录
				if (!MakeSureDirectoryPathExists(strResult))
				{
					SendResponse("450 Internal error creating the directory.");
				}
				else
				{
					SendResponse("250 Directory created successfully.");
				}
				break;
		}
	}
	else
	// 中断数据连接
	if (strCommand == "ABOR")
	{		
		if (m_pDataSocket)
		{
			if (m_nStatus != STATUS_IDLE)
			{
				SendResponse("426 Data connection closed.");
			}
			// 关闭连接线程
			m_pThread->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
		}
		SendResponse("226 ABOR command successful.");
	} 
	else
	// 获得系统信息
	if (strCommand == "SYST")
	{
		SendResponse("215 UNIX Slyar FTPserver");
	}
	else
	// 无动作
	if (strCommand == "NOOP")
	{
		SendResponse("200 OK");
	}
	else
	{
		SendResponse("502 Command not implemented.");
	}
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::DoChangeDirectory						*/
/* 作用     : 切换到指定目录										*/
/*																	*/
/********************************************************************/
void CControlSocket::DoChangeDirectory(LPCTSTR lpszDirectory)
{
	CString strLocalPath;
	int nResult = CheckDirectory(lpszDirectory, FTP_LIST, strLocalPath);
	switch(nResult)
	{
	case ERROR_ACCESS_DENIED:
		// 无权限访问
		SendResponse("550 \"%s\": Permission denied.", lpszDirectory);
		break;
	case ERROR_PATH_NOT_FOUND:
		// 目录不存在
		SendResponse("550 \"%s\": Directory not found.", lpszDirectory);
		break;
	default:
		// 成功
		CString strRelativePath;
		if (GetRelativePath(strLocalPath, strRelativePath))
		{
			m_strCurrentDir = strRelativePath;
		}
		SendResponse("250 \"%s\" is current directory.", m_strCurrentDir);
		break;
	}
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::CheckDirectory						*/
/* 作用     : 检查目录是否存在且用户是否有权访问					*/
/*																	*/
/********************************************************************/
int CControlSocket::CheckDirectory(CString strDirectory, int nOption, CString &strResult)
{
	// 兼容UNIX格式
	strDirectory.Replace("\\","/");
	while(strDirectory.Replace("//","/"));
	strDirectory.TrimRight("/");
	
	if (strDirectory.IsEmpty())
	{
		if (nOption == FTP_LIST)
		{
			strDirectory = "/";
		}
		else
		{
			// 目录不存在
			return ERROR_PATH_NOT_FOUND;
		}
	}
	else
	{
		// 不是当前目录
		if (strDirectory.Left(1) != "/")
		{ 
			// 路径变为当前路径的相对路径
			if (m_strCurrentDir.Right(1) != '/')
				strDirectory = m_strCurrentDir + "/" + strDirectory;
			else
				strDirectory = m_strCurrentDir + strDirectory;
		}
	}
	
	// 判断目录是否存在
	BOOL bPathExists = GetLocalPath(strDirectory, strResult);
	
	// 路径不存在且操作不是创建文件夹
	if (!bPathExists && (nOption != FTP_CREATE_DIR))
	{ 
		return ERROR_PATH_NOT_FOUND;
	}
	
	// 判断用户是否有权限访问
	if (!CheckAccessRights(strResult, nOption))
	{
		// 无权限
		return ERROR_ACCESS_DENIED;
	}
	
	// 路径不存在
	if (!bPathExists)
	{
		return ERROR_PATH_NOT_FOUND;
	}
	
	// 成功
	return ERROR_SUCCESS;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::CheckFileName							*/
/* 作用     : 检查文件是否存在且用户是否有权访问					*/
/*																	*/
/********************************************************************/
int CControlSocket::CheckFileName(CString strFilename, int nOption, CString &strResult)
{
	// 兼容UNIX格式
	strFilename.Replace("\\", "/");
	while(strFilename.Replace("//", "/"));
	strFilename.TrimRight("/");
	
	if (strFilename == "")
	{
		// 文件名为空
		return ERROR_FILE_NOT_FOUND;
	}
	
	// 保存当前目录
	CString strDirectory = m_strCurrentDir;
	
	// 客户端给出的是完整的路径，提取文件名
	int nPos = strFilename.ReverseFind('/');
	if (nPos != -1)
	{
		strDirectory = strFilename.Left(nPos);
		if (strDirectory == "")
		{
			strDirectory = "/";
		}
		strFilename = strFilename.Mid(nPos+1);
	}
	
	// 得到绝对路径
	CString strLocalPath;
	if (!GetLocalPath(strDirectory, strLocalPath))
	{
		// directory does not exist
		return ERROR_FILE_NOT_FOUND;
	}
	
	// 创建完整路径
	strResult = strLocalPath + "\\" + strFilename;
	
	BOOL bFileExist;
	bFileExist = FileExists(strResult, FALSE);

	// 检查文件是否存在
	if ((nOption != FTP_UPLOAD) && !bFileExist)
	{
		// 文件不存在
		return ERROR_FILE_NOT_FOUND;
	}

	// 重名文件覆盖上传
	if ((nOption == FTP_UPLOAD) && bFileExist && (!pWnd->m_bAllowDelete))
	{
		// 用户无权限覆盖
		return ERROR_ACCESS_DENIED;
	}
	
	// 检查文件访问权限
	if (!CheckAccessRights(strLocalPath, nOption))
	{
		// 用户无权限访问
		return ERROR_ACCESS_DENIED;
	}
	
	// 成功
	return ERROR_SUCCESS;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::CheckAccessRights						*/
/* 作用     : 检查访问权限											*/
/*																	*/
/********************************************************************/
BOOL CControlSocket::CheckAccessRights(LPCTSTR lpszDirectory, int nOption)
{
	// 保存完整路径
	CString strCheckDir = lpszDirectory;
	
	// 一层一层分析是否上级目录相同
	while(!strCheckDir.IsEmpty())
	{
		CString strPath1 = strCheckDir;
		strPath1.TrimRight("\\");
		CString strPath2 = pWnd->m_strHomeDirectory;
		strPath2.TrimRight("\\");
		
		// 找到一个匹配，某层上级目录相同
		if (strPath1.CompareNoCase(strPath2) == 0)
		{
			// 检查操作权限
			if (((!pWnd->m_bAllowDownload) && (nOption == FTP_DOWNLOAD)) ||
				((!pWnd->m_bAllowUpload) && (nOption == FTP_UPLOAD)) ||
				((!pWnd->m_bAllowRename) && (nOption == FTP_RENAME)) ||
				((!pWnd->m_bAllowDelete) && (nOption == FTP_DELETE)) ||
				((!pWnd->m_bAllowCreateDirectory) && (nOption == FTP_CREATE_DIR)) ||
				((!pWnd->m_bAllowDownload && !pWnd->m_bAllowUpload) && (nOption == FTP_LIST)))
			{
				return FALSE;
			}
			return TRUE;
		}
		// 截断下一个"\\"后的内容
		int nPos = strCheckDir.ReverseFind('\\');
		if (nPos != -1)
		{
			strCheckDir = strCheckDir.Left(nPos);
		}
		else
		{
			// 不存在匹配，清空目录，退出循环
			strCheckDir.Empty();
		}
	} 
	// 用户无权限访问
	return FALSE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::DestroyDataConnection					*/
/* 作用     : 关闭数据连接											*/
/*																	*/
/********************************************************************/
void CControlSocket::DestroyDataConnection()
{
	if (!m_pDataSocket)
	{
		return;
	}
	
	delete m_pDataSocket;
	
	// 重置传输信息
	m_pDataSocket = NULL;
	m_strRemoteHost = "";
	m_nRemotePort = -1;
	m_bPassiveMode = FALSE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::GetDirectoryList						*/
/* 作用     : 获取指定目录的文件列表								*/
/*																	*/
/********************************************************************/
BOOL CControlSocket::GetDirectoryList(LPCTSTR lpszDirectory, CString &strResult)
{
	CString strDirectory = lpszDirectory;
	
	// 清空结果
	strResult = "";
	
	// 去掉通配符，意思不变
	strDirectory.TrimRight('*');
	
	// 用户没有指定路径，使用当前路径
	if (strDirectory.IsEmpty())
	{
		strDirectory = m_strCurrentDir;
	}
	
	CString strLocalPath;
	int nResult = CheckDirectory(strDirectory, FTP_LIST, strLocalPath);
	switch(nResult)
	{
	case ERROR_ACCESS_DENIED:
		// 用户无权限访问
		SendResponse("550 \"%s\": Permission denied.", lpszDirectory);
		return FALSE;
		
	case ERROR_PATH_NOT_FOUND:
		// 路径不存在
		SendResponse("550 \"%s\": Directory not found.", lpszDirectory);
		return FALSE;
		
	default:
		// 成功
		break;
	}
	
	CFileFind find;
	BOOL bFound = FALSE;
	
	// 判断是文件还是文件夹
	if ((GetFileAttributes(strLocalPath) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		CString strPath = strLocalPath;
		if (strPath.Right(1)==_T("\\"))
			bFound = find.FindFile(strLocalPath + "*.*");
		else
			bFound = find.FindFile(strLocalPath + "\\*.*");
	}
	else
	{
		// 文件
		bFound = find.FindFile(strLocalPath);
	}
	
	while (bFound)
	{
		bFound = find.FindNextFile();
		
		// 跳过 "." 和 ".." 
		if (find.IsDots())
			continue;
		
		// 权限信息
		if (find.IsDirectory())
			strResult += "drwx------";
		else
			strResult += "-rwx------";
		
		// 组信息
		strResult += " 1 user group ";

		// 文件大小
		CString strLength;
		strLength.Format("%d", find.GetLength());
		CString strFiller = "              ";
		strResult += strFiller.Left(strFiller.GetLength() - strLength.GetLength());
		strResult += strLength;

		// 文件时间
		strResult += GetFileDate(find);

		// 文件名
		strResult += find.GetFileName();

		// 本行结束
		strResult += "\r\n";
	}
	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::CreateDataConnection					*/
/* 作用     : 创建数据连接											*/
/*																	*/
/********************************************************************/
BOOL CControlSocket::CreateDataConnection()
{
	// 判断是PASV还是PORT模式
	if (!m_bPassiveMode)
	{
		// 需要使用PORT命令启动模式
		if (m_strRemoteHost == "" || m_nRemotePort == -1)
		{
			SendResponse("425 Can't open data connection.");
			return FALSE;
		}
		
		m_pDataSocket = new CDataSocket(this);
		if (m_pDataSocket->Create())
		{
			m_pDataSocket->AsyncSelect();
			
			// 连接对端主机
			if (m_pDataSocket->Connect(m_strRemoteHost, m_nRemotePort) == 0)
			{
				if (GetLastError() != WSAEWOULDBLOCK)
				{
					SendResponse("425 Can't open data connection.");
					return FALSE;
				}
			}
		}
		else
		{
			SendResponse("421 Failed to create data connection socket.");
			return FALSE;
		}
	}
	
	// 等待连接(最大10s)
	DWORD dwTickCount = GetTickCount() + 10000;
	while (!m_pDataSocket->m_bConnected)
	{
		DoEvents();
		if (dwTickCount < GetTickCount())
		{
			SendResponse("421 Failed to create data connection socket.");
			return FALSE;
		}
	}
	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::GetRelativePath						*/
/* 作用     : 转换绝对路径为相对路径								*/
/*																	*/
/********************************************************************/
BOOL CControlSocket::GetRelativePath(LPCTSTR lpszLocalPath, CString &strRelativePath)
{
	CString strOffset = pWnd->m_strHomeDirectory;
	CString strLocalPath = lpszLocalPath;
	
	strOffset.MakeLower();
	strLocalPath.MakeLower();
	
	if (strOffset.Right(1) != '\\')
		strOffset += "\\";
	
	if (strLocalPath.Right(1) != '\\')
		strLocalPath += "\\";
	
	if (strOffset == strLocalPath)
	{
		strRelativePath = "/";
	}
	else
	{
		strRelativePath = strLocalPath;
		strRelativePath.Replace(strOffset, "/");
		strRelativePath.Replace("\\", "/");
		strRelativePath.TrimRight('/');
	}
	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CControlSocket::GetLocalPath							*/
/* 作用     : 获取绝对路径											*/
/*																	*/
/********************************************************************/
BOOL CControlSocket::GetLocalPath(LPCTSTR lpszRelativePath, CString &strLocalPath)
{
	CString strRelativePath = lpszRelativePath;
	
	// 兼容UNIX格式
	strRelativePath.Replace("\\","/");
	while(strRelativePath.Replace("//","/"));
	
	CStringList partList;
	CString strSub;
	int nCount=0;
	BOOL bPathExists = TRUE;
	
	// 分解路径
	while(AfxExtractSubString(strSub, strRelativePath, nCount++, '/'))
	{
		// 去掉无意义的'.'
		if (strSub != "..")
		{
			strSub.TrimLeft('.');
			strSub.TrimRight('.');
		}
		
		if (!strSub.IsEmpty())
			partList.AddTail(strSub);
	}
	
	// 查找根目录
	CString strHomeDir;
	if (lpszRelativePath[0] == '/')
	{
		// 绝对路径
		strHomeDir = pWnd->m_strHomeDirectory;
	}
	else
	{
		// 相对路径，继续递归查找
		GetLocalPath(m_strCurrentDir, strHomeDir);
	}
	
	while(!partList.IsEmpty())
	{
		CString strPart = partList.GetHead();
		partList.RemoveHead();
		
		CString strCheckDir;
		
		if (strPart == "..")
		{
			// 回到上一级
			int nPos = strHomeDir.ReverseFind('\\');
			if (nPos != -1)
			{
				strCheckDir = strHomeDir.Left(nPos);
			}
		}
		else
		{
			strCheckDir = strHomeDir + "\\" + strPart;
		}
		
		// 目录是否存在
		if (FileExists(strCheckDir, TRUE))
		{
			strHomeDir = strCheckDir;
		}
		else
		{
			// 文件是否存在
			if (FileExists(strCheckDir, FALSE))
			{
				strHomeDir = strCheckDir;
			}
			else
			{
				strHomeDir = strCheckDir;
				
				// 目录不存在
				bPathExists = FALSE;
			}
		}
	}
	
	// 成功
	strLocalPath = strHomeDir;
	
	// 驱动器符号还原':\\'
	if (strLocalPath.Right(1) == ':')
		strLocalPath += "\\";
	
	return bPathExists;
}
