// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapon.h"
#include "Ammo.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Shooter.h"
#include "BulletHitInterface.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

#pragma once  
// Sets default values
AShooterCharacter::AShooterCharacter()	:
	// 조준/비 조준에 대한 기본 비율
	BaseTurnRate(45.f),
	BaseLookupRate(45.f),
	
	// 보기 값에 대한 카메라 필드
	CameraDefaultFOV(0.f), // BeginPlay 에서 설정
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// 마우스 모양 감도 스케일 팩터
	MouseHipTurnRate(1.0f),
	MouseHipLookupRate(1.0f),
	MouseAimingTurnRate(0.6f),
	MouseAimingLookupRate(0.6f),
	// 무기를 조준할 때 true
	bAiming(false),
	// 조준/비 조준 회전율 
	HipTurnRate(90.f),
	HipLookupRate(90.f),
	AimingTurnRate(20.f),
	AimingLookupRate(20.f),
	//십자선 확산 계수
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	// 자동 총 발사 속도
	bShouldFire(true),
	bFireButtonPressed(false),
	// 총알 발사 타이머 변수
	ShooterTimeDuration(0.05f),
	bFiringBullet(false),
	// 항목 추적 변수
	bShouldTraceForItem(false),
	OverlappedItemCount(0),
	//카메라 인터프 위치 변수
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	// 시작 탄약 수
	Starting9mmAmmo(90),
	StartingARAmmo(120),
	//전투 변수
	CombatState(ECombatState::ECS_Unoccupied),
	bCrouching(false),
	BaseMovementSpeed(650.f),
	CrouchMovementSpeed(300.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	bAimingButtonPressed(false),
	// 소리 타이머 설정 
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f),
	// 아이콘 애니메이션 속성
	HighlightSlot(-1),
	Health(100.f),
	MaxHealth(100.f),
	StunChange(0.5f),
	bDead(false)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러가 움직일때마다 같이 움직임 
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 컨트롤러가 회전할떄 회전하지않음 컨트롤러가 카메라에만 영향을 끼침 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// 입력하는 방향으로 캐릭터가 움직임 그리고 회전을할때의 회전율   
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// HandSceneComponent 생성
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	// 보간 구성요소 생성
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Component"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 1"));
	InterpComp1->SetupAttachment(GetFollowCamera());

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 2"));
	InterpComp2->SetupAttachment(GetFollowCamera());

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 3"));
	InterpComp3->SetupAttachment(GetFollowCamera());

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 4"));
	InterpComp4->SetupAttachment(GetFollowCamera());

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 5"));
	InterpComp5->SetupAttachment(GetFollowCamera());

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Component 6"));
	InterpComp6->SetupAttachment(GetFollowCamera());

	//AbilityComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilityComp"));
	

}

float AShooterCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
		auto EnemyController = Cast<AEnemyController>(EventInstigator);
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName(TEXT("CharacterDead")), true);
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	// 기본 무기를 생성하고 손에 부착 
	EquipWeapon(SpawnDefaultWeapon());
	//인벤토리에 현재 장착아이탬 표시 
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->SetCharacter(this);

	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	// 각 interp 위치에 대해 FInterpLocation struce를 배열에 추가 하여 만듬 
	InitializeInterpLocation();


}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// 컨트롤러 회전을 얻음 그리고 해당 구성요소를 가져와 새로운 회전자로 초기화
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator RotationYaw{ 0,Rotation.Yaw,0 };

		// 회전 행렬 해당 행열의 축 중 하나를 얻어옴 
		const FVector Direction{ FRotationMatrix{RotationYaw}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// 컨트롤러 회전을 얻음 그리고 해당 구성요소를 가져와 새로운 회전자로 초기화
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator RotationYaw{ 0,Rotation.Yaw,0 };

		// 회전 행렬 해당 행열의 축 중 하나를 얻어옴 
		const FVector Direction{ FRotationMatrix{RotationYaw}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookupRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Value)
{
	float TurnScaleFactor;

	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}
	AddControllerYawInput(Value *TurnScaleFactor);
}

void AShooterCharacter::Lookup(float Value)
{
	float LookupScaleFactor;

	if (bAiming)
	{
		LookupScaleFactor = MouseAimingLookupRate;
	}
	else
	{
		LookupScaleFactor = MouseHipLookupRate;
	}
	AddControllerPitchInput(Value *LookupScaleFactor);
}

void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		PlayFireSound();
		SendBullet();
		PlayGunFireMontage();
		// 무기의 탄약에서 1 빼기
		EquippedWeapon->DecrementAmmo();

		StartFireTimer();

		// 무기의 유형이 권총일때 
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			// 슬라이드 타이머 시작 
			EquippedWeapon->StartSlideTimer();
		}
	}
	//십자선에 대한 총알 발사 타이머 시작
	StartCrosshairBulletFire();
}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading && CombatState != ECombatState::ECS_Equipping && CombatState!=ECombatState::ECS_Stunned )
	{
		Aim();
	}
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	// 현재 카메라 시야 설정 
	if (bAiming)
	{
		// FOV 확대를 위해 보간
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);

	}
	else
	{
		// 기본 FOV로 보간
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);

}
 // 십자선 스프레드 계산
void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	// 십자선 속도 계수 계산
	FVector2D WalkSpeedRange(0.f, 600.f);
	FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// 	공기 계수에서 십자선 계산
	if (GetCharacterMovement()->IsFalling()) // is in air 
	{
		// 공중에서 십자선을 천천히 펼침
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else // 캐릭터가 바닥에 있을때 
	{
		// 지상에 있는 동안 십자선을 빠르게 축소
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// 십자선 조준 계수 계산
	if (bAiming)
	{
		// 빠르게 작은 십자형으로 축소
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		// 에임선을 다시 되돌림 
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	// 0.05초후 발사 
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor-CrosshairAimFactor + CrosshairShootingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairShooterTimer, this, &AShooterCharacter::FinishCrosshairBulletFire, ShooterTimeDuration);
	
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;											
}

void AShooterCharacter::SetLookupRates()
{

	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookupRate = AimingLookupRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookupRate = HipLookupRate;
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector & MuzzleSocketLocation, FHitResult&OutHitResult)
{	
	// 십자형 추적 적중 확인
	FVector OutBeamLocation;
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshair(CrosshairHitResult,OutBeamLocation);

	if (bCrosshairHit)
	{
		// 임시 빔 위치 - 여전히 총에서 추적해야 함
		OutBeamLocation = CrosshairHitResult.Location;

	}
	else
	{
		// 해당 추적의 끝점 
		

	}

	// 두 번째 추적을 수행
	const FVector WeapontraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeapontraceEnd{ MuzzleSocketLocation + StartToEnd* 1.25f };
	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeapontraceStart, WeapontraceEnd, ECollisionChannel::ECC_Visibility);

	if (!OutHitResult.bBlockingHit) // 배럴과 BeamEndPoint 사이의 객체
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}
	return true;

	/*
	if (WeapontraceHit.bBlockingHit) // 배럴과 BeamEndPoint 사이의 객체
	{
		OutBeamLocation = WeapontraceHit.Location;
		return true;
	}
	return false;
	  */

	// 현재 HUD의 화면 크기를 가져옴 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	// 십자선의 GetScreen 공간 위치
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 십자선의 세계 위치 및 방향 가져오기
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)// 디프로젝션에 성공했다
	{
		FHitResult ScreenTranceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		//  빔 포인트를 라인 트레이스 및 포인트로 설정
		OutBeamLocation = End;
		// 십자선 세계 위치에서 바깥쪽으로 추적
		GetWorld()->LineTraceSingleByChannel(ScreenTranceHit, Start, End, ECollisionChannel::ECC_Visibility);

		if (ScreenTranceHit.bBlockingHit) //흔적이 있었는가.
		{

			// 빔 끝점은 이제 추적 적중 위치입니다.
			OutBeamLocation = ScreenTranceHit.Location;
			
		}
		

	}

	
}

bool AShooterCharacter::TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation)
{	
	// 현재 HUD의 화면 크기를 가져옴 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// 십자선의 GetScreen 공간 위치
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// 십자선의 세계 위치 및 방향 가져오기
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Crosshair의 Wold 위치에서 바깥쪽으로 트레이스
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;

	FireWeapon();
}
void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, EquippedWeapon->GetAutoFireRate());
	
}

void AShooterCharacter::AutoFireReset()
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr) return;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed && EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}
	else
	{
		// Reload Weapon
		ReloadWeapon();
	}
	
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItem)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshair(ItemTraceResult, HitLocation);
		const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
		if (TraceHitItem)
		{
			if (HighlightSlot == -1)
			{
				// 현재슬롯을 강조하지않고 하나만을 강조
				HighlightInventorySlot();
			}

		}
		else
		{
			// 현재 강조되고있는 슬롯 하이라이트 
			if (HighlightSlot != -1)
			{
				// 하나의 하이라이트 인벤토리 슬롯을 호출
				UnHighlightInventorySlot();

			}
		}

		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);

			// 만약 그렇다면, 우리가 아이템을 추적하고 명중하고 그것이 성공적으로 캐스팅된다면
			//항목으로 설정하고 항목 상태가 장비 해석인 경우 Trece 히트 항목을 설정
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				// 항목의 픽업 위젯 표시
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCusomDepth();

				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					// 인벤토리가 꽉 차있을때
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					//인벤토리가 여유가 있을때 
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}

			// 해당 프레임에 부딪혔을경우 
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// 이 프레임에서 마지막 프레임과 다른 아이탬 을 치고 있거나 아이탬 이 null입니다.
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}

			//다음 프레임에 대한 HitItem에 대한 참조 저장
			TraceHitItemLastFrame = TraceHitItem;
		} 
	}
	else if (TraceHitItemLastFrame)
	{
		// 더 이상 항목이 겹치지 않을때  항목 마지막 프레임에 위젯이 표시되지 않도록 함
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}


AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	// TSubclassOf 변수 확인
	if (DefaultWeaponClass)
	{
		// 무기생성 
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon * WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightSocket"));

		if (HandSocket)
		{
			// 손에 무기를 장착시킴 
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (EquippedWeapon == nullptr)
		{
			// -1 = 아직 장착중인 무기가없으면 아이콘애니메이션 을 전송할 필요없음 
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());

		}
		else if(!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		// 무기를 새로 생성한 무기로 장착 
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}

}
// 무기 교체 ( 드롭 형식 ) 
void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
	

}

void AShooterCharacter::SelectButtonPressed()																					   
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this,true);
		TraceHitItem = nullptr;
		
	}
}
void AShooterCharacter::SelectButtonReleased()
{

}


void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap,true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

// 탄약 값으로 탄약 지도 초기화
void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;		
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

void AShooterCharacter::InterpCapsuleHalfHegiht(float DeltaTime)
{
	float TargetCapsuleHalfHeight;
	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHegiht{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),TargetCapsuleHalfHeight,DeltaTime,20.f) };

	// 웅크리면 음수, 서 있으면 양수
	const float DeltaCpasuleHalfHeight{ InterpHalfHegiht - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MashOffset{ 0.f,0.f,-DeltaCpasuleHalfHeight };
	GetMesh()->AddLocalOffset(MashOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHegiht);
}

void AShooterCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}

}

void AShooterCharacter::PickupAmmo(class AAmmo* Ammo)
{
	// AmmoMap에 Ammo의 AmmoType이 포함되어 있는지 확인하십시오.
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// 탄약 유형에 대한 AmmoMap에서 탄약 양 확인
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();
		
		// 지도에서 이 유형의 탄약 양을 설정합니다.
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;

		if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
		{
			// 총에 탄약이 비어 있는지 확인
			if (EquippedWeapon->GetAmmo() == 0)
			{
				ReloadWeapon();
			}
		}
	}
	Ammo->Destroy();
}

void AShooterCharacter::InitializeInterpLocation()
{
	FInterpLocation WeaponLocation{ WeaponInterpComp,0 };
	InterpLocation.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ InterpComp1,0 };
	InterpLocation.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ InterpComp2,0 };
	InterpLocation.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ InterpComp3,0 };
	InterpLocation.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ InterpComp4,0 };
	InterpLocation.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ InterpComp5,0 };
	InterpLocation.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ InterpComp6,0 };
	InterpLocation.Add(InterpLoc6);
}

void AShooterCharacter::FKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AShooterCharacter::OneKeyPressed()
{
	if(EquippedWeapon->GetSlotIndex() == 1 ) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);

}

void AShooterCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AShooterCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AShooterCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AShooterCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

// 새 아이탬 장착 설정 함수 
void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{

	const bool bChanExchangeItems = (CurrentItemIndex != NewItemIndex) && 
		(NewItemIndex < Inventory.Num()) && 
		(CombatState == ECombatState::ECS_Unoccupied || CombatState == ECombatState::ECS_Equipping);

	if (bChanExchangeItems)
	{
		if (bAiming)
		{
			StopAiming();
		}
		auto OldEquippedWeapon = EquippedWeapon;
		auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon);

		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);									  

		CombatState = ECombatState::ECS_Equipping;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}

		NewWeapon->PlayEquipSound(true);
	}

	
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	if (Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}
	return -1; // 인벤토리가 가득찼을때 반환
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot{ GetEmptyInventorySlot() };
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightSlot = EmptySlot;
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightSlot, false);
	HighlightSlot = -1;
}

void AShooterCharacter::Stun()
{
	if (Health <= 0.f)return;
	CombatState = ECombatState::ECS_Stunned;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

EPhysicalSurface AShooterCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f,0.f,-400.f) };

	// F쿼리 충돌
	FCollisionQueryParams QueryParams;
	// 반환으로 인하여 라인트레이스 추적 가능 
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);
	
	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

void AShooterCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (bAimingButtonPressed)
	{
		Aim();
	}
}

void AShooterCharacter::Die()
{
	bDead = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocation.Num(); i++)
	{
		if (InterpLocation[i].ItemCount < LowestCount)
		{
			LowestIndex = 1;
			LowestCount = InterpLocation[i].ItemCount;
		}
	}

	return LowestIndex;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 조준할 때 확대/축소를 위한 핸들 보간
	CameraInterpZoom(DeltaTime);
	// 조준에 따라 모양 감도 변경
	SetLookupRates();
	// 십자선 스프레드 승수 계산
	CalculateCrosshairSpread(DeltaTime);
	
	// 중복 항목수 확인 후 항목 추적 
	TraceForItems();
	
	//웅크리기 / 서기 기준으로 캡슐 절반 높이를 보간합니다.
	InterpCapsuleHalfHegiht(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// check 매크로 함수로 압력구성요소가 유효한지 확인 
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnRate);
	PlayerInputComponent->BindAxis("LookupRate", this, &AShooterCharacter::LookupRate);

	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("Lookup", this, &AShooterCharacter::Lookup);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AShooterCharacter::FKeyPressed);
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AShooterCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AShooterCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AShooterCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &AShooterCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &AShooterCharacter::FiveKeyPressed);
	


}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

bool AShooterCharacter::WeaponHasAmmo()
{
	
	if (EquippedWeapon == nullptr) return false;

	// 탄약을 호출후 0보다 크면 true 작으면 false
	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
	// 발사 사운드 
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	// 총알 발사	
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarelSocket");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		if (EquippedWeapon->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), SocketTransform);
		}

		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);

		if (bBeamEnd)
		{
			// 히트 액터가 BulletHitInterface를 구현

			if (BeamHitResult.Actor.IsValid())
			{
				// 캐스트 와 마찬가지 
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());
				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult,this,GetController());
				}
				// 대미지를 입었을때 사용
				AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor.Get());
				
				if (HitEnemy)
				{	
					// 피해 초기화
					int32 Damage{};
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						Damage = EquippedWeapon->GetHeadShotDamage();
						 // 해드에 맞았을때 
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());
						// 대미지를 전달하고 위치에 띄움( 해드샷 여부 )
						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location,true);
					}
					else
					{
						Damage = EquippedWeapon->GetDamage();	
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());
						// 대미지를 전달하고 위치에 띄움 ( 해드샷 여부 ) 
						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location,false);
					}
				}
			}
			else
			{
				  // Spawn 의 기본 파티클 
				if (ImpactParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, BeamHitResult.Location);
				}

			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticle, SocketTransform);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamHitResult.Location);
			}
		}
	}
}

void AShooterCharacter::PlayGunFireMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontage)
	{
		AnimInstance->Montage_Play(FireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (EquippedWeapon == nullptr) return;

	// 올바른 유형의 탄약이 있는지
	if (CarringAmmo() && !EquippedWeapon->ClipIsFull())	
	{
		if (bAiming)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}

	}
}

void AShooterCharacter::FinishReloading()
{
	// TODO : Update AmmoMap

	if (CombatState == ECombatState::ECS_Stunned) return;

	//전투 상태 업데이트
	CombatState = ECombatState::ECS_Unoccupied;
	
	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (EquippedWeapon == nullptr)return;

	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	// 탄약 업데이트
	if (AmmoMap.Contains(AmmoType))
	{
		// EquippedWeapon 유형의 캐릭터가 소지하고 있는 탄약의 양
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// EquipedWeapon  남은 탄약수 
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			// 가지고있는 탄약을 탄창에 다시 재장전
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			// 사용 가능 탄약이 남은 탄약보다 적을때 가져와서 채울수있게함 
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::FinishEquipping()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	// 전투상태를 비어있는상태로 
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
}

bool AShooterCharacter::CarringAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AShooterCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	// 장착 무기의 클립 뼈 인덱스
	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	// 클립의 변환 저장
	ClipTransforms = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);
	
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransforms);

	EquippedWeapon->SetMovingClip(true);

}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}


void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItem = false;
	}
	else
	{
		OverlappedItemCount += Amount;																	
		bShouldTraceForItem = true;
	}
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	Item->PlayEquipSound();

	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		// 인벤토리에 빈공간이있으면 인벤토리에 무기 획득
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			// 인벤토리에 아이탬이 추가될때 슬롯 설정 
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			//인벤토리에서 장비 교체 상태인지 확인
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else	// 아니라면 인벤토리 자리 부족
		{
			SwapWeapon(Weapon);
		}
		
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}
FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocation.Num())
	{
		return InterpLocation[Index];
	}
	return FInterpLocation();
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount >1)return;
	if (InterpLocation.Num() >= Index)
	{
		InterpLocation[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSountTimer, this, &AShooterCharacter::ResetPickupSoundTimer,PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::ResetEquipSoundTimer, EquipSoundResetTime);
}

/*
UAbilitySystemComponent* AShooterCharacter::GetAbilitySystemComponent() const
{
	return AbilityComp;
}
void AShooterCharacter::AquireAbility(TSubclassOf<UGameplayAbility>AbilityToAquire)
{
	if (AbilityComp)
	{
		if (HasAuthority() && AbilityToAquire)
		{
			FGameplayAbilitySpecDef SpecDef = FGameplayAbilitySpecDef();
			SpecDef.Ability = AbilityToAquire;
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(SpecDef, 1);
			AbilityComp->GiveAbility(AbilitySpec);
		}
		AbilityComp->InitAbilityActorInfo(this, this);
	}
}
  */

