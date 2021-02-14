// Fill out your copyright notice in the Description page of Project Settings.

// HUD Cpp ���� 

// HUD �ۼ��� ���־� ��Ʃ��� ������Ʈ �̸� �ؿ��ִ� Build.cs���� �߰��Ұ� 
//PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" , "UMG"});
//PrivateDependencyModuleNames.AddRange(new string[]{ "Slate", "SlateCore" });

#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//  BP_HUD ����	�������� ������ �����ߴ��� Ȯ���ϱ� ���ظ��� ����
	
	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
	}
	// ȭ�鿡 �� �� ���
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
		// View ����
		FVector2D Alignment(0.f, 0.f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
		
	}
	
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
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


void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyHealthBar)
	{
		// ������ ��ġ�� ã�ƾ��� 
		// �׷������ؼ��� 3D ������ �������� ȭ������ ��ȯ �ϱ�����
		//x,y ��ǥ�� ��Ÿ���� 2���� Vector ���
		FVector2D PositionViewport;

		// ������ġ�� ��ũ���� �����Ҽ��ִ±�� 
		ProjectWorldLocationToScreen(EnemyLocation, PositionViewport);
		PositionViewport.Y -= 85.f;

		// ����� ������ ũ�� ����
		FVector2D SizeViewport(300.f, 25.f);

		EnemyHealthBar->SetPositionInViewport(PositionViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeViewport);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputModeGameAndUI;
		SetInputMode(InputModeGameAndUI);
		bShowMouseCursor = true;

	}
}
void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameModeOnly();
		bShowMouseCursor = false;

		bPauseMenuVisible = false;
		//PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}
void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}
void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}

