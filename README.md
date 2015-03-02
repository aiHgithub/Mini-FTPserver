# Slyar FTPserver
http://www.slyar.com/blog/slyar-ftpserver.html

![Slyar FTPserver](http://www.slyar.com/blog/wp-content/uploads/2010/12/SlyarFTP.png)

Multi-threaded FTP Server, implemented blocking/synchronous and unblocking/asynchronous socket;

Functions: user manager, upload/download/edit/delete privilege manager, autostart, desktop tray;

**FTPserver.dsp**
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

**FTPserver.h**
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CFTPserverApp application class.

**FTPserver.cpp**
    This is the main application source file that contains the application
    class CFTPserverApp.

**FTPserver.rc**
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

**FTPserver.clw**
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

**res\FTPserver.ico**
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file FTPserver.rc.

**res\FTPserver.rc2**
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.

**FTPserverDlg.h**, **FTPserverDlg.cpp** - the dialog
    These files contain your CFTPserverDlg class.  This class defines
    the behavior of your application's main dialog.  The dialog's
    template is in FTPserver.rc, which can be edited in Microsoft
	Visual C++.

**StdAfx.h**, **StdAfx.cpp**
    These files are used to build a precompiled header (PCH) file
    named FTPserver.pch and a precompiled types file named StdAfx.obj.

**Resource.h**
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.
