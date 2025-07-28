#pragma once

#include "CoreMinimal.h"
#include "DreamAccountTypes.generated.h"

struct FDreamAccountUser;
struct FDreamAccountResult;
enum class EDreamAccountResultType : uint8;

using FDreamAccountResultCallback = TFunction<void(const FDreamAccountResult&)>;

/**
 * @brief 账户操作结果类型枚举
 * 
 * 定义了用户账户相关操作的结果类型，用于标识不同的账户操作状态
 * 该枚举可以在蓝图中使用
 */
UENUM(BlueprintType)
enum class EDreamAccountResultType : uint8
{
	None, // 无操作/初始状态
	Register, // 注册操作
	Login, // 登录操作
	Auth, // 认证操作
};


/**
 * FDreamAccountUser 结构体
 * 
 * 该结构体用于表示用户账户信息，包含用户的基本信息如用户名、密码、用户ID等。
 * 支持从JSON对象初始化用户数据。
 */
USTRUCT(BlueprintType)
struct FDreamAccountUser
{
	GENERATED_BODY()

public:
	/**
	 * 默认构造函数
	 * 
	 * 初始化一个空的用户账户对象
	 */
	FDreamAccountUser()
	{
	}

	/**
	 * 从TSharedRef<FJsonObject>构造函数
	 * 
	 * 通过传入的JSON对象引用初始化用户账户信息
	 * 
	 * @param InUserJsonObject 包含用户信息的JSON对象引用
	 */
	FDreamAccountUser(const TSharedRef<FJsonObject>& InUserJsonObject);

	/**
	 * 从TSharedPtr<FJsonObject>指针构造函数
	 * 
	 * 通过传入的JSON对象指针初始化用户账户信息
	 * 
	 * @param InUserJsonObject 指向包含用户信息的JSON对象的指针
	 */
	FDreamAccountUser(const TSharedPtr<FJsonObject>* InUserJsonObject);

public:
	/** 用户名 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserName = FString();

	/** 用户密码 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserPassword = FString();

	/** 用户ID，默认值为9999 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UserID = 9999;

	/** 用户登录禁用标志，true表示禁用登录 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUserLoginDisable = false;

	/** 用户登录禁用原因，默认为"normal login" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserLoginDisableReason = TEXT("normal login");
};


/**
 * @brief 账户操作结果结构体，用于存储账户注册或登录的结果信息
 * 
 * 该结构体继承自USTRUCT，可以在Unreal Engine的Blueprint中使用，
 * 用于封装账户操作的结果，包括操作类型、消息、错误原因、用户信息和访问令牌等
 */
USTRUCT(BlueprintType)
struct FDreamAccountResult
{
	GENERATED_BODY()

public:
	/**
	 * @brief 默认构造函数
	 * 
	 * 初始化账户结果结构体，将有效性标志设置为false
	 */
	FDreamAccountResult()
		: bIsValidResult(false)
	{
	}

	/**
	 * @brief 注册结果构造函数
	 * 
	 * 用于创建注册操作的结果对象
	 * 
	 * @param InResultType 操作结果类型枚举值
	 * @param InMessage 操作返回的消息信息
	 * @param InErrorReason 错误原因描述
	 * @param InUser 账户用户信息对象
	 */
	FDreamAccountResult(EDreamAccountResultType InResultType, FString InMessage, FString InErrorReason, FDreamAccountUser InUser)
		: ResultType(InResultType), Message(InMessage), ErrorReason(InErrorReason), User(InUser), bIsValidResult(true)
	{
	}

	/**
	 * @brief 登录结果构造函数
	 * 
	 * 用于创建登录操作的结果对象，包含访问令牌
	 * 
	 * @param InResultType 操作结果类型枚举值
	 * @param InMessage 操作返回的消息信息
	 * @param InErrorReason 错误原因描述
	 * @param InUser 账户用户信息对象
	 * @param InToken 访问令牌字符串
	 */
	FDreamAccountResult(EDreamAccountResultType InResultType, FString InMessage, FString InErrorReason, FDreamAccountUser InUser, FString InToken)
		: ResultType(InResultType), Message(InMessage), ErrorReason(InErrorReason), User(InUser), Token(InToken), bIsValidResult(true)
	{
	}

public:
	/** 操作结果类型，标识账户操作的具体结果 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDreamAccountResultType ResultType = EDreamAccountResultType::None;

	/** 操作返回的消息信息，通常包含操作结果的描述 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message = TEXT("");

	/** 错误原因描述，默认值为"normal login" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ErrorReason = TEXT("normal login");

	/** 账户用户信息对象，包含用户的基本信息 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDreamAccountUser User = FDreamAccountUser();

	/** 访问令牌字符串，用于用户身份验证 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Token = TEXT("");

	/** 结果有效性标志，标识该结果对象是否包含有效数据 */
	bool bIsValidResult;
};
