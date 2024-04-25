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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpCPU;
			NN<UI::GUIListView> lvMain;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUIButton> btnUpload;
			NN<UI::GUIButton> btnCopyInfo;

			NN<UI::GUITabPage> tpCache;
			NN<UI::GUIListBox> lbCache;

			NN<UI::GUITabPage> tpFeature;
			NN<UI::GUIListView> lvFeature;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;

			static void __stdcall OnUploadClick(AnyType userObj);
			static void __stdcall OnCopyInfoClick(AnyType userObj);
		public:
			AVIRCPUInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCPUInfoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
