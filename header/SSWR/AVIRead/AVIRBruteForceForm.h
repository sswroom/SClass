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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Crypto::Hash::BruteForceAttack *bforce;
			UInt64 lastCnt;
			Int64 lastTime;

			NotNullPtr<UI::GUILabel> lblHashType;
			NotNullPtr<UI::GUIComboBox> cboHashType;
			NotNullPtr<UI::GUILabel> lblHashValue;
			NotNullPtr<UI::GUITextBox> txtHashValue;
			NotNullPtr<UI::GUILabel> lblEncoding;
			NotNullPtr<UI::GUIComboBox> cboEncoding;
			NotNullPtr<UI::GUILabel> lblMinLen;
			NotNullPtr<UI::GUITextBox> txtMinLen;
			NotNullPtr<UI::GUILabel> lblMaxLen;
			NotNullPtr<UI::GUITextBox> txtMaxLen;
			NotNullPtr<UI::GUILabel> lblCharType;
			NotNullPtr<UI::GUIComboBox> cboCharType;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUILabel> lblStatus;
			NotNullPtr<UI::GUITextBox> txtStatus;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
		public:
			AVIRBruteForceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRBruteForceForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
