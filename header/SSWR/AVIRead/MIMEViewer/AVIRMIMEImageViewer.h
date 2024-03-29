#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEIMAGEVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEIMAGEVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUIPictureBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEImageViewer : public AVIRMIMEViewer
			{
			private:
				NotNullPtr<UI::GUIPictureBoxDD> pbContent;

				Text::MIMEObj::UnknownMIMEObj *obj;
				Media::ImageList *imgList;

			public:
				AVIRMIMEImageViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEImageViewer();
			};
		}
	}
}
#endif