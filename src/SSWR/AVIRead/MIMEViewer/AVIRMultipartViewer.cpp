#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMultipartViewer.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer::AVIRMultipartViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::MultipartMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;
	NEW_CLASS(this->subViewers, Data::ArrayList<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *>());

	UI::GUITabPage *tp;
	UOSInt i;
	UOSInt j;
	Text::IMIMEObj *subObj;
	SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;
	NEW_CLASS(this->tcParts, UI::GUITabControl(ui, ctrl));
	this->tcParts->SetDockType(UI::GUIControl::DOCK_FILL);

	const UTF8Char *defMsg = obj->GetDefMsg();
	if (defMsg && defMsg[0])
	{
		UI::GUITextBox *txt;
		tp = this->tcParts->AddTabPage((const UTF8Char*)"Default");
		NEW_CLASS(txt, UI::GUITextBox(ui, tp, obj->GetDefMsg(), true));
		txt->SetDockType(UI::GUIControl::DOCK_FILL);
		txt->SetReadOnly(true);
	}

	i = 0;
	j = obj->GetPartCount();
	while (i < j)
	{
		subObj = obj->GetPartObj(i);
		tp = this->tcParts->AddTabPage(subObj->GetSourceNameObj());
		viewer = SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(core, ui, tp, sess, subObj);
		this->subViewers->Add(viewer);
		i++;
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer::~AVIRMultipartViewer()
{
	SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;
	UOSInt i;
	i = this->subViewers->GetCount();
	while (i-- > 0)
	{
		viewer = this->subViewers->GetItem(i);
		DEL_CLASS(viewer);
	}
	DEL_CLASS(this->subViewers);
}
