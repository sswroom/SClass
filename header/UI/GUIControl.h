#ifndef _SM_UI_GUICONTROL
#define _SM_UI_GUICONTROL
#include "Handles.h"
#include "Data/ArrayList.h"
#include "Math/Size2D.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorInfo.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUICore.h"

namespace UI
{
	class GUIClientControl;
	class GUIForm;

	class GUIDropData
	{
	public:
		virtual UOSInt GetCount() = 0;
		virtual const UTF8Char *GetName(UOSInt index) = 0;
		virtual Bool GetDataText(const UTF8Char *name, NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
		virtual IO::Stream *GetDataStream(const UTF8Char *name) = 0;
	};

	class GUIDropHandler
	{
	public:
		typedef enum
		{
			DE_NONE,
			DE_COPY,
			DE_MOVE,
			DE_LINK
		} DragEffect;
	public:
		virtual DragEffect DragEnter(GUIDropData *data) = 0;
		virtual void DropData(GUIDropData *data, OSInt x, OSInt y) = 0;
	};

	class GUIControl
	{
	public:
		typedef enum
		{
			DOCK_NONE,
			DOCK_LEFT,
			DOCK_TOP,
			DOCK_RIGHT,
			DOCK_BOTTOM,
			DOCK_FILL
		} DockType;

		typedef enum
		{
			MBTN_LEFT,
			MBTN_RIGHT,
			MBTN_MIDDLE,
			MBTN_X1,
			MBTN_X2
		} MouseButton;

		typedef enum
		{
			CT_ARROW,
			CT_HAND,
			CT_SIZEH,
			CT_SIZEV,
			CT_SIZENW,
			CT_SIZENE
		} CursorType;

		typedef enum
		{
			GK_NONE,
			GK_BACKSPACE,
			GK_TAB,
			GK_CLEAR,
			GK_ENTER,
			GK_SHIFT,
			GK_CONTROL,
			GK_ALT,
			GK_PAUSE,
			GK_CAPITAL,
			GK_KANA,
			GK_JUNJA,
			GK_FINAL,
			GK_KANJI,
			GK_ESCAPE,
			GK_CONVERT,
			GK_NONCONVERT,
			GK_ACCEPT,
			GK_MODECHANGE,
			GK_SPACE,
			GK_PAGEUP,
			GK_PAGEDOWN,
			GK_END,
			GK_HOME,
			GK_LEFT,
			GK_UP,
			GK_RIGHT,
			GK_DOWN,
			GK_SELECT,
			GK_PRINT,
			GK_EXECUTE,
			GK_PRINTSCREEN,
			GK_INSERT,
			GK_DELETE,
			GK_HELP,
			GK_0,
			GK_1,
			GK_2,
			GK_3,
			GK_4,
			GK_5,
			GK_6,
			GK_7,
			GK_8,
			GK_9,
			GK_A,
			GK_B,
			GK_C,
			GK_D,
			GK_E,
			GK_F,
			GK_G,
			GK_H,
			GK_I,
			GK_J,
			GK_K,
			GK_L,
			GK_M,
			GK_N,
			GK_O,
			GK_P,
			GK_Q,
			GK_R,
			GK_S,
			GK_T,
			GK_U,
			GK_V,
			GK_W,
			GK_X,
			GK_Y,
			GK_Z,
			GK_LWIN,
			GK_RWIN,
			GK_APPS,
			GK_SLEEP,
			GK_NUMPAD0,
			GK_NUMPAD1,
			GK_NUMPAD2,
			GK_NUMPAD3,
			GK_NUMPAD4,
			GK_NUMPAD5,
			GK_NUMPAD6,
			GK_NUMPAD7,
			GK_NUMPAD8,
			GK_NUMPAD9,
			GK_MULTIPLY,
			GK_ADD,
			GK_SEPARATOR,
			GK_SUBTRACT,
			GK_DECIMAL,
			GK_DIVIDE,
			GK_F1,
			GK_F2,
			GK_F3,
			GK_F4,
			GK_F5,
			GK_F6,
			GK_F7,
			GK_F8,
			GK_F9,
			GK_F10,
			GK_F11,
			GK_F12,
			GK_F13,
			GK_F14,
			GK_F15,
			GK_F16,
			GK_F17,
			GK_F18,
			GK_F19,
			GK_F20,
			GK_F21,
			GK_F22,
			GK_F23,
			GK_F24,
			GK_NUMLOCK,
			GK_SCROLLLOCK,
			GK_OEM_1,
			GK_OEM_PLUS,
			GK_OEM_COMMA,
			GK_OEM_MINUS,
			GK_OEM_PERIOD,
			GK_OEM_2,
			GK_OEM_3,
			GK_OEM_4,
			GK_OEM_5,
			GK_OEM_6,
			GK_OEM_7
		} GUIKey;

		typedef enum
		{
			DET_NOERROR,
			DET_UNKNOWN,
			DET_OUTOFMEMORY,
			DET_INVALIDCONTROL,
			DET_ALREADYREGISTER
		} DragErrorType;

		typedef Bool (__stdcall *MouseEventHandler)(void *userObj, Math::Coord2D<OSInt> scnCoord, MouseButton btn);


	protected:
		Data::ArrayList<UIEvent> resizeHandlers;
		Data::ArrayList<void *> resizeHandlersObjs;
		Bool inited;
		ControlHandle *hwnd;
		void *hFont;
		void *hbrBackground;
		NotNullPtr<GUICore> ui;
		DockType dockType;
		Optional<GUIClientControl> parent;
		Double lxPos;
		Double lyPos;
		Double lxPos2;
		Double lyPos2;
		Bool selfResize;
		MonitorHandle *currHMon;

		Text::String *fontName;
		Double fontHeightPt;
		Bool fontIsBold;
		Double hdpi;
		Double ddpi;

		void *dropHdlr;

	public:
		void InitControl(void *hInst, void *parentHWnd, const WChar *className, const UTF8Char *txt, UInt32 style, UInt32 exStyle, Double x, Double y, Double w, Double h);
		void InitControl(void *hInst, Optional<UI::GUIClientControl> parent, const WChar *className, const UTF8Char *txt, UInt32 style, UInt32 exStyle, Double x, Double y, Double w, Double h);

	protected:
		GUIControl(NotNullPtr<GUICore> ui, Optional<UI::GUIClientControl> parent);
	public:
		virtual ~GUIControl();

		void *GetHInst();
		virtual void *GetFont();
		virtual void Show();
		virtual void Close();
		virtual void SetText(Text::CStringNN text);
		virtual UTF8Char *GetText(UTF8Char *buff);
		virtual Bool GetText(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual void SetSize(Double width, Double height);
		virtual void SetSizeP(Math::Size2D<UOSInt> size);
		virtual Math::Size2DDbl GetSize();
		virtual Math::Size2D<UOSInt> GetSizeP();
		virtual void SetPosition(Double x, Double y);
		virtual Math::Coord2D<OSInt> GetPositionP();
		virtual Math::Coord2D<OSInt> GetScreenPosP();
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
		virtual void SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn);
		void SetRect(Double left, Double top, Double width, Double height, Bool updateScn);
		virtual void SetFont(const UTF8Char *name, UOSInt nameLen, Double ptSize, Bool isBold);
		void InitFont();
		virtual void SetDockType(DockType dockType);
		virtual DockType GetDockType();
		virtual void SetVisible(Bool isVisible);
		virtual Bool GetVisible();
		virtual void SetEnabled(Bool isEnable);
		virtual void SetBGColor(UInt32 bgColor);
		void *GetBGBrush();
		virtual Bool IsFormFocused();
		virtual void Focus();
		virtual OSInt GetScrollHPos();
		virtual OSInt GetScrollVPos();
		void ScrollTo(OSInt x, OSInt y);

		virtual Text::CStringNN GetObjectClass() const = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void OnSizeChanged(Bool updateScn);
		virtual void OnPosChanged(Bool updateScn);
		virtual void OnShow();
		virtual void OnMonitorChanged();

		virtual void HandleSizeChanged(UIEvent handler, void *userObj);

		virtual void UpdateFont();
		virtual void UpdatePos(Bool redraw);
		void UpdateBGColor();
		virtual void Redraw();
		void SetCapture();
		void ReleaseCapture();
		void SetCursor(CursorType curType);

		UInt32 GetColorBg();
		UInt32 GetColorText();
		UInt32 GetColorTextAlt();
		UInt32 GetColorHightlight();
		UInt32 GetColorHightlightText();

		virtual Optional<GUIClientControl> GetParent();
		GUIForm *GetRootForm();
		ControlHandle *GetHandle();
		MonitorHandle *GetHMonitor();
		Media::MonitorInfo *GetMonitorInfo();
		virtual void SetDPI(Double hdpi, Double ddpi);
		Double GetHDPI();
		Double GetDDPI();

		Media::DrawFont *CreateDrawFont(NotNullPtr<Media::DrawImage> img);
		static UInt32 GUIKey2OSKey(GUIKey guiKey);
		static GUIKey OSKey2GUIKey(UInt32 osKey);
		static Text::CStringNN GUIKeyGetName(GUIKey guiKey);

		DragErrorType HandleDropEvents(GUIDropHandler *hdlr);

		virtual void DestroyObject();
	};
}
#endif
