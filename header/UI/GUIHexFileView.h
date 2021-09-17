#ifndef _SM_UI_GUIHEXFILEVIEW
#define _SM_UI_GUIHEXFILEVIEW
#include "IO/StmData/FileData.h"
#include "UI/GUITextView.h"

namespace UI
{
	class GUIHexFileView : public GUITextView
	{
	public:
		typedef void (__stdcall *OffsetChgHandler)(void *userObj, UInt64 offset);
	private:
		IO::FileStream *fs;
		IO::StmData::FileData *fd;
		UInt64 fileSize;
		UInt64 currOfst;
		Data::ArrayList<OffsetChgHandler> *hdlrList;
		Data::ArrayList<void *> *hdlrObjList;

	public:
		GUIHexFileView(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng);
		virtual ~GUIHexFileView();

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

		Bool LoadFile(const UTF8Char *fileName, Bool dynamicSize);
		void GetTextPos(Double scnPosX, Double scnPosY, UInt64 *byteOfst);
		void GoToOffset(UInt64 ofst);
		UInt64 GetCurrOfst();
		UOSInt GetFileData(UInt64 ofst, UOSInt size, UInt8 *outBuff);

		void HandleOffsetChg(OffsetChgHandler hdlr, void *hdlrObj);
	};
}
#endif
