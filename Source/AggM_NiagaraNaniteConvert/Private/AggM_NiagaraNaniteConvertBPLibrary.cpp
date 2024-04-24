// Copyright Epic Games, Inc. All Rights Reserved.

#include "AggM_NiagaraNaniteConvertBPLibrary.h"
#include "AggM_NiagaraNaniteConvert.h"


UAggM_NiagaraNaniteConvertBPLibrary::UAggM_NiagaraNaniteConvertBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

//First default example function called "SampleFunction"
float UAggM_NiagaraNaniteConvertBPLibrary::AggM_NiagaraNaniteConvertSampleFunction(float Param)
{
	return -1;
}

TArray<FNiagaraExportData> UAggM_NiagaraNaniteConvertBPLibrary::GPUExportCPP(const TArray<FBasicParticleData>& NiagaraExportData)
{
    TArray<FNiagaraExportData> GPUExportCPPReturnArray;                         //Creates the return list of transforms array by found meshindex
    
    //FTransform Transform;         //moved inside loop to clear each loop                                           //Creates this loop's variable to add to the transform array, 
    //int32 meshindex;          //moved inside loop to clear each loop                                                //Var for Mesh Index, only used and set when inside loop


    for (const FBasicParticleData& ExportData : NiagaraExportData)              // Loop through each index of NiagaraExportData
    {
        FTransform Transform;                                                       //Creates this loop's variable to add to the transform array, 
        int32 meshindex;                                                            //Var for Mesh Index, only used and set when inside loop

        // Convert position, rotation, and scale data into a transform
        Transform.SetLocation(ExportData.Position);                         //ez

        //Hardcode scale for now, pass in packed scale later
        Transform.SetScale3D(FVector(0.2, 0.2, 0.2));                       //ez set scale to 1         

        //packing code : inital w = 1.0. 1.0 / 1000 = .001 + 0.1 = .101 + (MeshIndex * 100 = 0) = 0 + 0.101 = 0.101 packed)
        //harder, adds the W from the fraction of the Scale (Size (0.101) - longSize(0) = .101 - 0.1f = .001 * 1000 = 1.0 W unpacked)
        Transform.SetRotation(FQuat(ExportData.Velocity.X, ExportData.Velocity.Y, ExportData.Velocity.Z, ((((ExportData.Size - (long)ExportData.Size)) - 0.1f) * 1000)));

        Transform.NormalizeRotation();                                      //Normalize math into rotation ranges (dont want to send 409 degrees afterall..)

        meshindex = ((long)((long)ExportData.Size / 1000));                 //Depack MeshIndex from Scale and store 

        // Add the transform to the return array ID(by mesh ID depacked from size)
        if (GPUExportCPPReturnArray.IsValidIndex(meshindex))                //Do we have this mesh index id in our array?
        {
            GPUExportCPPReturnArray[meshindex].Positions.Add(Transform);    //Adds Transform to MeshIndexArray
        }
        else {                                                              //If we do not have the meshindex already in the mesh index array, we will grow the array to fit it.
            GPUExportCPPReturnArray.SetNum(meshindex + 1);                  //Grows array if meshindex is outside of it
            GPUExportCPPReturnArray[meshindex].Positions.Add(Transform);    //Adds Transform to MeshIndexArray 
        }
    }
    return GPUExportCPPReturnArray;                                             //Return List of Transform Arrays by MeshIndex
}


//Below is an attempt at spitting out a Thread for EACH PARTICLE DATA, the problem is locking the return array, and returning the data. 

TArray<FNiagaraExportData> UAggM_NiagaraNaniteConvertBPLibrary::GPUExportCPP_Parallel(const TArray<FBasicParticleData>& NiagaraExportData)
{
    TArray<FNiagaraExportData> GPUExportCPPReturnArray;
    int32 MaxIndex = 0;

    for (const FBasicParticleData& ExportData : NiagaraExportData) //for each particle... 
    {
        int32 meshindex = (int32)(ExportData.Size / 1000); //int32 cuts fractions
        if (meshindex > MaxIndex)
        {
            MaxIndex = meshindex;
        }
    }
    GPUExportCPPReturnArray.SetNum(MaxIndex + 1);

    // Mutex 
    FCriticalSection Mutex;

    ParallelFor(NiagaraExportData.Num(), [&](int32 Index)
        {
            const FBasicParticleData& ExportData = NiagaraExportData[Index];
            FTransform Transform;

            Transform.SetLocation(ExportData.Position);
            Transform.SetScale3D(FVector(0.2f, 0.2f, 0.2f));
            Transform.SetRotation(FQuat(ExportData.Velocity.X, ExportData.Velocity.Y, ExportData.Velocity.Z, ((((ExportData.Size - (long)ExportData.Size)) - 0.1f) * 1000)));
            Transform.NormalizeRotation();

            int32 meshindex = (int32)(ExportData.Size / 1000);

            // Synchronize access to the shared array
            Mutex.Lock();
           // if (GPUExportCPPReturnArray.IsValidIndex(meshindex))
            //{
            //    GPUExportCPPReturnArray[meshindex].Positions.Add(Transform);
            //}
            //else
           // {
                GPUExportCPPReturnArray[meshindex].Positions.Add(Transform);
           // }
            Mutex.Unlock();
        });

    return GPUExportCPPReturnArray;
}
