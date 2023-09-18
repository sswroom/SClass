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
			NotNullPtr<UI::GUITimer> tmrMain;
			NotNullPtr<UI::GUILabel> lblCnt;
			NotNullPtr<UI::GUITextBox> txtCnt;
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Int64 paintCnt;
			Int64 dispCnt;

			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRPaintCntForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRPaintCntForm();

			virtual Bool OnPaint();
			virtual void OnMonitorChanged();
		};
	}
}
#endif
