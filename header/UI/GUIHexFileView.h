#ifndef _SM_UI_GUIHEXFILEVIEW
#define _SM_UI_GUIHEXFILEVIEW
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "IO/FileAnalyse/FileAnalyser.h"
#include "IO/StmData/FileData.h"
#include "UI/GUITextView.h"

namespace UI
{
	class GUIHexFileView : public GUITextView
	{
	public:
		typedef void (CALLBACKFUNC OffsetChgHandler)(AnyType userObj, UInt64 offset);
	private:
		IO::FileStream *fs;
		IO::StreamData *fd;
		Optional<IO::FileAnalyse::FileAnalyser> analyse;
		Optional<IO::FileAnalyse::FrameDetail> frame;
		UInt64 fileSize;
		UInt64 currOfst;
		Data::ArrayListObj<Data::CallbackStorage<OffsetChgHandler>> hdlrList;
		UInt32 frameColor;
		UInt32 fieldColor;

	public:
		GUIHexFileView(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng, Optional<Media::ColorSess> colorSess);
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
		virtual void EventMouseDown(IntOS scnX, IntOS scnY, MouseButton btn);
		virtual void EventMouseUp(IntOS scnX, IntOS scnY, MouseButton btn);
		virtual void EventMouseMove(IntOS scnX, IntOS scnY);
		virtual void EventTimerTick();
		virtual void DrawImage(NN<Media::DrawImage> dimg);
		virtual void UpdateCaretPos();

		Bool LoadFile(Text::CStringNN fileName, Bool dynamicSize);
		Bool LoadData(NN<IO::StreamData> data, Optional<IO::FileAnalyse::FileAnalyser> fileAnalyse);
		void GetTextPos(Double scnPosX, Double scnPosY, UInt64 *byteOfst);
		void GoToOffset(UInt64 ofst);
		UInt64 GetCurrOfst();
		UInt64 GetFileSize();
		UIntOS GetFileData(UInt64 ofst, UIntOS size, Data::ByteArray outBuff);

		void HandleOffsetChg(OffsetChgHandler hdlr, AnyType hdlrObj);

		Text::CString GetAnalyzerName();
		Bool GetFrameName(NN<Text::StringBuilderUTF8> sb);
		UIntOS GetFieldInfos(NN<Data::ArrayListNN<const IO::FileAnalyse::FrameDetail::FieldInfo>> fieldList);
		UIntOS GetAreaInfos(NN<Data::ArrayListNN<const IO::FileAnalyse::FrameDetail::FieldInfo>> areaList);
		Bool GoToNextUnkField();
		Optional<Data::ByteBuffer> GetDevrivedBuff() const;
		Optional<IO::FileAnalyse::FileAnalyser> CreateDevrivedAnaylse() const;
	};
}
#endif
