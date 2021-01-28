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
			UI::GUITimer *tmrMain;
			UI::GUILabel *lblCnt;
			UI::GUITextBox *txtCnt;
			SSWR::AVIRead::AVIRCore *core;
			Int64 paintCnt;
			Int64 dispCnt;

			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRPaintCntForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRPaintCntForm();

			virtual Bool OnPaint();
			virtual void OnMonitorChanged();
		};
	};
};
#endif
