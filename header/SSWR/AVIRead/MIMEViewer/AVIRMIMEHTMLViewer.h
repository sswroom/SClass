#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEHTMLVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEHTMLVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEHTMLViewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUITextBox> txtHTML;

				Text::MIMEObj::UnknownMIMEObj *obj;

			public:
				AVIRMIMEHTMLViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEHTMLViewer();
			};
		}
	}
}
#endif