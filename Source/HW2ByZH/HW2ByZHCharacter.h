// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EquipActor.h"
#include "GameFramework/Character.h"
#include "HW2ByZHCharacter.generated.h"

UENUM(BlueprintType)
enum class FightState:uint8
{
	OutFight UMETA(DisplayName = "非战斗"),
	GUNMODE UMETA(DisplayName = "持枪"),
	LEIMODE UMETA(DisplayName = "持雷")
};

UCLASS(config=Game)
class AHW2ByZHCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AHW2ByZHCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// 战斗状态，T为战斗态，F为普通态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Fight)
	bool bFightState;

	// 战斗状态使用动画资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fight)
	class TSubclassOf<UAnimInstance> FightAnimClass;

	// 非战斗状态使用动画资产
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fight)
	class TSubclassOf<UAnimInstance> NormalAnimClass;

	// 正在装备的武器
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	AEquipActor* EquippedWeapon;
	
	// 由EquipActor自动维护的进入拾取范围的武器
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<AEquipActor*> WeaponWhichCanPickSet;

protected:

	// 战斗状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=FightState)
	FightState FightState = FightState::OutFight;

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	
	// 切换战斗态
	void SwitchFightState(enum FightState ChangeFightState);
	
	// 按T
	void Test();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// 捡起武器
	UFUNCTION(BlueprintCallable)
	void PickUpWeapon(AActor* Weapon);

	// 按F捡起附件武器
	UFUNCTION()
	void PickUpWeaponByOverlap();

	// 丢弃武器
	UFUNCTION(BlueprintCallable)
	void DestroyNowWeapon();
};