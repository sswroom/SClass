#ifndef _SM_MAP_LEICA_LEICALEVELFILE
#define _SM_MAP_LEICA_LEICALEVELFILE
#include "Data/StringMapNN.hpp"
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
				NN<Text::String> pointName;
				Measurement backPoint;
				Measurement interPoint;
				Measurement forePoint;
				HeightMeasure height;
			} LevelPoint;
		private:
			Data::StringMapNN<LevelPoint> pointMap;
			Data::ArrayListNN<LevelPoint> pointList;
			Double startLevel;
			Double endLevel;

		public:
			LeicaLevelFile();
			virtual ~LeicaLevelFile();

			void AddPointHeight(Text::CStringNN pointName, NN<HeightMeasure> height);
			void AddMeasurement(Text::CStringNN pointName, NN<Measurement> point, UTF8Char pointId);
			void SetStartLevel(Double startLevel);
			void SetEndLevel(Double endLevel);

			virtual FileType GetFileType();
			virtual Bool ExportExcel(NN<IO::SeekableStream> stm, Text::CStringNN fileName);

			UOSInt GetPointCnt();
			Text::CString GetPointName(UOSInt index);
			Optional<const Measurement> GetPointBack(UOSInt index);
			Optional<const Measurement> GetPointInter(UOSInt index);
			Optional<const Measurement> GetPointFore(UOSInt index);
			Optional<const HeightMeasure> GetPointHeight(UOSInt index);

			Bool CalcPointLevs(NN<Data::ArrayList<Double>> calcLevs, NN<Data::ArrayList<Double>> adjLevS, NN<Data::ArrayList<Double>> adjLevD);

			static void ClearMeasurement(NN<Measurement> measure);
			static void ClearHeight(NN<HeightMeasure> height);
		};
	}
}
#endif
