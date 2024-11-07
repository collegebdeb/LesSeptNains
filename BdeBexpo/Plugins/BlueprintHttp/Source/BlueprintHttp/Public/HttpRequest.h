// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequest.generated.h"

class IHttpRequest;
class IHttpResponse;
class UHttpRequest;
class UHttpResponse;

/**
 *  A non hexaustive list of common MIME-Types to use for Content-Type. 
 **/
UENUM(BlueprintType)
enum class EHttpMimeType : uint8
{
	url		UMETA(DisplayName="application/x-www-form-urlencoded",		ToolTip="Data encoded in URL"),
	form	UMETA(DisplayName="multipart/form-data",					ToolTip="Form data"),
	txt		UMETA(DisplayName="text/plain",								ToolTip="Text"),
	html	UMETA(DisplayName="text/html",								ToolTip="HyperText Markup Language (HTML)"),
	aac		UMETA(DisplayName="audio/aac",								ToolTip="AAC audio"),
	arc		UMETA(DisplayName="application/x-freearc",					ToolTip="Archive document"),
	avi		UMETA(DisplayName="video/x-msvideo",						ToolTip="Audio Video Interleave"),
	bin		UMETA(DisplayName="application/octet-stream",				ToolTip="Any kind of binary data"),
	bmp		UMETA(DisplayName="image/bmp",								ToolTip="Windows OS/2 Bitmap Graphics"),
	css		UMETA(DisplayName="text/css",								ToolTip="Cascading Style Sheets"),
	csv		UMETA(DisplayName="text/csv",								ToolTip="Comma-separated values"),
	doc		UMETA(DisplayName="application/msword",						ToolTip="Microsoft Word"),
	gz		UMETA(DisplayName="application/gzip",						ToolTip="GZip Compressed Archive"),
	gif		UMETA(DisplayName="image/gif",								ToolTip="Graphics Interchange Format (GIF)"),
	jpg		UMETA(DisplayName="image/jpeg",								ToolTip="JPEG images"),
	js		UMETA(DisplayName="text/javascript",						ToolTip="JavaScript"),
	json    UMETA(DisplayName="application/json",						ToolTip="JSON format"),
	mp3		UMETA(DisplayName="audio/mpeg",								ToolTip="MP3 audio"),
	mpkg	UMETA(DisplayName="application/vnd.apple.installer+xml",	ToolTip="Apple Installer Package"),
	png		UMETA(DisplayName="image/png",								ToolTip="Portable Network Graphics"),
	pdf		UMETA(DisplayName="application/pdf",						ToolTip="Adobe Portable Document Format (PDF)"),
	php		UMETA(DisplayName="application/x-httpd-php",				ToolTip="Hypertext Preprocessor (PHP)"),
	rar		UMETA(DisplayName="application/vnd.rar",					ToolTip="RAR archive"),
	tar		UMETA(DisplayName="application/x-tar",						ToolTip="Tape Archive (TAR)"),
	xml		UMETA(DisplayName="application/xml",						ToolTip="XML"),
	zip		UMETA(DisplayName="application/zip",						ToolTip="ZIP archive"),
	_7z		UMETA(DisplayName="application/x-7z-compressed",			ToolTip="7-zip archive")
};

/**
 *	A request verb to tell the server the action we want to perform
 **/
UENUM(BlueprintType)
enum class EHttpVerb : uint8
{
	GET		UMETA(ToolTip="The GET method requests a representation of the specified resource. Requests using GET should only retrieve data."),
	POST	UMETA(ToolTip="The POST method is used to submit an entity to the specified resource, often causing a change in state or side effects on the server."),
	PUT		UMETA(ToolTip="The PUT method replaces all current representations of the target resource with the request payload."),
	PATCH	UMETA(ToolTip="The PATCH method is used to apply partial modifications to a resource."),
	HEAD	UMETA(ToolTip="The HEAD method asks for a response identical to that of a GET request, but without the response body."),
	DELETE	UMETA(ToolTip="The DELETE method deletes the specified resource."),
	// NOT SUPPORTED BY CURL IMPLEMENTATION: CONNECT	UMETA(ToolTip="The CONNECT method establishes a tunnel to the server identified by the target resource."),
	// NOT SUPPORTED BY CURL IMPLEMENTATION: OPTIONS	UMETA(ToolTip="The OPTIONS method is used to describe the communication options for the target resource."),
	// NOT SUPPORTED BY CURL IMPLEMENTATION: TRACE		UMETA(ToolTip="The TRACE method performs a message loop-back test along the path to the target resource."),
	
	MAX_COUNT UMETA(Hidden)
};

/**
 *	Status of a request
 **/
UENUM(BlueprintType, DisplayName = "HTTP Request Status")
enum class EBlueprintHttpRequestStatus : uint8
{
	NotStarted				UMETA(DisplayName="Not Started",				ToolTip = "Has not been started via ProcessRequest()."),
	Processing				UMETA(DisplayName="Processing",					ToolTip = "Currently being ticked and processed."),
	Failed					UMETA(DisplayName="Failed",						ToolTip = "Finished but failed."),
	Failed_ConnectionError	UMETA(DisplayName="Failed: Connection Error",	ToolTip = "Failed because it was unable to connect (safe to retry)."),
	Succeeded				UMETA(DisplayName="Succeeded",					ToolTip = "Finished and was successful.")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRequestComplete,       UHttpRequest*const, Request, UHttpResponse*const, Response,   const bool,     bConnectedSuccessfully);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRequestProgress,       UHttpRequest*const, Request, const int32,         BytesSent,  const int32,    BytesReceived);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRequestHeaderReceived, UHttpRequest*const, Request, const FString&,      HeaderName, const FString&, NewHeaderValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRequestWillRetry,		 UHttpRequest*const, Request, UHttpResponse*const, Response,   const float,    SecondsToRetry);

/**
 *  Wrapper class around native HTTP request pointer.
 **/
UCLASS(BlueprintType)
class BLUEPRINTHTTP_API UHttpRequest : public UObject
{
	GENERATED_BODY()
public:
	/* Don't use it but use UHttpRequest::CreateRequest() instead. */
	UHttpRequest();

	/* Creates an HTTP request. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	static UPARAM(DisplayName = "Request") UHttpRequest* CreateRequest();

	/* Set the specified verb for the request. */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetVerb(const EHttpVerb InVerb);

private: 
	/* Allows to add a custom verb to the request if the verb wanted is not in EHttpVerb. */
	//UFUNCTION(BlueprintCallable, Category = HTTP)
	// Setting an invalid verb fail an assertion on windows (CURL) only.
	// For this reason this method is made private to prevent its use as all
	// the supported verbs are in the enum.
	// The Interface provided by the Engine doesn't allow us to edit CURL custom verb.
	void SetCustomVerb(const FString& Verb);

public:
	/* Add the pair to the header. */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetHeader(const FString& Key, const FString& Value);

	/* Add a group of headers to the existing headers. */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetHeaders(const TMap<FString, FString> &Headers);

	/* Add the pair to the end of the header. */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void AppendToHeader(const FString& Key, const FString& Value);

	/* Set this request's URL */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetURL(const FString& Url);
	
	/**
	 *	Sets the Content-Type property with the specified Mime-Type.
	 *  If the wanted MimeType is not in the enum, you can still use your
	 *  own type by adding a header with the key Content-Type and your MimeType
	 *  as value.
	 **/
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetMimeType(const EHttpMimeType MimeType);
	
	/**
	 *	Sets the Content-Type property with the specified Mime-Type.
	 *  If the wanted MimeType is not in the enum, you can still use your
	 *  own type by adding a header with the key Content-Type and your MimeType
	 *  as value.
	 **/
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetMimeTypeAsString(const FString & MimeType);

	/* Set this request's content as binary data */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetContent(const TArray<uint8>& Content);

	/* Set this request's content as a FString */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetContentAsString(const FString& Content);

	/**
	 * Sets the content of the request to stream from a file.
	 * @param FileName - filename from which to stream the body.
	 * @return True if the file is valid and will be used to stream the request. False otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void SetContentAsStreamedFile(const FString& FileName, bool & bFileValid);

	/* Returns a map of paired headers. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Headers") TMap<FString, FString> GetAllHeaders() const;

	/* Returns the request's content as binary data. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	void GetContent(TArray<uint8>& OutContent) const;

	/* Returns the request's content as a FString. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Content") FString GetContentAsString() const;

	/* Returns the Content-Length from header if available or zero. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Length") int32 GetContentLength() const;

	/* Returns the Content-Type from header if available. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Type") FString GetContentType() const;

	/* Returns the time it took the server to fully respond to the request. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Time") float GetElapsedTime() const;

	/* Returns this header's key. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Header") FString GetHeader(const FString& Key) const;

	/* Returns the status  */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Status") EBlueprintHttpRequestStatus GetStatus() const;

	/* Returns the URL used to send the request. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "URL") FString GetURL() const;

	/* Returns the value of the specified parameter in the URL. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Parameter") FString GetURLParameter(const FString& ParameterName) const;

	/* Returns the verb used by this request. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Verb") FString GetVerb() const;

	/**
	 * Called to begin processing the request.
	 * OnProcessRequestComplete delegate is always called when the request completes or on error if it is bound.
	 * A request can be re-used but not while still being processed.
	 * @return if the request was successfully started.
	 */
	UFUNCTION(BlueprintCallable, Category = HTTP, meta = (Keywords = "send process HTTP request"))
	UPARAM(DisplayName = "Has Started") bool ProcessRequest();

	/* Cancel the request that is still being processed. */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void CancelRequest();

	/**
	 * Delegate called when the request is completed.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = HTTP)
	FOnRequestComplete OnRequestComplete;

	/** Delegate that will get called once per tick with bytes downloaded so far */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = HTTP)
	FOnRequestProgress OnRequestProgress;

	/**
	 * Called when the header has been received.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = HTTP)
	FOnRequestHeaderReceived OnRequestHeaderReceived;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = HTTP)
	FOnRequestWillRetry OnRequestWillRetry;

private:
	void OnRequestCompleteInternal (TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> RawResponse, bool bConnectedSuccessfully);
	void OnRequestProgressInternal (TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, const int32 BytesSent, const int32 BytesReceived);
	void OnHeaderReceivedInternal  (TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, const FString& HeaderName, const FString& HeaderValue);
	void OnRequestWillRetryInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> RawResponse, float SecondsToRetry);

	UHttpResponse* CreateResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> & RawRequest, TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> & RawResponse);

	FString ConvertEnumVerbToString(const EHttpVerb InVerb);

	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request;

};
