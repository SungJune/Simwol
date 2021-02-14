// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"
#include "TimerManager.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ī�޶� �� �̶�°��� �˷��� (����)
	// �浹�߻��� �÷��̾ ������
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CamearaBoom"));
	CameraBoom ->SetupAttachment (GetRootComponent());
	// ī�޶��� �����ȰŸ� ���� ī�޶� ����� 
	CameraBoom->TargetArmLength = 600.f;
	// ��Ʈ�ѷ� �������� ȸ��
	CameraBoom->bUsePawnControlRotation = true;

	// ĸ�� ������Ʈ ������ ����
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	// ������� ī�޶� ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCameara"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// ī�޶� �� ī�޶��� ��Ʈ�ѷ��������� ��ġ�ϵ��� ���� �� �Ѵ�  
	FollowCamera->bUsePawnControlRotation = false;

	// �Է� ȸ���� ����
	BaseTurnRate = 65.f;
	BaseLookupRate = 65.f;

	// ��Ʈ�ѷ� ȸ���� ȸ�� ���� (ĸ�� ��Ʈ�ѷ�)
	// ī�޶󿡸� ������ ��Ĩ�ϴ�
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// ĳ���� �̵� ����
	GetCharacterMovement()->bOrientRotationToMovement = true; // ���ڰ� �Է� �������� �̵�
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ȸ�� �ӵ�
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;
	Stamina = 120.f;
	coins = 0;
	
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	bShiftKeyDown = false;
	bLMBDown = false;
	bESCDown = false;

	//Initiallze Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;


	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;

	MinTime = 1.f;

	MaxTime = 3.f;

	bTeleport = false;
	
	
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "SunTemple")
	{
		// ����̵��� ������Ʈ �ҷ���(?)
		LoadGameNoSwitch();

		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}

	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) 
		return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;
	
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		// ����Ʈ Ű�� ��������
		if (bShiftKeyDown)
		{
			// ���׹̳� �� ���°� �⺻���¹̳� ���� �Ҹ�� ���¹̳��� ���� �� ���� �ּҽ��¹̳� ���� �۰ų� ������ ���� 
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				// ������ ���׹̳� ���¸� ���� 
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);

				// ���׹̳� ������ �ݿ�
				Stamina -= DeltaStamina;
			}
			else 
			{
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight)
			{
				// ���� ���¸� Sprinting ����  
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else //Shift up
		{
			// Sprinting �� ���ϰ������� ���� ���¹̳� ���� �Ҹ�� ���¹̳��� ���ϰ� �װ��� �ְ� ���¹̳� ���� ũ�ų� ������ ����
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				// ���� ���¹̳��� �ִ� ���¹̳��� ���� 
				Stamina = MaxStamina;
			}
			else
			{
				// ���罺�¹̳��� �Ҹ�� ���¹̳� ȸ��
				Stamina += DeltaStamina;
			}
			// ���� ���¸� �������� 
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:

		if (bShiftKeyDown)
		{
			// ���¹̳� ���� �� ���׹̳� ���� 0 ���� �۰ų� ������ 
			if (Stamina - DeltaStamina <= 0.f)
			{
				// ���� ���¸� ���׹̳� ���ҷ� ����
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				// ���� ���¸� �븻 ���� ����
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;

				if (bMovingForward || bMovingRight)
				{
					// ���� ���¸� Sprinting ����  
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:

		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:

		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		break;
	
	}

	if (bInterpToEnemy && CombatMontage)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);  

		SetActorRotation(InterpRotation);
	}

	// ���� ��ǥ��ġ�� ������ǥ�� ���ٰ��Ҽ��ֵ���
	// ������ǥ��ġ�� ������Ʈ
	if (CombatTarget)
	{
		// ��������� ���� ��ġ ������
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

	//FVector PlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// ĳ���� ���� 
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Ÿ�ӿ��� �׽�Ʈ
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AMainCharacter::Teleport);
	PlayerInputComponent->BindAction("Teleport", IE_Released, this, &AMainCharacter::TeleportEnd);

	// ĳ����  Shift �޸��� 
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);

	// Pause Menu
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainCharacter::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMainCharacter::ESCUp);

	// ������ ���콺 Ŭ�� ����(?)
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	// �÷��̾� �̵�
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	// ĳ���� ȸ��
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
	
}

//  PauseMenu ������ ������ ����
bool AMainCharacter::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Value != 0.0f) && (!bAttacking) &&
			(MovementStatus != EMovementStatus::EMS_Dead) &&
			!MainPlayerController->bPauseMenuVisible;
	}
	return false;
}

void AMainCharacter::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMainCharacter::LookUp(float Value)
{
	if (CanMove(Value))
	{
		AddControllerPitchInput(Value);
	}
}

// �� �Ʒ� ������
void AMainCharacter:: MoveForward(float Value)
{
	bMovingForward = false;

	if (CanMove(Value))
	{
		//  find out which way is forward
		// GetControlRotation() �� ��Ʈ�ѷ��� ���ϴ� ������ �˷��ִ� ȸ���� �� ��ȯ�ϴ� �Լ� 
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}



// ���� ������ ������
void AMainCharacter:: MoveRight(float Value)
{
	bMovingRight = false;

	if (CanMove(Value))
	{
		//  find out which way is forward
		// GetControlRotation() �� ��Ʈ�ѷ��� ���ϴ� ������ �˷��ִ� ȸ���� �� ��ȯ�ϴ� �Լ� 
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		bMovingRight = true;
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	//if (MainPlayerController) if (!MainPlayerController->bPauseMenuVisible) return;

	// ���⸦ �տ� ����
	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon)
	{
		Attack();
	}
}
void AMainCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMainCharacter::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}
void AMainCharacter::ESCUp()
{
	bESCDown = false;
}

void AMainCharacter::DecrementHealth(float Amount)
{
	// ���ظ� �Ծ�����  ü�� - �������� �� 0���� �۰ų� ������ ���� 
	if (Health - Amount <=0.f)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

void AMainCharacter::IncrementCoins(int32 Amount)
{
	coins += Amount;
}

// ���� ȿ��
void AMainCharacter::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}

void AMainCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

/*
void AMainCharacter::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		//Super::Jump();
	}
}
*/
void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMainCharacter::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMainCharacter::ShiftKeyUp()
{
	// ��� 
	bShiftKeyDown = false;
}

void AMainCharacter::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}

void AMainCharacter::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		SetInterpToEnemy (true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;

			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.8f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				;
			}	
		}
		
	}
	
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);

	if (bLMBDown)
	{
		Attack();
	}
}

void AMainCharacter::PlaySwingSound()
{
	// ���Ⱑ ��ȿ�ҋ�
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}

}

void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;
	}
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();
		
		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}
// ȭ�� ��ȯ
void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		// FName �� FString ���� �ʱ�ȭ
		// FName�� ���ڿ� ���ͷ��� �ʱ�ȭ
		FName CurreuntLevelName(*CurrentLevel);
		if (CurreuntLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}

	}
}

// ���� ���� 
void AMainCharacter::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = coins;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	SaveGameInstance->CharacterStats.LevelName = MapName;
	
	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex); 
}

//���� �ҷ�����
void AMainCharacter::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadeGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	LoadeGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadeGameInstance->PlayerName, LoadeGameInstance->UserIndex));

	Health = LoadeGameInstance->CharacterStats.Health;
	MaxHealth = LoadeGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadeGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadeGameInstance->CharacterStats.MaxStamina;
	coins = LoadeGameInstance->CharacterStats.Coins;

	//GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadeGameInstance->CharacterStats.WeaponName;
		
			AWeapon* WeaponToEuipe = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
			WeaponToEuipe->Equip(this);
		}
	}
	if (SetPosition)
	{
		SetActorLocation(LoadeGameInstance->CharacterStats.Location);
		SetActorRotation(LoadeGameInstance->CharacterStats.Rotation);
	}
	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	if (LoadeGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName(*LoadeGameInstance->CharacterStats.LevelName);
		SwitchLevel(LevelName);
	}

}

void AMainCharacter::LoadGameNoSwitch()
{
	UFirstSaveGame* LoadeGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	LoadeGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadeGameInstance->PlayerName, LoadeGameInstance->UserIndex));

	Health = LoadeGameInstance->CharacterStats.Health;
	MaxHealth = LoadeGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadeGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadeGameInstance->CharacterStats.MaxStamina;
	coins = LoadeGameInstance->CharacterStats.Coins;

	//GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadeGameInstance->CharacterStats.WeaponName;

			AWeapon* WeaponToEuipe = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
			WeaponToEuipe->Equip(this);
		}
	}
	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}

void AMainCharacter::Teleport()
{
	/*
	if (bTeleport)
	{
		
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HitParticles)
		{
			FVector PlayerPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
			GetWorldTimerManager().SetTimer(TeleportTimer, this, &AMainCharacter::Teleport, 1.f);

			switch (ForwardVector)
			{
			case 'W':
				// ���� �÷��̾� ��ġ�� ���� 
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, SocketLocation, FRotator(0.f), false);
				break;

			case 'S':
				// ���� �÷��̾� ��ġ�� ���� 
				AnimInstance->Montage_Play(CombatMontage, 1.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			case 'A':
				// ���� �÷��̾� ��ġ�� ���� 
				AnimInstance->Montage_Play(CombatMontage, 1.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;

			case 'D':// ���� �÷��̾� ��ġ�� ���� 
				AnimInstance->Montage_Play(CombatMontage, 1.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				break;
			}
		}
		
		
	}
	*/
}


void AMainCharacter::TeleportEnd()
{

}












/*
void AMainCharacter::ShowPickupLocation()
{

	for (auto Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
	}
	
}
*/