#ifndef _SM_SSWR_AVIREAD_AVIRMACGENFORM
#define _SM_SSWR_AVIREAD_AVIRMACGENFORM
#include "Data/BTreeUTF8Map.hpp"
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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUILabel> lblVendor;
			NN<UI::GUIComboBox> cboVendor;
			NN<UI::GUIButton> btnGenerate;
			NN<UI::GUILabel> lblColonFormat;
			NN<UI::GUITextBox> txtColonFormat;
			NN<UI::GUILabel> lblPlainFormat;
			NN<UI::GUITextBox> txtPlainFormat;
			NN<UI::GUILabel> lblAdapter;
			NN<UI::GUIComboBox> cboAdapter;
			NN<UI::GUIButton> btnAdapterSet;

			Data::BTreeUTF8Map<Optional<Data::ArrayListNN<Net::MACInfo::MACEntry>>> macMap;

			static void __stdcall OnGenerateClicked(AnyType userObj);
			static void __stdcall OnAdapterSetClicked(AnyType userObj);
			static OSInt __stdcall ListCompare(Optional<Data::ArrayListNN<Net::MACInfo::MACEntry>> list1, Optional<Data::ArrayListNN<Net::MACInfo::MACEntry>> list2);
		public:
			AVIRMACGenForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMACGenForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
