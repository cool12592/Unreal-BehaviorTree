// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "PlayerCharacter.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerController.h"
//#include "Net/UnrealNetwork.h"
#include "Engine.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	isShowInventory = false;
	Inven_Items_MAP.Add(3, 1);
	Inven_Items_MAP.Add(1, 3);
	QuickSlotItemArray.Init(0, 2);
}

void UInventoryComponent::MyInventoryCopyFunc(const UInventoryComponent* other , class AMyPlayerController* controller)
{
	Inven_Items_MAP.Empty();
	if (other==false) return;
	Inven_Items_MAP = other->Inven_Items_MAP;
	NowWeaponWearing_number = other->NowWeaponWearing_number;
	QuickSlotItemArray = other->QuickSlotItemArray;
	//if(!myOwner && controller)
	myOwner = controller;
}

// Called when the game starts

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}




void UInventoryComponent::ShowInventory()
{
	if (isShowInventory == false)
	{
		if (!uiInventoryWidget)
		{
			FName path = TEXT("/Game/My__/UI/Inven/Inventory.Inventory_C");
			UClass* GeneratedBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *path.ToString()));
			uiInventoryWidget = CreateWidget<UUserWidget>(GetWorld(), GeneratedBP);

		}
		

		if(uiInventoryWidget && uiInventoryWidget->IsInViewport()==false)
			uiInventoryWidget->AddToViewport();
		
		if (uiInventoryWidget)
			uiInventoryWidget->SetVisibility(ESlateVisibility::Visible);

		if (myOwner)
		myOwner->bShowMouseCursor = true;

		isShowInventory = true;
	}


}

void UInventoryComponent::CloseInventory()
{
	if (isShowShop) return;

	if (uiInventoryWidget)
		uiInventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	
	if (myOwner)
	myOwner->bShowMouseCursor = false;
	isShowInventory = false;
}


void UInventoryComponent::UseItem(int ItemID)
{
	if (myOwner == false)
		return;
	
	auto* player = Cast<APlayerCharacter>(myOwner->GetCharacter());
	if (player == false)
		return;

	bool me = false;
	if (myOwner == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		me = true;

	if (me && Inven_Items_MAP.Find(ItemID) == false)
		return;

	if (me && player->enable_attack == false)
		return;

	ItemType itemType = static_cast<ItemType>(ItemID);
	switch (itemType)
	{
	case ItemType::HP_Potion:
		if(me)
			Inven_Items_MAP[ItemID]--;
		player->HP_HEAL(20.F);

		break;
	case ItemType::Stamina_Potion:
		if(me)
			Inven_Items_MAP[ItemID]--;
		player->STAMINA_HEAL(20.F);

		break;
	case ItemType::BasicWeapon:
	{	
		NowWeaponWearing_number = 3;
		FOutputDeviceNull pAR;
		player->CallFunctionByNameWithArguments(TEXT("TempWeaponChange1"), pAR, nullptr, true);
		if (me && uiInventoryWidget)
		{
			FOutputDeviceNull pAR2;
			uiInventoryWidget->CallFunctionByNameWithArguments(TEXT("SetWeaponUI_POS"), pAR2, nullptr, true);
		}
		break;
	}
	case ItemType::SpecialWeapon:
	{
		NowWeaponWearing_number = 4;
		FOutputDeviceNull pAR;
		player->CallFunctionByNameWithArguments(TEXT("TempWeaponChange2"), pAR, nullptr, true);
		if (me && uiInventoryWidget)
		{
			FOutputDeviceNull pAR2;
			uiInventoryWidget->CallFunctionByNameWithArguments(TEXT("SetWeaponUI_POS"), pAR2, nullptr, true);
		}
		break;
	}
	default:
		break;
	}
	if (me && Inven_Items_MAP[ItemID] <= 0)
		Inven_Items_MAP.Remove(ItemID);
	if (me&& uiInventoryWidget)
	{
		FOutputDeviceNull pAR;
		uiInventoryWidget->CallFunctionByNameWithArguments(TEXT("UpdateMyInventory"), pAR, nullptr, true);
	}
}
