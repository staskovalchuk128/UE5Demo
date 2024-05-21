// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Buttons/AmmoSelectionBtn.h"
#include "Components/CanvasPanelSlot.h"

UAmmoSelectionBtn::UAmmoSelectionBtn()
{
	USlateWidgetStyleAsset* btnStyle = ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset>(TEXT("/Script/SlateCore.SlateWidgetStyleAsset'/Game/UI/Buttons/AmmoSelectionBtnStyle.AmmoSelectionBtnStyle'")).Object;
	SButton::FArguments btnDefaults;
	
	btnDefaults.ButtonStyle(btnStyle);
	SetStyle(*btnDefaults._ButtonStyle);
	
}
