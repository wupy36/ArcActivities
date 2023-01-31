// 2017-2019 Puny Human

#include "ActivityNode_Stage.h"

#include "EdGraph/EdGraph.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"	   

#include "SGraphEditorActionMenuAI.h"

#include "Graph/EdGraph_Activity.h"
#include "Graph/EdGraphSchema_Activity.h"
#include "AIGraphTypes.h"

#include "ToolMenus.h"

#include "ActivityNode_Objective.h"
#include "ActivityNode_Service.h"

#include "DataModel/Activity.h"
#include "DataModel/ActivityStage.h"

#define LOCTEXT_NAMESPACE "ActivityNode"

UActivityNode_Stage::UActivityNode_Stage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ArcClassData = FArcGraphNodeClassData(UActivityStage::StaticClass(), FString());

	TopSubNodeClass = UActivityNode_Service::StaticClass();
	BottomSubNodeClass = UActivityNode_Objective::StaticClass(); 
}



void UActivityNode_Stage::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, FActivityNodePinTypes::Input, TEXT("Input"));
	CreatePin(EGPD_Output, FActivityNodePinTypes::Output_Success, TEXT("Success"));
	CreatePin(EGPD_Output, FActivityNodePinTypes::Output_Fail, TEXT("Fail"));
}

const FSlateBrush* UActivityNode_Stage::GetNodeIcon() const
{
	return Super::GetNodeIcon();
}


void UActivityNode_Stage::GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	Super::GetNodeContextMenuActions(Menu, Context);

	FToolMenuSection& Section = Menu->AddSection("StageNodeActions", LOCTEXT("StageNodeActionsHeader", "Actions"));
	{
		UEdGraph* Graph = const_cast<UEdGraph*>(Context->Graph.Get());
		Section.AddMenuEntry("AddObjective", LOCTEXT("StageNodeAddObjective", "Add Objective"),
			FText(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateUObject(const_cast<UActivityNode_Stage*>(this), &UActivityNode_Stage::AddObjectiveNode, Graph)));
	}
	
	AddContextMenuActionsServices(Menu, Context);
}

FText UActivityNode_Stage::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	switch (TitleType)
	{
	case ENodeTitleType::FullTitle:
		return IsValid(GetActivityStage()) ? GetActivityStage()->StageName : LOCTEXT("NullKey", "ERROR: NULL NODE");
	default:
		return Super::GetNodeTitle(TitleType);
	}
}

FText UActivityNode_Stage::GetDescription() const
{
	return IsValid(GetActivityStage()) ? GetActivityStage()->StageDescription : LOCTEXT("Null Description", "ERROR: NULL NODE");
}

void UActivityNode_Stage::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin->Direction == EGPD_Output)
	{
		UActivityStage* OurStage = GetActivityStage();

		UActivityStage* ConnectionStage = nullptr;
		if (Pin->LinkedTo.Num() > 0)
		{
			UActivityNode_Stage* StageNode = Cast<UActivityNode_Stage>(Pin->LinkedTo[0]->GetOwningNode());
			ConnectionStage = StageNode->GetActivityStage();
		}

		if (Pin == GetOutputPin(0))
		{
			OurStage->NextStage_Success = ConnectionStage;
		}
		else if (Pin == GetOutputPin(1))
		{
			OurStage->NextStage_Failure = ConnectionStage;
		}
	}
}

UActivityStage* UActivityNode_Stage::GetActivityStage() const
{
	return Cast<UActivityStage>(NodeInstance);
}

void UActivityNode_Stage::OnSubNodeAdded(UAIGraphNode* SubNode)
{
	Super::OnSubNodeAdded(SubNode);

	if (UActivityNode_Objective* ObjectiveNode = Cast<UActivityNode_Objective>(SubNode))
	{		   		
		GetActivityStage()->Objectives.AddUnique(ObjectiveNode->GetObjective());
	}
	if (UActivityNode_Service* ServiceNode = Cast<UActivityNode_Service>(SubNode))
	{
		GetActivityStage()->StageServices.AddUnique(ServiceNode->GetService());
	}
}

void UActivityNode_Stage::OnSubNodeRemoved(UAIGraphNode* SubNode)
{
	Super::OnSubNodeRemoved(SubNode);

	if (UActivityNode_Objective* ObjectiveNode = Cast<UActivityNode_Objective>(SubNode))
	{		   		
		GetActivityStage()->Objectives.Remove(ObjectiveNode->GetObjective());
	}
	if (UActivityNode_Service* ServiceNode = Cast<UActivityNode_Service>(SubNode))
	{
		GetActivityStage()->StageServices.Remove(ServiceNode->GetService());
	}
}

void UActivityNode_Stage::AddObjectiveNode(UEdGraph* Graph)
{
	UActivityNode_Objective* ObjTemplate = NewObject<UActivityNode_Objective>(Graph);
	AddSubNode(ObjTemplate, Graph);
}

#undef LOCTEXT_NAMESPACE
