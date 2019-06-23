//----------------------------------------------------------------------------
// ObjectWindow - OWL NExt
// Copyright 1999-2000. Yura Bidus. All Rights reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TEditPropDlg (TPropertyPage).
//  (Generated by OWL 6.x Class Expert for MS VC++, Version 1.5)
//----------------------------------------------------------------------------
#include <coolprj/pch.h>
#pragma hdrstop

#include <owl/checkbox.h>
#include <owl/configfl.h>
#include <coolprj/defs.h>
#include <coolprj/clrpropdlg.h>
#include <coolprj/cooledit.h>
#include <coolprj/cooledit.rh>  // Definition of cooledit

using namespace owl;
using namespace std;


#define CF_NOTTFONTS  0x10000000L

struct ENUMSETUP {
  int      Mode;
  void*    Ptr;
  uint32  Flags;
};

struct ENUMSIZE {
  TComboBox*  pComboBox;
  int          MinSize;
  int          MaxSize;
};

#define ENUM_FONTNAMES    1
#define ENUM_FONTSTYLES    2
#define ENUM_STYLEMATCH    3

bool DrawMaskedBitmap (HDC hDC, int x, int y, HBITMAP hbm, int XOrigin, int nWidth);
static void  FillListFromResource (ENUMSIZE& enmSize, int nID);
static int CALLBACK EnumFontFamProc (const LOGFONT * elf, const TEXTMETRIC *, DWORD nType, LPARAM lpData);
static int CALLBACK EnumFontSizeProc (const LOGFONT *, const TEXTMETRIC * lptm, DWORD, LPARAM lpData);

//
//
//
TEditPropDlgXfer::TEditPropDlgXfer(const TPSheetFunctor& functor)
:
  Callback(functor)
  ,Dirty(false)
{
  memset(&LogFont,0,sizeof(LogFont));
}
//
void TEditPropDlgXfer::LoadData(TConfigFile* config)
{
  TCoolTextBuffer txtBuffer;

  if(config)
    txtBuffer.RestoreSyntaxDescr(*config);
  else
    txtBuffer.BuildDefSyntaxDescr();

//  TIniConfigFile file(".\\cooledit.ini");
//  txtBuffer.RestoreSyntaxDescr(file);
//  txtBuffer.BuildDefSyntaxDescr();

  txtBuffer.GetFont(LogFont);
  //LogFont.lfHeight *= -20;
  ArrayUnderline.Grow(COLORINDEX_LAST);
  ArrayItalic.Grow(COLORINDEX_LAST);
  ArrayBold.Grow(COLORINDEX_LAST);
  ColorText.Grow(COLORINDEX_LAST);
  ColorBk.Grow(COLORINDEX_LAST);

  for(int i = 0; i < COLORINDEX_LAST; i++){
    TCoolTextBuffer::TSyntaxDescr& descr = txtBuffer.GetSyntaxDescr(i);
    ArrayUnderline.Add((descr.FontIndex&0x04) != 0);
    ArrayItalic.Add((descr.FontIndex&0x02) != 0);
    ArrayBold.Add((descr.FontIndex&0x01) != 0);
    ColorText.Add(descr.TxColor);
    ColorBk.Add(descr.BkColor);
  }
}
//
void TEditPropDlgXfer::SaveData(TConfigFile& file)
{
  TCoolTextBuffer txtBuffer;

  //LogFont.lfHeight /= -20;
  txtBuffer.SetFont(LogFont);
  for(int i = 0; i < COLORINDEX_LAST; i++){
    int fondIdx = 0;
    if(ArrayBold[i])
      fondIdx |= 0x01;
    if(ArrayItalic[i])
      fondIdx |= 0x02;
    if(ArrayUnderline[i])
      fondIdx |= 0x04;
    txtBuffer.SetSyntaxDescr(i, TCoolTextBuffer::TSyntaxDescr(
                             ColorText[i],ColorBk[i],fondIdx)
                            );
  }
  txtBuffer.SaveSyntaxDescr(file);
}

//{{TEditPropDlg Implementation}}

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TEditPropDlg, TPropertyPage)
  //{{TEditPropDlgRSP_TBL_BEGIN}}
  EV_CBN_SELCHANGE(IDC_EDITFONTNAME,  EvFontname),
  EV_CBN_EDITCHANGE(IDC_EDITFONTNAME, EvFontname),
  EV_CBN_EDITCHANGE(IDC_EDITFONTSIZE, EvFontsize),
  EV_CBN_SELCHANGE(IDC_EDITFONTSIZE,  EvFontsize),
  EV_CBN_SELCHANGE(IDC_EDITCLRTEXT,    EvClrtext),
  EV_CBN_SELCHANGE(IDC_EDITCLRBK,      EvClrbk),
  EV_COMMAND(IDC_EDITBOLD,            EvCommandEditBold),
  EV_COMMAND(IDC_EDITITALIC,          EvCommandEditItalic),
  EV_COMMAND(IDC_EDITUNDERLINE,        EvCommandEditUnderline),
  EV_LBN_SELCHANGE(IDC_EDITCATEGORY,  EvSelchangeEditCategory),
  EV_WM_MEASUREITEM,
  EV_PSN_APPLY(Apply),
  EV_PSN_RESET(Reset),
  //{{TEditPropDlgRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TEditPropDlg
//
TEditPropDlg::TEditPropDlg(TPropertySheet* parent, TEditPropDlgXfer& xfer,
                           TResId iconRes, TModule* module)
:
  TPropertyPage(parent, IDD_COOLEDITPROP, NULL, iconRes, module),
  SampleFont(0),
  XFer(xfer)
{
//{{TEditPropDlgXFER_USE}}
  Fontname  = new TFontNameList(this, IDC_EDITFONTNAME, FontList);
  Fontsize  = new TComboBox(this, IDC_EDITFONTSIZE, 1);
  Category  = new TListBox(this, IDC_EDITCATEGORY);
  ColorText = new TColorList(this, IDC_EDITCLRTEXT);
  ColorBk    = new TColorList(this, IDC_EDITCLRBK);
  Underline = new TCheckBox(this, IDC_EDITUNDERLINE, 0);
  Italic    = new TCheckBox(this, IDC_EDITITALIC, 0);
  Bold      = new TCheckBox(this, IDC_EDITBOLD, 0);
  FontSample= new TFontSample(this, IDC_EDITSAMPLE);
//{{TEditPropDlgXFER_USE_END}}

  ENUMSETUP  enmSetup;
  enmSetup.Mode    = ENUM_FONTNAMES;
  enmSetup.Ptr    = &FontList;
  enmSetup.Flags  = CF_BOTH|CF_FIXEDPITCHONLY;

  TScreenDC  dc;
  dc.SetMapMode (MM_TWIPS);
  dc.EnumFontFamilies(NULL, EnumFontFamProc, &enmSetup);

  ColorText->AddDefColors();
  ColorBk->AddDefColors();
}
//
TEditPropDlg::~TEditPropDlg()
{
  delete SampleFont;
}
//
void TEditPropDlg::SetupWindow()
{
  // Call base class function.
  TPropertyPage::SetupWindow();

  for (int i = 0; i < (int)FontList.Size(); i++)
    Fontname->AddString(FontList[i].GetName());

  TStringArray array;
  ExtractStrings(array, IDS_SYNTAXDESCR,_T('|'),GetModule());
  for (int j = 0; j < (int)array.Size(); j++)
    Category->AddString(array[j].c_str());

  int  index = Fontname->FindStringExact(owl::tstring(XFer.LogFont.lfFaceName), -1);
  if (index != -1){
    // set up name
    Fontname->SetSelIndex(index);
    EvFontname();
  }
  if(XFer.LogFont.lfHeight){
    _TCHAR buff[10];
    TScreenDC  dc;
    CyPixPerInch = dc.GetDeviceCaps(LOGPIXELSY);

    int size = -MulDiv(XFer.LogFont.lfHeight, 72, CyPixPerInch);
    Fontsize->SetText(_itot(size, buff, 10));
  }
  Category->SetSelIndex(0);
  EvSelchangeEditCategory();
  XFer.Dirty = false;
}
//
void TEditPropDlg::PageChanged()
{
  XFer.Dirty = true;
  GetSheet()->PageChanged(*this);
}
//
void TEditPropDlg::PageUnchanged()
{
  XFer.Dirty = false;
  GetSheet()->PageUnchanged(*this);
}
//
void TEditPropDlg::EvFontname()
{
  int  index = GetSelItem(Fontname);
  if(index<0)
    return;
  TScreenDC  dc;
  FontSelected(dc, index);
  PageChanged();
}
//
void TEditPropDlg::SinchronizeData()
{
  int index = Category->GetSelIndex();
  if(index < 0)
    return;

  XFer.ArrayBold[index]      = Bold->GetCheck() == BF_CHECKED;
  XFer.ArrayItalic[index]    = Italic->GetCheck() == BF_CHECKED;
  XFer.ArrayUnderline[index]= Underline->GetCheck() == BF_CHECKED;

  int  nFontName = Fontname->GetSelIndex();

  int sizeIndex = Fontsize->GetSelIndex();
  _TCHAR  szText[LF_FACESIZE];
  if(sizeIndex == -1)
    Fontsize->GetText(szText, 4);
  else
    Fontsize->GetString(szText, sizeIndex);
  int nFontSize = _ttoi (szText);

  if(nFontName == -1 || nFontSize < 4 || nFontSize > 255)
    return;  // no change

  // set up font
  //memcpy(&XFer.LogFont, &FontList[nFontName].GetLogFont(), sizeof(LOGFONT));

  // get bold/italic
  XFer.LogFont.lfItalic    = XFer.ArrayItalic[index];
  XFer.LogFont.lfWeight    = XFer.ArrayBold[index] ? FW_BOLD : FW_REGULAR;
  XFer.LogFont.lfUnderline= XFer.ArrayUnderline[index];
  lstrcpy(XFer.LogFont.lfFaceName, FontList[nFontName].GetName ());

  // get size
  XFer.LogFont.lfHeight  = 0 + (nFontSize * -20);  // calculate TWIPS height

  delete SampleFont;

  SampleFont = new TFont(XFer.LogFont);

  XFer.LogFont.lfHeight  = -MulDiv(nFontSize, CyPixPerInch, 72);

  FontSample->SetWindowFont(*SampleFont, true);
}
//
void TEditPropDlg::SetFontsize()
{
  GetSelItem(Fontsize);
  SinchronizeData();
}
//
void TEditPropDlg::EvFontsize()
{
  SetFontsize();
  PageChanged();
}
//
void TEditPropDlg::SetClrtext(int index)
{
  TColorDrawItem* item = (TColorDrawItem*)ColorText->GetItemData(ColorText->GetSelIndex());

  TColor color = item->GetColor();
  if(color == TColor::None)
    color = TCoolTextBuffer::GetDefSyntaxDescr(index)->TxColor;

  FontSample->SetTextColor(color);

  XFer.ColorText[index] = color;
}
//
void TEditPropDlg::EvClrtext()
{
  int index = Category->GetSelIndex();
  if(index < 0)
    return;

  SetClrtext(index);

  PageChanged();
}
//
void TEditPropDlg::SetClrbk(int index)
{
  TColorDrawItem* item = (TColorDrawItem*)ColorBk->GetItemData(ColorBk->GetSelIndex());

  TColor color = item->GetColor();
  if(color == TColor::None)
    color = TCoolTextBuffer::GetDefSyntaxDescr(index)->BkColor;

  FontSample->SetBkColor(color);

  XFer.ColorBk[index] = color;
}
//
void TEditPropDlg::EvClrbk()
{
  int index = Category->GetSelIndex();
  if(index < 0)
    return;

  SetClrbk(index);

  PageChanged();
}
//
void TEditPropDlg::EvCommandEditBold()
{
  SinchronizeData();
  PageChanged();
}
//
void TEditPropDlg::EvCommandEditItalic()
{
  SinchronizeData();
  PageChanged();
}
//
void TEditPropDlg::EvCommandEditUnderline()
{
  SinchronizeData();
  PageChanged();
}
//
void TEditPropDlg::EvSelchangeEditCategory()
{
  int index = Category->GetSelIndex();
  if(index < 0)
    return;

  // 1. Set colors
  if(XFer.ColorText[index]==TCoolTextBuffer::GetDefSyntaxDescr(index)->TxColor)
    ColorText->SetSelIndex(0);
  else{
    int idx = FindColorIndex16(XFer.ColorText[index]);
    if(idx > 0)
      ColorText->SetSelIndex(idx);
    else
      ColorText->SetSelIndex(0);
  }
  if(XFer.ColorBk[index]==TCoolTextBuffer::GetDefSyntaxDescr(index)->BkColor)
    ColorBk->SetSelIndex(0);
  else{
    int idx = FindColorIndex16(XFer.ColorBk[index]);
    if(idx > 0)
      ColorBk->SetSelIndex(idx);
    else
      ColorBk->SetSelIndex(0);
  }
  // 2. Set Underline/Italic/Bold
  Bold->SetCheck(XFer.ArrayBold[index]);
  Italic->SetCheck(XFer.ArrayItalic[index]);
  Underline->SetCheck(XFer.ArrayUnderline[index]);

  SetClrtext(index);
  SetClrbk(index);
  SinchronizeData();
}
//
void TEditPropDlg::EvMeasureItem(uint /*ctrlId*/, MEASUREITEMSTRUCT & measureInfo)
{
  TEXTMETRIC  tm;
  HFONT        hFont;
  TScreenDC    dc;
  hFont = GetWindowFont();
  if (hFont)
    dc.SelectObject (TFont(hFont));
  dc.GetTextMetrics(tm);
  measureInfo.itemHeight = tm.tmHeight;
}
//
int TEditPropDlg::GetSelItem(TComboBox* ptList)
{
  int  nIndex = ptList->GetSelIndex();

  if(nIndex == -1){
    owl::tstring szEditText = ptList->GetText();
    nIndex = ptList->FindStringExact(szEditText, -1);
    if (nIndex == -1)
      return -1;
    owl::tstring szLBText = ptList->GetString(nIndex);
    if (_tcsicmp (szLBText.c_str(), szEditText.c_str()) != 0)
      return -1;

    ptList->SendMessage(CB_SETCURSEL, nIndex);
  }
  return nIndex;
}
//
void TEditPropDlg::FontSelected(TDC& dc, int index)
{
  int     nSizeSelect;

  owl::tstring szSize = Fontsize->GetText();
  Fontsize->SetText(_T(""));
  Fontsize->ClearList();

  ENUMSIZE  enmSize;
  enmSize.pComboBox = Fontsize;
  enmSize.MinSize = 0;
  enmSize.MaxSize = 255;

  int nMapMode = dc.SetMapMode(MM_TWIPS);
  if((FontList[index].GetType() & TRUETYPE_FONTTYPE) ||
       (FontList[index].GetFamily() & TMPF_VECTOR))
    FillListFromResource(enmSize, IDS_FONTSIZES);
  else
    dc.EnumFonts(FontList[index].GetName(), EnumFontSizeProc, &enmSize);

  dc.SetMapMode(nMapMode);

  if (Fontsize->GetCount()){
    nSizeSelect = Fontsize->FindStringExact(szSize, -1);

    if (nSizeSelect == -1)
      nSizeSelect = Fontsize->FindStringExact (owl::tstring(_T("10")), -1);
    Fontsize->SetSelIndex(nSizeSelect == -1 ? 0 : nSizeSelect);
    SetFontsize();
  }
}
//
int TEditPropDlg::Apply(TPshNotify& notify)
{
  int retval = Inherited::Apply(notify);
  if(retval==PSNRET_NOERROR){
    SinchronizeData();
    if(XFer.Callback){
      retval = XFer.Callback(this,TEditPropDlgXfer::ctApply);
      if(retval==PSNRET_NOERROR)
        PageUnchanged();
    }
  }
  return retval;
}
//
void TEditPropDlg::Reset(TPshNotify& notify)
{
  Inherited::Reset(notify);
  if(XFer.Callback && XFer.Dirty){
    XFer.Callback(this,TEditPropDlgXfer::ctReset);
    PageUnchanged();
  }
}
//-----------------------------------------------------------------------------
// utility functions
//
static int CALLBACK
EnumFontFamProc (const LOGFONT* plf, const TEXTMETRIC* lptm,
                 DWORD type, LPARAM lpData)
{
  ENUMLOGFONT* elf = (ENUMLOGFONT*) plf;
  ENUMSETUP* setup = (ENUMSETUP*) lpData;

  switch (setup->Mode){
    case ENUM_FONTNAMES: {
      if (setup->Flags&CF_NOTTFONTS && (type&TRUETYPE_FONTTYPE))
        break;    // _IS_ a TrueType font (internal usage)
      if (setup->Flags&CF_TTONLY && !(type&TRUETYPE_FONTTYPE))
        break;    // not a TrueType font
      if (setup->Flags&CF_WYSIWYG && type!=(TRUETYPE_FONTTYPE|DEVICE_FONTTYPE))
        break;    // not available on both

      BYTE tmPitchAndFamily = (BYTE)(lptm->tmPitchAndFamily&0xF);
      if (setup->Flags&CF_NOVECTORFONTS && tmPitchAndFamily==TMPF_VECTOR)
        break;    // is a vector font

      if(setup->Flags & CF_SCALABLEONLY){
        if(!((tmPitchAndFamily&TMPF_VECTOR)||(tmPitchAndFamily&TMPF_TRUETYPE))){
          break;  // not a scalable font
        }
      }

      if(setup->Flags&CF_FIXEDPITCHONLY){
        if((elf->elfLogFont.lfPitchAndFamily&0xF)!=FIXED_PITCH)
          break;  // not a fixed font
      }

      int  charSet = elf->elfLogFont.lfCharSet;

      if(setup->Flags&CF_ANSIONLY && charSet!=ANSI_CHARSET)
        break;    // not an ANSI font

      if(setup->Flags&CF_NOOEMFONTS && charSet==OEM_CHARSET)
        break;    // is an OEM font

      TFontList* fontList = (TFontList*)setup->Ptr;
      fontList->Add(TFontDescr(type, elf->elfLogFont, lptm->tmPitchAndFamily));
      break;
    }
  }
  return TRUE;
}
//-----------------------------------------------------------------------------
static int CALLBACK
EnumFontSizeProc (const LOGFONT*, const TEXTMETRIC* lptm, DWORD, LPARAM lpData)
{
  ENUMSIZE* enmSize = (ENUMSIZE *) lpData;
  _TCHAR    szTemp[10];
  int  nSize = lptm->tmHeight + lptm->tmExternalLeading;
  nSize = (nSize + 10) / 20;
  wsprintf( szTemp, _T("%2d"), nSize );

  if (nSize >= enmSize->MinSize && nSize <= enmSize->MaxSize){
    // Dont add it if already there - added by B. Zink
    if ( enmSize->pComboBox->FindStringExact(owl::tstring(szTemp), -1 ) < 0){
      enmSize->pComboBox->AddString (szTemp);
    }
  }
   return TRUE;
}
//
// class TODComboBox
// ~~~~~ ~~~~~~~~~~~
// owner draw combobox - DrawItem function handles basic
// owner-drawing mechanism
//
void TODComboBox::DrawItem (DRAWITEMSTRUCT & drawInfo)
{
  TDC      dc(drawInfo.hDC);
  TRect    rc(drawInfo.rcItem);
  HBRUSH  hBrush;      // background brush

  if(drawInfo.itemState & ODS_SELECTED){
    dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
    dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
    hBrush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
  }
  else
    hBrush = (HBRUSH) SendMessage(WM_CTLCOLORLISTBOX, (WPARAM)drawInfo.hDC, (LPARAM)Handle);

  if(hBrush)
    dc.FillRect(rc, TBrush(hBrush));

  if (drawInfo.itemState & ODS_SELECTED)
    DeleteObject (hBrush);
  if ((int)drawInfo.itemID == -1)
    return;
  DrawText(dc, rc, drawInfo);
}
//
// class TFontNameList
// ~~~~~ ~~~~~~~~~~~~~
// combobox containing fontnames (with small icons)
//
TFontNameList::TFontNameList(TWindow* parent, int Id, TFontList& fonts,
                             TModule* module)
:
  TODComboBox (parent, Id, LF_FACESIZE, module),
  Fontlist (fonts),
  BmpType(*GetModule(), IDB_FONTTYPE)
{
}
//
void TFontNameList::DrawText(TDC& dc, TRect& rc, DRAWITEMSTRUCT & DrawInfo)
{
  int  nType = Fontlist[DrawInfo.itemID].GetType ();

  if (nType & TRUETYPE_FONTTYPE)
    nType = 0;
  else{
    if (nType & DEVICE_FONTTYPE)
      nType = 20;
    else
      nType = -1;
  }

  if (nType != -1)
    DrawMaskedBitmap(dc, rc.left, rc.top, BmpType, nType, 20);

  rc.left  += 20;

  dc.DrawText (Fontlist[DrawInfo.itemID].GetName(), -1, rc, DT_LEFT | DT_TOP);
}

//
TColorDrawItem::TColorDrawItem(TDrawItemProxy* proxy,
                               const TColor& color,
                               const owl::tstring& text)
:
  TButtonTextDrawItem(proxy),
  Color(color)
{
  if(Color != TColor::None)
    SetStyle(sBitmap);
  SetText(text.c_str());
}
//
void TColorDrawItem::GetButtonSize(TSize& btnSize)
{
  if(Index == sepNoBitmap) // == -2
    return;
  if(Color != TColor::None)
    btnSize = TSize(20,5);
  else
    btnSize = TSize(0,0);
}
//
void TColorDrawItem::PaintBitmap(TDC& dc, TRect& rect)
{
  //celArray.BitBlt(Index, dc, rect.left, rect.top);
  if(Color != TColor::None){
    TColor oldBkColor = dc.SetBkColor(Color);
    rect.Inflate(-2,-2);
    dc.ExtTextOut(rect.TopLeft(), ETO_OPAQUE, &rect, _T(""), 0, 0);
    dc.SetBkColor(oldBkColor);
    if (Color == TColor::Black)
      dc.FrameRect(rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
    else
      dc.FrameRect(rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
  }
}
//-----------------------------------------------------------------------------
// class TColorList
// ~~~~~ ~~~~~~~~~~
// combobox containing colors
TColorList::TColorList (TWindow* parent, int Id, TModule* module)
:
  TComboBox (parent, Id, 0, module),
  Proxy(0)
{
  SetProxy(new TDrawItemProxy);
}
//
TColorList::~TColorList ()
{
  delete Proxy;
}
//
void TColorList::SetProxy(TDrawItemProxy*  proxy)
{
  CHECK(GetHandle()==0);
  delete Proxy;
  Proxy = proxy;
}
//
void TColorList::AddDefColors()
{
  TStringArray array;
  ExtractStrings(array, IDS_COLORNAMES,_T('|'),GetModule());

  SetColor(TColor::None, array[0]);
  for (int i = 0; i < 16; i++)
    SetColor(GetDefColor16(i), array[i+1]);
}
//
void TColorList::SetColor(const TColor& color, const owl::tstring& text)
{
  TColorDrawItem* item = new TColorDrawItem(Proxy, color, text);
  Proxy->RegisterItem(item); // auto destroy feature
  if(GetHandle())
    AddString((LPCTSTR)item); // don't use flag item_string
}
//
void TColorList::SetupWindow ()
{
  TComboBox::SetupWindow();

  TDrawItemProxy::TDrawItemArray& items =  Proxy->GetItems();
  for (int i = 0; i < (int)items.Size(); i++)
    AddString((LPCTSTR)items[i]);
}
//
// class TFontSample
// ~~~~~ ~~~~~~~~~~~
//
TFontSample::TFontSample(TWindow* parent, int resId, TModule* module)
:
  TStatic(parent, resId, 1, module)
{
  TextColor = TColor::Black;
  BkColor    = TColor::White;
}
//
void TFontSample::SetupWindow()
{
  TStatic::SetupWindow();
  ClearFlag (wfPredefinedClass);
}
//
void TFontSample::SetTextColor(const TColor& clr)
{
  TextColor = clr;
  if(GetHandle())
    Invalidate ();
}
//
void TFontSample::SetBkColor(const TColor& clr)
{
  BkColor = clr;
  if(GetHandle())
    Invalidate ();
}
//
void TFontSample::Paint(TDC& dc, bool /*erase*/, TRect&)
{
  TRect  clientRect;
  GetClientRect(clientRect);
  _TCHAR szChars[30];

  dc.SetBkColor(BkColor);
  dc.ExtTextOut (0, 0, ETO_OPAQUE, &clientRect, _T(""), 0);

  GetText(szChars, sizeof(szChars));

  dc.SetTextColor(TextColor);
  HFONT hfont = GetWindowFont();

  dc.SetMapMode (MM_TWIPS);
  dc.DPtoLP (&clientRect.TopLeft ());
  dc.DPtoLP (&clientRect.BottomRight ());

  if (hfont)
    dc.SelectObject (TFont (hfont));

  WORD wFormat;

  if(dc.GetTextExtent(szChars, lstrlen (szChars)).cx > clientRect.right)
  {
  /*  int  nDiff = -20 * 72 / 16;  // move up 1/16 of an inch
    dc.SetTextAlign (TA_LEFT | TA_BASELINE);
    dc.ExtTextOut (0, rcClient.bottom - nDiff, ETO_CLIPPED, &rcClient, szChars, lstrlen (szChars));
    */
    wFormat = DT_LEFT;
  }
  else{
    wFormat = DT_CENTER;
  }

  dc.DrawText(szChars, -1, clientRect, (uint16)(wFormat | DT_VCENTER | DT_SINGLELINE));

  //TStatic::Paint(dc, erase, rect);
}
//
// =========================================================
// class TFontDescr
// ~~~~~ ~~~~~~~~~~
//
// defines an entry in the font array
// =========================================================
TFontDescr::TFontDescr ()
{
  memset(&lf, 0, sizeof (LOGFONT));
  Type = Family = 0;
}
//
TFontDescr::TFontDescr(int nTypeParam, const LOGFONT& lfParam, int nFamParam)
{
  memcpy (&lf, &lfParam, sizeof (LOGFONT));

  Type   = nTypeParam;
  Family  = nFamParam;
}
//
bool TFontDescr::operator< (const TFontDescr& other) const
{
  return (lstrcmpi (GetName (), other.GetName ()) < 0);
}
//
bool TFontDescr::operator== (const TFontDescr& other) const
{
  return (lstrcmpi(GetName(), other.GetName()) == 0);
}
//
// =========================================================
// FillListFromResource
//
// populates a lists from a string resource
// =========================================================
static void FillListFromResource(ENUMSIZE& enmSize, int /*nID*/)
{
  TStringArray array;
  ExtractStrings(array, IDS_FONTSIZES, _T('|'), enmSize.pComboBox->GetModule());
  for (int j = 0; j < (int)array.Size(); j++){
    int  nSize = _ttoi(array[j].c_str());
    if (nSize >= enmSize.MinSize && nSize <= enmSize.MaxSize)
      enmSize.pComboBox->AddString(array[j].c_str());
  }
}
//-----------------------------------------------------------------------------