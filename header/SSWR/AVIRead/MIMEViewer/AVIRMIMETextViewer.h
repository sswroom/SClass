#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMETEXTVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMETEXTVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/TextMIMEObj.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMETextViewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUITextBox> txtContent;

				NN<Text::MIMEObj::TextMIMEObj> txt;

			public:
				AVIRMIMETextViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::TextMIMEObj> txt);
				virtual ~AVIRMIMETextViewer();
			};
		}
	}
}
#endif