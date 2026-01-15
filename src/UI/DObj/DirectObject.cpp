#include "Stdafx.h"
#include "Math/Math_C.h"
#include "UI/DObj/DirectObject.h"

UI::DObj::DirectObject::DirectObject(Math::Coord2D<IntOS> tl)
{
	this->tl = tl;
	this->currMoveType = MT_NONE;
}

UI::DObj::DirectObject::~DirectObject()
{
}


Math::Coord2D<IntOS> UI::DObj::DirectObject::GetCurrPos()
{
	if (this->currMoveType == MT_NONE)
	{
		return this->tl;
	}
	else
	{
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Double dur = Int64_Double(currTime.DiffMS(this->moveTime)) * 0.001;
		if (dur >= this->moveDur)
		{
			this->currMoveType = MT_NONE;
			this->tl = this->destTL;
			return this->tl;
		}
		else if (dur < 0)
		{
			this->moveTime.SetCurrTimeUTC();
			return this->tl;
		}
		else
		{
			if (this->currMoveType == MT_CONSTANT)
			{
				Math::Coord2DDbl diff = (this->destTL - this->tl).ToDouble() * dur / this->moveDur;
				return this->tl + Math::Coord2D<IntOS>(Double2IntOS(diff.x), Double2IntOS(diff.y));
			}
			else if (this->currMoveType == MT_ACC)
			{
				Double currDur = dur;
				Math::Coord2DDbl diff = (this->destTL - this->tl).ToDouble() / this->moveDur / this->moveDur * currDur * currDur;
				return this->tl + Math::Coord2D<IntOS>(Double2IntOS(diff.x), Double2IntOS(diff.y));
			}
			else if (this->currMoveType == MT_ACCDEACC)
			{
				Double currDur = dur;
				Math::Coord2DDbl diff = (this->destTL - this->tl).ToDouble() / this->moveDur / this->moveDur * 2;
				if (currDur > this->moveDur * 0.5)
				{
					currDur = this->moveDur - currDur;
					diff = diff * currDur * currDur;
					return this->destTL - Math::Coord2D<IntOS>(Double2IntOS(diff.x), Double2IntOS(diff.y));
				}
				else
				{
					diff = diff * currDur * currDur;
					return this->tl + Math::Coord2D<IntOS>(Double2IntOS(diff.x), Double2IntOS(diff.y));
				}
			}
			else
			{
				return Math::Coord2D<IntOS>(0, 0);
			}
		}
	}
}

Bool UI::DObj::DirectObject::IsMoving()
{
	return this->currMoveType != MT_NONE;
}

void UI::DObj::DirectObject::MoveToPos(Math::Coord2D<IntOS> destTL, Double dur, MoveType mType)
{
	Math::Coord2D<IntOS> tl = this->GetCurrPos();
	this->tl = tl;
	this->destTL = destTL;
	this->moveDur = dur;
	this->moveTime.SetCurrTimeUTC();
	this->currMoveType = mType;
}
