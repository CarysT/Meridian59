//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
/// \file
/// Implements the TTooltip class
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include <owl/window.h>
#include <owl/gdiobjec.h>
#include <owl/tooltip.h>
#include <owl/commctrl.h>
#include <owl/uimetric.h>

namespace
{

  //
  // This function solves a problem for Unicode builds without a manifest for Common Controls
  // version 6. In this case, the classic non-themed version of Common Controls is used (5.x),
  // and this version expects the old size of the TOOLINFO structure. Consequently, naively using
  // the size of TOOLINFO to initialize TOOLINFO::cbSize will cause tool tips to not show.
  //
  int GetToolInfoStructSize_()
  {
    PRECONDITION(owl::TCommCtrl::IsAvailable());
    static size_t s = owl::TCommCtrl::Dll()->GetCtrlVersion() >= 0x60000 ?
      sizeof(TOOLINFO) : TTTOOLINFO_V1_SIZE;
    return static_cast<int>(s);
  }

} // namespace

namespace owl {

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlControl);        // General Controls diagnostic group

//
// Size of container of tools
//
const int InitSize = 5;               // Initial size of tool container
const int InitDelta= 5;               // Inc/Dec of tools container
const int DefDelay = 500;             // Default delay is 1/2 second
const int HorzPad  = 4;               // Side pads around text of tooltip
const int VertPad  = 2;               // Top/Down pads around text of tooltip
const int DefTipTextCacheSize = 128;  // Default cache size of tip text

//
// Timer id used by Tooltip window
//
const unsigned TOOLTIP_TIMERID = 0xABBA;

//
/// This is the default constructor of TToolInfo. It's used mainly when retrieving
/// information about the current tool of the tooltip control or for initializing a
/// brand new tool to be registered with the control. For example:
/// \code
///   TToolInfo ti;
///   tooltip.GetCurrentTool(ti);
/// \endcode
//
TToolInfo::TToolInfo(bool allocCache)
{
  memset(this, 0, sizeof(TOOLINFO));
  cbSize = GetToolInfoStructSize_();
  if (allocCache) {
    CacheText = new tchar[DefTipTextCacheSize];
    lpszText = (tchar*)CacheText;
  }
}

//
/// Constructor for a tool implemented as a rectangular area within a window's
/// client area. The window argument receives the TTN_NEEDTEXT notification in case
/// the txt argument defaults to LPSTR_TEXTCALLBACK.
//
TToolInfo::TToolInfo(HWND window, const TRect& rc, uint toolId,
                     LPCTSTR txt /*=LPSTR_TEXTCALLBACK*/)
{
  memset(this, 0, sizeof(TOOLINFO));
  cbSize = GetToolInfoStructSize_();
  SetToolInfo(window, toolId);
  SetRect(rc);

  // NOTE: When we're using the Common Control implementation we don't want
  //       to cache the text since we won't keep a copy of the TToolInfo
  //       structure around.
  //
  SetText(txt, false);
}

//
/// String overload
//
TToolInfo::TToolInfo(HWND window, const TRect& rc, uint toolId, const tstring& txt)
{
  memset(this, 0, sizeof(TOOLINFO));
  cbSize = GetToolInfoStructSize_();
  SetToolInfo(window, toolId);
  SetRect(rc);
  SetText(txt);
}

//
/// Constructor for a tool implemented as a rectangular area within a window's
/// client area. The strRes and hInst arguments specify a string resource of the
/// message to be used by the tooltip window.
//
TToolInfo::TToolInfo(HWND window, const TRect& rc, uint toolId,
                     int strRes, HINSTANCE hInst)
{
  memset(this, 0, sizeof(TOOLINFO));
  cbSize = GetToolInfoStructSize_();
  SetToolInfo(window, toolId, rc);
  SetText(strRes, hInst);
}

//
/// Constructor for a tool implemented as windows (child/controls). The parent
/// argument receives the TTN_NEEDTEXT notification in case the txt argument
/// defaults to LPSTR_TEXTCALLBACK.
//
TToolInfo::TToolInfo(HWND parent, HWND toolWnd,
                     LPCTSTR txt /*=LPSTR_TEXTCALLBACK*/)
{
  memset(this, 0, sizeof(TOOLINFO));
  cbSize = GetToolInfoStructSize_();
  SetToolInfo(toolWnd, parent);
  SetText(txt, false);
}

//
/// String overload
//
TToolInfo::TToolInfo(HWND parent, HWND toolWnd, const tstring& txt)
{
  memset(this, 0, sizeof(TOOLINFO));
  cbSize = GetToolInfoStructSize_();
  SetToolInfo(toolWnd, parent);
  SetText(txt);
}

//
/// Constructor for a tool implemented as a window (child/control). The strRes and
/// hInst arguments specify a string resource to be used by the tooltip window.
//
TToolInfo::TToolInfo(HWND parent, HWND toolWnd,
                     int strRes, HINSTANCE hInst)
{
  memset(this, 0, sizeof(TOOLINFO));
  cbSize = GetToolInfoStructSize_();
  SetToolInfo(toolWnd, parent);
  SetText(strRes, hInst);
}

//
//
//
TToolInfo::TToolInfo(const TToolInfo& other)
{
  // Use assignment operator
  //
  *this = other;
}

//
//
//
TToolInfo&
TToolInfo::operator =(const TToolInfo& other)
{
  if (&other != this) {
    *((TOOLINFO*)this) = *((TOOLINFO*)&other);
    if (other.lpszText == other.GetCacheText())
      SetText(other.GetCacheText());
    /***
    else
     'other.lpszText' is assumed to be NULL, LPSTR_CALLBACK or pointing
      to a buffer with a long lifetime. In all three cases a shallow copy of
      the pointer is safe.
    ***/
  }
  return *this;
}

//
/// Sets the `hwnd` and `uId` members of the TOOLINFO base.
/// TOOLINFO::hwnd is set to `window`, and TOOLINFO::uId is set to `toolId`.
/// The flag TTF_IDISHWND is cleared in TOOLINFO::uFlags.
//
void
TToolInfo::SetToolInfo(HWND window, uint toolId)
{
  PRECONDITION(::IsWindow(window));
  hwnd = window;
  uFlags &= ~TTF_IDISHWND;
  uId = toolId;
}

//
/// Sets the `hwnd`, `uId` and `rect` members of the TOOLINFO base.
/// TOOLINFO::hwnd is set to `window`, TOOLINFO::uId is set to `toolId`, and TOOLINFO::rect is set to `rc`.
/// The flag TTF_IDISHWND is cleared in TOOLINFO::uFlags.
//
void
TToolInfo::SetToolInfo(HWND window, uint toolId, const TRect& rc)
{
  SetToolInfo(window, toolId);
  SetRect(rc);
}

//
/// Sets the `hwnd` and `uId` members of the TOOLINFO base.
/// TOOLINFO::hwnd is set to `parent`, and TOOLINFO::uId is set to `toolHwnd`.
/// The flag TTF_IDISHWND is set in TOOLINFO::uFlags.
//
void
TToolInfo::SetToolInfo(HWND toolHwnd, HWND parent)
{
  PRECONDITION(::IsWindow(toolHwnd));
  PRECONDITION(::IsWindow(parent));
  hwnd = parent;
  uFlags |= TTF_IDISHWND;
  uId = reinterpret_cast<UINT_PTR>(toolHwnd);
}

//
/// Sets the text of this tool by providing a buffer that contains
/// the string. The boolean 'copy' flag specifies whether the method
/// should make a local copy of the string.
//
void
TToolInfo::SetText(LPCTSTR text, bool copy)
{
  if (text == LPSTR_TEXTCALLBACK || !text || !copy)
  {
    lpszText = (tchar*)text;
    CacheText= 0;
  }
  else
  {
    CacheText = strnewdup(text);
    lpszText = (tchar*)CacheText;
  }
}

//
/// Returns the actual HWND linked to a tool. For tools implemented as a rectangle
/// within a client area, the window's handle is returned. For tools associated with
/// a control, the handle of the control is returned.
//
HWND
TToolInfo::GetToolWindow() const
{
  return (uFlags & TTF_IDISHWND) ? HWND(uId) : hwnd;
}

//
/// Retrieves the actual RECT linked to a tool. For tools implemented as a rectangle
/// within a client area, that rectangle is retrieved. For tools associated with a
/// control, the latter's client area is retrieved.
//
void
TToolInfo::GetToolRect(TRect& rc) const
{
  if (uFlags & TTF_IDISHWND) {
    CHECK(::IsWindow(HWND(uId)));
    ::GetClientRect(HWND(uId), &rc);
  }
  else {
    rc = rect;
  }
}

//
/// This method determines whether a particular location of a window is relevant to
/// this tool. For tools implemented as a rectangle within a window's client area,
/// simply check that the pt argument is within that rectangle. For tools
/// representing a child window, check that pt is within the client area of the
/// child window.
/// Returns true if succesful, or false if it fails.
/// \note  The pt argument must be relative to the window's client area.
//
bool
TToolInfo::IsPointInTool(HWND win, const TPoint& pt) const
{
  HWND window = GetToolWindow();
  if (window == win) {
    TRect rc;
    GetToolRect(rc);
    if (rc.Contains(pt))
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------
// TTooltipText
//

//
/// Sets text of tooltip to specified buffer.
/// NOTE: The buffer pointed to by the specified parameter must be
/// valid for as long as the TTooltipText points to it.
/// For temporary buffers, use the 'CopyText' method instead.
//
void
TTooltipText::SetText(LPCTSTR buff)
{
  lpszText = (LPTSTR)buff;
}

//
/// Sets the text of the tooltip. The text is copied into the
/// buffer owned by the 'TTooltipText'.
/// \note The internal buffer can only handle 80 characters including the null-terminator.
/// Longer strings will be truncated. Use SetText instead to avoid this.
//
void
TTooltipText::CopyText(LPCTSTR text)
{
  PRECONDITION(text != 0);
  const size_t maxLen = COUNTOF(szText) - 1;
  WARNX(OwlControl, _tcslen(text) > maxLen, 0, _T("Tip text is too large (truncated)."));
  szText[0] = _T('\0');
  _tcsncat(szText, text, maxLen);
}

//
/// Sets the text of the tooltip. The 'resId' identifies a string resource
/// found in the module pointed to by the 'hInstance' parameter.
//
void
TTooltipText::SetText(int resId, HINSTANCE hInstance)
{
  lpszText = CONST_CAST(LPTSTR, MAKEINTRESOURCE(resId));
  hinst = hInstance;
}

//----------------------------------------------------------------------------
// TTooltipEnabler
//

//
/// Constructs enabler object to be sent to a window so that the latter can provide
/// the text of the specified tool.
//
TTooltipEnabler::TTooltipEnabler(TTooltipText& tt, HWND hReceiver)
:
  TCommandEnabler(
    (tt.uFlags & TTF_IDISHWND) ? ::GetDlgCtrlID(reinterpret_cast<HWND>(tt.hdr.idFrom)) : static_cast<uint>(tt.hdr.idFrom),
    hReceiver),
  TipText(tt)
{
  Flags |= NonSender;
}

//
/// Sets the text of the tool specified by the TTooltipEnabler object.
/// \note The text is copied to the internal buffer in the TTooltipText structure.
/// This buffer can only handle 80 characters including the null-terminator.
/// Longer strings will be truncated.
//
void
TTooltipEnabler::SetText(LPCTSTR text)
{
  TCommandEnabler::Enable(true);// only to set Handled
  TipText.CopyText(text);
}

//
/// SetCheck does nothing but serve as a place-holder function.
//
void
TTooltipEnabler::SetCheck(int /*check*/)
{
}

//----------------------------------------------------------------------------
// TTooltip
//

//
/// Constructor for Ttooltip. Initializes its data fields using parameters passed
/// and default values. By default, a Tooltip associated with the TTooltip will be
/// active regardless of whether it's owner is active or inactive.
//
TTooltip::TTooltip(TWindow* parent, bool alwaysTip, TModule* module)
:
  TControl(parent, 0, _T(""), 0, 0, 0, 0, module)
{
  Attr.Style = (WS_POPUP | WS_DISABLED);

  if (alwaysTip)
    Attr.Style |= TTS_ALWAYSTIP;


  Attr.ExStyle |= WS_EX_WINDOWEDGE;

  ///BGM removing this because it makes tooltips (and other popups, such
  /// as dialogs) show up behind the frame when the window is first created.
  /// see bug 43291.
  ///
  /// Most tooltips don't need to be topmost. However, when the window
  /// they are servicing is reparented [for example, docking toolbars which
  /// are docked/undocked], the tooltip's owner may no longer be the
  /// window's owner. In that case, the tip may show up behind the owner
  /// of the reparented window [unless, the tooltip has EX_TOPMOST].
  ///

  Attr.ExStyle |= WS_EX_TOPMOST; //??????????????

  // For 32-bit platforms we rely on the Common Control Library
  //
  CHECK(TCommCtrl::IsAvailable());
}

TTooltip::TTooltip(TWindow* parent, TWindowFlag f, TModule* m)
  : TControl(parent, 0, _T(""), 0, 0, 0, 0, m)
{
  PRECONDITION(TCommCtrl::IsAvailable());
  PRECONDITION(f == wfAutoCreate);

  // See comment about WS_EX_TOPMOST in the other constructor.
  //
  SetStyle(WS_POPUP | WS_DISABLED | TTS_ALWAYSTIP);
  ModifyExStyle(0, WS_EX_WINDOWEDGE | WS_EX_TOPMOST);
  SetFlag(f);
}

//
/// Constructor to alias a non-OWL TOOLTIP control. Specially useful when used with
/// controls that automatically create a tooltip (eg TabControls with TCS_TOOLTIPS
/// style).
//
TTooltip::TTooltip(HWND handle, TModule* module)
:
  TControl(handle, module)
{
  PRECONDITION(handle);
  CHECK(TCommCtrl::IsAvailable());
}

//
/// Destructor of TTooltip class.
//
TTooltip::~TTooltip()
{
}

TTooltip* TTooltip::Make(TWindow* parent, bool alwaysTip, TModule* m)
{
  TTooltip* t = new TTooltip(parent, alwaysTip, m);
  if (parent->IsWindow())
  {
    bool r = t->Create();
    CHECK(r); InUse(r);
  }
  else
    t->SetFlag(wfAutoCreate);
  return t;
}

//
/// Returns the native class of the tooltip control or the class implementing OWL's
/// version of tooltips.
//
TWindow::TGetClassNameReturnType
TTooltip::GetClassName()
{
  PRECONDITION(TCommCtrl::IsAvailable());
  return TOOLTIPS_CLASS;
}

bool
TTooltip::AddTool(HWND ctrl, const tstring& tip, bool enableSubclassing)
{
  TToolInfo t(::GetParent(ctrl), ctrl, tip);
  t.EnableSubclassing(enableSubclassing);
  return AddTool(t);
}

bool
TTooltip::AddTool(HWND ctrl, int tipResId, TModule* m, bool enableSubclassing)
{
  PRECONDITION(GetModule());
  TModule& module = m ? *m : *GetModule();
  TToolInfo t(::GetParent(ctrl), ctrl, tipResId, module.GetHandle());
  t.EnableSubclassing(enableSubclassing);
  return AddTool(t);
}

bool
TTooltip::AddTool(const TRect& tipArea, uint toolId, const tstring& tip, bool enableSubclassing)
{
  PRECONDITION(GetParent());
  TToolInfo t(GetParent()->GetHandle(), tipArea, toolId, tip);
  t.EnableSubclassing(enableSubclassing);
  return AddTool(t);
}

bool
TTooltip::AddTool(const TRect& tipArea, uint toolId, int tipResId, TModule* m, bool enableSubclassing)
{
  PRECONDITION(GetParent() && GetModule());
  TModule& module = m ? *m : *GetModule();
  TToolInfo t(GetParent()->GetHandle(), tipArea, toolId, tipResId, module.GetHandle());
  t.EnableSubclassing(enableSubclassing);
  return AddTool(t);
}

} // OWL namespace
/* ========================================================================== */

