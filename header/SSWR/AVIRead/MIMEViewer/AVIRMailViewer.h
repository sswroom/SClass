#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMAILVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMAILVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/MailMessage.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMailViewer : public AVIRMIMEViewer
			{
			private:
				NotNullPtr<UI::GUITabControl> tcHeader;
				NotNullPtr<UI::GUITabPage> tpHeading;
				NotNullPtr<UI::GUIListView> lvRecp;
				NotNullPtr<UI::GUIPanel> pnlFrom;
				NotNullPtr<UI::GUILabel> lblFrom;
				NotNullPtr<UI::GUITextBox> txtFrom;
				NotNullPtr<UI::GUIPanel> pnlSubject;
				NotNullPtr<UI::GUILabel> lblSubject;
				NotNullPtr<UI::GUITextBox> txtSubject;
				NotNullPtr<UI::GUIPanel> pnlDate;
				NotNullPtr<UI::GUILabel> lblDate;
				NotNullPtr<UI::GUITextBox> txtDate;

				NotNullPtr<UI::GUITabPage> tpRAW;
				NotNullPtr<UI::GUIListView> lvRAW;

				NotNullPtr<UI::GUIPanel> pnlContent;

				Text::MIMEObj::MailMessage *mail;
				SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;

			public:
				AVIRMailViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::MailMessage *mail);
				virtual ~AVIRMailViewer();
			};
		}
	}
}
#endif