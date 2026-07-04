#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Enums.h"

class SPinnedSection;
class SExtendedSlateBorder;
class SVerticalTextBlock;
class SVerticalEditableText;
class SWindow;

class SPinnedTab : public SCompoundWidget
{
	DECLARE_DELEGATE_OneParam(FOnTabClickedSignature, TSharedPtr<SPinnedTab>);
	DECLARE_DELEGATE_OneParam(FOnRemoveSignature, TSharedPtr<SPinnedTab>);
	DECLARE_DELEGATE_ThreeParams(FOnNameChangedSignature, TSharedPtr<SPinnedTab>, FText, FText);

public:
	SLATE_BEGIN_ARGS(SPinnedTab)
		: _InitInRenameMode(false)
		{
		}
		SLATE_EVENT(FOnTabClickedSignature, OnTabClickedDelegate)
		SLATE_EVENT(FOnRemoveSignature, OnRemoveDelegate)
		SLATE_EVENT(FOnNameChangedSignature, OnNameChangedDelegate)
		SLATE_ARGUMENT(FText, Name)
		SLATE_ARGUMENT(TSharedPtr<SPinnedSection>, Widget)
		SLATE_ARGUMENT(bool, IsPersistent)
		SLATE_ARGUMENT(bool, InitInRenameMode)
	SLATE_END_ARGS();

	SPinnedTab();

	void Construct(const FArguments& InArgs);
	void SetName(FText InName);
	void EditName(bool EnableEditing);
	void ActivateRenameBox();
	TSharedPtr<SPinnedSection> SetSelected(bool IsSelected = false);
	TSharedPtr<SPinnedSection> GetSection();
	FString GetName();

	bool GetIsPersistent() { return bIsPersistent; }

private:
	virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

	void OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod);

	void OnRemoveClicked();
	void BuildContextMenu(FMenuBuilder& Builder);

	void OpenRenameBox();
	bool CanEdit();
	void OpenColorPicker(FMenuBuilder& Builder);
	void RemoveTab();

	void OnSetColorFromColorPicker(FLinearColor NewColor);
	void OnColorPickerCancelled(FLinearColor OriginalColor);

private:
	TSharedPtr<SExtendedSlateBorder> Background;
	TSharedPtr<SVerticalTextBlock> Text;
	TSharedPtr<SVerticalEditableText> RenameTextBox;

	FLinearColor BaseColor;
	FLinearColor HoverColor;
	FLinearColor SelectedColor;

	FLinearColor InitialColor;

	TSharedPtr<SPinnedSection> Widget;

	FOnTabClickedSignature OnTabClickedDelegate;
	FOnNameChangedSignature OnNameChangedDelegate;
	FOnRemoveSignature OnRemoveDelegate;

	float HoveredRatio = 3.42857f;
	float SelectedRatio = 5.14286f;

	bool bIsSelected;
	bool bIsPersistent = false;

};