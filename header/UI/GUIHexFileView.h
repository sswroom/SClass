#ifndef _SM_UI_GUIHEXFILEVIEW
#define _SM_UI_GUIHEXFILEVIEW
#include "IO/FileAnalyse/IFileAnalyse.h"
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
		IO::StreamData *fd;
		Optional<IO::FileAnalyse::IFileAnalyse> analyse;
		Optional<IO::FileAnalyse::FrameDetail> frame;
		UInt64 fileSize;
		UInt64 currOfst;
		Data::ArrayList<OffsetChgHandler> hdlrList;
		Data::ArrayList<void *> hdlrObjList;
		UInt32 frameColor;
		UInt32 fieldColor;

	public:
		GUIHexFileView(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> deng);
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
		virtual void DrawImage(NotNullPtr<Media::DrawImage> dimg);
		virtual void UpdateCaretPos();

		Bool LoadFile(Text::CStringNN fileName, Bool dynamicSize);
		Bool LoadData(NotNullPtr<IO::StreamData> data, Optional<IO::FileAnalyse::IFileAnalyse> fileAnalyse);
		void GetTextPos(Double scnPosX, Double scnPosY, UInt64 *byteOfst);
		void GoToOffset(UInt64 ofst);
		UInt64 GetCurrOfst();
		UInt64 GetFileSize();
		UOSInt GetFileData(UInt64 ofst, UOSInt size, Data::ByteArray outBuff);

		void HandleOffsetChg(OffsetChgHandler hdlr, void *hdlrObj);

		Text::CString GetAnalyzerName();
		Bool GetFrameName(NotNullPtr<Text::StringBuilderUTF8> sb);
		UOSInt GetFieldInfos(NotNullPtr<Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo *>> fieldList);
		UOSInt GetAreaInfos(NotNullPtr<Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo *>> areaList);
		Bool GoToNextUnkField();
	};
}
#endif
