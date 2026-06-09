#ifndef _SM_SSWR_AVIREAD_AVIRLOGZIPPERFORM
#define _SM_SSWR_AVIREAD_AVIRLOGZIPPERFORM
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLogZipperForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblLogDir;
			NN<UI::GUITextBox> txtLogDir;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			static void __stdcall OnStartClicked(AnyType userObj);
		public:
			AVIRLogZipperForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogZipperForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
