//! \file GiGdipCanvas.cpp
//! \brief 实现使用GDI+实现的画布适配器类 GiGdipCanvas
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GiGdipCanvas.h"

#define ULONG_PTR DWORD
#include <objbase.h>
#include <GdiPlus.h>
using namespace Gdiplus;

#ifdef _MSC_VER
#pragma comment(lib,"GdiPlus.lib")
#endif

#define SAFEDEL(p) if (p) delete p; p = NULL

static long         _canvasCount = 0;   // GiCanvasGdip count
static ULONG_PTR    _gdipToken = 0;     // GDI+ token

//! GiGdipCanvas 的内部实现类
struct GiGdipCanvas::Impl
{
    Graphics*       gs;                 // 绘图输出对象
    Graphics*       bufGs;              // 缓冲绘图用的输出对象
    Bitmap*         bufBmp;             // 缓冲位图
    CachedBitmap*   cachedBmp[2];       // 后备缓冲位图
    Pen*            pen;                // 当前画笔
    SolidBrush*     brush;              // 当前画刷
    GraphicsPath*   path;               // 当前路径
    Color           bkcolor;            // 背景色
    GraphicsState   savegs[10];         // 保存的绘图状态
    RECT            clip;               // 初始剪切区域

    Impl() : gs(NULL), bufGs(NULL), bufBmp(NULL)
        , pen(NULL), brush(NULL), path(NULL)
    {
        cachedBmp[0] = cachedBmp[1] = NULL;

        if (1 == InterlockedIncrement(&_canvasCount)) { // 第一个对象
            SolidBrush* p = new SolidBrush(Color());    // 尝试创建GDI+对象
            if (!p) {                                   // 失败就初始化GDI+
                GdiplusStartupInput gdipStartupInput;
                GdiplusStartup(&_gdipToken, &gdipStartupInput, NULL);
            }
            delete p;
        }
    }

    ~Impl()
    {
        SAFEDEL(cachedBmp[0]);
        SAFEDEL(cachedBmp[1]);

        if (0 == InterlockedDecrement(&_canvasCount)    // 最后一个对象
            && _gdipToken != 0)                         // 已初始化GDI+
        {
            GdiplusShutdown(_gdipToken);                // 销毁GDI+
            _gdipToken = 0;
        }
    }

    Graphics* getGs() const { return bufGs ? bufGs : gs; }
    CachedBitmap*& cachedBitmap(bool secondBmp) { return cachedBmp[secondBmp ? 1 : 0]; }
};

GiGdipCanvas::GiGdipCanvas()
{
    _impl = new Impl();
}

GiGdipCanvas::~GiGdipCanvas()
{
    endPaint();
    delete _impl;
}

bool GiGdipCanvas::beginPaint(HWND hwnd, HDC hdc)
{
    if (_impl->getGs() || !hdc) {
        return false;
    }

    COLORREF bkcolor = ::GetBkColor(hdc);
    _impl->bkcolor.SetFromCOLORREF(bkcolor);

    SetRectEmpty(&_impl->clip);
    GetClipBox(hdc, &_impl->clip);
    if (IsRectEmpty(&_impl->clip)) {
        GetClientRect(hwnd, &_impl->clip);
    }

    _impl->gs = new Graphics(hdc);
    if (!_impl->gs) {
        return false;
    }

    if (!IsRectEmpty(&_impl->clip)) {
        _impl->bufBmp = new Bitmap(
            _impl->clip.right  - _impl->clip.left,
            _impl->clip.bottom - _impl->clip.top);
        _impl->bufGs = Graphics::FromImage(_impl->bufBmp);

        Matrix mat(1, 0, 0, 1, -(float)_impl->clip.left, -(float)_impl->clip.top);
        _impl->bufGs->SetTransform(&mat);
    }
    _beginPaint();

    return true;
}

bool GiGdipCanvas::beginPaintBuffered(int width, int height, COLORREF bkcolor)
{
    if (_impl->getGs() || width < 2 || height < 2) {
        return false;
    }

    _impl->bufBmp = new Bitmap(width, height);
    _impl->gs = Graphics::FromImage(_impl->bufBmp);

    if (!_impl->gs) {
        SAFEDEL(_impl->bufBmp);
        return false;
    }

    if (bkcolor == (COLORREF)-1) {
        _impl->bkcolor = Color::Transparent;
    }
    else {
        _impl->bkcolor.SetFromCOLORREF(bkcolor);
    }
    _impl->getGs()->Clear(_impl->bkcolor);

    _beginPaint();

    return true;
}

void GiGdipCanvas::_beginPaint()
{
    memset(_impl->savegs, 0, sizeof(_impl->savegs));
    setPen(~ _impl->bkcolor.ToCOLORREF(), 0, 0, 0, 0);
    _impl->getGs()->SetSmoothingMode(SmoothingModeAntiAlias);
}

void GiGdipCanvas::endPaint()
{
    if (_impl->bufGs && _impl->gs) {
        _impl->gs->SetInterpolationMode(InterpolationModeInvalid);
        _impl->gs->DrawImage(_impl->bufBmp, _impl->clip.left, _impl->clip.top);
    }
    SAFEDEL(_impl->bufGs);
    SAFEDEL(_impl->bufBmp);
    SAFEDEL(_impl->pen);
    SAFEDEL(_impl->brush);
    SAFEDEL(_impl->path);
    SAFEDEL(_impl->gs);
}

bool GiGdipCanvas::drawCachedBitmap(int x, int y, bool secondBmp)
{
    CachedBitmap* bmp = _impl->cachedBitmap(secondBmp);
    bool ret = false;

    if (bmp && _impl->getGs()) {
        ret = (_impl->getGs()->DrawCachedBitmap(bmp, x, y) == Ok);
    }

    return ret;
}

bool GiGdipCanvas::drawCachedBitmap2(GiGdipCanvas& cv, bool secondBmp, int x, int y)
{
    CachedBitmap* bmp = cv._impl->cachedBitmap(secondBmp);
    bool ret = false;

    if (bmp && _impl->getGs()) {
        ret = (_impl->getGs()->DrawCachedBitmap(bmp, x, y) == Ok);
    }

    return ret;
}

void GiGdipCanvas::saveCachedBitmap(bool secondBmp)
{
    if (_impl->getGs() && _impl->bufBmp) {
        SAFEDEL(_impl->cachedBitmap(secondBmp));
        _impl->cachedBitmap(secondBmp) = new CachedBitmap(
            _impl->bufBmp, _impl->getGs());
    }
}

bool GiGdipCanvas::hasCachedBitmap(bool secondBmp) const
{
    return _impl->cachedBitmap(secondBmp) != NULL;
}

void GiGdipCanvas::clearCachedBitmap(bool clearAll)
{
    SAFEDEL(_impl->cachedBitmap(false));
    if (clearAll) {
        SAFEDEL(_impl->cachedBitmap(true));
    }
}

bool GiGdipCanvas::isBufferedDrawing() const
{
    return _impl->bufBmp != NULL;
}

void GiGdipCanvas::setPen(int argb, float width, int style, float phase, float)
{
    if (!_impl->pen) {
        _impl->pen = new Pen(Color(argb ? argb : Color::Black));
    }
    else if (argb != 0) {
        _impl->pen->SetColor(Color(argb));
    }
    if (width > 0.1f) {
        _impl->pen->SetWidth(width);
    }
    if (style >= 0 && style <= DashStyleDashDotDot) {
        _impl->pen->SetDashStyle((DashStyle)style);
        _impl->pen->SetDashOffset(phase);
    }
}

void GiGdipCanvas::setBrush(int argb, int style)
{
    if (0 == style) {
        if (!_impl->brush) {
            _impl->brush = new SolidBrush(Color(argb));
        }
        else {
            _impl->brush->SetColor(Color(argb));
        }
    }
}

void GiGdipCanvas::saveClip()
{
    if (_impl->getGs()) {
        int i = 0;
        for (; _impl->savegs[i]; i++) ;

        if (i + 1 < sizeof(_impl->savegs)/sizeof(_impl->savegs[0])) {
            _impl->savegs[i] = _impl->getGs()->Save();
        }
    }
}

void GiGdipCanvas::restoreClip()
{
    if (_impl->getGs()) {
        int i = sizeof(_impl->savegs)/sizeof(_impl->savegs[0]);
        while (--i >= 0) {
            if (_impl->savegs[i]) {
                _impl->getGs()->Restore(_impl->savegs[i]);
                _impl->savegs[i] = 0;
                break;
            }
        }
    }
}

void GiGdipCanvas::clearWindow()
{
    if (_impl->getGs()) {
        _impl->getGs()->Clear(_impl->bkcolor);
    }
}

void GiGdipCanvas::clearRect(float x, float y, float w, float h)
{
    if (_impl->getGs()) {
        GraphicsState state = _impl->getGs()->Save();
        _impl->getGs()->IntersectClip(RectF(x, y, w, h));
        _impl->getGs()->Clear(_impl->bkcolor); // Color::Transparent?
        _impl->getGs()->Restore(state);
    }
}

void GiGdipCanvas::drawRect(float x, float y, float w, float h, bool stroke, bool fill)
{
    if (_impl->getGs()) {
        if (_impl->brush && fill) {
            _impl->getGs()->FillRectangle(_impl->brush, x, y, w, h);
        }
        if (_impl->pen && stroke) {
            _impl->getGs()->DrawRectangle(_impl->pen, x, y, w, h);
        }
    }
}

bool GiGdipCanvas::clipRect(float x, float y, float w, float h)
{
    if (_impl->getGs()) {
        _impl->getGs()->IntersectClip(RectF(x, y, w, h));
    }
    return true;
}

void GiGdipCanvas::drawLine(float x1, float y1, float x2, float y2)
{
    if (_impl->getGs() && _impl->pen) {
        _impl->getGs()->DrawLine(_impl->pen, x1, y1, x2, y2);
    }
}

void GiGdipCanvas::drawEllipse(float x, float y, float w, float h, bool stroke, bool fill)
{
    if (_impl->getGs()) {
        if (_impl->brush && fill) {
            _impl->getGs()->FillEllipse(_impl->brush, x, y, w, h);
        }
        if (_impl->pen && stroke) {
            _impl->getGs()->DrawEllipse(_impl->pen, x, y, w, h);
        }
    }
}

void GiGdipCanvas::beginPath()
{
    if (_impl->path) {
        _impl->path->Reset();
    }
    else {
        _impl->path = new GraphicsPath;
    }
}

void GiGdipCanvas::moveTo(float x, float y)
{
    if (_impl->path) {
        if (_impl->path->GetPointCount() > 0) {
            _impl->path->StartFigure();
        }
        _impl->path->AddLine(x, y, x, y);
    }
}

void GiGdipCanvas::lineTo(float x, float y)
{
    if (_impl->path) {
        PointF pt;
        _impl->path->GetLastPoint(&pt);
        _impl->path->AddLine(pt.X, pt.Y, x, y);
    }
}

void GiGdipCanvas::bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    if (_impl->path) {
        PointF pt;
        _impl->path->GetLastPoint(&pt);
        _impl->path->AddBezier(pt.X, pt.Y, c1x, c1y, c2x, c2y, x, y);
    }
}

void GiGdipCanvas::quadTo(float cpx, float cpy, float x, float y)
{
    if (_impl->path) {
        PointF pt;
        _impl->path->GetLastPoint(&pt);
        _impl->path->AddBezier(pt.X, pt.Y, 
            (pt.X + cpx * 2) / 3, (pt.Y + cpy * 2) / 3,
            (x + cpx * 2) / 3,    (y + cpy * 2) / 3,   x, y);
    }
}

void GiGdipCanvas::closePath()
{
    if (_impl->path) {
        _impl->path->CloseFigure();
    }
}

void GiGdipCanvas::drawPath(bool stroke, bool fill)
{
    if (_impl->getGs() && _impl->path) {
        if (_impl->brush && fill) {
            _impl->getGs()->FillPath(_impl->brush, _impl->path);
        }
        if (_impl->pen && stroke) {
            _impl->getGs()->DrawPath(_impl->pen, _impl->path);
        }
        delete _impl->path;
        _impl->path = NULL;
    }
}

bool GiGdipCanvas::clipPath()
{
    if (_impl->getGs() && _impl->path) {
        _impl->getGs()->SetClip(_impl->path, CombineModeIntersect);
    }
    return true;
}

bool GiGdipCanvas::drawHandle(float x, float y, int)
{
    Bitmap* bmp = NULL;//new Bitmap(L"A:\\vgdot1.png");
    bool ret = false;

    if (bmp) {
        float w = (float)bmp->GetWidth() / bmp->GetHorizontalResolution();
        float h = (float)bmp->GetHeight() / bmp->GetVerticalResolution();
        w *= 96;
        h *= 96;
        ret = _impl->getGs()->DrawImage(bmp, x - w / 2, y - h / 2) == Ok;
        delete bmp;
    }

    return ret;
}

bool GiGdipCanvas::drawBitmap(const char*, float, float, 
                              float, float, float)
{
    // TODO: drawBitmap
    return false;
}

float GiGdipCanvas::drawTextAt(const char* text, float x, float y, float, int align)
{
    float w = 0;

    if (text && _impl->getGs()) {
#if 0
        Color color;
        _impl->pen->GetColor(&color);
        SolidBrush brush(color);

        Font font(&FontFamily(L"Arial"), 72);
        _impl->getGs()->DrawString(L"Look at this text汉字!", -1, 
            &font, PointF(0, 0), &brush);
#else
        HDC hdc = _impl->getGs()->GetHDC();

        SetBkMode(hdc, TRANSPARENT);
        align = (1 == align) ? TA_CENTER : (2 == align ? TA_RIGHT : TA_LEFT);
        SetTextAlign(hdc, align | TA_TOP);

        if (_impl->pen) {
            Color color;
            _impl->pen->GetColor(&color);
            SetTextColor(hdc, color.ToCOLORREF());
        }

        TextOutA(hdc, (int)(x + 0.5f), (int)(y + 0.5f), text, strlen(text));

        SIZE size;
        if (GetTextExtentPointA(hdc, text, strlen(text), &size)) {
            w = (float)size.cx;
        }

        _impl->getGs()->ReleaseHDC(hdc);
#endif
    }

    return w;
}

static int getEncoderClsid(const wchar_t* format, CLSID* clsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* imageCodec = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    imageCodec = (ImageCodecInfo*)(malloc(size));
    if (imageCodec == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, imageCodec);

    for (UINT j = 0; j < num; ++j)
    {
        if ( wcscmp(imageCodec[j].MimeType, format) == 0 )
        {
            *clsid = imageCodec[j].Clsid;
            free(imageCodec);
            return j;  // Success
        }
    }

    free(imageCodec);
    return -1;  // Failure
}

static struct {
    const wchar_t* ext;
    const wchar_t* encoder;
} s_extEncoders[] = {
    { L".bmp",  L"image/bmp" },
    { L".dib",  L"image/bmp" },
    { L".rle",  L"image/bmp" },

    { L".jpg",  L"image/jpeg" },
    { L".jpeg", L"image/jpeg" },
    { L".jpe",  L"image/jpeg" },
    { L".jfif", L"image/jpeg" },

    { L".tif",  L"image/tiff" },
    { L".tiff", L"image/tiff" },

    { L".gif",  L"image/gif" },
    { L".png",  L"image/png" },
    { L".wmf",  L"image/x-wmf" },
    { L".emf",  L"image/x-emf" },
    { L".icon", L"image/x-icon" },
};
static const int s_extCount = sizeof(s_extEncoders)/sizeof(s_extEncoders[0]);

static struct EncoderItem {
    const wchar_t*  encoderType;
    CLSID           clsidEncoder;
    EncoderItem() { memset(this, 0, sizeof(EncoderItem)); }
} _encoders[s_extCount];

bool GiGdipCanvas::getEncoder(const wchar_t* filename, wchar_t format[20], CLSID& clsidEncoder)
{
    const wchar_t* ext = filename ? wcsrchr(filename, '.') : NULL;
    int i = ext ? s_extCount : 0;

    while (--i >= 0) {
        if (_wcsicmp(s_extEncoders[i].ext, ext) == 0) {
            if (!_encoders[i].encoderType) {
                getEncoderClsid(s_extEncoders[i].encoder, &_encoders[i].clsidEncoder);
                _encoders[i].encoderType = s_extEncoders[i].encoder;
            }
            lstrcpynW(format, s_extEncoders[i].encoder, 20);
            clsidEncoder = _encoders[i].clsidEncoder;
            break;
        }
    }

    return i >= 0;
}

GiGdipCanvas* GiGdipCanvas::thumbnailImage(int maxWidth, int maxHeight)
{
    GiGdipCanvas* image = NULL;
    int width = _impl->bufBmp ? _impl->bufBmp->GetWidth() : 0;
    int height = _impl->bufBmp ? _impl->bufBmp->GetHeight() : 0;

    if (width > 0 && height > 0)
    {
        if (maxWidth < 1 || maxWidth > width)
            maxWidth = width;
        if (maxHeight < 1 || maxHeight > height)
            maxHeight = height;

        if (maxWidth < width || maxHeight < height) {
            if (width / height > maxWidth / maxHeight) {
                width = maxHeight * width / height;
                height = maxHeight;
            }
            else {
                height = maxWidth * height / width;
                width = maxWidth;
            }
        }

        Image* tbbmp = _impl->bufBmp->GetThumbnailImage(width, height, NULL, NULL);
        if (tbbmp != NULL) {
            image = new GiGdipCanvas();
            image->_impl->bufBmp = (Bitmap*)tbbmp;
        }
    }

    return image;
}

bool GiGdipCanvas::save(const wchar_t* filename, long jpegQuality)
{
    bool ret = false;
    wchar_t format[20];
    CLSID clsidEncoder;
    
    if (_impl->bufBmp && getEncoder(filename, format, clsidEncoder))
    {
        if (wcscmp(L"image/jpeg", format) == 0) {
            EncoderParameters encoderParameters;
            
            encoderParameters.Count = 1;
            encoderParameters.Parameter[0].Guid = EncoderQuality;
            encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
            encoderParameters.Parameter[0].NumberOfValues = 1;
            encoderParameters.Parameter[0].Value = &jpegQuality;
            
            ret = (Ok == _impl->bufBmp->Save(filename, &clsidEncoder, &encoderParameters));
        }
        else {
            ret = (Ok == _impl->bufBmp->Save(filename, &clsidEncoder, NULL));
        }
    }
    
    return ret;
}
