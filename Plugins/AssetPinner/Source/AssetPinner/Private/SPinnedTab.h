#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Enums.h"

class SPinnedSection;
class SExtendedSlateBorder;
class SVerticalTextBlock;
class SVerticalEditableText;

class SPinnedTab : public SCompoundWidget
{
	DECLARE_DELEGATE_OneParam(FOnTabClickedSignature, SPinnedTab*);
	DECLARE_DELEGATE_OneParam(FOnRemoveSignature, SPinnedTab*);
	DECLARE_DELEGATE_ThreeParams(FOnNameChangedSignature, SPinnedTab*, FText, FText);

public:
	SLATE_BEGIN_ARGS(SPinnedTab)
		{
		}
		SLATE_EVENT(FOnTabClickedSignature, OnTabClickedDelegate)
		SLATE_EVENT(FOnRemoveSignature, OnRemoveDelegate)
		SLATE_EVENT(FOnNameChangedSignature, OnNameChangedDelegate)
		SLATE_ARGUMENT(FText, Name)
		SLATE_ARGUMENT(TSharedPtr<SPinnedSection>, Widget)
		SLATE_ARGUMENT(bool, IsPersistent)
	SLATE_END_ARGS();

	SPinnedTab();

	void Construct(const FArguments& InArgs);
	void SetName(FText InName);
	void EditName(bool EnableEditing);
	void ActivateRenameBox();
	TSharedPtr<SPinnedSection> SetSelected(bool IsSelected = false);
	TSharedPtr<SPinnedSection> GetSection();
	FString GetName();
	void InitInRenameMode() { bInitInRenameMode = true; }

	bool GetIsPersistent() { return bIsPersistent; }

private:
	virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

	void OnNameChanged(const FText& InText, ETextCommit::Type CommitMethod);

	void OnRemoveClicked();
	void BuildContextMenu(FMenuBuilder& Builder);

private:
	TSharedPtr<SExtendedSlateBorder> Background;
	TSharedPtr<SVerticalTextBlock> Text;
	TSharedPtr<SVerticalEditableText> RenameTextBox;

	FLinearColor BaseColor;
	FLinearColor HoverColor;
	FLinearColor SelectedColor;

	TSharedPtr<SPinnedSection> Widget;

	FOnTabClickedSignature OnTabClickedDelegate;
	FOnNameChangedSignature OnNameChangedDelegate;
	FOnRemoveSignature OnRemoveDelegate;

	bool bIsSelected;
	bool bIsPersistent = false;
	bool bInitInRenameMode = false;

};