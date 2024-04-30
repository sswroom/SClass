#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMULTIPARTVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMULTIPARTVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMultipartViewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUITabControl> tcParts;
				NN<UI::GUIPanel> pnlSMIME;
				NN<UI::GUILabel> lblSignState;
				NN<UI::GUITextBox> txtSignState;

				Data::ArrayListNN<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer> subViewers;
				NN<Text::MIMEObj::MultipartMIMEObj> obj;

			public:
				AVIRMultipartViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::MultipartMIMEObj> obj);
				virtual ~AVIRMultipartViewer();
			};
		}
	}
}
#endif