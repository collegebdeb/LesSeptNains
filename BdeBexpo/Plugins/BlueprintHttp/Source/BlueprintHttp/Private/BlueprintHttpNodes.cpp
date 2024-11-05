// Copyright Pandores Marketplace 2024. All Rights Reserved.

#include "BlueprintHttpNodes.h"
#include "BlueprintHttpLibrary.h"
#include "HttpResponse.h"
#include "Http.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


UHttpDownloadFileProxy* UHttpDownloadFileProxy::HttpDownloadFile(const FString& FileUrl, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const FString& Content, const TMap<FString, FString>& Headers, const FString& SaveFileLocation)
{
    UHttpDownloadFileProxy* const Proxy = NewObject<UHttpDownloadFileProxy>();

    Proxy->ContentLength = 0;
    Proxy->Downloaded    = 0;

    Proxy->Request = UHttpRequest::CreateRequest();

    Proxy->Request->SetVerb           (Verb);
    Proxy->Request->SetHeaders        (Headers);
    Proxy->Request->SetURL            (UBlueprintHttpLibrary::AddParametersToUrl(FileUrl, UrlParameters));
    Proxy->Request->SetMimeType       (MimeType);
    Proxy->Request->SetContentAsString(Content);

    Proxy->SaveLocation = SaveFileLocation;

    return Proxy;
}

void UHttpDownloadFileProxy::Activate()
{
    Request->OnRequestComplete      .AddDynamic(this, &UHttpDownloadFileProxy::OnRequestCompleted);
    Request->OnRequestProgress      .AddDynamic(this, &UHttpDownloadFileProxy::OnRequestTick);
    Request->OnRequestHeaderReceived.AddDynamic(this, &UHttpDownloadFileProxy::OnHeadersReceived);

    if (!Request->ProcessRequest())
    {
        OnFileDownloadError.Broadcast(0, 0, 0.f);
        SetReadyToDestroy();
    }
}

void UHttpDownloadFileProxy::OnRequestCompleted(UHttpRequest* const Req, UHttpResponse* const Response, const bool bConnectedSuccessfully)
{
    if (!bConnectedSuccessfully)
    {
        OnFileDownloadError.Broadcast(ContentLength, Downloaded, GetPercents());
    }
    else if (Response && Response->GetResponseCode() < 400)
    {    
        TArray<uint8> Content;
        Response->GetContent(Content);
        if (!FFileHelper::SaveArrayToFile(Content, *SaveLocation))
        {
            UE_LOG(LogHttp, Error, TEXT("Download file error: Failed to save data to \"%s\"."), *FPaths::ConvertRelativePathToFull(SaveLocation));
            OnFileDownloadError.Broadcast(ContentLength, Downloaded, GetPercents());
        }
        else
        {
            OnFileDownloaded.Broadcast(ContentLength, Downloaded, GetPercents());
        }
    }
    else
    {
        UE_LOG(LogHttp, Error, TEXT("Download file error: Server responded with an invalid code: \"%d\"."), Response ? Response->GetResponseCode() : -1);
        OnFileDownloadError.Broadcast(ContentLength, Downloaded, GetPercents());
    }

    SetReadyToDestroy();
}

void UHttpDownloadFileProxy::OnRequestTick(UHttpRequest* const Req, const int32 BytesSent, const int32 BytesReceived)
{
    Downloaded = BytesReceived;

    OnDownloadProgress.Broadcast(ContentLength, Downloaded, GetPercents());
}

void UHttpDownloadFileProxy::OnHeadersReceived(UHttpRequest* const Req  , const FString& HeaderName, const FString& NewHeaderValue)
{
    if (HeaderName.Equals(TEXT("Content-Length"), ESearchCase::IgnoreCase))
    {
        ContentLength = FCString::Atoi(*NewHeaderValue);
    }
}

UProcessHttpRequestProxy::UProcessHttpRequestProxy(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , RequestWrapper(nullptr)
    , BytesSent(0)
    , BytesReceived(0)
{}


UProcessHttpRequestProxy* UProcessHttpRequestProxy::InlineProcessRequest(UHttpRequest* const Request)
{
    UProcessHttpRequestProxy* const Proxy = NewObject<UProcessHttpRequestProxy>();

    if (Request)
    {
        Proxy->RequestWrapper = Request;

    }
    else
    {
        Proxy->SetReadyToDestroy();
    }

    return Proxy;
}

void UProcessHttpRequestProxy::OnTickInternal(UHttpRequest* const Request, const int32 InBytesSent, const int32 InBytesReceived)
{
    OnResponse.Broadcast(100, FHeaders(), TEXT(""), TEXT(""), Request->GetElapsedTime(),
        Request->GetStatus(), BytesSent, BytesReceived);
}

void UProcessHttpRequestProxy::OnCompleteInternal(UHttpRequest* const Request, UHttpResponse* const Response, const bool bConnectedSuccessfully)
{
    if (bConnectedSuccessfully)
    {
        OnResponse.Broadcast(Response->GetResponseCode(), Response->GetAllHeaders(),
            Response->GetContentType(), Response->GetContentAsString(), Request->GetElapsedTime(),
            Request->GetStatus(), BytesSent, BytesReceived);
    }
    else
    {
        OnError.Broadcast(Response->GetResponseCode(), Response->GetAllHeaders(), 
            Response->GetContentType(), Response->GetContentAsString(), Request->GetElapsedTime(), 
            Request->GetStatus(), BytesSent, BytesReceived);
    }
}

USendHttpRequestProxyBase::USendHttpRequestProxyBase(const FObjectInitializer& ObjectInitializer)
    : Super()
    , RequestWrapper(NewObject<UHttpRequest>())
    , BytesSent(0)
    , BytesReceived(0)
{
    RequestWrapper->OnRequestProgress.AddDynamic(this, &USendHttpRequestProxyBase::_OnTickInternal);
    RequestWrapper->OnRequestComplete.AddDynamic(this, &USendHttpRequestProxyBase::_OnCompleteInternal);
}

void USendHttpRequestProxyBase::_OnTickInternal(UHttpRequest* const Request, const int32 InBytesSent, const int32 InBytesReceived)
{
    BytesSent       = InBytesSent;
    BytesReceived   = InBytesReceived;

    OnTickInternal();
}

void USendHttpRequestProxyBase::_OnCompleteInternal(UHttpRequest* const Request, UHttpResponse* const Response, const bool bConnectedSuccessfully)
{
    if (bConnectedSuccessfully)
    {
        OnSuccessInternal(Response);
    }
    else
    {
        OnErrorInternal(Response);
    }

    SetReadyToDestroy();
}

void USendHttpRequestProxyBase::SendRequest()
{
    if (!RequestWrapper->ProcessRequest())
    {
        OnErrorInternal(nullptr);
    }
}

USendHttpRequestProxy* USendHttpRequestProxy::SendHttpRequest(const FString& ServerUrl, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const FString& Content, const TMap<FString, FString>& Headers)
{
    USendHttpRequestProxy* const Proxy = NewObject<USendHttpRequestProxy>();

    UHttpRequest* const Request = Proxy->GetRequest();

    Request->SetURL(UBlueprintHttpLibrary::AddParametersToUrl(ServerUrl, UrlParameters));
    Request->SetMimeType(MimeType);
    Request->SetVerb(Verb);
    Request->SetContentAsString(Content);
    Request->SetHeaders(Headers);

    Proxy->SendRequest();

    return Proxy;
}

void USendHttpRequestProxy::OnTickInternal()
{
    OnTick.Broadcast(100, FHeaders(), FString(), FString(), 
        GetRequest()->GetElapsedTime(), GetRequest()->GetStatus(), 
        GetBytesSent(), GetBytesReceived());
}

void USendHttpRequestProxy::OnSuccessInternal(UHttpResponse* const Response)
{
    OnResponse.Broadcast(Response->GetResponseCode(), FHeaders(Response->GetAllHeaders()), 
        Response->GetContentType(), Response->GetContentAsString(), Response->GetElapsedTime(), 
        GetRequest()->GetStatus(), GetBytesSent(), GetBytesReceived());
    SetReadyToDestroy();
}

void USendHttpRequestProxy::OnErrorInternal(UHttpResponse* const Response)
{
    OnError.Broadcast(Response->GetResponseCode(), 
        FHeaders(Response->GetAllHeaders()), Response->GetContentType(), Response->GetContentAsString(), 
        Response->GetElapsedTime(), GetRequest()->GetStatus(), GetBytesSent(), GetBytesReceived());
    SetReadyToDestroy();
}

USendBinaryHttpRequestProxy* USendBinaryHttpRequestProxy::SendBinaryHttpRequest(const FString& ServerUrl, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const TArray<uint8>& Content, const TMap<FString, FString>& Headers)
{
    USendBinaryHttpRequestProxy* const Proxy = NewObject<USendBinaryHttpRequestProxy>();

    UHttpRequest* const Request = Proxy->GetRequest();

    Request->SetURL(UBlueprintHttpLibrary::AddParametersToUrl(ServerUrl, UrlParameters));
    Request->SetVerb(Verb);
    Request->SetContent(Content);
    Request->SetMimeType(MimeType);
    Request->SetHeaders(Headers);

    Proxy->SendRequest();

    return Proxy;
}


void USendBinaryHttpRequestProxy::OnTickInternal()
{
    OnTick.Broadcast(100, FHeaders(), FString(), TArray<uint8>(),
        GetRequest()->GetElapsedTime(), GetRequest()->GetStatus(),
        GetBytesSent(), GetBytesReceived());
}

void USendBinaryHttpRequestProxy::OnSuccessInternal(UHttpResponse* const Response)
{
    TArray<uint8> Content;
    Response->GetContent(Content);
    OnResponse.Broadcast(Response->GetResponseCode(), FHeaders(Response->GetAllHeaders()),
        Response->GetContentType(), Content, Response->GetElapsedTime(),
        GetRequest()->GetStatus(), GetBytesSent(), GetBytesReceived());
}

void USendBinaryHttpRequestProxy::OnErrorInternal(UHttpResponse* const Response)
{
    TArray<uint8> Content;
    Response->GetContent(Content);
    OnError.Broadcast(Response->GetResponseCode(),
        FHeaders(Response->GetAllHeaders()), Response->GetContentType(), Content,
        Response->GetElapsedTime(), GetRequest()->GetStatus(), GetBytesSent(), GetBytesReceived());
}

