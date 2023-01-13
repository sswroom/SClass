#ifndef _SM_UI_DOBJ_DIRECTOBJECT
#define _SM_UI_DOBJ_DIRECTOBJECT
#include "Data/DateTime.h"
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
			OSInt left;
			OSInt top;
			MoveType currMoveType;
			OSInt destX;
			OSInt destY;
			Data::DateTime moveTime;
			Double moveDur;
		public:
			DirectObject(OSInt left, OSInt top);
			virtual ~DirectObject();

			void GetCurrPos(OSInt *left, OSInt *top);
			Bool IsMoving();
			void MoveToPos(OSInt destX, OSInt destY, Double dur, MoveType mType);
			virtual Bool IsChanged() = 0;
			virtual Bool DoEvents() = 0;
			virtual void DrawObject(Media::DrawImage *dimg) = 0;

			virtual Bool IsObject(OSInt x, OSInt y) = 0;
//			virtual System::Windows::Forms::Cursor ^GetCursor() = 0;
			virtual void OnMouseDown() = 0;
			virtual void OnMouseUp() = 0;
			virtual void OnMouseClick() = 0;
		};
	}
}
#endif
