#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Enums.h"
#include "Structs.h"
class UPinnedAssetSubsystem;
class SPinnedTab;
class SPinnedSlot;
class SWidgetSwitcher;

enum class EditState
{
	Unfocused,
	NotInEditMode,
	InEditMode
};

class SPinnedWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPinnedWindow)
		{
		}
	SLATE_END_ARGS();

	SPinnedWindow();
	~SPinnedWindow();

	void Construct(const FArguments& InArgs);
	EditState CheckInEditMode();

	UFUNCTION()
	void SwitchTab(TSharedRef<SWidget> Widget);

private:
	void Refresh(const TArray<FPinnedAssetData>& List);

	void OnListChangedCallback(const TArray<FPinnedAssetData>& List);
	void OnTabClicked(TSharedPtr<SPinnedTab> Initiator);
	void OnTabRenamed(TSharedPtr<SPinnedTab> Initiator, FText OldName, FText NewName);
	void OnTabRemoved(TSharedPtr<SPinnedTab> Initiator);
	void OnClearButtonClicked();
	FReply OnNewTabClicked();

	virtual FReply OnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply OnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

	bool FindSection(FString Name, int& OutIndex);
	bool ContainsSection(FString Name);

private:
	EditState EditMode;

	TSharedPtr<SWidgetSwitcher> TabController;
	TSharedPtr<SVerticalBox> TabList;
	TSharedPtr<SButton> NewTabButton;
	TArray<TWeakPtr<SPinnedSlot>> Slots;

	float MinSize = 30;
	float Size = 100;
	float Ratio = 1.25;
	FString ConfigPath;

	UPinnedAssetSubsystem* PinnedAssetSubsystem;
	TSharedPtr<SPinnedTab> ActiveTab;
	TSharedPtr<SPinnedTab> PinnedTab;
	TArray<FSection> SectionMap;

	int DefaultNameIndex;

};