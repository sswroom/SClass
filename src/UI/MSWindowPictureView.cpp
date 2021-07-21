#include "stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Media/ImageList.h"
#include "Media/ABlend/AlphaBlend8_8.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "UI/MSWindowClientControl.h"
#include "UI/MSWindowPictureView.h"
#include <windows.h>
#include <ddraw.h>

#undef FindNextFile

UInt32 __stdcall UI::MSWindowPictureView::ThreadProc(void *userObj)
{
	WChar sbuff[512];
	WChar *sptr;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	ScreenItem *item;
	OSInt i;
	IO::StmData::FileData *fd;
	IO::ParsedObject *pobj;
	IO::ParsedObject::ParserType t;
	Media::ImageList *imgList;
	Media::StaticImage *img;

	UI::MSWindowPictureView *me = (UI::MSWindowPictureView*)userObj;

	me->threadRunning = true;
	while (!me->threadToStop)
	{
		if (me->dirChanged)
		{
			me->threadMut->Lock();
			me->dirChanged = false;
			sptr = Text::StrConcat(sbuff, me->currDir);
			me->threadMut->Unlock();

			me->ClearPreviews();
			if (sptr[-1] != '\\')
			{
				*sptr++ = '\\';
			}
			Text::StrConcat(sptr, L"*.*");

			sess = IO::Path::FindFile(sbuff);
			if (sess)
			{
				while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
				{
					if (me->dirChanged)
						break;
					if (me->threadToStop)
						break;

					if (pt == IO::Path::PT_DIRECTORY)
					{
						if (Text::StrCompare(sptr, L".") == 0)
						{
						}
						else if (Text::StrCompare(sptr, L"..") == 0)
						{
							sptr[-1] = 0;
							i = Text::StrLastIndexOf(sbuff, '\\');
							sbuff[i] = 0;
							item = MemAlloc(ScreenItem, 1);
							item->previewImg = me->GenFolderImage();
							item->fileName = Text::StrCopyNew(sptr);
							item->fullPath = Text::StrCopyNew(sbuff);
							item->isDir = true;
							me->previewMut->Lock();
							me->previewItems->Add(item);
							me->previewMut->Unlock();
							sbuff[i] = '\\';
							sptr[-1] = '\\';
						}
						else
						{
							item = MemAlloc(ScreenItem, 1);
							item->previewImg = me->GenFolderImage();
							item->fileName = Text::StrCopyNew(sptr);
							item->fullPath = Text::StrCopyNew(sbuff);
							item->isDir = true;
							me->previewMut->Lock();
							me->previewItems->Add(item);
							me->previewMut->Unlock();
						}
					}
					else if (pt == IO::Path::PT_FILE)
					{
						NEW_CLASS(fd, IO::StmData::FileData(sbuff, false));
						pobj = me->parsers->ParseFile(fd, &t);
						DEL_CLASS(fd);
						if (pobj)
						{
							if (t == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
							{
								imgList = (Media::ImageList*)pobj;
								imgList->ToStaticImage(0);
								img = (Media::StaticImage*)imgList->GetImage(0, 0);
								img->To32bpp();

								item = MemAlloc(ScreenItem, 1);
								item->previewImg = me->previewResizer->ProcessToNew(img);
								item->fileName = Text::StrCopyNew(sptr);
								item->fullPath = Text::StrCopyNew(sbuff);
								item->isDir = false;
								me->previewMut->Lock();
								me->previewItems->Add(item);
								me->previewMut->Unlock();

								me->UpdatePreview();
							}
							DEL_CLASS(pobj);
						}
					}
				}
				IO::Path::FindFileClose(sess);
			}
		}

		me->threadEvt->Wait(10000);
	}
	me->threadRunning = false;
	return 0;
}

Media::StaticImage *UI::MSWindowPictureView::GenFolderImage()
{
	Int32 points[] = {10, 40, 25, 10, 85, 10, 100, 40};
	Media::DrawImage *img = this->eng->CreateImage32(240, 240);
	Media::DrawBrush *b = img->NewBrushARGB(0xffffeb85);
	Media::DrawPen *p = img->NewPenARGB(0xffc0c0c0, 1, 0, 0);
	img->DrawRect(10, 40, 220, 190, p, b);
	img->DrawPolygonI(points, 4, p, b);
	img->DelBrush(b);
	img->DelPen(p);
	Media::StaticImage *outImg = img->ToStaticImage();
	this->eng->DeleteImage(img);
	return outImg;
}

void UI::MSWindowPictureView::UpdatePreview()
{
	LPDIRECTDRAWSURFACE7 surface;
	surface = (LPDIRECTDRAWSURFACE7)this->surfaceBuff;
	if (surface == 0)
		return;

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	RECT rc;
	OSInt hSize;
	OSInt hPos;
	OSInt vPos;
	OSInt i;
	OSInt j;
	OSInt imgOfstX;
	OSInt imgOfstY;
	Int32 imgDrawH;
	ScreenItem *item;

	DDBLTFX ddbltfx;
	ZeroMemory( &ddbltfx, sizeof(ddbltfx) );
	ddbltfx.dwSize      = sizeof(ddbltfx);
	ddbltfx.dwFillColor = 0;
	surface->Blt(0, 0, 0, DDBLT_COLORFILL, &ddbltfx);

	rc.left = 0;
	rc.top = 0;
	rc.right = this->surfaceW;
	rc.bottom = this->surfaceH;

	if (DD_OK == surface->Lock(&rc, &ddsd, DDLOCK_WAIT, 0))
	{
		this->previewMut->Lock();
		hPos = 0;
		vPos = 0;
		hSize = this->surfaceW / 240;
		i = 0;
		j = this->previewItems->GetCount();
		while (i < j)
		{
			item = this->previewItems->GetItem(i);
			if (item->previewImg)
			{
				imgOfstX = (240 - item->previewImg->info->width) >> 1;
				imgOfstY = (240 - item->previewImg->info->height) >> 1;
				imgDrawH = item->previewImg->info->height;
				if (imgOfstY + vPos + imgDrawH > (Int32)ddsd.dwHeight)
				{
					imgDrawH = ddsd.dwHeight - vPos - imgOfstY;
				}
				if (imgDrawH > 0)
				{
					if (hSize <= 0)
					{
						this->alphaBlend->Blend(((UInt8*)ddsd.lpSurface) + ddsd.lPitch * (vPos + imgOfstY), ddsd.lPitch, item->previewImg->data, item->previewImg->GetBpl(), this->surfaceW, imgDrawH, item->previewImg->info->atype);
					}
					else
					{
						this->alphaBlend->Blend(((UInt8*)ddsd.lpSurface) + ddsd.lPitch * (vPos + imgOfstY) + (hPos * 240 + imgOfstX) * 4, ddsd.lPitch, item->previewImg->data, item->previewImg->GetBpl(), item->previewImg->info->width, imgDrawH, item->previewImg->info->atype);
					}
				}

				if (++hPos >= hSize)
				{
					hPos = 0;
					vPos += 240;
				}
			}

			i++;
		}
		hSize = ddsd.dwWidth / 240;
		

		this->previewMut->Unlock();
		surface->Unlock(0);
		this->DrawToScreen();
	}
}

void UI::MSWindowPictureView::ClearPreviews()
{
	ScreenItem *item;
	OSInt i;
	this->previewMut->Lock();
	i = this->previewItems->GetCount();
	while (i-- > 0)
	{
		item = this->previewItems->RemoveAt(i);
		if (item->previewImg)
		{
			DEL_CLASS(item->previewImg);
		}
		if (item->fileName)
		{
			Text::StrDelNew(item->fileName);
		}
		if (item->fullPath)
		{
			Text::StrDelNew(item->fullPath);
		}
		MemFree(item);
	}
	this->previewMut->Unlock();
}

UI::MSWindowPictureView::MSWindowPictureView(void *hInst, UI::MSWindowClientControl *parent, Parser::ParserList *parsers, Media::ColorManagerSess *colorSess, Media::DrawEngine *eng) : UI::MSWindowDDrawControl(hInst, parent, false)
{
	this->hInst = hInst;
	this->parsers = parsers;
	this->colorSess = colorSess;
	this->eng = eng;
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_PDISPLAY);

	NEW_CLASS(this->previewItems, Data::ArrayList<ScreenItem*>());
	NEW_CLASS(this->previewMut, Sync::Mutex());
	NEW_CLASS(this->previewResizer, Media::Resizer::LanczosResizer8_C8(3, 3, &srcColor, &srcColor, colorSess, Media::AT_NO_ALPHA));
	NEW_CLASS(this->alphaBlend, Media::ABlend::AlphaBlend8_8());
	this->previewResizer->SetTargetWidth(240);
	this->previewResizer->SetTargetHeight(240);
	this->previewResizer->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);

	WChar sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	OSInt i = Text::StrLastIndexOf(sbuff, '\\');
	sbuff[i] = 0;
	this->currDir = Text::StrCopyNew(sbuff);
	this->dirChanged = true;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadMut, Sync::Mutex());
	NEW_CLASS(this->threadEvt, Sync::Event(true, L"UI.MSWindowPictureView.threadEvt"));
	Sync::Thread::Create(ThreadProc, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

UI::MSWindowPictureView::~MSWindowPictureView()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}

	ClearPreviews();
	DEL_CLASS(this->alphaBlend);
	DEL_CLASS(this->previewItems);
	DEL_CLASS(this->previewMut);
	DEL_CLASS(this->previewResizer);

	if (this->currDir)
	{
		Text::StrDelNew(this->currDir);
	}
	DEL_CLASS(this->threadMut);
	DEL_CLASS(this->threadEvt);
}

const WChar *UI::MSWindowPictureView::GetObjectClass()
{
	return L"PictureView";
}

OSInt UI::MSWindowPictureView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::MSWindowPictureView::SetDirectory(const WChar *dir)
{
	if (IO::Path::GetPathType(dir) != IO::Path::PT_DIRECTORY)
		return;
	this->threadMut->Lock();
	if (this->currDir)
	{
		Text::StrDelNew(this->currDir);
	}
	this->currDir = Text::StrCopyNew(dir);
	this->dirChanged = true;
	this->threadMut->Unlock();
	this->threadEvt->Set();
}

void UI::MSWindowPictureView::OnSurfaceCreated()
{
	UpdatePreview();
}
