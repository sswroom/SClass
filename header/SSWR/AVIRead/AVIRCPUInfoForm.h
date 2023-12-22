#ifndef _SM_SSWR_AVIREAD_AVIRCPUINFOFORM
#define _SM_SSWR_AVIREAD_AVIRCPUINFOFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCPUInfoForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpCPU;
			NotNullPtr<UI::GUIListView> lvMain;
			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUIButton> btnUpload;
			NotNullPtr<UI::GUIButton> btnCopyInfo;

			NotNullPtr<UI::GUITabPage> tpCache;
			NotNullPtr<UI::GUIListBox> lbCache;

			NotNullPtr<UI::GUITabPage> tpFeature;
			NotNullPtr<UI::GUIListView> lvFeature;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnUploadClick(void *userObj);
			static void __stdcall OnCopyInfoClick(void *userObj);
		public:
			AVIRCPUInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCPUInfoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
