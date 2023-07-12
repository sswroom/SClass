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
		private:
			UI::GUIPanel *pnlCtrl;
			UI::GUITabControl *tcMain;
			UI::GUIProgressBar *prgStatus;

			UI::GUITabPage *tpFiles;
			UI::GUIListView *lvFiles;

			UI::GUITabPage *tpStatus;
			UI::GUIPanel *pnlStatus;
			UI::GUILabel *lblStatusFile;
			UI::GUITextBox *txtStatusFile;
			UI::GUIPanel *pnlStatusBNT;
			UI::GUILabel *lblStatusFileSize;
			UI::GUITextBox *txtStatusFileSize;
			UI::GUILabel *lblStatusCurrSize;
			UI::GUITextBox *txtStatusCurrSize;
			UI::GUILabel *lblStatusCurrSpeed;
			UI::GUITextBox *txtStatusCurrSpeed;
			UI::GUILabel *lblStatusTimeLeft;
			UI::GUITextBox *txtStatusTimeLeft;
			UI::GUIListView *lvStatus;
			UI::GUIVSplitter *vspStatus;
			UI::GUIRealtimeLineChart *rlcStatus;

			UI::GUITabPage *tpInfo;
			UI::GUITextBox *txtInfo;

			UI::GUIPopupMenu *mnuPopup;

			SSWR::AVIRead::AVIRCore *core;
			IO::PackageFile *packFile;

			Sync::Mutex statusFileMut;
			Bool statusFileChg;
			Text::String *statusFile;
			UInt64 statusFileSize;
			UInt64 statusDispSize;
			Double statusDispSpd;
			IO::ActiveStreamReader::BottleNeckType statusBNT;
			IO::ActiveStreamReader::BottleNeckType statusDispBNT;

			Sync::Mutex fileMut;
			Data::ArrayListNN<Text::String> fileNames;
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
			void DisplayPackFile(IO::PackageFile *packFile);
		public:
			AVIRPackageForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::PackageFile *packFile);
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
