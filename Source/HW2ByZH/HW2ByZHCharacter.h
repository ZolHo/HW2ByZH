// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EquipActor.h"
#include "GunActor.h"
#include "LeiActor.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "HW2ByZHCharacter.generated.h"

UENUM(BlueprintType)
enum class FightState:uint8
{
	OutFight UMETA(DisplayName = "非战斗"),
	GUNMODE UMETA(DisplayName = "持枪"),
	LEIMODE UMETA(DisplayName = "持雷")
};

// 战斗状态切换时候的代理
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeStateDelegate);

// 子弹数目会出现变化的时候，通知给UMG更改UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmorNumerChangeDelegate);

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
	
	// 另一个CameraBoom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom2;
	// 另一个摄像机
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera2;
	
	
public:
	AHW2ByZHCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// // 战斗状态使用动画资产
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fight)
	// class TSubclassOf<UAnimInstance> FightAnimClass;
	//
	// // 非战斗状态使用动画资产
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fight)
	// class TSubclassOf<UAnimInstance> NormalAnimClass;

	// 是否开镜状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Fight)
	bool bIsOpenMirror = false;

	// 正在装备的武器
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	AEquipActor* EquippedWeapon;
	
	// 由EquipActor自动维护的进入拾取范围的武器
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Weapon)
	TSet<AEquipActor*> WeaponWhichCanPickSet;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation_Hip;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation_Aim;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	// 枪类装备槽
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon)
	AGunActor* GunWeapon;

	// 雷雷装备槽
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon)
	ALeiActor* LeiWeapon;

	// 弹孔贴图
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	UMaterial* BulletHoleMaterial;

	// 子弹粒子特效
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	UParticleSystem* BulletHitParticleSystem;

	
	// 变换战斗状态代理，把一些处理交给蓝图及关卡蓝图等
	UPROPERTY(BlueprintAssignable)
	FChangeStateDelegate ChangeStateDelegate;

	// 子弹变化
	UPROPERTY(BlueprintAssignable)
	FOnAmorNumerChangeDelegate OnAmorNumerChangeDelegate;

	UPROPERTY()
	USplineComponent* ProjectileSplineComponent;

protected:

	// 战斗状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=FightState)
	FightState NowFightState = FightState::OutFight;

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

	// 鼠标点击左键事件处理
	void LeftClickDispatch() ;

	// 鼠标点击右键事件处理
	void RightClickDispatch();

	// 鼠标左键松开事件处理
	void LeftClickReleasedDispatch();
	
	// 处理开枪效果
	void OnGunFire();
	
	// 按T
	void Test();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	// 切换开镜
	void ToggleOpenMirror();

	// 获取手雷抛出的方向和速度
	FVector GetLeiVelocityForce();

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

	// 装弹函数
	UFUNCTION(BlueprintCallable)
	void ReBullet();

private:
	// tick函数
	virtual void Tick(float DeltaSeconds) override;

	// 是否未弹起左键，实现长按功能
	bool bIsLeftPressed;
};