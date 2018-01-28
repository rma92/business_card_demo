#include <windows.h>
#include "res.h"

RECT union_displays_rect;
HENHMETAFILE hemf;

HWND hWnd1;

HDC images[1024];
HBITMAP images_bmp[1024];
RECT image_size;
HBRUSH transparent_color;

RECT textTempRect;
LPCTSTR szName[128];
LPCTSTR szSubname[128];
LPCTSTR szTel[128];
LPCTSTR szURL[128];
LPCTSTR szEmail[128];

//Holds the original size of the bitmap for double buffering,
//in case the window size is changed.
RECT orig_buffer_size;
HDC hmemdc;
HBITMAP hmembmp;
int timer_time = 1000/60; //frame in ms
int rm_width_dx = -5;
int rm_width = 300;
int rm_height = 600; //updated in wm_create
int rm_max_width = 600; //updated in wm_create

int render_scale = 0.5; //multiply everything drawn by this;
void CALLBACK MonitorEnum( HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData )
{
  UnionRect( &union_displays_rect, &union_displays_rect, lprcMonitor );
}

void update_display_info()
{
  CHAR msg[32];
  SetRectEmpty( &union_displays_rect );
  EnumDisplayMonitors( NULL, NULL, MonitorEnum, NULL );
}

/*
  Load metafiles from resources, buffer them into new images for blt later.
*/
void load_pictures()
{
  HRSRC hrsrc;
  HANDLE hres;
  DWORD dw;
  DWORD i;
  DWORD img_count;
  transparent_color = CreateSolidBrush( RGB( 0, 255, 0 ) );
  image_size.left = 0;
  image_size.top = 0;
  image_size.right = union_displays_rect.right - union_displays_rect.left;
  image_size.bottom = union_displays_rect.bottom - union_displays_rect.top;
  for( i = 0, img_count = MIN_IMG; img_count <= MAX_IMG; ++i, ++img_count )
  {
    images[i] = CreateCompatibleDC( GetDC( hWnd1 ) );
    images_bmp[i] = CreateCompatibleBitmap( GetDC( hWnd1 ), image_size.right, image_size.bottom );
    SelectObject( images[i], images_bmp[i] );
    FillRect( images[i], &image_size, transparent_color );
    //HENHMETAFILE hemf;
    hrsrc = FindResource( GetModuleHandle( NULL ), MAKEINTRESOURCE( img_count ), "ENHANCEDMETAFILE" );
    hres = LoadResource( GetModuleHandle( NULL ), hrsrc );
    dw = SizeofResource ( GetModuleHandle( NULL ), hrsrc );
    hemf = SetEnhMetaFileBits( dw, hres );

    PlayEnhMetaFile( images[i], hemf, &image_size );
  }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HMENU hM, hSM;
  POINT cursor;

  HRSRC hrsrc;
  HANDLE hres;
  DWORD dw;
  switch(message)
  {
    case WM_CREATE:
      //CreateWindowEx(0, "BUTTON", "A Button", WS_CHILD | WS_VISIBLE, 10, 40, 72, 20, hWnd, (HMENU)33, NULL, NULL);
      update_display_info();
      SetWindowPos( hWnd, 0, union_displays_rect.left, union_displays_rect.top, union_displays_rect.right, union_displays_rect.bottom, 0);
      load_pictures();
      rm_height = ( union_displays_rect.bottom - union_displays_rect.top ) * 0.4;
      rm_max_width = rm_height * 1.6;
      rm_width = rm_max_width * 0.5;
      rm_width_dx = rm_max_width * .02;
      SetTimer( hWnd, IDT_TIMER1, timer_time, NULL );
      
      //Load Strings
      LoadString( GetModuleHandle( NULL ) , IDS_NAME, (char*) szName, sizeof( szName ) );
      LoadString( GetModuleHandle( NULL ) , IDS_SUBTITLE, (char*) szSubname, sizeof( szSubname ) );
      LoadString( GetModuleHandle( NULL ) , IDS_TEL, (char*) szTel, sizeof( szTel ) );
      LoadString( GetModuleHandle( NULL ) , IDS_URL, (char*) szURL, sizeof( szURL ) );
      LoadString( GetModuleHandle( NULL ) , IDS_EMAIL, (char*) szEmail, sizeof( szEmail ) );
      
      break;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDM_FILE_DISPLAYINFO://display info
          update_display_info();
          break;
        case IDM_FILE_EXIT://File > Exit
          PostQuitMessage(0);
          break;
      }
    break;
    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT cr;
      GetClientRect( hWnd, &cr );
      hdc = BeginPaint( hWnd, &ps );
      if( hmemdc == 0 )
      {
        hmemdc = CreateCompatibleDC( hdc );
        orig_buffer_size = cr;
        hmembmp = CreateCompatibleBitmap( GetDC( hWnd1 ), orig_buffer_size.right, orig_buffer_size.bottom );
        SelectObject( hmemdc, hmembmp );
        FillRect( hmemdc, &cr, RGB(255,255,255) );
      }

      FillRect( hmemdc, &orig_buffer_size, RGB(255,255,255));
      TransparentBlt( hmemdc, (rm_max_width - rm_width) * 0.5, 0, rm_width, rm_height, images[0], 0, 0, image_size.right, image_size.bottom, RGB( 0, 255, 0) );
      textTempRect = orig_buffer_size;
      textTempRect.left = rm_max_width;
      DrawText( hmemdc, szName, -1, &textTempRect, DT_LEFT );
      
      //double buffer
      StretchBlt ( hdc, 0, 0, cr.right, cr.bottom, hmemdc, 0, 0, orig_buffer_size.right, orig_buffer_size.bottom, SRCCOPY );
      EndPaint( hWnd, &ps );
    }
    break;
    case WM_TIMER:
    {
      switch( wParam )
      {
        case IDT_TIMER1:
        {
          rm_width += rm_width_dx;
          printf(" %d ", rm_width );
          if( rm_width < 1 || rm_width > rm_max_width )
          {
            rm_width_dx *= -1;
          }
          InvalidateRect( hWnd, NULL, FALSE );
        }
        break;
      }
    }
    break;
    case WM_SIZE:
      return DefWindowProc( hWnd, message, wParam, lParam );
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg1;
  WNDCLASS wc1;
  //TCHAR szAppName [32];

  //LoadString( hInst, IDS_APPNAME, (char*) szAppName, sizeof( szAppName ) );

  ZeroMemory(&wc1, sizeof wc1);
  wc1.hInstance = hInst;
  wc1.lpszClassName = AppName;
  wc1.lpfnWndProc = (WNDPROC)WndProc;
  wc1.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
  wc1.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
  wc1.hIcon = LoadIcon(NULL, IDI_INFORMATION);
  wc1.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc1.lpszMenuName = "MAINMENU";
  if(RegisterClass(&wc1) == FALSE) return 0;
  hWnd1 = CreateWindow(AppName, AppName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 10, 10, 360, 240, 0, 0, hInst, 0);
  if(hWnd1 == NULL) return 0;
  while(GetMessage(&msg1,NULL,0,0) > 0){
    TranslateMessage(&msg1);
    DispatchMessage(&msg1);
  }
  return msg1.wParam;
}

