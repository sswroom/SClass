#ifndef _SM_UI_DOBJ_DIRECTOBJECT
#define _SM_UI_DOBJ_DIRECTOBJECT
#include "Data/DateTime.h"
#include "Math/Coord2D.h"
#include "Media/DrawEngine.h"

namespace UI
{
	namespace DObj
	{
		class DirectObject
		{
		public:
			typedef enum
			{
				MT_NONE,
				MT_CONSTANT,
				MT_ACC,
				MT_ACCDEACC
			} MoveType;
		private:
			Math::Coord2D<OSInt> tl;
			MoveType currMoveType;
			Math::Coord2D<OSInt> destTL;
			Data::DateTime moveTime;
			Double moveDur;
		public:
			DirectObject(Math::Coord2D<OSInt> tl);
			virtual ~DirectObject();

			Math::Coord2D<OSInt> GetCurrPos();
			Bool IsMoving();
			void MoveToPos(Math::Coord2D<OSInt> destTL, Double dur, MoveType mType);
			virtual Bool IsChanged() = 0;
			virtual Bool DoEvents() = 0;
			virtual void DrawObject(NotNullPtr<Media::DrawImage> dimg) = 0;

			virtual Bool IsObject(Math::Coord2D<OSInt> scnPos) = 0;
//			virtual System::Windows::Forms::Cursor ^GetCursor() = 0;
			virtual void OnMouseDown() = 0;
			virtual void OnMouseUp() = 0;
			virtual void OnMouseClick() = 0;
		};
	}
}
#endif
