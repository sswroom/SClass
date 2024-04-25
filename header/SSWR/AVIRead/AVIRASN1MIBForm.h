#ifndef _SM_SSWR_AVIREAD_AVIRASN1MIBFORM
#define _SM_SSWR_AVIREAD_AVIRASN1MIBFORM
#include "Net/ASN1MIB.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRASN1MIBForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Net::ASN1MIB mib;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUILabel> lblFile;
			NN<UI::GUITextBox> txtFile;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpObjects;
			NN<UI::GUIListView> lvObjects;
			NN<UI::GUIVSplitter> vspObjects;
			NN<UI::GUIListView> lvObjectsVal;

			NN<UI::GUITabPage> tpOID;
			NN<UI::GUIListView> lvOID;

			NN<UI::GUITabPage> tpOIDText;
			NN<UI::GUITextBox> txtOIDText;

			static void __stdcall OnFileDroped(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnObjectsSelChg(AnyType userObj);
			void LoadFile(Text::CStringNN fileName);
		public:
			AVIRASN1MIBForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRASN1MIBForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
