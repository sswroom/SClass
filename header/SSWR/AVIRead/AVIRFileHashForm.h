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
				NN<Text::String> fileName;
				Int32 status;
				IO::FileCheck *fchk;
			} FileStatus;

		private:
			NN<UI::GUIPanel> pnlCheckType;
			NN<UI::GUILabel> lblCheckType;
			NN<UI::GUIComboBox> cboCheckType;
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUITabPage> tpTasks;
			NN<UI::GUITabPage> tpFiles;

			NN<UI::GUIPanel> pnlStatus;
			NN<UI::GUIPanel> pnlStatusFile;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUILabel> lblSpeed;
			NN<UI::GUITextBox> txtSpeed;
			NN<UI::GUILabel> lblTotalSize;
			NN<UI::GUITextBox> txtTotalSize;
			NN<UI::GUIProgressBar> prgFile;
			NN<UI::GUIRealtimeLineChart> rlcSpeed;

			NN<UI::GUIPanel> pnlTasks;
			NN<UI::GUIButton> btnTasksClear;
			NN<UI::GUIListView> lvTasks;

			NN<UI::GUIListView> lvFiles;

			NN<SSWR::AVIRead::AVIRCore> core;
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

			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnCheckTypeChg(AnyType userObj);
			static UInt32 __stdcall HashThread(AnyType userObj);
			void AddFile(Text::CString fileName);
			void UpdateUI();
		public:
			AVIRFileHashForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileHashForm();

			virtual void OnMonitorChanged();

			virtual void ProgressStart(Text::CString name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
