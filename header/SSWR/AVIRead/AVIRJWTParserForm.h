#ifndef _SM_SSWR_AVIREAD_AVIRJWTPARSERFORM
#define _SM_SSWR_AVIREAD_AVIRJWTPARSERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRJWTParserForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::SSLEngine *ssl;

			UI::GUITextBox *txtJWT;
			UI::GUIHSplitter *hspJWT;
			UI::GUIPanel *pnlMain;
			UI::GUIPanel *pnlResult;
			UI::GUIButton *btnParse;
			UI::GUICheckBox *chkIgnoreSignValid;
			UI::GUILabel *lblStatus;
			UI::GUITextBox *txtStatus;

			UI::GUILabel *lblIssuer;
			UI::GUITextBox *txtIssuer;
			UI::GUILabel *lblSubject;
			UI::GUITextBox *txtSubject;
			UI::GUILabel *lblAudience;
			UI::GUITextBox *txtAudience;
			UI::GUILabel *lblExpTime;
			UI::GUITextBox *txtExpTime;
			UI::GUILabel *lblNotBefore;
			UI::GUITextBox *txtNotBefore;
			UI::GUILabel *lblIssueAt;
			UI::GUITextBox *txtIssueAt;
			UI::GUILabel *lblJWTId;
			UI::GUITextBox *txtJWTId;

			UI::GUIListView *lvPayload;
		
			static void __stdcall OnParseClicked(void *userObj);

		public:
			AVIRJWTParserForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJWTParserForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
