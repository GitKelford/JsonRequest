// Copyright ©2024 Ilya Polyakov All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ReadWrithJsonToDataTable.generated.h"

class FJsonObject;

USTRUCT(BlueprintType)
struct FJsonStruct
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Json | Test Task")
		FString Title;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Json | Test Task")
		FString Price_1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Json | Test Task")
		FString ImageURL;
	
};

UCLASS()
class JSONREQUEST_API UReadWrithJsonToDataTable : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
	
public:

	/**
	* This function work only in Editor!
	* Don't use for package build.
	*
	* Read a series of struct based on a json file formatted like a data table
	*
	*@param FilePath            The path of the source file: "<project_dir>/Json/MyJson.json"
	*@param bOutSuccess         If the action was a success or not
	*@param OutInfoMassage      More information about the action's result
	*
	*@return The struct
	*/
	UFUNCTION(BlueprintCallable, Category="Json | Test Task")
	static FString ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMassage);
	
	/**
	 * This function work only in Editor!
	 * Don't use for package build.
	 *
	 * Read a series of struct based on a json file formatted like a data table
	 *
	 *@param FilePath            The path of the source file: "<project_dir>/Json/MyJson.json"
	 *@param bOutSuccess         If the action was a success or not
	 *@param OutInfoMassage      More information about the action's result
	 *@param OutCatalogTitle     Catalog Title form JSON Data Table
	 *
	 *@return The struct
	 */
	UFUNCTION(BlueprintCallable, Category="Json | Test Task")
		static TMap<FString, FJsonStruct> ReadStructFromJsonFile_DataTableFormat(FString FilePath, bool& bOutSuccess, FString& OutInfoMassage, FString& OutCatalogTitle);
	
	FString TitleCatalog;
};
