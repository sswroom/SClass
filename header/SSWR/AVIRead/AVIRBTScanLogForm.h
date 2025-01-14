#ifndef _SM_SSWR_AVIREAD_AVIRBTSCANLOGFORM
#define _SM_SSWR_AVIREAD_AVIRBTSCANLOGFORM
#include "IO/BTScanLog.h"
#include "Net/MACInfoList.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBTScanLogForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUIButton> btnStore;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUIListView> lvContent;

			NN<IO::BTScanLog> btLog;
			Net::MACInfoList macList;

			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnStoreClicked(AnyType userObj);
			static void __stdcall OnContentDblClicked(AnyType userObj, UOSInt index);
			static void __stdcall OnContentSelChg(AnyType userObj);
			Bool LogFileStore();
			void LogUIUpdate();

			void UpdateStatus();
		public:
			AVIRBTScanLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::BTScanLog> btLog);
			virtual ~AVIRBTScanLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
