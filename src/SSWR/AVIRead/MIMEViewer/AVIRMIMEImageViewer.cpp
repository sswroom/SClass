#include "Stdafx.h"
#include "IO/StmData/MemoryDataRef.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEImageViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer::AVIRMIMEImageViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;
	this->imgList = 0;
	UOSInt buffSize;
	const UInt8 *buff = this->obj->GetRAWData(&buffSize);
	{
		IO::StmData::MemoryDataRef data(buff, buffSize);
		this->imgList = (Media::ImageList*)core->GetParserList()->ParseFileType(data, IO::ParserType::ImageList);
	}

	this->pbContent = ui->NewPictureBoxDD(ctrl, sess, true, false);
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
