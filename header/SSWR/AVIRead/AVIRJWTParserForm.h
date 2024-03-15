#ifndef _SM_SSWR_AVIREAD_AVIRJWTPARSERFORM
#define _SM_SSWR_AVIREAD_AVIRJWTPARSERFORM
#include "Crypto/Token/JWToken.h"
#include "Net/AzureManager.h"
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
			Optional<Net::SSLEngine> ssl;
			Net::AzureManager *azure;
			Crypto::Token::JWToken *token;
			Crypto::Token::JWToken::VerifyType verifyType;

			NotNullPtr<UI::GUITextBox> txtJWT;
			NotNullPtr<UI::GUIHSplitter> hspJWT;
			NotNullPtr<UI::GUIPanel> pnlMain;
			NotNullPtr<UI::GUIPanel> pnlResult;
			NotNullPtr<UI::GUIButton> btnParse;
			NotNullPtr<UI::GUILabel> lblParseStatus;
			NotNullPtr<UI::GUITextBox> txtParseStatus;
			NotNullPtr<UI::GUILabel> lblVerifyType;
			NotNullPtr<UI::GUITextBox> txtVerifyType;
			NotNullPtr<UI::GUILabel> lblVerifyStatus;
			NotNullPtr<UI::GUITextBox> txtVerifyStatus;
			NotNullPtr<UI::GUILabel> lblIssuer;
			NotNullPtr<UI::GUITextBox> txtIssuer;
			NotNullPtr<UI::GUILabel> lblSubject;
			NotNullPtr<UI::GUITextBox> txtSubject;
			NotNullPtr<UI::GUILabel> lblAudience;
			NotNullPtr<UI::GUITextBox> txtAudience;
			NotNullPtr<UI::GUILabel> lblExpTime;
			NotNullPtr<UI::GUITextBox> txtExpTime;
			NotNullPtr<UI::GUILabel> lblNotBefore;
			NotNullPtr<UI::GUITextBox> txtNotBefore;
			NotNullPtr<UI::GUILabel> lblIssueAt;
			NotNullPtr<UI::GUITextBox> txtIssueAt;
			NotNullPtr<UI::GUILabel> lblJWTId;
			NotNullPtr<UI::GUITextBox> txtJWTId;

			NotNullPtr<UI::GUIListView> lvPayload;
		
			static void __stdcall OnParseClicked(AnyType userObj);

		public:
			AVIRJWTParserForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJWTParserForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
