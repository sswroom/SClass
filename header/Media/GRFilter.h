#ifndef _SM_MEDIA_GRFILTER
#define _SM_MEDIA_GRFILTER
#include "Data/ArrayListNN.h"

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
		Data::ArrayListNN<LayerSetting> layers;

	public:
		GRFilter();
		~GRFilter();

		UOSInt GetLayerCount();
		UOSInt AddLayer();
		Bool RemoveLayer(UOSInt layer);
		void SetParameter(UOSInt layer, OSInt hOfst, OSInt vOfst, OSInt level, Int32 status);
		Bool GetParameter(UOSInt layer, OptOut<OSInt> hOfst, OptOut<OSInt> vOfst, OptOut<OSInt> level, OptOut<Int32> status);

		void ProcessImage32(UnsafeArray<UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl);
		void ProcessImage64(UnsafeArray<UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl);
	};
}
#endif
