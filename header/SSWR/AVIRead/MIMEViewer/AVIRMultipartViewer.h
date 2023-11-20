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
				UI::GUITabControl *tcParts;
				NotNullPtr<UI::GUIPanel> pnlSMIME;
				UI::GUILabel *lblSignState;
				UI::GUITextBox *txtSignState;

				Data::ArrayList<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer*> subViewers;
				Text::MIMEObj::MultipartMIMEObj *obj;

			public:
				AVIRMultipartViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::MultipartMIMEObj *obj);
				virtual ~AVIRMultipartViewer();
			};
		}
	}
}
#endif