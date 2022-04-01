// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:						    
	AShooterPlayerController();

protected:
	virtual void BeginPlay() override;

private:
	// 전체 HUD 오버레이 블루프린트 클래스 참조
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	//생성 후 HUD 오버레이 위젯을 보관하는 변수
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Widget, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;

};
