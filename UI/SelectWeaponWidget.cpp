#include "UI/SelectWeaponWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/BackgroundBlur.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Engine/Font.h"
#include "Blueprint/WidgetTree.h"
#include "Misc/OutputDeviceNull.h"

#include "UI/Buttons/AmmoSelectionBtn.h"

#include "Components/CanvasPanelSlot.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Engine/GameViewportClient.h"

#include "Bullets/GameBulletActor.h"
#include "MeleeWeapons/MeleeWeaponsManager.h"

#include "ResourceManager.h"

#include "Player/InventoryActorComponent.h"


USelectWeaponWidget::USelectWeaponWidget(const FObjectInitializer& ObjeectInitializer)
	:UUserWidget(ObjeectInitializer)
{
	btnTexture = ConstructorHelpers::FObjectFinder<UTexture2D>(TEXT("/Script/Engine.Texture2D'/Game/Textures/Buttons/ammoSelection.ammoSelection'")).Object;
	btnActiveTexture = ConstructorHelpers::FObjectFinder<UTexture2D>(TEXT("/Script/Engine.Texture2D'/Game/Textures/Buttons/ammoSelected.ammoSelected'")).Object;
	items = {};

}

void USelectWeaponWidget::Init(UInventoryActorComponent* inventory, BulletUsageType usageType)
{
	TArray<InventorySlotItemType> selectionTypes = { InventorySlotItemType::BULLETS };
	if (usageType == BulletUsageType::FOR_MEN)
	{
		selectionTypes = { InventorySlotItemType::MAN_THROWING_WEAPON, InventorySlotItemType::MAN_MELEE_WEAPON };
	}

	const TArray<FInventorySlotItem> &invItems = inventory->GetItemsByTypes(selectionTypes);

	for (auto &it: invItems)
	{
		if (IsMeleeWeapon(it.item.objectType))
		{
			items.push_back(WeaponSelectionItem(MeleeWeaponsManager::GetWeaponData(it.item.objectType)));
		}
		else
		{
			items.push_back(WeaponSelectionItem(BulletsManager::GetBulletData(it.item.objectType), it.quantity));
		}
	}

	//Load textures

	for (auto& it : items)
	{
		if (it.isMelee)
		{
			it.texture = ResourceManager::Get<UTexture2D>(it.meleeWeaponData.texture2dName);
		}
		else
		{
			it.texture = ResourceManager::Get<UTexture2D>(it.bulletData.texture2dName);
		}
	}
}

void USelectWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USelectWeaponWidget::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

}

TSharedRef<SWidget> USelectWeaponWidget::RebuildWidget()
{
	TSharedRef<SWidget> widget = Super::RebuildWidget();

	
	UWidget* rootCanvas = Cast<UWidget>(WidgetTree->FindWidget("RootCanvas"));
	rootCanvas->ForceLayoutPrepass();

	//Setup btn styles
	{
		btnNormalStyle.SetResourceObject(btnTexture);
		btnActiveStyle.SetResourceObject(btnActiveTexture);

		btnStyle.SetNormal(btnNormalStyle);
		btnStyle.SetHovered(btnActiveStyle);
		btnStyle.SetPressed(btnActiveStyle);
		btnStyle.SetDisabled(btnActiveStyle);
	}

	canvasItems = Cast<UCanvasPanel>(WidgetTree->FindWidget("CanvasItems"));

	FGeometry geom = rootCanvas->GetPaintSpaceGeometry();
	FVector2D canvasSize = rootCanvas->GetDesiredSize();

	FVector2D btnSize(150.0f, 150.0f);
	float itemOffsetX = canvasSize.X / 2.0f - btnSize.X / 2.0f, itemOffsetY = canvasSize.Y / 2.0f - btnSize.Y / 2.0f;

	numItems = (int)items.size();

	float generalOffset = 270.0f;


	float angle = 2.0f * PI / (float)(numItems);
	FVector2D pos;

	for (int i = 0; i < numItems; i++)
	{
		
		pos.X = sinf((float)i * angle) * generalOffset + itemOffsetX;
		pos.Y = abs(cosf((float)i * angle) * generalOffset - itemOffsetY);

		UButton* btn = CreateBtn(canvasItems, pos, btnSize, items[i]);
		items[i].btnPtr = btn;
	}

	if(items.size() > 0) ActivateBtn(items[0]);
	
	return widget;
}

UButton* USelectWeaponWidget::CreateBtn(UCanvasPanel* canvas, FVector2D position, FVector2D size, const WeaponSelectionItem& item)
{
	UButton* btn;
	btn = WidgetTree->ConstructWidget<UButton>(UAmmoSelectionBtn::StaticClass());
	canvas->AddChild(btn);

	Cast<UCanvasPanelSlot>(btn->Slot)->SetPosition(position);
	Cast<UCanvasPanelSlot>(btn->Slot)->SetSize(size);

	UImage* img = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	img->SetBrushResourceObject(item.texture);
	canvas->AddChild(img);
	
	Cast<UCanvasPanelSlot>(img->Slot)->SetPosition(position + (size - size * 0.6f) / 2.0f);
	Cast<UCanvasPanelSlot>(img->Slot)->SetSize(size * 0.6f);

	UTextBlock* countText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	std::wstring countStr = item.count == -5 ? L"∞" : std::to_wstring(item.count);
	if (item.isMelee) countStr = L"";
	countText->SetText(FText::FromString(countStr.c_str()));
	canvas->AddChild(countText);

	Cast<UCanvasPanelSlot>(countText->Slot)->SetPosition(FVector2D(position.X + size.X / 2.55f, position.Y + (size.Y - size.Y * 0.65f) / 0.5f));
	Cast<UCanvasPanelSlot>(countText->Slot)->SetSize(size * 0.6f);


	return btn;
}



FEventReply USelectWeaponWidget::RedirectMouseMoveToWidget(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, const FVector2D mouseScreenPos)
{
	FEventReply reply;
	reply.NativeReply = NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.GetCursorDelta().Size() > 0)
	{
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

		FVector2D mouseDelta = mouseScreenPos - ViewportSize / 2.0f;
		mouseDelta.Normalize();

		FVector2D maxSize = InGeometry.GetAbsoluteSize();

		FVector4 ray = FVector4(mouseScreenPos.X, mouseScreenPos.Y,
			mouseScreenPos.X + maxSize.X * mouseDelta.X, mouseScreenPos.Y + maxSize.Y * mouseDelta.Y);

		size_t itemsCount = items.size();
		int nextItemIndex = GetActiveItemIndex() + 1;


		if (nextItemIndex > itemsCount - 1)
		{
			nextItemIndex = 0;
		}


		for (auto &it: items)
		{
			FGeometry btnGeom = it.btnPtr->GetPaintSpaceGeometry();
			FGeometry btnCachedGeom = it.btnPtr->GetCachedGeometry();
			FVector2D btnSize = btnGeom.GetAbsoluteSize();

			FVector2D vp;
			FVector2D btnPos;
			USlateBlueprintLibrary::LocalToViewport(GetWorld(), btnCachedGeom, FVector2D(), btnPos, vp);


			FVector4 btnVec = FVector4(btnPos.X, btnPos.Y, btnPos.X + btnSize.X, btnPos.Y + btnSize.Y);
			if (CheckRayCollision(btnVec, ray))
			{
				DeactivateAll();
				ActivateBtn(it);
				break;
			}

		}
	}

	return reply;
}

void USelectWeaponWidget::DeactivateAll()
{
	for (auto &it: items)
	{
		it.isActive = false;
		btnStyle.SetNormal(btnNormalStyle);
		it.btnPtr->SetStyle(btnStyle);
	}
}

void USelectWeaponWidget::ActivateBtn(WeaponSelectionItem& item)
{
	item.isActive = true;
	btnStyle.SetNormal(btnActiveStyle);
	item.btnPtr->SetStyle(btnStyle);

	//Update info about the bullet
	UTextBlock* bulletNameTextBlock = Cast<UTextBlock>(WidgetTree->FindWidget("selectedAmmoName"));
	bulletNameTextBlock->SetText(FText::FromString(item.isMelee ? item.meleeWeaponData.name.c_str() : item.bulletData.name.c_str()));

	//AmmoCountTxt
	UTextBlock* bulletCountTextBlock = Cast<UTextBlock>(WidgetTree->FindWidget("AmmoCountTxt"));
	std::wstring countStr = item.count == -5 ? L"∞" : std::to_wstring(item.count);
	if (item.isMelee) countStr = L"";
	bulletCountTextBlock->SetText(FText::FromString(countStr.c_str()));


	UImage* bulletImgBlock = Cast<UImage>(WidgetTree->FindWidget("selectedAmmoImg"));
	bulletImgBlock->SetBrushResourceObject(item.texture);


	int maxDamage = item.isMelee ? MeleeWeaponsManager::GetMaxDamage() : BulletsManager::GetMaxDamage();
	int damage = item.isMelee ? item.meleeWeaponData.damage : item.bulletData.damage;

	UProgressBar* dmgProgressBar = Cast<UProgressBar>(WidgetTree->FindWidget("DamageBar"));
	dmgProgressBar->SetPercent((float)damage / (float)maxDamage);


}

const WeaponSelectionItem& USelectWeaponWidget::GetActiveItem()
{
	return items[GetActiveItemIndex()];
}

int USelectWeaponWidget::GetActiveItemIndex()
{
	int index = 0;
	size_t itemsCount = items.size();
	for (size_t i = 0; i < itemsCount; i++)
	{
		if (items[i].isActive)
		{
			index = (int)i;
			break;
		}
	}
	return index;
}

bool USelectWeaponWidget::CheckRayCollision(FVector4 rect, FVector4 ray)
{
	// Find min and max X for the segment

	double minX = ray.X;
	double maxX = ray.Z;

	if (ray.X > ray.Z)
	{
		minX = ray.Z;
		maxX = ray.X;
	}

	// Find the intersection of the segment's and rectangle's x-projections

	if (maxX > rect.Z)
	{
		maxX = rect.Z;
	}

	if (minX < rect.X)
	{
		minX = rect.X;
	}

	if (minX > maxX) // If their projections do not intersect return false
	{
		return false;
	}

	// Find corresponding min and max Y for min and max X we found before

	double minY = ray.Y;
	double maxY = ray.W;

	double dx = ray.Z - ray.X;

	if (fabs(dx) > 0.0)
	{
		double a = (ray.W - ray.Y) / dx;
		double b = ray.Y - a * ray.X;
		minY = a * minX + b;
		maxY = a * maxX + b;
	}

	if (minY > maxY)
	{
		double tmp = maxY;
		maxY = minY;
		minY = tmp;
	}

	// Find the intersection of the segment's and rectangle's y-projections

	if (maxY > rect.W)
	{
		maxY = rect.W;
	}

	if (minY < rect.Y)
	{
		minY = rect.Y;
	}

	if (minY > maxY) // If Y-projections do not intersect return false
	{
		return false;
	}

	return true;
}

void USelectWeaponWidget::SelectNextItem()
{
	int nextItemIndex = GetActiveItemIndex() + 1;
	nextItemIndex = nextItemIndex > numItems - 1 ? 0 : nextItemIndex;
	DeactivateAll();
	ActivateBtn(items[nextItemIndex]);
}

void USelectWeaponWidget::SelectPrevItem()
{
	int nextItemIndex = GetActiveItemIndex() - 1;
	nextItemIndex = nextItemIndex < 0 ? numItems - 1 : nextItemIndex;
	DeactivateAll();
	ActivateBtn(items[nextItemIndex]);
}