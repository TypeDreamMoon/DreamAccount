#include "DreamAccountUtil.h"

#include "HttpModule.h"
#include "Http.h"

void FDreamAccountUtil::SendHttpRequest(const FString& URL, const FString& Verb, const FString& Content, const TMap<FString, FString>& Headers, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> OnComplete)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(Verb);
	HttpRequest->SetContentAsString(Content);
	HttpRequest->SetTimeout(30); // 30秒超时

	// 设置所有Header
	for (const TPair<FString, FString>& Pair : Headers)
	{
		HttpRequest->SetHeader(Pair.Key, Pair.Value);
	}

	HttpRequest->OnProcessRequestComplete().BindLambda(OnComplete);

	HttpRequest->ProcessRequest();
}

TSharedPtr<FJsonObject> FDreamAccountUtil::ParseJsonFromResponse(FHttpResponsePtr Response)
{
	FString Content = Response->GetContentAsString();
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		return JsonObject;
	}
	return nullptr;
}

void FDreamAccountUtil::HandleCommonErrorResponse(FHttpResponsePtr Response, EDreamAccountResultType Type, const FDreamAccountResultCallback& OnResult)
{
	TSharedPtr<FJsonObject> Json = ParseJsonFromResponse(Response);
	FString Message = Json.IsValid() ? Json->GetStringField(TEXT("message")) : TEXT("Unknown Error");
	FString Error = Json.IsValid() ? Json->GetStringField(TEXT("error")) : TEXT("UNKNOWN_ERROR");

	OnResult(FDreamAccountResult(Type, Message, Error, FDreamAccountUser()));
}
