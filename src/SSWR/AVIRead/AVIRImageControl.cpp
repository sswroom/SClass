#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.h"
#include "Exporter/GUIJPGExporter.h"
#include "Exporter/GUIPNGExporter.h"
#include "Exporter/TIFFExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRImageControl.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIClientControl.h"

#include <stdio.h>

UInt32 __stdcall SSWR::AVIRead::AVIRImageControl::FolderThread(void *userObj)
{
	SSWR::AVIRead::AVIRImageControl *me = (SSWR::AVIRead::AVIRImageControl*)userObj;
	me->threadState = 1;
	me->folderCtrlEvt->Set();
	while (true)
	{
		me->folderThreadEvt->Wait();
		if (me->threadCtrlCode == 2)
		{
			break;
		}
		else if (me->threadCtrlCode == 1)
		{
			me->threadState = 2;
			me->threadCtrlCode = 0;
			me->folderCtrlEvt->Set();
			me->InitDir();
			me->threadState = 1;
		}
		else if (me->threadCtrlCode == 3)
		{
			me->threadCtrlCode = 0;
			me->folderCtrlEvt->Set();
		}
		else
		{
			if (me->exportList->HasItems())
			{
				me->threadState = 3;
				me->ExportQueued();
				me->threadState = 1;
			}
		}
	}
	me->threadState = 0;
	me->folderCtrlEvt->Set();
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRImageControl::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRImageControl *me = (SSWR::AVIRead::AVIRImageControl*)userObj;
	if (me->imgMapUpdated)
	{
		UOSInt imgCnt;
		UOSInt totalHeight;
		me->imgMapUpdated = false;
		Sync::MutexUsage mutUsage(me->imgMut);
		imgCnt = me->imgMap->GetCount();
		mutUsage.EndUse();
		totalHeight = imgCnt * (20 + 12 + 12 + me->previewSize);

		UOSInt scnW;
		UOSInt scnH;
		me->GetSizeP(&scnW, &scnH);
		me->SetVScrollBar(0, totalHeight, (UOSInt)Double2OSInt(UOSInt2Double(scnH) / me->GetHDPI() * me->GetDDPI()));
		me->imgUpdated = true;
	}

	if (me->imgUpdated)
	{
		me->imgUpdated = false;
		me->Redraw();
	}
}

void SSWR::AVIRead::AVIRImageControl::InitDir()
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *sptr2End;
	UTF8Char *sptr3;
	UTF8Char *sarr[6];
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	Text::StringBuilderUTF8 sb;

	sptr = this->folderPath->ConcatTo(sbuff);
	sptr2 = this->folderPath->ConcatTo(sbuff2);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
		*sptr2++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr2, UTF8STRC("Cache"));
	IO::Path::CreateDirectory(sbuff2);
	*sptr2++ = IO::Path::PATH_SEPERATOR;

	UOSInt i;
	UOSInt colCnt;
	Data::ICaseStringUTF8Map<ImageSetting*> *imgSettMap;
	Data::ArrayList<ImageSetting*> *imgSettList;
	ImageSetting *imgSett;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	Parser::ParserList *parsers;
	Text::UTF8Reader *reader;
	IO::FileStream *fs;
	IO::StmData::FileData *fd;
	NEW_CLASS(imgSettMap, Data::ICaseStringUTF8Map<ImageSetting*>());
	sptr3 = Text::StrConcatC(sptr, UTF8STRC("Setting.txt"));
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr3), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		while (reader->ReadLine(&sb, 4096))
		{
			sptr3 = sb.ToString();
			colCnt = Text::StrSplit(sarr, 11, sptr3, '\t');
			if (colCnt == 5)
			{
				imgSett = MemAlloc(ImageSetting, 1);
				imgSett->flags = Text::StrToInt32(sarr[1]);
				imgSett->brightness = Text::StrToDouble(sarr[2]);
				imgSett->contrast = Text::StrToDouble(sarr[3]);
				imgSett->gamma = Text::StrToDouble(sarr[4]);
				imgSett->cropEnabled = false;
				imgSett->cropLeft = 0;
				imgSett->cropTop = 0;
				imgSett->cropWidth = 0;
				imgSett->cropHeight = 0;
				imgSett = imgSettMap->Put(sarr[0], imgSett);
				if (imgSett)
					MemFree(imgSett);
			}
			else if (colCnt == 10)
			{
				imgSett = MemAlloc(ImageSetting, 1);
				imgSett->flags = Text::StrToInt32(sarr[1]);
				imgSett->brightness = Text::StrToDouble(sarr[2]);
				imgSett->contrast = Text::StrToDouble(sarr[3]);
				imgSett->gamma = Text::StrToDouble(sarr[4]);
				imgSett->cropEnabled = Text::StrToInt32(sarr[5]) != 0;
				imgSett->cropLeft = Text::StrToInt32(sarr[6]);
				imgSett->cropTop = Text::StrToInt32(sarr[7]);
				imgSett->cropWidth = Text::StrToInt32(sarr[8]);
				imgSett->cropHeight = Text::StrToInt32(sarr[9]);
				imgSett = imgSettMap->Put(sarr[0], imgSett);
				if (imgSett)
					MemFree(imgSett);
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);

	sptr3 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr3 - sbuff));
	if (sess)
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
		
		Media::Resizer::LanczosResizer8_C8 resizer(4, 3, &srcProfile, &destProfile, this->colorSess, Media::AT_NO_ALPHA);
		Exporter::GUIPNGExporter exporter;
		resizer.SetTargetWidth(this->previewSize);
		resizer.SetTargetHeight(this->previewSize);
		parsers = this->core->GetParserList();
		UOSInt currCnt = 0;
		while (this->threadCtrlCode != 2 && this->threadCtrlCode != 3 && (sptr3 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				Sync::MutexUsage mutUsage(this->ioMut);
				NEW_CLASS(fd, IO::StmData::FileData(CSTRP(sbuff, sptr3), false));
				Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParserType::ImageList);
				DEL_CLASS(fd);
				mutUsage.EndUse();
				if (imgList)
				{
					Media::StaticImage *simg;
					Media::Image *img;
					imgList->ToStaticImage(0);
					img = imgList->GetImage(0, 0);
					simg = img->CreateStaticImage();
					DEL_CLASS(imgList);
					if (simg)
					{
						Media::StaticImage *simg2;
						sptr2End = Text::StrConcatC(Text::StrConcatC(sptr2, sptr, (UOSInt)(sptr3 - sptr)), UTF8STRC(".png"));
						simg->To32bpp();
						simg2 = resizer.ProcessToNew(simg);
						NEW_CLASS(imgList, Media::ImageList(CSTRP(sptr2, sptr2End)));
						imgList->AddImage(simg2, 0);
						mutUsage.BeginUse();
						NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff2, sptr2End), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
						exporter.ExportFile(fs, sbuff2, imgList, 0);
						DEL_CLASS(fs);
						mutUsage.EndUse();
						DEL_CLASS(imgList);
						DEL_CLASS(simg);

						status = MemAlloc(SSWR::AVIRead::AVIRImageControl::ImageStatus, 1);
						status->filePath = Text::String::NewNotNull(sbuff);
						status->cacheFile = Text::String::NewNotNull(sbuff2);
						status->fileName = status->filePath->v + (sptr - sbuff);
						imgSett = imgSettMap->Get(sptr);
						if (imgSett)
						{
							MemCopyNO(&status->setting, imgSett, sizeof(ImageSetting));
						}
						else
						{
							status->setting.brightness = 1.0;
							status->setting.contrast = 1.0;
							status->setting.gamma = 1.0;
							status->setting.flags = 0;
							status->setting.cropEnabled = false;
							status->setting.cropLeft = 0;
							status->setting.cropTop = 0;
							status->setting.cropWidth = 0;
							status->setting.cropHeight = 0;
						}
						status->previewImg = 0;
						status->previewImg2 = 0;
						Sync::MutexUsage imgMutUsage(this->imgMut);
						this->imgMap->Put(status->fileName, status);
						this->imgMapUpdated = true;
						imgMutUsage.EndUse();
					}
				}
				currCnt++;
				if (this->progHdlr)
				{
					this->progHdlr(this->progHdlrObj, currCnt);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	imgSettList = imgSettMap->GetValues();
	i = imgSettList->GetCount();
	while (i-- > 0)
	{
		imgSett = imgSettList->GetItem(i);
		MemFree(imgSett);
	}
	DEL_CLASS(imgSettMap);
}

void SSWR::AVIRead::AVIRImageControl::ExportQueued()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	if (this->folderPath == 0)
		return;

	sptr = this->folderPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("Output"));
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;

	ImageStatus *status;
	Media::StaticImage *img;
	Media::ImageList *imgList;
	Exporter::TIFFExporter tifExporter;
	Exporter::GUIJPGExporter jpgExporter;
	IO::FileStream *fs;
	while (this->threadCtrlCode != 2 && this->threadCtrlCode != 3)
	{
		Sync::MutexUsage mutUsage(this->exportMut);
		status = (ImageStatus*)this->exportList->Get();
		mutUsage.EndUse();
		if (status == 0)
			break;

		img = this->LoadImage(status->fileName);
		if (img)
		{
			this->ApplySetting(img, img, &status->setting);
			sptr2 = Text::StrConcat(sptr, status->fileName);
			NEW_CLASS(imgList, Media::ImageList(CSTRP(sbuff, sptr2)));
			imgList->AddImage(img, 0);
			Sync::MutexUsage ioMutUsage(this->ioMut);
			if (this->exportFmt == EF_JPG)
			{
				img->To32bpp();
				sptr2 = IO::Path::ReplaceExt(sptr, (const UTF8Char*)"jpg");
				NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
				void *param = jpgExporter.CreateParam(imgList);
				if (param)
				{
					jpgExporter.SetParamInt32(param, 0, 100);
				}
				jpgExporter.ExportFile(fs, sbuff, imgList, param);
				if (param)
				{
					jpgExporter.DeleteParam(param);
				}
				DEL_CLASS(fs);
			}
			else if (this->exportFmt == EF_TIF)
			{
				sptr2 = IO::Path::ReplaceExt(sptr, (const UTF8Char*)"tif");
				NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
				tifExporter.ExportFile(fs, sbuff, imgList, 0);
				DEL_CLASS(fs);
			}
			ioMutUsage.EndUse();
			DEL_CLASS(imgList);
		}
		MemFree(status);

		UOSInt currCnt;
		mutUsage.BeginUse();
		this->exportCurrCnt++;
		currCnt = this->exportCurrCnt;
		mutUsage.EndUse();
		if (this->progHdlr)
		{
			this->progHdlr(this->progHdlrObj, currCnt);
		}
	}
}

void SSWR::AVIRead::AVIRImageControl::ThreadCancelTasks()
{
	this->threadCtrlCode = 3;
	this->folderThreadEvt->Set();
	while (this->threadState != 0 && this->threadState != 1)
	{
		this->folderCtrlEvt->Wait();
	}
}

void SSWR::AVIRead::AVIRImageControl::EndFolder()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	UOSInt i;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
	if (this->folderPath == 0)
		return;
	sptr = this->folderPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("Cache"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.png"));
	sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr2 - sbuff));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PathType::File)
			{
				IO::Path::DeleteFile(sbuff);
			}
		}
		IO::Path::FindFileClose(sess);
	}
	sptr[-1] = 0;
	IO::Path::RemoveDirectory(sbuff);
	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap->GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		status->filePath->Release();
		status->cacheFile->Release();
		if (status->previewImg)
		{
			this->deng->DeleteImage(status->previewImg);
		}
		if (status->previewImg2)
		{
			this->deng->DeleteImage(status->previewImg2);
		}
		MemFree(status);
	}
	Sync::MutexUsage expMutUsage(this->exportMut);
	while (true)
	{
		status = (SSWR::AVIRead::AVIRImageControl::ImageStatus*)this->exportList->Get();
		if (status == 0)
			break;
		MemFree(status);
	}
	expMutUsage.EndUse();
	this->imgMap->Clear();
	this->imgMapUpdated = true;
	mutUsage.EndUse();
}

Bool SSWR::AVIRead::AVIRImageControl::GetCameraName(Text::StringBuilderUTF8 *sb, Media::EXIFData *exif)
{
	if (exif == 0)
	{
		return false;
	}
	const Char *make = exif->GetPhotoMake();
	const Char *model = exif->GetPhotoModel();
	if (make && model)
	{
		if (Text::StrStartsWith(model, make))
		{
			sb->AppendSlow((const UTF8Char*)model);
		}
		else
		{
			sb->AppendSlow((const UTF8Char*)make);
			sb->AppendC(UTF8STRC(" "));
			sb->AppendSlow((const UTF8Char*)model);
		}
	}
	else if (make)
	{
		sb->AppendSlow((const UTF8Char*)make);
	}
	else if (model)
	{
		sb->AppendSlow((const UTF8Char*)model);
	}
	else
	{
		return false;
	}
	return true;
}

Double *SSWR::AVIRead::AVIRImageControl::GetCameraGamma(Text::CString cameraName, UInt32 *gammaCnt)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRImageControl::CameraCorr *camera;
	Sync::MutexUsage mutUsage(this->cameraMut);
	camera = this->cameraMap->Get(cameraName);
	if (camera)
	{
		mutUsage.EndUse();
		*gammaCnt = camera->gammaCnt;
		return camera->gammaParam;
	}
	camera = MemAlloc(SSWR::AVIRead::AVIRImageControl::CameraCorr, 1);
	camera->gammaCnt = 0;
	camera->gammaParam = 0;
	this->cameraMap->Put(cameraName, camera);

	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, cameraName.v, cameraName.leng);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".gamma"));
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Double> gammaVals;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (true)
	{
		Double val;
		sb.ClearStr();
		if (!reader->ReadLine(&sb, 512))
		{
			break;
		}
		if (Text::StrToDouble(sb.ToString(), &val))
		{
			gammaVals.Add(val);
		}
	}
	if (gammaVals.GetCount() > 0)
	{
		UOSInt i;
		camera->gammaCnt = (UInt32)gammaVals.GetCount();
		camera->gammaParam = MemAlloc(Double, camera->gammaCnt);
		MemCopyNO(camera->gammaParam, gammaVals.GetArray(&i), sizeof(Double) * camera->gammaCnt);
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);

	mutUsage.EndUse();
	*gammaCnt = camera->gammaCnt;
	return camera->gammaParam;
}

SSWR::AVIRead::AVIRImageControl::AVIRImageControl(UI::GUICore *ui, UI::GUIClientControl *parent, SSWR::AVIRead::AVIRCore *core, UI::GUIForm *frm, Media::ColorManagerSess *colorSess) : UI::GUICustomDrawVScroll(ui, parent, core->GetDrawEngine())
{
	this->core = core;
	this->folderPath = 0;
	this->colorSess = colorSess;
	this->colorSess->AddHandler(this);
	this->exportCurrCnt = 0;
	this->exportFmt = EF_JPG;
	this->keyHdlr = 0;
	this->keyObj = 0;
	NEW_CLASS(this->ioMut, Sync::Mutex());
	NEW_CLASS(this->folderMut, Sync::Mutex());
	NEW_CLASS(this->folderThreadEvt, Sync::Event(true));
	NEW_CLASS(this->folderCtrlEvt, Sync::Event(true));
	NEW_CLASS(this->imgMap, Data::ICaseStringUTF8Map<SSWR::AVIRead::AVIRImageControl::ImageStatus*>());
	NEW_CLASS(this->imgMut, Sync::Mutex());
	NEW_CLASS(this->filterMut, Sync::Mutex());
	NEW_CLASS(this->filter, Media::RGBColorFilter(this->core->GetColorMgr()));
	NEW_CLASS(this->exportList, Data::SyncLinkedList());
	NEW_CLASS(this->exportMut, Sync::Mutex());
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destColor(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->dispResizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srcColor, &destColor, colorSess, Media::AT_NO_ALPHA));
	NEW_CLASS(this->cameraMut, Sync::Mutex());
	NEW_CLASS(this->cameraMap, Data::StringMap<SSWR::AVIRead::AVIRImageControl::CameraCorr*>());
	this->imgMapUpdated = true;
	this->imgUpdated = false;
	this->previewSize = 160;
	this->currSel = 0;
	this->dispImg = 0;
	this->dispImgChg = true;
	this->progHdlr = 0;
	this->progHdlrObj = 0;

	this->threadState = 0;
	this->threadCtrlCode = 0;
	Sync::Thread::Create(FolderThread, this);
	while (this->threadState == 0)
		this->folderCtrlEvt->Wait();
	
	frm->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRImageControl::~AVIRImageControl()
{
	SetFolder(0);
	this->threadCtrlCode = 2;
	this->folderThreadEvt->Set();
	while (this->threadState != 0)
	{
		this->folderCtrlEvt->Wait(10);
	}
	UOSInt i;
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::CameraCorr *> *cameraList = this->cameraMap->GetValues();
	CameraCorr *camera;
	i = cameraList->GetCount();
	while (i-- > 0)
	{
		camera = cameraList->GetItem(i);
		if (camera->gammaParam)
		{
			MemFree(camera->gammaParam);
		}
		MemFree(camera);
	}
	DEL_CLASS(this->cameraMap);
	DEL_CLASS(this->cameraMut);

	DEL_CLASS(this->dispResizer);
	DEL_CLASS(this->exportList);
	DEL_CLASS(this->exportMut);
	DEL_CLASS(this->filter);
	DEL_CLASS(this->filterMut);
	DEL_CLASS(this->folderMut);
	DEL_CLASS(this->folderThreadEvt);
	DEL_CLASS(this->folderCtrlEvt);
	DEL_CLASS(this->ioMut);
	DEL_CLASS(this->imgMap);
	DEL_CLASS(this->imgMut);
	this->colorSess->RemoveHandler(this);
}

Text::CString SSWR::AVIRead::AVIRImageControl::GetObjectClass()
{
	return CSTR("AVIRImageControl");
}

OSInt SSWR::AVIRead::AVIRImageControl::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void SSWR::AVIRead::AVIRImageControl::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void SSWR::AVIRead::AVIRImageControl::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus *> *imgList;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	UOSInt i;

	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap->GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		if (status->previewImg2)
		{
			this->deng->DeleteImage(status->previewImg2);
			status->previewImg2 = 0;
		}
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRImageControl::SetDPI(Double hdpi, Double ddpi)
{
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus *> *imgList;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	UOSInt i;

	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}

	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap->GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		if (status->previewImg2)
		{
			this->deng->DeleteImage(status->previewImg2);
			status->previewImg2 = 0;
		}
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRImageControl::OnDraw(Media::DrawImage *dimg)
{
	Media::DrawBrush *b;
	Media::DrawFont *f;
	Media::DrawBrush *barr[5];
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	UOSInt i;
	UOSInt j;
	UOSInt scnW = dimg->GetWidth();
	UOSInt scnH = dimg->GetHeight();

	barr[0] = dimg->NewBrushARGB(0xffc0c0c0);
	barr[1] = dimg->NewBrushARGB(0xffff8080);
	barr[2] = dimg->NewBrushARGB(0xff8080ff);
	barr[3] = dimg->NewBrushARGB(0xffcc80cc);

	barr[4] = dimg->NewBrushARGB(0xff000000);

	Double hdpi = this->GetHDPI();
	Double ddpi = this->GetDDPI();
	UInt32 itemTH = (UInt32)Double2Int32((20 + 12 + 12 + this->previewSize) * hdpi / ddpi);
	UInt32 itemBH = (UInt32)Double2Int32((20 + 12 + this->previewSize) * hdpi / ddpi);
	UInt32 itemH = (UInt32)Double2Int32((20 + this->previewSize) * hdpi / ddpi);
	UInt32 scrPos = (UInt32)Double2Int32(UOSInt2Double(this->GetVScrollPos()) * hdpi / ddpi);

	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap->GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		status->setting.flags &= ~8;
	}
	i = this->GetVScrollPos() / (20 + 12 + 12 + this->previewSize);
	j = (UInt32)Double2Int32(UOSInt2Double(this->GetVScrollPos()) + UOSInt2Double(scnH) * ddpi / hdpi) / (20 + 12 + 12 + this->previewSize);

	if (imgList->GetCount() > 0)
	{
		if (j >= imgList->GetCount())
		{
			j = imgList->GetCount() - 1;
		}
		f = dimg->NewFontPt(UTF8STRC("Arial"), 9, Media::DrawEngine::DFS_ANTIALIAS, 0);
		b = dimg->NewBrushARGB(0xff000000);
		UOSInt strLen;
		Double strSz[2];
		while (i <= j)
		{
			status = imgList->GetItem(i);
			status->setting.flags |= 8;
			if (status->previewImg == 0)
			{
				status->previewImg = this->deng->LoadImage(status->cacheFile->ToCString());
				if (status->previewImg)
				{
					status->previewImg2 = this->deng->CreateImage32((UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetWidth()) * hdpi / ddpi), (UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetHeight()) * hdpi / ddpi), Media::AT_NO_ALPHA);
					this->UpdateImgPreview(status);
				}
			}
			else if (status->previewImg2 == 0)
			{
				status->previewImg2 = this->deng->CreateImage32((UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetWidth()) * hdpi / ddpi), (UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetHeight()) * hdpi / ddpi), Media::AT_NO_ALPHA);
				this->UpdateImgPreview(status);
			}
			dimg->DrawRect(0, OSInt2Double((OSInt)(i * itemTH - scrPos)), UOSInt2Double(scnW), itemBH, 0, barr[status->setting.flags & 3]);
			dimg->DrawRect(0, OSInt2Double((OSInt)(i * itemTH - scrPos + itemBH)), UOSInt2Double(scnW), itemTH - itemBH, 0, barr[4]);
			if (status->previewImg2)
			{
				status->previewImg2->SetHDPI(dimg->GetHDPI());
				status->previewImg2->SetVDPI(dimg->GetVDPI());
				dimg->DrawImagePt(status->previewImg2, UOSInt2Double((scnW - status->previewImg2->GetWidth()) >> 1), OSInt2Double((OSInt)(i * itemTH - scrPos + ((itemH - status->previewImg2->GetHeight()) >> 1))));
			}
			if (status->fileName)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendSlow(status->fileName);
				strLen = sb.GetLength();
				if (f && dimg->GetTextSizeC(f, sb.ToString(), strLen, strSz))
				{
					dimg->DrawString((UOSInt2Double(scnW) - strSz[0]) * 0.5, UOSInt2Double(i * itemTH - scrPos + itemH), sb.ToString(), f, b);
				}
			}
			i++;
		}
		if ((j + 1) * itemTH - scrPos < scnH)
		{
			dimg->DrawRect(0, UOSInt2Double((j + 1) * itemTH - scrPos), UOSInt2Double(scnW), UOSInt2Double(scnH) - UOSInt2Double((j + 1) * itemTH - scrPos), 0, barr[4]);
		}
		dimg->DelBrush(b);
		dimg->DelFont(f);
	}

	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		if ((status->setting.flags & 8) == 0)
		{
			if (status->previewImg != 0)
			{
				this->deng->DeleteImage(status->previewImg);
				status->previewImg = 0;
			}
			if (status->previewImg2 != 0)
			{
				this->deng->DeleteImage(status->previewImg2);
				status->previewImg2 = 0;
			}
		}
	}
	mutUsage.EndUse();

	dimg->DelBrush(barr[4]);
	dimg->DelBrush(barr[3]);
	dimg->DelBrush(barr[2]);
	dimg->DelBrush(barr[1]);
	dimg->DelBrush(barr[0]);
}

void SSWR::AVIRead::AVIRImageControl::OnMouseDown(OSInt scrollY, Int32 xPos, Int32 yPos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	if (btn == UI::GUIClientControl::MBTN_LEFT)
	{
		UOSInt i;
		UOSInt j;
		Double hdpi = this->GetHDPI();
		Double ddpi = this->GetDDPI();
		Int32 scrPos = Double2Int32(OSInt2Double(scrollY) * hdpi / ddpi);
		UOSInt clickIndex = (UInt32)((scrPos + yPos) / Double2Int32((20 + 12 + 12 + this->previewSize) * hdpi / ddpi));
		if (keys & UI::GUICustomDrawVScroll::KBTN_CONTROL)
		{
			Sync::MutexUsage mutUsage(this->imgMut);
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status = this->imgMap->GetValues()->GetItem((UOSInt)clickIndex);
			status->setting.flags ^= 1;
			mutUsage.EndUse();
			this->Redraw();
			this->currSel = clickIndex;
		}
		else if (keys & UI::GUICustomDrawVScroll::KBTN_SHIFT)
		{
			Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
			Sync::MutexUsage mutUsage(this->imgMut);
			imgList = this->imgMap->GetValues();
			i = imgList->GetCount();
			while (i-- > 0)
			{
				status = imgList->GetItem(i);
				status->setting.flags &= ~1;
			}
			if (this->currSel > clickIndex)
			{
				i = clickIndex;
				j = this->currSel;
			}
			else
			{
				i = this->currSel;
				j = clickIndex;
			}
			if (j >= imgList->GetCount())
			{
				j = imgList->GetCount() - 1;
			}
			while (i <= j)
			{
				status = imgList->GetItem(i);
				status->setting.flags |= 1;
				i++;
			}
			mutUsage.EndUse();
			this->Redraw();
		}
		else
		{
			Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
			Sync::MutexUsage mutUsage(this->imgMut);
			imgList = this->imgMap->GetValues();
			i = imgList->GetCount();
			while (i-- > 0)
			{
				status = imgList->GetItem(i);
				status->setting.flags &= ~1;
			}
			status = imgList->GetItem((UOSInt)clickIndex);
			if (status)
			{
				status->setting.flags |= 1;
				this->currSel = clickIndex;
			}
			mutUsage.EndUse();
			this->Redraw();
			if (this->dispImg != status)
			{
				this->dispImg = status;
				if (this->dispHdlr)
				{
					if (status)
					{
						this->dispHdlr(this->dispHdlrObj, status->fileName, &status->setting);
					}
					else
					{
						this->dispHdlr(this->dispHdlrObj, 0, 0);
					}
				}
			}
		}
	}
}

void SSWR::AVIRead::AVIRImageControl::OnKeyDown(UInt32 keyCode)
{
	UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(keyCode);
	if (key == UI::GUIControl::GK_UP)
	{
		this->MoveUp();
	}
	else if (key == UI::GUIControl::GK_DOWN)
	{
		this->MoveDown();
	}
	else if (this->keyHdlr)
	{
		this->keyHdlr(this->keyObj, key);
	}
}

void SSWR::AVIRead::AVIRImageControl::SetFolder(const UTF8Char *folderPath)
{
	Bool loading = this->IsLoadingDir();
	ThreadCancelTasks();
	if (this->folderPath)
	{
		if (!loading)
		{
			this->SaveSetting();
		}
		Sync::MutexUsage mutUsage(this->folderMut);
		this->EndFolder();
		this->folderPath->Release();
		this->folderPath = 0;
		mutUsage.EndUse();
	}
	if (folderPath)
	{
		Sync::MutexUsage mutUsage(this->folderMut);
		this->folderPath = Text::String::NewNotNull(folderPath);
		this->folderChanged = true;
		mutUsage.EndUse();

		this->threadCtrlCode = 1;
		this->folderThreadEvt->Set();
		while (this->threadCtrlCode == 1 && this->threadState != 0)
		{
			this->folderCtrlEvt->Wait();
		}
	}
	this->currSel = 0;
	this->dispImg = 0;
}

Text::String *SSWR::AVIRead::AVIRImageControl::GetFolder()
{
	return this->folderPath;
}

Bool SSWR::AVIRead::AVIRImageControl::SaveSetting()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
	UOSInt i;
	UOSInt j;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	if (this->folderPath == 0)
	{
		return false;
	}

	Text::StringBuilderUTF8 sb;
	sptr = this->folderPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("Setting.txt"));

	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	writer->WriteSignature();
	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap->GetValues();
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		status = imgList->GetItem(i);
		sb.ClearStr();
		sb.AppendSlow(status->fileName);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendI32(status->setting.flags);
		sb.AppendC(UTF8STRC("\t"));
		Text::SBAppendF64(&sb, status->setting.brightness);
		sb.AppendC(UTF8STRC("\t"));
		Text::SBAppendF64(&sb, status->setting.contrast);
		sb.AppendC(UTF8STRC("\t"));
		Text::SBAppendF64(&sb, status->setting.gamma);
		writer->WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	mutUsage.EndUse();
	DEL_CLASS(writer);
	DEL_CLASS(fs);
	return true;
}

void SSWR::AVIRead::AVIRImageControl::SetDispImageHandler(DispImageChanged hdlr, void *userObj)
{
	this->dispHdlr = hdlr;
	this->dispHdlrObj = userObj;
}

void SSWR::AVIRead::AVIRImageControl::SetProgressHandler(ProgressUpdated hdlr, void *userObj)
{
	this->progHdlr = hdlr;
	this->progHdlrObj = userObj;
}

Media::StaticImage *SSWR::AVIRead::AVIRImageControl::LoadImage(const UTF8Char *fileName)
{
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	Media::StaticImage *outImg = 0;
	Media::ImageList *imgList = 0;
	IO::StmData::FileData *fd;

	Sync::MutexUsage mutUsage(this->imgMut);
	status = this->imgMap->Get(fileName);
	if (status)
	{
		Sync::MutexUsage ioMutUsage(this->ioMut);
		NEW_CLASS(fd, IO::StmData::FileData(status->filePath, false));
		imgList = (Media::ImageList*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
		DEL_CLASS(fd);
		ioMutUsage.EndUse();
	}
	mutUsage.EndUse();

	if (imgList)
	{
		outImg = imgList->GetImage(0, 0)->CreateStaticImage();
		DEL_CLASS(imgList);
	}

	if (status->setting.cropEnabled)
	{
		//////////////////////////
	}
	return outImg;
}

Media::StaticImage *SSWR::AVIRead::AVIRImageControl::LoadOriImage(const UTF8Char *fileName)
{
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	Media::StaticImage *outImg = 0;
	Media::ImageList *imgList = 0;
	IO::StmData::FileData *fd;

	Sync::MutexUsage mutUsage(this->imgMut);
	status = this->imgMap->Get(fileName);
	if (status)
	{
		Sync::MutexUsage ioMutUsage(this->ioMut);
		NEW_CLASS(fd, IO::StmData::FileData(status->filePath, false));
		imgList = (Media::ImageList*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
		DEL_CLASS(fd);
		ioMutUsage.EndUse();
	}
	mutUsage.EndUse();

	if (imgList)
	{
		outImg = imgList->GetImage(0, 0)->CreateStaticImage();
		DEL_CLASS(imgList);
	}
	return outImg;
}

void SSWR::AVIRead::AVIRImageControl::ApplySetting(Media::StaticImage *srcImg, Media::StaticImage *destImg, SSWR::AVIRead::AVIRImageControl::ImageSetting *setting)
{
	Double *gammaParam;
	UInt32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	if (this->GetCameraName(&sb, srcImg->exif))
	{
		gammaParam = this->GetCameraGamma(sb.ToCString(), &gammaCnt);
	}
	else
	{
		gammaParam = 0;
		gammaCnt = 0;
	}

	Sync::MutexUsage mutUsage(this->filterMut);
	this->filter->SetParameter((setting->brightness - 1.0) * setting->contrast, setting->contrast, setting->gamma, srcImg->info->color, srcImg->info->storeBPP, srcImg->info->pf, (setting->flags & 240) >> 4);
	this->filter->SetGammaCorr(gammaParam, gammaCnt);
	this->filter->ProcessImage(srcImg->data, destImg->data, srcImg->info->dispWidth, srcImg->info->dispHeight, (srcImg->info->storeWidth * (srcImg->info->storeBPP >> 3)), (destImg->info->storeWidth * (srcImg->info->storeBPP >> 3)), false);
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRImageControl::UpdateImgPreview(SSWR::AVIRead::AVIRImageControl::ImageStatus *img)
{
	Media::DrawImage *srcImg = img->previewImg;
	Media::DrawImage *destImg = img->previewImg2;
	UOSInt sWidth = srcImg->GetWidth();
	UOSInt sHeight = srcImg->GetHeight();
	UOSInt sbpl = srcImg->GetImgBpl();
	Bool srev;
	UInt8 *sptr = srcImg->GetImgBits(&srev);
	UOSInt dWidth = destImg->GetWidth();
	UOSInt dHeight = destImg->GetHeight();
	UOSInt dbpl = destImg->GetImgBpl();
	Bool drev;
	UInt8 *dptr = destImg->GetImgBits(&drev);

	UInt8 *tmpBuff = MemAllocA(UInt8, sHeight * (UOSInt)sbpl);

	Double *gammaParam;
	UInt32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	Media::EXIFData *exif = srcImg->GetEXIF();
	if (exif && this->GetCameraName(&sb, exif))
	{
		gammaParam = this->GetCameraGamma(sb.ToCString(), &gammaCnt);
	}
	else
	{
		gammaParam = 0;
		gammaCnt = 0;
	}
	Sync::MutexUsage mutUsage(this->filterMut);
	this->filter->SetParameter((img->setting.brightness - 1.0) * img->setting.contrast, img->setting.contrast, img->setting.gamma, srcImg->GetColorProfile(), srcImg->GetBitCount(), srcImg->GetPixelFormat(), (img->setting.flags & 240) >> 4);
	this->filter->SetGammaCorr(gammaParam, gammaCnt);
	this->filter->ProcessImage(sptr, tmpBuff, sWidth, sHeight, sbpl, sbpl, srev ^ drev);
	this->dispResizer->Resize(tmpBuff, (OSInt)sbpl, UOSInt2Double(sWidth), UOSInt2Double(sHeight), 0, 0, dptr, (OSInt)dbpl, dWidth, dHeight);
	mutUsage.EndUse();
	MemFreeA(tmpBuff);
	srcImg->GetImgBitsEnd(false);
	destImg->GetImgBitsEnd(true);
}

void SSWR::AVIRead::AVIRImageControl::UpdateImgSetting(SSWR::AVIRead::AVIRImageControl::ImageSetting *setting)
{
	Data::ArrayList<ImageStatus*> *imgList;
	ImageStatus *status;
	UOSInt i;
	Bool chg = false;
	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap->GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		if (status->setting.flags & 1)
		{
			if (status->setting.brightness != setting->brightness || status->setting.contrast != setting->contrast || status->setting.gamma != setting->gamma || (status->setting.flags & 240) != setting->flags)
			{
				status->setting.brightness = setting->brightness;
				status->setting.contrast = setting->contrast;
				status->setting.gamma = setting->gamma;
				status->setting.flags = (status->setting.flags & ~240) | setting->flags;
				chg = true;
				if (status->previewImg && status->previewImg2)
				{
					UpdateImgPreview(status);
				}
			}
		}
	}
	mutUsage.EndUse();
	if (chg)
	{
		this->imgUpdated = true;
	}
}

Bool SSWR::AVIRead::AVIRImageControl::IsLoadingDir()
{
	return this->threadState == 2;
}

void SSWR::AVIRead::AVIRImageControl::SetExportFormat(ExportFormat fmt)
{
	this->exportFmt = fmt;
}

UOSInt SSWR::AVIRead::AVIRImageControl::ExportSelected()
{
	ImageStatus *status;
	ImageStatus *status2;
	Data::ArrayList<ImageStatus*> *imgList;
	UOSInt i;
	UOSInt j;
	UOSInt cnt = 0;
	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap->GetValues();
	j = imgList->GetCount();
	i = 0;
	while (i < j)
	{
		status = imgList->GetItem(i);
		if (status->setting.flags & 1)
		{
			status2 = MemAlloc(ImageStatus, 1);
			MemCopyNO(status2, status, sizeof(ImageStatus));
			Sync::MutexUsage exportMutUsage(this->exportMut);
			this->exportList->Put(status2);
			exportMutUsage.EndUse();
			cnt++;
		}
		i++;
	}
	mutUsage.EndUse();
	if (cnt > 0)
	{
		Sync::MutexUsage exportMutUsage(this->exportMut);
		this->exportCurrCnt = 0;
		cnt = this->exportList->GetCount();
		exportMutUsage.EndUse();
		this->folderThreadEvt->Set();
	}
	return cnt;
}

void SSWR::AVIRead::AVIRImageControl::MoveUp()
{
	Data::SortableArrayList<const UTF8Char *> *nameList;
	Data::ArrayList<ImageStatus *> *imgList;
	ImageStatus *status;
	OSInt i;
	UOSInt j;
	if (this->folderPath == 0)
		return;

	Sync::MutexUsage mutUsage(this->imgMut);
	nameList = this->imgMap->GetKeys();
	imgList = this->imgMap->GetValues();
	if (this->dispImg)
	{
		i = nameList->SortedIndexOf(this->dispImg->fileName);
		if (i == -1)
		{
			i = (OSInt)nameList->GetCount() - 1;
		}
		else if (i == 0)
		{
		}
		else
		{
			i = i - 1;
		}
	}
	else
	{
		i = (OSInt)nameList->GetCount() - 1;
	}
	this->currSel = (UOSInt)i;
	j = imgList->GetCount();
	while (j-- > 0)
	{
		status = imgList->GetItem(j);
		status->setting.flags &= ~1;
	}

	if (i < 0)
	{
		this->dispImg = 0;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, 0, 0);
		}
	}
	else
	{
		this->dispImg = this->imgMap->GetValues()->GetItem((UOSInt)i);
		this->dispImg->setting.flags |= 1;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, this->dispImg->fileName, &this->dispImg->setting);
		}
	}
	mutUsage.EndUse();
	this->imgUpdated = true;
	if (i >= 0)
	{
		this->MakeVisible((UOSInt)i * (20 + 12 + 12 + this->previewSize));
	}
}

void SSWR::AVIRead::AVIRImageControl::MoveDown()
{
	Data::SortableArrayList<const UTF8Char *> *nameList;
	Data::ArrayList<ImageStatus *> *imgList;
	ImageStatus *status;
	OSInt i;
	UOSInt j;
	if (this->folderPath == 0)
		return;

	Sync::MutexUsage mutUsage(this->imgMut);
	nameList = this->imgMap->GetKeys();
	imgList = this->imgMap->GetValues();
	if (this->dispImg)
	{
		i = nameList->SortedIndexOf(this->dispImg->fileName);
		if (i == -1)
		{
			i = 0;
		}
		else if ((UOSInt)i >= nameList->GetCount() - 1)
		{
		}
		else
		{
			i = i + 1;
		}
	}
	else
	{
		i = 0;
	}
	this->currSel = (UOSInt)i;
	j = imgList->GetCount();
	while (j-- > 0)
	{
		status = imgList->GetItem(j);
		status->setting.flags &= ~1;
	}

	if (i < 0)
	{
		this->dispImg = 0;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, 0, 0);
		}
	}
	else
	{
		this->dispImg = this->imgMap->GetValues()->GetItem((UOSInt)i);
		this->dispImg->setting.flags |= 1;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, this->dispImg->fileName, &this->dispImg->setting);
		}
	}
	mutUsage.EndUse();
	this->imgUpdated = true;
	if (i >= 0)
	{
		this->MakeVisible((UOSInt)(i - 1) * (20 + 12 + 12 + this->previewSize));
	}
}

void SSWR::AVIRead::AVIRImageControl::SelectAll()
{
	Sync::MutexUsage mutUsage(this->imgMut);
	Data::ArrayList<ImageStatus*> *imgList = this->imgMap->GetValues();
	UOSInt i = imgList->GetCount();
	ImageStatus *status;
	while (i-- > 0)
	{
		status = imgList->GetItem(i);;
		status->setting.flags |= 1;
	}
	mutUsage.EndUse();
	this->Redraw();
}

void SSWR::AVIRead::AVIRImageControl::HandleKeyDown(KeyDownHandler keyHdlr, void *keyObj)
{
	this->keyObj = keyObj;
	this->keyHdlr = keyHdlr;
}
