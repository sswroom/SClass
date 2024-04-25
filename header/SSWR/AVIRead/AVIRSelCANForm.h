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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::CANHandler> hdlr;
			IO::CANListener *listener;
			Optional<Net::SSLEngine> ssl;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpAXCAN;
			NN<UI::GUILabel> lblAXCANBitRate;
			NN<UI::GUIComboBox> cboAXCANBitRate;
			NN<UI::GUIButton> btnAXCANSerial;
			NN<UI::GUIButton> btnAXCANFile;

			static void __stdcall OnAXCANSerialClicked(AnyType userObj);
			static void __stdcall OnAXCANFileClicked(AnyType userObj);
		public:
			AVIRSelCANForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, NN<IO::CANHandler> hdlr);
			virtual ~AVIRSelCANForm();

			virtual void OnMonitorChanged();

			IO::CANListener *GetListener() const;
		};
	}
}
#endif
