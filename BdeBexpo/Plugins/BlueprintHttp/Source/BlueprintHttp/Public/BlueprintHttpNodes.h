// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "HttpRequest.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BlueprintHttpNodes.generated.h"

/* The comma in TMap<FString, FString> breaks the delegate definition. */
USTRUCT(BlueprintType)
struct FHeaders
{
    GENERATED_BODY()
public:
    FHeaders() {};
    FHeaders(const TMap<FString, FString>& InHeaders) : Headers(InHeaders) {};
    FHeaders(TMap<FString, FString>&& InHeaders) : Headers(MoveTemp(InHeaders)) {};

    /* Wrapper to have a TMap as a delegate parameter as they aren't supported. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Headers)
    TMap<FString, FString> Headers;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFileDownloadedEvent, const int32, TotalSizeInBytes, const int32, TotalBytesReceived, const float, PercentDownloaded);

UCLASS()
class UHttpDownloadFileProxy final : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:

    UPROPERTY(BlueprintAssignable)
    FOnFileDownloadedEvent OnFileDownloaded;
    
    UPROPERTY(BlueprintAssignable)
    FOnFileDownloadedEvent OnFileDownloadError;

    UPROPERTY(BlueprintAssignable)
    FOnFileDownloadedEvent OnDownloadProgress;

public:
    virtual void Activate();

    /**
     * Download a file through an HTTP request.
     * @param FileUrl           The URL of the file we want to download.
     * @param UrlParameters     The parameters of the URL.
     * @param Verb              The verb used for the request.
     * @param MimeType          The Request content mime-type.
     * @param Content           The request's content.
     * @param Headers           The request's headers.
     * @param SaveFileLocation  Where we want to save the download.
    */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Headers, UrlParameters", DisplayName = "Download File through HTTP"))
    static UHttpDownloadFileProxy* HttpDownloadFile(const FString& FileUrl, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const FString& Content, const TMap<FString, FString>& Headers, const FString& SaveFileLocation);

private:
    UFUNCTION()
    void OnRequestCompleted (UHttpRequest* const Request, UHttpResponse* const Response, const bool bConnectedSuccessfully);
    UFUNCTION()
    void OnRequestTick      (UHttpRequest* const Request, const int32 BytesSent,        const int32 BytesReceived);
    UFUNCTION()
    void OnHeadersReceived  (UHttpRequest* const Request, const FString& HeaderName,     const FString& NewHeaderValue);

    FORCEINLINE float GetPercents() const
    {
        return ContentLength != 0 ? (Downloaded * 100.f / ContentLength) : 0.f;
    }

private:
    UPROPERTY()
    UHttpRequest* Request;

    int32 ContentLength;
    int32 Downloaded;

    FString SaveLocation;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FOnRequestEvent,       const int32, ResponseCode, const FHeaders&, Headers, const FString&, ContentType, const FString&,       Content, const float, TimeElapsed, const EBlueprintHttpRequestStatus, ConnectionStatus, const int32, BytesSent, const int32, BytesReceived);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FOnBinaryRequestEvent, const int32, ResponseCode, const FHeaders&, Headers, const FString&, ContentType, const TArray<uint8>&, Content, const float, TimeElapsed, const EBlueprintHttpRequestStatus, ConnectionStatus, const int32, BytesSent, const int32, BytesReceived);

/* Process a request and show delegates as execution pins */
UCLASS()
class UProcessHttpRequestProxy final : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:
    UProcessHttpRequestProxy(const FObjectInitializer& ObjectInitializer);

    virtual void Activate() override {};

    /* Called when a successful response is sent from the server. */
    UPROPERTY(BlueprintAssignable)
    FOnRequestEvent OnResponse;

    /* Called when an error occured. */
    UPROPERTY(BlueprintAssignable)
    FOnRequestEvent OnError;

    /* Called when this request ticks and has downloaded or uploaded a part of the data. */
    UPROPERTY(BlueprintAssignable)
    FOnRequestEvent OnTick;

    /* Process the request with the parameters provided earlier. */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (BlueprintInternalUseOnly = "true", DisplayName="Send Initialized Http Request"))
    static UProcessHttpRequestProxy* InlineProcessRequest(UHttpRequest* const Request);

private:
    UFUNCTION()
    void OnCompleteInternal(UHttpRequest* const Request, UHttpResponse* const Response, const bool bConnectedSuccessfully);
    UFUNCTION()
    void OnTickInternal(UHttpRequest* const Request, const int32 InBytesSent, const int32 InBytesReceived);

    UPROPERTY()
    UHttpRequest* RequestWrapper;

    int32 BytesSent;
    int32 BytesReceived;

};

/* Allows easy construction of asynchronous request based nodes for Blueprint use */
UCLASS(Abstract)
class USendHttpRequestProxyBase : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:
    USendHttpRequestProxyBase(const FObjectInitializer& ObjectInitializer);

    virtual void Activate() override {};

protected:
    /* Request Events */
    virtual void OnTickInternal   ()                              {};
    virtual void OnSuccessInternal(UHttpResponse* const Response) {};
    virtual void OnErrorInternal  (UHttpResponse* const Response) {};

    /* Request data */
    FORCEINLINE int32 GetBytesSent()     const { return BytesSent     ; }
    FORCEINLINE int32 GetBytesReceived() const { return BytesReceived ; }
    FORCEINLINE UHttpRequest* GetRequest()     { return RequestWrapper; }

    /* Send the request and handle failed launch. */
    void SendRequest();

private:
    UFUNCTION()
    void _OnCompleteInternal(UHttpRequest* const Request, UHttpResponse* const Response, const bool bConnectedSuccessfully);
    UFUNCTION()
    void _OnTickInternal(UHttpRequest* const Request, const int32 InBytesSent, const int32 InBytesReceived);

    UPROPERTY()
    UHttpRequest* RequestWrapper;

    int32 BytesSent;
    int32 BytesReceived;

};

/**
 *   Custom Node to easily send an HTTP(s) request to a server.
 */
UCLASS()
class USendHttpRequestProxy final : public USendHttpRequestProxyBase
{
    GENERATED_BODY()

public:

    /* Called when a successful response is sent from the server. */
    UPROPERTY(BlueprintAssignable)
    FOnRequestEvent OnResponse;

    /* Called when an error occured. */
    UPROPERTY(BlueprintAssignable)
    FOnRequestEvent OnError;

    /* Called when this request ticks and has downloaded or uploaded a part of the data. */
    UPROPERTY(BlueprintAssignable)
    FOnRequestEvent OnTick;

    /** 
     *   Send an Http Request to the specified URL.
     *   @param ServerUrl      The server we want to contact.
     *   @param UrlParameters  The optional unescaped parameters to add to the URL.
     *   @param Verb           The verb we want to use for this request. (GET, HEAD, POST, ...)
     *   @param Content        This request's content.
     *   @param Headers        This request's headers.
     **/
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Headers, UrlParameters"),  Category = HTTP)
    static USendHttpRequestProxy* SendHttpRequest(const FString & ServerUrl, const TMap<FString, FString> & UrlParameters, const EHttpVerb Verb, 
        const EHttpMimeType MimeType, const FString& Content, const TMap<FString, FString>& Headers);

protected:
    virtual void OnTickInternal();
    virtual void OnErrorInternal(UHttpResponse* const Response);
    virtual void OnSuccessInternal(UHttpResponse* const Response);
};



/**
 *   Custom Node to easily send an HTTP(s) request with binary data to a server.
 */
UCLASS()
class USendBinaryHttpRequestProxy final : public USendHttpRequestProxyBase
{
    GENERATED_BODY()

public:

    /* Called when a successful response is sent from the server. */
    UPROPERTY(BlueprintAssignable)
    FOnBinaryRequestEvent OnResponse;

    /* Called when an error occured. */
    UPROPERTY(BlueprintAssignable)
    FOnBinaryRequestEvent OnError;

    /* Called when this request ticks and has downloaded or uploaded a part of the data. */
    UPROPERTY(BlueprintAssignable)
    FOnBinaryRequestEvent OnTick;

    /**
     *   Send an Http Request to the specified URL.
     *   @param ServerUrl      The server we want to contact.
     *   @param UrlParameters  The optional unescaped parameters to add to the URL.
     *   @param Verb           The verb we want to use for this request. (GET, HEAD, POST, ...)
     *   @param Content        This request's content.
     *   @param Headers        This request's headers.
     **/
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Headers, UrlParameters, Content"), Category = HTTP)
    static USendBinaryHttpRequestProxy* SendBinaryHttpRequest(const FString& ServerUrl, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const TArray<uint8>& Content, const TMap<FString, FString>& Headers);

protected:
    virtual void OnTickInternal() override;
    virtual void OnErrorInternal(UHttpResponse* const Response) override;
    virtual void OnSuccessInternal(UHttpResponse* const Response) override;

};
