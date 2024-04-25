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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpAlgorithm;
			NN<UI::GUILabel> lblAlgorithm;
			NN<UI::GUIComboBox> cboAlgorithm;
			NN<UI::GUIButton> btnSpeed;
			NN<UI::GUILabel> lblSpeed;
			NN<UI::GUITextBox> txtSpeed;

			NN<UI::GUITabPage> tpCompare;
			NN<UI::GUIPanel> pnlCompare;
			NN<UI::GUIButton> btnCompare;
			NN<UI::GUIListView> lvCompare;

			NN<SSWR::AVIRead::AVIRCore> core;

			static Double __stdcall HashTestSpeed(Crypto::Hash::IHash *hash);
			static void __stdcall OnCompareClicked(AnyType userObj);
			static void __stdcall OnSpeedClicked(AnyType userObj);
		public:
			AVIRHashTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRHashTestForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
