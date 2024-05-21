#include "UI/ManageCityWidget.h"

UManageCityWidget::UManageCityWidget(const FObjectInitializer& ObjeectInitializer) : UUserWidget(ObjeectInitializer)
{

}

void UManageCityWidget::SetCastlePtr(ACastleLevelInstance* ptr)
{
	castlePtr = ptr;
}