// Fill out your copyright notice in the Description page of Project Settings.


#include "DreamAccountSubsystem.h"

#include "DreamAccountSettings.h"
#include "DreamAccountUtil.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UDreamAccountSubsystem::UserRegister(FDreamAccountUser User, FOnAccountResult OnResult)
{
	auto Callback = [OnResult](const FDreamAccountResult& Result)
	{
		if (OnResult.IsBound())
		{
			OnResult.Execute(Result);
		}
	};

	UserRegister_Internal(User, Callback);
}


void UDreamAccountSubsystem::UserRegister_Internal(FDreamAccountUser User, FDreamAccountResultCallback Callback)
{
	if (User.UserName.IsEmpty() || User.UserPassword.IsEmpty())
	{
		Callback(FDreamAccountResult(EDreamAccountResultType::Register, TEXT(""), TEXT("Username and password cannot be empty"), FDreamAccountUser()));
		return;
	}

	const UDreamAccountSettings* Settings = UDreamAccountSettings::Get();
	if (!Settings)
	{
		Callback(FDreamAccountResult(EDreamAccountResultType::Register, TEXT(""), TEXT("Account settings not available"), FDreamAccountUser()));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("user_name"), User.UserName);
	JsonObject->SetStringField(TEXT("user_password"), User.UserPassword);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TMap<FString, FString> Headers;
	Headers.Add(TEXT("Content-Type"), TEXT("application/json;charset=UTF-8"));

	FDreamAccountUtil::SendHttpRequest(
		Settings->AccountServerApiURL + TEXT("/api/account/register"),
		TEXT("POST"),
		JsonString,
		Headers,
		[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!bWasSuccessful || !Response.IsValid())
			{
				Callback(FDreamAccountResult(EDreamAccountResultType::Register, TEXT(""), TEXT("Network request failed"), FDreamAccountUser()));
				return;
			}

			if (Response->GetResponseCode() != 200 && Response->GetResponseCode() != 201)
			{
				FDreamAccountUtil::HandleCommonErrorResponse(Response, EDreamAccountResultType::Register, Callback);
				return;
			}

			TSharedPtr<FJsonObject> Json = FDreamAccountUtil::ParseJsonFromResponse(Response);
			FDreamAccountUser ResultUser;
			if (Json.IsValid())
			{
				const TSharedPtr<FJsonObject>* UserObj;
				if (Json->TryGetObjectField(TEXT("user"), UserObj))
				{
					ResultUser = FDreamAccountUser(UserObj);
				}
			}

			FString Message = Json.IsValid() ? Json->GetStringField(TEXT("message")) : TEXT("");
			Callback(FDreamAccountResult(EDreamAccountResultType::Register, Message, TEXT("NORMAL"), ResultUser));
		});
}


void UDreamAccountSubsystem::UserLogin(FDreamAccountUser User, FOnAccountResult OnResult)
{
	auto Callback = [OnResult](const FDreamAccountResult& Result)
	{
		if (OnResult.IsBound())
		{
			OnResult.Execute(Result);
		}
	};

	UserLogin_Internal(User, Callback);
}


void UDreamAccountSubsystem::UserLogin_Internal(FDreamAccountUser User, FDreamAccountResultCallback Callback)
{
	if (User.UserName.IsEmpty() || User.UserPassword.IsEmpty())
	{
		Callback(FDreamAccountResult(EDreamAccountResultType::Login, TEXT(""), TEXT("Username and password cannot be empty"), FDreamAccountUser()));
		return;
	}

	const UDreamAccountSettings* Settings = UDreamAccountSettings::Get();
	if (!Settings)
	{
		Callback(FDreamAccountResult(EDreamAccountResultType::Login, TEXT(""), TEXT("Account settings not available"), FDreamAccountUser()));
		return;
	}

	// 构造 JSON 请求体
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("user_name"), User.UserName);
	JsonObject->SetStringField(TEXT("user_password"), User.UserPassword);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TMap<FString, FString> Headers;
	Headers.Add(TEXT("Content-Type"), TEXT("application/json;charset=UTF-8"));

	FDreamAccountUtil::SendHttpRequest(
		Settings->AccountServerApiURL + TEXT("/api/account/login"),
		TEXT("POST"),
		JsonString,
		Headers,
		[this, Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!bWasSuccessful || !Response.IsValid())
			{
				Callback(FDreamAccountResult(EDreamAccountResultType::Login, TEXT(""), TEXT("Network request failed"), FDreamAccountUser()));
				return;
			}

			if (Response->GetResponseCode() != 200 && Response->GetResponseCode() != 201)
			{
				FDreamAccountUtil::HandleCommonErrorResponse(Response, EDreamAccountResultType::Login, Callback);
				return;
			}

			TSharedPtr<FJsonObject> Json = FDreamAccountUtil::ParseJsonFromResponse(Response);

			FDreamAccountUser LoggedInUser;
			FString Token;

			if (Json.IsValid())
			{
				const TSharedPtr<FJsonObject>* UserObject;
				if (Json->TryGetObjectField(TEXT("user"), UserObject))
				{
					LoggedInUser = FDreamAccountUser(UserObject);
				}
				Json->TryGetStringField(TEXT("token"), Token);
			}

			if (!Token.IsEmpty())
			{
				SetToken(Token);
			}

			FString Message = Json.IsValid() ? Json->GetStringField(TEXT("message")) : TEXT("");
			Callback(FDreamAccountResult(EDreamAccountResultType::Login, Message, TEXT("NORMAL"), LoggedInUser, Token));
		});
}


void UDreamAccountSubsystem::AuthToken(FOnAccountResult OnResult)
{
	auto Callback = [OnResult](const FDreamAccountResult& Result)
	{
		if (OnResult.IsBound())
		{
			OnResult.Execute(Result);
		}
	};

	AuthToken_Internal(Callback);
}


void UDreamAccountSubsystem::AuthToken_Internal(FDreamAccountResultCallback Callback)
{
	if (Token.IsEmpty())
	{
		Callback(FDreamAccountResult(
			EDreamAccountResultType::Auth,
			TEXT(""),
			TEXT("No token available for authentication"),
			FDreamAccountUser()));
		return;
	}

	const UDreamAccountSettings* Settings = UDreamAccountSettings::Get();
	if (!Settings)
	{
		Callback(FDreamAccountResult(
			EDreamAccountResultType::Auth,
			TEXT(""),
			TEXT("Account settings not available"),
			FDreamAccountUser()));
		return;
	}

	TMap<FString, FString> Headers;
	Headers.Add(TEXT("Content-Type"), TEXT("application/json;charset=UTF-8"));
	Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));

	FDreamAccountUtil::SendHttpRequest(
		Settings->AccountServerApiURL + TEXT("/api/account/auth"),
		TEXT("GET"),
		TEXT(""), // No body for GET
		Headers,
		[Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!bWasSuccessful || !Response.IsValid())
			{
				Callback(FDreamAccountResult(EDreamAccountResultType::Auth, TEXT(""), TEXT("Network request failed"), FDreamAccountUser()));
				return;
			}

			if (Response->GetResponseCode() != 200 && Response->GetResponseCode() != 201)
			{
				FDreamAccountUtil::HandleCommonErrorResponse(Response, EDreamAccountResultType::Auth, Callback);
				return;
			}

			TSharedPtr<FJsonObject> Json = FDreamAccountUtil::ParseJsonFromResponse(Response);

			FDreamAccountUser AuthUser;
			if (Json.IsValid())
			{
				Json->TryGetStringField(TEXT("user_name"), AuthUser.UserName);
				Json->TryGetNumberField(TEXT("user_id"), AuthUser.UserID);
			}

			FString Message = Json.IsValid() ? Json->GetStringField(TEXT("message")) : TEXT("");

			Callback(FDreamAccountResult(EDreamAccountResultType::Auth, Message, TEXT("NORMAL"), AuthUser));
		});
}


void UDreamAccountSubsystem::UserLogout()
{
	ClearToken();
}


void UDreamAccountSubsystem::ClearToken()
{
	SetToken(FString());
}


void UDreamAccountSubsystem::SetToken(FString NewToken)
{
	Token = NewToken;

	OnTokenChanged.Broadcast();
}
