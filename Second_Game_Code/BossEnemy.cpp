// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "AIController.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MainPlayerController.h"
#include "BossPlayerController.h"


// Sets default values
ABossEnemy::ABossEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BossAgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BossAgroSphere"));
	BossAgroSphere->SetupAttachment(GetRootComponent());
	BossAgroSphere->InitSphereRadius(600.f);

	BossCombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BossCombatSphere"));
	BossCombatSphere->SetupAttachment(GetRootComponent());
	BossCombatSphere->InitSphereRadius(300.f);

	RightCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCombatCollision"));
	RightCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("GRIFFON_RightSocket"));

	LeftCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCombatCollision"));
	LeftCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("GRIFFON_LeftSocket"));

	MouseCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MouseCombatCollision"));
	MouseCombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("GRIFFON_MouseSocket"));


	BossHealth = 200.f;
	BossMaxHealth = 300.f;

	BossDamage = 50.f;

	DeathDlay = 3.f;

	bOverlappingCombatSphere = false;
	bHasValidTarget = false;

	AttackMinTime = 1.f;
	AttackMaxTime = 3.f;

	
}

// Called when the game starts or when spawned
void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	BossAIController = Cast<AAIController>(GetController());//11�� 13��

	BossAgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::BossAgroSphereOnOverlapBegin);
	BossAgroSphere->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::BossAgroSphereOnOverlapEnd);

	BossCombatSphere->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::BossCombatSphereOnOverlapBegin);
	BossCombatSphere->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::BossCombatSphereOnOverlapEnd);

	RightCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::BossCombatOnOverlapBegin);
	RightCombatCollision->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::BossCombatOnOverlapEnd);

	LeftCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::BossCombatOnOverlapBegin);
	LeftCombatCollision->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::BossCombatOnOverlapEnd);

	MouseCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemy::BossCombatOnOverlapBegin);
	MouseCombatCollision->OnComponentEndOverlap.AddDynamic(this, &ABossEnemy::BossCombatOnOverlapEnd);


	// ������ ���� �ݸ���
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// �� �� ���� �ݸ���
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//�θ� ���� �ݸ���
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MouseCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	MouseCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MouseCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
}

// Called every frame
void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABossEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABossEnemy::BossAttack()
{
	if (Alive() && bHasValidTarget)
	{
		if (BossAIController)
		{
			BossAIController->StopMovement();
			SetBossEnemyMovementStatus(EBossEnemyMovementStatus::EMS_Attacking);
		}
		if (!bAttacking)
		{
			bAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(BossCombatMontage, 1.45f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), BossCombatMontage);
				
				//AnimInstance->Montage_Play(BossCombatMontage, 1.56f);
				//AnimInstance->Montage_JumpToSection(FName("Attack_2"), BossCombatMontage);
				
				
				/*
				int32 Section = FMath::RandRange(0, 1);
				switch (Section)
				{
				case 0:
					AnimInstance->Montage_Play(BossCombatMontage, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Attack"), BossCombatMontage);
					break;

				case 1:
					AnimInstance->Montage_Play(BossCombatMontage, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Attack_2"), BossCombatMontage);
					break;

				default:
					;
				}
				*/
				
			}
			
			if (BossHealth <= 150.f && bAttacking)
			{
				int32 PatonSection = FMath::RandRange(0, 3);
				switch (PatonSection)
				{
				case 0:
					AnimInstance->Montage_Play(BossPatonCombatMontage, 1.7f);
					AnimInstance->Montage_JumpToSection(FName("PatonAttack"), BossPatonCombatMontage);
					break;

				case 1:
					AnimInstance->Montage_Play(BossPatonCombatMontage, 1.7f);
					AnimInstance->Montage_JumpToSection(FName("PatonAttack_1"), BossPatonCombatMontage);
					break;

				case 2:
					AnimInstance->Montage_Play(BossCombatMontage, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Attack"), BossCombatMontage);
					break;
				case 3:
					AnimInstance->Montage_Play(BossCombatMontage, 1.5f);
					AnimInstance->Montage_JumpToSection(FName("Attack_2"), BossCombatMontage);
					break;

				default:
					;
				}
			}
			
		}	
		/*
		if (BossHealth <= 150.f&& bAttacking)
		{
			//bAttacking = true;
			UAnimInstance* AnimInstances = GetMesh()->GetAnimInstance();
			if (AnimInstances)
			{
				int32 PatonSection = FMath::RandRange(0, 3);
				switch (PatonSection)
				{
				case 0:
					AnimInstances->Montage_Play(BossPatonCombatMontage, 1.7f);
					AnimInstances->Montage_JumpToSection(FName("PatonAttack"), BossPatonCombatMontage);
					break;

				case 1:
					AnimInstances->Montage_Play(BossPatonCombatMontage, 1.8f);
					AnimInstances->Montage_JumpToSection(FName("PatonAttack_1"), BossPatonCombatMontage);
					break;

				case 2:
					AnimInstances->Montage_Play(BossCombatMontage, 1.5f);
					AnimInstances->Montage_JumpToSection(FName("Attack"), BossCombatMontage);
					break;
				case 3:
					AnimInstances->Montage_Play(BossCombatMontage, 1.6f);
					AnimInstances->Montage_JumpToSection(FName("Attack_2"), BossCombatMontage);
					break;

				default:
					;
				}
			}
				
			
		}
		*/
	}
}

void ABossEnemy::BossAttackEnd()
{
	bAttacking = false;
	if (bOverlappingCombatSphere)
	{
		// ���ݼӵ��� �ð����� ����
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &ABossEnemy::BossAttack, AttackTime);
		BossAttack();
	}
}

void ABossEnemy::BossAgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//11�� 13��
	if (OtherActor && Alive())
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if (Character)
		{
			MoveToTarget(Character);

			if (Character->MainPlayerController)
			{
				Character->MainPlayerController->Boss_DisplayHealthBar();
			}
		}
	}
}

void ABossEnemy::BossAgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//11�� 13��
	if (OtherActor)
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		//ABossPlayerController* BossController = Cast<ABossPlayerController>(OtherActor);
		if (Character)
		{
			bHasValidTarget = false;
			
			if (Character->BossEnemyCombatTarget == this)
			{
				// 11/17�� 
				Character->SetBossCombatTarget(nullptr);
			}
			Character->SetHasBossCombatTarget(false);
			
			
			
			if (Character->MainPlayerController)
			{
				Character->MainPlayerController->Boss_RemoveHealthBar();
			}
			

			SetBossEnemyMovementStatus(EBossEnemyMovementStatus::EMS_Idle); //11�� 29��
			if (BossAIController)
			{
				BossAIController->StopMovement();
			}
		}
		
		/*
		if (BossController)
		{
			BossController->RemoveEnemyHealthBar();
		}
		*/
		
	}
}

void ABossEnemy::BossCombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && Alive())
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		//ABossPlayerController* BossController = Cast<ABossPlayerController>(OtherActor);
		if(Character)
		{
			bHasValidTarget = true;
			Character->SetBossCombatTarget(this);
			Character->SetHasBossCombatTarget(true);
			
			BossCombatTarget = Character;
			bOverlappingCombatSphere = true;
			BossAttack();
		}
		
	}
}

void ABossEnemy::BossCombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 11�� 29�� 
	if (OtherActor)
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if (Character)
		{
			bOverlappingCombatSphere = false;
			if (BossEnemyMovementStatus != EBossEnemyMovementStatus::EMS_Attacking)
			{
				MoveToTarget(Character);
				BossCombatTarget = nullptr;
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}


void ABossEnemy::BossCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if (Character)
		{
			if (Character->BossPlayerHitParticles)
			{
				const USkeletalMeshSocket* BossTipSocket = GetMesh()->GetSocketByName("GRIFFON_MouseTipSocket");
				const USkeletalMeshSocket* BossTipSocket_L = GetMesh()->GetSocketByName("GRIFFON_-LHandTipSocket");
				const USkeletalMeshSocket* BossTipSocket_R = GetMesh()->GetSocketByName("GRIFFON_-RHandTipSocket");
				if (BossTipSocket)
				{
					FVector BossSocketLocation = BossTipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Character->PlayerHitParticles, BossSocketLocation, FRotator(0.f), false);
				}
				if (BossTipSocket_L)
				{
					FVector BossSocketLocation_L = BossTipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Character->PlayerHitParticles, BossSocketLocation_L, FRotator(0.f), false);
				}
				if (BossTipSocket_R)
				{
					FVector BossSocketLocation_R = BossTipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Character->PlayerHitParticles, BossSocketLocation_R, FRotator(0.f), false);
				}
			}
			if (DamageTypeclass)
			{
				UGameplayStatics::ApplyDamage(Character, BossDamage, BossAIController, this, DamageTypeclass);
			}
		}
	}
}

 void ABossEnemy::BossCombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}



//11�� 13��
void ABossEnemy::MoveToTarget(AMainCharacter* Target)
{
	SetBossEnemyMovementStatus(EBossEnemyMovementStatus::EMS_MoveToTarget);
	if (BossAIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavPath;
		BossAIController->MoveTo(MoveRequest, &NavPath);
	}
}


void ABossEnemy::ActivateCollision()
{
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABossEnemy::DeactivateCollision()
{
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ABossEnemy::ActivateCollision_a()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABossEnemy::DeactivateCollision_a()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ABossEnemy::ActivateCollision_b()
{
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void ABossEnemy::DeactivateCollision_b()
{
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossEnemy::ActivateCollision_c()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABossEnemy::DeactivateCollision_c()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossEnemy::ActivateCollision_m()
{
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABossEnemy::DeactivateCollision_m()
{
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossEnemy::ActivateCollision_Paton()
{
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
void ABossEnemy::DeactivateCollision_Paton()
{
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossEnemy::ActivateCollision_Paton_a()
{
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
void ABossEnemy::DeactivateCollision_Paton_a()
{
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 11/ 29
bool ABossEnemy::Alive()
{
	return GetBossEnemyMovementStatus() != EBossEnemyMovementStatus::EMS_Dead;
}

// 12/5��
void ABossEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(BossCombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), BossCombatMontage);
	}
	SetBossEnemyMovementStatus(EBossEnemyMovementStatus::EMS_Dead);

	BossAgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BossCombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MouseCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 12/5��
void ABossEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetWorldTimerManager().SetTimer(DeathTimer, this, &ABossEnemy::Disappear, DeathDlay);
}


//11�� 13��
void ABossEnemy::Disappear()
{
	Destroy();
}

// 12/5��
float ABossEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	if (BossHealth - DamageAmount <= 0.f)
	{
		BossHealth -= DamageAmount;
		Die();
	}
	else
	{
		BossHealth -= DamageAmount;
	}
	return DamageAmount;
}