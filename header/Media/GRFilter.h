#ifndef _SM_MEDIA_GRFILTER
#define _SM_MEDIA_GRFILTER
#include "Data/ArrayListNN.hpp"

namespace Media
{
	class GRFilter
	{
	private:
		typedef struct
		{
			IntOS hOfst;
			IntOS vOfst;
			IntOS level;
			Int32 status;
		} LayerSetting;

	private:
		Data::ArrayListNN<LayerSetting> layers;

	public:
		GRFilter();
		~GRFilter();

		UIntOS GetLayerCount();
		UIntOS AddLayer();
		Bool RemoveLayer(UIntOS layer);
		void SetParameter(UIntOS layer, IntOS hOfst, IntOS vOfst, IntOS level, Int32 status);
		Bool GetParameter(UIntOS layer, OptOut<IntOS> hOfst, OptOut<IntOS> vOfst, OptOut<IntOS> level, OptOut<Int32> status);

		void ProcessImage32(UnsafeArray<UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl);
		void ProcessImage64(UnsafeArray<UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS width, UIntOS height, IntOS sbpl, IntOS dbpl);
	};
}
#endif
