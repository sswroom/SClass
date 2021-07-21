#ifndef _SM_UI_GUITEXTFILEVIEW
#define _SM_UI_GUITEXTFILEVIEW
#include "Data/ArrayListUInt64.h"
#include "IO/FileStream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITextFileView : public GUIControl
	{
	public:
		typedef void (__stdcall *TextPosEvent)(void *userObj, UInt32 textPosX, UOSInt textPosY);
	private:
		Data::ArrayList<TextPosEvent> *textPosUpdHdlr;
		Data::ArrayList<void *> *textPosUpdObj;
		IO::FileStream *fs;
		static OSInt useCnt;
		UInt32 codePage;
		void *drawFont;
		UOSInt lastLineCnt;

		const UTF8Char *fileName;
		UInt8 *readBuff;
		UInt64 readBuffOfst;
		UOSInt readBuffSize;
		Data::ArrayListUInt64 *lineOfsts;
		UInt32 fileCodePage;
		UInt64 fileSize;

		Sync::Mutex *mut;
		Sync::Event *evtThread;
		Bool threadToStop;
		Bool threadRunning;
		Bool loadNewFile;
		Bool readingFile;
		UInt32 pageLineCnt;
		Int32 pageLineHeight;
		void *bgBmp;
		Int32 dispLineNumW;
		UInt32 selStartX;
		UOSInt selStartY;
		UInt32 selEndX;
		UOSInt selEndY;
		UInt32 selLastX;
		UOSInt selLastY;
		Bool mouseDown;
		Bool isSearching;
		const UTF8Char *srchText;

		UInt32 caretX;
		UOSInt caretY;
		Int32 caretDispX;
		Int32 caretDispY;

		static OSInt __stdcall TFVWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		static UInt32 __stdcall ProcThread(void *userObj);
		void Init(void *hInst);
		void Deinit(void *hInst);
		void OnPaint();
		void UpdateScrollBar();
		void CopySelected();
		void UpdateCaretPos();
		void EnsureCaretVisible();
		UOSInt GetLineCharCnt(UOSInt lineNum);
		void GetPosFromByteOfst(UInt64 byteOfst, UInt32 *txtPosX, UOSInt *txtPosY);
		void UpdateCaretSel(Bool noRedraw);

		void EventTextPosUpdated();
	public:
		GUITextFileView(UI::GUICore *ui, UI::GUIClientControl *parent);
		virtual ~GUITextFileView();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void UpdateFont();

		Bool IsLoading();
		UOSInt GetLineCount();
		void SetCodePage(UInt32 codePage);
		Bool LoadFile(const UTF8Char *fileName);
		const UTF8Char *GetFileName();
		void GetTextPos(Int32 scnPosX, Int32 scnPosY, UInt32 *textPosX, UOSInt *textPosY);
		UOSInt GetTextPosY();
		UInt32 GetTextPosX();
		void GoToText(UOSInt newPosY, UInt32 newPosX);
		void SearchText(const UTF8Char *txt);

		void HandleTextPosUpdate(TextPosEvent hdlr, void *obj);
	};
}
#endif
