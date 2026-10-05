#ifndef _SM_SSWR_AVIREAD_AVIRREDISCONNFORM
#define _SM_SSWR_AVIREAD_AVIRREDISCONNFORM
#include "IO/ConfigFile.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRRedisConnForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblServer;
			NN<UI::GUITextBox> txtServer;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblPWD;
			NN<UI::GUITextBox> txtPWD;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<IO::ConfigFile> redis;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRRedisConnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRRedisConnForm();

			virtual void OnMonitorChanged();

			Optional<IO::ConfigFile> GetRedis() const;
		};
	}
}
#endif
