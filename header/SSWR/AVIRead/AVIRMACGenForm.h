#ifndef _SM_SSWR_AVIREAD_AVIRMACGENFORM
#define _SM_SSWR_AVIREAD_AVIRMACGENFORM
#include "Data/BTreeUTF8Map.h"
#include "Net/MACInfo.h"
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
		class AVIRMACGenForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUILabel *lblVendor;
			UI::GUIComboBox *cboVendor;
			UI::GUIButton *btnGenerate;
			UI::GUILabel *lblColonFormat;
			UI::GUITextBox *txtColonFormat;
			UI::GUILabel *lblPlainFormat;
			UI::GUITextBox *txtPlainFormat;
			UI::GUILabel *lblAdapter;
			UI::GUIComboBox *cboAdapter;
			UI::GUIButton *btnAdapterSet;

			Data::BTreeUTF8Map<Data::ArrayList<Net::MACInfo::MACEntry*>*> *macMap;

			static void __stdcall OnGenerateClicked(void *userObj);
			static void __stdcall OnAdapterSetClicked(void *userObj);
			static OSInt __stdcall ListCompare(Data::ArrayList<Net::MACInfo::MACEntry*> *list1, Data::ArrayList<Net::MACInfo::MACEntry*> *list2);
		public:
			AVIRMACGenForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMACGenForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
