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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::SSLEngine> ssl;
			Net::AzureManager *azure;
			Crypto::Token::JWToken *token;
			Crypto::Token::JWToken::VerifyType verifyType;

			NN<UI::GUITextBox> txtJWT;
			NN<UI::GUIHSplitter> hspJWT;
			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUIPanel> pnlResult;
			NN<UI::GUIButton> btnParse;
			NN<UI::GUILabel> lblParseStatus;
			NN<UI::GUITextBox> txtParseStatus;
			NN<UI::GUILabel> lblVerifyType;
			NN<UI::GUITextBox> txtVerifyType;
			NN<UI::GUILabel> lblVerifyStatus;
			NN<UI::GUITextBox> txtVerifyStatus;
			NN<UI::GUILabel> lblIssuer;
			NN<UI::GUITextBox> txtIssuer;
			NN<UI::GUILabel> lblSubject;
			NN<UI::GUITextBox> txtSubject;
			NN<UI::GUILabel> lblAudience;
			NN<UI::GUITextBox> txtAudience;
			NN<UI::GUILabel> lblExpTime;
			NN<UI::GUITextBox> txtExpTime;
			NN<UI::GUILabel> lblNotBefore;
			NN<UI::GUITextBox> txtNotBefore;
			NN<UI::GUILabel> lblIssueAt;
			NN<UI::GUITextBox> txtIssueAt;
			NN<UI::GUILabel> lblJWTId;
			NN<UI::GUITextBox> txtJWTId;

			NN<UI::GUIListView> lvPayload;
		
			static void __stdcall OnParseClicked(AnyType userObj);

		public:
			AVIRJWTParserForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRJWTParserForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
