// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "NiagaraDataInterfaceExport.h"                                         //*grants access to niagara array structure variable types
#include <mutex>
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AggM_NiagaraNaniteConvertBPLibrary.generated.h"


/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*/

USTRUCT(BlueprintType)
struct FNiagaraExportData                                                       //*StructureArrayDataType That is Returned to Blueprint, array of transforms, to be divided by meshindex 
{
    GENERATED_BODY()                                                            //Required body of the var

        //Defines the one Property (variable/data) we store in this structure
        //EditAnywhere and BlueprintReadWrite exposes the Variable as full access to blueprints
        //UI Sorting Category for the Editor of "Niagara Export"
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara Export")
        TArray<FTransform> Positions;                                           //Defines the Data as a T-Array or List of F-Transforms named "Positions" 
};


UCLASS()
class UAggM_NiagaraNaniteConvertBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

//First default example function
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "AggM_NiagaraNaniteConvert sample test testing"), Category = "AggM_NiagaraNaniteConvertTesting")
	static float AggM_NiagaraNaniteConvertSampleFunction(float Param);

    UFUNCTION(BlueprintCallable, Category = "GPU Export")                                                       //Defined as a Function, blueprint accessible and with the UI List sorting group of "GPU Export")
        //Defines our call, by first setting what Return data will be sent as. a T-Array or list of FNiagaraExportData Structures defined above.
        //GPUExportCPP is the name of our call/function and here it is defined. This must match the CPP Body definition
        //Within the () are our inputs a T-Array or list of F-BasicParticleData which we have named as a variable "NiagaraExportData"
        //FBasicParticleData is a custom data type exported from Niagara Systems as an Array/struct of 2 Vector3s and a Float.
        //We gain access to that existing structure definition by including the NiagaraDataInterfaceExport.h file and adding Niagara to our Project Build.cs file:
        //" Navigate to your project's build.cs file in ...\ProjectName\Source\ProjectName\ProjectName.build.cs and add "Niagara" to PublicDependencyModuleNames:"
        ///  Example:   PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Niagara" });
        static TArray<FNiagaraExportData> GPUExportCPP(const TArray<FBasicParticleData>& NiagaraExportData);

    UFUNCTION(BlueprintCallable, Category = "GPU Export")                                                       //Defined as a Function, blueprint accessible and with the UI List sorting group of "GPU Export")
        //Defines our call, by first setting what Return data will be sent as. a T-Array or list of FNiagaraExportData Structures defined above.
        //GPUExportCPP is the name of our call/function and here it is defined. This must match the CPP Body definition
        //Within the () are our inputs a T-Array or list of F-BasicParticleData which we have named as a variable "NiagaraExportData"
        //FBasicParticleData is a custom data type exported from Niagara Systems as an Array/struct of 2 Vector3s and a Float.
        //We gain access to that existing structure definition by including the NiagaraDataInterfaceExport.h file and adding Niagara to our Project Build.cs file:
        //" Navigate to your project's build.cs file in ...\ProjectName\Source\ProjectName\ProjectName.build.cs and add "Niagara" to PublicDependencyModuleNames:"
        ///  Example:   PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Niagara" });
        static TArray<FNiagaraExportData> GPUExportCPP_Parallel(const TArray<FBasicParticleData>& NiagaraExportData);

};
