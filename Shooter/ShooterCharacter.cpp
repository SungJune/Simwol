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
	// ����/�� ���ؿ� ���� �⺻ ����
	BaseTurnRate(45.f),
	BaseLookupRate(45.f),
	
	// ���� ���� ���� ī�޶� �ʵ�
	CameraDefaultFOV(0.f), // BeginPlay ���� ����
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(0.f),
	ZoomInterpSpeed(20.f),
	// ���콺 ��� ���� ������ ����
	MouseHipTurnRate(1.0f),
	MouseHipLookupRate(1.0f),
	MouseAimingTurnRate(0.6f),
	MouseAimingLookupRate(0.6f),
	// ���⸦ ������ �� true
	bAiming(false),
	// ����/�� ���� ȸ���� 
	HipTurnRate(90.f),
	HipLookupRate(90.f),
	AimingTurnRate(20.f),
	AimingLookupRate(20.f),
	//���ڼ� Ȯ�� ���
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	// �ڵ� �� �߻� �ӵ�
	bShouldFire(true),
	bFireButtonPressed(false),
	// �Ѿ� �߻� Ÿ�̸� ����
	ShooterTimeDuration(0.05f),
	bFiringBullet(false),
	// �׸� ���� ����
	bShouldTraceForItem(false),
	OverlappedItemCount(0),
	//ī�޶� ������ ��ġ ����
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	// ���� ź�� ��
	Starting9mmAmmo(90),
	StartingARAmmo(120),
	//���� ����
	CombatState(ECombatState::ECS_Unoccupied),
	bCrouching(false),
	BaseMovementSpeed(650.f),
	CrouchMovementSpeed(300.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	bAimingButtonPressed(false),
	// �Ҹ� Ÿ�̸� ���� 
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	PickupSoundResetTime(0.2f),
	EquipSoundResetTime(0.2f),
	// ������ �ִϸ��̼� �Ӽ�
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
	CameraBoom->bUsePawnControlRotation = true; // ��Ʈ�ѷ��� �����϶����� ���� ������ 
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ��Ʈ�ѷ��� ȸ���ҋ� ȸ���������� ��Ʈ�ѷ��� ī�޶󿡸� ������ ��ħ 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// �Է��ϴ� �������� ĳ���Ͱ� ������ �׸��� ȸ�����Ҷ��� ȸ����   
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// HandSceneComponent ����
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	// ���� ������� ����
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

	// �⺻ ���⸦ �����ϰ� �տ� ���� 
	EquipWeapon(SpawnDefaultWeapon());
	//�κ��丮�� ���� ���������� ǥ�� 
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->SetCharacter(this);

	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	// �� interp ��ġ�� ���� FInterpLocation struce�� �迭�� �߰� �Ͽ� ���� 
	InitializeInterpLocation();


}

void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// ��Ʈ�ѷ� ȸ���� ���� �׸��� �ش� ������Ҹ� ������ ���ο� ȸ���ڷ� �ʱ�ȭ
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator RotationYaw{ 0,Rotation.Yaw,0 };

		// ȸ�� ��� �ش� �࿭�� �� �� �ϳ��� ���� 
		const FVector Direction{ FRotationMatrix{RotationYaw}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// ��Ʈ�ѷ� ȸ���� ���� �׸��� �ش� ������Ҹ� ������ ���ο� ȸ���ڷ� �ʱ�ȭ
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator RotationYaw{ 0,Rotation.Yaw,0 };

		// ȸ�� ��� �ش� �࿭�� �� �� �ϳ��� ���� 
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
		// ������ ź�࿡�� 1 ����
		EquippedWeapon->DecrementAmmo();

		StartFireTimer();

		// ������ ������ �����϶� 
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			// �����̵� Ÿ�̸� ���� 
			EquippedWeapon->StartSlideTimer();
		}
	}
	//���ڼ��� ���� �Ѿ� �߻� Ÿ�̸� ����
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
	// ���� ī�޶� �þ� ���� 
	if (bAiming)
	{
		// FOV Ȯ�븦 ���� ����
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);

	}
	else
	{
		// �⺻ FOV�� ����
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);

}
 // ���ڼ� �������� ���
void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	// ���ڼ� �ӵ� ��� ���
	FVector2D WalkSpeedRange(0.f, 600.f);
	FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// 	���� ������� ���ڼ� ���
	if (GetCharacterMovement()->IsFalling()) // is in air 
	{
		// ���߿��� ���ڼ��� õõ�� ��ħ
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else // ĳ���Ͱ� �ٴڿ� ������ 
	{
		// ���� �ִ� ���� ���ڼ��� ������ ���
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// ���ڼ� ���� ��� ���
	if (bAiming)
	{
		// ������ ���� ���������� ���
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		// ���Ӽ��� �ٽ� �ǵ��� 
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	// 0.05���� �߻� 
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
	// ������ ���� ���� Ȯ��
	FVector OutBeamLocation;
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshair(CrosshairHitResult,OutBeamLocation);

	if (bCrosshairHit)
	{
		// �ӽ� �� ��ġ - ������ �ѿ��� �����ؾ� ��
		OutBeamLocation = CrosshairHitResult.Location;

	}
	else
	{
		// �ش� ������ ���� 
		

	}

	// �� ��° ������ ����
	const FVector WeapontraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeapontraceEnd{ MuzzleSocketLocation + StartToEnd* 1.25f };
	GetWorld()->LineTraceSingleByChannel(OutHitResult, WeapontraceStart, WeapontraceEnd, ECollisionChannel::ECC_Visibility);

	if (!OutHitResult.bBlockingHit) // �跲�� BeamEndPoint ������ ��ü
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}
	return true;

	/*
	if (WeapontraceHit.bBlockingHit) // �跲�� BeamEndPoint ������ ��ü
	{
		OutBeamLocation = WeapontraceHit.Location;
		return true;
	}
	return false;
	  */

	// ���� HUD�� ȭ�� ũ�⸦ ������ 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	// ���ڼ��� GetScreen ���� ��ġ
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// ���ڼ��� ���� ��ġ �� ���� ��������
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)// ���������ǿ� �����ߴ�
	{
		FHitResult ScreenTranceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f };

		//  �� ����Ʈ�� ���� Ʈ���̽� �� ����Ʈ�� ����
		OutBeamLocation = End;
		// ���ڼ� ���� ��ġ���� �ٱ������� ����
		GetWorld()->LineTraceSingleByChannel(ScreenTranceHit, Start, End, ECollisionChannel::ECC_Visibility);

		if (ScreenTranceHit.bBlockingHit) //������ �־��°�.
		{

			// �� ������ ���� ���� ���� ��ġ�Դϴ�.
			OutBeamLocation = ScreenTranceHit.Location;
			
		}
		

	}

	
}

bool AShooterCharacter::TraceUnderCrosshair(FHitResult& OutHitResult, FVector& OutHitLocation)
{	
	// ���� HUD�� ȭ�� ũ�⸦ ������ 
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// ���ڼ��� GetScreen ���� ��ġ
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// ���ڼ��� ���� ��ġ �� ���� ��������
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Crosshair�� Wold ��ġ���� �ٱ������� Ʈ���̽�
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
				// ���罽���� ���������ʰ� �ϳ����� ����
				HighlightInventorySlot();
			}

		}
		else
		{
			// ���� �����ǰ��ִ� ���� ���̶���Ʈ 
			if (HighlightSlot != -1)
			{
				// �ϳ��� ���̶���Ʈ �κ��丮 ������ ȣ��
				UnHighlightInventorySlot();

			}
		}

		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);

			// ���� �׷��ٸ�, �츮�� �������� �����ϰ� �����ϰ� �װ��� ���������� ĳ���õȴٸ�
			//�׸����� �����ϰ� �׸� ���°� ��� �ؼ��� ��� Trece ��Ʈ �׸��� ����
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				// �׸��� �Ⱦ� ���� ǥ��
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCusomDepth();

				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					// �κ��丮�� �� ��������
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					//�κ��丮�� ������ ������ 
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}

			// �ش� �����ӿ� �ε�������� 
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// �� �����ӿ��� ������ �����Ӱ� �ٸ� ������ �� ġ�� �ְų� ������ �� null�Դϴ�.
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}

			//���� �����ӿ� ���� HitItem�� ���� ���� ����
			TraceHitItemLastFrame = TraceHitItem;
		} 
	}
	else if (TraceHitItemLastFrame)
	{
		// �� �̻� �׸��� ��ġ�� ������  �׸� ������ �����ӿ� ������ ǥ�õ��� �ʵ��� ��
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}


AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	// TSubclassOf ���� Ȯ��
	if (DefaultWeaponClass)
	{
		// ������� 
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
			// �տ� ���⸦ ������Ŵ 
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (EquippedWeapon == nullptr)
		{
			// -1 = ���� �������� ���Ⱑ������ �����ִܾϸ��̼� �� ������ �ʿ���� 
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());

		}
		else if(!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		// ���⸦ ���� ������ ����� ���� 
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}

}
// ���� ��ü ( ��� ���� ) 
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

// ź�� ������ ź�� ���� �ʱ�ȭ
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

	// ��ũ���� ����, �� ������ ���
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
	// AmmoMap�� Ammo�� AmmoType�� ���ԵǾ� �ִ��� Ȯ���Ͻʽÿ�.
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// ź�� ������ ���� AmmoMap���� ź�� �� Ȯ��
		int32 AmmoCount{ AmmoMap[Ammo->GetAmmoType()] };
		AmmoCount += Ammo->GetItemCount();
		
		// �������� �� ������ ź�� ���� �����մϴ�.
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;

		if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
		{
			// �ѿ� ź���� ��� �ִ��� Ȯ��
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

// �� ������ ���� ���� �Լ� 
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
	return -1; // �κ��丮�� ����á���� ��ȯ
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

	// F���� �浹
	FCollisionQueryParams QueryParams;
	// ��ȯ���� ���Ͽ� ����Ʈ���̽� ���� ���� 
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

	// ������ �� Ȯ��/��Ҹ� ���� �ڵ� ����
	CameraInterpZoom(DeltaTime);
	// ���ؿ� ���� ��� ���� ����
	SetLookupRates();
	// ���ڼ� �������� �¼� ���
	CalculateCrosshairSpread(DeltaTime);
	
	// �ߺ� �׸�� Ȯ�� �� �׸� ���� 
	TraceForItems();
	
	//��ũ���� / ���� �������� ĸ�� ���� ���̸� �����մϴ�.
	InterpCapsuleHalfHegiht(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// check ��ũ�� �Լ��� �з±�����Ұ� ��ȿ���� Ȯ�� 
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

	// ź���� ȣ���� 0���� ũ�� true ������ false
	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::PlayFireSound()
{
	// �߻� ���� 
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->GetFireSound());
	}
}

void AShooterCharacter::SendBullet()
{
	// �Ѿ� �߻�	
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
			// ��Ʈ ���Ͱ� BulletHitInterface�� ����

			if (BeamHitResult.Actor.IsValid())
			{
				// ĳ��Ʈ �� �������� 
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.Actor.Get());
				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitResult,this,GetController());
				}
				// ������� �Ծ����� ���
				AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.Actor.Get());
				
				if (HitEnemy)
				{	
					// ���� �ʱ�ȭ
					int32 Damage{};
					if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						Damage = EquippedWeapon->GetHeadShotDamage();
						 // �ص忡 �¾����� 
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());
						// ������� �����ϰ� ��ġ�� ���( �ص弦 ���� )
						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location,true);
					}
					else
					{
						Damage = EquippedWeapon->GetDamage();	
						UGameplayStatics::ApplyDamage(BeamHitResult.Actor.Get(), Damage, GetController(), this, UDamageType::StaticClass());
						// ������� �����ϰ� ��ġ�� ��� ( �ص弦 ���� ) 
						HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location,false);
					}
				}
			}
			else
			{
				  // Spawn �� �⺻ ��ƼŬ 
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

	// �ùٸ� ������ ź���� �ִ���
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

	//���� ���� ������Ʈ
	CombatState = ECombatState::ECS_Unoccupied;
	
	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (EquippedWeapon == nullptr)return;

	const auto AmmoType{ EquippedWeapon->GetAmmoType() };

	// ź�� ������Ʈ
	if (AmmoMap.Contains(AmmoType))
	{
		// EquippedWeapon ������ ĳ���Ͱ� �����ϰ� �ִ� ź���� ��
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// EquipedWeapon  ���� ź��� 
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			// �������ִ� ź���� źâ�� �ٽ� ������
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			// ��� ���� ź���� ���� ź�ຸ�� ������ �����ͼ� ä����ְ��� 
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::FinishEquipping()
{
	if (CombatState == ECombatState::ECS_Stunned) return;
	// �������¸� ����ִ»��·� 
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

	// ���� ������ Ŭ�� �� �ε���
	int32 ClipBoneIndex{ EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName()) };
	// Ŭ���� ��ȯ ����
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
		// �κ��丮�� ������������� �κ��丮�� ���� ȹ��
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			// �κ��丮�� �������� �߰��ɶ� ���� ���� 
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			//�κ��丮���� ��� ��ü �������� Ȯ��
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else	// �ƴ϶�� �κ��丮 �ڸ� ����
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

