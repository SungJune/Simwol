// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "Blueprint/USerWidget.h"

AShooterPlayerController::AShooterPlayerController()
{

}
void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// HUDOverlayClass TSubclassOf ������ Ȯ��
	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}

}