//----------------------------------------------------------------------------
// ObjectWindow - OWL NExt
// Copyright 1999. Yura Bidus. All Rights reserved.
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
//  Class definition for TCoolGrid (owl::TControl).
//  (Generated by OWL 6.x Class Expert for MS VC++, Version 1.5)
//----------------------------------------------------------------------------
#if !defined(COOLGRID_H) // Sentry, use file only if it's not already included.
#define COOLGRID_H

#include <owl/control.h>
#include <owl/edit.h>
#include <owl/listbox.h>
#include <owl/combobox.h>
#include <owl/bitset.h>
#include <owl/functor.h>
#include <owl/scroller.h>
#include <owl/control.h>
#include <coolprj/defs.h>

#include <vector>
#include <map>

//
// Member and non-member action and conditional structures used with
// ForEach and FirstThat.
//
struct TCellActionOperator;
struct TCellCondOperator;
struct TColActionOperator;
struct TColCondOperator;
struct TCellFunctor;
struct TColumnFunctor;

#define COOLGRID_ENDEDIT (WM_OWLFIRST+1)

class _COOLCLASS TCoolGrid;

namespace owl {class _OWLCLASS TCelArray;};


//////////////////////////////////////////////
// typesafe incapsulation of cell location,
class _COOLCLASS TCellPos {
  public:
    TCellPos():col(-1),row(-1){}
    TCellPos(int _col, int _row):col(_col),row(_row){}
    TCellPos(const TCellPos& pos):col(pos.col),row(pos.row){}
    bool Valid() const
      {
        return col >= 0 && row >= 0;
      }
    bool operator ==(const TCellPos& other) const
      {
        return owl::ToBool(other.col == col && other.row == row);
      }
    bool operator !=(const TCellPos& other) const
      {
        return owl::ToBool(other.col != col || other.row != row);
      }

  public:
    int row;
    int col;
};

//---------------------------
// class TCellRange
// ~~~~~ ~~~~~~~~~~
//
class _COOLCLASS TCellRange {
  public:
    TCellRange():scol(-1),srow(-1),erow(-1),ecol(-1){}
    TCellRange(int _scol, int _srow, int _ecol, int _erow)
       :scol(_scol),srow(_srow),ecol(_ecol),erow(_erow){}
    TCellRange(const TCellRange& pos):scol(pos.scol),
        srow(pos.srow),ecol(pos.ecol),erow(pos.erow){}
    TCellRange(const TCellPos& spos, const TCellPos& epos):scol(spos.col),
        srow(spos.row),ecol(epos.col),erow(epos.row){}
    void Normalize();
    TCellPos Start() const { return TCellPos(scol,srow); }
    TCellPos End()const    { return TCellPos(ecol,erow); }
    bool Valid() const
      {
        return scol >= 0 && srow >= 0 && ecol >= 0 && erow >= 0;
      }
    bool Empty() const
      {
        return scol == ecol && srow == erow;
      }
    bool operator ==(const TCellRange& other) const
      {
        return owl::ToBool(other.scol == scol && other.srow == srow &&
                      other.ecol == ecol && other.erow == erow);
      }
    bool operator !=(const TCellRange& other) const
      {
        return owl::ToBool(other.scol != scol || other.srow == srow ||
                      other.ecol != ecol || other.erow == erow);
      }

  public:
    int srow;
    int scol;
    int erow;
    int ecol;
};

//
// class TCoolGridScroller
// ~~~~~ ~~~~~~~~~~~~~~~~~
//
//
class _COOLCLASS TCoolGridScroller: public owl::TScroller {
    typedef owl::TScroller Inherited;
  public:
    TCoolGridScroller(owl::TWindow* window, int xUnit, int yUnit, long xRange,
                  long yRange);
    virtual void SetSBarRange();
    virtual void  ScrollTo(long x, long y);

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TCoolGridScroller(const TCoolGridScroller&);
    TCoolGridScroller& operator=(const TCoolGridScroller&);
};

//{{owl::TControl = TCoolGrid}}
class _COOLCLASS TCoolGrid: public owl::TControl {
  typedef owl::TControl Inherited;
  public:
    //---------------------------------
    // InPlace edit class group
    class _COOLCLASS TInPlaceEdit {
      public:
        TInPlaceEdit();
        virtual ~TInPlaceEdit(){}
        virtual void Activate(const owl::TRect& rect, owl::uint firstKey) = 0;
        virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction) = 0;
        virtual TCoolGrid*  GetGrid() = 0;
        virtual void Delete(){}
        owl::uint         GetLastKey() { return LastKey; }
        // called on end edit to allow TCoolGrid extract data
        void         EndEdit();
      protected:
        owl::uint        LastKey;
        bool        ExitOnArrows;
      private:
        TInPlaceEdit(const TInPlaceEdit& edit);
        TInPlaceEdit& operator=(const TInPlaceEdit& edit);
    };
    //------------------------------------------------------------------------
    // class TInPlaceEditCtrl
    // ~~~~~ ~~~~~~~~~~~~~~~~
    class _COOLCLASS TInPlaceEditCtrl: public TInPlaceEdit, public owl::TEdit {
      public:
        TInPlaceEditCtrl(TCoolGrid& parent, owl::uint textLimit = 255);
        virtual ~TInPlaceEditCtrl();
        virtual void Activate(const owl::TRect& rect, owl::uint firstKey);
        virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction);
        virtual TCoolGrid*  GetGrid();
        virtual void Delete() { Clear(); }

        // responce functions
        void EvChar(owl::uint key, owl::uint repeatCount, owl::uint flags);
        void EvKeyDown(owl::uint key, owl::uint repeatCount, owl::uint flags);
        void EvKillFocus(HWND hWndGetFocus);
        owl::uint EvGetDlgCode(const MSG* msg);
      protected:
        DECLARE_RESPONSE_TABLE(TInPlaceEditCtrl);
    };
    //---------------------------------------------------------------------------
    // class TInPlaceListBox
    // ~~~~~ ~~~~~~~~~~~~~~~~
    // DropDownList combobox
    class _COOLCLASS TInPlaceListBox: public TInPlaceEdit, public owl::TComboBox {
      public:
        TInPlaceListBox(TCoolGrid& parent, owl::uint textLimit = 255);
        virtual ~TInPlaceListBox();
        virtual void SetupWindow();
        virtual void Activate(const owl::TRect& rect, owl::uint firstKey);
        virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction);
        virtual TCoolGrid*  GetGrid();
        owl::TComboBoxData& GetData() { return Data; }
        virtual void Delete() { Clear(); }

      protected:
        // responce functions
        void EvKeyDown(owl::uint key, owl::uint repeatCount, owl::uint flags);
        owl::uint EvGetDlgCode(const MSG*);
        void EvKillFocus(HWND hWndGetFocus);
        void Undo();

      protected_data:
        owl::TComboBoxData Data;

        DECLARE_RESPONSE_TABLE(TInPlaceListBox);
    };
    //-------------------------------------------------------------
    // class TInPlaceComboBox
    // ~~~~~ ~~~~~~~~~~~~~~~~
    // DropDown combobox
    class _COOLCLASS TInPlaceComboBox: public TInPlaceListBox {
      typedef TInPlaceListBox Inherited;
      public:
        class _COOLCLASS TLocalEdit: public owl::TEdit {
          typedef owl::TEdit Inherited;
          public:
            TLocalEdit(owl::TWindow* parent, HWND hWnd);
            TInPlaceComboBox* GetComboBox();

            // responce functions
            void EvKeyDown(owl::uint key, owl::uint repeatCount, owl::uint flags);
            void EvChar(owl::uint key, owl::uint repeatCount, owl::uint flags);
            owl::uint EvGetDlgCode(const MSG*);
            void EvKillFocus(HWND hWndGetFocus);
            void EvLButtonDblClk(owl::uint modKeys, const owl::TPoint& point);
          DECLARE_RESPONSE_TABLE(TLocalEdit);
        };
        friend class TLocalEdit;
      public:
        TInPlaceComboBox(TCoolGrid& parent, owl::uint textLimit = 255);
        virtual ~TInPlaceComboBox(){}

        void SetReadOnly(bool readonly = true);
        bool IsReadOnly() { return ReadOnly; }

        virtual void Activate(const owl::TRect& rect, owl::uint firstKey);
        virtual void SetupWindow();

      protected:
        // responce functions
        void EvKillFocus(HWND hWndGetFocus);
        void EvKeyDown(owl::uint key, owl::uint repeatCount, owl::uint flags);
      protected_data:
        TLocalEdit*  LocalEdit;
        bool         ReadOnly;

        DECLARE_RESPONSE_TABLE(TInPlaceComboBox);
    };
    //-------------------------------------------------
    // class TCell
    // ~~~~~ ~~~~~
    //
    class _COOLCLASS TCell: public owl::TBitFlags<owl::uint> {
      public:
        enum TCellFlags {
          cfTransfer = 0x00001,
          cfFocused  = 0x00002,
          //cfSelected = 0x00004,
        };
      public:
        TCell();
        virtual ~TCell();
        virtual TInPlaceEdit* GetInPlaceEdit(){ return 0; } // use default
        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
        virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction) { return 0;}

        // Mouse functins for Button like controls
        virtual void MouseDown(owl::uint modKeys, const owl::TPoint& point){}
        virtual void MouseMove(owl::uint modKeys, const owl::TPoint& point){}
        virtual void MouseUp(owl::uint modKeys, const owl::TPoint& point){}

        virtual void SysColorChange(){}

        void         EnableTransfer() { Set(cfTransfer);  }
        void         DisableTransfer(){ Clear(cfTransfer);}
        void         SetParent(TCoolGrid* parent) { Parent = parent; }
      protected_data:
        TCoolGrid*  Parent;
      private:
        TCell(const TCell& cell);
        TCell& operator=(const TCell& cell);
    };

    //---------------------------------------------
    // class TCellMap
    // ~~~~~ ~~~~~~~~
    class _COOLCLASS TCellMap: public std::map<int,TCell*
 #if defined(RWSTD_NO_DEFAULT_TEMPLATES) // for BC++ 5.0x compatibility
    ,std::less<int>
 #endif
    >{
      public:
        TCellMap(){}
        ~TCellMap(){ Flush(true);}
        void Flush(bool erase=true);
    };

    //-------------------------------------------
    // class TTextCell
    // ~~~~~ ~~~~~~~~~
    class _COOLCLASS TTextCell: public TCell{
      public:
        TTextCell();
        TTextCell(LPCTSTR text, owl::uint textLimit=255);
        TTextCell(const owl::tstring& text, owl::uint textLimit=255);
        virtual ~TTextCell();
        void    Clear()                       { SetText(0);     }
        void    SetText(LPCTSTR text);
        void    SetText(const owl::tstring& text);
        LPCTSTR GetText() const               { return Text;    }
        void    SetFormat(owl::uint format)        { Format = format;}
        owl::uint    GetFormat() const             { return Format;  }
        void    SetBgColor(const owl::TColor& clr) { BgColor = clr;  }
        const owl::TColor& GetBgColor() const      { return BgColor; }
        void    SetTxColor(const owl::TColor& clr) { TxColor = clr;  }
        const owl::TColor& GetTxColor() const      { return TxColor; }

        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
        // transfer function exist but flag cfTransfer cleared, because static do not
        // participated in transfer mechanizm by default.
        virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction);

      protected:
        LPTSTR  Text;
        owl::uint    TextLimit; // for transfer mechanism
        owl::uint    Format;
        owl::TColor  BgColor;
        owl::TColor  TxColor;
    };
    //------------------------------------------------
    // class TBitmapCell
    // ~~~~~ ~~~~~~~~~~~
    class _COOLCLASS TBitmapCell: public TCell {
      typedef TCell Inherited;
      public:
        TBitmapCell(const owl::TResId& resIdorIndex, int numImages,
                    int startImage = 0, bool sharedCels = false);
        virtual ~TBitmapCell();
        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
        virtual void SysColorChange();

        int SelectImage(int imageNum, bool immediate);
        int GetImage() const { return CurrentImage; }
      protected:
        owl::TResId     ResId;
        owl::TCelArray* ImageArray;
        int        ImageIndex;
        int        CurrentImage;
        int        NumImages;
        owl::TPoint     BitmapOrigin;
    };
    //------------------------------------------------
    // class TBitmapTextCell
    // ~~~~~ ~~~~~~~~~~~~~~~
    class _COOLCLASS TBitmapTextCell: public TTextCell {
      typedef TTextCell Inherited;
      public:
        TBitmapTextCell(const owl::TResId& resIdorIndex, int numImages,
                        LPCTSTR text = 0, owl::uint textLimit=255,
                        int startImage = 0, bool sharedCels = false);
        virtual ~TBitmapTextCell();
        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
        virtual void Layout(const owl::TRect& src,owl::TRect& txr,owl::TRect& bmpr);
        virtual void PaintBitmap(owl::TDC& dc, const owl::TRect& rect);
        virtual void SysColorChange();

        int SelectImage(int imageNum, bool immediate);
        int GetImage() const { return CurrentImage; }
      protected:
        owl::TResId     ResId;
        owl::TCelArray* ImageArray;
        int        ImageIndex;
        int        CurrentImage;
        int        NumImages;
        owl::TPoint     BitmapOrigin;
    };
    //------------------------------------------------
    // class TButtonCell
    // ~~~~~ ~~~~~~~~~~~
    class _COOLCLASS TButtonCell: public TBitmapTextCell {
      typedef TBitmapTextCell Inherited;
      public:
        enum TCellFlags {
          cfButtonDown = 0x00008,
        };
      public:
        TButtonCell(const owl::TResId& resIdorIndex, int numImages,
                        LPCTSTR text = 0, owl::uint textLimit=255,
                        int startImage = 0, bool sharedCels = false);
        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
        virtual void MouseDown(owl::uint modKeys, const owl::TPoint& point);
        virtual void MouseUp(owl::uint modKeys, const owl::TPoint& point);
        // new virtual
        virtual void Action(owl::uint modKeys, const owl::TPoint& point){}
    };
    //------------------------------------------------
    // class TCommandCell
    // ~~~~~ ~~~~~~~~~~~~
    class _COOLCLASS TCommandCell: public TButtonCell {
      typedef TButtonCell Inherited;
      public:
        TCommandCell(int Id, const owl::TResId& resIdorIndex, int numImages,
                     LPCTSTR text = 0, owl::uint textLimit=255,
                     int startImage = 0, bool sharedCels = false);
        virtual void Action(owl::uint modKeys, const owl::TPoint& point);
        int GetId() { return Id;}
      protected:
        int Id;
    };
    //------------------------------------------------
    // class TEditCell
    // ~~~~~ ~~~~~~~~~
    class _COOLCLASS TEditCell: public TTextCell {
      typedef TTextCell Inherited;
      public:
        TEditCell();
        TEditCell(LPCTSTR text, owl::uint textLimit=255);
        TEditCell(const owl::tstring& text, owl::uint textLimit=255);
        virtual ~TEditCell();
        virtual void SetInPlaceEdit(TInPlaceEdit*  edit);
        virtual TInPlaceEdit* GetGetInPlaceEdit(){ return InPlaceEdit; }

      protected_data:
        TInPlaceEdit* InPlaceEdit;
    };
    //----------------------------------------------------
    // class TComboBoxCell
    // ~~~~~ ~~~~~~~~~~~~~
    // ComboBox with stiel -> DropList
    class _COOLCLASS TComboBoxCell: public TTextCell {
      typedef TTextCell Inherited;
      public:
        enum TCellFlags {
          cfNoShowButton = 0x00008,
        };
        TComboBoxCell();
        TComboBoxCell(LPCTSTR text, owl::uint textLimit=255);
        TComboBoxCell(const owl::tstring& text, owl::uint textLimit=255);
        virtual ~TComboBoxCell();
        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
        virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction);
        void    SetInPlaceEdit(TInPlaceEdit*  edit);
        virtual TInPlaceEdit* GetGetInPlaceEdit(){ return InPlaceEdit; }
        void    ShowButton(bool show=true)
                  {
                    show?TCell::Clear(cfNoShowButton):TCell::Set(cfNoShowButton);
                  }
      protected_data:
        TInPlaceEdit* InPlaceEdit;
    };
    //------------------------------------------------
    // class THeaderCell
    // ~~~~~ ~~~~~~~~~~~
    class _COOLCLASS THeaderCell: public TTextCell {
      public:
        THeaderCell(){}
        THeaderCell(LPCTSTR text, owl::uint textLimit=255);
        THeaderCell(const owl::tstring& text, owl::uint textLimit=255);
        virtual ~THeaderCell(){}
        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
    };
    //-----------------------------------------------
    // TCellFunctor is a ptr to a fn taking TCoolGrid& and returning a TCell*
    //
    typedef owl::TFunctor2<TCell*, TCoolGrid&, const TCellPos&> TCellFunctor;
    //----------------------------------------------------
    // class TColumn
    // ~~~~~ ~~~~~~~
    class _COOLCLASS TColumn: public owl::TBitFlags<owl::uint> {
      public:
        enum TColumnFlags {
          cfTransfer = 0x00001,
        };
      public:
        TColumn(TCoolGrid& parent);
        virtual ~TColumn();
        virtual TInPlaceEdit* GetInPlaceEdit(){ return InPlaceEdit;}
        virtual void Paint(owl::TDC& dc, const owl::TRect& rect);
        virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction);

        TCell*  FirstThat(TCellCondOperator& test);
        TCell*  LastThat(TCellCondOperator& test);
        void    ForEach(TCellActionOperator& action);

        TCell* operator [](int loc);

        // Hight level function
        // TColumn owns cells and destroy them upon exit
        void  SetCell(int loc, TCell& cell);
        void  RemoveCell(int loc);  // destroys cell
        int   CellCount()  { return CellMap.size();} // actual count of cells

        // additional functions
        void          SetInPlaceEdit(TInPlaceEdit*  edit);
        void          SetFont(const owl::TFont& font);
        owl::TFont*        GetFont() { return Font ? Font : Parent.GetFont(); }
        void          SetBoldFont(const owl::TFont& font);
        owl::TFont*        GetBoldFont() const { return BoldFont ? BoldFont : Parent.GetBoldFont(); }

        TCellFunctor& GetCellBuilder()            { return CellBuilder;  }
        void          SetCellBuilder(TCellFunctor& func){ CellBuilder = func;}
        owl::TColor        GetDefBkColor() { return DefColor; }
        void          SetDefBkColor(const owl::TColor& clr) { DefColor = clr; }

        int           GetId() { return Id; }
        TCoolGrid&    GetParent(){ return Parent; }

        void          SetHeader(const owl::tstring& text);
        void          SetHeader(THeaderCell* cell);
        THeaderCell*  GetHeader() { return Header; }

      protected:
        void PaintDefaultHeader(owl::TDC& dc, const owl::TRect& rect);
        void PaintHeader(owl::TDC& dc, const owl::TRect& rect, THeaderCell* cell);

      protected_data:
        TCoolGrid&    Parent;
        int           Id;
        TCellMap      CellMap;      //
        owl::TColor        DefColor;     //
        owl::TFont*        Font;         // if exist it used?????
        owl::TFont*        BoldFont;     // if exist it used?????
        TInPlaceEdit* InPlaceEdit;  // if exist it used
        TCellFunctor  CellBuilder;  // if exist it used
        THeaderCell*  Header;

      private:
        TColumn(const TColumn& column);
        TColumn& operator=(const TColumn& column);
        friend class TCoolGrid;
    };
    friend class TColumn;
    //-----------------------------------------------
    // TColumnFunctor
    typedef owl::TFunctor2<TColumn*,TCoolGrid&,int> TColFunctor;

    //--------------------------------------------------
    // class TColumnMap
    // ~~~~~ ~~~~~~~~~~
    class _COOLCLASS TColumnMap: public std::map<int,TColumn*
 #if defined(RWSTD_NO_DEFAULT_TEMPLATES) // for BC++ 5.0x compatibility
    ,std::less<int>
 #endif
    >{
      public:
        TColumnMap(){}
        ~TColumnMap(){ Flush(true);}
        void Flush(bool erase=true);
    };
    //--------------------------------------------------
    // class TDimMap
    // ~~~~~ ~~~~~~~
    class TDimMap: public std::map<int,int
 #if defined(RWSTD_NO_DEFAULT_TEMPLATES) // for BC++ 5.0x compatibility
    ,std::less<int>
 #endif
    >{
      public:
        TDimMap(){}
    };
    //--------------------------------------------------
    // class THeaderMap
    // ~~~~~ ~~~~~~~~~~
    //class THeaderMap: public std::map<owl::uint,THeaderCell*>{
    class THeaderMap: public TCellMap {
      public:
        THeaderMap(){}
    };

    enum TGridFlags{
      gfVertHeader  = 0x00001,
      gfHorizHeader = 0x00002,
      gfBothHeaders = gfVertHeader|gfHorizHeader,
      gfResVHeader  = 0x00004,
      gfResHHeader  = 0x00008,
      gfResBoth     = gfResVHeader|gfResHHeader,
      gfRowSelect   = 0x00010,
      gfMabyEdit    = 0x00020,
      gfSelectable  = 0x00040,
      gfInVResize   = 0x00080,
      gfInHResize   = 0x00100,
      gfNoRowInvert = 0x00200,
    };
    // return flags from HitTest
    enum THitResult{
      hrNone,
      hrVHeader,
      hrVHDivider,
      hrHHeader,
      hrHHDivider,
      hrBothHds,
      hrGridBody,
    };
    struct TGridSelection{
      TGridSelection(TCoolGrid* parent):Parent(parent){}
      virtual ~TGridSelection(){}
      virtual void Paint(owl::TDC& dc) = 0;
      virtual void PaintHHeader(owl::TDC& dc){}
      virtual void PaintVHeader(owl::TDC& dc){}

      TCoolGrid* Parent;
    };
    struct _COOLCLASS TColSelection: public TGridSelection{
      TColSelection(TCoolGrid* parent, int col):TGridSelection(parent),Column(col){}
      virtual void Paint(owl::TDC& dc);
      virtual void PaintHHeader(owl::TDC& dc);

      int Column;
    };
    friend TColSelection;
    struct _COOLCLASS TRowSelection: public TGridSelection{
      TRowSelection(TCoolGrid* parent, int row):TGridSelection(parent),Row(row){}
      virtual void Paint(owl::TDC& dc);
      virtual void PaintVHeader(owl::TDC& dc);

      int Row;
    };
    friend TRowSelection;
    struct _COOLCLASS TRandSelection: public TGridSelection{
      TRandSelection(TCoolGrid* parent):TGridSelection(parent){}
      virtual void Paint(owl::TDC& dc);
    };
    friend TRandSelection;
    struct _COOLCLASS TAllSelection: public TGridSelection{
      TAllSelection(TCoolGrid* parent):TGridSelection(parent){}
      virtual void Paint(owl::TDC& dc);
      virtual void PaintHHeader(owl::TDC& dc);
      virtual void PaintVHeader(owl::TDC& dc);
    };
    friend TAllSelection;

  public:
    TCoolGrid(owl::TWindow* parent,int id,LPCTSTR title,int x, int y, int w, int h,owl::TModule* module = 0);
    virtual ~TCoolGrid();

    // Iterators that takes action class
    TColumn*  FirstThat(TColCondOperator& test);
    TColumn*  LastThat(TColCondOperator& test);
    void      ForEach(TColActionOperator& action);
    TCell*    FirstThat(TCellCondOperator& test);
    TCell*    LastThat(TCellCondOperator& test);
    void      ForEach(TCellActionOperator& action);

    // High level functions
    void      SetCell(const TCellPos& pos, TCell& cell);
    void      RemoveCell(const TCellPos& pos);

    // TCoolGrid own its columns, and destroys all of them when destroyed
    void      SetColumn(owl::uint loc, TColumn& col);
    void      RemoveColumn(owl::uint loc);
    owl::uint      ColumnCount() const { return ColumnMap.size(); }
    TColumn*  GetColumn(int loc);
    TColumn*  CreateColumn(int loc);

    // set/get grid dimentions
    void      SetGridDim(owl::uint cols, owl::uint rows, int col_width=-1, int row_width=-1);
    owl::uint      GetColCount() const { return ColCount; }
    owl::uint      GetRowCount() const { return RowCount; }
    void      SetRowHeight(owl::uint loc, owl::uint height);
    owl::uint      GetRowHeight(owl::uint loc) const;
    void      SetColumnWidth(owl::uint loc, owl::uint width);
    owl::uint      GetColumnWidth(owl::uint loc) const;

    // sells
    TCell*    GetCell(const TCellPos& pos);
    TCell*    GetFocusedCell()            { return GetCell(FocusedCell);  }
    const TCellPos& GetFocusPos()  const { return FocusedCell;           }

    TCellPos  Point2Pos(const owl::TPoint& point) const;
    void      GetCellRect(const TCellPos& pos, owl::TRect& rect) const;

    void      SetInPlaceEdit(TInPlaceEdit*  edit);
    TInPlaceEdit* GetInPlaceEdit(){ return InPlaceEdit;}

    void      SetColumnEdit(int column, TInPlaceEdit* edit);
    TInPlaceEdit* GetColumnEdit(int column);

    void      SetFont(const owl::TFont& font);
    owl::TFont*    GetFont() const                       { return Font;          }
    void      SetBoldFont(const owl::TFont& font);
    owl::TFont*    GetBoldFont() const                   { return BoldFont;      }

    // properties
    void      EnableVertHeader(bool enable=true)    { SetResetFlag(gfVertHeader, enable, true); }
    bool      IsVertHeader() const                  { return IsFlagSet(gfVertHeader); }
    void      EnableHorizHeader(bool enable=true)   { SetResetFlag(gfHorizHeader, enable, true); }
    bool      IsHorizHeader() const                 { return IsFlagSet(gfHorizHeader);}
    void      EnableVResize(bool enable = true)     { SetResetFlag(gfResVHeader, enable, false); }
    bool      IsVertResizable() const               { return IsFlagSet(gfResVHeader); }
    void      EnableHResize(bool enable = true)     { SetResetFlag(gfResHHeader, enable, false); }
    bool      IsHorResizable() const                { return IsFlagSet(gfResHHeader); }
    void      EnableRowSelection(bool enable = true){ SetResetFlag(gfRowSelect, enable, false); }
    bool      IsRowSelection() const                { return IsFlagSet(gfRowSelect);  }
    void      EnableSelection(bool enable = true);
    bool      IsSelectable() const                  { return IsFlagSet(gfSelectable); }
    void      EnableRowInvert(bool enable = true);
    bool      IsRowInverted() const                 { return !IsFlagSet(gfNoRowInvert);}

    THitResult    HitTest(const owl::TPoint& point);

    TCellFunctor& GetCellBuilder()                  { return CellBuilder;   }
    TColFunctor&  GetColBuilder()                   { return ColBuilder;    }
    void          SetCellBuilder(TCellFunctor& func){ CellBuilder = func;   }
    void          SetColBuilder(TColFunctor& func)  { ColBuilder = func;    }

    void          ShowInplaceEdit(owl::uint key);

    int           GetHeaderWidth() const      { return HeaderWidth;     }
    void          SetHeaderWidth(int width);
    int           GetHeaderHeight() const     { return HeaderHeight;    }
    void          SetHeaderHeight(int height);
    int           GetTopPos() const           { return Scroller->YPos;  }
    int           GetOffsetPos() const        { return Scroller->XPos;  }
    int           GetLineWidth() const        { return LineWidth;       }
    int           GetLineHeight() const       { return LineHeight;      }

    const owl::tstring& GetDefHorFormat() const      { return DefHorFormat;  }
    void          SetDefHorFormat(const owl::tstring& format);
    const owl::tstring& GetDefVertFormat() const     { return DefVertFormat; }
    void          SetDefVertFormat(const owl::tstring& format);
    void          SetHorTitle(owl::uint column, const owl::tstring& text);
    void          SetVertTitle(owl::uint row, const owl::tstring& text);
    void          SetVertTitle(owl::uint row, THeaderCell& cell);

    // action
    void          Cell2EditTransfer(TCell& cell, TInPlaceEdit& edit);
    void          Edit2CellTransfer(TInPlaceEdit& edit, TCell& cell);

    // Getting & releasing of the mouse by cells.
    //
    bool          CellSetCapture(TCell& cell);
    void          CellReleaseCapture(TCell& cell);
    TCell*        CellGetCaptured() { return Capture; }

    // Shared CelArray management
    //
    virtual void  SetCelArray(owl::TCelArray* sharedCels);
    virtual owl::TCelArray& GetCelArray(int minX = 0, int minY = 0);

//{{TCoolGridVIRTUAL_BEGIN}}
  public:
    virtual void Paint(owl::TDC& dc, bool erase, owl::TRect& rect);
    virtual void SetupWindow();
    virtual owl::uint Transfer(void* buffer, owl::TTransferDirection direction);
    virtual TGetClassNameReturnType GetClassName();
    virtual void GetWindowClass(WNDCLASS& wndClass);
    virtual int  FirstVisibleColumn();
    virtual int  FirstVisibleRow();
    virtual bool SetFocusCell(const TCellPos& pos);
  protected:
    virtual void PaintFocusedCell(owl::TDC& dc, const owl::TRect& rect);
    virtual void PaintCornerCell(owl::TDC& dc, const owl::TRect& rect);
    virtual void PaintDefaultColumn(owl::TDC& dc, const owl::TRect& rect, int index);
    virtual void PaintHeaderColumn(owl::TDC& dc, const owl::TRect& rect);
    virtual void AdjustScroller();
//{{TCoolGridVIRTUAL_END}}


    // internal functions???
    int           X2Index(int poin_x) const;
    int           Y2Index(int poin_y) const;
    int           Index2X(int pos_x) const;
    int           Index2Y(int pos_y) const;
    int           FindMaxColWidth() const;
    void          PaintHorizHeaderCell(owl::TDC& dc, const owl::TRect& rect, int index);
    void          PaintVDivider(const owl::TPoint& point, bool redraw = false);
    void          PaintHDivider(const owl::TPoint& point, bool redraw = false);

  protected:
    void          PaintRowLines(owl::TDC& dc, const owl::TRect& rect);
    void          PaintColLines(owl::TDC& dc, const owl::TRect& rect);
    void          PaintVertHeaderCell(owl::TDC& dc, const owl::TRect& rect, int index);
    bool          IsFlagSet(owl::uint flag) const  { return (Flags&flag) != 0;}
    void          SetFlag(owl::uint flag)          { Flags |= flag;  }
    void          ClearFlag(owl::uint flag)        { Flags &= ~flag; }
    void          InvalidateHeaders(const owl::TRect& rect);
    void          InvalidateCell(const TCellPos& pos);
    void          ResizeColumn(int column, int delta);
    void          ResizeRow(int row, int delta);
    void          CleanupSelection();
    void          SetResetFlag(owl::uint flag, bool status, bool redraw = false);
    void          PaintGridLines(owl::TDC& dc, const owl::TRect& rect);

  protected:
    owl::uint32          Flags;
    TColumnMap      ColumnMap;
    TDimMap         HeightMap;
    TDimMap         WidthMap;

    owl::TBitmap*        MemBitmap;
    owl::TFont*          Font;
    owl::TFont*          BoldFont;
    owl::TColor          DefColor;

    int             Width;
    int             Height;
    int             ColCount;
    int             RowCount;
    owl::TSize           CharSize;

    // Header data
    int             HeaderWidth;
    int             HeaderHeight;
    owl::tstring      DefVertFormat;
    owl::tstring      DefHorFormat;
    THeaderMap      VertTitles;

    // internal ??? -> can it be const???
    int             LineWidth;
    int             LineHeight;

    //
    TCellPos        FocusedCell;  // current cell
    TCell*          Capture;      // Cell that has captured the mouse
    TInPlaceEdit*   InPlaceEdit;  // global edit
    TInPlaceEdit*   CurrentEdit;  // CurrentEdit

    TColFunctor     ColBuilder;   // Column Builder
    TCellFunctor    CellBuilder;  // cell builder

    // Selection
    TGridSelection* Selection;
    TCellPos        SelectionStart; // start selection
    owl::TPoint          MouseDownPoint;
    // draw Resizer
    int             MinDividerIndex;
    owl::TPoint          LastDrawPoint;

    owl::TCelArray*      SharedCels;     // CelArray that can be shared by gadgets

  public://??????????????
    static int  DefaultWidth; //????? did we need it??
    static int  DefaultHeight;//????? did we need it??

//{{TCoolGridRSP_TBL_BEGIN}}
  protected:
    void EvLButtonDown(owl::uint modKeys, const owl::TPoint& point);
    void EvSize(owl::uint sizeType, const owl::TSize& size);
    void EvLButtonDblClk(owl::uint modKeys, const owl::TPoint& point);
    void EvKeyDown(owl::uint key, owl::uint repeatCount, owl::uint flags);
    owl::TResult EvEndEdit(owl::TParam1 wp, owl::TParam2 lp);
    void EvChar(owl::uint key, owl::uint repeatCount, owl::uint flags);
    bool EvSetCursor(HWND hWndCursor, owl::uint hitTest, owl::uint mouseMsg);
    void EvMouseMove(owl::uint modKeys, const owl::TPoint& point);
    void EvLButtonUp(owl::uint modKeys, const owl::TPoint& point);
    void EvSysColorChange();
//{{TCoolGridRSP_TBL_END}}

    DECLARE_RESPONSE_TABLE(TCoolGrid);
}; //{{TCoolGrid}}

//
//
//
#define TCell_FUNCTOR(func)              TFunctionTranslator2<TCoolGrid::TCell*,TCoolGrid&, const TCellPos&, TCoolGrid::TCell* (*)(TCoolGrid&, const TCellPos&)>(func)
#define TCell_MFUNCTOR(obj,memberFunc)   TMemberTranslator2<TCoolGrid::TCell*,TCoolGrid&, const TCellPos&, TCoolGrid::TCell* (obj::*)(TCoolGrid&, const TCellPos&)>(obj, memberFunc)
#define TColumn_FUNCTOR(func)            TFunctionTranslator2<TCoolGrid::TColumn*,TCoolGrid&, int, TCoolGrid::TColumn* (*)(TCoolGrid&, int)>(func)
#define TColumn_MFUNCTOR(obj,memberFunc) TMemberTranslator2<TCoolGrid::TColumn*,TCoolGrid&, int, TCoolGrid::TColumn* (obj::*)(TCoolGrid&, int)>(obj, memberFunc)

//
// Member and non-member action and conditional function types used with
// ForEach and FirstThat.
//
struct TCellActionOperator{
  virtual void operator()(TCoolGrid::TCell* cell) = 0;
};
struct TCellCondOperator{
  virtual bool operator()(TCoolGrid::TCell* cell) = 0;
};
struct TColActionOperator{
  virtual void operator()(TCoolGrid::TColumn* col) = 0;
};
struct TColCondOperator{
  virtual bool operator()(TCoolGrid::TColumn* col) = 0;
};


#endif  // COOLGRID_H sentry.

