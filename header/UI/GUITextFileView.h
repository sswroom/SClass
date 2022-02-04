#ifndef _SM_UI_GUITEXTFILEVIEW
#define _SM_UI_GUITEXTFILEVIEW
#include "Data/ArrayListUInt64.h"
#include "IO/FileStream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "UI/GUITextView.h"

namespace UI
{
	class GUITextFileView : public GUITextView
	{
	public:
		typedef void (__stdcall *TextPosEvent)(void *userObj, UInt32 textPosX, UOSInt textPosY);
	private:
		Data::ArrayList<TextPosEvent> *textPosUpdHdlr;
		Data::ArrayList<void *> *textPosUpdObj;
		IO::FileStream *fs;
		UInt32 codePage;
//		void *drawFont;
		UOSInt lastLineCnt;

		Text::String *fileName;
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
		UInt32 dispLineNumW;
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

		static UInt32 __stdcall ProcThread(void *userObj);

		void EnsureCaretVisible();
		void UpdateCaretSel(Bool noRedraw);
		void CopySelected();
		UOSInt GetLineCharCnt(UOSInt lineNum);
		void GetPosFromByteOfst(UInt64 byteOfst, UInt32 *txtPosX, UOSInt *txtPosY);

		void EventTextPosUpdated();
	public:
		GUITextFileView(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng);
		virtual ~GUITextFileView();

		virtual void EventLineUp();
		virtual void EventLineDown();
		virtual void EventPageUp();
		virtual void EventPageDown();
		virtual void EventLeft();
		virtual void EventRight();
		virtual void EventHome();
		virtual void EventEnd();
		virtual void EventLineBegin();
		virtual void EventLineEnd();
		virtual void EventCopy();
		virtual void EventMouseDown(OSInt scnX, OSInt scnY, MouseButton btn);
		virtual void EventMouseUp(OSInt scnX, OSInt scnY, MouseButton btn);
		virtual void EventMouseMove(OSInt scnX, OSInt scnY);
		virtual void EventTimerTick();
		virtual void DrawImage(Media::DrawImage *dimg);
		virtual void UpdateCaretPos();

		Bool IsLoading();
		UOSInt GetLineCount();
		void SetCodePage(UInt32 codePage);
		Bool LoadFile(const UTF8Char *fileName);
		Text::String *GetFileName();
		void GetTextPos(OSInt scnPosX, OSInt scnPosY, UInt32 *textPosX, UOSInt *textPosY);
		UOSInt GetTextPosY();
		UInt32 GetTextPosX();
		void GoToText(UOSInt newPosY, UInt32 newPosX);
		void SearchText(const UTF8Char *txt);

		void HandleTextPosUpdate(TextPosEvent hdlr, void *obj);
	};
}
#endif
