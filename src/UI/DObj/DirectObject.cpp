#include "Stdafx.h"
#include "Math/Math.h"
#include "UI/DObj/DirectObject.h"

UI::DObj::DirectObject::DirectObject(OSInt left, OSInt top)
{
	this->left = left;
	this->top = top;
	this->currMoveType = MT_NONE;
	NEW_CLASS(this->moveTime, Data::DateTime());
}

UI::DObj::DirectObject::~DirectObject()
{
	DEL_CLASS(this->moveTime);
}


void UI::DObj::DirectObject::GetCurrPos(OSInt *left, OSInt *top)
{
	if (this->currMoveType == MT_NONE)
	{
		*left = this->left;
		*top = this->top;
	}
	else
	{
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Double dur = Int64_Double(currTime.DiffMS(this->moveTime)) * 0.001;
		if (dur >= this->moveDur)
		{
			this->currMoveType = MT_NONE;
			this->left = this->destX;
			this->top = this->destY;
			*left = this->left;
			*top = this->top;
		}
		else if (dur < 0)
		{
			this->moveTime->SetCurrTimeUTC();
			*left = this->left;
			*top = this->top;
		}
		else
		{
			if (this->currMoveType == MT_CONSTANT)
			{
				*left = this->left + Double2Int32(OSInt2Double(this->destX - this->left) * dur / this->moveDur);
				*top = this->top + Double2Int32(OSInt2Double(this->destY - this->top) * dur / this->moveDur);
			}
			else if (this->currMoveType == MT_ACC)
			{
				Double aX = OSInt2Double(this->destX - this->left) / this->moveDur / this->moveDur;
				Double aY = OSInt2Double(this->destY - this->top) / this->moveDur / this->moveDur;
				Double currDur = dur;
				*left = this->left + Double2Int32(aX * currDur * currDur);
				*top = this->top + Double2Int32(aY * currDur * currDur);
			}
			else if (this->currMoveType == MT_ACCDEACC)
			{
				Double aX = (this->destX - this->left) / this->moveDur / this->moveDur * 2;
				Double aY = (this->destY - this->top) / this->moveDur / this->moveDur * 2;
				Double currDur = dur;
				if (currDur > this->moveDur * 0.5)
				{
					currDur = this->moveDur - currDur;
					*left = this->destX - Double2Int32(aX * currDur * currDur);
					*top = this->destY - Double2Int32(aY * currDur * currDur);
				}
				else
				{
					*left = this->left + Double2Int32(aX * currDur * currDur);
					*top = this->top + Double2Int32(aY * currDur * currDur);
				}
			}
		}
	}
}

Bool UI::DObj::DirectObject::IsMoving()
{
	return this->currMoveType != MT_NONE;
}

void UI::DObj::DirectObject::MoveToPos(OSInt destX, OSInt destY, Double dur, MoveType mType)
{
	OSInt left;
	OSInt top;
	this->GetCurrPos(&left, &top);
	this->left = left;
	this->top = top;
	this->destX = destX;
	this->destY = destY;
	this->moveDur = dur;
	this->moveTime->SetCurrTimeUTC();
	this->currMoveType = mType;
}
