// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ItemBase.generated.h"

USTRUCT(BlueprintType)
struct FItem_Info :public FTableRowBase
{
	GENERATED_BODY()
public:
	FItem_Info()
	{
		ItemName = FText::FromString("ItemName");
		ItemDescription = FText::FromString("This Item is ....");
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemID;

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
};
