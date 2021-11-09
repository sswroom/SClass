#include "Stdafx.h"
#include "DB/QueryConditions.h"
#include "Text/StringBuilderUTF8.h"

DB::QueryConditions::QueryConditions()
{
	NEW_CLASS(this->conditionList, Data::ArrayList<Condition*>());
}

DB::QueryConditions::~QueryConditions()
{
	LIST_FREE_FUNC(this->conditionList, DEL_CLASS);
	DEL_CLASS(this->conditionList);
}

Bool DB::QueryConditions::IsValid(Data::VariObject *obj)
{
	return ObjectValid(obj, this->conditionList);
}

Bool DB::QueryConditions::ToWhereClause(Text::StringBuilderUTF *sb, DB::DBUtil::ServerType svrType, UOSInt maxDBItem, Data::ArrayList<Condition*> *clientConditions)
{
	Text::StringBuilderUTF8 sbTmp;
	Bool hasOr = false;
	UOSInt i = 0;
	UOSInt j = this->conditionList->GetCount();
	while (i < j)
	{
		Condition *condition = this->conditionList->GetItem(i);
		if (condition->GetType() == ConditionType::Or)
		{
			hasOr = true;
			break;
		}
		i++;
	}

	if (hasOr)
	{
		UOSInt splitType = 1;
		i = 0;
		j = this->conditionList->GetCount();
		while (i < j)
		{
			Condition *condition = this->conditionList->GetItem(i);
			if (condition->GetType() == ConditionType::Or)
			{
				if (splitType != 0)
				{

				}
				else
				{
					sb->AppendChar(')', 1);
					splitType = 2;
				}
			}
			else
			{
				sbTmp.ClearStr();
				if (!condition->ToWhereClause(&sbTmp, svrType, maxDBItem))
				{
					clientConditions->Add(condition);
				}
				else
				{
					if (splitType == 2)
					{
						sb->Append((const UTF8Char*)" or (");
						splitType = 0;
					}
					else if (splitType != 0)
					{
						sb->AppendChar('(', 1);
						splitType = 0;
					}
					else
					{
						sb->Append((const UTF8Char*)" and ");
					}
					sb->Append(sbTmp.ToString());
				}
			}
			i++;
		}
		if (splitType == 0)
		{
			sb->AppendChar(')', 1);
		}
	}
	else
	{
		Bool hasCond = false;
		i = 0;
		j = this->conditionList->GetCount();
		while (i < j)
		{
			Condition *condition = this->conditionList->GetItem(i);
			sbTmp.ClearStr();
			if (!condition->ToWhereClause(&sbTmp, svrType, maxDBItem))
			{
				clientConditions->Add(condition);
			}
			else
			{
				if (hasCond)
				{
					sb->Append((const UTF8Char*)" and ");
				}
				sb->Append(sbTmp.ToString());
				hasCond = true;
			}
			i++;
		}
	}
	return true;
}

UOSInt DB::QueryConditions::GetCount()
{
	return this->conditionList->GetCount();
}

DB::QueryConditions::Condition *DB::QueryConditions::GetItem(UOSInt index)
{
	return this->conditionList->GetItem(index);
}

Data::ArrayList<DB::QueryConditions::Condition*> *DB::QueryConditions::GetList()
{
	return this->conditionList;
}

const UTF8Char *DB::QueryConditions::CompareConditionGetStr(CompareCondition cond)
{
	switch (cond)
	{
	case CompareCondition::Equal:
		return (const UTF8Char*)" = ";
	case CompareCondition::Greater:
		return (const UTF8Char*)" > ";
	case CompareCondition::Less:
		return (const UTF8Char*)" < ";
	case CompareCondition::GreaterOrEqual:
		return (const UTF8Char*)" >= ";
	case CompareCondition::LessOrEqual:
		return (const UTF8Char*)" <= ";
	case CompareCondition::NotEqual:
		return (const UTF8Char*)" <> ";
	}
	return (const UTF8Char*)"";
}

Bool DB::QueryConditions::ObjectValid(Data::VariObject *obj, Data::ArrayList<Condition*> *conditionList)
{
	Bool ret = true;
	Condition *cond;
	UOSInt i = 0;
	UOSInt j = conditionList->GetCount();
	while (i < j)
	{
		cond = conditionList->GetItem(i);
		if (cond->GetType() == ConditionType::Or)
		{
			if (ret)
				return true;
			ret = true;
		}
		else
		{
			ret = ret && cond->IsValid(obj);
		}
		i++;
	}
	return ret;
}
