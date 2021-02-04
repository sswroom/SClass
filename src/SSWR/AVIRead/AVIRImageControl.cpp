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
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "UI/GUIClientControl.h"

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
		OSInt imgCnt;
		OSInt totalHeight;
		me->imgMapUpdated = false;
		me->imgMut->Lock();
		imgCnt = me->imgMap->GetCount();
		me->imgMut->Unlock();
		totalHeight = imgCnt * (20 + 12 + 12 + me->previewSize);

		UOSInt scnW;
		UOSInt scnH;
		me->GetSizeP(&scnW, &scnH);
		me->SetVScrollBar(0, totalHeight, Math::Double2Int32(scnH / me->GetHDPI() * me->GetDDPI()));
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
	UTF8Char *sptr3;
	UTF8Char *sarr[6];
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	Text::StringBuilderUTF8 sb;

	sptr = Text::StrConcat(sbuff, this->folderPath);
	sptr2 = Text::StrConcat(sbuff2, this->folderPath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
		*sptr2++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"Cache");
	IO::Path::CreateDirectory(sbuff2);
	*sptr2++ = IO::Path::PATH_SEPERATOR;

	OSInt i;
	OSInt colCnt;
	Data::ICaseStringUTF8Map<ImageSetting*> *imgSettMap;
	Data::ArrayList<ImageSetting*> *imgSettList;
	ImageSetting *imgSett;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	Parser::ParserList *parsers;
	Text::UTF8Reader *reader;
	IO::FileStream *fs;
	IO::StmData::FileData *fd;
	NEW_CLASS(imgSettMap, Data::ICaseStringUTF8Map<ImageSetting*>());
	Text::StrConcat(sptr, (const UTF8Char*)"Setting.txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
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

	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
		
		Media::Resizer::LanczosResizer8_C8 resizer(4, 3, &srcProfile, &destProfile, this->colorSess, Media::AT_NO_ALPHA);
		Exporter::GUIPNGExporter exporter;
		resizer.SetTargetWidth(this->previewSize);
		resizer.SetTargetHeight(this->previewSize);
		parsers = this->core->GetParserList();
		OSInt currCnt = 0;
		while (this->threadCtrlCode != 2 && this->threadCtrlCode != 3 && IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PT_FILE)
			{
				this->ioMut->Lock();
				NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
				Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
				DEL_CLASS(fd);
				this->ioMut->Unlock();
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
						Text::StrConcat(Text::StrConcat(sptr2, sptr), (const UTF8Char*)".png");
						simg->To32bpp();
						simg2 = resizer.ProcessToNew(simg);
						NEW_CLASS(imgList, Media::ImageList(sptr));
						imgList->AddImage(simg2, 0);
						this->ioMut->Lock();
						NEW_CLASS(fs, IO::FileStream(sbuff2, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
						exporter.ExportFile(fs, sbuff2, imgList, 0);
						DEL_CLASS(fs);
						this->ioMut->Unlock();
						DEL_CLASS(imgList);
						DEL_CLASS(simg);

						status = MemAlloc(SSWR::AVIRead::AVIRImageControl::ImageStatus, 1);
						status->filePath = Text::StrCopyNew(sbuff);
						status->cacheFile = Text::StrCopyNew(sbuff2);
						status->fileName = status->filePath + (sptr - sbuff);
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
						this->imgMut->Lock();
						this->imgMap->Put(status->fileName, status);
						this->imgMapUpdated = true;
						this->imgMut->Unlock();
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
	if (this->folderPath == 0)
		return;

	sptr = Text::StrConcat(sbuff, this->folderPath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"Output");
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
		this->exportMut->Lock();
		status = (ImageStatus*)this->exportList->Get();
		this->exportMut->Unlock();
		if (status == 0)
			break;

		img = this->LoadImage(status->fileName);
		if (img)
		{
			this->ApplySetting(img, img, &status->setting);
			Text::StrConcat(sptr, status->fileName);
			NEW_CLASS(imgList, Media::ImageList(sbuff));
			imgList->AddImage(img, 0);
			this->ioMut->Lock();
			if (this->exportFmt == EF_JPG)
			{
				img->To32bpp();
				IO::Path::ReplaceExt(sptr, (const UTF8Char*)"jpg");
				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
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
				IO::Path::ReplaceExt(sptr, (const UTF8Char*)"tif");
				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
				tifExporter.ExportFile(fs, sbuff, imgList, 0);
				DEL_CLASS(fs);
			}
			this->ioMut->Unlock();
			DEL_CLASS(imgList);
		}
		MemFree(status);

		OSInt currCnt;
		this->exportMut->Lock();
		this->exportCurrCnt++;
		currCnt = this->exportCurrCnt;
		this->exportMut->Unlock();
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
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	OSInt i;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
	if (this->folderPath == 0)
		return;
	sptr = Text::StrConcat(sbuff, this->folderPath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"Cache");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, (const UTF8Char*)"*.png");
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PT_FILE)
			{
				IO::Path::DeleteFile(sbuff);
			}
		}
		IO::Path::FindFileClose(sess);
	}
	sptr[-1] = 0;
	IO::Path::RemoveDirectory(sbuff);
	this->imgMut->Lock();
	imgList = this->imgMap->GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		Text::StrDelNew(status->filePath);
		Text::StrDelNew(status->cacheFile);
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
	this->exportMut->Lock();
	while (true)
	{
		status = (SSWR::AVIRead::AVIRImageControl::ImageStatus*)this->exportList->Get();
		if (status == 0)
			break;
		MemFree(status);
	}
	this->exportMut->Unlock();
	this->imgMap->Clear();
	this->imgMapUpdated = true;
	this->imgMut->Unlock();
}

Bool SSWR::AVIRead::AVIRImageControl::GetCameraName(Text::StringBuilderUTF *sb, Media::EXIFData *exif)
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
			sb->Append((const UTF8Char*)model);
		}
		else
		{
			sb->Append((const UTF8Char*)make);
			sb->Append((const UTF8Char*)" ");
			sb->Append((const UTF8Char*)model);
		}
	}
	else if (make)
	{
		sb->Append((const UTF8Char*)make);
	}
	else if (model)
	{
		sb->Append((const UTF8Char*)model);
	}
	else
	{
		return false;
	}
	return true;
}

Double *SSWR::AVIRead::AVIRImageControl::GetCameraGamma(const UTF8Char *cameraName, Int32 *gammaCnt)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRImageControl::CameraCorr *camera;
	this->cameraMut->Lock();
	camera = this->cameraMap->Get(cameraName);
	if (camera)
	{
		this->cameraMut->Unlock();
		*gammaCnt = camera->gammaCnt;
		return camera->gammaParam;
	}
	camera = MemAlloc(SSWR::AVIRead::AVIRImageControl::CameraCorr, 1);
	camera->gammaCnt = 0;
	camera->gammaParam = 0;
	this->cameraMap->Put(cameraName, camera);

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, cameraName);
	Text::StrConcat(sptr, (const UTF8Char*)".gamma");
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Double> gammaVals;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
		camera->gammaCnt = (Int32)gammaVals.GetCount();
		camera->gammaParam = MemAlloc(Double, camera->gammaCnt);
		MemCopyNO(camera->gammaParam, gammaVals.GetArray(&i), sizeof(Double) * camera->gammaCnt);
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);

	this->cameraMut->Unlock();
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
	NEW_CLASS(this->ioMut, Sync::Mutex());
	NEW_CLASS(this->folderMut, Sync::Mutex());
	NEW_CLASS(this->folderThreadEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRImageControl.folderThreadEvt"));
	NEW_CLASS(this->folderCtrlEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRImageControl.folderCtrlEvt"));
	NEW_CLASS(this->imgMap, Data::ICaseStringUTF8Map<SSWR::AVIRead::AVIRImageControl::ImageStatus*>());
	NEW_CLASS(this->imgMut, Sync::Mutex());
	NEW_CLASS(this->filterMut, Sync::Mutex());
	NEW_CLASS(this->filter, Media::RGBColorFilter(this->core->GetColorMgr()));
	NEW_CLASS(this->exportList, Data::LinkedList());
	NEW_CLASS(this->exportMut, Sync::Mutex());
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destColor(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->dispResizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srcColor, &destColor, colorSess, Media::AT_NO_ALPHA));
	NEW_CLASS(this->cameraMut, Sync::Mutex());
	NEW_CLASS(this->cameraMap, Data::StringUTF8Map<SSWR::AVIRead::AVIRImageControl::CameraCorr*>());
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
	OSInt i;
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

const UTF8Char *SSWR::AVIRead::AVIRImageControl::GetObjectClass()
{
	return (const UTF8Char*)"AVIRImageControl";
}

OSInt SSWR::AVIRead::AVIRImageControl::OnNotify(Int32 code, void *lParam)
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
	OSInt i;

	this->imgMut->Lock();
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
	this->imgMut->Unlock();
}

void SSWR::AVIRead::AVIRImageControl::SetDPI(Double hdpi, Double ddpi)
{
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus *> *imgList;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	OSInt i;

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

	this->imgMut->Lock();
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
	this->imgMut->Unlock();
}

void SSWR::AVIRead::AVIRImageControl::OnDraw(Media::DrawImage *dimg)
{
	Media::DrawBrush *b;
	Media::DrawFont *f;
	Media::DrawBrush *barr[5];
	Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	OSInt i;
	OSInt j;
	OSInt scnW = dimg->GetWidth();
	OSInt scnH = dimg->GetHeight();

	barr[0] = dimg->NewBrushARGB(0xffc0c0c0);
	barr[1] = dimg->NewBrushARGB(0xffff8080);
	barr[2] = dimg->NewBrushARGB(0xff8080ff);
	barr[3] = dimg->NewBrushARGB(0xffcc80cc);

	barr[4] = dimg->NewBrushARGB(0xff000000);

	Double hdpi = this->GetHDPI();
	Double ddpi = this->GetDDPI();
	Int32 itemTH = Math::Double2Int32((20 + 12 + 12 + this->previewSize) * hdpi / ddpi);
	Int32 itemBH = Math::Double2Int32((20 + 12 + this->previewSize) * hdpi / ddpi);
	Int32 itemH = Math::Double2Int32((20 + this->previewSize) * hdpi / ddpi);
	Int32 scrPos = Math::Double2Int32(this->GetVScrollPos() * hdpi / ddpi);

	this->imgMut->Lock();
	imgList = this->imgMap->GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		status->setting.flags &= ~8;
	}
	i = this->GetVScrollPos() / (20 + 12 + 12 + this->previewSize);
	j = Math::Double2Int32(this->GetVScrollPos() + scnH * ddpi / hdpi) / (20 + 12 + 12 + this->previewSize);

	if (i < 0)
	{
		i = 0;
	}
	if ((UOSInt)j >= imgList->GetCount())
	{
		j = imgList->GetCount() - 1;
	}
	f = dimg->NewFontH(L"Arial", 12 * hdpi / ddpi, Media::DrawEngine::DFS_ANTIALIAS, 0);
	b = dimg->NewBrushARGB(0xff000000);
	OSInt strLen;
	Double strSz[2];
	while (i <= j)
	{
		status = imgList->GetItem(i);
		status->setting.flags |= 8;
		if (status->previewImg == 0)
		{
			status->previewImg = this->deng->LoadImage(status->cacheFile);
			if (status->previewImg)
			{
				status->previewImg2 = this->deng->CreateImage32(Math::Double2Int32(status->previewImg->GetWidth() * hdpi / ddpi), Math::Double2Int32(status->previewImg->GetHeight() * hdpi / ddpi), Media::AT_NO_ALPHA);
				this->UpdateImgPreview(status);
			}
		}
		else if (status->previewImg2 == 0)
		{
			status->previewImg2 = this->deng->CreateImage32(Math::Double2Int32(status->previewImg->GetWidth() * hdpi / ddpi), Math::Double2Int32(status->previewImg->GetHeight() * hdpi / ddpi), Media::AT_NO_ALPHA);
			this->UpdateImgPreview(status);
		}
		dimg->DrawRect(0, Math::OSInt2Double(i * itemTH - scrPos), Math::OSInt2Double(scnW), itemBH, 0, barr[status->setting.flags & 3]);
		dimg->DrawRect(0, Math::OSInt2Double(i * itemTH - scrPos + itemBH), Math::OSInt2Double(scnW), itemTH - itemBH, 0, barr[4]);
		if (status->previewImg2)
		{
			status->previewImg2->SetHDPI(dimg->GetHDPI());
			status->previewImg2->SetVDPI(dimg->GetVDPI());
			dimg->DrawImagePt(status->previewImg2, Math::OSInt2Double((scnW - status->previewImg2->GetWidth()) >> 1), Math::OSInt2Double(i * itemTH - scrPos + ((itemH - status->previewImg2->GetHeight()) >> 1)));
		}
		if (status->fileName)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(status->fileName);
			strLen = Text::StrCharCnt(sb.ToString());
			if (f && dimg->GetTextSizeUTF8(f, sb.ToString(), strLen, strSz))
			{
				dimg->DrawStringUTF8((scnW - strSz[0]) * 0.5, Math::OSInt2Double(i * itemTH - scrPos + itemH), sb.ToString(), f, b);
			}
		}
		i++;
	}
	if ((j + 1) * itemTH - scrPos < scnH)
	{
		dimg->DrawRect(0, Math::OSInt2Double((j + 1) * itemTH - scrPos), Math::OSInt2Double(scnW), scnH - Math::OSInt2Double((j + 1) * itemTH - scrPos), 0, barr[4]);
	}
	dimg->DelBrush(b);
	dimg->DelFont(f);
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
	this->imgMut->Unlock();

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
		OSInt i;
		OSInt j;
		Double hdpi = this->GetHDPI();
		Double ddpi = this->GetDDPI();
		Int32 scrPos = Math::Double2Int32(scrollY * hdpi / ddpi);
		OSInt clickIndex = (scrPos + yPos) / Math::Double2Int32((20 + 12 + 12 + this->previewSize) * hdpi / ddpi);
		if (keys & UI::GUICustomDrawVScroll::KBTN_CONTROL)
		{
			this->imgMut->Lock();
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status = this->imgMap->GetValues()->GetItem(clickIndex);
			status->setting.flags ^= 1;
			this->imgMut->Unlock();
			this->Redraw();
			this->currSel = clickIndex;
		}
		else if (keys & UI::GUICustomDrawVScroll::KBTN_SHIFT)
		{
			Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
			this->imgMut->Lock();
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
			if ((UOSInt)j >= imgList->GetCount())
			{
				j = imgList->GetCount() - 1;
			}
			while (i <= j)
			{
				status = imgList->GetItem(i);
				status->setting.flags |= 1;
				i++;
			}
			this->imgMut->Unlock();
			this->Redraw();
		}
		else
		{
			Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*> *imgList;
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
			this->imgMut->Lock();
			imgList = this->imgMap->GetValues();
			i = imgList->GetCount();
			while (i-- > 0)
			{
				status = imgList->GetItem(i);
				status->setting.flags &= ~1;
			}
			status = imgList->GetItem(clickIndex);
			if (status)
			{
				status->setting.flags |= 1;
				this->currSel = clickIndex;
			}
			this->imgMut->Unlock();
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

void SSWR::AVIRead::AVIRImageControl::SetFolder(const UTF8Char *folderPath)
{
	ThreadCancelTasks();
	if (this->folderPath)
	{
		this->folderMut->Lock();
		this->EndFolder();
		Text::StrDelNew(this->folderPath);
		this->folderPath = 0;
		this->folderMut->Unlock();
	}
	if (folderPath)
	{
		this->folderMut->Lock();
		this->folderPath = Text::StrCopyNew(folderPath);
		this->folderChanged = true;
		this->folderMut->Unlock();

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

const UTF8Char *SSWR::AVIRead::AVIRImageControl::GetFolder()
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
	OSInt i;
	OSInt j;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	if (this->folderPath == 0)
	{
		return false;
	}

	Text::StringBuilderUTF8 sb;
	sptr = Text::StrConcat(sbuff, this->folderPath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, (const UTF8Char*)"Setting.txt");

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	writer->WriteSignature();
	this->imgMut->Lock();
	imgList = this->imgMap->GetValues();
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		status = imgList->GetItem(i);
		sb.ClearStr();
		sb.Append(status->fileName);
		sb.Append((const UTF8Char*)"\t");
		sb.AppendI32(status->setting.flags);
		sb.Append((const UTF8Char*)"\t");
		Text::SBAppendF64(&sb, status->setting.brightness);
		sb.Append((const UTF8Char*)"\t");
		Text::SBAppendF64(&sb, status->setting.contrast);
		sb.Append((const UTF8Char*)"\t");
		Text::SBAppendF64(&sb, status->setting.gamma);
		writer->WriteLine(sb.ToString());
		i++;
	}
	this->imgMut->Unlock();
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

	this->imgMut->Lock();
	status = this->imgMap->Get(fileName);
	if (status)
	{
		this->ioMut->Lock();
		NEW_CLASS(fd, IO::StmData::FileData(status->filePath, false));
		imgList = (Media::ImageList*)this->core->GetParserList()->ParseFileType(fd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
		DEL_CLASS(fd);
		this->ioMut->Unlock();
	}
	this->imgMut->Unlock();

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

	this->imgMut->Lock();
	status = this->imgMap->Get(fileName);
	if (status)
	{
		this->ioMut->Lock();
		NEW_CLASS(fd, IO::StmData::FileData(status->filePath, false));
		imgList = (Media::ImageList*)this->core->GetParserList()->ParseFileType(fd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
		DEL_CLASS(fd);
		this->ioMut->Unlock();
	}
	this->imgMut->Unlock();

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
	Int32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	if (this->GetCameraName(&sb, srcImg->exif))
	{
		gammaParam = this->GetCameraGamma(sb.ToString(), &gammaCnt);
	}
	else
	{
		gammaParam = 0;
		gammaCnt = 0;
	}

	this->filterMut->Lock();
	this->filter->SetParameter((setting->brightness - 1.0) * setting->contrast, setting->contrast, setting->gamma, srcImg->info->color, srcImg->info->storeBPP, srcImg->info->pf, (setting->flags & 240) >> 4);
	this->filter->SetGammaCorr(gammaParam, gammaCnt);
	this->filter->ProcessImage(srcImg->data, destImg->data, srcImg->info->dispWidth, srcImg->info->dispHeight, srcImg->info->storeWidth * (srcImg->info->storeBPP >> 3), destImg->info->storeWidth * (srcImg->info->storeBPP >> 3));
	this->filterMut->Unlock();
}

void SSWR::AVIRead::AVIRImageControl::UpdateImgPreview(SSWR::AVIRead::AVIRImageControl::ImageStatus *img)
{
	Media::DrawImage *srcImg = img->previewImg;
	Media::DrawImage *destImg = img->previewImg2;
	OSInt sWidth = srcImg->GetWidth();
	OSInt sHeight = srcImg->GetHeight();
	OSInt sbpl = srcImg->GetImgBpl();
	Bool srev;
	UInt8 *sptr = srcImg->GetImgBits(&srev);
	OSInt dWidth = destImg->GetWidth();
	OSInt dHeight = destImg->GetHeight();
	OSInt dbpl = destImg->GetImgBpl();
	Bool drev;
	UInt8 *dptr = destImg->GetImgBits(&drev);

	UInt8 *tmpBuff = MemAllocA(UInt8, sHeight * sbpl);

	Double *gammaParam;
	Int32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	Media::EXIFData *exif = srcImg->GetEXIF();
	if (exif && this->GetCameraName(&sb, exif))
	{
		gammaParam = this->GetCameraGamma(sb.ToString(), &gammaCnt);
	}
	else
	{
		gammaParam = 0;
		gammaCnt = 0;
	}
	this->filterMut->Lock();
	this->filter->SetParameter((img->setting.brightness - 1.0) * img->setting.contrast, img->setting.contrast, img->setting.gamma, srcImg->GetColorProfile(), srcImg->GetBitCount(), srcImg->GetPixelFormat(), (img->setting.flags & 240) >> 4);
	this->filter->SetGammaCorr(gammaParam, gammaCnt);
	this->filter->ProcessImage(sptr, tmpBuff, sWidth, sHeight, sbpl, sbpl);
	this->dispResizer->Resize(tmpBuff, sbpl, Math::OSInt2Double(sWidth), Math::OSInt2Double(sHeight), 0, 0, dptr, dbpl, dWidth, dHeight);
	this->filterMut->Unlock();
	MemFreeA(tmpBuff);
	srcImg->GetImgBitsEnd(false);
	destImg->GetImgBitsEnd(true);
}

void SSWR::AVIRead::AVIRImageControl::UpdateImgSetting(SSWR::AVIRead::AVIRImageControl::ImageSetting *setting)
{
	Data::ArrayList<ImageStatus*> *imgList;
	ImageStatus *status;
	OSInt i;
	Bool chg = false;
	this->imgMut->Lock();
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
	this->imgMut->Unlock();
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

OSInt SSWR::AVIRead::AVIRImageControl::ExportSelected()
{
	ImageStatus *status;
	ImageStatus *status2;
	Data::ArrayList<ImageStatus*> *imgList;
	OSInt i;
	OSInt j;
	OSInt cnt = 0;
	this->imgMut->Lock();
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
			this->exportMut->Lock();
			this->exportList->Put(status2);
			this->exportMut->Unlock();
			cnt++;
		}
		i++;
	}
	this->imgMut->Unlock();
	if (cnt > 0)
	{
		this->exportMut->Lock();
		this->exportCurrCnt = 0;
		cnt = this->exportList->GetCount();
		this->exportMut->Unlock();
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
	OSInt j;
	if (this->folderPath == 0)
		return;

	this->imgMut->Lock();
	nameList = this->imgMap->GetKeys();
	imgList = this->imgMap->GetValues();
	if (this->dispImg)
	{
		i = nameList->SortedIndexOf(this->dispImg->fileName);
		if (i == -1)
		{
			i = nameList->GetCount() - 1;
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
		i = nameList->GetCount() - 1;
	}
	this->currSel = i;
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
		this->dispImg = this->imgMap->GetValues()->GetItem(i);
		this->dispImg->setting.flags |= 1;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, this->dispImg->fileName, &this->dispImg->setting);
		}
	}
	this->imgMut->Unlock();
	this->imgUpdated = true;
	if (i >= 0)
	{
		this->MakeVisible(i * (20 + 12 + 12 + this->previewSize));
	}
}

void SSWR::AVIRead::AVIRImageControl::MoveDown()
{
	Data::SortableArrayList<const UTF8Char *> *nameList;
	Data::ArrayList<ImageStatus *> *imgList;
	ImageStatus *status;
	OSInt i;
	OSInt j;
	if (this->folderPath == 0)
		return;

	this->imgMut->Lock();
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
	this->currSel = i;
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
		this->dispImg = this->imgMap->GetValues()->GetItem(i);
		this->dispImg->setting.flags |= 1;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, this->dispImg->fileName, &this->dispImg->setting);
		}
	}
	this->imgMut->Unlock();
	this->imgUpdated = true;
	if (i >= 0)
	{
		this->MakeVisible((i - 1) * (20 + 12 + 12 + this->previewSize));
	}
}

void SSWR::AVIRead::AVIRImageControl::SelectAll()
{
	this->imgMut->Lock();
	Data::ArrayList<ImageStatus*> *imgList = this->imgMap->GetValues();
	OSInt i = imgList->GetCount();
	ImageStatus *status;
	while (i-- > 0)
	{
		status = imgList->GetItem(i);;
		status->setting.flags |= 1;
	}
	this->imgMut->Unlock();
	this->Redraw();
}
