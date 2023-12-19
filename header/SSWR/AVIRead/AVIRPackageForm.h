#ifndef _SM_SSWR_AVIREAD_AVIRPACKAGEFORM
#define _SM_SSWR_AVIREAD_AVIRPACKAGEFORM
#include "Data/ArrayListNN.h"
#include "IO/PackageFile.h"
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
				Crypto::Hash::IHash *hash;
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
				Text::StringBuilderUTF8 *sbError;
			};
		private:
			NotNullPtr<UI::GUITabControl> tcMain;
			UI::GUIProgressBar *prgStatus;

			NotNullPtr<UI::GUITabPage> tpFiles;
			UI::GUIListView *lvFiles;

			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUIPanel> pnlStatus;
			NotNullPtr<UI::GUILabel> lblStatusFile;
			NotNullPtr<UI::GUITextBox> txtStatusFile;
			NotNullPtr<UI::GUIPanel> pnlStatusBNT;
			NotNullPtr<UI::GUILabel> lblStatusFileSize;
			NotNullPtr<UI::GUITextBox> txtStatusFileSize;
			NotNullPtr<UI::GUILabel> lblStatusCurrSize;
			NotNullPtr<UI::GUITextBox> txtStatusCurrSize;
			NotNullPtr<UI::GUILabel> lblStatusCurrSpeed;
			NotNullPtr<UI::GUITextBox> txtStatusCurrSpeed;
			NotNullPtr<UI::GUILabel> lblStatusTimeLeft;
			NotNullPtr<UI::GUITextBox> txtStatusTimeLeft;
			UI::GUIListView *lvStatus;
			NotNullPtr<UI::GUIVSplitter> vspStatus;
			UI::GUIRealtimeLineChart *rlcStatus;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUITextBox> txtInfo;

			NotNullPtr<UI::GUITabPage> tpTest;
			NotNullPtr<UI::GUITextBox> txtTest;

			UI::GUIPopupMenu *mnuPopup;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<IO::PackageFile> packFile;
			IO::PackageFile *rootPackFile;
			Bool packNeedDelete;
			Text::String *initSel;

			Sync::Mutex statusFileMut;
			Bool statusFileChg;
			Text::String *statusFile;
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
			Text::String *progName;
			UInt64 progStartCnt;
			Bool progStarted;
			UInt64 progUpdateCurr;
			UInt64 progUpdateNew;
			Bool progUpdated;
			Bool progEnd;

			static UInt32 __stdcall ProcessThread(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall LVDblClick(void *userObj, UOSInt index);
			static void __stdcall OnStatusDblClick(void *userObj, UOSInt index);
			static void __stdcall OnFilesRightClick(void *userObj, Math::Coord2DDbl coord, UOSInt index);
			static void __stdcall OnFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);

			void GoUpLevel();
			void OpenItem(UOSInt index);
			void TestPackage(NotNullPtr<IO::ActiveStreamReader> reader, NotNullPtr<ReadSession> sess, NotNullPtr<IO::PackageFile> pack);
			void DisplayPackFile(NotNullPtr<IO::PackageFile> packFile);
			UOSInt PackFileIndex(UOSInt lvIndex);
			void UpdatePackFile(NotNullPtr<IO::PackageFile> packFile, Bool needDelete, Text::CString initSel);
			void PasteFiles(NotNullPtr<Data::ArrayListStringNN> files, Bool move);
		public:
			AVIRPackageForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<IO::PackageFile> packFile);
			virtual ~AVIRPackageForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();

			virtual void ProgressStart(Text::CString name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
