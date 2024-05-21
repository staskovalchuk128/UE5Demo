/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Bullets/BulletsManager.h"
#include "MeleeWeapons/MeleeWeaponsManager.h"
#include "SelectWeaponWidget.generated.h"

class UCanvasPanel;
class UButton;
class UWidgetTree;
class UTexture2D;
class UInventoryActorComponent;

UCLASS()
class MYPROJECT_API USelectWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	USelectWeaponWidget(const FObjectInitializer& ObjectInitializer);
	void Init(UInventoryActorComponent* inventory, BulletUsageType usageType);

	virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void OnWidgetRebuilt() override;

	const WeaponSelectionItem& GetActiveItem();
protected:
	UFUNCTION(BlueprintCallable)
	FEventReply RedirectMouseMoveToWidget(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, const FVector2D mouseScreenPos);

	UFUNCTION(BlueprintCallable)
	void SelectNextItem();

	UFUNCTION(BlueprintCallable)
	void SelectPrevItem();
private:
	int numItems;
	UTexture2D* btnTexture, *btnActiveTexture;
	FButtonStyle btnStyle;
	FSlateBrush btnNormalStyle, btnActiveStyle;
	std::vector<WeaponSelectionItem> items;

	UCanvasPanel* canvasItems;
	UButton* CreateBtn(UCanvasPanel* canvas, FVector2D position, FVector2D size, const WeaponSelectionItem& item);

	int GetActiveItemIndex();

	void DeactivateAll();
	void ActivateBtn(WeaponSelectionItem& item);

	bool CheckRayCollision(FVector4 rect, FVector4 ray);
};
