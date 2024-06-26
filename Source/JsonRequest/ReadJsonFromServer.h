// Copyright ©2024 Ilya Polyakov All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "ReadJsonFromServer.generated.h"

UCLASS(Blueprintable)
class JSONREQUEST_API UReadJsonFromServer : public UUserWidget
{
	GENERATED_BODY()

public:

	/**
	* This function work only in Editor!
	* Don't use for package build.
	* More Info in UnrealEditor Log.
	*
	* Login and get Data form https://stanzza-api.aicrobotics.ru/api/auth/login
	* Using POST request for login
	* Using GET request for getData
	*
	* Saving data at .json file to path: "<project_dir>/json/MyJson.json"
	*
	*@param Login            email
	*@param Password         password
	*
	*/
	UFUNCTION(BlueprintCallable, Category="Json | Test Task")
	void Login(const FString& Login, const FString& Password);

	/*
	 * url for getting data https://stanzza-api.aicrobotics.ru/api/v1/catalog/06-02-013
	 */
	void GetData(const FString& AccessToken);

	UPROPERTY(BlueprintReadWrite, Category="Json | Test Task")
	FString GlobalFilePath = FPaths::ProjectDir() + TEXT("MyJson.json");

protected:

	void SendRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, const FString& Content);

	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess);

	void HandleError(const FString& ErrorMessage);

	void OnGetRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess);

	void WriteDataToFile(const FString& Data);
};

