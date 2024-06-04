#ifndef _SM_UI_GUITEXTFILEVIEW
#define _SM_UI_GUITEXTFILEVIEW
#include "AnyType.h"
#include "Data/ArrayListUInt64.h"
#include "Data/ByteBuffer.h"
#include "Data/CallbackStorage.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "UI/GUITextView.h"

namespace UI
{
	class GUITextFileView : public GUITextView
	{
	public:
		typedef void (__stdcall *TextPosEvent)(AnyType userObj, UInt32 textPosX, UOSInt textPosY);
	private:
		enum class LoadFileType
		{
			None,
			FilePath,
			FileData,
			FileStream
		};

		Data::ArrayList<Data::CallbackStorage<TextPosEvent>> textPosUpdHdlr;
		IO::SeekableStream *fs;
		UInt32 codePage;
//		AnyType drawFont;
		UOSInt lastLineCnt;

		NN<Text::String> fileName;
		IO::StreamData *fileData;
		Data::ByteBuffer readBuff;
		UInt64 readBuffOfst;
		UOSInt readBuffSize;
		Data::ArrayListUInt64 lineOfsts;
		UInt32 fileCodePage;
		UInt64 fileSize;

		Sync::Mutex mut;
		Sync::Event evtThread;
		Bool threadToStop;
		Bool threadRunning;
		LoadFileType loadNewFile;
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
		Optional<Text::String> srchText;

		UInt32 caretX;
		UOSInt caretY;
		Int32 caretDispX;
		Int32 caretDispY;

		static UInt32 __stdcall ProcThread(AnyType userObj);

		void EnsureCaretVisible();
		void UpdateCaretSel(Bool noRedraw);
		void CopySelected();
		UOSInt GetLineCharCnt(UOSInt lineNum);
		void GetPosFromByteOfst(UInt64 byteOfst, UInt32 *txtPosX, UOSInt *txtPosY);

		void EventTextPosUpdated();
		void ClearFileStatus();
	public:
		GUITextFileView(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng);
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
		virtual void DrawImage(NN<Media::DrawImage> dimg);
		virtual void UpdateCaretPos();

		Bool IsLoading();
		UOSInt GetLineCount();
		void SetCodePage(UInt32 codePage);
		Bool LoadFile(NN<Text::String> fileName);
		Bool LoadStreamData(NN<IO::StreamData> fd);
		NN<Text::String> GetFileName() const;
		void GetTextPos(OSInt scnPosX, OSInt scnPosY, OutParam<UInt32> textPosX, OutParam<UOSInt> textPosY);
		UOSInt GetTextPosY();
		UInt32 GetTextPosX();
		void GoToText(UOSInt newPosY, UInt32 newPosX);
		void SearchText(Text::CStringNN txt);

		void HandleTextPosUpdate(TextPosEvent hdlr, AnyType obj);
	};
}
#endif
