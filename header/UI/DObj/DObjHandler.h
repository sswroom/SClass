#ifndef _SM_UI_DOBJ_DOBJHANDLER
#define _SM_UI_DOBJ_DOBJHANDLER
#include "Media/ColorManager.h"
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "UI/GUIControl.h"
#include "UI/DObj/DirectObject.h"

namespace UI
{
	namespace DObj
	{
		class DObjHandler
		{
		protected:
			Media::ColorManagerSess *colorSess;
			Sync::Mutex objMut;
			Sync::Mutex updMut;
			Data::ArrayList<DirectObject*> objList;
			Bool shown;
			DirectObject *moveObj;
			DirectObject *downObj;
			Media::DrawEngine *deng;

		protected:
			virtual void DrawBkg(Media::DrawImage *dimg) = 0;
			virtual void OnObjectClear();
		public:
			DObjHandler(Media::DrawEngine *deng);
			virtual ~DObjHandler();

			void SetColorSess(Media::ColorManagerSess *colorSess);
			void ClearObjects();
			void AddObject(DirectObject *obj);
			Bool Check(Media::DrawImage *dimg);
			void DrawAll(Media::DrawImage *dimg);
			void BeginUpdate();
			void EndUpdate();

			void OnMouseDown(OSInt x, OSInt y, UI::GUIControl::MouseButton button);
			void OnMouseUp(OSInt x, OSInt y, UI::GUIControl::MouseButton button);
			void OnMouseMove(OSInt x, OSInt y);
			virtual void SizeChanged(Math::Size2D<UOSInt> size);
		};
	}
}
#endif
