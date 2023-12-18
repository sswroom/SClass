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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<UI::GUIPanel> pnlControl;
			NotNullPtr<UI::GUIButton> btnFile;
			NotNullPtr<UI::GUIButton> btnStore;
			NotNullPtr<UI::GUILabel> lblInfo;
			UI::GUIListView *lvContent;

			IO::BTScanLog *btLog;
			Net::MACInfoList *macList;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnStoreClicked(void *userObj);
			static void __stdcall OnContentDblClicked(void *userObj, UOSInt index);
			static void __stdcall OnContentSelChg(void *userObj);
			Bool LogFileStore();
			void LogUIUpdate();

			void UpdateStatus();
		public:
			AVIRBTScanLogForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::BTScanLog *btLog);
			virtual ~AVIRBTScanLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
