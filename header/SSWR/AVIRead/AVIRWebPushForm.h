#ifndef _SM_SSWR_AVIREAD_AVIRWEBPUSHFORM
#define _SM_SSWR_AVIREAD_AVIRWEBPUSHFORM
#include "Crypto/Cert/X509Key.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRWebPushForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblPrivateKey;
			NN<UI::GUITextBox> txtPrivateKey;
			NN<UI::GUIButton> btnPrivateKey;
			NN<UI::GUILabel> lblEndPoint;
			NN<UI::GUITextBox> txtEndPoint;
			NN<UI::GUILabel> lblEmail;
			NN<UI::GUITextBox> txtEmail;
			NN<UI::GUIButton> btnPush;
			NN<UI::GUILabel> lblPushStatus;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Crypto::Cert::X509Key> key;

			static void __stdcall OnPrivateKeyClicked(AnyType userData);
			static void __stdcall OnPushClicked(AnyType userData);
			static void __stdcall OnFiles(AnyType userData, Data::DataArray<NN<Text::String>> files);
			Bool OpenFile(Text::CStringNN fileName);
		public:
			AVIRWebPushForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRWebPushForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
