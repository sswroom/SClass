#include "Stdafx.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebPhotoController.h"
#include "Text/UTF8Reader.h"

Bool __stdcall SSWR::OrganWeb::OrganWebPhotoController::SvcPhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent)
{
	SSWR::OrganWeb::OrganWebPhotoController *me = (SSWR::OrganWeb::OrganWebPhotoController*)parent;
	RequestEnv env;
	me->ParseRequestEnv(req, resp, &env, false);

	UTF8Char sbuff[512];
	Int32 cateId;
	UInt32 width;
	UInt32 height;
	Int32 spId;
	Int32 id;
	if (req->GetQueryValueI32(CSTR("id"), &spId) &&
		req->GetQueryValueI32(CSTR("cateId"), &cateId) &&
		req->GetQueryValueU32(CSTR("width"), &width) &&
		req->GetQueryValueU32(CSTR("height"), &height) &&
		spId > 0 && width > 0 && height > 0 && cateId > 0 && width <= 10000 && height <= 10000
		)
	{
		if (req->GetQueryValueI32(CSTR("fileId"), &id))
		{
			me->ResponsePhotoId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueI32(CSTR("fileWId"), &id))
		{
			me->ResponsePhotoWId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueStr(CSTR("file"), sbuff, 512))
		{
			me->ResponsePhoto(req, resp, env.user, env.isMobile, spId, cateId, width, height, sbuff);
			return true;
		}
	}
	resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	return true;
}

void SSWR::OrganWeb::OrganWebPhotoController::ResponsePhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName)
{
	CategoryInfo *cate;
	SpeciesInfo *sp;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr2;
	Int32 rotateType = 0;
	UTF8Char *sptr;
	UTF8Char *sptrEnd = sbuff;
	Sync::RWMutexUsage mutUsage;
	sp = this->env->SpeciesGet(&mutUsage, speciesId);
	Bool notAdmin = (user == 0 || user->userType != 0);
	if (sp && sp->cateId == cateId)
	{
		cate = this->env->CateGet(&mutUsage, sp->cateId);
		if (cate && ((cate->flags & 1) == 0 || !notAdmin))
		{
			Text::StringBuilderUTF8 sb;
			Text::String *cacheDir = this->env->GetCacheDir();
			if (cacheDir && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
			{
				sptr = cacheDir->ConcatTo(sbuff);
				sptr2 = Text::StrInt32(sbuff2, cate->cateId);
				sptr = IO::Path::AppendPath(sbuff, sptr, CSTRP(sbuff2, sptr2));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = sp->dirName->ConcatTo(sptr);
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				if (Text::StrStartsWith(fileName, (const UTF8Char*)"web") && (fileName[3] == IO::Path::PATH_SEPERATOR || fileName[3] == '\\'))
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC("web"));
					IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptrEnd = Text::StrConcatC(Text::StrConcat(sptr, &fileName[4]), UTF8STRC(".jpg"));
				}
				else
				{
					sptrEnd = Text::StrConcatC(Text::StrConcat(sptr, fileName), UTF8STRC(".jpg"));
				}

				IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (fs.IsError())
				{
				}
				else
				{
					UInt8 *buff;
					UOSInt buffSize = (UOSInt)fs.GetLength();
					if (buffSize > 0)
					{
						buff = MemAlloc(UInt8, buffSize);
						fs.Read(buff, buffSize);
						resp->AddDefHeaders(req);
						resp->AddContentLength(buffSize);
						resp->AddContentType(CSTR("image/jpeg"));
						resp->Write(buff, buffSize);
						mutUsage.EndUse();
						MemFree(buff);
						return;
					}
					else
					{
					}
				}
			}

			{
				sb.ClearStr();
				sb.Append(cate->srcDir);
				sb.Append(sp->dirName);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				sb.AppendC(UTF8STRC("setting.txt"));
				IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
				if (!fs.IsError())
				{
					Text::PString sarr[3];
					sb.ClearStr();
					sb.AppendSlow(fileName);
					sb.AppendC(UTF8STRC("."));

					Text::UTF8Reader reader(&fs);
					while ((sptr2 = reader.ReadLine(sbuff2, 511)) != 0)
					{
						if (Text::StrSplitP(sarr, 3, {sbuff2, (UOSInt)(sptr2 - sbuff2)}, '\t') == 2)
						{
							if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, sb.ToString(), sb.GetLength()))
							{
								if (sarr[1].v[0] == 'R')
								{
									rotateType = Text::StrToInt32(&sarr[1].v[1]);
									break;
								}
							}
						}
					}
				}
			}

			sb.ClearStr();
			sb.Append(cate->srcDir);
			sb.Append(sp->dirName);
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.AppendSlow(fileName);
			if (IO::Path::PATH_SEPERATOR == '/')
			{
				sb.Replace('\\', '/');
			}
			sb.AppendC(UTF8STRC(".jpg"));
			mutUsage.EndUse();
			if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::File)
			{
				sb.RemoveChars(4);
				sb.AppendC(UTF8STRC(".pcx"));
				if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::File)
				{
					sb.RemoveChars(4);
					sb.AppendC(UTF8STRC(".tif"));
					if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::File)
					{
						sb.RemoveChars(4);
						sb.AppendC(UTF8STRC(".png"));
					}
				}
			}

			Media::ImageList *imgList;
			Media::StaticImage *simg;
			Media::StaticImage *lrimg;
			Media::StaticImage *dimg;
			{
				IO::StmData::FileData fd(sb.ToCString(), false);
				imgList = (Media::ImageList*)this->env->ParseFileType(&fd, IO::ParserType::ImageList);
			}
			if (imgList)
			{
				simg = imgList->GetImage(0, 0)->CreateStaticImage();
				DEL_CLASS(imgList);
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				Sync::MutexUsage mutUsage(&this->csconvMut);
				if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
				{
					SDEL_CLASS(this->csconv);
					this->csconvFCC = simg->info.fourcc;
					this->csconvBpp = simg->info.storeBPP;
					this->csconvPF = simg->info.pf;
					this->csconvColor.Set(simg->info.color);
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess());
				}
				if (this->csconv)
				{
					this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				}
				else
				{
					SDEL_CLASS(lrimg);
				}
				mutUsage.EndUse();
				DEL_CLASS(simg);

				if (lrimg)
				{
					this->lrgbLimiter.LimitImageLRGB(lrimg->data, lrimg->info.dispWidth, lrimg->info.dispHeight);
					Sync::MutexUsage mutUsage(&this->resizerMut);
					resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
					resizerLR->SetTargetWidth(imgWidth);
					resizerLR->SetTargetHeight(imgHeight);
					dimg = resizerLR->ProcessToNew(lrimg);
					mutUsage.EndUse();
					DEL_CLASS(lrimg)
				}
				else
				{
					dimg = 0;
				}
				if (dimg)
				{
					UInt8 *buff;
					UOSInt buffSize;
					dimg->info.color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());
					if (rotateType == 1)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW90);
					}
					else if (rotateType == 2)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW180);
					}
					else if (rotateType == 3)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW270);
					}

					if (false)//this->watermark && !Text::StrStartsWith(fileName, (const UTF8Char*)"web") && fileName[3] != IO::Path::PATH_SEPERATOR && fileName[3] != '\\')
					{
/*						Int32 xRand;
						Int32 yRand;
						Int16 fontSize = imgWidth / 12;
						OSInt leng = this->watermark->leng;
						Double sz[2];
						Int32 iWidth;
						Int32 iHeight;
						Media::DrawImage *gimg = (Media::DrawImage*)this->eng->ConvImage(dimg);
						Media::DrawImage *gimg2;
						Media::DrawBrush *b = gimg->NewBrushARGB(0xffffffff);
						Media::DrawFont *f;
						while (true)
						{
							f = gimg->NewFontW(L"Arial", fontSize, Media::DrawEngine::DFS_NORMAL);
							if (!gimg->GetTextSize(f, this->watermark, leng, sz))
							{
								gimg->DelFont(f);
								break;
							}
							if (sz[0] <= dimg->info.dispWidth && sz[1] <= dimg->info.dispHeight)
							{
								xRand = Double2Int32(dimg->info.dispWidth - sz[0]);
								yRand = Double2Int32(dimg->info.dispHeight - sz[1]);
								iWidth = Double2Int32(sz[0]);
								iHeight = Double2Int32(sz[1]);
								gimg2 = this->eng->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
								gimg2->DrawString(0, 0, this->watermark, f, b);
								gimg2->SetAlphaType(Media::AT_ALPHA);
								{
									Bool revOrder;
									UInt8 *bits = gimg2->GetImgBits(&revOrder);
									Int32 col = (this->random->NextInt30() & 0xffffff) | 0x5f808080;
									ImageUtil_ColorReplace32(bits, iWidth, iHeight, col);
								}
								gimg->DrawImagePt(gimg2, Double2Int32(this->random->NextDouble() * xRand), Double2Int32(this->random->NextDouble() * yRand));
								this->eng->DeleteImage(gimg2);
								gimg->DelFont(f);
								break;

							}
							else
							{
								gimg->DelFont(f);
								fontSize--;
							}
						}
						gimg->DelBrush(b);
						NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("SSWR::OrganWeb::OrganWebHandler.WebRequest"));
						gimg->SaveJPG(mstm);
						buff = mstm->GetBuff(&buffSize);
						resp->AddDefHeaders(req);
						resp->AddContentLength(buffSize);
						resp->AddContentType((const UTF8Char*)"image/jpeg"));
						resp->Write(buff, buffSize);

						if (this->cacheDir && imgWidth == PREVIEW_SIZE && imgHeight == PREVIEW_SIZE && buffSize > 0)
						{
							IO::FileStream fs(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							fs.Write(buff, buffSize);
						}

						DEL_CLASS(mstm);
						DEL_CLASS(dimg);
						this->eng->DeleteImage(gimg);*/
					}
					else
					{
						void *param;
						Media::ImageList nimgList(CSTR("Temp"));
						IO::MemoryStream mstm;
						nimgList.AddImage(dimg, 0);
						Exporter::GUIJPGExporter exporter;
						param = exporter.CreateParam(&nimgList);
						exporter.SetParamInt32(param, 0, 95);
						exporter.ExportFile(&mstm, CSTR(""), &nimgList, param);
						exporter.DeleteParam(param);
						ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

						if (cacheDir && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize() && mstm.GetLength() > 0)
						{
							IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							buff = mstm.GetBuff(&buffSize);
							fs.Write(buff, buffSize);
						}
					}
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
					return;
				}
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				return;
			}
		}
		else
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		mutUsage.EndUse();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganWeb::OrganWebPhotoController::ResponsePhotoId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileId)
{
	SpeciesInfo *sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UserFileInfo *userFile;
	Int32 rotateType = 0;
	Sync::RWMutexUsage mutUsage;
	sp = this->env->SpeciesGet(&mutUsage, speciesId);
	userFile = this->env->UserfileGet(&mutUsage, fileId);
	if (sp && sp->cateId == cateId && userFile && (userFile->fileType == 1 || userFile->fileType == 3))
	{
		if (sp->photoId == fileId && userFile->speciesId != sp->speciesId)
		{
			this->env->SpeciesUpdateDefPhoto(&mutUsage, sp->speciesId);
		}

		Text::String *cacheDir = this->env->GetCacheDir();
		Data::DateTime dt;
		WebUserInfo *user;
		user = this->env->UserGet(&mutUsage, userFile->webuserId);
		dt.SetTicks(userFile->fileTimeTicks);
		dt.ToUTCTime();
		rotateType = userFile->rotType;

		sptr = cacheDir->ConcatTo(sbuff2);
		sptr = IO::Path::AppendPath(sbuff2, sptr, CSTR("UserFile"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt32(sptr, userFile->webuserId);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMM");
		IO::Path::CreateDirectory(CSTRP(sbuff2, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr2 = userFile->dataFileName->ConcatTo(sptr);

		if (imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize() && userFile->prevUpdated == 0)
		{
			Data::DateTime dt2;
			Data::DateTime dt3;
			if (req->GetIfModifiedSince(&dt2) && IO::Path::GetFileTime(sbuff2, &dt3, 0, 0))
			{
				Int64 tdiff = dt2.ToTicks() - dt3.ToTicks();
				if (tdiff >= -1000 && tdiff <= 1000)
				{
					mutUsage.EndUse();
					resp->ResponseNotModified(req, -1);
					return;
				}
			}
			IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			UOSInt buffSize = (UOSInt)fs.GetLength();
			if (fs.IsError() || buffSize == 0)
			{
			}
			else
			{
				UInt8 *buff;
				buff = MemAlloc(UInt8, buffSize);
				fs.Read(buff, buffSize);
				fs.GetFileTimes(0, 0, &dt2);
				resp->AddDefHeaders(req);
				resp->AddContentLength(buffSize);
				resp->AddContentType(CSTR("image/jpeg"));
				resp->AddLastModified(&dt2);
				resp->Write(buff, buffSize);
				mutUsage.EndUse();
				MemFree(buff);
				return;
			}
		}

		sptr = this->env->UserfileGetPath(sbuff, userFile);
		mutUsage.EndUse();

		Media::ImageList *imgList;
		Media::StaticImage *simg;
		Media::StaticImage *lrimg;
		Media::StaticImage *dimg;
		{
			IO::StmData::FileData fd({sbuff, (UOSInt)(sptr - sbuff)}, false);
			imgList = (Media::ImageList*)this->env->ParseFileType(&fd, IO::ParserType::ImageList);
		}
		if (imgList)
		{
			simg = imgList->GetImage(0, 0)->CreateStaticImage();
			DEL_CLASS(imgList);
			Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
			NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			{
				Sync::MutexUsage mutUsage(&this->csconvMut);
				if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
				{
					SDEL_CLASS(this->csconv);
					this->csconvFCC = simg->info.fourcc;
					this->csconvBpp = simg->info.storeBPP;
					this->csconvPF = simg->info.pf;
					this->csconvColor.Set(simg->info.color);
					this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess());
				}
				if (this->csconv)
				{
					this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
				}
				else
				{
					SDEL_CLASS(lrimg);
				}
			}
			DEL_CLASS(simg);

			if (lrimg)
			{
				this->lrgbLimiter.LimitImageLRGB(lrimg->data, lrimg->info.dispWidth, lrimg->info.dispHeight);
				if (imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
				{
					Sync::MutexUsage mutUsage(&this->resizerMut);
					resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
					resizerLR->SetTargetWidth(imgWidth);
					resizerLR->SetTargetHeight(imgHeight);
					Double x1 = userFile->cropLeft;
					Double y1 = userFile->cropTop;
					Double x2 = UOSInt2Double(lrimg->info.dispWidth) - userFile->cropRight;
					Double y2 = UOSInt2Double(lrimg->info.dispHeight) - userFile->cropBottom;
					if (userFile->cropLeft < 0)
					{
						x1 = 0;
						x2 = UOSInt2Double(lrimg->info.dispWidth) - userFile->cropRight - userFile->cropLeft;
					}
					else if (userFile->cropRight < 0)
					{
						x1 = userFile->cropLeft + userFile->cropRight;
						x2 = UOSInt2Double(lrimg->info.dispWidth);
					}
					if (userFile->cropTop < 0)
					{
						y1 = 0;
						y2 = UOSInt2Double(lrimg->info.dispHeight) - userFile->cropBottom - userFile->cropTop;
					}
					else if (userFile->cropBottom < 0)
					{
						y1 = userFile->cropBottom + userFile->cropTop;
						y2 = UOSInt2Double(lrimg->info.dispHeight);
					}
					dimg = resizerLR->ProcessToNewPartial(lrimg, x1, y1, x2, y2);
					mutUsage.EndUse();
				}
				else
				{
					Sync::MutexUsage mutUsage(&this->resizerMut);
					resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
					resizerLR->SetTargetWidth(imgWidth);
					resizerLR->SetTargetHeight(imgHeight);
					dimg = resizerLR->ProcessToNew(lrimg);
					mutUsage.EndUse();
				}
				DEL_CLASS(lrimg)
			}
			else
			{
				dimg = 0;
			}
			if (dimg)
			{
				UInt8 *buff;
				UOSInt buffSize;
				dimg->info.color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());

				if (rotateType == 1)
				{
					dimg->RotateImage(Media::StaticImage::RotateType::CW90);
				}
				else if (rotateType == 2)
				{
					dimg->RotateImage(Media::StaticImage::RotateType::CW180);
				}
				else if (rotateType == 3)
				{
					dimg->RotateImage(Media::StaticImage::RotateType::CW270);
				}

				if (user && user->watermark)
				{
					Media::DrawImage *gimg = this->env->GetDrawEngine()->ConvImage(dimg);
					if ((cacheDir && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize()) || user != reqUser)
					{
						Int32 xRand;
						Int32 yRand;
						UInt32 fontSizePx = imgWidth / 12;
						Math::Size2D<Double> sz;
						UInt32 iWidth;
						UInt32 iHeight;
						Media::DrawImage *gimg2;
						Media::DrawBrush *b = gimg->NewBrushARGB(0xffffffff);
						Media::DrawFont *f;
						while (true)
						{
							f = gimg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
							sz = gimg->GetTextSize(f, user->watermark->ToCString());
							if (!sz.HasArea())
							{
								gimg->DelFont(f);
								break;
							}
							if (sz.width <= UOSInt2Double(dimg->info.dispWidth) && sz.height <= UOSInt2Double(dimg->info.dispHeight))
							{
								xRand = Double2Int32(UOSInt2Double(dimg->info.dispWidth) - sz.width);
								yRand = Double2Int32(UOSInt2Double(dimg->info.dispHeight) - sz.height);
								iWidth = (UInt32)Double2Int32(sz.width);
								iHeight = (UInt32)Double2Int32(sz.height);
								gimg2 = this->env->GetDrawEngine()->CreateImage32(iWidth, iHeight, Media::AT_NO_ALPHA);
								gimg2->DrawString(0, 0, user->watermark->ToCString(), f, b);
								gimg2->SetAlphaType(Media::AT_ALPHA);
								{
									Bool revOrder;
									UInt8 *bits = gimg2->GetImgBits(&revOrder);
									UInt32 col = (this->random.NextInt30() & 0xffffff) | 0x5f808080;
									if (bits)
									{
										ImageUtil_ColorReplace32(bits, iWidth, iHeight, col);
									}
								}
								gimg->DrawImagePt(gimg2, Double2Int32(this->random.NextDouble() * xRand), Double2Int32(this->random.NextDouble() * yRand));
								this->env->GetDrawEngine()->DeleteImage(gimg2);
								gimg->DelFont(f);
								break;

							}
							else
							{
								gimg->DelFont(f);
								fontSizePx--;
							}
						}
						gimg->DelBrush(b);
					}

					IO::MemoryStream mstm;
					gimg->SaveJPG(&mstm);
					ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

					if (cacheDir && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (userFile->prevUpdated)
						{
							this->env->UserFilePrevUpdated(&mutUsage, userFile);
						}
					}

					DEL_CLASS(dimg);
					this->env->GetDrawEngine()->DeleteImage(gimg);
				}
				else
				{
					void *param;
					Media::ImageList nimgList(CSTR("Temp"));
					IO::MemoryStream mstm;
					nimgList.AddImage(dimg, 0);
					Exporter::GUIJPGExporter exporter;
					param = exporter.CreateParam(&nimgList);
					exporter.SetParamInt32(param, 0, 95);
					exporter.ExportFile(&mstm, CSTR(""), &nimgList, param);
					exporter.DeleteParam(param);
					ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

					if (cacheDir && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (userFile->prevUpdated)
						{
							this->env->UserFilePrevUpdated(&mutUsage, userFile);
						}
					}
				}
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				return;
			}
		}
		else
		{
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		mutUsage.EndUse();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

void SSWR::OrganWeb::OrganWebPhotoController::ResponsePhotoWId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileWId)
{
	SpeciesInfo *sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	WebFileInfo *wfile;
	Int32 rotateType = 0;
	Sync::RWMutexUsage mutUsage;
	sp = this->env->SpeciesGet(&mutUsage, speciesId);
	if (sp && sp->cateId == cateId)
	{
		wfile = sp->wfiles.Get(fileWId);
		if (wfile)
		{
			Data::DateTime dt;
			Text::String *cacheDir = this->env->GetCacheDir();
			sptr = cacheDir->ConcatTo(sbuff2);
			sptr = IO::Path::AppendPath(sbuff2, sptr, CSTR("WebFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id >> 10);
			IO::Path::CreateDirectory(CSTRP(sbuff2, sptr));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id);
			sptr2 = Text::StrConcatC(sptr, UTF8STRC(".jpg"));

			if (imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize() && wfile->prevUpdated == 0)
			{
				IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				UOSInt buffSize = (UOSInt)fs.GetLength();
				if (fs.IsError() || buffSize == 0)
				{
				}
				else
				{
					UInt8 *buff;
					buff = MemAlloc(UInt8, buffSize);
					fs.Read(buff, buffSize);
					resp->AddDefHeaders(req);
					resp->AddContentLength(buffSize);
					resp->AddContentType(CSTR("image/jpeg"));
					resp->Write(buff, buffSize);
					mutUsage.EndUse();
					MemFree(buff);
					return;
				}
			}

			sptr = this->env->GetDataDir()->ConcatTo(sbuff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("WebFile"));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id >> 10);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrInt32(sptr, wfile->id);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
			mutUsage.EndUse();;

			Media::ImageList *imgList;
			Media::StaticImage *simg;
			Media::StaticImage *lrimg;
			Media::StaticImage *dimg;
			{
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				imgList = (Media::ImageList*)this->env->ParseFileType(&fd, IO::ParserType::ImageList);
			}
			if (imgList)
			{
				simg = imgList->GetImage(0, 0)->CreateStaticImage();
				DEL_CLASS(imgList);
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASS(lrimg, Media::StaticImage(simg->info.dispWidth, simg->info.dispHeight, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				{
					Sync::MutexUsage mutUsage(&this->csconvMut);
					if (this->csconv == 0 || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color->Equals(&this->csconvColor))
					{
						SDEL_CLASS(this->csconv);
						this->csconvFCC = simg->info.fourcc;
						this->csconvBpp = simg->info.storeBPP;
						this->csconvPF = simg->info.pf;
						this->csconvColor.Set(simg->info.color);
						this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, &this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, &color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess());
					}
					if (this->csconv)
					{
						this->csconv->ConvertV2(&simg->data, lrimg->data, simg->info.dispWidth, simg->info.dispHeight, simg->info.storeWidth, simg->info.storeHeight, (OSInt)lrimg->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
					}
					else
					{
						SDEL_CLASS(lrimg);
					}
				}
				DEL_CLASS(simg);

				if (lrimg)
				{
					this->lrgbLimiter.LimitImageLRGB(lrimg->data, lrimg->info.dispWidth, lrimg->info.dispHeight);
					if (imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
					{
						Sync::MutexUsage mutUsage(&this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetWidth(imgWidth);
						resizerLR->SetTargetHeight(imgHeight);
						Double x1 = wfile->cropLeft;
						Double y1 = wfile->cropTop;
						Double x2 = UOSInt2Double(lrimg->info.dispWidth) - wfile->cropRight;
						Double y2 = UOSInt2Double(lrimg->info.dispHeight) - wfile->cropBottom;
						if (wfile->cropLeft < 0)
						{
							x1 = 0;
							x2 = UOSInt2Double(lrimg->info.dispWidth) - wfile->cropRight - wfile->cropLeft;
						}
						else if (wfile->cropRight < 0)
						{
							x1 = wfile->cropLeft + wfile->cropRight;
							x2 = UOSInt2Double(lrimg->info.dispWidth);
						}
						if (wfile->cropTop < 0)
						{
							y1 = 0;
							y2 = UOSInt2Double(lrimg->info.dispHeight) - wfile->cropBottom - wfile->cropTop;
						}
						else if (wfile->cropBottom < 0)
						{
							y1 = wfile->cropBottom + wfile->cropTop;
							y2 = UOSInt2Double(lrimg->info.dispHeight);
						}
						dimg = resizerLR->ProcessToNewPartial(lrimg, x1, y1, x2, y2);
						mutUsage.EndUse();
					}
					else
					{
						Sync::MutexUsage mutUsage(&this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetWidth(imgWidth);
						resizerLR->SetTargetHeight(imgHeight);
						dimg = resizerLR->ProcessToNew(lrimg);
						mutUsage.EndUse();
					}
					DEL_CLASS(lrimg)
				}
				else
				{
					dimg = 0;
				}
				if (dimg)
				{
					UInt8 *buff;
					UOSInt buffSize;
					dimg->info.color->SetRAWICC(Media::ICCProfile::GetSRGBICCData());

					if (rotateType == 1)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW90);
					}
					else if (rotateType == 2)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW180);
					}
					else if (rotateType == 3)
					{
						dimg->RotateImage(Media::StaticImage::RotateType::CW270);
					}

					void *param;
					Media::ImageList nimgList(CSTR("Temp"));
					IO::MemoryStream mstm;
					nimgList.AddImage(dimg, 0);
					Exporter::GUIJPGExporter exporter;
					param = exporter.CreateParam(&nimgList);
					exporter.SetParamInt32(param, 0, 95);
					exporter.ExportFile(&mstm, CSTR(""), &nimgList, param);
					exporter.DeleteParam(param);
					ResponseMstm(req, resp, &mstm, CSTR("image/jpeg"));

					if (cacheDir && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
					{
						IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						buff = mstm.GetBuff(&buffSize);
						fs.Write(buff, buffSize);
						if (wfile->prevUpdated)
						{
							this->env->WebFilePrevUpdated(&mutUsage, wfile);
						}
					}
				}
				else
				{
					resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
					return;
				}
			}
			else
			{
				resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
				return;
			}
		}
		else
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return;
		}
	}
	else
	{
		mutUsage.EndUse();
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return;
	}
}

SSWR::OrganWeb::OrganWebPhotoController::OrganWebPhotoController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize) : OrganWebController(sessMgr, env, scnSize), csconvColor(Media::ColorProfile::CPT_SRGB)
{
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizerLR, Media::Resizer::LanczosResizerLR_C32(3, 3, &destProfile, this->env->GetColorSess(), Media::AT_NO_ALPHA, 0, Media::PF_B8G8R8A8));
	this->csconv = 0;
	this->csconvFCC = 0;
	this->csconvBpp = 0;
	this->csconvPF = Media::PF_UNKNOWN;

	this->AddService(CSTR("/photo.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhoto);
}

SSWR::OrganWeb::OrganWebPhotoController::~OrganWebPhotoController()
{
	DEL_CLASS(this->resizerLR);
	SDEL_CLASS(this->csconv);
}