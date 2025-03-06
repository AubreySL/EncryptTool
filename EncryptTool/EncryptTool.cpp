// EncryptTool.cpp : 定义应用程序的入口点。
//
#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "EncryptTool.h"
#include "Encrypto.h"
#include "string.h"
#include "windowsx.h"
#include "Base64.h"


#define MAX_LOADSTRING 100

#pragma comment(lib, "Comctl32.lib")

LPCWSTR char_to_lpcwstr(const char* cstr);
char* tchar_to_char(const TCHAR* tchar);
LRESULT CALLBACK EditTextProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditTextDecryptProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
WNDPROC OldTextProc;
WNDPROC OldTextDecodeProc;

#define IDC_MY_EDIT_ONE 10  //编辑框的ID 自己定义即可.
#define IDC_MY_EDIT_TWO 11 
#define IDC_MY_EDIT_THREE 12

#define IDC_MY_BUTTON_ONE 13
#define IDC_MY_BUTTON_TWO 14
#define IDC_STATUS_BAR 15
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。


    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ENCRYPTTOOL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENCRYPTTOOL));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENCRYPTTOOL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ENCRYPTTOOL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, 
       WS_OVERLAPPED | WS_CAPTION |
       WS_SYSMENU |
       WS_THICKFRAME |
       WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);


   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndEditContent;
    static HWND hwndEditKey;
    static HWND hwndEditTransformed;
    static HWND hwndButton;
    static HWND hwndButtonDecrypt;
    static HWND hwndStatusBar;
    static int cxClient;        // 客户区宽度
    static int cyClient;        // 客户区高度

    TCHAR lpszLatin[] = TEXT("Please input contents.");
    //获得父窗口的客服区坐标
    RECT rcParent;
    GetClientRect(hWnd, &rcParent);
    INT parentWindowWidth = rcParent.right - rcParent.left;
    INT parentWindowHeight = rcParent.bottom - rcParent.top;
    INT inputComHeight = (parentWindowHeight - 20 - 80) / 2 ;//20: margin 70:
    switch (message)
    {
    
    case WM_CREATE:
       
        //original text
        hwndEditContent = CreateWindowExW(
            0, L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            10, 10, parentWindowWidth - 20, inputComHeight,   // set size in WM_SIZE message 
            hWnd,         // parent window 
            (HMENU)IDC_MY_EDIT_ONE,   // edit control ID 
            (HINSTANCE)GetWindowLongPtrW(hWnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 

        RECT rcEdit;
        GetWindowRect(hwndEditContent, &rcEdit);
        //important key
        hwndEditKey = CreateWindowEx(
            0, L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_CHILD | WS_VISIBLE  | WS_BORDER | ES_LEFT,
            10,         // x position 
            rcEdit.bottom - rcEdit.top + 20,         // y position 
            300, 40,   // set size in WM_SIZE message 
            hWnd,         // parent window 
            (HMENU)IDC_MY_EDIT_TWO, // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        RECT rcKey;
        GetWindowRect(hwndEditKey, &rcKey);
        //encrypt button
        hwndButton = CreateWindowEx(
            0,
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"加密",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            rcKey.right+10,         // x position 
            rcEdit.bottom - rcEdit.top + 20,         // y position 
            100,        // Button width
            40,        // Button height
            hWnd,     // Parent window
            (HMENU)IDC_MY_BUTTON_ONE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        //Decrypt button
        hwndButtonDecrypt = CreateWindowEx(
            0,
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"解密",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            rcEdit.right / 2,         // x position 
            rcEdit.bottom - rcEdit.top + 20,         // y position 
            100,        // Button width
            40,        // Button height
            hWnd,     // Parent window
            (HMENU)IDC_MY_BUTTON_TWO,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        //decrypt text 
        hwndEditTransformed = CreateWindowEx(
            0, L"EDIT",   // predefined class 
            NULL,         // no window title 
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            10, rcEdit.bottom - rcEdit.top + 70, parentWindowWidth - 20, inputComHeight,   // set size in WM_SIZE message 
            hWnd,         // parent window 
            (HMENU)IDC_MY_EDIT_THREE,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);        // pointer not needed 
        //create status bar
        hwndStatusBar = CreateWindowEx(0,TEXT("msctls_statusbar32"),
            NULL,
            WS_CHILD | WS_VISIBLE,
            0,0,0,0,
            hWnd,
            (HMENU)IDC_STATUS_BAR,   // edit control ID 
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL
            );

        SendMessage(hwndStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)TEXT("Ready!"));

        //for test data - start
        //SetDlgItemText(hWnd, IDC_MY_EDIT_ONE, TEXT("hello world!"));
        //SetDlgItemText(hWnd, IDC_MY_EDIT_TWO, TEXT("qwe"));
        //SetDlgItemText(hWnd, IDC_MY_EDIT_THREE, TEXT("MTZARG42Nzl0Z2VBcm4rSTVYa1BpQkdMQT09"));
        //for test data - end
        // 
         //save previous proc
        OldTextProc = (WNDPROC)SetWindowLongPtr(hwndEditContent, GWLP_WNDPROC, (LONG_PTR)EditTextProc);
        OldTextDecodeProc = (WNDPROC)SetWindowLongPtr(hwndEditTransformed, GWLP_WNDPROC, (LONG_PTR)EditTextDecryptProc);

        return 0;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBoxW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            case IDC_MY_BUTTON_ONE:
            {
                //原文内容
                TCHAR *str=NULL;
                //密钥内容
                TCHAR *str2 =NULL;

                BYTE pData[MAX_PATH] = {0};
                DWORD dwDataLength = 0;

                int strLength = Edit_GetTextLength(hwndEditContent);
                if (!strLength) {
                    MessageBox(hWnd, TEXT("原文内容不能为空！"), TEXT("提示消息"), MB_OK);
                    return false;
                }
                if (strLength > MAX_PATH) {
                    MessageBox(hWnd, TEXT("原文内容超过260字符！"), TEXT("提示消息"), MB_OK);
                    return false;
                }
                size_t bufferLength = (strLength + 1) * sizeof(TCHAR);
                str = (TCHAR*)malloc(bufferLength);

                GetDlgItemText(hWnd, IDC_MY_EDIT_ONE, str, bufferLength);
                //MessageBox(hWnd, str, TEXT("原文内容:"), MB_OK);

                int str2Length = Edit_GetTextLength(hwndEditKey);
                if (!str2Length) {
                    MessageBox(hWnd, TEXT("密钥内容不能为空！"), TEXT("提示消息"), MB_OK);
                    return false;
                }
                size_t keyLength = (str2Length + 1) * sizeof(TCHAR);
                str2 = (TCHAR*)malloc(keyLength);

              
                GetDlgItemText(hWnd, IDC_MY_EDIT_TWO, str2, keyLength);
                //MessageBox(hWnd,str2, TEXT("密钥内容："), MB_OK);

                // 将 TCHAR 转为 char
                char* souce = tchar_to_char(str);
                strcpy_s((char*)pData, MAX_PATH, souce);
               
                char* key = tchar_to_char(str2);

                //原文长度
                dwDataLength =(DWORD)::strlen((char*)souce);
                //调用加密算法
                AesEncrypt((BYTE*)key, pData, dwDataLength, MAX_PATH);

                std::string encode_text="";
                for (size_t i = 0; i < dwDataLength; i++)
                {
                    encode_text += pData[i];
                }
                size_t  test = encode_text.length();
               //密文长度 + @ + 密文 base64 编码
                std::string encode_result = base64_encode(encode_text);
                std::string prefix(std::to_string(dwDataLength));
                prefix.append("@");
                std::string encode_result_all;
                encode_result_all.append(prefix);
                encode_result_all.append(encode_result);
                size_t len = encode_result_all.length();

                std::string second_encode_result = base64_encode(encode_result_all);
                LPCWSTR text_result = char_to_lpcwstr(second_encode_result.c_str());
                //MessageBox(hWnd, text_result, TEXT("加密后:"), NULL);
                SetDlgItemText(hWnd, IDC_MY_EDIT_THREE, text_result);
                SendMessage(hwndStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)TEXT("加密完成"));
                // 使用完后删除分配的内存
                free(str);
                str = NULL;
                free(str2);
                str2 = NULL;
                //free(pData);
                //pData = {0};

                souce = NULL;
                key = NULL;

                break;
            }
            
            case IDC_MY_BUTTON_TWO:
            {
                //密文内容
                TCHAR* str = NULL;
                //密钥内容
                TCHAR* str2 = NULL;

                BYTE pData[MAX_PATH] = { 0 };

                DWORD dwDataLength = 0;
               
                int strLength = Edit_GetTextLength(hwndEditTransformed);
                if (!strLength) {
                    MessageBox(hWnd, TEXT("密文内容不能为空！"), TEXT("提示消息"), MB_OK);
                    return false;
                }
                size_t bufferLength = (strLength + 1) * sizeof(TCHAR);
                str = (TCHAR*)malloc(bufferLength);

                GetDlgItemText(hWnd, IDC_MY_EDIT_THREE, str, bufferLength);
                //MessageBox(hWnd, str, TEXT("密文内容:"), MB_OK);

                int str2Length = Edit_GetTextLength(hwndEditKey);
                if (!str2Length) {
                    MessageBox(hWnd, TEXT("密钥内容不能为空！"), TEXT("提示消息"), MB_OK);
                    return false;
                }
                size_t keyLength = (str2Length + 1) * sizeof(TCHAR);
                str2 = (TCHAR*)malloc(keyLength);

                GetDlgItemText(hWnd, IDC_MY_EDIT_TWO, str2, keyLength);
                char* key = tchar_to_char(str2);
                //MessageBox(hWnd, str2, TEXT("密钥内容："), MB_OK);

                // 将 TCHAR 转为 char
                char* souce = tchar_to_char(str);

                std::string decode_first_text(souce);
                //dedcode
                std::string decode_first_result = base64_decode(decode_first_text);

                int pos = decode_first_result.find("@");
                if (pos < 0) {
                    MessageBox(hWnd, TEXT("base64 解码有误"), TEXT("提示:"), NULL);
                    return false;
                }
                std::string decode_substr_one = decode_first_result.substr(0, pos);
                std::string decode_substr_two = decode_first_result.substr(pos+1);
                std::string decode_second_result = base64_decode(decode_substr_two);

                dwDataLength = static_cast<DWORD>(std::stoi(decode_substr_one));

                int size = decode_second_result.length();
                if (dwDataLength) {
                    for (size_t i = 0; i < dwDataLength; i++)
                    {
                        pData[i] = decode_second_result[i];
                    }
                }
                else {
                    strcpy_s((char*)pData, MAX_PATH, decode_second_result.c_str());
                }
                
                
               //解密
                AesDecrypt((BYTE*)key, pData, dwDataLength, MAX_PATH);

                std::string temp;
                for (size_t i = 0; i < dwDataLength; i++)
                {
                    temp += pData[i];
                }
                LPCWSTR result = char_to_lpcwstr(temp.c_str());
                //MessageBox(hWnd, result, NULL, NULL);
                SetDlgItemText(hWnd, IDC_MY_EDIT_ONE, result);
                SendMessage(hwndStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)TEXT("解密完成"));
                // 使用完后删除分配的内存
                free(str);
                str = NULL;
                free(str2);
                str2 = NULL;
                souce = NULL;
                key = NULL;
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
           
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        // Remove the subclass from the edit control. 
        SetWindowLong(hwndEditContent, GWLP_WNDPROC, (LONG_PTR)OldTextProc);
        SetWindowLong(hwndEditTransformed, GWLP_WNDPROC, (LONG_PTR)OldTextDecodeProc);
        PostQuitMessage(0);
        break;
    case WM_SIZE: {
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        MoveWindow(hwndEditContent, 10, 10, cxClient - 20, inputComHeight, TRUE);

        RECT rcEdit;
        GetWindowRect(hwndEditContent, &rcEdit);
        MoveWindow(hwndEditKey, 10, rcEdit.bottom - rcEdit.top + 20, 300, 40, TRUE);

        RECT rcKey;
        GetWindowRect(hwndEditKey, &rcKey);
        MoveWindow(hwndButton, rcKey.right, rcEdit.bottom - rcEdit.top + 20, 100, 40, TRUE);
        RECT rcEncryBtn;
        GetWindowRect(hwndButton, &rcEncryBtn);
        MoveWindow(hwndButtonDecrypt, rcEncryBtn.right, rcEdit.bottom - rcEdit.top + 20, 100, 40, TRUE);

        MoveWindow(hwndEditTransformed, 10, rcEdit.bottom - rcEdit.top + 70, cxClient - 20, inputComHeight, TRUE);

        SendMessage(hwndStatusBar, WM_SIZE, 0, 0);


        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
 
// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

char* tchar_to_char(const TCHAR* tchar)
{
    int iLength;
    //获取字节长度   
    iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
    // 创建char数组，并进行转换
    char* cstr = new char[iLength];
    //将tchar值赋给_char    
    WideCharToMultiByte(CP_ACP, 0, tchar, -1, cstr, iLength, NULL, NULL);
    return cstr;
}

LPCWSTR char_to_lpcwstr(const char* cstr) {
    if (cstr == nullptr) return nullptr;

    int size = MultiByteToWideChar(CP_ACP, 0, cstr, -1, nullptr, 0);
    if (size <= 0) return nullptr;

    WCHAR* buffer = new WCHAR[size];
    MultiByteToWideChar(CP_ACP, 0, cstr, -1, buffer, size);

    return buffer;
}

LRESULT CALLBACK EditTextProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_KEYDOWN:
        {
             //A: 0x41 229:chinese code
            if ((wParam == 0x41 || wParam == 229) && GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, EM_SETSEL, 0, -1);
            }
            if ((wParam == 0x43 || wParam == 229) && GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, WM_COPY, 0, 0);
            }
            return 0;
        }
    }   

    return CallWindowProc(OldTextProc, hWnd, msg, wParam, lParam); 
}

LRESULT CALLBACK EditTextDecryptProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_KEYDOWN:
        {
            if ((wParam == 0x41 || wParam == 229) && GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, EM_SETSEL, 0, -1);
            }
            if ((wParam == 0x43 || wParam == 229) && GetKeyState(VK_CONTROL) < 0) {
                SendMessage(hWnd, WM_COPY, 0, 0);
            }
            return 0;
        }
    } 

    return CallWindowProc(OldTextDecodeProc, hWnd, msg, wParam, lParam);
}

