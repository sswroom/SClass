#ifndef _SM_SSWR_AVIREAD_AVIRHASHTESTFORM
#define _SM_SSWR_AVIREAD_AVIRHASHTESTFORM
#include "Crypto/Hash/IHash.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHashTestForm : public UI::GUIForm
		{
		private:
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpAlgorithm;
			NotNullPtr<UI::GUILabel> lblAlgorithm;
			NotNullPtr<UI::GUIComboBox> cboAlgorithm;
			NotNullPtr<UI::GUIButton> btnSpeed;
			NotNullPtr<UI::GUILabel> lblSpeed;
			NotNullPtr<UI::GUITextBox> txtSpeed;

			NotNullPtr<UI::GUITabPage> tpCompare;
			NotNullPtr<UI::GUIPanel> pnlCompare;
			NotNullPtr<UI::GUIButton> btnCompare;
			UI::GUIListView *lvCompare;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static Double __stdcall HashTestSpeed(Crypto::Hash::IHash *hash);
			static void __stdcall OnCompareClicked(void *userObj);
			static void __stdcall OnSpeedClicked(void *userObj);
		public:
			AVIRHashTestForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHashTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
