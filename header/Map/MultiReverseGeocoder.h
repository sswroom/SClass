#ifndef _SM_MAP_MULTIREVERSEGEOCODER
#define _SM_MAP_MULTIREVERSEGEOCODER
#include "Data/ArrayList.h"
#include "IO/IWriter.h"
#include "Sync/Mutex.h"
#include "Map/IReverseGeocoder.h"

namespace Map
{
	class MultiReverseGeocoder : public Map::IReverseGeocoder
	{
	private:
		Data::ArrayList<Map::IReverseGeocoder *> *revGeos;
		OSInt nextCoder;
		IO::IWriter *errWriter;
		Sync::Mutex *mut;
	public:
		MultiReverseGeocoder(IO::IWriter *errWriter);
		virtual ~MultiReverseGeocoder();

		virtual UTF8Char *SearchName(UTF8Char *buff, Double lat, Double lon, Int32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, Double lat, Double lon, Int32 lcid);
		void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	};
};
#endif
