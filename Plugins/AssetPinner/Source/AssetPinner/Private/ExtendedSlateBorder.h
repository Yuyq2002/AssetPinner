#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SExtendedSlateBorder : public SBorder
{
public:

	SLATE_BEGIN_ARGS(SExtendedSlateBorder)
		: _AllowContextMenu(true)
		, _ContextMenuExtender()
		{
		};

	/** Whether the context menu can be opened  */
	SLATE_ATTRIBUTE(bool, AllowContextMenu)

	/** Delegate to call before a context menu is opened. User returns the menu content or null to the disable context menu */
	SLATE_EVENT(FOnContextMenuOpening, OnContextMenuOpening)

	/** Menu extender for the right-click context menu */
	SLATE_EVENT(FMenuExtensionDelegate, ContextMenuExtender)

	SLATE_END_ARGS();

	/** See the AllowContextMenu attribute */
	void SetAllowContextMenu(const TAttribute< bool >& InAllowContextMenu);

	void Construct(const FArguments& InArgs);

protected:
	/** Whether to disable the context menu */
	TAttribute< bool > AllowContextMenu;

	/** Delegate to call before a context menu is opened */
	FOnContextMenuOpening OnContextMenuOpening;

	/** Menu extender for right-click context menu */
	TSharedPtr<FExtender> MenuExtender;
};