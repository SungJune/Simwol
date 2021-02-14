// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OPENRPG_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// UMG Editor Asset
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;
	
	// Create HUD Widgets
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "widgets")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere,BlueprintReadwrite,Category = "widgets")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "widgets")
	UUserWidget* EnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "widgets")
	TSubclassOf<UUserWidget> WBossHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "widgets")
	UUserWidget* BossHealthBar;


	//UFUNCTION(BlueprintCallable, Category = "Game | Player")
	//bool IsInputKeyDown(FKey Key) const;

	bool bEnemyHealthBarVisible;
	bool bBossHealthBarVisible;

	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();

	void Boss_DisplayHealthBar();
	void Boss_RemoveHealthBar();

	FVector EnemyLocation;
	FVector BossEnemyLocation;

	//void Disable();
	//void Enable();
	

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
