#ifndef _SM_SSWR_AVIREAD_AVIRFILEHASHFORM
#define _SM_SSWR_AVIREAD_AVIRFILEHASHFORM
#include "IO/FileCheck.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileHashForm : public UI::GUIForm, public IO::ProgressHandler
		{
		private:
			typedef struct
			{
				NotNullPtr<Text::String> fileName;
				Int32 status;
				IO::FileCheck *fchk;
			} FileStatus;

		private:
			NotNullPtr<UI::GUIPanel> pnlCheckType;
			NotNullPtr<UI::GUILabel> lblCheckType;
			NotNullPtr<UI::GUIComboBox> cboCheckType;
			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUITabPage> tpTasks;
			NotNullPtr<UI::GUITabPage> tpFiles;

			NotNullPtr<UI::GUIPanel> pnlStatus;
			NotNullPtr<UI::GUIPanel> pnlStatusFile;
			NotNullPtr<UI::GUILabel> lblFile;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUILabel> lblSpeed;
			NotNullPtr<UI::GUITextBox> txtSpeed;
			NotNullPtr<UI::GUILabel> lblTotalSize;
			NotNullPtr<UI::GUITextBox> txtTotalSize;
			NotNullPtr<UI::GUIProgressBar> prgFile;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcSpeed;

			NotNullPtr<UI::GUIPanel> pnlTasks;
			NotNullPtr<UI::GUIButton> btnTasksClear;
			NotNullPtr<UI::GUIListView> lvTasks;

			NotNullPtr<UI::GUIListView> lvFiles;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Sync::Event fileEvt;
			Int32 threadStatus;
			Bool threadToStop;
			Data::ArrayList<FileStatus*> fileList;
			Sync::Mutex fileMut;
			Bool fileListChg;
			Data::DateTime lastTimerTime;
			Sync::Mutex readMut;
			Text::String *progName;
			Bool progNameChg;
			UInt64 readSize;
			UInt64 totalRead;
			UInt64 progCount;
			UInt64 progCurr;
			UInt64 progLastCount;
			Crypto::Hash::HashType currHashType;

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnCheckTypeChg(void *userObj);
			static UInt32 __stdcall HashThread(void *userObj);
			void AddFile(Text::CString fileName);
			void UpdateUI();
		public:
			AVIRFileHashForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileHashForm();

			virtual void OnMonitorChanged();

			virtual void ProgressStart(Text::CString name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
