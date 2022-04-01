// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "WeaponType.h"


UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterRotation(FRotator(0.f)),
	CharacterRotationLastFrame(FRotator(0.f)),
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFram(0.f),
	YawDelta(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_Hip),
	RecoilWeight(1.0f),
	bTurningInPlace(false) ,
	bEquipping(false),
	EquippedWeaponType(EWeaponType::EWT_MAX),
	bShouldUseFABRIK(false)
{

}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (shooterCharacter==nullptr)
	{
		shooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (shooterCharacter)
	{
		bCrouching = shooterCharacter->GetCrouching();
		bReloading = shooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
		bEquipping = shooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;
		bShouldUseFABRIK = shooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied || shooterCharacter->GetCombatState() == ECombatState::ECS_FireTimerInProgress;


		// 캐릭터 스피드 를 가져옴 
		FVector Velocity{ shooterCharacter ->GetVelocity()};
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// 캐릭터 가 점프 한상황일때 
		bIsInAir = shooterCharacter->GetCharacterMovement()->IsFalling();

		//캐릭터가 움직이고있는가 
		if (shooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = shooterCharacter->GetBaseAimRotation();

		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(shooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if (shooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = shooterCharacter->GetAiming();

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (shooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}

		// 캐릭터가 유효한 무기를 가지고있는지 확인
		if (shooterCharacter->GetEquippedWeapon())
		{
			EquippedWeaponType = shooterCharacter->GetEquippedWeapon()->GetWeaponType();
		}
	}
	TurnInPlace();
	Lean(DeltaTime);

}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	shooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

//제자리 회전 처리 변수
void UShooterAnimInstance::TurnInPlace()
{
	if (shooterCharacter == nullptr) return;

	Pitch = shooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		//움직이고 있는 캐릭터를 제자리에 돌리고 싶지 않음

		RootYawOffset = 0.f;
		TIPCharacterYaw = shooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFram = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		TIPCharacterYawLastFram = TIPCharacterYaw;
		TIPCharacterYaw = shooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFram };

		// 루트 요 오프셋이 업데이트되고 [-180,180]으로 고정됨
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		// 회전하면 1.0, 회전하지 않으면 0.0
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// RootYawOffset이 0보다 크면 왼쪽 회전, RootYawOffset이 0보다 작으면 오른쪽 회전
			
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			// ABS는 절대값 
			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else
		{
			bTurningInPlace = false;
		}
	}
	// 반동 무게 설정
	if (bTurningInPlace)
	{
		if (bReloading || bEquipping)
		{
			RecoilWeight = 1.f;
		}
		else
		{
			RecoilWeight = 0.f;
		}

	}
	else  // 제자리에서 돌리지 않음
	{
		if (bCrouching)
		{
			if (bReloading || bEquipping)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.1f;
			}
		}
		else
		{
			if (bAiming || bReloading || bEquipping)
			{
				RecoilWeight = 1.f;
			}															 
			else
			{
				RecoilWeight = 0.5f;
			}
		}
	}
}

// 달리는 동안 기울기 계산 처리
void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (shooterCharacter == nullptr) return;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = shooterCharacter->GetActorRotation();

	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame) };

	const float Target{ Delta.Yaw / DeltaTime };

	// 시간이 조정된 Target쪽으로 보간 
	const float Interp {FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f)};

	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);


}
