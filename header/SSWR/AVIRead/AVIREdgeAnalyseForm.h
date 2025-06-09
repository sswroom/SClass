#ifndef _SM_SSWR_AVIREAD_AVIREDGEANALYSEFORM
#define _SM_SSWR_AVIREAD_AVIREDGEANALYSEFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREdgeAnalyseForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpWebHook;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnSave;
			NN<UI::GUIButton> btnHex;
			NN<UI::GUITextBox> txtEDID;
			
		public:
			AVIREdgeAnalyseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREdgeAnalyseForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
