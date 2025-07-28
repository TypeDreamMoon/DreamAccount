#include "DreamAccountTypes.h"

FDreamAccountUser::FDreamAccountUser(const TSharedRef<FJsonObject>& InUserJsonObject)
{
	FString JsonUserName;
	bool bHasUserName = InUserJsonObject->TryGetStringField(TEXT("user_name"), JsonUserName);
	int32 JsonUserId;
	bool bHasUserId = InUserJsonObject->TryGetNumberField(TEXT("user_id"), JsonUserId);

	UserName = bHasUserName ? JsonUserName : FString();
	UserID = bHasUserId ? JsonUserId : 9999;
}

FDreamAccountUser::FDreamAccountUser(const TSharedPtr<FJsonObject>* InUserJsonObject)
{
	FString JsonUserName;
	bool bHasUserName = InUserJsonObject->Get()->TryGetStringField(TEXT("user_name"), JsonUserName);
	int32 JsonUserId;
	bool bHasUserId = InUserJsonObject->Get()->TryGetNumberField(TEXT("user_id"), JsonUserId);

	UserName = bHasUserName ? JsonUserName : FString();
	UserID = bHasUserId ? JsonUserId : 9999;
}
