// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "HttpResponse.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

UHttpResponse::UHttpResponse()
	: Super()
	, RequestDuration(0.f)
{}

void UHttpResponse::InitInternal(TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> InResponse, const float& InRequestDuration)
{
	Response = InResponse;
	RequestDuration = InRequestDuration;
}

TMap<FString, FString> UHttpResponse::GetAllHeaders() const
{
	if (!Response)
	{
		return TMap<FString, FString>();
	}

	TArray<FString> Headers = Response->GetAllHeaders();

	TMap<FString, FString> OutHeaders;
	
	FString* Key;
	FString* Value;

	for (FString & Header : Headers)
	{
		Key		= nullptr;
		Value	= nullptr;

		Header.Split(TEXT(": "), Key, Value, ESearchCase::CaseSensitive);

		if (Key && Value)
		{
			OutHeaders.Emplace(*Key, *Value);
		}
	}

	return OutHeaders;
}

void UHttpResponse::GetContent(TArray<uint8>& OutContent) const
{
	if (Response)
	{
		OutContent = Response->GetContent();
	}
}

FString UHttpResponse::GetContentAsString() const
{
	if (Response)
	{
		return Response->GetContentAsString();
	}
	return TEXT("");
}

int32 UHttpResponse::GetContentLength() const
{
	return Response ? Response->GetContentLength() : 0;
}

FString UHttpResponse::GetContentType() const
{
	return Response ? Response->GetContentType() : TEXT("");
}

FString UHttpResponse::GetHeader(const FString& Key) const
{
	return Response ? Response->GetHeader(Key) : TEXT("");
}

int32 UHttpResponse::GetResponseCode() const
{
	return Response ? Response->GetResponseCode() : -1;
}

FString UHttpResponse::GetURL() const
{
	return Response ? Response->GetURL() : TEXT("");
}

FString UHttpResponse::GetURLParameter(const FString& ParameterName) const
{
	return Response ? Response->GetURLParameter(ParameterName) : TEXT("");
}

float UHttpResponse::GetElapsedTime() const
{
	return RequestDuration;
}

