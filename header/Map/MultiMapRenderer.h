#ifndef _SM_MAP_MULTIMAPRENDERER
#define _SM_MAP_MULTIMAPRENDERER
#include "Data/ArrayList.h"
#include "Map/MapRenderer.h"

namespace Map
{
	class MultiMapRenderer : public Map::MapRenderer, public Data::List<Map::MapRenderer*>
	{
	private:
		Int32 bgColor;
		Bool updating;
		UpdatedHandler updHdlr;
		void *updObj;
		Data::ArrayList<Map::MapRenderer *> *renderers;

		static void __stdcall OnUpdated(void *userObj);
	public:
		MultiMapRenderer(Int32 bgColor);
		virtual ~MultiMapRenderer();
		virtual void DrawMap(Media::DrawImage *img, Map::MapView *view);
		virtual void SetUpdatedHandler(UpdatedHandler updHdlr, void *userObj);

		void Add(Map::MapRenderer *renderer);
		void Clear();
		virtual OSInt GetCount();
		virtual Map::MapRenderer *GetItem(OSInt index);

		void BeginUpdate();
		void EndUpdate();
	};
};
#endif
