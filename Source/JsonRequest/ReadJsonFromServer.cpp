// Copyright ©2024 Ilya Polyakov All rights reserved.


#include "ReadJsonFromServer.h"

#include "JsonObjectConverter.h"

void UReadJsonFromServer::Login(const FString& Login, const FString& Password)
{
	
	UE_LOG(LogTemp, Warning, TEXT("Starting authorization process..."));
	UE_LOG(LogTemp, Warning, TEXT("Login: %s"), *Login);
	UE_LOG(LogTemp, Warning, TEXT("Password: %s"), *Password);

	// Http Request
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// POST Method
	HttpRequest->SetVerb(TEXT("POST"));

	// URL For Authorization
	HttpRequest->SetURL(TEXT("https://stanzza-api.aicrobotics.ru/api/auth/login"));
	
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Create JSON Object
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("login"), Login);
	JsonObject->SetStringField(TEXT("password"), Password);
	JsonObject->SetStringField(TEXT("fingerprint"), TEXT("UnrealEditor")); // Fingerprint = UnrealEditor

	// JSON to String
	FString AuthJsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&AuthJsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// Call AuthRequest
	SendRequest(HttpRequest, AuthJsonString);
}

void UReadJsonFromServer::SendRequest(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest, const FString& Content)
{

	HttpRequest->SetContentAsString(Content);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UReadJsonFromServer::OnRequestComplete);

	HttpRequest->ProcessRequest();
}

void UReadJsonFromServer::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
{

	if (bSuccess && HttpResponse.IsValid())
	{
		FString ResponseData = HttpResponse->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Received response data: %s"), *ResponseData);
		
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseData);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			FString AccessToken = JsonObject->GetStringField("access_token");
			FString RefreshToken = JsonObject->GetStringField("refresh_token");
			FString ExpiresAt = JsonObject->GetStringField("expired_at");
			
			UE_LOG(LogTemp, Warning, TEXT("Authorization successful!"));
			UE_LOG(LogTemp, Warning, TEXT("Access Token: %s"), *AccessToken);
			UE_LOG(LogTemp, Warning, TEXT("Refresh Token: %s"), *RefreshToken);
			UE_LOG(LogTemp, Warning, TEXT("Expires At: %s"), *ExpiresAt);

			GetData(AccessToken);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON response"));
			HandleError("Failed to deserialize JSON response");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP request failed"));
		HandleError("HTTP request failed");
	}
}

void UReadJsonFromServer::HandleError(const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Error: %s"), *ErrorMessage);
}

void UReadJsonFromServer::GetData(const FString& AccessToken)
{
	UE_LOG(LogTemp, Warning, TEXT("Getting Data..."));
	
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	// GET Method
	HttpRequest->SetVerb(TEXT("GET"));

	// Call to URL
	FString URL = TEXT("https://stanzza-api.aicrobotics.ru/api/v1/catalog/06-02-013");
	HttpRequest->SetURL(URL);
	
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));
	
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UReadJsonFromServer::OnGetRequestComplete);
	
	HttpRequest->ProcessRequest();
}

void UReadJsonFromServer::OnGetRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
{

	if (bSuccess && HttpResponse.IsValid())
	{
		FString ResponseData = HttpResponse->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Received response data: %s"), *ResponseData);
		
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseData);
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// Serialize to JSON
			FString JsonString;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
			
			UE_LOG(LogTemp, Warning, TEXT("Data retrieved successfully!"));

			WriteDataToFile(JsonString);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON response"));
			HandleError("Failed to deserialize JSON response");
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP request failed"));
		HandleError("HTTP request failed");
	}
}

void UReadJsonFromServer::WriteDataToFile(const FString& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Write Data to .txt file"));

	FString FilePath = FPaths::ProjectDir() + TEXT("MyJson.json");
	
	if (FFileHelper::SaveStringToFile(Data, *FilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Data saved to file: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save data to file: %s"), *FilePath);
	}
}
