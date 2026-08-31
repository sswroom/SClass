#ifndef _SM_DB_SQLVALUE
#define _SM_DB_SQLVALUE

namespace DB
{
	namespace SQL
	{
		class SQLValue
		{
		public:
			enum class ValueType
			{
				String,
				I32,
				I64,
				F64,
				Bool,
				Function,
				ObjectPath,
				Null
			};
		public:
			virtual ~SQLValue() {};

			virtual ValueType GetValueType() const = 0;
		};
	}
}
#endif
