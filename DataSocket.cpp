// DataSocket.cpp : implementation file
//

#include "stdafx.h"
#include "FTPserver.h"
#include "DataSocket.h"
#include "ControlSocket.h"
#include "ConnectThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PACKET_SIZE 4096

/////////////////////////////////////////////////////////////////////////////
// CDataSocket

CDataSocket::CDataSocket(CControlSocket *pSocket)
{
	m_pControlSocket = pSocket;
	m_strListing = "";
	m_File.m_hFile = NULL;
	m_bConnected = FALSE;

	m_nTotalBytesSend = 0;
	m_nTotalBytesTransfered = 0;
}

CDataSocket::~CDataSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CDataSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CDataSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CDataSocket member functions

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::SendListing								*/
/* 作用     : 发送目录信息											*/
/*																	*/
/********************************************************************/
void CDataSocket::SendListing(LPCTSTR lpszListing)
{
	m_strListing = lpszListing;
	// 要发送的长度
	m_nTotalBytesSend = m_strListing.GetLength();
	m_nTotalBytesTransfered = 0;
	// 开始发送
	OnSend(0);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::SendFile									*/
/* 作用     : 发送文件												*/
/*																	*/
/********************************************************************/
void CDataSocket::SendFile(LPCTSTR lpszFilename)
{
	if (!PrepareSendFile(lpszFilename))
	{
		m_pControlSocket->m_nStatus = STATUS_IDLE;
		
		m_pControlSocket->SendResponse("426 Connection closed; transfer aborted.");

		AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
		return;
	}
	
	// 开始发送
	OnSend(0);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::RetrieveFile								*/
/* 作用     : 接收文件												*/
/*																	*/
/********************************************************************/
void CDataSocket::RetrieveFile(LPCTSTR lpszFilename)
{
	if (!PrepareReceiveFile(lpszFilename))
	{
		m_pControlSocket->m_nStatus = STATUS_IDLE;
		
		m_pControlSocket->SendResponse("450 Can't access file.");
		
		AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
		return;
	}
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::PrepareReceiveFile						*/
/* 作用     : 准备接收文件											*/
/*																	*/
/********************************************************************/
BOOL CDataSocket::PrepareReceiveFile(LPCTSTR lpszFilename)
{
	// 文件已经打开则关闭先
	if (m_File.m_hFile != NULL)
	{
		m_File.Close();
	}
	
	// 打开目标文件
	if (!m_File.Open(lpszFilename, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyWrite))
	{
		return FALSE;
	}
	m_nTotalBytesReceive = 0;
	m_nTotalBytesTransfered = 0;
	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::PrepareSendFile							*/
/* 作用     : 准备发送文件											*/
/*																	*/
/********************************************************************/
BOOL CDataSocket::PrepareSendFile(LPCTSTR lpszFilename)
{
	// 文件已经打开则关闭先
	if (m_File.m_hFile != NULL)
	{
		m_File.Close();
	}
	
	// 打开目标文件
	if (!m_File.Open(lpszFilename, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary))
	{
		return FALSE;
	}
	m_nTotalBytesSend = m_File.GetLength();
	m_nTotalBytesTransfered = 0;
	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::OnAccept									*/
/* 作用     : 接受连接响应											*/
/*																	*/
/********************************************************************/
void CDataSocket::OnAccept(int nErrorCode) 
{
	// 创建临时对象接收连接
	CAsyncSocket tmpSocket;
	Accept(tmpSocket);
	
	// 分离SOCKET
	SOCKET socket = tmpSocket.Detach();
	Close();
	
	// 附加SOCKET到当前数据连接对象
	Attach(socket);
	
	// 已经连接
	m_bConnected = TRUE;
	
	CAsyncSocket::OnAccept(nErrorCode);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::OnClose									*/
/* 作用     : 关闭连接响应											*/
/*																	*/
/********************************************************************/
void CDataSocket::OnClose(int nErrorCode) 
{
	if (m_pControlSocket)
	{
		// 停止发送
		ShutDown(1);
		
		if (m_pControlSocket->m_nStatus == STATUS_UPLOAD)
		{
			while(Receive() != 0)
			{
				// 接收缓冲区中的数据				
			}
		}
		else
		{
			m_pControlSocket->SendResponse("426 Connection closed; transfer aborted.");

			AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
		}
	}
	m_pControlSocket->m_nStatus = STATUS_IDLE;	
	m_bConnected = FALSE;
	CAsyncSocket::OnClose(nErrorCode);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::OnConnect								*/
/* 作用     : 建立连接响应											*/
/*																	*/
/********************************************************************/
void CDataSocket::OnConnect(int nErrorCode) 
{
	m_bConnected = TRUE;
	if (nErrorCode)
	{
		m_pControlSocket->m_nStatus = STATUS_IDLE;
		
		m_pControlSocket->SendResponse("425 Can't open data connection.");

		AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
	}
	CAsyncSocket::OnConnect(nErrorCode);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::OnReceive								*/
/* 作用     : 接受数据响应											*/
/*																	*/
/********************************************************************/
void CDataSocket::OnReceive(int nErrorCode) 
{
	CAsyncSocket::OnReceive(nErrorCode);
	Receive();
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::OnSend									*/
/* 作用     : 发送数据响应											*/
/*																	*/
/********************************************************************/
void CDataSocket::OnSend(int nErrorCode) 
{
	CAsyncSocket::OnSend(nErrorCode);

	switch(m_pControlSocket->m_nStatus)
	{
		case STATUS_LIST:
		{
			while (m_nTotalBytesTransfered < m_nTotalBytesSend)
			{
				DWORD dwRead;
				int dwBytes;

				CString strDataBlock;
				
				dwRead = m_strListing.GetLength();
				
				if (dwRead <= PACKET_SIZE)
				{
					strDataBlock = m_strListing;
				}
				else
				{
					strDataBlock = m_strListing.Left(PACKET_SIZE);
					dwRead = strDataBlock.GetLength();
				}
				
				if ((dwBytes = Send(strDataBlock, dwRead)) == SOCKET_ERROR)
				{
					if (GetLastError() == WSAEWOULDBLOCK) 
					{
						Sleep(0);
						return;
					}
					else
					{
						TCHAR szError[256];
						wsprintf(szError, "Server Socket failed to send: %d", GetLastError());

						Close();

						m_nTotalBytesSend = 0;
						m_nTotalBytesTransfered = 0;

						m_pControlSocket->m_nStatus = STATUS_IDLE;

						m_pControlSocket->SendResponse("426 Connection closed; transfer aborted.");

						AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
					}
				}
				else
				{
					m_nTotalBytesTransfered += dwBytes;
					m_strListing = m_strListing.Mid(dwBytes);
				}
			}
			if (m_nTotalBytesTransfered == m_nTotalBytesSend)
			{
				Close();

				m_nTotalBytesSend = 0;
				m_nTotalBytesTransfered = 0;

				m_pControlSocket->m_nStatus = STATUS_IDLE;

				m_pControlSocket->SendResponse("226 Transfer complete");

				AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
			}
			break;
		}

		case STATUS_DOWNLOAD:
		{
			while (m_nTotalBytesTransfered < m_nTotalBytesSend)
			{
				// 储存文件
				byte data[PACKET_SIZE];
				
				m_File.Seek(m_nTotalBytesTransfered, CFile::begin);

				DWORD dwRead = m_File.Read(data, PACKET_SIZE);
    
				int dwBytes;

				if ((dwBytes = Send(data, dwRead)) == SOCKET_ERROR)
				{
					if (GetLastError() == WSAEWOULDBLOCK) 
					{
						Sleep(0);
						break;
					}
					else
					{
						TCHAR szError[256];
						wsprintf(szError, "Server Socket failed to send: %d", GetLastError());

						m_File.Close();
						m_File.m_hFile = NULL;

						Close();

						m_nTotalBytesSend = 0;
						m_nTotalBytesTransfered = 0;

						m_pControlSocket->m_nStatus = STATUS_IDLE;

						m_pControlSocket->SendResponse("426 Connection closed; transfer aborted.");

						AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
					}
				}
				else
				{
					m_nTotalBytesTransfered += dwBytes;

					// 更新活动时间
					((CConnectThread *)AfxGetThread())->UpdateTime();
				}
			}
			if (m_nTotalBytesTransfered == m_nTotalBytesSend)
			{
                m_File.Close();
				m_File.m_hFile = NULL;

                Close();

                m_nTotalBytesSend = 0;
                m_nTotalBytesTransfered = 0;


				m_pControlSocket->m_nStatus = STATUS_IDLE;

				m_pControlSocket->SendResponse("226 Transfer complete");

				AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
			}
			break;
		}
	}
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDataSocket::Receive									*/
/* 作用     : 覆盖接收数据函数										*/
/*																	*/
/********************************************************************/
int CDataSocket::Receive()
{
	int nRead = 0;
	
	// 上传状态
	if (m_pControlSocket->m_nStatus == STATUS_UPLOAD)
	{
		if (m_File.m_hFile == NULL)
			return 0;
		
		byte data[PACKET_SIZE];
		nRead = CAsyncSocket::Receive(data, PACKET_SIZE);
		
		switch(nRead)
		{
		case 0:
			{
				m_File.Close();
				m_File.m_hFile = NULL;
				Close();

				m_pControlSocket->SendResponse("226 Transfer complete");

				AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
				break;
			}
		case SOCKET_ERROR:
			{
				if (GetLastError() != WSAEWOULDBLOCK)
				{
					m_File.Close();
					m_File.m_hFile = NULL;
					Close();
					m_pControlSocket->SendResponse("426 Connection closed; transfer aborted.");

					AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
				}
				break;
			}
		default:
			{
				// 更新活动时间
				((CConnectThread *)AfxGetThread())->UpdateTime();
				TRY
				{
					m_File.Write(data, nRead);
				}
				CATCH_ALL(e)
				{
					m_File.Close();
					m_File.m_hFile = NULL;
					Close();
					m_pControlSocket->SendResponse("450 Can't access file.");

					AfxGetThread()->PostThreadMessage(WM_DESTROYDATACONNECTION, 0, 0);
					return 0;
				}
				END_CATCH_ALL;
				break;
			}
		}
	}
	return nRead;
}
