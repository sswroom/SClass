#ifndef _SM_SSWR_AVIREAD_AVIRPAINTCNTFORM
#define _SM_SSWR_AVIREAD_AVIRPAINTCNTFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPaintCntForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITimer> tmrMain;
			NN<UI::GUILabel> lblCnt;
			NN<UI::GUITextBox> txtCnt;
			NN<SSWR::AVIRead::AVIRCore> core;
			Int64 paintCnt;
			Int64 dispCnt;

			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRPaintCntForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPaintCntForm();

			virtual Bool OnPaint();
			virtual void OnMonitorChanged();
		};
	}
}
#endif
