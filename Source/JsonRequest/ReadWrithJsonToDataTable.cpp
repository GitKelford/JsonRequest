// Copyright ©2024 Ilya Polyakov All rights reserved.


#include "ReadWrithJsonToDataTable.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Containers/UnrealString.h"


FString UReadWrithJsonToDataTable::ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMassage)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		bOutSuccess = false;
		OutInfoMassage = FString::Printf(TEXT("Read String From File Failed - File don't exit - %s"), *FilePath);
		return "";
	}

	FString RetString = "";

	if (!FFileHelper::LoadFileToString(RetString, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMassage = FString::Printf(TEXT("Read String From File Failed - Was not able to read file. Is this a text file? - %s"), *FilePath);
		return "";
	}

	bOutSuccess = true;
	OutInfoMassage = FString::Printf(TEXT("Read String From File Successed - %s"), *FilePath);
	return RetString;
}

TMap<FString, FJsonStruct> UReadWrithJsonToDataTable::ReadStructFromJsonFile_DataTableFormat(FString FilePath, bool& bOutSuccess, FString& OutInfoMassage, FString& OutCatalogTitle)
{
    FString JsonString = UReadWrithJsonToDataTable::ReadStringFromFile(FilePath, bOutSuccess, OutInfoMassage);
    if (!bOutSuccess)
    {
        return TMap<FString, FJsonStruct>();
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        bOutSuccess = false;
        OutInfoMassage = FString::Printf(TEXT("Failed to deserialize JSON from file: %s"), *FilePath);
        return TMap<FString, FJsonStruct>();
    }
	
    if (!JsonObject->HasField("catalog"))
    {
        bOutSuccess = false;
        OutInfoMassage = FString::Printf(TEXT("JSON does not contain 'catalog' key"));
        return TMap<FString, FJsonStruct>();
    }
	
    TSharedPtr<FJsonObject> CatalogObject = JsonObject->GetObjectField("catalog");
	
    if (!CatalogObject->HasField("title"))
    {
        bOutSuccess = false;
        OutInfoMassage = FString::Printf(TEXT("'catalog' object does not contain 'title' field"));
        return TMap<FString, FJsonStruct>();
    }

    FString CatalogTitle = CatalogObject->GetStringField("title");
    OutCatalogTitle = CatalogTitle;
	
    const TArray<TSharedPtr<FJsonValue>>* ChildrenArray;
    if (!CatalogObject->TryGetArrayField("children", ChildrenArray))
    {
        bOutSuccess = false;
        OutInfoMassage = FString::Printf(TEXT("'catalog' object does not contain 'children' array"));
        return TMap<FString, FJsonStruct>();
    }
	
    if (ChildrenArray->Num() == 0)
    {
        bOutSuccess = false;
        OutInfoMassage = FString::Printf(TEXT("'catalog' object contains 'children' array but it is empty"));
        return TMap<FString, FJsonStruct>();
    }

    TMap<FString, FJsonStruct> RowsToStruct;


    for (const auto& ChildValue : *ChildrenArray)
    {
        TSharedPtr<FJsonObject> ChildObject = ChildValue->AsObject();
    	
        if (!ChildObject->HasField("title") || !ChildObject->HasField("price_1"))
        {
            continue; 
        }

        FString ChildTitle = ChildObject->GetStringField("title");
        FString ChildPrice = ChildObject->GetStringField("price_1");


        const TArray<TSharedPtr<FJsonValue>>* PhotoArray;
        if (!ChildObject->TryGetArrayField("photo", PhotoArray) || PhotoArray->Num() == 0)
        {
            continue; 
        }

        TSharedPtr<FJsonObject> PhotoObject = (*PhotoArray)[0]->AsObject();
        FString ImageURL = PhotoObject->GetStringField("preview");

        // Create FJsonStruct object
        FJsonStruct ChildStruct;
        ChildStruct.Title = ChildTitle;
        ChildStruct.Price_1 = ChildPrice;
        ChildStruct.ImageURL = ImageURL;
    	
        RowsToStruct.Add(FString::Printf(TEXT("%s - %s"), *CatalogTitle, *ChildTitle), ChildStruct);
    }

    bOutSuccess = true;
    OutInfoMassage = FString::Printf(TEXT("Successfully parsed JSON and extracted data"));
    return RowsToStruct;
}

