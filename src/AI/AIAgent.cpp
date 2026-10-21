#include "Stdafx.h"
#include "AI/AIAgent.h"

AI::AIAgent::~AIAgent()
{
}

Bool AI::AIAgent::AddSkill(NN<AISkill> skill)
{
	UIntOS i = this->skills.GetCount();
	while (i-- > 0)
	{
		if (this->skills.GetItemNoCheck(i)->GetName().Equals(skill->GetName()))
			return false;
	}
	this->skills.Add(skill);
	return true;
}

Optional<AI::AISkill> AI::AIAgent::RemoveSkill(Text::CStringNN name)
{
	UIntOS i = this->skills.GetCount();
	while (i-- > 0)
	{
		if (this->skills.GetItemNoCheck(i)->GetName().Equals(name))
			return this->skills.RemoveAt(i);
	}
	return nullptr;
}

UIntOS AI::AIAgent::GetSkillCount() const
{
	return this->skills.GetCount();
}

Optional<AI::AISkill> AI::AIAgent::GetSkill(UIntOS index) const
{
	return this->skills.GetItem(index);
}

Bool AI::AIAgent::InvokeSkill(Text::CStringNN name, NN<Data::VariObject> input, NN<Data::VariObject> output) const
{
	UIntOS i = this->skills.GetCount();
	while (i-- > 0)
	{
		NN<AISkill> skill = this->skills.GetItemNoCheck(i);
		if (skill->GetName().Equals(name))
			return skill->Invoke(input, output);
	}
	return false;
}
