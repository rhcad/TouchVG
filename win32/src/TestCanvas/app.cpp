#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static ATOM MyRegisterClass(HINSTANCE);
static BOOL InitInstance(HINSTANCE, int);
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    MSG msg;

    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize = sizeof(wcex);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszClassName  = L"Test";

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"Test", L"TestCanvas", 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
        NULL, NULL, hInstance, NULL);

    if (hwnd) {
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
    }

    return !!hwnd;
}

#include <GiGdipCanvas.h>
#include <testcanvas.h>

static void ondraw(HWND hwnd);
static void switchTest(HWND hwnd);
static void regen(HWND hwnd);
static void dyndraw(GiCanvas* canvas);

GiGdipCanvas    _canvas;
int             _tests = TestCanvas::kDynCurves;
POINTS          _pt = { 0, 0 };

// 主窗口的消息处理函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_ERASEBKGND:         // 不清除背景，避免闪烁
        break;
    case WM_PAINT:              // 重绘主窗口
        ondraw(hwnd);
        break;
    case WM_SIZE:               // 改变窗口大小
        regen(hwnd);
        break;
    case WM_DESTROY:            // 退出
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDBLCLK:      // 双击切换测试图形
        switchTest(hwnd);
        break;
    case WM_MOUSEMOVE:          // 鼠标移动
        _pt = MAKEPOINTS(lparam);
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    default:
        return DefWindowProc(hwnd, message, wparam, lparam);
    }

    return 0;
}

void ondraw(HWND hwnd)
{
    DWORD tick = GetTickCount();

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);            // 获取重绘消息的句柄数据

    SetBkColor(hdc, GetSysColor(COLOR_WINDOW)); // 设置背景色以便传到画布

    if (_canvas.beginPaint(hwnd, hdc)) {        // 开始在画布上绘制
        if (!_canvas.drawCachedBitmap()) {      // 显示上次保存的内容
            _canvas.clearWindow();              // 使用背景色清除显示
            TestCanvas::test(&_canvas, _tests); // 绘制测试图形
            if ((_tests & TestCanvas::kDynCurves) == 0)
                _canvas.saveCachedBitmap();     // 缓存显示的内容
        }
        dyndraw(&_canvas);                      // 绘制动态图形
        _canvas.endPaint();                     // 结束绘制
    }
    EndPaint(hwnd, &ps);
    tick = GetTickCount() - tick;
}

void dyndraw(GiCanvas* canvas)
{
    static float phase = 0;
    phase += 1;
    canvas->setPen(0, 0, 1, phase, 0);
    canvas->setBrush(0x88005500, 0);
    canvas->drawEllipse((float)_pt.x - 50.f, (float)_pt.y - 50.f, 100.f, 100.f, true, false);
}

void regen(HWND hwnd)
{
    _canvas.clearCachedBitmap();            // 需要重新构建显示
    InvalidateRect(hwnd, NULL, FALSE);      // 触发重绘消息
}

void switchTest(HWND hwnd)
{
    int a[] = { TestCanvas::kRect, TestCanvas::kLine, TestCanvas::kTextAt, TestCanvas::kEllipse, TestCanvas::kQuadBezier, TestCanvas::kCubicBezier, TestCanvas::kClearRect, 
        TestCanvas::kClearRect | TestCanvas::kPolygon | TestCanvas::kLine, TestCanvas::kClipPath, TestCanvas::kHandle, TestCanvas::kDynCurves };
    int n = sizeof(a)/sizeof(a[0]);
    int i = n;

    while (--i > 0 && a[i] != _tests) ;
    _tests = a[(i + 1) % n];
    regen(hwnd);

    GiGdipCanvas canvas;

    if (canvas.beginPaintBuffered(1024, 768)) {
        TestCanvas::test(&canvas, _tests);

        WCHAR filename[MAX_PATH];
        GetModuleFileNameW(NULL, filename, MAX_PATH);
        wcscpy_s(wcsrchr(filename, L'.'), 5, L".png");
        canvas.save(filename);

        canvas.endPaint();
    }
}
