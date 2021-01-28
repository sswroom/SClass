#ifndef _SM_MEDIA_GRFILTER
#define _SM_MEDIA_GRFILTER
#include "Data/ArrayList.h"

namespace Media
{
	class GRFilter
	{
	private:
		typedef struct
		{
			OSInt hOfst;
			OSInt vOfst;
			OSInt level;
			Int32 status;
		} LayerSetting;

	private:
		Data::ArrayList<LayerSetting*> *layers;

	public:
		GRFilter();
		~GRFilter();

		OSInt GetLayerCount();
		OSInt AddLayer();
		Bool RemoveLayer(OSInt layer);
		void SetParameter(OSInt layer, OSInt hOfst, OSInt vOfst, OSInt level, Int32 status);
		Bool GetParameter(OSInt layer, OSInt *hOfst, OSInt *vOfst, OSInt *level, Int32 *status);

		void ProcessImage32(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl);
		void ProcessImage64(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl);
	};
};
#endif
