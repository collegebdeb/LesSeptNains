// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "BlueprintHttpLibrary.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Http.h"
#include "HttpModule.h"
#include "Misc/Base64.h"
#include "EngineMinimal.h"

EHttpVerb UBlueprintHttpLibrary::StringToVerb(const FString& Verb)
{
	if (TEXT("POST")   == Verb) return EHttpVerb::POST;
	if (TEXT("PUT")    == Verb) return EHttpVerb::PUT;
	if (TEXT("PATCH")  == Verb) return EHttpVerb::PATCH;
	if (TEXT("HEAD")   == Verb) return EHttpVerb::HEAD;
	if (TEXT("DELETE") == Verb) return EHttpVerb::DELETE;
	return EHttpVerb::GET;
}

void UBlueprintHttpLibrary::HttpGlobal_GetAllowedDomains(TArray<FString>& AllowedDomains)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	AllowedDomains = FHttpModule::Get().GetAllowedDomains();								   
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

int32 UBlueprintHttpLibrary::HttpGlobal_GetHttpMaxConnectionsPerServer() 
{ 
	return FHttpModule::Get().GetHttpMaxConnectionsPerServer();	   
}

FString UBlueprintHttpLibrary::HttpGlobal_GetProxyAddress() 
{ 
	return FHttpModule::Get().GetProxyAddress();									   
}

int32 UBlueprintHttpLibrary::HttpGlobal_GetMaxReadBufferSize()
{
	return FHttpModule::Get().GetMaxReadBufferSize();
}

void UBlueprintHttpLibrary::HttpGlobal_AddDefaultHeader(const FString& HeaderName, const FString& HeaderValue)
{
	FHttpModule::Get().AddDefaultHeader(HeaderName, HeaderValue);
}

void UBlueprintHttpLibrary::HttpGlobal_SetProxyAddress(const FString& InProxyAddress)
{
	FHttpModule::Get().SetProxyAddress(InProxyAddress);
}

void UBlueprintHttpLibrary::HttpGlobal_SetMaxReadBufferSize(const int32 SizeInBytes)
{
	FHttpModule::Get().SetMaxReadBufferSize(SizeInBytes);
}

void UBlueprintHttpLibrary::HttpGlobal_SetHttpTimeout(const float Timeout)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	FHttpModule::Get().SetHttpTimeout(Timeout);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UBlueprintHttpLibrary::HttpGlobal_SetHttpThreadIdleMinimumSleepTimeInSeconds(const float Time)
{
	FHttpModule::Get().SetHttpThreadIdleMinimumSleepTimeInSeconds(Time);
}

void UBlueprintHttpLibrary::HttpGlobal_SetHttpThreadIdleFrameTimeInSeconds(const float Time)
{
	FHttpModule::Get().SetHttpThreadIdleFrameTimeInSeconds(Time);
}

void UBlueprintHttpLibrary::HttpGlobal_SetHttpThreadActiveMinimumSleepTimeInSeconds(const float Time)
{
	FHttpModule::Get().SetHttpThreadActiveMinimumSleepTimeInSeconds(Time);
}

void UBlueprintHttpLibrary::HttpGlobal_SetHttpThreadActiveFrameTimeInSeconds(const float Time)
{
	FHttpModule::Get().SetHttpThreadActiveFrameTimeInSeconds(Time);
}

void UBlueprintHttpLibrary::HttpGlobal_SetHttpDelayTime(const float Delay)
{
	FHttpModule::Get().SetHttpDelayTime(Delay);
}

const UEnum* GetEHttpResponseCodeEnumPointer()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	static const UEnum* const HttpResponseCodeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EHttpResponseCode"));
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	return HttpResponseCodeEnum;
}

FString UBlueprintHttpLibrary::HttpResponseCodeToString(const int32 ResponseCode)
{
	const UEnum* const HttpResponseCodeEnum = GetEHttpResponseCodeEnumPointer();

	if (!HttpResponseCodeEnum)
	{
		UE_LOG(LogHttp, Error, TEXT("EHttpResponseCode doesn't exist. Can't convert response code to string."));
		return TEXT("Failed to find lookup data.");
	}
	
	const EHttpResponseCode Code = HttpResponseCodeToEnum(ResponseCode);

	if (Code == EHttpResponseCode::CUknown)
	{
		return FString::Printf(TEXT("%d Unofficial Response Code"), ResponseCode);
	}

	return HttpResponseCodeEnum->GetDisplayNameTextByValue(static_cast<uint32>(Code)).ToString();
}


FString UBlueprintHttpLibrary::HttpResponseCodeToDescriptionString(const int32 ResponseCode)
{
#if WITH_EDITOR
	const UEnum* const HttpResponseCodeEnum = GetEHttpResponseCodeEnumPointer();

	if (!HttpResponseCodeEnum)
	{
		UE_LOG(LogHttp, Error, TEXT("EHttpResponseCode doesn't exist. Can't convert response code to string."));
		return TEXT("Failed to find lookup data.");
	}

	const EHttpResponseCode Code = HttpResponseCodeToEnum(ResponseCode);

	if (Code == EHttpResponseCode::CUknown)
	{
		return FString::Printf(TEXT("%d is an unofficial response code."), ResponseCode);
	}

	return HttpResponseCodeEnum->GetToolTipTextByIndex(static_cast<uint32>(Code)).ToString();
#else
	return TEXT("");
#endif
}

EHttpResponseCode UBlueprintHttpLibrary::HttpResponseCodeToEnum(const int32 ResponseCode)
{
#define C(Code) case Code: return EHttpResponseCode::C ## Code;
	switch (ResponseCode)
	{
		// 1XX Information Responses
		C(100);C(101);C(102);C(103);

		// 2XX Successful Responses
		C(200);C(201);C(202);C(203);C(204);C(205);C(206);C(207);C(208);C(226);

		// 3XX Redirection Messages
		C(300);C(301);C(302);C(303);C(304);C(305);C(306);C(307);C(308);

		// 4xx Client Error Responses
		C(400);C(401);C(402);C(403);C(404);C(405);C(406);C(407);C(408);C(409);C(410);
			   C(411);C(412);C(413);C(414);C(415);C(416);C(417);C(418);C(421);C(422);
			   C(423);C(424);C(425);C(426);C(428);C(429);C(431);C(451);

		// 5xx Server Error Messages
		C(500);C(501);C(502);C(503);C(504);C(505);C(506);C(507);C(508);C(510);C(511);

	default:
		return EHttpResponseCode::CUknown;
	}
#undef C
}

UHttpRequest* UBlueprintHttpLibrary::CreateInitializedRequest(const FString& Url, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const FString& Content, const TMap<FString, FString>& Headers)
{
	UHttpRequest* const Request = NewObject<UHttpRequest>();

	Request->SetURL(AddParametersToUrl(Url, UrlParameters));
	Request->SetVerb(Verb);
	Request->SetContentAsString(Content);
	Request->SetMimeType(MimeType);
	
	for (const auto& Pair : Headers)
	{
		Request->SetHeader(Pair.Key, Pair.Value);
	}

	return Request;
}

UHttpRequest* UBlueprintHttpLibrary::CreateInitializedBinaryRequest(const FString& Url, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const TArray<uint8>& Content, const TMap<FString, FString>& Headers)
{
	UHttpRequest* const Request = NewObject<UHttpRequest>();

	Request->SetURL(AddParametersToUrl(Url, UrlParameters));
	Request->SetVerb(Verb);
	Request->SetContent(Content);
	Request->SetMimeType(MimeType);
	
	for (const auto& Pair : Headers)
	{
		Request->SetHeader(Pair.Key, Pair.Value);
	}

	return Request;
}

FString UBlueprintHttpLibrary::AddParametersToUrl(FString InUrl, const TMap<FString, FString>& Parameters)
{
	const int32 ParametersCount = Parameters.Num();

	if (ParametersCount < 1)
	{
		return InUrl;
	}

	InUrl += TEXT("?");

	int32 i = 0;
	for (auto It = Parameters.begin(); It != Parameters.end(); ++i, ++It)
	{
		InUrl += FPlatformHttp::UrlEncode(It->Key) + TEXT("=") + FPlatformHttp::UrlEncode(It->Value);
		if (i != ParametersCount - 1)
		{
			InUrl += TEXT("&");
		}
	}

	return InUrl; 
}

FString UBlueprintHttpLibrary::CreateMimeType(const EHttpMimeType Type)
{
	static const TMap<EHttpMimeType, FString> Enum =
	{
		{EHttpMimeType::url		,"application/x-www-form-urlencoded",		},
		{EHttpMimeType::form	,"multipart/form-data",						},
		{EHttpMimeType::txt		,"text/plain",								},
		{EHttpMimeType::html	,"text/html",								},
		{EHttpMimeType::aac		,"audio/aac",								},
		{EHttpMimeType::arc		,"application/x-freearc",					},
		{EHttpMimeType::avi		,"video/x-msvideo",							},
		{EHttpMimeType::bin		,"application/octet-stream",				},
		{EHttpMimeType::bmp		,"image/bmp",								},
		{EHttpMimeType::css		,"text/css",								},
		{EHttpMimeType::csv		,"text/csv",								},
		{EHttpMimeType::doc		,"application/msword",						},
		{EHttpMimeType::gz		,"application/gzip",						},
		{EHttpMimeType::gif		,"image/gif",								},
		{EHttpMimeType::jpg		,"image/jpeg",								},
		{EHttpMimeType::js		,"text/javascript",							},
		{EHttpMimeType::json	,"application/json",						},
		{EHttpMimeType::mp3		,"audio/mpeg",								},
		{EHttpMimeType::mpkg	,"application/vnd.apple.installer+xml",		},
		{EHttpMimeType::png		,"image/png",								},
		{EHttpMimeType::pdf		,"application/pdf",							},
		{EHttpMimeType::php		,"application/x-httpd-php",					},
		{EHttpMimeType::rar		,"application/vnd.rar",						},
		{EHttpMimeType::tar		,"application/x-tar",						},
		{EHttpMimeType::xml		,"application/xml",							},
		{EHttpMimeType::zip		,"application/zip",							},
		{EHttpMimeType::_7z		,"application/x-7z-compressed",				}
	};

	const auto* const MimeType = Enum.Find(Type);
	
	return MimeType ? *MimeType : TEXT("INVALID_MIME_TYPE");
}

FString UBlueprintHttpLibrary::UrlEncodeString(const FString& StringToEscape)
{
	return FPlatformHttp::UrlEncode(StringToEscape);
}

FString UBlueprintHttpLibrary::UrlDecodeString(const FString& StringToDecode)
{
	return FPlatformHttp::UrlDecode(StringToDecode);
}

bool UBlueprintHttpLibrary::IsUrlEncoded(const TArray<uint8> Payload)
{
	return FPlatformHttp::IsURLEncoded(Payload);
}

FString UBlueprintHttpLibrary::GetUrlDomain(const FString& Url)
{
	return FPlatformHttp::GetUrlDomain(Url);
}

FString UBlueprintHttpLibrary::HtmlEncodeString(const FString& StringToEncode)
{
	return FPlatformHttp::HtmlEncode(StringToEncode);
}

FString UBlueprintHttpLibrary::GetOperatingSystemProxyAddress()
{
	return FPlatformHttp::GetOperatingSystemProxyAddress().Get(TEXT(""));
}

FString UBlueprintHttpLibrary::GetDefaultUserAgent()
{
	return FPlatformHttp::GetDefaultUserAgent();
}

FString UBlueprintHttpLibrary::EscapeUserAgentString(const FString& RawUserAgent)
{
	return FPlatformHttp::EscapeUserAgentString(RawUserAgent);
}

FString UBlueprintHttpLibrary::GetFileMimeType(const FString& FilePath)
{
	return FPlatformHttp::GetMimeType(FilePath);
}

void UBlueprintHttpLibrary::EncodeToBase64(const FString& Data, FString& OutData)
{
	OutData = FBase64::Encode(Data);
}

void UBlueprintHttpLibrary::EncodeToBase64Binary(const TArray<uint8>& Data, FString& OutData)
{
	OutData = FBase64::Encode(Data);
}

bool UBlueprintHttpLibrary::DecodeToBase64(const FString& Data, FString& OutData)
{
	return FBase64::Decode(Data, OutData);
}

bool UBlueprintHttpLibrary::DecodeToBase64Binary(const FString& Data, TArray<uint8>& OutData)
{
	return FBase64::Decode(Data, OutData);
}

