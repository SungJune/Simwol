// Fill out your copyright notice in the Description page of Project Settings.

#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
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
#include "Engine/SkeletalMeshSocket.h"
#include "Interactable.h"
#include "InvetoryPickUp.h"
#include "Engine/Engine.h"
#include "FirstGamesGameModeBase.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//카메라 붐 이라는것을 알려줌 (생성)
	// 충돌발생시 플레이어를 끌어당김
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CamearaBoom"));
	CameraBoom ->SetupAttachment (GetRootComponent());
	// 카메라의 설정된거리 부터 카메라 따라옴 
	CameraBoom->TargetArmLength = 600.f;
	// 컨트롤러 기준으로 회전
	CameraBoom->bUsePawnControlRotation = true;

	// 캡슐 컴포넌트 사이즈 조정
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	// 따라오는 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCameara"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// 카메라 와 카메라의 컨트롤러방향으로 일치하도록 조정 을 한다  
	FollowCamera->bUsePawnControlRotation = false;

	// 입력 회전율 설정
	BaseTurnRate = 65.f;
	BaseLookupRate = 65.f;

	// 컨트롤러 회전시 회전 안함 (캡슐 컨트롤러)
	// 카메라에만 영향을 미칩니다
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 캐릭터 이동 구성
	GetCharacterMovement()->bOrientRotationToMovement = true; // 문자가 입력 방향으로 이동
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // 회전 속도
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>("DestinationMarker");
	//DestinationMarker -> SetupAttachment(RootComponent);											

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

	bSkillDown = false;

	SkillDamage = 50.f;

	//스킬 시전시간 ( 사용 )  시간 
	ablilty1Duration = 3.f;

	//스킬 쿨 타임 시간 
	ablilty1ColldownTime = 5.f;
	
	Rech = 250.f;
	
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
		// 장소이동후 오브젝트 불러옴(?)
		LoadGameNoSwitch();

		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}
	Inventory.SetNum(4);
	CurrentInteractable = nullptr;
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckForInteractables();

	float DeltaStamina = StaminaDrainRate * DeltaTime;
	
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		// 쉬프트 키가 눌렸을때
		if (bShiftKeyDown)
		{
			// 스테미나 의 상태가 기본스태미나 에서 소모된 스태미나를 뺴고 그 값이 최소스태미나 보다 작거나 같을때 실행 
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				// 현재의 스테미나 상태를 감소 
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);

				// 스테미나 변경을 반영
				Stamina -= DeltaStamina;
			}
			else 
			{
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight)
			{
				// 현재 상태를 Sprinting 으로  
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else //Shift up
		{
			// Sprinting 을 안하고있을때 현재 스태미나 에서 소모된 스태미나를 더하고 그값이 최고 스태미나 보다 크거나 같을때 실행
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				// 현재 스태미나를 최대 스태미나로 설정 
				Stamina = MaxStamina;
			}
			else
			{
				// 현재스태미나의 소모된 스태미나 회복
				Stamina += DeltaStamina;
			}
			// 현재 상태를 정상으로 
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:

		if (bShiftKeyDown)
		{
			// 스태미나 에서 뺀 스테미나 값이 0 보다 작거나 같을때 
			if (Stamina - DeltaStamina <= 0.f)
			{
				// 현재 상태를 스테미나 감소로 설정
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				// 현재 상태를 노말 모드로 변경
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;

				if (bMovingForward || bMovingRight)
				{
					// 현재 상태를 Sprinting 으로  
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
		;
	
	}

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);  

		SetActorRotation(InterpRotation);
	}

	// 전투 목표위치가 전투목표와 같다고할수있도록
	// 전투목표위치를 업데이트
	if (CombatTarget)
	{
		// 전투대상의 액터 위치 를저장
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

	// 캐릭터 점프 
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// 스킬 사용  
	PlayerInputComponent->BindAction("Skill_Button", IE_Pressed, this, &AMainCharacter::SkillKey_Down);
	PlayerInputComponent->BindAction("Skill_Button", IE_Released, this, &AMainCharacter::SkillKeyup); 

	// 범위 스킬 
	PlayerInputComponent->BindAction("RangSkill", IE_Pressed, this, &AMainCharacter::RangeSkill);

	// 캐릭터  Shift 달리기 
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);

	// Pause Menu
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainCharacter::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMainCharacter::ESCUp);

	// 아이템 마우스 클릭 습득(?)
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	// 플레이어 이동
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	// 캐릭터 회전
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	// 인벤토리 및 상호작용 
	PlayerInputComponent->BindAction("Intaract",IE_Pressed, this, &AMainCharacter::Interact);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AMainCharacter::ToggleInventory);
	
}

//  PauseMenu 했을때 움직임 멈춤
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

// 위 아래 움직임
void AMainCharacter:: MoveForward(float Value)
{
	bMovingForward = false;

	if (CanMove(Value))
	{
		//  find out which way is forward
		// GetControlRotation() 는 컨트롤러가 향하는 방향을 알려주는 회전자 를 반환하는 함수 
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}



// 왼쪽 오른쪽 움직임
void AMainCharacter:: MoveRight(float Value)
{
	bMovingRight = false;

	if (CanMove(Value))
	{
		//  find out which way is forward
		// GetControlRotation() 는 컨트롤러가 향하는 방향을 알려주는 회전자 를 반환하는 함수 
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

	// 무기를 손에 부착
	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("Attacksa"));
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
	// 피해를 입었을때  체력 - 폭발피해 가 0보다 작거나 같을때 죽음 
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

// 포션 효과
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
	// 토글 
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

void AMainCharacter::SkillKey_Down()
{
	bHaseSkillHit = true;
	UAnimInstance* AnimInstances = GetMesh()->GetAnimInstance();
	AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
	const USkeletalMeshSocket* SkillSocket = GetMesh()->GetSocketByName("Main_L_Trail_01"); //현재 스킬 사용시 발동될 파티클 위치 
	if (!bSkillDown && EquippedWeapon)
	{
		if (AnimInstances && SkillMontage)
		{
			AnimInstances->Montage_Play(SkillMontage, 1.5f);	
			AnimInstances->Montage_JumpToSection(FName("Skill_Start"), SkillMontage);
			
			//해당 파티클 소켓 일때 파티클 발생
			if (SkillSocket)
			{
				FVector SkillSocketLocation = SkillSocket->GetSocketLocation(GetMesh());
				//SkillModParticles 은 스킬 사용시 발생될 파티클 ( 피격 x ) 자체적인 파티클
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillModParticles, SkillSocketLocation, FRotator(0.f), false); 
			}
			// 스킬 사용 시전 시간 
			//GetWorld()->GetTimerManager().SetTimer(ablilty1TimerHandle, this, &AMainCharacter::SkillKey_Down, ablilty1Duration, false);
			AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
			if (!bAttacking)
			{
				if (Weapon)
				{
					Weapon->Equip(this);
					SetActiveOverlappingItem(nullptr);
					UE_LOG(LogTemp, Warning, TEXT("Attacksala"));
				}
			}
		}
		/*
		if (ablilty1Duration <= 3.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("asss"));
			AnimInstances->Montage_Play(SkillMontage, 1.3f);
			AnimInstances->Montage_JumpToSection(FName("Skill_Attack"), SkillMontage);
			if (SkillSocket)
			{
				FVector SkillAttackLocation = SkillSocket->GetSocketLocation(GetMesh());
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SkillAttackModParticles, SkillAttackLocation, FRotator(0.f), false);
			}
		}
		*/

	}
}

void AMainCharacter::PlaySwingSound()
{
	// 무기가 유효할떄
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

// 스킬 을 사용했을때 시간이 쿨 타임 사용
void AMainCharacter::ResetAblilty1()
{
	hasUsedAblilty1 = true;
	GetWorld()->GetTimerManager().SetTimer(ablilty1TimerHandle, this, &AMainCharacter::Ablilty1CooldownComplete, ablilty1ColldownTime, false);
}

bool AMainCharacter::AddItemToInventory(AInvetoryPickUp * Item)	  
{
	if (Item != NULL)
	{
		// 아이템 인벤토리 의 첫번째 슬롯 
		const int32 AvailableSloat = Inventory.Find(nullptr);

		if (AvailableSloat != INDEX_NONE)
		{
			Inventory[AvailableSloat] = Item;
			return true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("더이상 수집할수없습니다."));
			return false;
		}
	}
	else return false;
}

UTexture2D* AMainCharacter::GetThumbnailAtInventoryslot(int32 slot)
{
	if (Inventory[slot] != NULL)
	{
		return Inventory[slot]->PickupThumbnail;
	}
	else return nullptr;
}

FString AMainCharacter::GiveItemNameAtInventorySlot(int32 slot)
{
	if (Inventory[slot] != NULL)
	{
		return Inventory[slot]->ItemName;
	}
	return FString("None");
}

void AMainCharacter::UseItemAtInventorySlot(int32 slot)
{
	if (Inventory[slot] != NULL)
	{
		Inventory[slot]->Use_Implementation();
		Inventory[slot] = NULL; // 사용된 아이템 인벤토리에서 삭제 
	}
}

void AMainCharacter::ToggleInventory()
{
	// 인벤토리를 열음 
	AFirstGamesGameModeBase* GameMode = Cast<AFirstGamesGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameMode->GetHUDState() == GameMode->HS_Ingame)
	{
		GameMode->ChangeHUDState(GameMode->HS_Inventory);
	}
	else
	{
		GameMode->ChangeHUDState(GameMode->HS_Ingame);
	}
	
}

void AMainCharacter::Interact()
{
	// 상호작용을 했을때 무조건 사용가능하도록함 
	if (CurrentInteractable != nullptr)
	{
		CurrentInteractable->Interact_Implementation();
	}
}

void AMainCharacter::CheckForInteractables()
{
	FVector StartTrace = FollowCamera->GetComponentLocation();
	FVector EndTrace = (FollowCamera->GetForwardVector() * Rech) + StartTrace;

	// 레이캐스트로 쐈을때 표적에맞는지 
	FHitResult HitResult;

	//액터 충돌
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	// 라인 추적
	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_WorldDynamic, CQP);

	AInteractable* PotentialInteractable = Cast<AInteractable>(HitResult.GetActor());
	if (PotentialInteractable == NULL)
	{
		HelpText = FString("");
		CurrentInteractable = nullptr;
		return;
	}
	else
	{
		CurrentInteractable = PotentialInteractable;
		HelpText = PotentialInteractable->InteractableHelpText;
	}
}

//스킬 쿨타임이 다되었을때 재사용 대기로 전환
void AMainCharacter::Ablilty1CooldownComplete()
{
	hasUsedAblilty1 = false;
}



// 화면 전환
void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		// FName 을 FString 으로 초기화
		// FName을 문자열 리터럴로 초기화
		FName CurreuntLevelName(*CurrentLevel);
		if (CurreuntLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}

	}
}



// 게임 저장 
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

//게임 불러오기
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

void AMainCharacter::RangeSkill()
{
	//DestinationMarker->SetVisibility(false);
	//DestinationMarker->Activate(false);
}











