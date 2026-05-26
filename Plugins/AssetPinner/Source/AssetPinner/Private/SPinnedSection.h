#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Enums.h"

class SExtendedSlateBorder;
class UPinnedAssetSubsystem;
class SPinnedSlot;
class SScrollBox;
class SWrapBox;
struct FPinnedAssetData;

class SPinnedSection : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPinnedSection)
		{
		}
	SLATE_END_ARGS();

	SPinnedSection();

	void Construct(const FArguments& InArgs);

	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;

	void ClearPinnedSlots();
	void AddPinnedSlot(TSharedRef<SPinnedSlot> NewPinnedSlot);
	void SetEnableScrolling(bool IsEnabled);

	TSharedPtr<SWidget> BuildSectionWidget();

private:
	TSharedPtr<SScrollBox> ScrollBox;
	TSharedPtr<SWrapBox> WrapBox;

};

class SHistorySection : public SPinnedSection
{
public:
	SLATE_BEGIN_ARGS(SHistorySection)
		{
		}
	SLATE_END_ARGS();

	SHistorySection();

	void Construct(const FArguments& InArgs);

private:
	FReply OnClearClicked();

};