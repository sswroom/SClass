#ifndef _SM_DATA_ML_KERAS
#define _SM_DATA_ML_KERAS
#include "Data/ML/MLModel.h"
#include "IO/LogTool.h"
#include "Text/CString.h"

namespace Data
{
	namespace ML
	{
		class Keras
		{
		public:
			static Optional<MLModel> LoadModel(Text::CStringNN fileName, NN<IO::LogTool> log);
		};
	}
}
#endif
