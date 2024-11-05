// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpResponse.generated.h"

class IHttpResponse;
class IHttpRequest;

/**
 *
 **/
UCLASS()
class BLUEPRINTHTTP_API UHttpResponse : public UObject
{
	GENERATED_BODY()

private:
	friend class UHttpRequest;

public:

	UHttpResponse();

	/* Returns a map of paired headers. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Headers") TMap<FString, FString> GetAllHeaders() const;

	/* Returns the request's content as binary data. */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	void GetContent(TArray<uint8>& OutContent) const;

	/* Returns the request's content as FString. */
	UFUNCTION(BlueprintCallable, Category = HTTP)
	UPARAM(DisplayName = "Content") FString GetContentAsString() const;

	/* Returns the Content-Length from header if available or zero. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Length") int32 GetContentLength() const;

	/* Returns the Content-Type from header if available. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Type") FString GetContentType() const;

	/* Returns this header's key. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Header") FString GetHeader(const FString& Key) const;

	/* Returns the status  */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Code") int32 GetResponseCode() const;

	/* Returns the URL used to send the request. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "URL") FString GetURL() const;

	/* Returns the value of the specified parameter in the URL. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Parameter") FString GetURLParameter(const FString& ParameterName) const;

	/* Returns the time it took the server to fully respond to the request. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
	UPARAM(DisplayName = "Elapsed Time") float GetElapsedTime() const;

private:
	// Can't use RAII with UObject.
	// Because of this workaround, Response can be nullptr.
	void InitInternal(TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> InResponse, const float &InRequestDuration);

	float RequestDuration;

	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> Response;

};
