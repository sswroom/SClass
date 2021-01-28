#include "stdafx.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Parser/FullParserList.h"
#include "SSWR/ColorDemo/ColorDemoForm.h"

void __stdcall SSWR::ColorDemo::ColorDemoForm::FileHandler(void *userObj, const WChar **files, OSInt nFiles)
{
	SSWR::ColorDemo::ColorDemoForm *me = (SSWR::ColorDemo::ColorDemoForm*)userObj;
	if (nFiles > 0)
	{
		OSInt i = 0;
		while (i < nFiles)
		{
			IO::StmData::FileData *fd;
			NEW_CLASS(fd, IO::StmData::FileData(files[i], false));
			Media::ImageList *imgList = (Media::ImageList*)me->parsers->ParseFileType(fd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
			DEL_CLASS(fd);
			if (imgList)
			{
				if (imgList->GetCount() > 0)
				{
					Int32 t;
					SDEL_CLASS(me->currImage);
					me->currImage = imgList->GetImage(0, &t)->CreateStaticImage();
					DEL_CLASS(imgList);
					me->CreatePrevImage();
					break;
				}
				else
				{
					DEL_CLASS(imgList);
				}
			}
		}
	}
}

void __stdcall SSWR::ColorDemo::ColorDemoForm::OnValueChanged(void *userObj, Int32 scrollPos)
{
	SSWR::ColorDemo::ColorDemoForm *me = (SSWR::ColorDemo::ColorDemoForm*)userObj;
	WChar sbuff[32];
	Double v = (scrollPos - 400) * 0.01;
	Text::StrDouble(sbuff, v);
	me->currValue = Math::Pow(2, v);
	me->lblValue->SetText(sbuff);

	if (me->currImage)
	{
		me->UpdatePrevImage();
	}
}

void __stdcall SSWR::ColorDemo::ColorDemoForm::OnPBResized(void *userObj)
{
	SSWR::ColorDemo::ColorDemoForm *me = (SSWR::ColorDemo::ColorDemoForm*)userObj;
	me->CreatePrevImage();
}

void SSWR::ColorDemo::ColorDemoForm::CreatePrevImage()
{
	this->pbMain->SetImage(0, false);
	SDEL_CLASS(this->currPrevImage);
	SDEL_CLASS(this->currDispImage);
	if (this->currImage)
	{
		this->currPrevImage = this->pbMain->CreatePreviewImage(this->currImage);
		this->currDispImage = this->currPrevImage->CreateStaticImage();
		this->UpdatePrevImage();
	}
}

void SSWR::ColorDemo::ColorDemoForm::UpdatePrevImage()
{
	Media::ColorProfile color(this->currPrevImage->info->color);
	if (color.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		color.GetRTranParam()->Set(this->colorMgr->GetDefPProfile()->GetRTranParam());
		color.GetGTranParam()->Set(this->colorMgr->GetDefPProfile()->GetGTranParam());
		color.GetBTranParam()->Set(this->colorMgr->GetDefPProfile()->GetBTranParam());
	}
	else if (color.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		color.GetRTranParam()->Set(this->colorMgr->GetDefVProfile()->GetRTranParam());
		color.GetGTranParam()->Set(this->colorMgr->GetDefVProfile()->GetGTranParam());
		color.GetBTranParam()->Set(this->colorMgr->GetDefVProfile()->GetBTranParam());
	}
	this->rgbFilter->SetParameter(0, this->currValue, 1.0, &color, this->currPrevImage->info->bpp);
	this->rgbFilter->ProcessImage(this->currPrevImage->data, this->currDispImage->data, this->currPrevImage->info->width, this->currPrevImage->info->height, this->currPrevImage->info->width * (this->currPrevImage->info->bpp >> 3), this->currPrevImage->info->width * (this->currPrevImage->info->bpp >> 3));
	this->pbMain->SetImage(this->currDispImage, true);
}

SSWR::ColorDemo::ColorDemoForm::ColorDemoForm(void *hInst, UI::MSWindowClientControl *parent, UI::MSWindowUI *ui, Media::ColorManager *colorMgr) : UI::MSWindowForm(hInst, parent, 1024, 480, ui)
{
	this->SetText(L"ColorDemo");
	this->HandleDropFiles(FileHandler, this);

	NEW_CLASS(this->parsers, Parser::FullParserList());
	NEW_CLASS(this->rgbFilter, Media::RGBColorFilter(colorMgr));
	this->colorMgr = colorMgr;
	this->colorSess = this->colorMgr->CreateSess(this->GetHMonitor());
	this->currValue = 1;
	this->currImage = 0;
	this->currPrevImage = 0;
	this->currDispImage = 0;

	NEW_CLASS(this->pnlMain, UI::MSWindowPanel(hInst, this));
	this->pnlMain->SetRect(0, 0, 10, UI::MSWindowHScrollBar::GetSystemSize() + 1, false);
	this->pnlMain->SetDockType(UI::MSWindowControl::DOCK_TOP);
	NEW_CLASS(this->lblValue, UI::MSWindowLabel(hInst, this->pnlMain, L"0"));
	this->lblValue->SetRect(0, 0, 50, 24, false);
	this->lblValue->SetDockType(UI::MSWindowControl::DOCK_RIGHT);
/*	NEW_CLASS(this->tbValue, UI::MSWindowTrackBar(hInst, this->pnlMain, 0, 800, 400));
	this->tbValue->SetDockType(UI::MSWindowControl::DOCK_FILL);
	this->tbValue->HandleScrolled(OnValueChanged, this);*/
	NEW_CLASS(this->hsbValue, UI::MSWindowHScrollBar(hInst, this->pnlMain, UI::MSWindowHScrollBar::GetSystemSize()));
	this->hsbValue->SetDockType(UI::MSWindowControl::DOCK_FILL);
	this->hsbValue->InitScrollBar(0, 809, 400, 10);
	this->hsbValue->HandlePosChanged(OnValueChanged, this);
	NEW_CLASS(this->pbMain, UI::MSWindowPictureBoxDD(hInst, this, this->colorSess, false, false));
	this->pbMain->SetDockType(UI::MSWindowControl::DOCK_FILL);
	this->pbMain->HandleSizeChanged(OnPBResized, this);
}

SSWR::ColorDemo::ColorDemoForm::~ColorDemoForm()
{
	DEL_CLASS(this->parsers);
	DEL_CLASS(this->rgbFilter);
	SDEL_CLASS(this->currImage);
	SDEL_CLASS(this->currPrevImage);
	SDEL_CLASS(this->currDispImage);
	this->colorMgr->DeleteSess(this->colorSess);
}

void SSWR::ColorDemo::ColorDemoForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
}
