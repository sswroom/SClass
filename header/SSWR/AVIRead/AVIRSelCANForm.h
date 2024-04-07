#ifndef _SM_SSWR_AVIREAD_AVIRSELCANFORM
#define _SM_SSWR_AVIREAD_AVIRSELCANFORM
#include "IO/CANHandler.h"
#include "IO/CANListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSelCANForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<IO::CANHandler> hdlr;
			IO::CANListener *listener;
			Optional<Net::SSLEngine> ssl;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpAXCAN;
			NotNullPtr<UI::GUILabel> lblAXCANBitRate;
			NotNullPtr<UI::GUIComboBox> cboAXCANBitRate;
			NotNullPtr<UI::GUIButton> btnAXCANSerial;
			NotNullPtr<UI::GUIButton> btnAXCANFile;

			static void __stdcall OnAXCANSerialClicked(AnyType userObj);
			static void __stdcall OnAXCANFileClicked(AnyType userObj);
		public:
			AVIRSelCANForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NotNullPtr<IO::CANHandler> hdlr);
			virtual ~AVIRSelCANForm();

			virtual void OnMonitorChanged();

			IO::CANListener *GetListener() const;
		};
	}
}
#endif
