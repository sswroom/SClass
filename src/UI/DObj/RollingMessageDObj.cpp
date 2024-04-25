#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/DrawImageTool.h"
#include "Media/ImageUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilder.h"
#include "UI/DObj/RollingMessageDObj.h"

void UI::DObj::RollingMessageDObj::FreeMessage(MessageInfo *msg)
{
	NN<Media::DrawImage> img;
	if (img.Set(msg->img))
	{
		this->deng->DeleteImage(img);
	}
	msg->message->Release();
	MemFree(msg);
}

UI::DObj::RollingMessageDObj::RollingMessageDObj(NN<Media::DrawEngine> deng, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, Double rollSpeed) : DirectObject(tl)
{
	this->deng = deng;
	this->size = size;
	this->rollSpeed = rollSpeed;
	this->lastRollPos = -1;
	this->startTime.SetCurrTimeUTC();
	this->nextMsgId = 1;
	this->lastMessage = 0;
	this->thisMessage = 0;
	this->nextMsgIndex = 0;
	this->lastMsgOfst = 0;
}

UI::DObj::RollingMessageDObj::~RollingMessageDObj()
{
	if (this->lastMessage && this->lastMessage->deleted)
		FreeMessage(this->lastMessage);
	if (this->thisMessage && this->thisMessage->deleted)
		FreeMessage(this->thisMessage);
	UOSInt i = this->msgMap.GetCount();
	while (i-- > 0)
	{
		FreeMessage(this->msgMap.GetItem(i));
	}
}

Bool UI::DObj::RollingMessageDObj::IsChanged()
{
	if (this->lastMessage && this->lastMessage->img == 0)
		return true;
	if (this->thisMessage && this->thisMessage->img == 0)
		return true;
	Data::DateTime currTime;
	currTime.SetCurrTimeUTC();
	Double t = currTime.Diff(this->startTime).GetTotalSec();
	OSInt currPos = Double2OSInt(UOSInt2Double(this->size.x) * t / this->rollSpeed);
	if (currPos != this->lastRollPos)
	{
		return true;
	}
	return false;
}

Bool UI::DObj::RollingMessageDObj::DoEvents()
{
	return false;
}

void UI::DObj::RollingMessageDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	Sync::MutexUsage mutUsage(this->msgMut);
	Math::Coord2DDbl scnPos = this->GetCurrPos().ToDouble();
	if (this->lastMessage || this->thisMessage)
	{
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Double t = currTime.Diff(this->startTime).GetTotalSec();
		OSInt currPos = Double2OSInt(UOSInt2Double(this->size.x) * t / this->rollSpeed);
		OSInt lastPos = currPos + this->lastMsgOfst;
		if (this->lastMessage)
		{
			NN<Media::DrawImage> img;
			if (this->lastMessage->img == 0)
			{
				this->lastMessage->img = this->GenerateImage(this->deng, this->lastMessage->message, this->size, dimg);
			}
			if (lastPos >= (OSInt)this->size.x)
			{
				if (this->lastMessage->deleted)
				{
					this->FreeMessage(this->lastMessage);
				}
				this->lastMessage = 0;
			}
			else if (img.Set(this->lastMessage->img))
			{
				OSInt ofst = 0;
				if (img->GetWidth() > this->size.x)
					ofst = -(OSInt)(img->GetWidth() - this->size.x);

				OSInt drawPos = ofst - lastPos;
				OSInt srcPos = 0;
				Math::Size2D<UOSInt> srcSize = img->GetSize();
				if (drawPos < 0)
				{
					srcPos = -drawPos;
					drawPos = 0;
					if ((UOSInt)srcPos >= srcSize.x)
					{
						srcSize.x = 0;
					}
					else
					{
						srcSize.x -= (UOSInt)srcPos;
					}
				}
				if (drawPos + (OSInt)srcSize.x > (OSInt)this->size.x)
				{
					srcSize.x = this->size.x - (UOSInt)drawPos;
				}
				dimg->DrawImagePt3(img, scnPos + Math::Coord2DDbl(OSInt2Double(drawPos), UOSInt2Double(this->size.y - img->GetHeight()) * 0.5), Math::Coord2DDbl(OSInt2Double(srcPos), 0), srcSize.ToDouble());
			}
		}
		if (this->thisMessage)
		{
			NN<Media::DrawImage> img;
			if (this->thisMessage->img == 0)
			{
				this->thisMessage->img = this->GenerateImage(this->deng, this->thisMessage->message, this->size, dimg);
			}
			if (img.Set(this->thisMessage->img))
			{
				UOSInt w = img->GetWidth();
				if (w < this->size.x)
					w = this->size.x;

				OSInt drawPos = (OSInt)this->size.x - currPos;
				OSInt srcPos = 0;
				Math::Size2D<UOSInt> srcSize = img->GetSize();
				if (drawPos < 0)
				{
					srcPos = -drawPos;
					drawPos = 0;
					if ((UOSInt)srcPos >= srcSize.x)
					{
						srcSize.x = 0;
					}
					else
					{
						srcSize.x -= (UOSInt)srcPos;
					}
				}
				if (drawPos + (OSInt)srcSize.x > (OSInt)this->size.x)
				{
					srcSize.x = this->size.x - (UOSInt)drawPos;
				}
				dimg->DrawImagePt3(img, scnPos + Math::Coord2DDbl(OSInt2Double(drawPos), UOSInt2Double(this->size.y - img->GetHeight()) * 0.5), Math::Coord2DDbl(OSInt2Double(srcPos), 0), srcSize.ToDouble());
				if (currPos >= (OSInt)w)
				{
					this->lastMessage = this->thisMessage;
					this->thisMessage = 0;
					this->lastMsgOfst = 0;
					if (this->msgMap.GetCount() == 0)
					{
						return;
					}
					while (this->nextMsgIndex >= this->msgMap.GetCount())
					{
						this->nextMsgIndex -= this->msgMap.GetCount();
					}
					this->thisMessage = this->msgMap.GetItem(this->nextMsgIndex++);
					if (this->thisMessage->img == 0)
					{
						this->thisMessage->img = this->GenerateImage(this->deng, this->thisMessage->message, this->size, dimg);
					}
					this->startTime = currTime;
					this->lastRollPos = 0;
				}
				else
				{
					this->lastRollPos = currPos;
				}
			}
		}
		else
		{
			if (this->msgMap.GetCount() == 0)
			{
				return;
			}
			while (this->nextMsgIndex >= this->msgMap.GetCount())
			{
				this->nextMsgIndex -= this->msgMap.GetCount();
			}
			this->thisMessage = this->msgMap.GetItem(this->nextMsgIndex++);
			this->lastMsgOfst = currPos;
			if (this->thisMessage->img == 0)
			{
				this->thisMessage->img = this->GenerateImage(this->deng, this->thisMessage->message, this->size, dimg);
			}
			this->startTime = currTime;
			this->lastRollPos = 0;
		}
	}
	else
	{
		if (this->msgMap.GetCount() == 0)
		{
			return;
		}
		while (this->nextMsgIndex >= this->msgMap.GetCount())
		{
			this->nextMsgIndex -= this->msgMap.GetCount();
		}
		this->thisMessage = this->msgMap.GetItem(this->nextMsgIndex++);
		if (this->thisMessage->img == 0)
		{
			this->thisMessage->img = this->GenerateImage(this->deng, this->thisMessage->message, this->size, dimg);
		}
		this->startTime.SetCurrTimeUTC();
		this->lastRollPos = 0;
	}
}

Bool UI::DObj::RollingMessageDObj::IsObject(Math::Coord2D<OSInt> scnPos)
{
	return false;
}

void UI::DObj::RollingMessageDObj::OnMouseDown()
{
}

void UI::DObj::RollingMessageDObj::OnMouseUp()
{
}

void UI::DObj::RollingMessageDObj::OnMouseClick()
{
}

UInt32 UI::DObj::RollingMessageDObj::AddMessage(Text::CString message)
{
	MessageInfo *msg;
	msg = MemAlloc(MessageInfo, 1);
	msg->id = Sync::Interlocked::IncrementU32(this->nextMsgId);
	msg->message = Text::String::New(message);
	msg->img = 0;
	msg->deleted = false;
	Sync::MutexUsage mutUsage(this->msgMut);
	this->msgMap.Put(msg->id, msg);
	return msg->id;
}

UInt32 UI::DObj::RollingMessageDObj::AddMessage(NN<Text::String> message)
{
	MessageInfo *msg;
	msg = MemAlloc(MessageInfo, 1);
	msg->id = Sync::Interlocked::IncrementU32(this->nextMsgId);
	msg->message = message->Clone();
	msg->img = 0;
	msg->deleted = false;
	Sync::MutexUsage mutUsage(this->msgMut);
	this->msgMap.Put(msg->id, msg);
	return msg->id;
}

void UI::DObj::RollingMessageDObj::RemoveMessage(UInt32 msgId)
{
	MessageInfo *msg;
	Sync::MutexUsage mutUsage(this->msgMut);
	msg = this->msgMap.Remove(msgId);
	if (msg)
	{
		if (this->lastMessage == msg || this->thisMessage == msg)
		{
			msg->deleted = true;
		}
		else
		{
			FreeMessage(msg);
		}
	}
}

void UI::DObj::RollingMessageDObj::SetSize(Math::Size2D<UOSInt> size)
{
	Sync::MutexUsage mutUsage(this->msgMut);
	NN<Media::DrawImage> img;
	if (this->lastMessage && img.Set(this->lastMessage->img))
	{
		this->deng->DeleteImage(img);
		this->lastMessage->img = 0;
	}
	if (this->thisMessage && img.Set(this->thisMessage->img))
	{
		this->deng->DeleteImage(img);
		this->thisMessage->img = 0;
	}
	MessageInfo *msg;
	UOSInt i = this->msgMap.GetCount();
	while (i-- > 0)
	{
		msg = this->msgMap.GetItem(i);
		if (img.Set(msg->img))
		{
			this->deng->DeleteImage(img);
			msg->img = 0;
		}
	}
	this->size = size;
}

