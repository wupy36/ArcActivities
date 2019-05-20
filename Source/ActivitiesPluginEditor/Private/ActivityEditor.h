// 2017-2019 Puny Human

#pragma once


#include "IDetailsView.h"
#include "AssetThumbnail.h"
#include "GraphEditor.h"
#include "AIGraphEditor.h"
#include "Misc/NotifyHook.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class UActivity;
class UActivityNode_Base;


class FActivityEditor: public FAIGraphEditor, public FWorkflowCentricApplication, public FNotifyHook
{
public:
	FActivityEditor();
	~FActivityEditor();

	void InitActivityEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UActivity* ActivityData);
	UActivity* GetActivityBeingEdited() const { return ActivityBeingEdited; }

	void OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor);


private:
	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph); 
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection) override;

	UActivity* ActivityBeingEdited;

	
	TSharedPtr<SGraphEditor> GraphEditor;

	TSharedPtr<IDetailsView> PropertyEditor;

	TArray<UActivityNode_Base*> LastSelectedNodes;

public:


	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	// End of IToolkit interface

protected:
	TSharedRef<SDockTab> SpawnTab_GraphEditor(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Actions(const FSpawnTabArgs& Args);

	TSharedPtr<class SGraphPalette> ActionPalette;

public:
	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	// End of FAssetEditorToolkit



};