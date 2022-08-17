// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyController();

	virtual void OnPossess(APawn* InPawn) override;

private:

	// Blackboard Component AI 
	UPROPERTY(BlueprintReadWrite,Category = "AI Behavior",meta=(AllowPrivateAccess="true"))
	class UBlackboardComponent* BlackboardComponent;

	// 利 俊 措茄 青悼飘府 备己 
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta=(AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

public:
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }

};
