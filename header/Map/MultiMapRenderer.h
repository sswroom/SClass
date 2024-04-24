#ifndef _SM_MAP_MULTIMAPRENDERER
#define _SM_MAP_MULTIMAPRENDERER
#include "Data/ArrayListNN.h"
#include "Map/MapRenderer.h"

namespace Map
{
	class MultiMapRenderer : public Map::MapRenderer, public Data::ReadingListNN<Map::MapRenderer>
	{
	private:
		Int32 bgColor;
		Bool updating;
		UpdatedHandler updHdlr;
		AnyType updObj;
		Data::ArrayListNN<Map::MapRenderer> renderers;

		static void __stdcall OnUpdated(AnyType userObj);
	public:
		MultiMapRenderer(Int32 bgColor);
		virtual ~MultiMapRenderer();
		virtual void DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OptOut<UInt32> imgDurMS);
		virtual void SetUpdatedHandler(UpdatedHandler updHdlr, AnyType userObj);

		UOSInt Add(NN<Map::MapRenderer> renderer);
		void Clear();
		virtual UOSInt GetCount();
		virtual NN<Map::MapRenderer> GetItemNoCheck(UOSInt index);
		virtual Optional<Map::MapRenderer> GetItem(UOSInt index);

		void BeginUpdate();
		void EndUpdate();
	};
}
#endif
