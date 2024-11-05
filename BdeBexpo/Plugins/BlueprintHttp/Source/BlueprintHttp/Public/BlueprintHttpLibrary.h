// Copyright Pandores Marketplace 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpResponseCode.h"
#include "HttpRequest.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintHttpLibrary.generated.h"

/**
 *  C++ Library containing usefull HTTP helpers for Blueprint.
 **/
UCLASS()
class BLUEPRINTHTTP_API UBlueprintHttpLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

    /**
     *  Try to convert the string to the EHttpVerb enum.
     *  If the string is invalid, it returns GET by default.
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HTTP")
    static EHttpVerb StringToVerb(const FString& Verb);

    /**
     * Gets the allowed domains for HTTP calls.
     * An empty array means all domains are allowed.
     **/
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Get Allowed Domains"))
    void HttpGlobal_GetAllowedDomains(TArray<FString> & AllowedDomains);

    /* Gets the max connections allowed per server */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Get HTTP Max Connections per Server"))
    int32 HttpGlobal_GetHttpMaxConnectionsPerServer();
    
    /* Gets the Proxy address used to contact servers. */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Get Proxy Address"))
    FString HttpGlobal_GetProxyAddress();
    
    /* Gets the max read buffer size for HTTP requests */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Get Max Read Buffer Size"))
    int32 HttpGlobal_GetMaxReadBufferSize();

    /**
     * Add a default header to be appended to future requests
     * If a request already specifies this header, then the defaulted version will not be used
     * @param HeaderName - Name of the header (e.g., "Content-Type")
     * @param HeaderValue - Value of the header
     */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Add Default Header"))
	static void HttpGlobal_AddDefaultHeader(const FString &HeaderName, const FString & HeaderValue);

    /**
     * Setter for the proxy address.
     * @param InProxyAddress - New proxy address to use.
     */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set Proxy Address"))
	static void HttpGlobal_SetProxyAddress(const FString &InProxyAddress);

    /**
     * Sets the max read buffer size for the requests.
     * @param SizeInBytes	The maximum number of bytes to use for the read buffer
     */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set Max Read Buffer Size"))
	static void HttpGlobal_SetMaxReadBufferSize(const int32 SizeInBytes);

    /* Sets timeout in seconds for the entire http request to complete */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set HTTP Timeout"))
	static void HttpGlobal_SetHttpTimeout(const float Timeout);

    /* Sets the mimimum tick rate of an idle HTTP thread */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set HTTP Thread Idle Minimum Sleep Time in Seconds"))
	static void HttpGlobal_SetHttpThreadIdleMinimumSleepTimeInSeconds(const float Time);

    /* Sets the target tick rate of an idle HTTP thread */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set HTTP Thread Idle Frame Time in Seconds"))
	static void HttpGlobal_SetHttpThreadIdleFrameTimeInSeconds(const float Time);

    /* Sets the mimimum tick rate of an active HTTP thread */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set HTTP Thread Active Minimum Sleep Time in Seconds"))
    static void HttpGlobal_SetHttpThreadActiveMinimumSleepTimeInSeconds(const float Time);

    /* Sets the target tick rate of an active HTTP thread */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set HTTP Thread Active Frame Time in Seconds"))
    static void HttpGlobal_SetHttpThreadActiveFrameTimeInSeconds(const float Time);

    /* Sets the min delay time for each http request */
    UFUNCTION(BlueprintCallable, Category = HTTP, meta = (DisplayName = "GLOBAL HTTP - Set HTTP Delay Time"))
    static void HttpGlobal_SetHttpDelayTime(const float Delay);

    /* Converts the response code to its official name code. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString HttpResponseCodeToString(const int32 ResponseCode);

    /**
     * Converts the response code to its use case. This is a text explaining
     * what this code actually means with an english sentence.
     * Available for loggin purpose. 
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString HttpResponseCodeToDescriptionString(const int32 ResponseCode);

    /**
     *  Helper for the creation of string MIME-Type. 
     *  @return MIME-Type The MIME type text associated with this type of files.
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString CreateMimeType(const EHttpMimeType Type);

    /**
     * Converts the response code to an enum to make easy comparisons. 
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static EHttpResponseCode HttpResponseCodeToEnum(const int32 ResponseCode);

    /**
     * Create a request and initialize it to the specified properties. 
     * You can override the MIME-Type with an header entry ('Content-Type')
     * if the wanted MIME-Type isn't defined in the enum.
     * @return The initialized request.
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (AutoCreateRefTerm = "Headers, UrlParameters"))
    static UHttpRequest* CreateInitializedRequest(const FString& Url, const TMap<FString, FString> & UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const FString& Content, const TMap<FString, FString>& Headers);

    /**
     * Create a request and initialize it to the specified properties. 
     * You can override the MIME-Type with an header entry ('Content-Type')
     * if the wanted MIME-Type isn't defined in the enum.
     * @return The initialized request.
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (AutoCreateRefTerm = "Headers, Content, UrlParameters"))
    static UHttpRequest* CreateInitializedBinaryRequest(const FString& Url, const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType, const TArray<uint8> & Content, const TMap<FString, FString>& Headers);

    /* Escape the Parameters and add them to the end of the URL*/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString AddParametersToUrl(FString InUrl, const TMap<FString, FString>& Parameters);

    /* Escape the following string to comply to URL specification. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "url escape encode string"))
    static FString UrlEncodeString(const FString& StringToEscape);

    /* Decode this URL encoded string. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "decode url parse string"))
    static FString UrlDecodeString(const FString& StringToDecode);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "url escaped encoded"))
    static bool IsUrlEncoded(const TArray<uint8> Payload);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString GetUrlDomain(const FString& Url);

    /* Encode this string to HTML entities. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "encode html parse entities"))
    static FString HtmlEncodeString(const FString& StringToEncode);

    /* Get the proxy address specified by the operating system. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString GetOperatingSystemProxyAddress();

    /**
     * Returns the default User-Agent string to use in HTTP requests.
     * Requests that explicitly set the User-Agent header will not use this value.
     * @return The User-Agent
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString GetDefaultUserAgent();

    /* Escape the specified User-Agent. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString EscapeUserAgentString(const FString & RawUserAgent);

    /**
     * Get the mime type for the file
     * @return the mime type for the file.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP)
    static FString GetFileMimeType(const FString & FilePath);
    
    /* Encodes a FString into a Base64 FString. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "base64 string encode", DisplayName = "Encode to Base64"))
    static void EncodeToBase64(const FString& Data, FString& OutData);
    
    /* Encodes a bynary array into a Base64 FString. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "base64 binary encode", DisplayName = "Encode to Base64 Binary"))
    static void EncodeToBase64Binary(const TArray<uint8>& Data, FString& OutData);

    /**
     * Decodes a FString into a Base64 FString. 
     * @return bSuccess If the decoding succeeded.
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "base64 string decode", DisplayName = "Decode from Base64"))
    static bool DecodeToBase64(const FString& Data, FString& OutData);
    
    /**
     * Decodes a bynary array into a Base64 FString. 
     * @return bSuccess If the decoding succeeded.
     **/
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = HTTP, meta = (Keywords = "base64 binary decode", DisplayName = "Decode from Base64 Binary"))
    static bool DecodeToBase64Binary(const FString& Data, TArray<uint8>& OutData);
};