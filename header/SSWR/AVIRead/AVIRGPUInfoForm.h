#ifndef _SM_SSWR_AVIREAD_AVIRGPUINFOFORM
#define _SM_SSWR_AVIREAD_AVIRGPUINFOFORM
#include "IO/GPUManager.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGPUInfoForm : public UI::GUIForm
		{
		private:
			UI::GUIListBox *lbGPU;
			UI::GUIHSplitter *hspGPU;
			UI::GUIListView *lvMain;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::GPUManager *gpuMgr;

			static void __stdcall OnGPUSelChange(void *userObj);
		public:
			AVIRGPUInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGPUInfoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
