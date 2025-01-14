#ifndef _SM_SSWR_AVIREAD_AVIRBRUTEFORCEFORM
#define _SM_SSWR_AVIREAD_AVIRBRUTEFORCEFORM
#include "Crypto/Hash/BruteForceAttack.h"
#include "IO/Device/IBuddy.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRBruteForceForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Crypto::Hash::BruteForceAttack> bforce;
			UInt64 lastCnt;
			Int64 lastTime;

			NN<UI::GUILabel> lblHashType;
			NN<UI::GUIComboBox> cboHashType;
			NN<UI::GUILabel> lblHashValue;
			NN<UI::GUITextBox> txtHashValue;
			NN<UI::GUILabel> lblEncoding;
			NN<UI::GUIComboBox> cboEncoding;
			NN<UI::GUILabel> lblMinLen;
			NN<UI::GUITextBox> txtMinLen;
			NN<UI::GUILabel> lblMaxLen;
			NN<UI::GUITextBox> txtMaxLen;
			NN<UI::GUILabel> lblCharType;
			NN<UI::GUIComboBox> cboCharType;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblStatus;
			NN<UI::GUITextBox> txtStatus;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
		public:
			AVIRBruteForceForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBruteForceForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
