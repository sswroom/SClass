#include "Stdafx.h"
#include "IO/StmData/MemoryDataRef.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEImageViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer::AVIRMIMEImageViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::UnknownMIMEObj> obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;
	this->imgList = nullptr;
	UOSInt buffSize;
	UnsafeArray<const UInt8> buff = this->obj->GetRAWData(buffSize);
	{
		IO::StmData::MemoryDataRef data(buff, buffSize);
		this->imgList = Optional<Media::ImageList>::ConvertFrom(core->GetParserList()->ParseFileType(data, IO::ParserType::ImageList));
	}

	this->pbContent = ui->NewPictureBoxDD(ctrl, sess, true, false);
	this->pbContent->SetDockType(UI::GUIControl::DOCK_FILL);
	NN<Media::ImageList> imgList;
	if (this->imgList.SetTo(imgList))
	{
		this->pbContent->SetImage(imgList->GetImage(0, 0), false);
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEImageViewer::~AVIRMIMEImageViewer()
{
	this->imgList.Delete();
}
