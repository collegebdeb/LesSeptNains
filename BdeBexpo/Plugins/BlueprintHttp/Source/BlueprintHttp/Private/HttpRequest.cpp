// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "HttpRequest.h"
#include "HttpModule.h"
#include "BlueprintHttpLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpResponse.h"
#include "Http.h"

UHttpRequest::UHttpRequest()
	: Super()
{
	Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &UHttpRequest::OnRequestCompleteInternal );
	Request->OnRequestProgress       ().BindUObject(this, &UHttpRequest::OnRequestProgressInternal );
	Request->OnHeaderReceived        ().BindUObject(this, &UHttpRequest::OnHeaderReceivedInternal  );
	//Request->OnRequestWillRetry	 ().BindUObject(this, &UHttpRequest::OnRequestWillRetryInternal);

}

UHttpRequest* UHttpRequest::CreateRequest()
{
	return NewObject<UHttpRequest>();
}

UHttpResponse* UHttpRequest::CreateResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe>& RawRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& RawResponse)
{
	UHttpResponse* const WrappedResponse = NewObject<UHttpResponse>();

	WrappedResponse->InitInternal(RawResponse, RawRequest->GetElapsedTime());

	return WrappedResponse;
}

FString UHttpRequest::ConvertEnumVerbToString(const EHttpVerb InVerb)
{
	constexpr uint8 VerbsCount = static_cast<uint8>(EHttpVerb::MAX_COUNT);

	const uint8 Uint8Verb = static_cast<uint8>(InVerb);
	
	if (Uint8Verb >= VerbsCount)
	{
		return TEXT("NONE");
	}

	static const FString Converted[VerbsCount] =
	{
		TEXT("GET"),
		TEXT("POST"),
		TEXT("PUT"),
		TEXT("PATCH"),
		TEXT("HEAD"),
		TEXT("DELETE")
		// TEXT("TRACE"),
		// TEXT("CONNECT"),
		// TEXT("OPTIONS"),
	};

	return Converted[Uint8Verb];
}

void UHttpRequest::SetVerb(const EHttpVerb InVerb)
{
	SetCustomVerb(ConvertEnumVerbToString(InVerb));
}

void UHttpRequest::SetCustomVerb(const FString& Verb)
{
	Request->SetVerb(Verb);
}

void UHttpRequest::SetHeader(const FString& Key, const FString& Value)
{
	Request->SetHeader(Key, Value);
}

void UHttpRequest::SetHeaders(const TMap<FString, FString>& Headers)
{
	for (const auto& Header : Headers)
	{
		Request->SetHeader(Header.Key, Header.Value);
	}
}

void UHttpRequest::AppendToHeader(const FString& Key, const FString& Value)
{
	Request->AppendToHeader(Key, Value);
}

void UHttpRequest::SetURL(const FString& Url)
{
	Request->SetURL(Url);
}

void UHttpRequest::SetMimeType(const EHttpMimeType MimeType)
{
	Request->AppendToHeader(TEXT("Content-Type"), UBlueprintHttpLibrary::CreateMimeType(MimeType));
}

void UHttpRequest::SetMimeTypeAsString(const FString& MimeType)
{
	Request->AppendToHeader(TEXT("Content-Type"), MimeType);
}

void UHttpRequest::SetContent(const TArray<uint8>& Content)
{
	Request->SetContent(Content);
}

void UHttpRequest::SetContentAsString(const FString & Content)
{
	Request->SetContentAsString(Content);
}

void UHttpRequest::SetContentAsStreamedFile(const FString& FileName, bool& bFileValid)
{
	bFileValid = Request->SetContentAsStreamedFile(FileName);
}

TMap<FString, FString> UHttpRequest::GetAllHeaders() const
{
	TArray<FString> Headers = Request->GetAllHeaders();

	TMap<FString, FString> OutHeaders;
	
	FString* Key	= nullptr;
	FString* Value	= nullptr;

	const FString Separator = TEXT(": ");

	for (FString & Header : Headers)
	{
		if (Header.Split(Separator, Key, Value, ESearchCase::CaseSensitive))
		{
			OutHeaders.Emplace(*Key, *Value);
		}
	}

	return OutHeaders;
}

void UHttpRequest::GetContent(TArray<uint8>& OutContent) const
{
	OutContent = Request->GetContent();
}

FString UHttpRequest::GetContentAsString() const
{
	const TArray<uint8>& Content = Request->GetContent();

	return BytesToString(Content.GetData(), Content.Num());
}

int32 UHttpRequest::GetContentLength() const
{
	return Request->GetContentLength();
}

FString UHttpRequest::GetContentType() const
{
	return Request->GetContentType();
}

float UHttpRequest::GetElapsedTime() const
{
	return Request->GetElapsedTime();
}

FString UHttpRequest::GetHeader(const FString& Key) const
{
	return Request->GetHeader(Key);
}

EBlueprintHttpRequestStatus UHttpRequest::GetStatus() const
{
	return static_cast<EBlueprintHttpRequestStatus>(Request->GetStatus());
}

FString UHttpRequest::GetURL() const
{
	return Request->GetURL();
}

FString UHttpRequest::GetURLParameter(const FString& ParameterName) const
{
	return Request->GetURLParameter(ParameterName);
}

FString UHttpRequest::GetVerb() const
{
	return Request->GetVerb();
}

bool UHttpRequest::ProcessRequest()
{
	if (Request->GetContentType() == TEXT(""))
	{
		SetMimeType(EHttpMimeType::txt);
	}

	return Request->ProcessRequest();
}

void UHttpRequest::CancelRequest()
{
	Request->CancelRequest();
}

void UHttpRequest::OnRequestCompleteInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>  RawResponse, bool bConnectedSuccessfully)
{
	OnRequestComplete.Broadcast(this, CreateResponse(RawRequest, RawResponse), bConnectedSuccessfully);
}

void UHttpRequest::OnRequestProgressInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, const int32 BytesSent, const int32 BytesReceived)
{
	OnRequestProgress.Broadcast(this, BytesSent, BytesReceived);
}

void UHttpRequest::OnHeaderReceivedInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, const FString& HeaderName, const FString& HeaderValue)
{
	OnRequestHeaderReceived.Broadcast(this, HeaderName, HeaderValue);
}

void UHttpRequest::OnRequestWillRetryInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> RawResponse, float SecondsToRetry)
{
	OnRequestWillRetry.Broadcast(this, CreateResponse(RawRequest, RawResponse), SecondsToRetry);
}

