#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Exporter/GUIJPGExporter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Net/MIME.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"
#include "SSWR/OrganWeb/OrganWebPhotoController.h"
#include "Text/UTF8Reader.h"

Bool __stdcall SSWR::OrganWeb::OrganWebPhotoController::SvcPhoto(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPhotoController> me = NN<SSWR::OrganWeb::OrganWebPhotoController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	UTF8Char sbuff[512];
	Int32 cateId;
	UInt32 width;
	UInt32 height;
	Int32 spId;
	Int32 id;
	if (req->GetQueryValueI32(CSTR("id"), spId) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetQueryValueU32(CSTR("width"), width) &&
		req->GetQueryValueU32(CSTR("height"), height) &&
		spId > 0 && width > 0 && height > 0 && cateId > 0 && width <= 10000 && height <= 10000
		)
	{
		if (req->GetQueryValueI32(CSTR("fileId"), id))
		{
			me->ResponsePhotoId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueI32(CSTR("fileWId"), id))
		{
			me->ResponsePhotoWId(req, resp, env.user, env.isMobile, spId, cateId, width, height, id);
			return true;
		}
		else if (req->GetQueryValueStr(CSTR("file"), sbuff, 512).NotNull())
		{
			me->ResponsePhoto(req, resp, env.user, env.isMobile, spId, cateId, width, height, sbuff);
			return true;
		}
	}
	resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
	return true;
}

Bool __stdcall SSWR::OrganWeb::OrganWebPhotoController::SvcPhotoDown(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPhotoController> me = NN<SSWR::OrganWeb::OrganWebPhotoController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	Int32 spId;
	Int32 cateId;
	Int32 fileId;
	if (req->GetQueryValueI32(CSTR("id"), spId) &&
		req->GetQueryValueI32(CSTR("cateId"), cateId) &&
		req->GetQueryValueI32(CSTR("fileId"), fileId))
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		NN<UserFileInfo> userFile;
		Sync::RWMutexUsage mutUsage;
		sptr = sbuff;
		if (me->env->UserfileGetCheck(mutUsage, fileId, spId, cateId, env.user, sptr).SetTo(userFile))
		{
			UOSInt buffSize;
			IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
			
			buffSize = (UOSInt)fd.GetDataSize();
			Data::ByteBuffer buff(buffSize);
			fd.GetRealData(0, buffSize, buff);
			resp->AddDefHeaders(req);
			resp->AddContentDisposition(false, UnsafeArray<const UTF8Char>(userFile->oriFileName->v), req->GetBrowser());
			resp->AddContentLength(buffSize);
			if (userFile->fileType == FileType::Audio)
			{
				resp->AddContentType(CSTR("image/png"));
			}
			else
			{
				resp->AddContentType(Net::MIME::GetMIMEFromFileName(userFile->oriFileName->v, userFile->oriFileName->leng));
			}
			mutUsage.EndUse();
			resp->Write(buff);
			return true;
		}
		else
		{
			mutUsage.EndUse();
			resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
			return true;
		}
	}
	else
	{
		resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		return true;
	}
}

Bool __stdcall SSWR::OrganWeb::OrganWebPhotoController::SvcFavicon(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent)
{
	NN<SSWR::OrganWeb::OrganWebPhotoController> me = NN<SSWR::OrganWeb::OrganWebPhotoController>::ConvertFrom(parent);
	RequestEnv env;
	me->ParseRequestEnv(req, resp, env, false);

	resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	return true;
}

void SSWR::OrganWeb::OrganWebPhotoController::ResponsePhoto(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Optional<WebUserInfo> user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName)
{
	NN<CategoryInfo> cate;
	NN<SpeciesInfo> sp;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr2;
	Int32 rotateType = 0;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd = sbuff;
	Sync::RWMutexUsage mutUsage;
	NN<WebUserInfo> nnuser;
	Bool notAdmin = (!user.SetTo(nnuser) || nnuser->userType != UserType::Admin);
	if (this->env->SpeciesGet(mutUsage, speciesId).SetTo(sp) && sp->cateId == cateId)
	{
		if (this->env->CateGet(mutUsage, sp->cateId).SetTo(cate) && ((cate->flags & 1) == 0 || !notAdmin))
		{
			Text::StringBuilderUTF8 sb;
			NN<Text::String> cacheDir = Text::String::OrEmpty(this->env->GetCacheDir());
			if (cacheDir->leng > 0 && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
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
					UOSInt buffSize = (UOSInt)fs.GetLength();
					if (buffSize > 0)
					{
						Data::ByteBuffer buff(buffSize);
						fs.Read(buff);
						resp->AddDefHeaders(req);
						resp->AddContentLength(buffSize);
						resp->AddContentType(CSTR("image/jpeg"));
						resp->Write(buff);
						mutUsage.EndUse();
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

					Text::UTF8Reader reader(fs);
					while (reader.ReadLine(sbuff2, 511).SetTo(sptr2))
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

			Optional<Media::ImageList> optimgList;
			NN<Media::ImageList> imgList;
			NN<Media::RasterImage> rimg;
			NN<Media::StaticImage> simg;
			Optional<Media::StaticImage> lrimg;
			NN<Media::StaticImage> lrimgnn;
			Optional<Media::StaticImage> dimg;
			{
				IO::StmData::FileData fd(sb.ToCString(), false);
				optimgList = Optional<Media::ImageList>::ConvertFrom(this->env->ParseFileType(fd, IO::ParserType::ImageList));
			}
			if (optimgList.SetTo(imgList))
			{
				if (imgList->GetImage(0, 0).SetTo(rimg))
				{
					simg = rimg->CreateStaticImage();
					optimgList.Delete();
					Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
					NEW_CLASSNN(lrimgnn, Media::StaticImage(simg->info.dispSize, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
					lrimg = lrimgnn;
					Sync::MutexUsage mutUsage(this->csconvMut);
					if (this->csconv.IsNull() || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color.Equals(this->csconvColor))
					{
						this->csconv.Delete();
						this->csconvFCC = simg->info.fourcc;
						this->csconvBpp = simg->info.storeBPP;
						this->csconvPF = simg->info.pf;
						this->csconvColor.Set(simg->info.color);
						this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess().Ptr());
					}
					NN<Media::CS::CSConverter> csconv;
					if (this->csconv.SetTo(csconv))
					{
						csconv->ConvertV2(&simg->data, lrimgnn->data, simg->info.dispSize.x, simg->info.dispSize.y, simg->info.storeSize.x, simg->info.storeSize.y, (OSInt)lrimgnn->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
					}
					else
					{
						lrimg.Delete();
					}
					mutUsage.EndUse();
					simg.Delete();

					if (lrimg.SetTo(lrimgnn))
					{
						this->lrgbLimiter.LimitImageLRGB(lrimgnn->data, lrimgnn->info.dispSize.x, lrimgnn->info.dispSize.y);
						Sync::MutexUsage mutUsage(this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetSize(Math::Size2D<UOSInt>(imgWidth, imgHeight));
						dimg = resizerLR->ProcessToNew(lrimgnn);
						mutUsage.EndUse();
						lrimg.Delete();
					}
					else
					{
						dimg = 0;
					}
					if (dimg.SetTo(simg))
					{
						simg->info.color.SetRAWICC(Media::ICCProfile::GetSRGBICCData());
						if (rotateType == 1)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW90);
						}
						else if (rotateType == 2)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW180);
						}
						else if (rotateType == 3)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW270);
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
								if (sz[0] <= dimg->info.dispSize.x && sz[1] <= dimg->info.dispSize.y)
								{
									xRand = Double2Int32(dimg->info.dispSize.x - sz[0]);
									yRand = Double2Int32(dimg->info.dispSize.y - sz[1]);
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
							Optional<IO::FileExporter::ParamData> param;
							Media::ImageList nimgList(CSTR("Temp"));
							IO::MemoryStream mstm;
							nimgList.AddImage(simg, 0);
							Exporter::GUIJPGExporter exporter;
							param = exporter.CreateParam(nimgList);
							exporter.SetParamInt32(param, 0, 95);
							exporter.ExportFile(mstm, CSTR(""), nimgList, param);
							exporter.DeleteParam(param);
							ResponseMstm(req, resp, mstm, CSTR("image/jpeg"));

							if (cacheDir->leng > 0 && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize() && mstm.GetLength() > 0)
							{
								IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
								fs.Write(mstm.GetArray());
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
					optimgList.Delete();
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

void SSWR::OrganWeb::OrganWebPhotoController::ResponsePhotoId(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Optional<WebUserInfo> reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileId)
{
	NN<SpeciesInfo> sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<UserFileInfo> userFile;
	Int32 rotateType = 0;
	Sync::RWMutexUsage mutUsage;
	if (this->env->SpeciesGet(mutUsage, speciesId).SetTo(sp) && sp->cateId == cateId && this->env->UserfileGet(mutUsage, fileId).SetTo(userFile) && userFile->speciesId == speciesId && (userFile->fileType == FileType::Image || userFile->fileType == FileType::Audio))
	{
		if (sp->photoId == fileId && userFile->speciesId != sp->speciesId)
		{
			this->env->SpeciesUpdateDefPhoto(mutUsage, sp->speciesId);
		}

		NN<Text::String> cacheDir = Text::String::OrEmpty(this->env->GetCacheDir());
		Data::DateTime dt;
		Optional<WebUserInfo> user;
		NN<WebUserInfo> nnuser;
		user = this->env->UserGet(mutUsage, userFile->webuserId);
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
			Data::Timestamp dt3;
			if (req->GetIfModifiedSince(dt2) && IO::Path::GetFileTime(CSTRP(sbuff2, sptr2), dt3, 0, 0))
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
				Data::ByteBuffer buff(buffSize);
				fs.Read(buff);
				fs.GetFileTimes(0, 0, &dt2);
				resp->AddDefHeaders(req);
				resp->AddContentLength(buffSize);
				resp->AddContentType(CSTR("image/jpeg"));
				resp->AddLastModified(dt2);
				resp->Write(buff);
				mutUsage.EndUse();
				return;
			}
		}

		sptr = this->env->UserfileGetPath(sbuff, userFile);
		if (userFile->fileType == FileType::Audio)
		{
			UOSInt i = Text::StrLastIndexOfC(sbuff, (UOSInt)(sptr - sbuff), '.');
			sptr = Text::StrConcatC(&sbuff[i + 1], UTF8STRC("png"));
		}
		mutUsage.EndUse();

		NN<Media::ImageList> imgList;
		Optional<Media::ImageList> optimgList;
		NN<Media::RasterImage> rimg;
		NN<Media::StaticImage> simg;
		Optional<Media::StaticImage> lrimg;
		NN<Media::StaticImage> lrimgnn;
		Optional<Media::StaticImage> dimg;
		{
			IO::StmData::FileData fd({sbuff, (UOSInt)(sptr - sbuff)}, false);
			optimgList = Optional<Media::ImageList>::ConvertFrom(this->env->ParseFileType(fd, IO::ParserType::ImageList));
		}
		if (optimgList.SetTo(imgList))
		{
			if (imgList->GetImage(0, 0).SetTo(rimg))
			{
				simg = rimg->CreateStaticImage();
				optimgList.Delete();
				Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
				NEW_CLASSNN(lrimgnn, Media::StaticImage(simg->info.dispSize, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				lrimg = lrimgnn;
				{
					Sync::MutexUsage mutUsage(this->csconvMut);
					if (this->csconv.IsNull() || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color.Equals(this->csconvColor))
					{
						this->csconv.Delete();
						this->csconvFCC = simg->info.fourcc;
						this->csconvBpp = simg->info.storeBPP;
						this->csconvPF = simg->info.pf;
						this->csconvColor.Set(simg->info.color);
						this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess().Ptr());
					}
					NN<Media::CS::CSConverter> csconv;
					if (this->csconv.SetTo(csconv))
					{
						csconv->ConvertV2(&simg->data, lrimgnn->data, simg->info.dispSize.x, simg->info.dispSize.y, simg->info.storeSize.x, simg->info.storeSize.y, (OSInt)lrimgnn->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
					}
					else
					{
						lrimg.Delete();
					}
				}
				simg.Delete();

				if (lrimg.SetTo(lrimgnn))
				{
					this->lrgbLimiter.LimitImageLRGB(lrimgnn->data, lrimgnn->info.dispSize.x, lrimgnn->info.dispSize.y);
					if (imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
					{
						Sync::MutexUsage mutUsage(this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetSize(Math::Size2D<UOSInt>(imgWidth, imgHeight));
						Double x1 = userFile->cropLeft;
						Double y1 = userFile->cropTop;
						Double x2 = UOSInt2Double(lrimgnn->info.dispSize.x) - userFile->cropRight;
						Double y2 = UOSInt2Double(lrimgnn->info.dispSize.y) - userFile->cropBottom;
						if (userFile->cropLeft < 0)
						{
							x1 = 0;
							x2 = UOSInt2Double(lrimgnn->info.dispSize.x) - userFile->cropRight - userFile->cropLeft;
						}
						else if (userFile->cropRight < 0)
						{
							x1 = userFile->cropLeft + userFile->cropRight;
							x2 = UOSInt2Double(lrimgnn->info.dispSize.x);
						}
						if (userFile->cropTop < 0)
						{
							y1 = 0;
							y2 = UOSInt2Double(lrimgnn->info.dispSize.y) - userFile->cropBottom - userFile->cropTop;
						}
						else if (userFile->cropBottom < 0)
						{
							y1 = userFile->cropBottom + userFile->cropTop;
							y2 = UOSInt2Double(lrimgnn->info.dispSize.y);
						}
						dimg = resizerLR->ProcessToNewPartial(lrimgnn, Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2));
						mutUsage.EndUse();
					}
					else
					{
						Sync::MutexUsage mutUsage(this->resizerMut);
						resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
						resizerLR->SetTargetSize(Math::Size2D<UOSInt>(imgWidth, imgHeight));
						dimg = resizerLR->ProcessToNew(lrimgnn);
						mutUsage.EndUse();
					}
					lrimg.Delete();
				}
				else
				{
					dimg = 0;
				}
				if (dimg.SetTo(simg))
				{
					UInt8 *buff;
					UOSInt buffSize;
					simg->info.color.SetRAWICC(Media::ICCProfile::GetSRGBICCData());

					if (rotateType == 1)
					{
						simg->RotateImage(Media::StaticImage::RotateType::CW90);
					}
					else if (rotateType == 2)
					{
						simg->RotateImage(Media::StaticImage::RotateType::CW180);
					}
					else if (rotateType == 3)
					{
						simg->RotateImage(Media::StaticImage::RotateType::CW270);
					}

					if (user.SetTo(nnuser) && nnuser->watermark->leng > 0)
					{
						NN<Media::DrawImage> gimg;
						if (this->env->GetDrawEngine()->ConvImage(simg).SetTo(gimg))
						{
							if ((imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize()) || user != reqUser)
							{
								Int32 xRand;
								Int32 yRand;
								UInt32 fontSizePx = imgWidth / 12;
								Math::Size2DDbl sz;
								UInt32 iWidth;
								UInt32 iHeight;
								NN<Media::DrawImage> gimg2;
								NN<Media::DrawBrush> b = gimg->NewBrushARGB(0xffffffff);
								NN<Media::DrawFont> f;
								while (true)
								{
									f = gimg->NewFontPx(CSTR("Arial"), fontSizePx, Media::DrawEngine::DFS_NORMAL, 0);
									sz = gimg->GetTextSize(f, nnuser->watermark->ToCString());
									if (!sz.HasArea())
									{
										gimg->DelFont(f);
										break;
									}
									if (sz.x <= UOSInt2Double(simg->info.dispSize.x) && sz.y <= UOSInt2Double(simg->info.dispSize.y))
									{
										xRand = Double2Int32(UOSInt2Double(simg->info.dispSize.x) - sz.x);
										yRand = Double2Int32(UOSInt2Double(simg->info.dispSize.y) - sz.y);
										iWidth = (UInt32)Double2Int32(sz.x);
										iHeight = (UInt32)Double2Int32(sz.y);
										if (this->env->GetDrawEngine()->CreateImage32(Math::Size2D<UOSInt>(iWidth, iHeight), Media::AT_NO_ALPHA).SetTo(gimg2))
										{
											gimg2->DrawString(Math::Coord2DDbl(0, 0), nnuser->watermark->ToCString(), f, b);
											gimg2->SetAlphaType(Media::AT_ALPHA);
											{
												Bool revOrder;
												UInt8 *bits = gimg2->GetImgBits(revOrder);
												UInt32 col = (this->random.NextInt30() & 0xffffff) | 0x5f808080;
												if (bits)
												{
													ImageUtil_ColorReplace32(bits, iWidth, iHeight, col);
												}
											}
											gimg->DrawImagePt(gimg2, Math::Coord2DDbl(this->random.NextDouble() * xRand, this->random.NextDouble() * yRand));
											this->env->GetDrawEngine()->DeleteImage(gimg2);
										}
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
							gimg->SaveJPG(mstm);
							ResponseMstm(req, resp, mstm, CSTR("image/jpeg"));

							if (cacheDir->leng > 0 && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
							{
								IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
								buff = mstm.GetBuff(buffSize);
								fs.Write(Data::ByteArrayR(buff, buffSize));
								if (userFile->prevUpdated)
								{
									this->env->UserFilePrevUpdated(mutUsage, userFile);
								}
							}

							dimg.Delete();
							this->env->GetDrawEngine()->DeleteImage(gimg);
						}
						else
						{
							resp->ResponseError(req, Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
						}
					}
					else
					{
						Optional<IO::FileExporter::ParamData> param;
						Media::ImageList nimgList(CSTR("Temp"));
						IO::MemoryStream mstm;
						nimgList.AddImage(simg, 0);
						Exporter::GUIJPGExporter exporter;
						param = exporter.CreateParam(nimgList);
						exporter.SetParamInt32(param, 0, 95);
						exporter.ExportFile(mstm, CSTR(""), nimgList, param);
						exporter.DeleteParam(param);
						ResponseMstm(req, resp, mstm, CSTR("image/jpeg"));

						if (cacheDir->leng > 0 && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
						{
							IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							buff = mstm.GetBuff(buffSize);
							fs.Write(Data::ByteArrayR(buff, buffSize));
							if (userFile->prevUpdated)
							{
								this->env->UserFilePrevUpdated(mutUsage, userFile);
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
				optimgList.Delete();
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

void SSWR::OrganWeb::OrganWebPhotoController::ResponsePhotoWId(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Optional<WebUserInfo> reqUser, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 fileWId)
{
	NN<SpeciesInfo> sp;
	UTF8Char sbuff2[512];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<WebFileInfo> wfile;
	Int32 rotateType = 0;
	Sync::RWMutexUsage mutUsage;
	if (this->env->SpeciesGet(mutUsage, speciesId).SetTo(sp) && sp->cateId == cateId)
	{
		if (sp->wfiles.Get(fileWId).SetTo(wfile))
		{
			Data::DateTime dt;
			NN<Text::String> cacheDir = Text::String::OrEmpty(this->env->GetCacheDir());
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
					Data::ByteBuffer buff(buffSize);
					fs.Read(buff);
					resp->AddDefHeaders(req);
					resp->AddContentLength(buffSize);
					resp->AddContentType(CSTR("image/jpeg"));
					resp->Write(buff);
					mutUsage.EndUse();
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

			Optional<Media::ImageList> optimgList;
			NN<Media::ImageList> imgList;
			NN<Media::RasterImage> rimg;
			NN<Media::StaticImage> simg;
			Optional<Media::StaticImage> lrimg;
			NN<Media::StaticImage> lrimgnn;
			Optional<Media::StaticImage> dimg;
			{
				IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
				optimgList = Optional<Media::ImageList>::ConvertFrom(this->env->ParseFileType(fd, IO::ParserType::ImageList));
			}
			if (optimgList.SetTo(imgList))
			{
				if (imgList->GetImage(0, 0).SetTo(rimg))
				{
					simg = rimg->CreateStaticImage();
					optimgList.Delete();
					Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
					NEW_CLASSNN(lrimgnn, Media::StaticImage(simg->info.dispSize, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, 0, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
					lrimg = lrimgnn;
					{
						Sync::MutexUsage mutUsage(this->csconvMut);
						if (this->csconv.IsNull() || this->csconvFCC != simg->info.fourcc || this->csconvBpp != simg->info.storeBPP || this->csconvPF != simg->info.pf || !simg->info.color.Equals(this->csconvColor))
						{
							this->csconv.Delete();
							this->csconvFCC = simg->info.fourcc;
							this->csconvBpp = simg->info.storeBPP;
							this->csconvPF = simg->info.pf;
							this->csconvColor.Set(simg->info.color);
							this->csconv = Media::CS::CSConverter::NewConverter(this->csconvFCC, this->csconvBpp, this->csconvPF, this->csconvColor, *(UInt32*)"LRGB", 64, Media::PF_UNKNOWN, color, Media::ColorProfile::YUVT_UNKNOWN, this->env->GetColorSess().Ptr());
						}
						NN<Media::CS::CSConverter> csconv;
						if (this->csconv.SetTo(csconv))
						{
							csconv->ConvertV2(&simg->data, lrimgnn->data, simg->info.dispSize.x, simg->info.dispSize.y, simg->info.storeSize.x, simg->info.storeSize.y, (OSInt)lrimgnn->GetDataBpl(), Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);
						}
						else
						{
							lrimg.Delete();
						}
					}
					simg.Delete();

					if (lrimg.SetTo(lrimgnn))
					{
						this->lrgbLimiter.LimitImageLRGB(lrimgnn->data, lrimgnn->info.dispSize.x, lrimgnn->info.dispSize.y);
						if (imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
						{
							Sync::MutexUsage mutUsage(this->resizerMut);
							resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
							resizerLR->SetTargetSize(Math::Size2D<UOSInt>(imgWidth, imgHeight));
							Double x1 = wfile->cropLeft;
							Double y1 = wfile->cropTop;
							Double x2 = UOSInt2Double(lrimgnn->info.dispSize.x) - wfile->cropRight;
							Double y2 = UOSInt2Double(lrimgnn->info.dispSize.y) - wfile->cropBottom;
							if (wfile->cropLeft < 0)
							{
								x1 = 0;
								x2 = UOSInt2Double(lrimgnn->info.dispSize.x) - wfile->cropRight - wfile->cropLeft;
							}
							else if (wfile->cropRight < 0)
							{
								x1 = wfile->cropLeft + wfile->cropRight;
								x2 = UOSInt2Double(lrimgnn->info.dispSize.x);
							}
							if (wfile->cropTop < 0)
							{
								y1 = 0;
								y2 = UOSInt2Double(lrimgnn->info.dispSize.y) - wfile->cropBottom - wfile->cropTop;
							}
							else if (wfile->cropBottom < 0)
							{
								y1 = wfile->cropBottom + wfile->cropTop;
								y2 = UOSInt2Double(lrimgnn->info.dispSize.y);
							}
							dimg = resizerLR->ProcessToNewPartial(lrimgnn, Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2));
							mutUsage.EndUse();
						}
						else
						{
							Sync::MutexUsage mutUsage(this->resizerMut);
							resizerLR->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
							resizerLR->SetTargetSize(Math::Size2D<UOSInt>(imgWidth, imgHeight));
							dimg = resizerLR->ProcessToNew(lrimgnn);
							mutUsage.EndUse();
						}
						lrimg.Delete();
					}
					else
					{
						dimg = 0;
					}
					if (dimg.SetTo(simg))
					{
						UInt8 *buff;
						UOSInt buffSize;
						simg->info.color.SetRAWICC(Media::ICCProfile::GetSRGBICCData());

						if (rotateType == 1)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW90);
						}
						else if (rotateType == 2)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW180);
						}
						else if (rotateType == 3)
						{
							simg->RotateImage(Media::StaticImage::RotateType::CW270);
						}

						Optional<IO::FileExporter::ParamData> param;
						Media::ImageList nimgList(CSTR("Temp"));
						IO::MemoryStream mstm;
						nimgList.AddImage(simg, 0);
						Exporter::GUIJPGExporter exporter;
						param = exporter.CreateParam(nimgList);
						exporter.SetParamInt32(param, 0, 95);
						exporter.ExportFile(mstm, CSTR(""), nimgList, param);
						exporter.DeleteParam(param);
						ResponseMstm(req, resp, mstm, CSTR("image/jpeg"));

						if (cacheDir->leng > 0 && imgWidth == GetPreviewSize() && imgHeight == GetPreviewSize())
						{
							IO::FileStream fs({sbuff2, (UOSInt)(sptr2 - sbuff2)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							buff = mstm.GetBuff(buffSize);
							fs.Write(Data::ByteArrayR(buff, buffSize));
							if (wfile->prevUpdated)
							{
								this->env->WebFilePrevUpdated(mutUsage, wfile);
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
					optimgList.Delete();
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
	NEW_CLASS(this->resizerLR, Media::Resizer::LanczosResizerLR_C32(3, 3, destProfile, this->env->GetColorSess().Ptr(), Media::AT_NO_ALPHA, 0, Media::PF_B8G8R8A8));
	this->csconv = 0;
	this->csconvFCC = 0;
	this->csconvBpp = 0;
	this->csconvPF = Media::PF_UNKNOWN;

	this->AddService(CSTR("/photo.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhoto);
	this->AddService(CSTR("/photodown.html"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhotoDown);
	this->AddService(CSTR("/img/photo"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhoto);
	this->AddService(CSTR("/img/photodown"), Net::WebUtil::RequestMethod::HTTP_GET, SvcPhotoDown);
	this->AddService(CSTR("/favicon.ico"), Net::WebUtil::RequestMethod::HTTP_GET, SvcFavicon);
}

SSWR::OrganWeb::OrganWebPhotoController::~OrganWebPhotoController()
{
	DEL_CLASS(this->resizerLR);
	this->csconv.Delete();
}
