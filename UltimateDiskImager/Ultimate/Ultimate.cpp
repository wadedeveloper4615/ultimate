
// Ultimate.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Ultimate.h"
#include "MainFrm.h"
#include "UltimateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUltimateApp

BEGIN_MESSAGE_MAP(CUltimateApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CUltimateApp::OnAppAbout)
END_MESSAGE_MAP()


// CUltimateApp construction

CUltimateApp::CUltimateApp() noexcept
{
	SetAppID(_T("Ultimate.AppID.NoVersion"));
}

// The one and only CUltimateApp object

CUltimateApp theApp;


// CUltimateApp initialization

BOOL CUltimateApp::InitInstance()
{
	CWinApp::InitInstance();
	EnableTaskbarInteraction(FALSE);
	SetRegistryKey(_T("Wade Software"));
	CUltimateDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

int CUltimateApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	return CWinApp::ExitInstance();
}

// CUltimateApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnForceMbr();
	afx_msg void OnForceNoMbr();
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CUltimateApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CUltimateApp message handlers



void DumpIt(CString& csTarg, const void* buffer, DWORD start_addr, unsigned size, BOOL Append) {
    BYTE* buf = (BYTE*)buffer;
    BYTE* temp_buf;
    unsigned i;
    CString csTemp;

    if (!Append)
        csTarg.Empty();
    while (size) {
        csTemp.Format(_T("%08X  "), start_addr); csTarg += csTemp;
        start_addr += 16;
        temp_buf = buf;
        for (i = 0; (i < 16) && (i < size); i++) {
            csTemp.Format(_T("%02X%c"), *temp_buf++, (i == 7) ? ((size > 8) ? '-' : ' ') : ' ');
            csTarg += csTemp;
        }
        for (; i < 16; i++)
            csTarg += "   ";
        csTarg += "  ";
        for (i = 0; (i < 16) && (i < size); i++) {
            if (isprint(*buf))
                csTarg += *buf; //csTarg.AppendChar(*buf);
            else
                csTarg += ".";  //csTarg.AppendChar('.');
            buf++;
        }
        size -= i;
        csTarg += "\r\n";
    }
}

const TCHAR szAllowedSet[] = _T("x0123456789abcdef");
const TCHAR szDelimiter[] = _T(" ,\r\n");

// converts a string of characters (as a dump) to an array of bytes
// using any char in szDelimiter as a delimiter.
// It will ignore all chars that are in Delimiter when finding the
//  next set of byte chars.
// For example, the ' ,, ' between 0xAA and 0x04 below is a single
//  delimiter.  There is no byte between 0xAA and 0x04.
//  "0x00 0x01 0xFF,0xAA ,, 0x04 0x05 0x06\r\n 0x07 0x08 0x09";
//
int ConvertDumpToBuffer(CString csSrc, void* buffer, int max) {
    BYTE* p = (BYTE*)buffer;
    CString cs;
    int len = 0;

    csSrc.MakeLower();
    while (csSrc.GetLength() && (len < max)) {
        cs = csSrc.SpanExcluding(szAllowedSet);
        csSrc = csSrc.Right(csSrc.GetLength() - cs.GetLength());
        cs = csSrc.SpanExcluding(szDelimiter);
        csSrc = csSrc.Right(csSrc.GetLength() - cs.GetLength());
        *p++ = convert8(cs);
        len++;
    }

    return len;
}

// returns TRUE if buffer is all zeros
BOOL IsBufferEmpty(const void* buffer, const int count) {
    BYTE* p = (BYTE*)buffer;
    BYTE byte = 0;

    for (int i = 0; i < count && !byte; i++)
        byte |= p[i];

    return (byte == 0);
}

DWORD64 convert64(CString& csSrc) {
    if (csSrc.Left(2) == "0x") return (DWORD64)_tcstoui64(csSrc, NULL, 16);
    else                       return (DWORD64)_tcstoui64(csSrc, NULL, 10);
    //if (csSrc.Left(2) == "0x") return (DWORD32) strtoul(csSrc, NULL, 16);
    //else                       return (DWORD32) strtoul(csSrc, NULL, 10);
}

DWORD32 convert32(CString& csSrc) {
    if (csSrc.Left(2) == "0x") return (DWORD32)_tcstoul(csSrc, NULL, 16);
    else                       return (DWORD32)_tcstoul(csSrc, NULL, 10);
}

WORD convert16(CString& csSrc) {
    if (csSrc.Left(2) == "0x") return (WORD)_tcstoul(csSrc, NULL, 16);
    else                       return (WORD)_tcstoul(csSrc, NULL, 10);
}

BYTE convert8(CString& csSrc) {
    if (csSrc.Left(2) == "0x") return (BYTE)_tcstoul(csSrc, NULL, 16);
    else                       return (BYTE)_tcstoul(csSrc, NULL, 10);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRC32
DWORD crc32_table[256]; // CRC lookup table array.

void crc32_initialize(void) {
    memset(crc32_table, 0, sizeof(crc32_table));

    // 256 values representing ASCII character codes.
    for (int i = 0; i <= 0xFF; i++) {
        crc32_table[i] = crc32_reflect(i, 8) << 24;

        for (int j = 0; j < 8; j++)
            crc32_table[i] = (crc32_table[i] << 1) ^ ((crc32_table[i] & (1 << 31)) ? CRC32_POLYNOMIAL : 0);

        crc32_table[i] = crc32_reflect(crc32_table[i], 32);
    }
}

// Reflection is a requirement for the official CRC-32 standard.
//  You can create CRCs without it, but they won't conform to the standard.
DWORD crc32_reflect(DWORD reflect, char ch) {
    DWORD ret = 0;

    // Swap bit 0 for bit 7 bit 1 For bit 6, etc....
    for (int i = 1; i < (ch + 1); i++) {
        if (reflect & 1)
            ret |= 1 << (ch - i);
        reflect >>= 1;
    }

    return ret;
}

DWORD crc32(void* data, DWORD len) {
    DWORD crc = 0xFFFFFFFF;
    crc32_partial(&crc, data, len);
    return (crc ^ 0xFFFFFFFF);
}

void crc32_partial(DWORD* crc, void* ptr, DWORD len) {
    BYTE* data = (BYTE*)ptr;
    while (len--)
        *crc = (*crc >> 8) ^ crc32_table[(*crc & 0xFF) ^ *data++];
}


static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
    switch (uMsg) {
    case BFFM_INITIALIZED:
        if (NULL != lpData)
            SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }
    return 0;
}

// HWND is the parent window.
// On entry: csCurrent is an optional start folder. Can be empty.
// On return: csCurrent is the name only of the folder or file selected.
// szPath receives the selected path on success. Must be MAX_PATH characters in length.
BOOL BrowseForFolder(HWND hwnd, CString& csCurrent, LPTSTR szPath, UINT ulFlags) {
    LPMALLOC pMalloc;
    BOOL ret = FALSE;

    // Gets the Shell's default allocator
    if (::SHGetMalloc(&pMalloc) == NOERROR) {
        BROWSEINFO bi = { 0, };
        LPITEMIDLIST pidl;
        // Get help on BROWSEINFO struct - it's got all the bit settings.
        bi.hwndOwner = hwnd;
        bi.pidlRoot = NULL;
        bi.pszDisplayName = szPath;
        bi.lpszTitle = _T("Select a Starting Directory");
        bi.ulFlags = ulFlags;                                  // BIF_NONEWFOLDERBUTTON  | BIF_USENEWUI  ?????
        bi.lpfn = BrowseCallbackProc;
        bi.lParam = (csCurrent.IsEmpty()) ? NULL : (LPARAM)(LPCTSTR)csCurrent;
        // This next call issues the dialog box.
        if ((pidl = ::SHBrowseForFolder(&bi)) != NULL) {
            csCurrent = szPath; // return the file name in szCurrent
            ret = ::SHGetPathFromIDList(pidl, szPath);
            pMalloc->Free(pidl); // Free the PIDL allocated by SHBrowseForFolder.
        }
        pMalloc->Release(); // Release the shell's allocator.
    }

    return ret;
}

void GUID_Format(CString& csguid, S_GUID* guid) {
    csguid.Format(_T("%08X-%04X-%04X-%04X-%02X%02X%02X%02X%02X%02X"),
        guid->data1, guid->data2, guid->data3, guid->data4,
        guid->data5[0], guid->data5[1], guid->data5[2], guid->data5[3], guid->data5[4], guid->data5[5]);
}

void GUID_Retrieve(CString& csguid, S_GUID* guid) {
    CString csToken, cs;

    AfxExtractSubString(csToken, csguid, 0, '-');
    cs = _T("0x") + csToken;
    guid->data1 = convert32(cs);

    AfxExtractSubString(csToken, csguid, 1, '-');
    cs = _T("0x") + csToken;
    guid->data2 = convert16(cs);

    AfxExtractSubString(csToken, csguid, 2, '-');
    cs = _T("0x") + csToken;
    guid->data3 = convert16(cs);

    AfxExtractSubString(csToken, csguid, 3, '-');
    cs = _T("0x") + csToken;
    guid->data4 = convert16(cs);

    AfxExtractSubString(csToken, csguid, 4, '-');
    for (int i = 0; i < 6; i++) {
        cs = _T("0x") + csToken.Mid(i * 2, 2);
        guid->data5[i] = convert8(cs);
    }
}

BOOL GUID_CheckFormat(CString csguid) {
    CString csToken, csTarg;
    int i;

    AfxExtractSubString(csToken, csguid, 0, '-');
    while (csToken.GetLength() < 8)
        csToken = _T("0") + csToken;
    csToken = csToken.Left(8);
    for (i = 0; i < 8; i++) {
        if (!isxdigit(csToken.GetAt(i))) {
            csToken = _T("00000000");
            break;
        }
    }
    csTarg += csToken + '-';

    AfxExtractSubString(csToken, csguid, 1, '-');
    while (csToken.GetLength() < 4)
        csToken = _T("0") + csToken;
    csToken = csToken.Left(4);
    for (i = 0; i < 4; i++) {
        if (!isxdigit(csToken.GetAt(i))) {
            csToken = _T("0000");
            break;
        }
    }
    csTarg += csToken + '-';

    AfxExtractSubString(csToken, csguid, 2, '-');
    while (csToken.GetLength() < 4)
        csToken = _T("0") + csToken;
    csToken = csToken.Left(4);
    for (i = 0; i < 4; i++) {
        if (!isxdigit(csToken.GetAt(i))) {
            csToken = _T("0000");
            break;
        }
    }
    csTarg += csToken + '-';

    AfxExtractSubString(csToken, csguid, 3, '-');
    while (csToken.GetLength() < 4)
        csToken = _T("0") + csToken;
    csToken = csToken.Left(4);
    for (i = 0; i < 4; i++) {
        if (!isxdigit(csToken.GetAt(i))) {
            csToken = _T("0000");
            break;
        }
    }
    csTarg += csToken + '-';

    AfxExtractSubString(csToken, csguid, 4, '-');
    while (csToken.GetLength() < 12)
        csToken = _T("0") + csToken;
    csToken = csToken.Left(12);
    for (i = 0; i < 12; i++) {
        if (!isxdigit(csToken.GetAt(i))) {
            csToken = _T("000000000000");
            break;
        }
    }
    csTarg += csToken;

    return (csTarg == csguid);
}

void GUID_Create(S_GUID* guid, const DWORD type) {
    switch (type) {
    case GUID_TYPE_MT:
        memset(guid, 0, sizeof(S_GUID));
        break;
    case GUID_TYPE_EFI: // EFI System Partition
        guid->data1 = 0xC12A7328;
        guid->data2 = 0xF81F;
        guid->data3 = 0x11D2;
        guid->data4 = 0xBA4B;
        guid->data5[0] = 0x00;
        guid->data5[1] = 0xA0;
        guid->data5[2] = 0xC9;
        guid->data5[3] = 0x3E;
        guid->data5[4] = 0xC9;
        guid->data5[5] = 0x3B;
        break;
    case GUID_TYPE_LEGACY: // Partition containing a legacy MBR
        guid->data1 = 0x024DEE41;
        guid->data2 = 0x33E7;
        guid->data3 = 0x11D3;
        guid->data4 = 0x9D69;
        guid->data5[0] = 0x00;
        guid->data5[1] = 0x08;
        guid->data5[2] = 0xC7;
        guid->data5[3] = 0x81;
        guid->data5[4] = 0xF3;
        guid->data5[5] = 0x9F;
        break;
    case GUID_TYPE_RANDOM: // create a random GUID
    default:
        guid->data1 = (rand() << 16) | rand();
        guid->data2 = rand();
        guid->data3 = rand();
        guid->data4 = rand();
        guid->data5[0] = (BYTE)(rand() & 0xFF);
        guid->data5[1] = (BYTE)(rand() & 0xFF);
        guid->data5[2] = (BYTE)(rand() & 0xFF);
        guid->data5[3] = (BYTE)(rand() & 0xFF);
        guid->data5[4] = (BYTE)(rand() & 0xFF);
        guid->data5[5] = (BYTE)(rand() & 0xFF);
        break;
    }
}

float GetUTCOffset(void) {
    CTime time(CTime::GetCurrentTime());
    tm t1, t2;
    time.GetLocalTm(&t1);
    time.GetGmtTm(&t2);
    //ATLTRACE(_T("Difference between local time and GMT is %d hours.\n"), t1.tm_hour - t2.tm_hour);
    return (float)(t1.tm_hour - t2.tm_hour);
}

BOOL fnmatch(const char* wild_text, const char* tame_text, const DWORD flags) {
    const BOOL case_sensitive = (flags & FNM_NOCASE) ? FALSE : TRUE;
    BOOL match = TRUE;
    char* after_last_wild = NULL; // The location after the last '*', if we?ve encountered one
    char* after_last_tame = NULL; // The location in the tame string, from which we started after last wildcard
    char t, w;

    // Walk the text strings one character at a time.
    while (1) {
        t = *tame_text;
        w = *wild_text;

        // How do you match a unique text string?
        if (!t || (t == '\0')) {
            // Easy: unique up on it!
            if (!w || (w == '\0'))
                break;                             // "x" matches "x"
            else if (w == '*') {
                wild_text++;
                continue;                          // "x*" matches "x" or "xy"
            }
            else if (after_last_tame) {
                if (!(*after_last_tame) || (*after_last_tame == '\0')) {
                    match = FALSE;
                    break;
                }
                tame_text = after_last_tame++;
                wild_text = after_last_wild;
                continue;
            }

            match = FALSE;
            break;                                     // "x" doesn't match "xy"
        }
        else {
            if (!case_sensitive) {
                // Lowercase the characters to be compared.
                if (t >= 'A' && t <= 'Z')
                    t += ('a' - 'A');

                if (w >= 'A' && w <= 'Z')
                    w += ('a' - 'A');
            }

            // How do you match a tame text string?
            if ((t != w) && (w != '?')) {
                // The tame way: unique up on it!
                if (w == '*') {
                    after_last_wild = (char*) ++wild_text;
                    after_last_tame = (char*)tame_text;
                    w = *wild_text;

                    if (!w || (w == '\0'))
                        break; // "*" matches "x"
                    continue; // "*y" matches "xy"
                }
                else if (after_last_wild) {
                    if (after_last_wild != wild_text) {
                        wild_text = after_last_wild;
                        w = *wild_text;
                        if (!case_sensitive && (w >= 'A') && (w <= 'Z'))
                            w += ('a' - 'A');
                        if ((t == w) || (t == '?'))
                            wild_text++;
                    }
                    tame_text++;
                    continue;    // "*sip*" matches "mississippi"
                }
                else {
                    match = FALSE;
                    break; // "x" doesn't match "y"
                }
            }
        }

        tame_text++;
        wild_text++;
    }

    return match;
}

/////////////////////////////////////////////////////////////////////////////
// Convert LBA to CHS with given parameters
// Sector   = (LBA mod SPT) + 1       (SPT = Sectors per Track)
// Head     = (LBA  /  SPT) mod Heads
// Cylinder = (LBA  /  SPT)  /  Heads
void gLBAtoCHS(const DWORD lba, WORD* cyl, BYTE* head, BYTE* sect, const DWORD spt, const DWORD heads) {
    if (lba < (1024 * spt * heads)) {
        if (sect) *sect = (BYTE)((lba % 63) + 1);
        if (head) *head = (BYTE)((lba / 63) % 16);
        if (cyl) *cyl = (WORD)((lba / 63) / 16);
    }
    else {
        if (sect) *sect = 0xFF;
        if (head) *head = 0xFE;
        if (cyl) *cyl = 0xFF;
    }
}

/////////////////////////////////////////////////////////////////////////////
// returns TRUE if val is a power of 2
BOOL power_of_two(DWORD val) {
    return ((val > 1) && ((val & (val - 1)) == 0));
}
