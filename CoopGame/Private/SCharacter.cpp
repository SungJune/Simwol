// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "SWeapon.h"
#include "SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "../CoopGame.h"



// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComps = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComps"));
	
	// 캐릭터 움직임 입력 
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 입력받는 방향으로 움직임
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 450.f, 0.0f);

	//점프
	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->AirControl = 0.2;
	
	BaseTurnRate = 65.f;
	BaseLookupRate = 65.f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "LeftHandBowSocket";
	
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;
	HealthComps->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	if (GetLocalRole() == ROLE_Authority)
	{
		// 기본 장착 무기 
		FActorSpawnParameters SpawnParms;
		SpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StartWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParms);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
		
	}
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bMantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	// 캐릭터 회전
	PlayerInputComponent->BindAxis("TurnRate", this, &ASCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BegineZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	//공격
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

}
void ASCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//AddMovementInput(GetActorForwardVector() * Value);
		AddMovementInput(Direction, Value);
	}
	

}
void ASCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//AddMovementInput(GetActorRightVector()* Value);
		AddMovementInput(Direction, Value);
	}
	
}

void ASCharacter::BeginCrouch()
{
	Crouch();
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}
void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::Jump()
{

}

void ASCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}


FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ASCharacter::BegineZoom()
{
	bMantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bMantsToZoom = false;
}

void ASCharacter::StartFire()
{
	if (!bAttacking)
	{
		bAttacking = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AnimAttack)
		{
			AnimInstance->Montage_Play(AnimAttack, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), AnimAttack);
			if (CurrentWeapon)
			{
				CurrentWeapon->StartFire();
			}
		}
	}
	
}
void ASCharacter::StopFire()
{
	bAttacking = false;
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta,
								const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f /*&& !bDied*/)
	{
		//DIe();
		//bDied = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASCharacter::DIe()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathAnim)
	{
		AnimInstance->Montage_Play(DeathAnim, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), DeathAnim);
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
}