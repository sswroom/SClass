#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/DrawImageTool.h"
#include "Media/ImageUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilder.hpp"
#include "UI/DObj/RollingMessageDObj.h"

void UI::DObj::RollingMessageDObj::FreeMessage(NN<MessageInfo> msg)
{
	NN<Media::DrawImage> img;
	if (msg->img.SetTo(img))
	{
		this->deng->DeleteImage(img);
	}
	msg->message->Release();
	MemFreeNN(msg);
}

UI::DObj::RollingMessageDObj::RollingMessageDObj(NN<Media::DrawEngine> deng, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> size, Double rollSpeed) : DirectObject(tl)
{
	this->deng = deng;
	this->size = size;
	this->rollSpeed = rollSpeed;
	this->lastRollPos = -1;
	this->startTime.SetCurrTimeUTC();
	this->nextMsgId = 1;
	this->lastMessage = nullptr;
	this->thisMessage = nullptr;
	this->nextMsgIndex = 0;
	this->lastMsgOfst = 0;
}

UI::DObj::RollingMessageDObj::~RollingMessageDObj()
{
	NN<MessageInfo> msg;
	if (this->lastMessage.SetTo(msg) && msg->deleted)
		FreeMessage(msg);
	if (this->thisMessage.SetTo(msg) && msg->deleted)
		FreeMessage(msg);
	UIntOS i = this->msgMap.GetCount();
	while (i-- > 0)
	{
		FreeMessage(this->msgMap.GetItemNoCheck(i));
	}
}

Bool UI::DObj::RollingMessageDObj::IsChanged()
{
	NN<MessageInfo> msg;
	if (this->lastMessage.SetTo(msg) && msg->img.IsNull())
		return true;
	if (this->thisMessage.SetTo(msg) && msg->img.IsNull())
		return true;
	Data::DateTime currTime;
	currTime.SetCurrTimeUTC();
	Double t = currTime.Diff(this->startTime).GetTotalSec();
	IntOS currPos = Double2IntOS(UIntOS2Double(this->size.x) * t / this->rollSpeed);
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
	NN<MessageInfo> msg;
	if (this->lastMessage.NotNull() || this->thisMessage.NotNull())
	{
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Double t = currTime.Diff(this->startTime).GetTotalSec();
		IntOS currPos = Double2IntOS(UIntOS2Double(this->size.x) * t / this->rollSpeed);
		IntOS lastPos = currPos + this->lastMsgOfst;
		if (this->lastMessage.SetTo(msg))
		{
			NN<Media::DrawImage> img;
			if (msg->img.IsNull())
			{
				msg->img = this->GenerateImage(this->deng, msg->message, this->size, dimg);
			}
			if (lastPos >= (IntOS)this->size.x)
			{
				if (msg->deleted)
				{
					this->FreeMessage(msg);
				}
				this->lastMessage = nullptr;
			}
			else if (msg->img.SetTo(img))
			{
				IntOS ofst = 0;
				if (img->GetWidth() > this->size.x)
					ofst = -(IntOS)(img->GetWidth() - this->size.x);

				IntOS drawPos = ofst - lastPos;
				IntOS srcPos = 0;
				Math::Size2D<UIntOS> srcSize = img->GetSize();
				if (drawPos < 0)
				{
					srcPos = -drawPos;
					drawPos = 0;
					if ((UIntOS)srcPos >= srcSize.x)
					{
						srcSize.x = 0;
					}
					else
					{
						srcSize.x -= (UIntOS)srcPos;
					}
				}
				if (drawPos + (IntOS)srcSize.x > (IntOS)this->size.x)
				{
					srcSize.x = this->size.x - (UIntOS)drawPos;
				}
				dimg->DrawImagePt3(img, scnPos + Math::Coord2DDbl(IntOS2Double(drawPos), UIntOS2Double(this->size.y - img->GetHeight()) * 0.5), Math::Coord2DDbl(IntOS2Double(srcPos), 0), srcSize.ToDouble());
			}
		}
		if (this->thisMessage.SetTo(msg))
		{
			NN<Media::DrawImage> img;
			if (msg->img.IsNull())
			{
				msg->img = this->GenerateImage(this->deng, msg->message, this->size, dimg);
			}
			if (msg->img.SetTo(img))
			{
				UIntOS w = img->GetWidth();
				if (w < this->size.x)
					w = this->size.x;

				IntOS drawPos = (IntOS)this->size.x - currPos;
				IntOS srcPos = 0;
				Math::Size2D<UIntOS> srcSize = img->GetSize();
				if (drawPos < 0)
				{
					srcPos = -drawPos;
					drawPos = 0;
					if ((UIntOS)srcPos >= srcSize.x)
					{
						srcSize.x = 0;
					}
					else
					{
						srcSize.x -= (UIntOS)srcPos;
					}
				}
				if (drawPos + (IntOS)srcSize.x > (IntOS)this->size.x)
				{
					srcSize.x = this->size.x - (UIntOS)drawPos;
				}
				dimg->DrawImagePt3(img, scnPos + Math::Coord2DDbl(IntOS2Double(drawPos), UIntOS2Double(this->size.y - img->GetHeight()) * 0.5), Math::Coord2DDbl(IntOS2Double(srcPos), 0), srcSize.ToDouble());
				if (currPos >= (IntOS)w)
				{
					this->lastMessage = this->thisMessage;
					this->thisMessage = nullptr;
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
					if (msg->img.IsNull())
					{
						msg->img = this->GenerateImage(this->deng, msg->message, this->size, dimg);
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
			this->thisMessage = msg = this->msgMap.GetItemNoCheck(this->nextMsgIndex++);
			this->lastMsgOfst = currPos;
			if (msg->img.IsNull())
			{
				msg->img = this->GenerateImage(this->deng, msg->message, this->size, dimg);
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
		this->thisMessage = msg = this->msgMap.GetItemNoCheck(this->nextMsgIndex++);
		if (msg->img.IsNull())
		{
			msg->img = this->GenerateImage(this->deng, msg->message, this->size, dimg);
		}
		this->startTime.SetCurrTimeUTC();
		this->lastRollPos = 0;
	}
}

Bool UI::DObj::RollingMessageDObj::IsObject(Math::Coord2D<IntOS> scnPos)
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

UInt32 UI::DObj::RollingMessageDObj::AddMessage(Text::CStringNN message)
{
	NN<MessageInfo> msg;
	msg = MemAllocNN(MessageInfo);
	msg->id = Sync::Interlocked::IncrementU32(this->nextMsgId);
	msg->message = Text::String::New(message);
	msg->img = nullptr;
	msg->deleted = false;
	Sync::MutexUsage mutUsage(this->msgMut);
	this->msgMap.Put(msg->id, msg);
	return msg->id;
}

UInt32 UI::DObj::RollingMessageDObj::AddMessage(NN<Text::String> message)
{
	NN<MessageInfo> msg;
	msg = MemAllocNN(MessageInfo);
	msg->id = Sync::Interlocked::IncrementU32(this->nextMsgId);
	msg->message = message->Clone();
	msg->img = nullptr;
	msg->deleted = false;
	Sync::MutexUsage mutUsage(this->msgMut);
	this->msgMap.Put(msg->id, msg);
	return msg->id;
}

void UI::DObj::RollingMessageDObj::RemoveMessage(UInt32 msgId)
{
	NN<MessageInfo> msg;
	Sync::MutexUsage mutUsage(this->msgMut);
	if (this->msgMap.Remove(msgId).SetTo(msg))
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

void UI::DObj::RollingMessageDObj::SetSize(Math::Size2D<UIntOS> size)
{
	Sync::MutexUsage mutUsage(this->msgMut);
	NN<Media::DrawImage> img;
	NN<MessageInfo> msg;
	if (this->lastMessage.SetTo(msg) && msg->img.SetTo(img))
	{
		this->deng->DeleteImage(img);
		msg->img = nullptr;
	}
	if (this->thisMessage.SetTo(msg) && msg->img.SetTo(img))
	{
		this->deng->DeleteImage(img);
		msg->img = nullptr;
	}
	UIntOS i = this->msgMap.GetCount();
	while (i-- > 0)
	{
		msg = this->msgMap.GetItemNoCheck(i);
		if (msg->img.SetTo(img))
		{
			this->deng->DeleteImage(img);
			msg->img = nullptr;
		}
	}
	this->size = size;
}

