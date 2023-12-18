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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::ASN1MIB mib;

			NotNullPtr<UI::GUIPanel> pnlRequest;
			NotNullPtr<UI::GUILabel> lblFile;
			UI::GUITextBox *txtFile;
			NotNullPtr<UI::GUIButton> btnBrowse;
			UI::GUITabControl *tcMain;

			NotNullPtr<UI::GUITabPage> tpObjects;
			UI::GUIListView *lvObjects;
			NotNullPtr<UI::GUIVSplitter> vspObjects;
			UI::GUIListView *lvObjectsVal;

			NotNullPtr<UI::GUITabPage> tpOID;
			UI::GUIListView *lvOID;

			NotNullPtr<UI::GUITabPage> tpOIDText;
			UI::GUITextBox *txtOIDText;

			static void __stdcall OnFileDroped(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnObjectsSelChg(void *userObj);
			void LoadFile(Text::CStringNN fileName);
		public:
			AVIRASN1MIBForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRASN1MIBForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
