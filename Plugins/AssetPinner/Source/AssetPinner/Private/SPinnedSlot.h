#pragma once

#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Enums.h"
#include "Structs.h"

class SExtendedSlateBorder;
class FAssetThumbnail;
class UPinnedAssetSubsystem;
class SPinnedSection;
struct FPinnedAssetData;

class SPinnedSlot : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPinnedSlot)
		: _Data()
		, _AssetData()
		, _Size(100)
		, _Outer(nullptr)
		{
		}
		SLATE_ARGUMENT(FPinnedAssetData, Data)
		SLATE_ARGUMENT(FAssetData, AssetData)
		SLATE_ARGUMENT(float, Size)
		SLATE_ARGUMENT(TWeakPtr<SPinnedSection>, Outer)
	SLATE_END_ARGS();

	SPinnedSlot();

	void Construct(const FArguments& InArgs);

	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void SetSize(int Width, int Height);
	FString GetAssetPath() const { return AssetPath; }

private:
	// -- FUNCTIONS --
	void BuildSlotContextMenu(FMenuBuilder& Builder);
	void GenerateTabSubMenu(FMenuBuilder& Builder);

	void Pin();
	bool CanPin();
	void Unpin();
	void LocateInBrowser();
	void SwitchTab(int Index);

private:
	// -- VARIABLES --
	FString AssetPath;
	EPathType PathType;
	FAssetData AssetData;

	TSharedPtr<SBox> SizeBox;
	TSharedPtr<SExtendedSlateBorder> Background;
	TSharedPtr<STextBlock> Name;
	TSharedPtr<SBox> ThumbnailHolder;
	TSharedPtr<FAssetThumbnail> Thumbnail;
	TWeakPtr<SPinnedSection> OuterWidget;

	FLinearColor BaseColor;
	FLinearColor HoverColor;
};