#ifndef _SM_SSWR_AVIREAD_AVIRELECTRONICSCALEFORM
#define _SM_SSWR_AVIREAD_AVIRELECTRONICSCALEFORM
#include "Math/Unit/Mass.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUIRealtimeLineChart.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRElectronicScaleForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::DrawEngine> eng;
			Media::DrawImage *dimg;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIGroupBox> grpStream;
			NotNullPtr<UI::GUILabel> lblStream;
			NotNullPtr<UI::GUITextBox> txtStream;
			NotNullPtr<UI::GUIButton> btnStream;
			NotNullPtr<UI::GUILabel> lblWeight;
			NotNullPtr<UI::GUITextBox> txtWeight;
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpHistory;
			NotNullPtr<UI::GUIRealtimeLineChart> rlcHistory;

			NotNullPtr<UI::GUITabPage> tpDisplay;
			NotNullPtr<UI::GUIPictureBoxSimple> pbsDisplay;

			IO::Stream *stm;
			Bool threadRunning;
			Bool threadToStop;
			Bool remoteClosed;

			Double currWeight;
			Math::Unit::Mass::MassUnit currWeightUnit;
			Bool currWeightUpd;

		private:
			static void __stdcall OnStreamClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static UInt32 __stdcall RecvThread(void *userObj);

			void StopStream();
		public:
			AVIRElectronicScaleForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRElectronicScaleForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
