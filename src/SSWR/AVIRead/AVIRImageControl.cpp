#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.h"
#include "Exporter/GUIJPGExporter.h"
#include "Exporter/GUIPNGExporter.h"
#include "Exporter/TIFFExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
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

UInt32 __stdcall SSWR::AVIRead::AVIRImageControl::FolderThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageControl> me = userObj.GetNN<SSWR::AVIRead::AVIRImageControl>();
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

void __stdcall SSWR::AVIRead::AVIRImageControl::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageControl> me = userObj.GetNN<SSWR::AVIRead::AVIRImageControl>();
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
	NN<Text::String> folderPath;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr2End;
	UnsafeArray<UTF8Char> sptr3;
	UnsafeArray<UTF8Char> sarr[11];
	if (!this->folderPath.SetTo(folderPath))
		return;
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;
	Text::StringBuilderUTF8 sb;

	sptr = folderPath->ConcatTo(sbuff);
	sptr2 = folderPath->ConcatTo(sbuff2);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	if (sptr2[-1] != IO::Path::PATH_SEPERATOR)
		*sptr2++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr2, UTF8STRC("Cache"));
	IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));
	*sptr2++ = IO::Path::PATH_SEPERATOR;

	UOSInt i;
	UOSInt colCnt;
	NN<const Data::ArrayList<ImageSetting*>> imgSettList;
	ImageSetting *imgSett;
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
	NN<Parser::ParserList> parsers;
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
					imgSett->brightness = Text::StrToDoubleOr(sarr[2], 0);
					imgSett->contrast = Text::StrToDoubleOr(sarr[3], 0);
					imgSett->gamma = Text::StrToDoubleOr(sarr[4], 1);
					imgSett->cropEnabled = false;
					imgSett->cropLeft = 0;
					imgSett->cropTop = 0;
					imgSett->cropWidth = 0;
					imgSett->cropHeight = 0;
					imgSett = imgSettMap.Put(UnsafeArray<const UTF8Char>(sarr[0]), imgSett);
					if (imgSett)
						MemFree(imgSett);
				}
				else if (colCnt == 10)
				{
					imgSett = MemAlloc(ImageSetting, 1);
					imgSett->flags = Text::StrToInt32(sarr[1]);
					imgSett->brightness = Text::StrToDoubleOr(sarr[2], 0);
					imgSett->contrast = Text::StrToDoubleOr(sarr[3], 0);
					imgSett->gamma = Text::StrToDoubleOr(sarr[4], 1);
					imgSett->cropEnabled = Text::StrToInt32(sarr[5]) != 0;
					imgSett->cropLeft = Text::StrToInt32(sarr[6]);
					imgSett->cropTop = Text::StrToInt32(sarr[7]);
					imgSett->cropWidth = Text::StrToInt32(sarr[8]);
					imgSett->cropHeight = Text::StrToInt32(sarr[9]);
					imgSett = imgSettMap.Put(UnsafeArray<const UTF8Char>(sarr[0]), imgSett);
					if (imgSett)
						MemFree(imgSett);
				}
				sb.ClearStr();
			}
		}
	}

	sptr3 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(sbuff, sptr3)).SetTo(sess))
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
		
		Media::Resizer::LanczosResizerRGB_C8 resizer(4, 3, srcProfile, destProfile, this->colorSess.Ptr(), Media::AT_ALPHA_ALL_FF);
		Exporter::GUIPNGExporter exporter;
		resizer.SetTargetSize(Math::Size2D<UOSInt>(this->previewSize, this->previewSize));
		parsers = this->core->GetParserList();
		UOSInt currCnt = 0;
		while (this->threadCtrlCode != 2 && this->threadCtrlCode != 3 && IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptr3))
		{
			if (pt == IO::Path::PathType::File)
			{
				Optional<Media::ImageList> imgList;
				NN<Media::ImageList> nnimgList;
				Sync::MutexUsage mutUsage(this->ioMut);
				{
					IO::StmData::FileData fd(CSTRP(sbuff, sptr3), false);
					imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList));
				}
				mutUsage.EndUse();
				if (imgList.SetTo(nnimgList))
				{
					NN<Media::StaticImage> simg;
					nnimgList->ToStaticImage(0);
					if (Optional<Media::StaticImage>::ConvertFrom(nnimgList->GetImage(0, 0)).SetTo(simg))
					{
						NN<Media::StaticImage> simg2;
						sptr2End = Text::StrConcatC(Text::StrConcatC(sptr2, sptr, (UOSInt)(sptr3 - sptr)), UTF8STRC(".png"));
						if (!resizer.IsSupported(simg->info))
						{
							simg->ToB8G8R8A8();
						}
						resizer.SetSrcPixelFormat(simg->info.pf, simg->pal);
						if (resizer.ProcessToNew(simg).SetTo(simg2))
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
						nnimgList.Delete();

						status = MemAllocNN(SSWR::AVIRead::AVIRImageControl::ImageStatus);
						status->filePath = Text::String::NewP(sbuff, sptr3);
						status->cacheFile = Text::String::NewP(sbuff2, sptr2End);
						status->fileName = status->filePath->ToCString().Substring((UOSInt)(sptr - sbuff));
						imgSett = imgSettMap.Get(UnsafeArray<const UTF8Char>(sptr));
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
						this->imgMap.PutC(status->fileName, status);
						this->imgMapUpdated = true;
						imgMutUsage.EndUse();
					}
					else
					{
						nnimgList.Delete();
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
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<Text::String> folderPath;
	if (!this->folderPath.SetTo(folderPath))
		return;

	sptr = folderPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("Output"));
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;

	NN<ImageStatus> status;
	NN<Media::StaticImage> img;
	Exporter::TIFFExporter tifExporter;
	Exporter::GUIJPGExporter jpgExporter;
	while (this->threadCtrlCode != 2 && this->threadCtrlCode != 3)
	{
		Sync::MutexUsage mutUsage(this->exportMut);
		if (!this->exportList.Get().GetOpt<ImageStatus>().SetTo(status))
			break;
		mutUsage.EndUse();

		if (this->LoadImage(status->fileName).SetTo(img))
		{
			this->ApplySetting(img, img, status->setting);
			sptr2 = status->fileName.ConcatTo(sptr);
			Media::ImageList imgList(CSTRP(sbuff, sptr2));
			imgList.AddImage(img, 0);
			Sync::MutexUsage ioMutUsage(this->ioMut);
			if (this->exportFmt == EF_JPG)
			{
				img->ToB8G8R8A8();
				sptr2 = IO::Path::ReplaceExt(sptr, UTF8STRC("jpg"));
				IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
				Optional<IO::FileExporter::ParamData> param = jpgExporter.CreateParam(imgList);
				if (!param.IsNull())
				{
					jpgExporter.SetParamInt32(param, 0, 100);
				}
				jpgExporter.ExportFile(fs, CSTRP(sbuff, sptr2), imgList, param);
				if (!param.IsNull())
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
		MemFreeNN(status);

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
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;
	NN<Media::DrawImage> img;
	UOSInt i;
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
	NN<Text::String> folderPath;
	if (!this->folderPath.SetTo(folderPath))
		return;
	sptr = folderPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("Cache"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.png"));
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).NotNull())
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
	i = this->imgMap.GetCount();
	while (i-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(i);
		status->filePath->Release();
		status->cacheFile->Release();
		if (status->previewImg.SetTo(img))
		{
			this->deng->DeleteImage(img);
		}
		if (status->previewImg2.SetTo(img))
		{
			this->deng->DeleteImage(img);
		}
		MemFreeNN(status);
	}
	Sync::MutexUsage expMutUsage(this->exportMut);
	while (true)
	{
		if (!this->exportList.Get().GetOpt<ImageStatus>().SetTo(status))
			break;
		MemFreeNN(status);
	}
	expMutUsage.EndUse();
	this->imgMap.Clear();
	this->imgMapUpdated = true;
	mutUsage.EndUse();
}

Bool SSWR::AVIRead::AVIRImageControl::GetCameraName(NN<Text::StringBuilderUTF8> sb, NN<Media::EXIFData> exif)
{
	Text::CString make = exif->GetPhotoMake();
	Text::CString model = exif->GetPhotoModel();
	Text::CStringNN nnmake;
	Text::CStringNN nnmodel;
	if (make.SetTo(nnmake) && model.SetTo(nnmodel))
	{
		if (nnmodel.StartsWith(nnmake.v, nnmake.leng))
		{
			sb->Append(nnmodel);
		}
		else
		{
			sb->Append(nnmake);
			sb->AppendC(UTF8STRC(" "));
			sb->Append(nnmodel);
		}
	}
	else if (make.SetTo(nnmake))
	{
		sb->Append(nnmake);
	}
	else if (model.SetTo(nnmodel))
	{
		sb->Append(nnmodel);
	}
	else
	{
		return false;
	}
	return true;
}

UnsafeArrayOpt<Double> SSWR::AVIRead::AVIRImageControl::GetCameraGamma(Text::CStringNN cameraName, OutParam<UInt32> gammaCnt)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<Double> gammaParam;
	NN<SSWR::AVIRead::AVIRImageControl::CameraCorr> camera;
	Sync::MutexUsage mutUsage(this->cameraMut);
	if (this->cameraMap.GetC(cameraName).SetTo(camera))
	{
		mutUsage.EndUse();
		gammaCnt.Set(camera->gammaCnt);
		return camera->gammaParam;
	}
	camera = MemAllocNN(SSWR::AVIRead::AVIRImageControl::CameraCorr);
	camera->gammaCnt = 0;
	camera->gammaParam = 0;
	this->cameraMap.PutC(cameraName, camera);

	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
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
			camera->gammaParam = gammaParam = MemAllocArr(Double, camera->gammaCnt);
			MemCopyNO(gammaParam.Ptr(), gammaVals.Arr().Ptr(), sizeof(Double) * camera->gammaCnt);
		}
	}

	mutUsage.EndUse();
	gammaCnt.Set(camera->gammaCnt);
	return camera->gammaParam;
}

SSWR::AVIRead::AVIRImageControl::AVIRImageControl(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUIForm> frm, NN<Media::ColorManagerSess> colorSess) : UI::GUICustomDrawVScroll(ui, parent, core->GetDrawEngine(), colorSess), filter(core->GetColorMgr())
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
	NEW_CLASSNN(this->dispResizer, Media::Resizer::LanczosResizerRGB_C8(3, 3, srcColor, destColor, colorSess, Media::AT_ALPHA_ALL_FF));
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
	SetFolder(nullptr);
	this->threadCtrlCode = 2;
	this->folderThreadEvt.Set();
	while (this->threadState != 0)
	{
		this->folderCtrlEvt.Wait(10);
	}
	UOSInt i;
	NN<const Data::ArrayListNN<SSWR::AVIRead::AVIRImageControl::CameraCorr>> cameraList = this->cameraMap.GetValues();
	NN<CameraCorr> camera;
	UnsafeArray<Double> gammaParam;
	i = cameraList->GetCount();
	while (i-- > 0)
	{
		camera = cameraList->GetItemNoCheck(i);
		if (camera->gammaParam.SetTo(gammaParam))
		{
			MemFreeArr(gammaParam);
		}
		MemFreeNN(camera);
	}

	this->dispResizer.Delete();
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

void SSWR::AVIRead::AVIRImageControl::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRImageControl::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
	UOSInt i;

	Sync::MutexUsage mutUsage(this->imgMut);
	NN<Media::DrawImage> img;
	i = this->imgMap.GetCount();
	while (i-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(i);
		if (status->previewImg2.SetTo(img))
		{
			this->deng->DeleteImage(img);
			status->previewImg2 = 0;
		}
	}
}

void SSWR::AVIRead::AVIRImageControl::SetDPI(Double hdpi, Double ddpi)
{
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
	UOSInt i;
	NN<Media::DrawImage> img;

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
	i = this->imgMap.GetCount();
	while (i-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(i);
		if (status->previewImg2.SetTo(img))
		{
			this->deng->DeleteImage(img);
			status->previewImg2 = 0;
		}
	}
}

void SSWR::AVIRead::AVIRImageControl::OnDraw(NN<Media::DrawImage> dimg)
{
	NN<Media::DrawBrush> b;
	NN<Media::DrawFont> f;
	NN<Media::DrawBrush> barr[5];
	NN<Media::DrawImage> img;
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
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
	i = this->imgMap.GetCount();
	while (i-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(i);
		status->setting.flags &= ~8;
	}
	i = this->GetVScrollPos() / (20 + 12 + 12 + this->previewSize);
	j = (UInt32)Double2Int32(UOSInt2Double(this->GetVScrollPos()) + UOSInt2Double(scnH) * ddpi / hdpi) / (20 + 12 + 12 + this->previewSize);

	if (this->imgMap.GetCount() > 0)
	{
		if (j >= this->imgMap.GetCount())
		{
			j = this->imgMap.GetCount() - 1;
		}
		f = dimg->NewFontPt(CSTR("Arial"), 9, Media::DrawEngine::DFS_ANTIALIAS, 0);
		b = dimg->NewBrushARGB(0xff000000);
		Math::Size2DDbl strSz;
		NN<Media::DrawImage> previewImg;
		while (i <= j)
		{
			status = this->imgMap.GetItemNoCheck(i);
			status->setting.flags |= 8;
			if (!status->previewImg.SetTo(previewImg))
			{
				status->previewImg = this->deng->LoadImage(status->cacheFile->ToCString());
				if (status->previewImg.SetTo(previewImg))
				{
					status->previewImg2 = this->deng->CreateImage32(Math::Size2D<UOSInt>((UInt32)Double2Int32(UOSInt2Double(previewImg->GetWidth()) * hdpi / ddpi), (UInt32)Double2Int32(UOSInt2Double(previewImg->GetHeight()) * hdpi / ddpi)), Media::AT_IGNORE_ALPHA);
					this->UpdateImgPreview(status);
				}
			}
			else if (status->previewImg2.IsNull())
			{
				status->previewImg2 = this->deng->CreateImage32(Math::Size2D<UOSInt>((UInt32)Double2Int32(UOSInt2Double(previewImg->GetWidth()) * hdpi / ddpi), (UInt32)Double2Int32(UOSInt2Double(previewImg->GetHeight()) * hdpi / ddpi)), Media::AT_IGNORE_ALPHA);
				this->UpdateImgPreview(status);
			}
			dimg->DrawRect(Math::Coord2DDbl(0, OSInt2Double((OSInt)(i * itemTH - scrPos))), Math::Size2DDbl(UOSInt2Double(scnW), itemBH), 0, barr[status->setting.flags & 3]);
			dimg->DrawRect(Math::Coord2DDbl(0, OSInt2Double((OSInt)(i * itemTH - scrPos + itemBH))), Math::Size2DDbl(UOSInt2Double(scnW), itemTH - itemBH), 0, barr[4]);
			NN<Media::DrawImage> previewImg2;
			if (status->previewImg2.SetTo(previewImg2))
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

	i = this->imgMap.GetCount();
	while (i-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(i);
		if ((status->setting.flags & 8) == 0)
		{
			if (status->previewImg.SetTo(img))
			{
				this->deng->DeleteImage(img);
				status->previewImg = 0;
			}
			if (status->previewImg2.SetTo(img))
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
			NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
			if (this->imgMap.GetItem((UOSInt)clickIndex).SetTo(status))
			{
				status->setting.flags ^= 1;
				mutUsage.EndUse();
				this->Redraw();
				this->currSel = clickIndex;
			}
		}
		else if (keys & UI::GUICustomDrawVScroll::KBTN_SHIFT)
		{
			NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
			Sync::MutexUsage mutUsage(this->imgMut);
			i = this->imgMap.GetCount();
			while (i-- > 0)
			{
				status = this->imgMap.GetItemNoCheck(i);
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
			if (j >= this->imgMap.GetCount())
			{
				j = this->imgMap.GetCount() - 1;
			}
			while (i <= j)
			{
				status = this->imgMap.GetItemNoCheck(i);
				status->setting.flags |= 1;
				i++;
			}
			mutUsage.EndUse();
			this->Redraw();
		}
		else
		{
			NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
			Optional<SSWR::AVIRead::AVIRImageControl::ImageStatus> clkStatus;
			Sync::MutexUsage mutUsage(this->imgMut);
			i = this->imgMap.GetCount();
			while (i-- > 0)
			{
				status = this->imgMap.GetItemNoCheck(i);
				status->setting.flags &= ~1;
			}
			clkStatus = 0;
			if (this->imgMap.GetItem((UOSInt)clickIndex).SetTo(status))
			{
				status->setting.flags |= 1;
				this->currSel = clickIndex;
				clkStatus = status;
			}
			mutUsage.EndUse();
			this->Redraw();
			if (this->dispImg != clkStatus)
			{
				this->dispImg = clkStatus;
				if (this->dispHdlr)
				{
					if (clkStatus.SetTo(status))
					{
						this->dispHdlr(this->dispHdlrObj, status->fileName, status->setting);
					}
					else
					{
						this->dispHdlr(this->dispHdlrObj, nullptr, 0);
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
	NN<Text::String> sfolderPath;
	Bool loading = this->IsLoadingDir();
	ThreadCancelTasks();
	if (this->folderPath.SetTo(sfolderPath))
	{
		if (!loading)
		{
			this->SaveSetting();
		}
		Sync::MutexUsage mutUsage(this->folderMut);
		this->EndFolder();
		sfolderPath->Release();
		this->folderPath = 0;
	}
	Text::CStringNN nnfolderPath;
	if (folderPath.SetTo(nnfolderPath) && nnfolderPath.leng > 0)
	{
		Sync::MutexUsage mutUsage(this->folderMut);
		this->folderPath = Text::String::New(nnfolderPath);
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

Optional<Text::String> SSWR::AVIRead::AVIRImageControl::GetFolder()
{
	return this->folderPath;
}

Bool SSWR::AVIRead::AVIRImageControl::SaveSetting()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
	NN<Text::String> folderPath;
	if (!this->folderPath.SetTo(folderPath))
	{
		return false;
	}

	Text::StringBuilderUTF8 sb;
	sptr = folderPath->ConcatTo(sbuff);
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
	i = 0;
	j = this->imgMap.GetCount();
	while (i < j)
	{
		status = this->imgMap.GetItemNoCheck(i);
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
		writer.WriteLine(sb.ToCString());
		i++;
	}
	return true;
}

void SSWR::AVIRead::AVIRImageControl::SetDispImageHandler(DispImageChanged hdlr, AnyType userObj)
{
	this->dispHdlr = hdlr;
	this->dispHdlrObj = userObj;
}

void SSWR::AVIRead::AVIRImageControl::SetProgressHandler(ProgressUpdated hdlr, AnyType userObj)
{
	this->progHdlr = hdlr;
	this->progHdlrObj = userObj;
}

Optional<Media::StaticImage> SSWR::AVIRead::AVIRImageControl::LoadImage(Text::CStringNN fileName)
{
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
	Optional<Media::StaticImage> outImg = 0;
	Optional<Media::ImageList> imgList = 0;
	NN<Media::ImageList> nnimgList;
	NN<Media::RasterImage> img;

	Sync::MutexUsage mutUsage(this->imgMut);
	if (this->imgMap.GetC(fileName).SetTo(status))
	{
		Sync::MutexUsage ioMutUsage(this->ioMut);
		IO::StmData::FileData fd(status->filePath, false);
		imgList = Optional<Media::ImageList>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList));
		ioMutUsage.EndUse();
	}
	mutUsage.EndUse();

	if (imgList.SetTo(nnimgList))
	{
		if (nnimgList->GetImage(0, 0).SetTo(img))
		{
			outImg = img->CreateStaticImage().Ptr();
		}
		nnimgList.Delete();
	}

	if (status->setting.cropEnabled)
	{
		//////////////////////////
	}
	return outImg;
}

Optional<Media::StaticImage> SSWR::AVIRead::AVIRImageControl::LoadOriImage(Text::CStringNN fileName)
{
	NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> status;
	Optional<Media::StaticImage> outImg = 0;
	Optional<Media::ImageList> imgList = 0;
	NN<Media::ImageList> nnimgList;
	NN<Media::RasterImage> img;

	Sync::MutexUsage mutUsage(this->imgMut);
	if (this->imgMap.GetC(fileName).SetTo(status))
	{
		Sync::MutexUsage ioMutUsage(this->ioMut);
		IO::StmData::FileData fd(status->filePath, false);
		imgList = Optional<Media::ImageList>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList));
		ioMutUsage.EndUse();
	}
	mutUsage.EndUse();

	if (imgList.SetTo(nnimgList))
	{
		if (nnimgList->GetImage(0, 0).SetTo(img))
		{
			outImg = img->CreateStaticImage().Ptr();
		}
		nnimgList.Delete();
	}
	return outImg;
}

void SSWR::AVIRead::AVIRImageControl::ApplySetting(NN<Media::StaticImage> srcImg, NN<Media::StaticImage> destImg, NN<SSWR::AVIRead::AVIRImageControl::ImageSetting> setting)
{
	UnsafeArrayOpt<Double> gammaParam;
	UInt32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	NN<Media::EXIFData> exif;
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

void SSWR::AVIRead::AVIRImageControl::UpdateImgPreview(NN<SSWR::AVIRead::AVIRImageControl::ImageStatus> img)
{
	NN<Media::DrawImage> srcImg;
	NN<Media::DrawImage> destImg;
	if (!img->previewImg.SetTo(srcImg) || !img->previewImg2.SetTo(destImg))
		return;
	UOSInt sWidth = srcImg->GetWidth();
	UOSInt sHeight = srcImg->GetHeight();
	UOSInt sbpl = srcImg->GetImgBpl();
	Bool srev;
	UnsafeArray<UInt8> sptr;
	UOSInt dWidth = destImg->GetWidth();
	UOSInt dHeight = destImg->GetHeight();
	UOSInt dbpl = destImg->GetImgBpl();
	Bool drev;
	UnsafeArray<UInt8> dptr;
	if (!srcImg->GetImgBits(srev).SetTo(sptr) || !destImg->GetImgBits(drev).SetTo(dptr))
		return;
	UInt8 *tmpBuff = MemAllocA(UInt8, sHeight * (UOSInt)sbpl);

	UnsafeArrayOpt<Double> gammaParam;
	UInt32 gammaCnt;
	Text::StringBuilderUTF8 sb;
	NN<Media::EXIFData> exif;
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

void SSWR::AVIRead::AVIRImageControl::UpdateImgSetting(NN<SSWR::AVIRead::AVIRImageControl::ImageSetting> setting)
{
	NN<ImageStatus> status;
	UOSInt i;
	Bool chg = false;
	Sync::MutexUsage mutUsage(this->imgMut);
	i = this->imgMap.GetCount();
	while (i-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(i);
		if (status->setting.flags & 1)
		{
			if (status->setting.brightness != setting->brightness || status->setting.contrast != setting->contrast || status->setting.gamma != setting->gamma || (status->setting.flags & 240) != setting->flags)
			{
				status->setting.brightness = setting->brightness;
				status->setting.contrast = setting->contrast;
				status->setting.gamma = setting->gamma;
				status->setting.flags = (status->setting.flags & ~240) | setting->flags;
				chg = true;
				if (status->previewImg.NotNull() && status->previewImg2.NotNull())
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
	NN<ImageStatus> status;
	NN<ImageStatus> status2;
	UOSInt i;
	UOSInt j;
	UOSInt cnt = 0;
	Sync::MutexUsage mutUsage(this->imgMut);
	j = this->imgMap.GetCount();
	i = 0;
	while (i < j)
	{
		status = this->imgMap.GetItemNoCheck(i);
		if (status->setting.flags & 1)
		{
			status2 = MemAllocNN(ImageStatus);
			status2.CopyFrom(status);
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
	NN<ImageStatus> status;
	OSInt i;
	UOSInt j;
	if (this->folderPath.IsNull())
		return;

	Sync::MutexUsage mutUsage(this->imgMut);
	if (this->dispImg.SetTo(status))
	{
		i = this->imgMap.IndexOfC(status->fileName);
		if (i == -1)
		{
			i = (OSInt)this->imgMap.GetCount() - 1;
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
		i = (OSInt)this->imgMap.GetCount() - 1;
	}
	this->currSel = (UOSInt)i;
	j = this->imgMap.GetCount();
	while (j-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(j);
		status->setting.flags &= ~1;
	}

	if (i < 0)
	{
		this->dispImg = 0;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, nullptr, 0);
		}
	}
	else
	{
		this->dispImg = status = this->imgMap.GetItemNoCheck((UOSInt)i);
		status->setting.flags |= 1;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, status->fileName, &status->setting);
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
	NN<ImageStatus> status;
	OSInt i;
	UOSInt j;
	if (this->folderPath.IsNull())
		return;

	Sync::MutexUsage mutUsage(this->imgMut);
	if (this->dispImg.SetTo(status))
	{
		i = this->imgMap.IndexOfC(status->fileName);
		if (i == -1)
		{
			i = 0;
		}
		else if ((UOSInt)(i + 1) >= this->imgMap.GetCount())
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
	j = this->imgMap.GetCount();
	while (j-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(j);
		status->setting.flags &= ~1;
	}

	if (i < 0)
	{
		this->dispImg = 0;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, nullptr, 0);
		}
	}
	else
	{
		this->dispImg = status = this->imgMap.GetItemNoCheck((UOSInt)i);
		status->setting.flags |= 1;
		if (this->dispHdlr)
		{
			this->dispHdlr(this->dispHdlrObj, status->fileName, status->setting);
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
	UOSInt i = this->imgMap.GetCount();
	NN<ImageStatus> status;
	while (i-- > 0)
	{
		status = this->imgMap.GetItemNoCheck(i);;
		status->setting.flags |= 1;
	}
	mutUsage.EndUse();
	this->Redraw();
}

void SSWR::AVIRead::AVIRImageControl::HandleKeyDown(KeyDownHandler keyHdlr, AnyType keyObj)
{
	this->keyObj = keyObj;
	this->keyHdlr = keyHdlr;
}
