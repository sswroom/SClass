#ifndef _SM_DB_SQLFUNCTIONVALUE
#define _SM_DB_SQLFUNCTIONVALUE
#include "Data/ArrayListNN.hpp"
#include "DB/SQL/SQLObjectPath.h"

namespace DB
{
	namespace SQL
	{
		class SQLFunctionValue : public SQLValue
		{
		private:
			NN<SQLObjectPath> objectPath;
			Data::ArrayListNN<SQLValue> paramList;
		public:
			SQLFunctionValue(NN<SQLObjectPath> objPath);
			virtual ~SQLFunctionValue();

			virtual ValueType GetValueType() const;
			NN<SQLObjectPath> GetObjectPath() const;
			void AddParam(NN<SQLValue> param);
			UIntOS GetParamCount() const;
			Optional<SQLValue> GetParam(UIntOS index) const;
		};
	}
}
#endif
