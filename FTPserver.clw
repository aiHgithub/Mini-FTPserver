; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CFTPserverDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "FTPserver.h"

ClassCount=9
Class1=CFTPserverApp
Class2=CFTPserverDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_QUIT
Resource2=IDR_MAINFRAME
Class4=CListenSocket
Class5=CConnectThread
Class6=CControlSocket
Class7=CDataSocket
Resource3=IDR_POPUP
Class8=CSystemTray
Resource4=IDD_ABOUTBOX
Class9=CDlgQuit
Resource5=IDD_FTPSERVER_DIALOG

[CLS:CFTPserverApp]
Type=0
HeaderFile=FTPserver.h
ImplementationFile=FTPserver.cpp
Filter=N
LastObject=ID_APP_SHOW

[CLS:CFTPserverDlg]
Type=0
HeaderFile=FTPserverDlg.h
ImplementationFile=FTPserverDlg.cpp
Filter=D
LastObject=CFTPserverDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=FTPserverDlg.h
ImplementationFile=FTPserverDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_FTPSERVER_DIALOG]
Type=1
Class=CFTPserverDlg
ControlCount=26
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342177294
Control9=IDC_STATIC,static,1342308353
Control10=IDC_EDIT_PORT,edit,1350631552
Control11=IDC_EDIT_MAXUSER,edit,1350631552
Control12=IDC_EDIT_USER,edit,1350631552
Control13=IDC_EDIT_PASSWORD,edit,1350631584
Control14=IDC_EDIT_WELCOME,edit,1350631552
Control15=IDC_EDIT_QUIT,edit,1350631552
Control16=IDC_EDIT_FOLDER,edit,1350631552
Control17=IDC_BTN_BROWSE,button,1342242816
Control18=IDC_CHECK_DOWNLOAD,button,1342242819
Control19=IDC_CHECK_UPLOAD,button,1342242819
Control20=IDC_CHECK_RENAME,button,1342242819
Control21=IDC_CHECK_DELETE,button,1342242819
Control22=IDC_CHECK_CREATEDIR,button,1342242819
Control23=IDC_BTN_START,button,1342242817
Control24=IDC_BTN_STOP,button,1342242816
Control25=IDC_BTN_QUIT,button,1342242816
Control26=IDC_STATE,static,1342308864

[CLS:CListenSocket]
Type=0
HeaderFile=ListenSocket.h
ImplementationFile=ListenSocket.cpp
BaseClass=CAsyncSocket
Filter=N
VirtualFilter=q
LastObject=CListenSocket

[CLS:CConnectThread]
Type=0
HeaderFile=ConnectThread.h
ImplementationFile=ConnectThread.cpp
BaseClass=CWinThread
Filter=N
LastObject=CConnectThread

[CLS:CControlSocket]
Type=0
HeaderFile=ControlSocket.h
ImplementationFile=ControlSocket.cpp
BaseClass=CSocket
Filter=N
LastObject=CControlSocket
VirtualFilter=uq

[CLS:CDataSocket]
Type=0
HeaderFile=DataSocket.h
ImplementationFile=DataSocket.cpp
BaseClass=CAsyncSocket
Filter=N
VirtualFilter=q
LastObject=CDataSocket

[MNU:IDR_POPUP]
Type=1
Class=?
Command1=ID_APP_SHOW
Command2=ID_APP_EXIT
CommandCount=2

[CLS:CSystemTray]
Type=0
HeaderFile=SystemTray.h
ImplementationFile=SystemTray.cpp
BaseClass=CDialog
Filter=D
LastObject=CSystemTray

[DLG:IDD_QUIT]
Type=1
Class=CDlgQuit
ControlCount=3
Control1=IDC_BTN_MINSIZE,button,1342242816
Control2=IDC_BTN_EXIT,button,1342242816
Control3=IDC_BTN_CANCEL,button,1342242816

[CLS:CDlgQuit]
Type=0
HeaderFile=DlgQuit.h
ImplementationFile=DlgQuit.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_BTN_MINSIZE

