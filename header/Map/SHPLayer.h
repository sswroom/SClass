#ifndef _SM_MAP_SHPLAYER
#define _SM_MAP_SHPLAYER
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "DB/DBFFile.h"
#include "IO/FileStream.h"
#include "Map/IMapDrawLayer.h"

namespace Map
{
	class SHPLayer : public IMapDrawLayer
	{
	private:
		const WChar *shpName;

		typedef struct
		{
			Double x1;
			Double y1;
			Double x2;
			Double y2;
			Int32 nPoints;
			Int32 nParts;
			UInt32 ofst;
		} RecHdr;

		DB::DBFFile *dbf;
		IO::FileStream *shpStm;

		Map::DrawLayerType layerType;
		Data::ArrayListDbl *ptX;
		Data::ArrayListDbl *ptY;
		Data::ArrayList<RecHdr*> *recs;

		Bool isPoint;
		Double xMax;
		Double xMin;
		Double yMax;
		Double yMin;

	public:
		SHPLayer(const WChar *layerName, Text::Encoding *enc);
		virtual ~SHPLayer();

		Bool IsError();
		virtual const WChar *GetName();
		static void LatLon2XY(Double lat, Double lon, Int32 *x, Int32 *y);

		virtual DrawLayerType GetLayerType();
		virtual Int32 GetObjectIds(Data::ArrayListInt *outArr, void **nameArr, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual Int32 GetObjectIdsHP(Data::ArrayListInt *outArr, void **nameArr, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual Int32 GetObjectIds(Data::ArrayListInt *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual void ReleaseNameArr(void *nameArr);
		virtual WChar *GetString(WChar *buff, void *nameArr, Int32 id, Int32 strIndex);
		virtual Int32 GetStringCnt(void *nameArr);
		virtual Int32 GetBlockSize();

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObject *GetObjectById(void *session, Int32 id);
		virtual DrawObject *GetObjectByIdHP(void *session, Int32 id);
		virtual Math::Vector2D *GetNewVectorById(void *session, Int32 id);
		virtual void ReleaseObject(void *session, DrawObject *obj);
	};
};
#endif
