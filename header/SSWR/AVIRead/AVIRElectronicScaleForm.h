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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::DrawEngine> eng;
			Optional<Media::DrawImage> dimg;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIGroupBox> grpStream;
			NN<UI::GUILabel> lblStream;
			NN<UI::GUITextBox> txtStream;
			NN<UI::GUIButton> btnStream;
			NN<UI::GUILabel> lblWeight;
			NN<UI::GUITextBox> txtWeight;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpHistory;
			NN<UI::GUIRealtimeLineChart> rlcHistory;

			NN<UI::GUITabPage> tpDisplay;
			NN<UI::GUIPictureBoxSimple> pbsDisplay;

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
			static UInt32 __stdcall RecvThread(AnyType userObj);

			void StopStream();
		public:
			AVIRElectronicScaleForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRElectronicScaleForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
