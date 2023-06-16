#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/PNGExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/DeinterlaceLR.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/CS/TransferFunc.h"
#include "Media/Resizer/DeintResizerLR_C32.h"
#include "Media/Resizer/LanczosResizerLR_C16.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIVideoBoxDD.h"
#include "UI/MessageDialog.h"

//DeinterlaceResizer
//#define _DEBUG

/*void UI::GUIVideoBoxDD::UpdateFromBuff(VideoBuff *vbuff)
{
	DrawRect rect;

	this->VideoBeginProc();
	Sync::MutexUsage mutUsage(this->surfaceMut);
	UOSInt vwidth = this->videoInfo->dispWidth - cropLeft - cropRight;
	UOSInt vheight = this->videoInfo->dispHeight - cropTop - cropBottom;
	if (this->videoInfo->ftype == Media::FT_FIELD_BF || this->videoInfo->ftype == Media::FT_FIELD_TF)
	{
		vheight = vheight << 1;
	}
	this->CalDisplayRect(vwidth, vheight, &rect);
	UOSInt surfaceW = this->surfaceW;
	UOSInt surfaceH = this->surfaceH;
	mutUsage.EndUse();
	if (rect.width == vbuff->destW && rect.height == vbuff->destH && this->IsSurfaceReady() && vbuff->destBitDepth == this->bitDepth)
	{
		UOSInt dlineSize;
		UInt8 *dptr = this->LockSurfaceBegin(surfaceW, surfaceH, &dlineSize);
		if (dptr)
		{
			dptr = dptr + (rect.top * (OSInt)dlineSize + rect.left * (OSInt)(this->bitDepth >> 3));
			if (this->bitDepth == 16)
			{
				this->outputCopier->Copy16(vbuff->destBuff, (OSInt)rect.width * 2, dptr, (OSInt)dlineSize, rect.width, rect.height);
			}
			else
			{
				this->outputCopier->Copy16(vbuff->destBuff, (OSInt)rect.width * 4, dptr, (OSInt)dlineSize, rect.width, rect.height);
			}
			this->LockSurfaceEnd();
		}
	}
	this->VideoEndProc();
	
}*/

void UI::GUIVideoBoxDD::LockUpdateSize(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->surfaceMut);
}

void UI::GUIVideoBoxDD::DrawFromSurface(Media::MonitorSurface *surface, OSInt destX, OSInt destY, UOSInt buffWidth, UOSInt buffHeight, Bool clearScn)
{
	this->DisplayFromSurface(surface, destX, destY, buffWidth, buffHeight, clearScn);
}

void UI::GUIVideoBoxDD::BeginUpdateSize()
{
	this->updatingSize = true;
	this->switching = true;
}

void UI::GUIVideoBoxDD::EndUpdateSize()
{
	this->updatingSize = false;
	this->switching = false;
}

UI::GUIVideoBoxDD::GUIVideoBoxDD(UI::GUICore *ui, UI::GUIClientControl *parent, Media::ColorManagerSess *colorSess, UOSInt buffCnt, UOSInt threadCnt) : UI::GUIDDrawControl(ui, parent, false, colorSess), Media::VideoRenderer(colorSess, this->UI::GUIDDrawControl::surfaceMgr, buffCnt, threadCnt)
{
	this->UpdateRefreshRate(this->GetRefreshRate());
	this->debugLog = 0;
	this->debugFS = 0;
	this->debugLog2 = 0;
	this->debugFS2 = 0;

	if (this->colorSess)
	{
		this->colorSess->AddHandler(this);
	}
	this->maHdlr = 0;
	this->maHdlrObj = 0;
	this->maDown = 0;
	this->maDownX = 0;
	this->maDownY = 0;
	this->maDownTime = 0;

#ifdef _DEBUG
	NEW_CLASS(this->debugFS, IO::FileStream(CSTR("videoProc.log"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(this->debugLog, IO::StreamWriter(this->debugFS, 65001));
	NEW_CLASS(this->debugFS2, IO::FileStream(CSTR("videoDisp.log"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(this->debugLog2, IO::StreamWriter(this->debugFS2, 65001));
#endif
	this->OnMonitorChanged();
}

UI::GUIVideoBoxDD::~GUIVideoBoxDD()
{
	if (this->colorSess)
	{
		this->colorSess->RemoveHandler(this);
	}

#ifdef _DEBUG
	SDEL_CLASS(this->debugLog);
	SDEL_CLASS(this->debugFS);
	SDEL_CLASS(this->debugLog2);
	SDEL_CLASS(this->debugFS2);
#endif
}

Text::CString UI::GUIVideoBoxDD::GetObjectClass()
{
	return CSTR("VideoBoxDD");
}

OSInt UI::GUIVideoBoxDD::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIVideoBoxDD::OnSizeChanged(Bool updateScn)
{
	if (this->switching)
		return;
	Bool curr10Bit = false;
	if (this->currScnMode == SM_VFS || this->currScnMode == SM_FS)
	{
		curr10Bit = this->colorSess->Get10BitColor();
	}
	this->curr10Bit = curr10Bit;
	this->toClear = true;

	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers.GetItem(i)(this->resizeHandlersObjs.GetItem(i));
	}
}

void UI::GUIVideoBoxDD::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
	if (this->video && this->tstats[0].csconv)
	{
		if (!playing)
		{
			this->dispForceUpdate = true;
			this->dispEvt.Set();
		}
	}
}

void UI::GUIVideoBoxDD::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	if (this->video && this->tstats[0].csconv)
	{
		if (!playing)
		{
			this->dispForceUpdate = true;
			this->dispEvt.Set();
		}
	}
}

void UI::GUIVideoBoxDD::OnMonitorChanged()
{
	this->UpdateRefreshRate(this->GetRefreshRate());
	this->manualDeint = false;
	this->UpdateDispInfo(this->surfaceW, this->surfaceH, this->bitDepth, this->GetPixelFormat());
}

void UI::GUIVideoBoxDD::OnSurfaceCreated()
{
	this->UpdateOutputSize(this->surfaceW, this->surfaceH);
	if (!playing)
	{
		this->dispForceUpdate = true;
		this->dispEvt.Set();
	}
}

void UI::GUIVideoBoxDD::OnMouseWheel(OSInt x, OSInt y, Int32 amount)
{
}

void UI::GUIVideoBoxDD::OnMouseMove(OSInt x, OSInt y)
{
}

void UI::GUIVideoBoxDD::OnMouseDown(OSInt x, OSInt y, MouseButton button)
{
	if (button == UI::GUIControl::MBTN_LEFT && !this->maDown)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		this->maDownTime = dt.ToTicks();
		this->maDown = true;
		this->maDownX = x;
		this->maDownY = y;
	}
}

void UI::GUIVideoBoxDD::OnMouseUp(OSInt x, OSInt y, MouseButton button)
{
	if (this->maDown && button == UI::GUIControl::MBTN_LEFT)
	{
		this->maDown = false;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.ToTicks() - this->maDownTime < 1000 && this->maHdlr != 0)
		{
			OSInt xDiff = x - this->maDownX;
			OSInt yDiff = y - this->maDownY;
			OSInt xType;
			OSInt yType;
			Double dpi = this->GetHDPI();
			if (OSInt2Double(xDiff) >= dpi)
			{
				xType = 1;
			}
			else if (OSInt2Double(xDiff) <= -dpi)
			{
				xType = -1;
			}
			else
			{
				xType = 0;
			}
			if (OSInt2Double(yDiff) >= dpi)
			{
				yType = 1;
			}
			else if (OSInt2Double(yDiff) <= -dpi)
			{
				yType = -1;
			}
			else
			{
				yType = 0;
			}
			if (xType == 1 && yType == 0)
			{
				this->maHdlr(this->maHdlrObj, UI::GUIVideoBoxDD::MA_START, x, y);
			}
			else if (xType == -1 && yType == 0)
			{
				this->maHdlr(this->maHdlrObj, UI::GUIVideoBoxDD::MA_STOP, x, y);
			}
			else if (xType == 0 && yType == 1)
			{
				this->maHdlr(this->maHdlrObj, UI::GUIVideoBoxDD::MA_PAUSE, x, y);
			}
		}
	}
}

void UI::GUIVideoBoxDD::HandleMouseActon(MouseActionHandler hdlr, void *userObj)
{
	this->maHdlr = hdlr;
	this->maHdlrObj = userObj;
}

void UI::GUIVideoBoxDD::DestroyObject()
{
	this->StopPlay();
	this->SetVideo(0);
	this->StopThreads();
}
