// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemBase.h"
#include "InventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class NOTEBOOK_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
		
public:	
	
	// Sets default values for this component's properties
	UInventoryComponent();
	//UInventoryComponent(const UInventoryComponent& other);
	UFUNCTION(BlueprintCallable)
	void MyInventoryCopyFunc(const UInventoryComponent* other, class AMyPlayerController* controller);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MyUI")
		void ShowInventory();
	UFUNCTION(BlueprintCallable, Category = "MyUI")
		void CloseInventory();

	UFUNCTION(BlueprintCallable)
		void AddToInventory(ItemType itemType, int count);

	TSubclassOf<class UUserWidget>uiInventoryBPClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UUserWidget* uiInventoryWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isShowInventory;

	TSubclassOf<class UUserWidget>uiShopBPClass;
	UPROPERTY()
		UUserWidget* uiShopWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isShowShop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<ItemType, int32>  InvenItemMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class AMyPlayerController* myOwner;
	
	UFUNCTION(BlueprintCallable, Category = "MyUI")
		void UseItem(ItemType itemType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ItemType wearingWeapon;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> QuickSlotItemArray;
};
