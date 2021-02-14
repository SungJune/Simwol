// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset); 
	}
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	if (WBossHealthBar)
	{
		BossHealthBar = CreateWidget<UUserWidget>(this, WBossHealthBar);
		if (BossHealthBar)
		{
			BossHealthBar->AddToViewport();
			BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}
		FVector2D Boss_Alignment(0.f, 0.f);
		BossHealthBar->SetAlignmentInViewport(Boss_Alignment);
	}
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if(EnemyHealthBar)
	{ 
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}
void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::Boss_DisplayHealthBar()
{
	if (BossHealthBar)
	{
		bBossHealthBarVisible = false;
		BossHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}
void AMainPlayerController::Boss_RemoveHealthBar()
{
	if (BossHealthBar)
	{
		bBossHealthBarVisible = false;
		BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);
		PositionInViewport.Y -= 85.f;

		FVector2D SizeInViewport(300.f, 25.f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

/*
//2021 01 17 �߰�
void AMainPlayerController::Disable()
{
	DisableInput(Cast<APlayerController>(this));
}
void AMainPlayerController::Enable()
{
	EnableInput(Cast<APlayerController>(this));
}
*/