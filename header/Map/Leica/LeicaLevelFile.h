#ifndef _SM_MAP_LEICA_LEICALEVELFILE
#define _SM_MAP_LEICA_LEICALEVELFILE
#include "Data/StringMap.h"
#include "Map/Leica/LeicaGSIFile.h"

namespace Map
{
	namespace Leica
	{
		class LeicaLevelFile : public Map::Leica::LeicaGSIFile
		{
		public:
			typedef struct
			{
				Int32 horizonalDistance;
				Int32 heightDiff;
				Int32 distExtra[2];
			} Measurement;

			typedef struct
			{
				Int32 elevation;
				Int32 distExtra[2];
			} HeightMeasure;
		private:
			typedef struct
			{
				const WChar *pointName;
				Measurement backPoint;
				Measurement interPoint;
				Measurement forePoint;
				HeightMeasure height;
			} LevelPoint;
		private:
			Data::StringMap<LevelPoint*> *pointMap;
			Data::ArrayList<LevelPoint*> *pointList;
			Double startLevel;
			Double endLevel;

		public:
			LeicaLevelFile();
			virtual ~LeicaLevelFile();

			void AddPointHeight(const WChar *pointName, HeightMeasure *height);
			void AddMeasurement(const WChar *pointName, Measurement *point, WChar pointId);
			void SetStartLevel(Double startLevel);
			void SetEndLevel(Double endLevel);

			virtual FileType GetFileType();
			virtual Bool ExportExcel(IO::SeekableStream *stm, const WChar *fileName);

			OSInt GetPointCnt();
			const WChar *GetPointName(OSInt index);
			const Measurement *GetPointBack(OSInt index);
			const Measurement *GetPointInter(OSInt index);
			const Measurement *GetPointFore(OSInt index);
			const HeightMeasure *GetPointHeight(OSInt index);

			Bool CalcPointLevs(Data::ArrayList<Double> *calcLevs, Data::ArrayList<Double> *adjLevS, Data::ArrayList<Double> *adjLevD);

			static void ClearMeasurement(Measurement *measure);
			static void ClearHeight(HeightMeasure *height);
		};
	}
}
#endif
