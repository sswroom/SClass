namespace Map
{
	typedef struct
	{
		Int32 extraType;
		Int32 color;
		Double lat;
		Double lon;
	} MAP_EXTRA;

	class MapLayer
	{
	public:
		static Int32 Draw(UInt8 *buff, Int32 width, Int32 height, Double lat, Double lon, Int32 scale, WChar *lang, Int32 imgFormat, Data::ArrayList<MAP_EXTRA*> *drawExtra, Bool *isLayerEmpty);
	};
};
