#ifndef _SM_SSWR_AVIREAD_AVIRPACKAGEFORM
#define _SM_SSWR_AVIREAD_AVIRPACKAGEFORM
#include "AnyType.h"
#include "Data/ArrayListNN.hpp"
#include "IO/PackageFile.h"
#include "IO/TableWriter.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/String.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPackageForm : public UI::GUIForm, public IO::ProgressHandler
		{
		private:
			typedef enum
			{
				AT_COPY,
				AT_MOVE,
				AT_DELETE,
				AT_COPYFAIL,
				AT_MOVEFAIL,
				AT_DELETEFAIL,
				AT_SUCCEED,
				AT_RETRYCOPY,
				AT_RETRYMOVE
			} ActionType;

		public:
			struct ReadSession
			{
				Optional<Crypto::Hash::HashAlgorithm> hash;
				Crypto::Hash::HashType currHash;
				UInt64 fileReadSize;
				UOSInt fileCnt;
				UOSInt dirCnt;
				UOSInt errorCnt;
				UInt64 totalSizeUncomp;
				UInt64 totalCompSize;
				UInt64 totalSize;
				UInt64 totalStoreSize;
				Data::Duration totalDurUncomp;
				Data::Duration totalDurComp;
				Double slowestSpeedUncomp;
				Double slowestSpeedComp;
				NN<Text::StringBuilderUTF8> sbError;
			};
		private:
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUIProgressBar> prgStatus;

			NN<UI::GUITabPage> tpFiles;
			NN<UI::GUIListView> lvFiles;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUIPanel> pnlStatus;
			NN<UI::GUILabel> lblStatusFile;
			NN<UI::GUITextBox> txtStatusFile;
			NN<UI::GUIPanel> pnlStatusBNT;
			NN<UI::GUILabel> lblStatusFileSize;
			NN<UI::GUITextBox> txtStatusFileSize;
			NN<UI::GUILabel> lblStatusCurrSize;
			NN<UI::GUITextBox> txtStatusCurrSize;
			NN<UI::GUILabel> lblStatusCurrSpeed;
			NN<UI::GUITextBox> txtStatusCurrSpeed;
			NN<UI::GUILabel> lblStatusTimeLeft;
			NN<UI::GUITextBox> txtStatusTimeLeft;
			NN<UI::GUIListView> lvStatus;
			NN<UI::GUIVSplitter> vspStatus;
			NN<UI::GUIRealtimeLineChart> rlcStatus;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUITextBox> txtInfo;

			NN<UI::GUITabPage> tpTest;
			NN<UI::GUITextBox> txtTest;

			NN<UI::GUIPopupMenu> mnuPopup;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::PackageFile> packFile;
			Optional<IO::PackageFile> rootPackFile;
			Bool packNeedDelete;
			Optional<Text::String> initSel;

			Sync::Mutex statusFileMut;
			Bool statusFileChg;
			Optional<Text::String> statusFile;
			UInt64 statusFileSize;
			UInt64 statusDispSize;
			Double statusDispSpd;
			IO::ActiveStreamReader::BottleNeckType statusBNT;
			IO::ActiveStreamReader::BottleNeckType statusDispBNT;

			Sync::Mutex fileMut;
			Data::ArrayListStringNN fileNames;
			Data::ArrayList<ActionType> fileAction;
			Bool statusChg;
			Bool threadRunning;
			Bool threadToStop;
			Sync::Event threadEvt;

			Sync::Mutex readMut;
			UInt64 readTotal;
			UInt64 readCurr;
			UInt64 readLast;
			Text::CString readCurrFile;
			UInt64 readFileCnt;
			Data::DateTime readLastTimer;
			Data::DateTime readReadTime;

			Sync::Mutex progMut;
			Optional<Text::String> progName;
			UInt64 progStartCnt;
			Bool progStarted;
			UInt64 progUpdateCurr;
			UInt64 progUpdateNew;
			Bool progUpdated;
			Bool progEnd;

			static UInt32 __stdcall ProcessThread(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall LVDblClick(AnyType userObj, UOSInt index);
			static void __stdcall OnStatusDblClick(AnyType userObj, UOSInt index);
			static void __stdcall OnFilesRightClick(AnyType userObj, Math::Coord2DDbl coord, UOSInt index);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);

			void GoUpLevel();
			void OpenItem(UOSInt index);
			void TestPackage(NN<IO::ActiveStreamReader> reader, NN<ReadSession> sess, NN<IO::PackageFile> pack);
			void DisplayPackFile(NN<IO::PackageFile> packFile);
			UOSInt PackFileIndex(UOSInt lvIndex);
			void UpdatePackFile(NN<IO::PackageFile> packFile, Bool needDelete, Text::CString initSel);
			void PasteFiles(NN<Data::ArrayListStringNN> files, Bool move);
			void WriteFileList(NN<IO::TableWriter> writer);
			static void WriteFileListInner(NN<IO::TableWriter> writer, NN<IO::PackageFile> packFile, UnsafeArray<UTF8Char> sbuff, UnsafeArray<UTF8Char> sptr);
		public:
			AVIRPackageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::PackageFile> packFile);
			virtual ~AVIRPackageForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			virtual void ProgressStart(Text::CStringNN name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
