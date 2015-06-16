
// shanxunDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "shanxun.h"
#include "shanxunDlg.h"
#include "afxdialogex.h"
#include <stdio.h>
#include "md5.h"
#include<ras.h>
#include <winhttp.h>

#include<winsock2.h>
//连接 ws2_32.lib
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winhttp.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"RASAPI32.Lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
    byte username[35]="@GDPF.XY";
	char *mm="";
    byte res[35];
CString user="",password="";//账号信息

static void getPIN(byte *userName, byte *PIN)
{
    int i,j;//循环变量
    long timedivbyfive;//时间除以五
    time_t timenow;//当前时间，从time()获得
    byte RADIUS[16]="singlenet01";//凑位字符
    byte timeByte[4];//时间 div 5
    byte beforeMD5[32];//时间 div 5+用户名+凑位
    CMD5 md5;//MD5结构体
    byte afterMD5[16];//MD5输出
    char MD501H[2]; //MD5前两位
    byte MD501[3];
    byte timeHash[4]; //时间div5经过第一次转后后的值
    byte temp[32]; //第一次转换时所用的临时数组
    byte PIN27[6]; //PIN的2到7位，由系统时间转换

    //code
    //info("sxplugin : using zjxinlisx01");
    //strcpy(RADIUS, "zjxinlisx01");
    timenow = time(NULL);
    timedivbyfive = timenow / 5;

    for(i = 0; i < 4; i++) {
        timeByte[i] = (byte)(timedivbyfive >> (8 * (3 - i)) & 0xFF);
    }
    for(i = 0; i < 4; i++) {
        beforeMD5[i]= timeByte[i];
    }
    for(i = 4; i < 16 && userName[i-4]!='@' ; i++) {
        beforeMD5[i] = userName[i-4];
    }
    j=0;
    while(RADIUS[j]!='\0')
        beforeMD5[i++] = RADIUS[j++];

    md5.GenerateMD5(beforeMD5,i);

   // MD5_Update (&md5, beforeMD5, i);
   // printf("%d,%s\n",i,beforeMD5);
    //MD5_Final (afterMD5, &md5);
    string t=md5.ToString();
    for(int i=0;i<16;i++)
        afterMD5[i]=t[i];
    //afterMD5=t;
    //cout<<t<<","<<t.size()<<endl;;
//就是这一块，翻译成JAVA时总是失败
  /*  MD501H[0] = afterMD5[0] >> 4 & 0xF;
    MD501H[1] = afterMD5[0] & 0xF;

    sprintf((char*)MD501,"%x%x",(char)MD501H[0],(char)MD501H[1]);//到这里
*/
    MD501[0]=t[0];
    MD501[1]=t[1];
//我翻译的java代码
/* M
        D501H[0] = (char) (afterMD5[0] >> 4 & 0xF);
        MD501H[1] = (char) (afterMD5[0] & 0xF);
        byte MD1=(byte)(MD501H[0]);byte MD2=(byte)(MD501H[1]);

       String MD=String.format("%x%x",MD1,MD2);
       MD501=MD.toCharArray();*/


    for(i = 0; i < 32; i++) {
        temp[i] = timeByte[(31 - i) / 8] & 1;
        timeByte[(31 - i) / 8] = timeByte[(31 - i) / 8] >> 1;
    }

    for (i = 0; i < 4; i++) {
        timeHash[i] = temp[i] * 128 + temp[4 + i] * 64 + temp[8 + i]
            * 32 + temp[12 + i] * 16 + temp[16 + i] * 8 + temp[20 + i]
            * 4 + temp[24 + i] * 2 + temp[28 + i];
    }

    temp[1] = (timeHash[0] & 3) << 4;
    temp[0] = (timeHash[0] >> 2) & 0x3F;
    temp[2] = (timeHash[1] & 0xF) << 2;
    temp[1] = (timeHash[1] >> 4 & 0xF) + temp[1];
    temp[3] = timeHash[2] & 0x3F;
    temp[2] = ((timeHash[2] >> 6) & 0x3) + temp[2];
    temp[5] = (timeHash[3] & 3) << 4;
    temp[4] = (timeHash[3] >> 2) & 0x3F;

    for (i = 0; i < 6; i++) {
        PIN27[i] = temp[i] + 0x020;
        if(PIN27[i]>=0x40) {
            PIN27[i]++;
        }
    }

    PIN[0] = '\r';
    PIN[1] = '\n';

    memcpy(PIN+2, PIN27, 6);

    PIN[8] = MD501[0];
    PIN[9] = MD501[1];

    strcpy((char*)PIN+10, (char*)userName);
}
HRASCONN handle=NULL;
void ras(){
    RASDIALPARAMS  ms;
    RASDIALPARAMS params;
	handle=NULL;
    memset(&ms, '\0', sizeof(params));
    params.dwSize=sizeof(RASDIALPARAMS);
    strcpy(params.szEntryName,"shanxun");
    strcpy(params.szPhoneNumber,"");
    strcpy(params.szCallbackNumber,"");


	strcpy(params.szPassword, password.GetString());  //密码
    strcpy(params.szDomain,  "");
	getPIN((byte *)user.GetString(),res);
    strcpy(params.szUserName,(char*)res);
    int a =RasDial(NULL, NULL, &params, NULL, NULL, &handle);
    if (a!=ERROR_SUCCESS)
    {
		
        printf("正在拨打的计算机没有应答，稍后请再试");
        DWORD off=RasHangUp(handle);
         if (off==0)
        {
         printf("连接已断开...\n");
        }
         else{
          printf("断开连接出错...\n");
        }
    }
    else
    {

       // cout<<"连接成功\n"<<endl;
    }

}
bool httpsend(){
  DWORD dwSize = 0;
  DWORD dwDownloaded = 0;
  LPSTR pszOutBuffer;
  BOOL  bResults = FALSE;
  HINTERNET  hSession = NULL,
             hConnect = NULL,
             hRequest = NULL;

  // Use WinHttpOpen to obtain a session handle.
  hSession = WinHttpOpen( L"WinHTTP Example/1.0",
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME,
                          WINHTTP_NO_PROXY_BYPASS, 0 );

  // Specify an HTTP server.
  if( hSession )
    hConnect = WinHttpConnect( hSession, L"127.0.0.1",
                               INTERNET_DEFAULT_HTTP_PORT, 0 );

  // Create an HTTP request handle.
  if( hConnect )
    hRequest = WinHttpOpenRequest( hConnect, L"GET",
									NULL,//pwszObjectName
									NULL, WINHTTP_NO_REFERER,
                                   WINHTTP_DEFAULT_ACCEPT_TYPES,
                                   WINHTTP_FLAG_SECURE );
    else
        return false;
  if (hRequest&&0)
	  bResults = WinHttpAddRequestHeaders(hRequest,
	  L"If-Modified-Since: Mon, 20 Nov 2000 20:00:00 GMT",
	  (ULONG)-1L,
	  WINHTTP_ADDREQ_FLAG_ADD);

  // Send a request.
  if( hRequest )
    bResults = WinHttpSendRequest( hRequest,
                                   WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                   WINHTTP_NO_REQUEST_DATA, 0,
                                   0, 0 );


  // End the request.
  if( bResults )
    bResults = WinHttpReceiveResponse( hRequest, NULL );
  else
	  return false;
  // Keep checking for data until there is nothing left.
  if( bResults )
  {
    do
    {
      // Check for available data.
      dwSize = 0;
      if( !WinHttpQueryDataAvailable( hRequest, &dwSize ) )
        printf( "Error %u in WinHttpQueryDataAvailable.\n",
                GetLastError( ) );

      // Allocate space for the buffer.
      pszOutBuffer = new char[dwSize+1];
      if( !pszOutBuffer )
      {
        printf( "Out of memory\n" );
        dwSize=0;
      }
      else
      {
        // Read the data.
        ZeroMemory( pszOutBuffer, dwSize+1 );

        if( !WinHttpReadData( hRequest, (LPVOID)pszOutBuffer,
                              dwSize, &dwDownloaded ) )
          printf( "Error %u in WinHttpReadData.\n", GetLastError( ) );
        else
          printf( "%s", pszOutBuffer );

        // Free the memory allocated to the buffer.
        delete [] pszOutBuffer;
      }
    } while( dwSize > 0 );
  }


  // Report any errors.
  if( !bResults )
    printf( "Error %d has occurred.\n", GetLastError( ) );

  // Close any open handles.
  if( hRequest ) WinHttpCloseHandle( hRequest );
  if( hConnect ) WinHttpCloseHandle( hConnect );
  if( hSession ) WinHttpCloseHandle( hSession );
  return true;
}

#define IsHexNum(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))

CString Utf8ToStringT(LPSTR str)
{
	_ASSERT(str);
	USES_CONVERSION;
	WCHAR *buf;
	int length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	buf = new WCHAR[length + 1];
	ZeroMemory(buf, (length + 1) * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, length);

	return (CString(W2T(buf)));
}
CString UrlDecode(LPCTSTR url)
{
	_ASSERT(url);
	USES_CONVERSION;
	LPSTR _url = T2A(const_cast<LPTSTR>(url));
	int i = 0;
	int length = (int)strlen(_url);
	CHAR *buf = new CHAR[length];
	ZeroMemory(buf, length);
	LPSTR p = buf;
	char tmp[4];
	while (i < length)
	{
		if (i <= length - 3 && _url[i] == '%' && IsHexNum(_url[i + 1]) && IsHexNum(_url[i + 2]))
		{
			memset(tmp, 0, sizeof(tmp));
			memcpy(tmp, _url + i + 1, 2);
			sscanf(tmp, "%x", p++);
			i += 3;
		}
		else
		{
			*(p++) = _url[i++];
		}
	}
	return Utf8ToStringT(buf);
}
std::string UrlEncode(const std::string& szToEncode)
{
	std::string src = szToEncode;
	char hex[] = "0123456789ABCDEF";
	string dst;

	for (size_t i = 0; i < src.size(); ++i)
	{
		unsigned char cc = src[i];
		if (isascii(cc))
		{
			if (cc == ' ')
			{
				dst += "%20";
			}
			else
				dst += cc;
		}
		else
		{
			unsigned char c = static_cast<unsigned char>(src[i]);
			dst += '%';
			dst += hex[c / 16];
			dst += hex[c % 16];
		}
	}
	return dst;
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
END_MESSAGE_MAP()


// CshanxunDlg 对话框

BEGIN_DHTML_EVENT_MAP(CshanxunDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



CshanxunDlg::CshanxunDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CshanxunDlg::IDD, CshanxunDlg::IDH, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CshanxunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CshanxunDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON1, &CshanxunDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_WM_DESTROY()
//	ON_EN_CHANGE(IDC_EDIT3, &CshanxunDlg::OnEnChangeEdit3)
ON_BN_CLICKED(IDC_BUTTON1, &CshanxunDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CshanxunDlg 消息处理程序

BOOL CshanxunDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_EDIT1)->SetWindowTextA(user);
	GetDlgItem(IDC_EDIT2)->SetWindowTextA(password);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CshanxunDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CshanxunDlg::OnPaint()
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
		CDHtmlDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CshanxunDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CshanxunDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CshanxunDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}


//void CAboutDlg::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if(!httpsend())
//        ras();
//    else
//		this->MessageBox("连接成功");
//	CDialogEx::OnTimer(nIDEvent);
//}


void CshanxunDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	 WORD  wVersionRequested; 
 WSADATA  wsaData; 
 char  name[255]; 
 CString  ip;
 int err;

 PHOSTENT  hostinfo; 
 wVersionRequested  =  MAKEWORD( 1, 1 );//版本号1.1

 //1.加载套接字库 
 err = WSAStartup( wVersionRequested, &wsaData );
 if ( err != 0 ) {
  return;
 }

 //判断是否我们请求的winsocket版本，如果不是
 //则调用WSACleanup终止winsocket的使用并返回            

 if ( LOBYTE( wsaData.wVersion ) != 1 ||
  HIBYTE( wsaData.wVersion ) != 1 ) {

   WSACleanup( );
   return;
  }///...if

 if  ( err  ==  0  ) 
 { 
  
  if(  gethostname  (  name,  sizeof(name))  ==  0) 
  { 
   if((hostinfo  =  gethostbyname(name))  !=  NULL) 
   { 
    ip  =  inet_ntoa  (*(struct  in_addr  *)*hostinfo->h_addr_list); 
   } 
  } 
  
  WSACleanup(  ); 
 }   

 this->GetDlgItem(IDC_EDIT1)->GetWindowTextA(user);
	GetDlgItem(IDC_EDIT2)->GetWindowTextA(password);
//CString m_IPAddress = ip;
GetDlgItem(IDC_STATIC)->SetWindowTextA(ip);
 if(ip[0]=='1'&&ip[1]=='1'&&ip[2]=='5')
	 ;
 else
     ras();
	
	CDHtmlDialog::OnTimer(nIDEvent);
}


void CshanxunDlg::OnDestroy()
{
	CDHtmlDialog::OnDestroy();
	RasHangUp(handle);
	// TODO: 在此处添加消息处理程序代码
}


//void CshanxunDlg::OnEnChangeEdit3()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDHtmlDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//}


void CshanxunDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	SetTimer(1, 2000, NULL);
}
