// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class SHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface // Interface ��� 
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// ü�¹� ���̴� �Լ� 
	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	// ü�¹� ����� �Լ�
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	void Die();

	//  float PlayRate �� �ִϸ��̼� �ӵ� 
	void PlayHitMontage(FName Section, float PlayRate = 1.0f);

	// Ÿ�̸��� �ݹ� �Լ�
	void ResetHitReactTimer();

	//  �ؽ�Ʈ ��� �Լ� 
	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber,FVector Location);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	// ��ĥ�� �ν�
	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate);

	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	UFUNCTION()
	void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ���� �浹 Ȱ��ȭ / ��Ȱ��ȭ
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeapon();
	
	UFUNCTION(BlueprintCallable)
	void ActivateRightWeapon();
	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeapon();

	void DoDamage(class AShooterCharacter* Victim);
	void SpwanBlood(AShooterCharacter* Victim,FName SocketName);

	// ĳ���͸� ������Ű�� �Լ�
	void StunCharacter(AShooterCharacter* Victim);

	void ResetConAttack();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	UFUNCTION()
	void DestroyEnemy();

private:

	// Ÿ���� �Ծ����� �߻��ϴ� ����Ʈ 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParticles;

	// Ÿ������������ �߻��ϴ� ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	// ���� ü�� 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	// �ִ� ä��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	// �ص弦 ��ġ ǥ�� �̸� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

	// ü�¹� ǥ�� �ð� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;

	FTimerHandle HealthBarTimer;

	// ���� ���ݰ� ���� �� �ִ� �ִϸ��̼�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	FTimerHandle HitReactTimer;

	// ���߽� �����ϴ� �ּҽð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitReactTimerMin;

	// ���߽� �����ϴ� �ִ� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitReactTimerMax;
	
	// ��Ʈ ��Ÿ�ָ� ����Ҽ��ִ��� Ȯ��
	bool bCanHitReact;

	//TMap �� ����Ͽ� ������ ���� ��ġ�� �ؽ�Ʈ ���
	UPROPERTY(VisibleAnywhere,Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	// �ؽ�Ʈ ����� ������� �ð�
	UPROPERTY(EditAnywhere,  Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

	// AI 
	UPROPERTY(EditAnywhere,Category = "BehaviroTree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	// ���Ͱ� �����̴� �Ÿ� (����) , MakeEditWidget �ǰ�� ���������� �����ϰ� ó�� 
	UPROPERTY(EditAnywhere, Category = "BehaviroTree", meta = (AllowPrivateAccess = "true",MakeEditWidget = "true"))
	FVector PatrolPoint;

	// ���Ͱ� 2��° �̵��ҰŸ� 
	UPROPERTY(EditAnywhere, Category = "BehaviroTree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;

	class AEnemyController* EnemyController;

	// ���Ͱ� �÷��̾ �ν��� ��ü
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	// �ǰݽ� ���� ����
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bStunned;

	// ���� Ȯ�� ( �ǰݽ� ) 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChane;
	
	// ���ݹ����������� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange;

	// ���ݹ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatRangeSphere;

	// ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	// ��Ÿ�� ���� �̸�
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;		

	// ���� ���� �浹ü
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftWeaponCollision;

	// ������ ���� �浹ü
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	// ���� ���ݷ�
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FName LeftWeaponSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FName RightWeaponSocket;

	// ���� ���ݽð� ����
	UPROPERTY(VisibleAnywhere,Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;

	FTimerHandle AttackWaitTimer;

	// �ּ��� ���� �ð� 
	UPROPERTY(EditAnywhere,Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AttackWaitTime;

	// ���� ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	bool bDying;

	FTimerHandle DeathTimer;

	// ���� ����ð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float DeathTime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �������̽����� ����Ͽ� ����Ͽ� Implementation ���
	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController*ShooterController) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE FString GetHeadBone() const { return HeadBone; }

	// FVector ���� �ؽ�Ʈ ��ġ 
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
