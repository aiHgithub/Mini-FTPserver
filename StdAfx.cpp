// stdafx.cpp : source file that includes just the standard includes

#include "stdafx.h"

/********************************************************************/
/*																	*/
/* 函数名称 : DoEvents												*/
/* 作用     : 启动消息循环											*/
/*																	*/
/********************************************************************/
void DoEvents()
{
	MSG msg;
	       
	while (PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))         
	{            
		TranslateMessage(&msg);            
		DispatchMessage(&msg);         
	}      
}

/********************************************************************/
/*																	*/
/* 函数名称 : BrowseForFolder										*/
/* 作用     : 选择路径												*/
/*																	*/
/********************************************************************/
CString BrowseForFolder(HWND hWnd, LPCSTR lpszTitle, UINT nFlags)
{
	CString strResult = "";
	
	LPMALLOC lpMalloc;
	
	if (::SHGetMalloc(&lpMalloc) != NOERROR)
	{
		return strResult; 
	}
	
	char szBuffer[_MAX_PATH];
	char szDisplayName[_MAX_PATH];
	
	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = hWnd;
	browseInfo.pidlRoot = NULL; 
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.lpszTitle = lpszTitle;
	browseInfo.ulFlags = nFlags;
	browseInfo.lpfn = NULL;
	browseInfo.lParam = 0;
	
	LPITEMIDLIST lpItemIDList;
	
	if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
	{
		if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
		{
			if (szBuffer[0] == '\0')
			{
				AfxMessageBox("打开路径失败!", MB_ICONSTOP|MB_OK);
				return strResult;
			}
			
			strResult = szBuffer;
			return strResult;
		}
		else
		{
			AfxMessageBox("打开路径失败!", MB_ICONSTOP|MB_OK);
			return strResult;
		}
		lpMalloc->Free(lpItemIDList);
		lpMalloc->Release();      
	}
	return strResult;
}

/********************************************************************/
/*																	*/
/* 函数名称 : MakeSureDirectoryPathExists							*/
/* 作用     : 确保路径存在											*/
/*																	*/
/********************************************************************/
BOOL MakeSureDirectoryPathExists(LPCTSTR lpszDirPath)
{
	CString strDirPath = lpszDirPath;
	
	int nPos = 0;
	
	while((nPos = strDirPath.Find('\\', nPos+1)) != -1) 
	{
		CreateDirectory(strDirPath.Left(nPos), NULL);
	}
	return CreateDirectory(strDirPath, NULL);
}

/********************************************************************/
/*																	*/
/* 函数名称 : FileExists											*/
/* 作用     : 确保文件存在											*/
/*																	*/
/********************************************************************/
BOOL FileExists(LPCTSTR lpszFileName, BOOL bIsDirCheck)
{
	DWORD dwAttributes = GetFileAttributes(lpszFileName);
    if (dwAttributes == 0xFFFFFFFF)
        return FALSE;
	
	if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		if (bIsDirCheck)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if (!bIsDirCheck)
			return TRUE;
		else
			return FALSE;
	}
}

/********************************************************************/
/*																	*/
/* 函数名称 : GetFileDate											*/
/* 作用     : 获取文件时间											*/
/*																	*/
/********************************************************************/
CString GetFileDate(CFileFind &find)
{
	CString strResult;
	
	CTime time = CTime::GetCurrentTime();
	
	find.GetLastWriteTime(time);
	
	CTimeSpan timeSpan = CTime::GetCurrentTime() - time;
	
	if (timeSpan.GetDays() > 356)
	{
		strResult = time.Format(" %b %d %Y ");
	}
	else
	{
		strResult.Format(" %s %02d:%02d ", time.Format("%b %d"), time.GetHour(), time.GetMinute());
	}
	return strResult;
}

/********************************************************************/
/*																	*/
/* 函数名称 : WaitWithMessageLoop									*/
/* 作用     : 等待直到收到消息										*/
/*																	*/
/********************************************************************/
BOOL WaitWithMessageLoop(HANDLE hEvent, int nTimeout)
{   
	DWORD dwRet;
	
	while (1)
	{
		dwRet = MsgWaitForMultipleObjects(1, &hEvent, FALSE, nTimeout, QS_ALLINPUT);
		if (dwRet == WAIT_OBJECT_0)
		{
			TRACE0("WaitWithMessageLoop() event triggered.\n");
			return TRUE;      
		}   
		else
		{
			if (dwRet == WAIT_OBJECT_0 + 1)
			{
				AfxGetApp()->PumpMessage();
			}  
			else
				if (dwRet == WAIT_TIMEOUT)
				{
					return FALSE;
				}
				else
				{
					return TRUE;
				}
		}

	}
}


/********************************************************************/
/*																	*/
/* 函数名称 : GetLocalHostIP										*/
/* 作用     : 获取本机IP											*/
/*																	*/
/********************************************************************/
int GetLocalHostIP(CString &strIPAddress)
{
	char szHostName[256];
	
	if(gethostname(szHostName, sizeof(szHostName)))
	{
		return -1;
	}
	
	PHOSTENT hostinfo;
	if((hostinfo = gethostbyname(szHostName)) == NULL)
		return GetLastError();
	
	LPCSTR ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
	
	strIPAddress = ip;
	
	return 0;
}