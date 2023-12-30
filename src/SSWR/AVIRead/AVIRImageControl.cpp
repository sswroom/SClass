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
#include "Sync/ThreadUtil.h"
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
	me->folderCtrlEvt.Set();
	while (true)
	{
		me->folderThreadEvt.Wait();
		if (me->threadCtrlCode == 2)
		{
			break;
		}
		else if (me->threadCtrlCode == 1)
		{
			me->threadState = 2;
			me->threadCtrlCode = 0;
			me->folderCtrlEvt.Set();
			me->InitDir();
			me->threadState = 1;
		}
		else if (me->threadCtrlCode == 3)
		{
			me->threadCtrlCode = 0;
			me->folderCtrlEvt.Set();
		}
		else
		{
			if (me->exportList.HasItems())
			{
				me->threadState = 3;
				me->ExportQueued();
				me->threadState = 1;
			}
		}
	}
	me->threadState = 0;
	me->folderCtrlEvt.Set();
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
		imgCnt = me->imgMap.GetCount();
		mutUsage.EndUse();
		totalHeight = imgCnt * (20 + 12 + 12 + me->previewSize);

		Math::Size2D<UOSInt> scnSize = me->GetSizeP();
		me->SetVScrollBar(0, totalHeight, (UOSInt)Double2OSInt(UOSInt2Double(scnSize.y) / me->GetHDPI() * me->GetDDPI()));
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
	UTF8Char *sarr[11];
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
	IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));
	*sptr2++ = IO::Path::PATH_SEPERATOR;

	UOSInt i;
	UOSInt colCnt;
	NotNullPtr<const Data::ArrayList<ImageSetting*>> imgSettList;
	ImageSetting *imgSett;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	NotNullPtr<Parser::ParserList> parsers;
	Data::ICaseStringUTF8Map<ImageSetting*> imgSettMap;
	sptr3 = Text::StrConcatC(sptr, UTF8STRC("Setting.txt"));
	{
		IO::FileStream fs(CSTRP(sbuff, sptr3), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			while (reader.ReadLine(sb, 4096))
			{
				sptr3 = sb.v;
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
					imgSett = imgSettMap.Put(sarr[0], imgSett);
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
					imgSett = imgSettMap.Put(sarr[0], imgSett);
					if (imgSett)
						MemFree(imgSett);
				}
				sb.ClearStr();
			}
		}
	}

	sptr3 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr3));
	if (sess)
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
		
		Media::Resizer::LanczosResizer8_C8 resizer(4, 3, srcProfile, destProfile, this->colorSess.Ptr(), Media::AT_NO_ALPHA);
		Exporter::GUIPNGExporter exporter;
		resizer.SetTargetSize(Math::Size2D<UOSInt>(this->previewSize, this->previewSize));
		parsers = this->core->GetParserList();
		UOSInt currCnt = 0;
		while (this->threadCtrlCode != 2 && this->threadCtrlCode != 3 && (sptr3 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				Media::ImageList *imgList;
				Sync::MutexUsage mutUsage(this->ioMut);
				{
					IO::StmData::FileData fd(CSTRP(sbuff, sptr3), false);
					imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParserType::ImageList);
				}
				mutUsage.EndUse();
				if (imgList)
				{
					NotNullPtr<Media::StaticImage> simg;
					imgList->ToStaticImage(0);
					if (simg.Set((Media::StaticImage*)imgList->GetImage(0, 0)))
					{
						Media::StaticImage *simg2;
						sptr2End = Text::StrConcatC(Text::StrConcatC(sptr2, sptr, (UOSInt)(sptr3 - sptr)), UTF8STRC(".png"));
						simg->To32bpp();
						simg2 = resizer.ProcessToNew(simg);
						{
							Media::ImageList imgList(CSTRP(sptr2, sptr2End));
							imgList.AddImage(simg2, 0);
							mutUsage.BeginUse();
							{
								IO::FileStream fs(CSTRP(sbuff2, sptr2End), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
								exporter.ExportFile(fs, CSTRP(sbuff2, sptr2End), imgList, 0);
							}
							mutUsage.EndUse();
						}
						DEL_CLASS(imgList);

						status = MemAlloc(SSWR::AVIRead::AVIRImageControl::ImageStatus, 1);
						status->filePath = Text::String::NewP(sbuff, sptr3);
						status->cacheFile = Text::String::NewP(sbuff2, sptr2End);
						status->fileName = status->filePath->ToCString().Substring((UOSInt)(sptr - sbuff));
						imgSett = imgSettMap.Get(sptr);
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
						this->imgMap.Put(status->fileName.v, status);
						this->imgMapUpdated = true;
						imgMutUsage.EndUse();
					}
					else
					{
						DEL_CLASS(imgList);
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

	imgSettList = imgSettMap.GetValues();
	i = imgSettList->GetCount();
	while (i-- > 0)
	{
		imgSett = imgSettList->GetItem(i);
		MemFree(imgSett);
	}
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
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;

	ImageStatus *status;
	NotNullPtr<Media::StaticImage> img;
	Exporter::TIFFExporter tifExporter;
	Exporter::GUIJPGExporter jpgExporter;
	while (this->threadCtrlCode != 2 && this->threadCtrlCode != 3)
	{
		Sync::MutexUsage mutUsage(this->exportMut);
		status = (ImageStatus*)this->exportList.Get();
		mutUsage.EndUse();
		if (status == 0)
			break;

		if (img.Set(this->LoadImage(status->fileName.v)))
		{
			this->ApplySetting(img, img, status->setting);
			sptr2 = status->fileName.ConcatTo(sptr);
			Media::ImageList imgList(CSTRP(sbuff, sptr2));
			imgList.AddImage(img, 0);
			Sync::MutexUsage ioMutUsage(this->ioMut);
			if (this->exportFmt == EF_JPG)
			{
				img->To32bpp();
				sptr2 = IO::Path::ReplaceExt(sptr, UTF8STRC("jpg"));
				IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
				void *param = jpgExporter.CreateParam(imgList);
				if (param)
				{
					jpgExporter.SetParamInt32(param, 0, 100);
				}
				jpgExporter.ExportFile(fs, CSTRP(sbuff, sptr2), imgList, param);
				if (param)
				{
					jpgExporter.DeleteParam(param);
				}
			}
			else if (this->exportFmt == EF_TIF)
			{
				sptr2 = IO::Path::ReplaceExt(sptr, UTF8STRC("tif"));
				IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
				tifExporter.ExportFile(fs, CSTRP(sbuff, sptr2), imgList, 0);
			}
			ioMutUsage.EndUse();
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
	this->folderThreadEvt.Set();
	while (this->threadState != 0 && this->threadState != 1)
	{
		this->folderCtrlEvt.Wait();
	}
}

void SSWR::AVIRead::AVIRImageControl::EndFolder()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	NotNullPtr<Media::DrawImage> img;
	UOSInt i;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*>> imgList;
	if (this->folderPath == 0)
		return;
	sptr = this->folderPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("Cache"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.png"));
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
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
	imgList = this->imgMap.GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		status->filePath->Release();
		status->cacheFile->Release();
		if (img.Set(status->previewImg))
		{
			this->deng->DeleteImage(img);
		}
		if (img.Set(status->previewImg2))
		{
			this->deng->DeleteImage(img);
		}
		MemFree(status);
	}
	Sync::MutexUsage expMutUsage(this->exportMut);
	while (true)
	{
		status = (SSWR::AVIRead::AVIRImageControl::ImageStatus*)this->exportList.Get();
		if (status == 0)
			break;
		MemFree(status);
	}
	expMutUsage.EndUse();
	this->imgMap.Clear();
	this->imgMapUpdated = true;
	mutUsage.EndUse();
}

Bool SSWR::AVIRead::AVIRImageControl::GetCameraName(NotNullPtr<Text::StringBuilderUTF8> sb, NotNullPtr<Media::EXIFData> exif)
{
	Text::CString make = exif->GetPhotoMake();
	Text::CString model = exif->GetPhotoModel();
	if (make.v && model.v)
	{
		if (model.StartsWith(make.v, make.leng))
		{
			sb->Append(model);
		}
		else
		{
			sb->Append(make);
			sb->AppendC(UTF8STRC(" "));
			sb->Append(model);
		}
	}
	else if (make.v)
	{
		sb->Append(make);
	}
	else if (model.v)
	{
		sb->Append(model);
	}
	else
	{
		return false;
	}
	return true;
}

Double *SSWR::AVIRead::AVIRImageControl::GetCameraGamma(Text::CStringNN cameraName, OutParam<UInt32> gammaCnt)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRImageControl::CameraCorr *camera;
	Sync::MutexUsage mutUsage(this->cameraMut);
	camera = this->cameraMap.Get(cameraName);
	if (camera)
	{
		mutUsage.EndUse();
		gammaCnt.Set(camera->gammaCnt);
		return camera->gammaParam;
	}
	camera = MemAlloc(SSWR::AVIRead::AVIRImageControl::CameraCorr, 1);
	camera->gammaCnt = 0;
	camera->gammaParam = 0;
	this->cameraMap.Put(cameraName, camera);

	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, cameraName);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".gamma"));
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Double> gammaVals;
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		while (true)
		{
			Double val;
			sb.ClearStr();
			if (!reader.ReadLine(sb, 512))
			{
				break;
			}
			if (Text::StrToDouble(sb.ToString(), val))
			{
				gammaVals.Add(val);
			}
		}
		if (gammaVals.GetCount() > 0)
		{
			camera->gammaCnt = (UInt32)gammaVals.GetCount();
			camera->gammaParam = MemAlloc(Double, camera->gammaCnt);
			MemCopyNO(camera->gammaParam, gammaVals.Ptr(), sizeof(Double) * camera->gammaCnt);
		}
	}

	mutUsage.EndUse();
	gammaCnt.Set(camera->gammaCnt);
	return camera->gammaParam;
}

SSWR::AVIRead::AVIRImageControl::AVIRImageControl(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<SSWR::AVIRead::AVIRCore> core, UI::GUIForm *frm, NotNullPtr<Media::ColorManagerSess> colorSess) : UI::GUICustomDrawVScroll(ui, parent, core->GetDrawEngine()), filter(core->GetColorMgr())
{
	this->core = core;
	this->folderPath = 0;
	this->colorSess = colorSess;
	this->colorSess->AddHandler(*this);
	this->exportCurrCnt = 0;
	this->exportFmt = EF_JPG;
	this->keyHdlr = 0;
	this->keyObj = 0;
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destColor(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->dispResizer, Media::Resizer::LanczosResizer8_C8(3, 3, srcColor, destColor, colorSess.Ptr(), Media::AT_NO_ALPHA));
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
	Sync::ThreadUtil::Create(FolderThread, this);
	while (this->threadState == 0)
		this->folderCtrlEvt.Wait();
	
	frm->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRImageControl::~AVIRImageControl()
{
	SetFolder(CSTR_NULL);
	this->threadCtrlCode = 2;
	this->folderThreadEvt.Set();
	while (this->threadState != 0)
	{
		this->folderCtrlEvt.Wait(10);
	}
	UOSInt i;
	NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::CameraCorr *>> cameraList = this->cameraMap.GetValues();
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

	DEL_CLASS(this->dispResizer);
	this->colorSess->RemoveHandler(*this);
}

Text::CStringNN SSWR::AVIRead::AVIRImageControl::GetObjectClass() const
{
	return CSTR("AVIRImageControl");
}

OSInt SSWR::AVIRead::AVIRImageControl::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void SSWR::AVIRead::AVIRImageControl::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRImageControl::RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam)
{
	NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus *>> imgList;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	UOSInt i;

	Sync::MutexUsage mutUsage(this->imgMut);
	NotNullPtr<Media::DrawImage> img;
	imgList = this->imgMap.GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		if (img.Set(status->previewImg2))
		{
			this->deng->DeleteImage(img);
			status->previewImg2 = 0;
		}
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRImageControl::SetDPI(Double hdpi, Double ddpi)
{
	NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus *>> imgList;
	SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
	UOSInt i;
	NotNullPtr<Media::DrawImage> img;

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
	imgList = this->imgMap.GetValues();
	i = imgList->GetCount();
	while (i-- > 0)
	{
		status = imgList->GetItem(i);
		if (img.Set(status->previewImg2))
		{
			this->deng->DeleteImage(img);
			status->previewImg2 = 0;
		}
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRImageControl::OnDraw(NotNullPtr<Media::DrawImage> dimg)
{
	NotNullPtr<Media::DrawBrush> b;
	NotNullPtr<Media::DrawFont> f;
	NotNullPtr<Media::DrawBrush> barr[5];
	NotNullPtr<Media::DrawImage> img;
	NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*>> imgList;
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
	imgList = this->imgMap.GetValues();
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
		f = dimg->NewFontPt(CSTR("Arial"), 9, Media::DrawEngine::DFS_ANTIALIAS, 0);
		b = dimg->NewBrushARGB(0xff000000);
		Math::Size2DDbl strSz;
		while (i <= j)
		{
			status = imgList->GetItem(i);
			status->setting.flags |= 8;
			if (status->previewImg == 0)
			{
				status->previewImg = this->deng->LoadImage(status->cacheFile->ToCString());
				if (status->previewImg)
				{
					status->previewImg2 = this->deng->CreateImage32(Math::Size2D<UOSInt>((UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetWidth()) * hdpi / ddpi), (UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetHeight()) * hdpi / ddpi)), Media::AT_NO_ALPHA);
					this->UpdateImgPreview(status);
				}
			}
			else if (status->previewImg2 == 0)
			{
				status->previewImg2 = this->deng->CreateImage32(Math::Size2D<UOSInt>((UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetWidth()) * hdpi / ddpi), (UInt32)Double2Int32(UOSInt2Double(status->previewImg->GetHeight()) * hdpi / ddpi)), Media::AT_NO_ALPHA);
				this->UpdateImgPreview(status);
			}
			dimg->DrawRect(Math::Coord2DDbl(0, OSInt2Double((OSInt)(i * itemTH - scrPos))), Math::Size2DDbl(UOSInt2Double(scnW), itemBH), 0, barr[status->setting.flags & 3]);
			dimg->DrawRect(Math::Coord2DDbl(0, OSInt2Double((OSInt)(i * itemTH - scrPos + itemBH))), Math::Size2DDbl(UOSInt2Double(scnW), itemTH - itemBH), 0, barr[4]);
			NotNullPtr<Media::DrawImage> previewImg2;
			if (previewImg2.Set(status->previewImg2))
			{
				previewImg2->SetHDPI(dimg->GetHDPI());
				previewImg2->SetVDPI(dimg->GetVDPI());
				dimg->DrawImagePt(previewImg2, Math::Coord2DDbl(UOSInt2Double((scnW - previewImg2->GetWidth()) >> 1), OSInt2Double((OSInt)(i * itemTH - scrPos + ((itemH - previewImg2->GetHeight()) >> 1)))));
			}
			if (status->fileName.leng > 0)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(status->fileName);
				if ((strSz = dimg->GetTextSize(f, sb.ToCString())).HasArea())
				{
					dimg->DrawString(Math::Coord2DDbl((UOSInt2Double(scnW) - strSz.x) * 0.5, UOSInt2Double(i * itemTH - scrPos + itemH)), sb.ToCString(), f, b);
				}
			}
			i++;
		}
		if ((j + 1) * itemTH - scrPos < scnH)
		{
			dimg->DrawRect(Math::Coord2DDbl(0, UOSInt2Double((j + 1) * itemTH - scrPos)), Math::Size2DDbl(UOSInt2Double(scnW), UOSInt2Double(scnH) - UOSInt2Double((j + 1) * itemTH - scrPos)), 0, barr[4]);
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
			if (img.Set(status->previewImg))
			{
				this->deng->DeleteImage(img);
				status->previewImg = 0;
			}
			if (img.Set(status->previewImg2))
			{
				this->deng->DeleteImage(img);
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

void SSWR::AVIRead::AVIRImageControl::OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	if (btn == UI::GUIClientControl::MBTN_LEFT)
	{
		UOSInt i;
		UOSInt j;
		Double hdpi = this->GetHDPI();
		Double ddpi = this->GetDDPI();
		Int32 scrPos = Double2Int32(OSInt2Double(scrollY) * hdpi / ddpi);
		UOSInt clickIndex = (UInt32)((scrPos + pos.y) / Double2Int32((20 + 12 + 12 + this->previewSize) * hdpi / ddpi));
		if (keys & UI::GUICustomDrawVScroll::KBTN_CONTROL)
		{
			Sync::MutexUsage mutUsage(this->imgMut);
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status = this->imgMap.GetValues()->GetItem((UOSInt)clickIndex);
			status->setting.flags ^= 1;
			mutUsage.EndUse();
			this->Redraw();
			this->currSel = clickIndex;
		}
		else if (keys & UI::GUICustomDrawVScroll::KBTN_SHIFT)
		{
			NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*>> imgList;
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
			Sync::MutexUsage mutUsage(this->imgMut);
			imgList = this->imgMap.GetValues();
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
			NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*>> imgList;
			SSWR::AVIRead::AVIRImageControl::ImageStatus *status;
			Sync::MutexUsage mutUsage(this->imgMut);
			imgList = this->imgMap.GetValues();
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
						this->dispHdlr(this->dispHdlrObj, CSTR_NULL, 0);
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

void SSWR::AVIRead::AVIRImageControl::SetFolder(Text::CString folderPath)
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
	if (folderPath.leng > 0)
	{
		Sync::MutexUsage mutUsage(this->folderMut);
		this->folderPath = Text::String::New(folderPath).Ptr();
		this->folderChanged = true;
		mutUsage.EndUse();

		this->threadCtrlCode = 1;
		this->folderThreadEvt.Set();
		while (this->threadCtrlCode == 1 && this->threadState != 0)
		{
			this->folderCtrlEvt.Wait();
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
	NotNullPtr<const Data::ArrayList<SSWR::AVIRead::AVIRImageControl::ImageStatus*>> imgList;
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

	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Text::UTF8Writer writer(fs);
	writer.WriteSignature();
	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap.GetValues();
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		status = imgList->GetItem(i);
		sb.ClearStr();
		sb.Append(status->fileName);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendI32(status->setting.flags);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendDouble(status->setting.brightness);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendDouble(status->setting.contrast);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendDouble(status->setting.gamma);
		writer.WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
	mutUsage.EndUse();
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

	Sync::MutexUsage mutUsage(this->imgMut);
	status = this->imgMap.Get(fileName);
	if (status)
	{
		Sync::MutexUsage ioMutUsage(this->ioMut);
		IO::StmData::FileData fd(status->filePath, false);
		imgList = (Media::ImageList*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
		ioMutUsage.EndUse();
	}
	mutUsage.EndUse();

	if (imgList)
	{
		outImg = imgList->GetImage(0, 0)->CreateStaticImage().Ptr();
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

	Sync::MutexUsage mutUsage(this->imgMut);
	status = this->imgMap.Get(fileName);
	if (status)
	{
		Sync::MutexUsage ioMutUsage(this->ioMut);
		IO::StmData::FileData fd(status->filePath, false);
		imgList = (Media::ImageList*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
		ioMutUsage.EndUse();
	}
	mutUsage.EndUse();

	if (imgList)
	{
		outImg = imgList->GetImage(0, 0)->CreateStaticImage().Ptr();
		DEL_CLASS(imgList);
	}
	return outImg;
}

void SSWR::AVIRead::AVIRImageControl::ApplySetting(NotNullPtr<Media::StaticImage> srcImg, NotNullPtr<Media::StaticImage> destImg, NotNullPtr<SSWR::AVIRead::AVIRImageControl::ImageSetting> setting)
{
	Double *gammaParam;
	UInt32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	NotNullPtr<Media::EXIFData> exif;
	if (srcImg->exif.SetTo(exif) && this->GetCameraName(sb, exif))
	{
		gammaParam = this->GetCameraGamma(sb.ToCString(), gammaCnt);
	}
	else
	{
		gammaParam = 0;
		gammaCnt = 0;
	}

	Sync::MutexUsage mutUsage(this->filterMut);
	this->filter.SetParameter((setting->brightness - 1.0) * setting->contrast, setting->contrast, setting->gamma, srcImg->info.color, srcImg->info.storeBPP, srcImg->info.pf, (setting->flags & 240) >> 4);
	this->filter.SetGammaCorr(gammaParam, gammaCnt);
	this->filter.ProcessImage(srcImg->data, destImg->data, srcImg->info.dispSize.x, srcImg->info.dispSize.y, (srcImg->info.storeSize.x * (srcImg->info.storeBPP >> 3)), (destImg->info.storeSize.x * (srcImg->info.storeBPP >> 3)), false);
}

void SSWR::AVIRead::AVIRImageControl::UpdateImgPreview(SSWR::AVIRead::AVIRImageControl::ImageStatus *img)
{
	Media::DrawImage *srcImg = img->previewImg;
	Media::DrawImage *destImg = img->previewImg2;
	UOSInt sWidth = srcImg->GetWidth();
	UOSInt sHeight = srcImg->GetHeight();
	UOSInt sbpl = srcImg->GetImgBpl();
	Bool srev;
	UInt8 *sptr = srcImg->GetImgBits(srev);
	UOSInt dWidth = destImg->GetWidth();
	UOSInt dHeight = destImg->GetHeight();
	UOSInt dbpl = destImg->GetImgBpl();
	Bool drev;
	UInt8 *dptr = destImg->GetImgBits(drev);

	UInt8 *tmpBuff = MemAllocA(UInt8, sHeight * (UOSInt)sbpl);

	Double *gammaParam;
	UInt32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	NotNullPtr<Media::EXIFData> exif;
	if (srcImg->GetEXIF().SetTo(exif) && this->GetCameraName(sb, exif))
	{
		gammaParam = this->GetCameraGamma(sb.ToCString(), gammaCnt);
	}
	else
	{
		gammaParam = 0;
		gammaCnt = 0;
	}
	Sync::MutexUsage mutUsage(this->filterMut);
	this->filter.SetParameter((img->setting.brightness - 1.0) * img->setting.contrast, img->setting.contrast, img->setting.gamma, srcImg->GetColorProfile(), srcImg->GetBitCount(), srcImg->GetPixelFormat(), (img->setting.flags & 240) >> 4);
	this->filter.SetGammaCorr(gammaParam, gammaCnt);
	this->filter.ProcessImage(sptr, tmpBuff, sWidth, sHeight, sbpl, sbpl, srev ^ drev);
	this->dispResizer->Resize(tmpBuff, (OSInt)sbpl, UOSInt2Double(sWidth), UOSInt2Double(sHeight), 0, 0, dptr, (OSInt)dbpl, dWidth, dHeight);
	mutUsage.EndUse();
	MemFreeA(tmpBuff);
	srcImg->GetImgBitsEnd(false);
	destImg->GetImgBitsEnd(true);
}

void SSWR::AVIRead::AVIRImageControl::UpdateImgSetting(SSWR::AVIRead::AVIRImageControl::ImageSetting *setting)
{
	NotNullPtr<const Data::ArrayList<ImageStatus*>> imgList;
	ImageStatus *status;
	UOSInt i;
	Bool chg = false;
	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap.GetValues();
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
	NotNullPtr<const Data::ArrayList<ImageStatus*>> imgList;
	UOSInt i;
	UOSInt j;
	UOSInt cnt = 0;
	Sync::MutexUsage mutUsage(this->imgMut);
	imgList = this->imgMap.GetValues();
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
			this->exportList.Put(status2);
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
		cnt = this->exportList.GetCount();
		exportMutUsage.EndUse();
		this->folderThreadEvt.Set();
	}
	return cnt;
}

void SSWR::AVIRead::AVIRImageControl::MoveUp()
{
	NotNullPtr<Data::SortableArrayList<const UTF8Char *>> nameList;
	NotNullPtr<const Data::ArrayList<ImageStatus *>> imgList;
	ImageStatus *status;
	OSInt i;
	UOSInt j;
	if (this->folderPath == 0)
		return;

	Sync::MutexUsage mutUsage(this->imgMut);
	nameList = this->imgMap.GetKeys();
	imgList = this->imgMap.GetValues();
	if (this->dispImg)
	{
		i = nameList->SortedIndexOf(this->dispImg->fileName.v);
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
			this->dispHdlr(this->dispHdlrObj, CSTR_NULL, 0);
		}
	}
	else
	{
		this->dispImg = this->imgMap.GetValues()->GetItem((UOSInt)i);
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
		UOSInt firstPos = ((UOSInt)i * (20 + 12 + 12 + this->previewSize));
		this->MakeVisible(firstPos, firstPos + this->previewSize + 20 + 12 + 12);
	}
}

void SSWR::AVIRead::AVIRImageControl::MoveDown()
{
	NotNullPtr<Data::SortableArrayList<const UTF8Char *>> nameList;
	NotNullPtr<const Data::ArrayList<ImageStatus *>> imgList;
	ImageStatus *status;
	OSInt i;
	UOSInt j;
	if (this->folderPath == 0)
		return;

	Sync::MutexUsage mutUsage(this->imgMut);
	nameList = this->imgMap.GetKeys();
	imgList = this->imgMap.GetValues();
	if (this->dispImg)
	{
		i = nameList->SortedIndexOf(this->dispImg->fileName.v);
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
			this->dispHdlr(this->dispHdlrObj, CSTR_NULL, 0);
		}
	}
	else
	{
		this->dispImg = this->imgMap.GetValues()->GetItem((UOSInt)i);
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
		UOSInt firstPos = ((UOSInt)i * (20 + 12 + 12 + this->previewSize));
		this->MakeVisible(firstPos, firstPos + this->previewSize + 20 + 12 + 12);
	}
}

void SSWR::AVIRead::AVIRImageControl::SelectAll()
{
	Sync::MutexUsage mutUsage(this->imgMut);
	NotNullPtr<const Data::ArrayList<ImageStatus*>> imgList = this->imgMap.GetValues();
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
