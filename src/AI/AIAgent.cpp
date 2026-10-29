#include "Stdafx.h"
#include "AI/AIAgent.h"

AI::AIAgent::~AIAgent()
{
}

Bool AI::AIAgent::AddTool(NN<AITool> tool)
{
	UIntOS i = this->tools.GetCount();
	while (i-- > 0)
	{
		if (this->tools.GetItemNoCheck(i)->GetName().Equals(tool->GetName()))
			return false;
	}
	this->tools.Add(tool);
	return true;
}

Optional<AI::AITool> AI::AIAgent::RemoveTool(Text::CStringNN name)
{
	UIntOS i = this->tools.GetCount();
	while (i-- > 0)
	{
		if (this->tools.GetItemNoCheck(i)->GetName().Equals(name))
			return this->tools.RemoveAt(i);
	}
	return nullptr;
}

UIntOS AI::AIAgent::GetToolCount() const
{
	return this->tools.GetCount();
}

Optional<AI::AITool> AI::AIAgent::GetTool(UIntOS index) const
{
	return this->tools.GetItem(index);
}

Bool AI::AIAgent::InvokeTool(Text::CStringNN name, NN<Data::VariObject> input, NN<Data::VariObject> output) const
{
	UIntOS i = this->tools.GetCount();
	while (i-- > 0)
	{
		NN<AITool> tool = this->tools.GetItemNoCheck(i);
		if (tool->GetName().Equals(name))
			return tool->Invoke(input, output);
	}
	printf("AIAgent: tool not found: %s\n", name.v.Ptr());
	return false;
}
