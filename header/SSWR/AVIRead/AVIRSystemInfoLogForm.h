#ifndef _SM_SSWR_AVIREAD_AVIRSYSTEMINFOLOGFORM
#define _SM_SSWR_AVIREAD_AVIRSYSTEMINFOLOGFORM
#include "IO/SystemInfoLog.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSystemInfoLogForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpSystem;
			NotNullPtr<UI::GUILabel> lblOSName;
			UI::GUITextBox *txtOSName;
			NotNullPtr<UI::GUILabel> lblOSVer;
			UI::GUITextBox *txtOSVer;
			NotNullPtr<UI::GUILabel> lblOSLocale;
			UI::GUITextBox *txtOSLocale;
			NotNullPtr<UI::GUILabel> lblArchitecture;
			UI::GUITextBox *txtArchitecture;
			NotNullPtr<UI::GUILabel> lblProductType;
			UI::GUITextBox *txtProductType;

			NotNullPtr<UI::GUITabPage> tpDevices;
			UI::GUIListView *lvDevices;

			NotNullPtr<UI::GUITabPage> tpDrivers;
			UI::GUIListView *lvDrivers;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SystemInfoLog *sysInfo;

		public:
			AVIRSystemInfoLogForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SystemInfoLog *sysInfo);
			virtual ~AVIRSystemInfoLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
