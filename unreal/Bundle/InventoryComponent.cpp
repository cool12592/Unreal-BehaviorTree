// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "PlayerCharacter.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "MyPlayerController.h"
#include "Engine.h"
// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	isShowInventory = false;
	AddToInventory(ItemType::BasicWeapon, 1);
	AddToInventory(ItemType::HP_Potion, 3);
	wearingWeapon = ItemType::BasicWeapon;
	QuickSlotItemArray.Init(0, 2);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UInventoryComponent::MyInventoryCopyFunc(const UInventoryComponent* other , class AMyPlayerController* controller)
{
	InvenItemMap.Empty();
	if (other==false) return;
	InvenItemMap = other->InvenItemMap;
	wearingWeapon = other->wearingWeapon;
	QuickSlotItemArray = other->QuickSlotItemArray;
	//if(!myOwner && controller)
	myOwner = controller;
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

void UInventoryComponent::AddToInventory(ItemType itemType, int count)
{
	if (InvenItemMap.Contains(itemType))
		InvenItemMap[itemType] += count;
	else
		InvenItemMap.Add(itemType, 1);
}

void UInventoryComponent::UseItem(ItemType itemType)
{
	if (myOwner == false)
		return;

	auto* player = Cast<APlayerCharacter>(myOwner->GetCharacter());
	if (player == false)
		return;

	bool isMine = false;
	if (myOwner == UGameplayStatics::GetPlayerController(GetWorld(), 0))
		isMine = true;

	if (isMine && InvenItemMap.Find(itemType) == false)
		return;

	if (isMine && player->enable_attack == false)
		return;

	switch (itemType)
	{
	case ItemType::HP_Potion:
		if (isMine)
			InvenItemMap[itemType]--;
		player->HP_HEAL(20.F);
		break;

	case ItemType::Stamina_Potion:
		if (isMine)
			InvenItemMap[itemType]--;
		player->STAMINA_HEAL(20.F);
		break;

	case ItemType::BasicWeapon:
	{
		wearingWeapon = ItemType::BasicWeapon;
		FOutputDeviceNull pAR;
		player->CallFunctionByNameWithArguments(TEXT("WeaponChange"), pAR, nullptr, true);
		if (isMine && uiInventoryWidget)
		{
			FOutputDeviceNull pAR2;
			uiInventoryWidget->CallFunctionByNameWithArguments(TEXT("SetEquipUI"), pAR2, nullptr, true);
		}
		break;
	}
	case ItemType::SpecialWeapon:
	{
		wearingWeapon = ItemType::SpecialWeapon;
		FOutputDeviceNull pAR;
		player->CallFunctionByNameWithArguments(TEXT("WeaponChange"), pAR, nullptr, true);
		if (isMine && uiInventoryWidget)
		{
			FOutputDeviceNull pAR2;
			uiInventoryWidget->CallFunctionByNameWithArguments(TEXT("SetEquipUI"), pAR2, nullptr, true);
		}
		break;
	}
	default:
		break;
	}
	if (isMine && InvenItemMap[itemType] <= 0)
		InvenItemMap.Remove(itemType);

	if (isMine && uiInventoryWidget)
	{
		FOutputDeviceNull pAR;
		uiInventoryWidget->CallFunctionByNameWithArguments(TEXT("UpdateMyInventory"), pAR, nullptr, true);
	}
}