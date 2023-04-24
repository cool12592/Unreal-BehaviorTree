// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType)
enum class ItemType : uint8
{
	HP_Potion = 0   UMETA(DisplayName = "HP_Potion"),
	Stamina_Potion = 1  UMETA(DisplayName = "Stamina_Potion"),
	BasicWeapon = 2     UMETA(DisplayName = "BasicWeapon"),
	SpecialWeapon = 3   UMETA(DisplayName = "SpecialWeapon"),
};

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

	TSubclassOf<class UUserWidget>uiInventoryBPClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UUserWidget* uiInventoryWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isShowInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isShowShop;
	//PlayerItem Array
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> MyItemArray_ch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<int32, int32>  Inven_Items_MAP;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class AMyPlayerController* myOwner;
	
	
	UFUNCTION(BlueprintCallable, Category = "MyUI")
		void UseItem(int ItemID);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int NowWeaponWearing_number=3;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> QuickSlotItemArray;
	
};
