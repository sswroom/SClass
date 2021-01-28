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

			UI::GUITabPage *tpAlgorithm;
			UI::GUILabel *lblAlgorithm;
			UI::GUIComboBox *cboAlgorithm;
			UI::GUIButton *btnSpeed;
			UI::GUILabel *lblSpeed;
			UI::GUITextBox *txtSpeed;

			UI::GUITabPage *tpCompare;
			UI::GUIPanel *pnlCompare;
			UI::GUIButton *btnCompare;
			UI::GUIListView *lvCompare;

			SSWR::AVIRead::AVIRCore *core;

			static Double __stdcall HashTestSpeed(Crypto::Hash::IHash *hash);
			static void __stdcall OnCompareClicked(void *userObj);
			static void __stdcall OnSpeedClicked(void *userObj);
		public:
			AVIRHashTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRHashTestForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
