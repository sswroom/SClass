#include "Stdafx.h"
#include "IO/StmData/MemoryData.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEImageViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer::AVIRMIMEImageViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;
	this->imgList = 0;
	UOSInt buffSize;
	const UInt8 *buff = this->obj->GetRAWData(&buffSize);
	IO::StmData::MemoryData *data;
	NEW_CLASS(data, IO::StmData::MemoryData(buff, buffSize));
	this->imgList = (Media::ImageList*)core->GetParserList()->ParseFileType(data, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
	DEL_CLASS(data);

	NEW_CLASS(this->pbContent, UI::GUIPictureBoxDD(ui, ctrl, sess, true, false));
	this->pbContent->SetDockType(UI::GUIControl::DOCK_FILL);
	if (this->imgList)
	{
		this->pbContent->SetImage(imgList->GetImage(0, 0), false);
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer::~AVIRMIMEImageViewer()
{
	if (this->imgList)
	{
		DEL_CLASS(this->imgList);
	}
}
