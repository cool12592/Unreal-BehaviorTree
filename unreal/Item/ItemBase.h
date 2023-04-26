// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class ItemType : uint8
{
	HP_Potion = 0		UMETA(DisplayName = "HP_Potion"),
	Stamina_Potion = 1  UMETA(DisplayName = "Stamina_Potion"),
	BasicWeapon = 2     UMETA(DisplayName = "BasicWeapon"),
	SpecialWeapon = 3   UMETA(DisplayName = "SpecialWeapon"),
	none = 4			UMETA(DisplayName = "none"),
};

USTRUCT(BlueprintType)
struct FItem_Info :public FTableRowBase
{
	GENERATED_BODY()
public:
	FItem_Info()
	{
		ItemName = FText::FromString("ItemName");
		ItemDescription = FText::FromString("This Item is ....");
		isStackable = false;
		isUsageable = false;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ItemThumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemGold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isStackable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isUsageable;

};
UCLASS()
class NOTEBOOK_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
