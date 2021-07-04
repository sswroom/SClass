#ifndef _SM_SSWR_AVIREAD_AVIRX509FILEFORM
#define _SM_SSWR_AVIREAD_AVIRX509FILEFORM
#include "Crypto/X509File.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRX509FileForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Crypto::X509File *file;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpASN1;
			UI::GUITextBox *txtASN1;

		public:
			AVIRX509FileForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Crypto::X509File *file);
			virtual ~AVIRX509FileForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
