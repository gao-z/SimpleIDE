#include <windows.h>
#include <commdlg.h>
#include "resource.h"
#include "IDEFile.h"
#include "CXCompiler.h"

#define ID_EDIT     101
#define UNTITLED TEXT ("(untitled)")

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
TCHAR    szAppName[] = TEXT ("SimpleIDE") ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     HACCEL   hAccel ;
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON)) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = szAppName ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("Failed to RegisterClass!"),
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }

     hwnd = CreateWindow (szAppName, szAppName,
                          WS_OVERLAPPEDWINDOW,
                          GetSystemMetrics (SM_CXSCREEN) / 4,
                          GetSystemMetrics (SM_CYSCREEN) / 4,
                          GetSystemMetrics (SM_CXSCREEN) / 2,
                          GetSystemMetrics (SM_CYSCREEN) / 2,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;

     hAccel = LoadAccelerators (hInstance, szAppName) ;

     while (GetMessage (&msg, NULL, 0, 0))
     {
          if (!TranslateAccelerator (hwnd, hAccel, &msg))
          {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
          }
     }
     return msg.wParam ;
}

void DoCaption (HWND hwnd, TCHAR * szTitleName)
{
     TCHAR szCaption[64 + MAX_PATH] ;

     wsprintf (szCaption, TEXT ("%s - %s"), szAppName,
               szTitleName[0] ? szTitleName : UNTITLED) ;

     SetWindowText (hwnd, szCaption) ;
}

void OkMessage (HWND hwnd, TCHAR * szMessage, TCHAR * szTitleName)
{
     TCHAR szBuffer[64 + MAX_PATH] ;

     wsprintf (szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED) ;

     MessageBox (hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
}

short AskAboutSave (HWND hwnd, TCHAR * szTitleName)
{
     TCHAR szBuffer[64 + MAX_PATH] ;
     int   iReturn ;

     wsprintf (szBuffer, TEXT ("Save current changes in %s?"),
               szTitleName[0] ? szTitleName : UNTITLED) ;

     iReturn = MessageBox (hwnd, szBuffer, szAppName,
                           MB_YESNOCANCEL | MB_ICONQUESTION) ;

     if (iReturn == IDYES)
          if (!SendMessage (hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))
               iReturn = IDCANCEL ;

     return iReturn ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     static BOOL      bNeedSave = FALSE ;
     static HWND hwndEdit ;
     static TCHAR     szFileName[MAX_PATH], szTitleName[MAX_PATH] ;
     int         iSelect, iEnable ;

     switch (message)
     {
     case WM_CREATE:
          hwndEdit = CreateWindow (TEXT ("edit"), NULL,
                              WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                              WS_BORDER | ES_LEFT | ES_MULTILINE |
                              ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                              0, 0, 0, 0, hwnd, (HMENU) ID_EDIT,
                              ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;
          IDEFileInitialize (hwnd) ;
          DoCaption (hwnd, szTitleName) ;
          return 0 ;

     case WM_SETFOCUS:
          SetFocus (hwndEdit) ;
          return 0 ;

     case WM_SIZE:
          MoveWindow (hwndEdit, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE) ;
          return 0 ;

     case WM_INITMENUPOPUP:
          if (lParam == 1)
          {
               EnableMenuItem ((HMENU) wParam, IDM_EDIT_UNDO,
                    SendMessage (hwndEdit, EM_CANUNDO, 0, 0) ?
                                   MF_ENABLED : MF_GRAYED) ;

               EnableMenuItem ((HMENU) wParam, IDM_EDIT_PASTE,
                    IsClipboardFormatAvailable (CF_TEXT) ?
                                   MF_ENABLED : MF_GRAYED) ;

               iSelect = SendMessage (hwndEdit, EM_GETSEL, 0, 0) ;

               if (HIWORD (iSelect) == LOWORD (iSelect))
                    iEnable = MF_GRAYED ;
               else
                    iEnable = MF_ENABLED ;

               EnableMenuItem ((HMENU) wParam, IDM_EDIT_CUT,   iEnable) ;
               EnableMenuItem ((HMENU) wParam, IDM_EDIT_COPY,  iEnable) ;
               EnableMenuItem ((HMENU) wParam, IDM_EDIT_CLEAR, iEnable) ;
               return 0 ;
          }
          break ;

     case WM_COMMAND:
          if (lParam && LOWORD (wParam) == ID_EDIT)
          {
               switch (HIWORD (wParam))
               {
               case EN_UPDATE :
                    bNeedSave = TRUE ;
                    return 0 ;

               case EN_ERRSPACE :
               case EN_MAXTEXT :
                    MessageBox (hwnd, TEXT ("Edit control out of space."),
                                szAppName, MB_OK | MB_ICONSTOP) ;
                    return 0 ;
               }
               break ;
          }

          switch (LOWORD (wParam))
          {
          case IDM_FILE_NEW:
               if (bNeedSave && IDCANCEL == AskAboutSave (hwnd, szTitleName))
                    return 0 ;

               SetWindowText (hwndEdit, TEXT ("\0")) ;
               szFileName[0]  = '\0' ;
               szTitleName[0] = '\0' ;
               DoCaption (hwnd, szTitleName) ;
               bNeedSave = FALSE ;
               return 0 ;

          case IDM_FILE_OPEN:
               if (bNeedSave && IDCANCEL == AskAboutSave (hwnd, szTitleName))
                    return 0 ;

               if (IDEFileOpenDlg (hwnd, szFileName, szTitleName))
               {
                    if (!IDEFileRead (hwndEdit, szFileName))
                    {
                         OkMessage (hwnd, TEXT ("Could not read file %s!"),
                                    szTitleName) ;
                         szFileName[0]  = '\0' ;
                         szTitleName[0] = '\0' ;
                    }
               }

               DoCaption (hwnd, szTitleName) ;
               bNeedSave = FALSE ;
               return 0 ;

          case IDM_FILE_SAVE:
               if (szFileName[0])
               {
                    if (IDEFileWrite (hwndEdit, szFileName))
                    {
                         bNeedSave = FALSE ;
                         return 1 ;
                    }
                    else
                    {
                         OkMessage (hwnd, TEXT ("Could not write file %s"),
                                    szTitleName) ;
                         return 0 ;
                    }
               }

          case IDM_FILE_SAVE_AS:
               if (IDEFileSaveDlg (hwnd, szFileName, szTitleName))
               {
                    DoCaption (hwnd, szTitleName) ;

                    if (IDEFileWrite (hwndEdit, szFileName))
                    {
                         bNeedSave = FALSE ;
                         return 1 ;
                    }
                    else
                    {
                         OkMessage (hwnd, TEXT ("Could not write file %s"),
                                    szTitleName) ;
                         return 0 ;
                    }
               }
               return 0 ;

          case IDM_APP_EXIT:
               SendMessage (hwnd, WM_CLOSE, 0, 0) ;
               return 0 ;

          case IDM_EDIT_UNDO:
               SendMessage (hwndEdit, WM_UNDO, 0, 0) ;
               return 0 ;

          case IDM_EDIT_CUT:
               SendMessage (hwndEdit, WM_CUT, 0, 0) ;
               return 0 ;

          case IDM_EDIT_COPY:
               SendMessage (hwndEdit, WM_COPY, 0, 0) ;
               return 0 ;

          case IDM_EDIT_PASTE:
               SendMessage (hwndEdit, WM_PASTE, 0, 0) ;
               return 0 ;

          case IDM_EDIT_CLEAR:
               SendMessage (hwndEdit, WM_CLEAR, 0, 0) ;
               return 0 ;

          case IDM_EDIT_SELECT_ALL:
               SendMessage (hwndEdit, EM_SETSEL, 0, -1) ;
               return 0 ;

          case IDM_BUILD_BUILD_AND_RUN:
               if (!szFileName[0])
               {
                    if (IDEFileSaveDlg (hwnd, szFileName, szTitleName))
                    {
                         DoCaption (hwnd, szTitleName) ;
                    }
                    else
                         return 0 ;
               }
               if (IDEFileWrite (hwndEdit, szFileName))
               {
                    bNeedSave = FALSE ;
                    build(szFileName);
               }
               else
               {
                    OkMessage (hwnd, TEXT ("Could not save file %s"),
                               szTitleName) ;
               }

          case IDM_BUILD_RUN:
               interpret();
               return 0 ;

          case IDM_BUILD_BUILD:
               if (!szFileName[0])
               {
                    if (IDEFileSaveDlg (hwnd, szFileName, szTitleName))
                    {
                         DoCaption (hwnd, szTitleName) ;
                    }
                    else
                         return 0 ;
               }
               if (IDEFileWrite (hwndEdit, szFileName))
               {
                    bNeedSave = FALSE ;
                    build(szFileName);
               }
               else
               {
                    OkMessage (hwnd, TEXT ("Could not save file %s"),
                               szTitleName) ;
               }
               return 0 ;

          case IDM_BUILD_DEBUG:
               if (!szFileName[0])
               {
                    if (IDEFileSaveDlg (hwnd, szFileName, szTitleName))
                    {
                         DoCaption (hwnd, szTitleName) ;
                    }
                    else
                         return 0 ;
               }
               if (IDEFileWrite (hwndEdit, szFileName))
               {
                    bNeedSave = FALSE ;
                    build(szFileName);
                    debug();
               }
               else
               {
                    OkMessage (hwnd, TEXT ("Could not save file %s"),
                               szTitleName) ;
               }
               return 0 ;

          case IDM_HELP_HELP:
               OkMessage (hwnd, TEXT ("Help not yet implemented!"),
                                TEXT ("\0")) ;
               return 0 ;

          case IDM_APP_ABOUT:
               OkMessage (hwnd, TEXT ("CX”Ô—‘±‡“Î∆˜\nCompleted by ∏ﬂËÂ on 2019/9/26.\n\n—ß∫≈:10165102204"),
                                TEXT ("\0")) ;
               return 0 ;
          }
          break ;

     case WM_CLOSE:
          if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
               DestroyWindow (hwnd) ;

          return 0 ;

     case WM_QUERYENDSESSION:
          if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
               return 1 ;

          return 0 ;

     case WM_DESTROY :
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}
