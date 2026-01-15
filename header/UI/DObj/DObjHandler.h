#ifndef _SM_UI_DOBJ_DOBJHANDLER
#define _SM_UI_DOBJ_DOBJHANDLER
#include "Data/ArrayListNN.hpp"
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
			Sync::Mutex objMut;
			Sync::Mutex updMut;
			Data::ArrayListNN<DirectObject> objList;
			Bool shown;
			DirectObject *moveObj;
			DirectObject *downObj;
			NN<Media::DrawEngine> deng;

		protected:
			virtual void DrawBkg(NN<Media::DrawImage> dimg) = 0;
			virtual void OnObjectClear();
		public:
			DObjHandler(NN<Media::DrawEngine> deng);
			virtual ~DObjHandler();

//			void SetColorSess(Media::ColorManagerSess *colorSess);
			void ClearObjects();
			void AddObject(NN<DirectObject> obj);
			Bool Check(NN<Media::DrawImage> dimg);
			void DrawAll(NN<Media::DrawImage> dimg);
			void BeginUpdate();
			void EndUpdate();

			void OnMouseDown(Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton button);
			void OnMouseUp(Math::Coord2D<IntOS> scnPos, UI::GUIControl::MouseButton button);
			void OnMouseMove(Math::Coord2D<IntOS> scnPos);
			virtual void SizeChanged(Math::Size2D<UIntOS> size);
		};
	}
}
#endif
