#pragma once

#include "CoreMinimal.h"
#include "DreamAccountTypes.h"
#include "Interfaces/IHttpRequest.h"

/**
 * FDreamAccountUtil类
 * 提供账户相关的工具函数，包括HTTP请求发送、JSON解析和错误处理功能
 */
class DREAMACCOUNT_API FDreamAccountUtil
{
public:
	/**
	 * 发送HTTP请求
	 * @param URL 请求的目标URL地址
	 * @param Verb HTTP请求方法（如GET、POST等）
	 * @param Content 请求体内容
	 * @param Headers HTTP请求头信息映射表
	 * @param OnComplete 请求完成后的回调函数，参数分别为：请求指针、响应指针、是否成功标志
	 */
	static void SendHttpRequest(
		const FString& URL,
		const FString& Verb,
		const FString& Content,
		const TMap<FString, FString>& Headers,
		TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> OnComplete
	);

	/**
	 * 从HTTP响应中解析JSON对象
	 * @param Response HTTP响应指针
	 * @return 解析后的JSON对象共享指针，解析失败时返回空指针
	 */
	static TSharedPtr<FJsonObject> ParseJsonFromResponse(
		FHttpResponsePtr Response);

	/**
	 * 处理通用错误响应
	 * @param Response HTTP响应指针
	 * @param Type 账户结果类型枚举值
	 * @param OnResult 账户结果回调函数
	 */
	static void HandleCommonErrorResponse(
		FHttpResponsePtr Response,
		EDreamAccountResultType Type,
		const FDreamAccountResultCallback& OnResult
	);
};
