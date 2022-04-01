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


		// ĳ���� ���ǵ� �� ������ 
		FVector Velocity{ shooterCharacter ->GetVelocity()};
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// ĳ���� �� ���� �ѻ�Ȳ�϶� 
		bIsInAir = shooterCharacter->GetCharacterMovement()->IsFalling();

		//ĳ���Ͱ� �����̰��ִ°� 
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

		// ĳ���Ͱ� ��ȿ�� ���⸦ �������ִ��� Ȯ��
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

//���ڸ� ȸ�� ó�� ����
void UShooterAnimInstance::TurnInPlace()
{
	if (shooterCharacter == nullptr) return;

	Pitch = shooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0 || bIsInAir)
	{
		//�����̰� �ִ� ĳ���͸� ���ڸ��� ������ ���� ����

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

		// ��Ʈ �� �������� ������Ʈ�ǰ� [-180,180]���� ������
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		// ȸ���ϸ� 1.0, ȸ������ ������ 0.0
		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			// RootYawOffset�� 0���� ũ�� ���� ȸ��, RootYawOffset�� 0���� ������ ������ ȸ��
			
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			// ABS�� ���밪 
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
	// �ݵ� ���� ����
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
	else  // ���ڸ����� ������ ����
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

// �޸��� ���� ���� ��� ó��
void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (shooterCharacter == nullptr) return;
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = shooterCharacter->GetActorRotation();

	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation,CharacterRotationLastFrame) };

	const float Target{ Delta.Yaw / DeltaTime };

	// �ð��� ������ Target������ ���� 
	const float Interp {FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f)};

	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);


}
