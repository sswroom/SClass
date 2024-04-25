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
			NN<UI::GUIListBox> lbGPU;
			NN<UI::GUIHSplitter> hspGPU;
			NN<UI::GUIListView> lvMain;

			NN<SSWR::AVIRead::AVIRCore> core;
			IO::GPUManager *gpuMgr;

			static void __stdcall OnGPUSelChange(AnyType userObj);
		public:
			AVIRGPUInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGPUInfoForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
