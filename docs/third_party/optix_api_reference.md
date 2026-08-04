## NVIDIA OptiX 9.1

API Reference Manual

18 November 2025
Version 9.1

---

## Table of Contents

**1 NVIDIA OptiX 9.1 API 1**
**2 Module Index 1**
2.1 Modules... .... .. ...... ................................ 1
**3 Class Index 1**
3.1 Class List.. ..... .. ...... ................................ 1
**4 File Index 5**
4.1 File List.. ... ........... ................................ 5
**5 Module Documentation6**
5.1 Device API... ... ........................................ 6
5.2 Cooperative Vector... ........ .............................. 70
5.3 Function Table... .... .. ...... ............................. 78
5.4 Host API... ... ......................................... 79
5.5 Error handling. ... ... ....... .............................. 79
5.6 Device context... ... ...................................... 80
5.7 Pipelines... ... ......................................... 85
5.8 Modules... .... .. ...... ................................ 88
5.9 Tasks. ... ................................ .. .. .. .. .. .. . 91
5.10 Program groups... ... ....... .............................. 94
5.11 Launches. ..... .. ................................ .. .. .. . 95
5.12 Acceleration structures. ... ................................... 96
5.13 Cooperative Vector... ........ .............................. 105
5.14 Denoiser. ... ........................................... 107
5.15 Utilities.. ... ........... ................................ 113
5.16 Types.. ... .............................. ...... .. .. .. .. 120
**6 Namespace Documentation176**
6.1 optix_impl Namespace Reference. . . . . . . . . . ..... .................. 176
6.2 optix_internal Namespace Reference. . . . . . . . ..... ................. . 180
**7 Class Documentation180**
7.1 OptixAabb Struct Reference.. ......... ........................ . 180
7.2 OptixAccelBufferSizes Struct Reference. . . . . . . . ..................... 181
7.3 OptixAccelBuildOptions Struct Reference............... ............. 182
7.4 OptixAccelEmitDesc Struct Reference. . . . . . . . . .. ................... 182
7.5 OptixBuildInput Struct Reference. . . . . . . . . . ..... .................. 183
7.6 OptixBuildInputCurveArray Struct Reference................ .......... 184
7.7 OptixBuildInputCustomPrimitiveArray Struct Reference................... 186
7.8 OptixBuildInputInstanceArray Struct Reference. . . . . . . . ..... ........... 188
7.9 OptixBuildInputOpacityMicromap Struct Reference. . . . . . . . .. ............ 188
7.10 OptixBuildInputSphereArray Struct Reference. . . . . . . . ................. 190
7.11 OptixBuildInputTriangleArray Struct Reference. . . . . . . . ............... . 192
7.12 OptixBuiltinISOptions Struct Reference. . . . . . . . ..................... 195
7.13 OptixClusterAccelBuildInput Struct Reference......................... 195
7.14 OptixClusterAccelBuildInputClusters Struct Reference............ ........ 196
7.15 OptixClusterAccelBuildInputClustersArgs Struct Reference. . . . . . . . . ...... .. 197
7.16 OptixClusterAccelBuildInputGrids Struct Reference. . . . . . . . .. ............ 197
7.17 OptixClusterAccelBuildInputGridsArgs Struct Reference........... ........ 198
7.18 OptixClusterAccelBuildInputTemplatesArgs Struct Reference. . . . . . . . . ...... . 199
7.19 OptixClusterAccelBuildInputTriangles Struct Reference. . . . . . . . . . .......... 200

---

7.20 OptixClusterAccelBuildInputTrianglesArgs Struct Reference................ . 202
7.21 OptixClusterAccelBuildModeDesc Struct Reference. . . . . . . . .............. 204
7.22 OptixClusterAccelBuildModeDescExplicitDest Struct Reference. . . . . . . . . . . .. .. 205
7.23 OptixClusterAccelBuildModeDescGetSize Struct Reference. . . . . . . . . ...... .. . 206
7.24 OptixClusterAccelBuildModeDescImplicitDest Struct Reference. . . . . . . . ....... 207
7.25 OptixClusterAccelPrimitiveInfo Struct Reference................ ........ 208
7.26 OptixCoopVec< T, N > Class Template Reference............ ........... 208
7.27 OptixCoopVecMatrixDescription Struct Reference...................... . 210
7.28 OptixDenoiserGuideLayer Struct Reference. . . . . . . . . .................. 211
7.29 OptixDenoiserLayer Struct Reference. . . . . . . . . ..................... 212
7.30 OptixDenoiserOptions Struct Reference. . . . . . . . . . ..... .............. 212
7.31 OptixDenoiserParams Struct Reference. . . . . . . . ..... ................ 213
7.32 OptixDenoiserSizes Struct Reference. . . . . . . . . . ..... ................ 214
7.33 OptixDeviceContextOptions Struct Reference. . . . . . . . ..... ............. 215
7.34 OptixFunctionTable Struct Reference. . . . . . . . ........ .............. . 216
7.35 OptixImage2D Struct Reference............. ..................... 227
7.36 OptixIncomingHitObject Struct Reference. . . . . . . . . ..... ............. . 228
7.37 OptixInstance Struct Reference.................................. 229
7.38 OptixMatrixMotionTransform Struct Reference........... .............. 230
7.39 OptixMicromapBuffers Struct Reference........... .................. 231
7.40 OptixMicromapBufferSizes Struct Reference. . . . . . . . .. ................ 232
7.41 OptixModuleCompileBoundValueEntry Struct Reference. . . . . . . . . . ........ . 232
7.42 OptixModuleCompileOptions Struct Reference. . . . . . . . . .. .............. 233
7.43 OptixMotionOptions Struct Reference.............................. 234
7.44 OptixNetworkDescription Struct Reference.......................... . 235
7.45 OptixOpacityMicromapArrayBuildInput Struct Reference. . . . . . . . . ........ . 236
7.46 OptixOpacityMicromapDesc Struct Reference. . . . . . . . . ................. 237
7.47 OptixOpacityMicromapHistogramEntry Struct Reference. . . . . . . . . .......... 237
7.48 OptixOpacityMicromapUsageCount Struct Reference. . . . . . . . . ............ 238
7.49 OptixOutgoingHitObject Struct Reference. . . . . . . . . ................... 238
7.50 OptixPayloadType Struct Reference. . . . . . . . .. .................... . 239
7.51 OptixPipelineCompileOptions Struct Reference. . . . . . . . . . . .. ............ 239
7.52 OptixPipelineLinkOptions Struct Reference. . . . . . . . ..... ............. . 241
7.53 OptixProgramGroupCallables Struct Reference. . . . . . . . ..... ............ 242
7.54 OptixProgramGroupDesc Struct Reference. . . . . . . . . . ..... ............ 243
7.55 OptixProgramGroupHitgroup Struct Reference. . . . . . . . .. ............... 244
7.56 OptixProgramGroupOptions Struct Reference. . . . . . . . ..... ............ 245
7.57 OptixProgramGroupSingleModule Struct Reference. . . . . . . . ..... ......... 245
7.58 OptixRelocateInput Struct Reference............... ................ 246
7.59 OptixRelocateInputInstanceArray Struct Reference. . . . . . . . .. ............. 247
7.60 OptixRelocateInputOpacityMicromap Struct Reference. . . . . . . . . .. ......... 247
7.61 OptixRelocateInputTriangleArray Struct Reference. . . . . . . . . . ............. 248
7.62 OptixRelocationInfo Struct Reference. . . . . . . . .. ..................... 248
7.63 OptixShaderBindingTable Struct Reference. . . . . . . . . . ..... ............ 248
7.64 OptixSRTData Struct Reference.................................. 250
7.65 OptixSRTMotionTransform Struct Reference. . . . . . . . . ..... ............ 252
7.66 OptixStackSizes Struct Reference. . . . . . . . . . ..... .................. 253
7.67 OptixStaticTransform Struct Reference. . . . . . . . ........ .............. 255
7.68 OptixTraverseData Struct Reference. . . . . . . . ..... ................. . 255
7.69 OptixUtilDenoiserImageTile Struct Reference. . . . . . . . . ................. 256
7.70 optix_internal::TypePack<... > Struct Template Reference.................. 256
**File Documentation 256**

---

8.1 optix_device_impl.h File Reference................ ................ 256
8.2 optix_device_impl.h. ..... ........ .......................... 297
8.3 optix_device_impl_transformations.h File Reference...................... 341
8.4 optix_device_impl_transformations.h. . . . . . . . .. .................... 342
8.5 optix_micromap_impl.h File Reference. . . . . . . . . . . .. ................. 349
8.6 optix_micromap_impl.h.. .......... ....................... .. . 350
8.7 optix.h File Reference. ... .......... .......................... 352
8.8 optix.h. .... ... ............................ .. .. .. .. .. .. 353
8.9 optix_denoiser_tiling.h File Reference. . . . . . . . ..... ................. 353
8.10 optix_denoiser_tiling.h. .... ..... .......................... .. . 354
8.11 optix_device.h File Reference.................................. . 359
8.12 optix_device.h. ... ... ...... ............................... 369
8.13 optix_function_table.h File Reference. . . . . . . . . ...................... 383
8.14 optix_function_table.h. ... ......... ..................... .. .. . 383
8.15 optix_function_table_definition.h File Reference. . . . . . . . . .. ............ . 389
8.16 optix_function_table_definition.h. . . . . . . . . . . .. .................... 389
8.17 optix_host.h File Reference... .......... ........................ 390
8.18 optix_host.h. ... ... ................................ .. .. .. 393
8.19 optix_micromap.h File Reference. . . . . . . . . .. ...................... 399
8.20 optix_micromap.h. ... ............ .................... .. .. .. 400
8.21 optix_stack_size.h File Reference................................. 401
8.22 optix_stack_size.h... ... ....... ............................. 402
8.23 optix_stubs.h File Reference................................... . 406
8.24 optix_stubs.h. .... ... ...... ............................... 406
8.25 optix_types.h File Reference.................................... 420
8.26 optix_types.h. ... ... ...... ......................... .. .. .. 431
8.27 main.dox File Reference... ................................... 454

---

## 1 NVIDIA OptiX 9.1 API

This document describes the NVIDIA OptiX application programming interface. See <sup>https</sup>
://raytracing-docs.nvidia.com/ for more information about programming with NVIDIA

OptiX.

## 2 Module Index

## 2.1 Modules

Here is a list of all modules:
Device API
Cooperative Vector
Function Table
Host API
Error handling
Device context
Pipelines
Modules
Tasks
Program groups
Launches

6
70
78
79
79
80
85
88
91
94
95

Acceleration structures96
Cooperative Vector105

Denoiser
Utilities
Types
3 Class Index
3.1 Class List

107
113
120

Here are the classes, structs, unions and interfaces with brief descriptions:

OptixAabb
AABB inputs
OptixAccelBufferSizes

180

Struct for querying builder allocation requirements181

OptixAccelBuildOptions

Build options for acceleration structures182
OptixAccelEmitDesc
Specifies a type and output destination for emitted post-build properties182

OptixBuildInput
Build inputs
OptixBuildInputCurveArray
Curve inputs

183
184

---

OptixBuildInputCustomPrimitiveArray
Custom primitive inputs186
OptixBuildInputInstanceArray
Instance and instance pointer inputs188
OptixBuildInputOpacityMicromap 188
OptixBuildInputSphereArray
Sphere inputs 190
OptixBuildInputTriangleArray
Triangle inputs 192
OptixBuiltinISOptions
Specifies the options for retrieving an intersection program for a built-in primitive type.
The primitive type must not be OPTIX_PRIMITIVE_TYPE_CUSTOM195
OptixClusterAccelBuildInput 195
OptixClusterAccelBuildInputClusters 196
OptixClusterAccelBuildInputClustersArgs
Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_
CLUSTERS builds 197
OptixClusterAccelBuildInputGrids 197
OptixClusterAccelBuildInputGridsArgs
Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_
FROM_GRIDS builds198
OptixClusterAccelBuildInputTemplatesArgs
Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_
FROM_TEMPLATES builds199
OptixClusterAccelBuildInputTriangles 200
OptixClusterAccelBuildInputTrianglesArgs
Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_
FROM_TRIANGLES builds and OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_
FROM_TRIANGLES builds202
OptixClusterAccelBuildModeDesc 204
OptixClusterAccelBuildModeDescExplicitDest 205
OptixClusterAccelBuildModeDescGetSize 206
OptixClusterAccelBuildModeDescImplicitDest 207
OptixClusterAccelPrimitiveInfo 208
OptixCoopVec< T, N >
The API does not require the use of this class specifically, but it must define a certain interface as spelled out by the public members of the class. Note that not all types of T are
supported. Only 8 and 32 bit signed and unsigned integral types along with 16 and 32 bit
floating point values208
OptixCoopVecMatrixDescription
Each matrix's offset from the base address is expressed with offsetInBytes. This allows for
non-uniform matrices to be tightly packed210
OptixDenoiserGuideLayer
Guide layer for the denoiser211

---

OptixDenoiserLayer
Input/Output layers for the denoiser212
OptixDenoiserOptions
Options used by the denoiser212
OptixDenoiserParams
Various parameters used by the denoiser213
OptixDenoiserSizes
Various sizes related to the denoiser214
OptixDeviceContextOptions
Parameters used foroptixDeviceContextCreate( )215
OptixFunctionTable
The function table containing all API functions216
OptixImage2D
Image descriptor used by the denoiser227
OptixIncomingHitObject 228
OptixInstance
Instances 229
OptixMatrixMotionTransform
Represents a matrix motion transformation230
OptixMicromapBuffers
Buffer inputs for opacity micromap array builds231
OptixMicromapBufferSizes
Conservative memory requirements for building a opacity micromap array232
OptixModuleCompileBoundValueEntry
Struct for specifying specializations for pipelineParams as specified in OptixPipelineCompileOptions
::pipelineLaunchParamsVariableName232
OptixModuleCompileOptions
Compilation options for module233
OptixMotionOptions
Motion options 234
OptixNetworkDescription 235
OptixOpacityMicromapArrayBuildInput
Inputs to opacity micromap array construction236
OptixOpacityMicromapDesc
Opacity micromap descriptor237
OptixOpacityMicromapHistogramEntry
Opacity micromap histogram entry. Specifies how many opacity micromaps of a specific
type are input to the opacity micromap array build. Note that while this is similar to
OptixOpacityMicromapUsageCount, the histogram entry specifies how many opacity
micromaps of a specific type are combined into a opacity micromap array237

---

OptixOpacityMicromapUsageCount
Opacity micromap usage count for acceleration structure builds. Specifies how many opacity micromaps of a specific type are referenced by triangles when building the AS. Note
that while this is similar to OptixOpacityMicromapHistogramEntry, the usage count
specifies how many opacity micromaps of a specific type are referenced by triangles in the
AS 238
OptixOutgoingHitObject 238
OptixPayloadType
Specifies a single payload type239
OptixPipelineCompileOptions
Compilation options for all modules of a pipeline239
OptixPipelineLinkOptions
Link options for a pipeline241
OptixProgramGroupCallables
Program group representing callables242
OptixProgramGroupDesc
Descriptor for program groups243
OptixProgramGroupHitgroup
Program group representing the hitgroup244
OptixProgramGroupOptions
Program group options245
OptixProgramGroupSingleModule
Program group representing a single module245
OptixRelocateInput
Relocation inputs 246
OptixRelocateInputInstanceArray
Instance and instance pointer inputs247
OptixRelocateInputOpacityMicromap 247
OptixRelocateInputTriangleArray
Triangle inputs 248
OptixRelocationInfo
Used to store information related to relocation of optix data structures248
OptixShaderBindingTable
Describes the shader binding table (SBT)248
OptixSRTData
Represents an SRT transformation250
OptixSRTMotionTransform
Represents an SRT motion transformation252
OptixStackSizes
Describes the stack size requirements of a program group253
OptixStaticTransform
Static transform 255

---

OptixTraverseData
Hit Object Struct to store the data collected in a hit object during traversal in an internal format using optixHitObjectGetTraverseData( ). The hit object can be reconstructed
using that data at a later point with optixMakeHitObjectWithTraverseData( ) 255
OptixUtilDenoiserImageTile
Tile definition 256
optix_internal::TypePack<... > 256
4 File Index
4.1 File List
Here is a list of all files with brief descriptions:
**optix_device_impl.h**
**OptiX public API 256**
**optix_device_impl_transformations.h**
**OptiX public API 341**
**optix_micromap_impl.h**
**OptiX micromap helper functions349**
**optix.h**
**OptiX public API header352**
**optix_denoiser_tiling.h**
**OptiX public API header353**
**optix_device.h**
**OptiX public API header359**
**optix_function_table.h**
**OptiX public API header383**
**optix_function_table_definition.h**
**OptiX public API header389**
**optix_host.h**
**OptiX public API header390**
**optix_micromap.h**
**OptiX micromap helper functions399**
**optix_stack_size.h**
**OptiX public API header401**
**optix_stubs.h**
**OptiX public API header406**
**optix_types.h**
**OptiX public API header420**

---

## 5 Module Documentation

## 5.1 Device API

## Modules
• Cooperative Vector

## Classes
• struct OptixIncomingHitObject
• struct OptixOutgoingHitObject

## Functions
• template<typename... Payload>
static __forceinline__ __device__ void optixTrace (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixTraverse (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int missSBTIndex, Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixTrace (OptixPayloadTypeID type, OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixTraverse (OptixPayloadTypeID type, OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)
• static __forceinline__ __device__ void optixReorder (unsigned int coherenceHint, unsigned int numCoherenceHintBitsFromLSB)
• static __forceinline__ __device__ void optixReorder ()
• template<typename... Payload>
static __forceinline__ __device__ void optixInvoke (Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixInvoke (OptixPayloadTypeID type, Payload &... payload)
• static __forceinline__ __device__ void optixMakeHitObject (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float raYTime, unsigned int rayFlags, OptixTraverseData traverseData, const OptixTraversableHandle *transforms, unsigned int numTransforms)
• static __forceinline__ __device__ void optixMakeMissHitObject (unsigned int missSBTIndex, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, unsigned int rayFlags)
• static __forceinline__ __device__ void optixMakeNopHitObject ()
• static __forceinline__ __device__ void optixHitObjectGetTraverseData (OptixTraverseData *data)
• static __forceinline__ __device__ bool optixHitObjectIsHit ()
• static __forceinline__ __device__ bool optixHitObjectIsMiss ()
• static __forceinline__ __device__ bool optixHitObjectIsNop ()

---

•static __forceinline__ __device__ voidoptixHitObjectSetSbtRecordIndex(unsigned int
sbtRecordIndex)

•static __forceinline__ __device__ unsigned intoptixHitObjectGetSbtRecordIndex()

•static __forceinline__ __device__OptixTraversableHandle

optixHitObjectGetGASTraversableHandle()

•static __forceinline__ __device__ voidoptixSetPayload_0(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_1(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_2(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_3(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_4(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_5(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_6(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_8(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_7(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_9(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_10(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_11(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_12(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_15(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_13(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_16(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_14(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_17(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_18(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_19(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_20(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_21(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_22(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_23(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_24(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_25(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_26(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_27(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_28(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_29(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_30(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_31(unsigned int p)

•static __forceinline__ __device__ unsigned intoptixGetPayload_0()

•static __forceinline__ __device__ unsigned intoptixGetPayload_1()

•static __forceinline__ __device__ unsigned intoptixGetPayload_2()

•static __forceinline__ __device__ unsigned intoptixGetPayload_4()

•static __forceinline__ __device__ unsigned intoptixGetPayload_3()

•static __forceinline__ __device__ unsigned intoptixGetPayload_5()

•static __forceinline__ __device__ unsigned intoptixGetPayload_6()

•static __forceinline__ __device__ unsigned intoptixGetPayload_7()

•static __forceinline__ __device__ unsigned intoptixGetPayload_8()

•static __forceinline__ __device__ unsigned intoptixGetPayload_9()

•static __forceinline__ __device__ unsigned intoptixGetPayload_10()

•static __forceinline__ __device__ unsigned intoptixGetPayload_11()

•static __forceinline__ __device__ unsigned intoptixGetPayload_12()

---

• static __forceinline__ __device__ unsigned int optixGetPayload_13 ()
• static __forceline__ __device__ unsigned int optixGetPayload_14 ()
• static __forceline__ __device__ unsigned int optixGetPayload_15 ()
• static __forceline__ __device__ unsigned int optixGetPayload_16 ()
• static __forceline__ __device__ unsigned int optixGetPayload_17 ()
• static __forceline__ __device__ unsigned int optixGetPayload_18 ()
• static __forceline__ __device__ unsigned int optixGetPayload_19 ()
• static __forceline__ __device__ unsigned int optixGetPayload_20 ()
• static __forceline__ __device__ unsigned int optixGetPayload_21 ()
• static __forceline__ __device__ unsigned int optixGetPayload_22 ()
• static __forceline__ __device__ unsigned int optixGetPayload_23 ()
• static __forceline__ __device__ unsigned int optixGetPayload_24 ()
• static __forceline__ __device__ unsigned int optixGetPayload_25 ()
• static __forceline__ __device__ unsigned int optixGetPayload_26 ()
• static __forceline__ __device__ unsigned int optixGetPayload_27 ()
• static __forceline__ __device__ unsigned int optixGetPayload_28 ()
• static __forceline__ __device__ unsigned int optixGetPayload_29 ()
• static __forceline__ __device__ unsigned int optixGetPayload_30 ()
• static __forceline__ __device__ unsigned int optixGetPayload_31 ()
• static __forceline__ __device__ void optixSetPayloadTypes (unsigned int typeMask)
• static __forceline__ __device__ unsigned int optixUndefinedValue ()
• static __forceline__ __device__ unsigned int optixGetRemainingTraceDepth ()
• static __forceline__ __device__ float3 optixGetWorldRayOrigin ()
• static __forceline__ __device__ float3 optixHitObjectGetWorldRayOrigin ()
• static __forceline__ __device__ float3 optixGetWorldRayDirection ()
• static __forceline__ __device__ float3 optixHitObjectGetWorldRayDirection ()
• static __forceline__ __device__ float3 optixGetObjectRayOrigin ()
• static __forceline__ __device__ float3 optixGetObjectRayDirection ()
• static __forceline__ __device__ float optixGetRayTmin ()
• static __forceline__ __device__ float optixHitObjectGetRayTmin ()
• static __forceline__ __device__ float optixGetRayTmax ()
• static __forceline__ __device__ float optixHitObjectGetRayTmax ()
• static __forceline__ __device__ float optixGetRayTime ()
• static __forceline__ __device__ float optixHitObjectGetRayTime ()
• static __forceline__ __device__ unsigned int optixGetRayFlags ()
• static __forceline__ __device__ unsigned int optixHitObjectGetRayFlags ()
• static __forceline__ __device__ unsigned int optixGetRayVisibilityMask ()
• static __forceline__ __device__ OptixTraversableHandle optixGetInstanceTraversableFromIAS
(OptixTraversableHandle ias, unsigned int instIdx)
• static __forceline__ __device__ void optixGetTriangleVertexData (OptixTraversableHandle gas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float3 data[3])
• static __forceline__ __device__ void optixGetTriangleVertexDataFromHandle
(OptixTraversableHandle gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float3 data[3])
• static __forceline__ __device__ void optixGetTriangleVertexData (float3 data[3])
• static __forceline__ __device__ void optixHitObjectGetTriangleVertexData (float3 data[3])
• static __forceline__ __device__ void optixGetLinearCurveVertexData (OptixTraversableHandle gas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[2])
• static __forceline__ __device__ void optixGetLinearCurveVertexDataFromHandle
(OptixTraversableHandle gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[2])

---

•static __forceinline__ __device__ voidoptixGetLinearCurveVertexData(float4 data[2])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexData
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetLinearCurveVertexData(float4 data[2])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexData(float4 data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineRocapsVertexData(float4
data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetQuadraticBSplineVertexData(float4
data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetQuadraticBSplineRocapsVertexData
(float4 data[3])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBSplineVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBSplineRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCatmullRomVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCatmullRomRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

---

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBezierVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBezierRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetRibbonVertexData(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[3])

•static __forceinline__ __device__ voidoptixGetRibbonVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetRibbonVertexData(float4 data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetRibbonVertexData(float4 data[3])

•static __forceinline__ __device__ float3optixGetRibbonNormal(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float2 ribbonParameters)

•static __forceinline__ __device__ float3optixGetRibbonNormalFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float2
ribbonParameters)

•static __forceinline__ __device__ float3optixGetRibbonNormal(float2 ribbonParameters)

•static __forceinline__ __device__ float3optixHitObjectGetRibbonNormal(float2
ribbonParameters)

•static __forceinline__ __device__ voidoptixGetSphereData(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[1])

•static __forceinline__ __device__ voidoptixGetSphereDataFromHandle(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[1])

•static __forceinline__ __device__ voidoptixGetSphereData(float4 data[1])

•static __forceinline__ __device__ voidoptixHitObjectGetSphereData(float4 data[1])

•static __forceinline__ __device__OptixTraversableHandleoptixGetGASTraversableHandle()

•static __forceinline__ __device__ floatoptixGetGASMotionTimeBegin(OptixTraversableHandle
gas)

•static __forceinline__ __device__ floatoptixGetGASMotionTimeEnd(OptixTraversableHandle
gas)

•static __forceinline__ __device__ unsigned intoptixGetGASMotionStepCount
(OptixTraversableHandlegas)

•static __forceinline__ __device__ voidoptixGetWorldToObjectTransformMatrix(float m[12])

•static __forceinline__ __device__ voidoptixGetObjectToWorldTransformMatrix(float m[12])

•static __forceinline__ __device__ float3optixTransformPointFromWorldToObjectSpace(float3
point)

•static __forceinline__ __device__ float3optixTransformVectorFromWorldToObjectSpace(float3
vec)

•static __forceinline__ __device__ float3optixTransformNormalFromWorldToObjectSpace(float3
normal)

•static __forceinline__ __device__ float3optixTransformPointFromObjectToWorldSpace(float3
point)

•static __forceinline__ __device__ float3optixTransformVectorFromObjectToWorldSpace(float3
vec)

•static __forceinline__ __device__ float3optixTransformNormalFromObjectToWorldSpace(float3
normal)

•static __forceinline__ __device__ voidoptixHitObjectGetWorldToObjectTransformMatrix(float
m[12])

•static __forceinline__ __device__ voidoptixHitObjectGetObjectToWorldTransformMatrix(float
m[12])

---

• static __forceinline__ __device__ float3 optixHitObjectTransformPointFromWorldToObjectSpace (float3 point)
• static __forceline__ __device__ float3 optixHitObjectTransformVectorFromWorldToObjectSpace (float3 vec)
• static __forceline__ __device__ float3 optixHitObjectTransformNormalFromWorldToObjectSpace (float3 normal)
• static __forceline__ __device__ float3 optixHitObjectTransformPointFromObjectToWorldSpace (float3 point)
• static __forceline__ __device__ float3 optixHitObjectTransformVectorFromObjectToWorldSpace (float3 vec)
• static __forceline__ __device__ float3 optixHitObjectTransformNormalFromObjectToWorldSpace (float3 normal)
• template<typename HitState >
static __forceline__ __device__ void optixGetWorldToObjectTransformMatrix (const HitState &hs, float m[12])
• template<typename HitState >
static __forceline__ __device__ void optixGetObjectToWorldTransformMatrix (const HitState &hs, float m[12])
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformPointFromWorldToObjectSpace (const HitState &hs, float3 point)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace (const HitState &hs, float3 vec)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformPointFromObjectToWorldSpace (const HitState &hs, float3 normal)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace (const HitState &hs, float3 point)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace (const HitState &hs, float3 vec)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace (const HitState &hs, float3 normal)
• static __forceline__ __device__ unsigned int optixGetTransformListSize ()
• static __forceline__ __device__ unsigned int optixHitObjectGetTransformListSize ()
• static __forceline__ __device__ OptixTraversableHandle optixGetTransformListHandle (unsigned int index)
• static __forceline__ __device__ OptixTraversableHandle optixHitObjectGetTransformListHandle (unsigned int index)
• static __forceline__ __device__ OptixTransformType optixGetTransformTypeFromHandle (OptixTraversableHandle handle)
• static __forceline__ __device__ const OptixStaticTransform *
optixGetStaticTransformFromHandle (OptixTraversableHandle handle)
• static __forceline__ __device__ const OptixSRTMotionTransform *
optixGetSRTMotionTransformFromHandle (OptixTraversableHandle handle)
• static __forceline__ __device__ const OptixMatrixMotionTransform *
optixGetMatrixMotionTransformFromHandle (OptixTraversableHandle handle)
• static __forceline__ __device__ unsigned int optixGetInstanceIdFromHandle (OptixTraversableHandle handle)

---

•static __forceinline__ __device__OptixTraversableHandleoptixGetInstanceChildFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ const float4 *∗* optixGetInstanceTransformFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ const float4 *∗* optixGetInstanceInverseTransformFromHandle
(OptixTraversableHandlehandle)

•static __device__ __forceinline__CUdeviceptroptixGetGASPointerFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5, unsigned int a6)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5, unsigned int a6, unsigned int a7)

•static __forceinline__ __device__ unsigned intoptixGetAttribute_0()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_1()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_2()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_3()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_4()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_5()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_6()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_7()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_0()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_1()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_2()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_3()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_4()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_5()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_6()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_7()

•static __forceinline__ __device__ voidoptixTerminateRay()

•static __forceinline__ __device__ voidoptixIgnoreIntersection()

•static __forceinline__ __device__ unsigned intoptixGetPrimitiveIndex()

•static __forceinline__ __device__ unsigned intoptixGetClusterId()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetClusterId()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetPrimitiveIndex()

•static __forceinline__ __device__ unsigned intoptixGetSbtGASIndex()

---

• static __forceinline__ __device__ unsigned int optixHitObjectGetSbtGASIndex ()
• static __forceline__ __device__ unsigned int optixGetInstanceId ()
• static __forceline__ __device__ unsigned int optixHitObjectGetInstanceId ()
• static __forceline__ __device__ unsigned int optixGetInstanceIndex ()
• static __forceline__ __device__ unsigned int optixHitObjectGetInstanceIndex ()
• static __forceline__ __device__ unsigned int optixGetHitKind ()
• static __forceline__ __device__ unsigned int optixHitObjectGetHitKind ()
• static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType (unsigned int hitKind)
• static __forceline__ __device__ bool optixIsFrontFaceHit (unsigned int hitKind)
• static __forceline__ __device__ bool optixIsBackFaceHit (unsigned int hitKind)
• static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType ()
• static __forceline__ __device__ bool optixIsFrontFaceHit ()
• static __forceline__ __device__ bool optixIsBackFaceHit ()
• static __forceline__ __device__ bool optixIsTriangleHit ()
• static __forceline__ __device__ bool optixIsTriangleFrontFaceHit ()
• static __forceline__ __device__ bool optixIsTriangleBackFaceHit ()
• static __forceline__ __device__ float2 optixGetTriangleBarycentrics ()
• static __forceline__ __device__ float2 optixHitObjectGetTriangleBarycentrics ()
• static __forceline__ __device__ float optixGetCurveParameter ()
• static __forceline__ __device__ float optixHitObjectGetCurveParameter ()
• static __forceline__ __device__ float2 optixGetRibbonParameters ()
• static __forceline__ __device__ float2 optixHitObjectGetRibbonParameters ()
• static __forceline__ __device__ uint3 optixGetLaunchIndex ()
• static __forceline__ __device__ uint3 optixGetLaunchDimensions ()
• static __forceline__ __device__ CUdeviceptr optixGetSbtDataPointer ()
• static __forceline__ __device__ CUdeviceptr optixHitObjectGetSbtDataPointer ()
• static __forceline__ __device__ void optixThrowException (int exceptionCode)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4, unsigned int exceptionDetail5, unsigned int exceptionDetail6)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4, unsigned int exceptionDetail5, unsigned int exceptionDetail6, unsigned int exceptionDetail7)

---

<u>14</u>

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_0()

•static __forceinline__ __device__ intoptixGetExceptionCode()

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_1()

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_2()

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_3()

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_4()

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_5()

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_6()

•static __forceinline__ __device__ unsigned intoptixGetExceptionDetail_7()

•static __forceinline__ __device__ char *∗* optixGetExceptionLineInfo()

•template<typename ReturnT , typename... ArgTypes>

static __forceinline__ __device__ ReturnToptixDirectCall(unsigned int sbtIndex, ArgTypes...
args)

•template<typename ReturnT , typename... ArgTypes>

static __forceinline__ __device__ ReturnToptixContinuationCall(unsigned int sbtIndex,
ArgTypes... args)

•static __forceinline__ __device__ uint4optixTexFootprint2D(unsigned long long tex, unsigned
int texInfo, float x, float y, unsigned int *∗*singleMipLevel)

•static __forceinline__ __device__ uint4optixTexFootprint2DLod(unsigned long long tex,
unsigned int texInfo, float x, float y, float level, bool coarse, unsigned int *∗*singleMipLevel)

•static __forceinline__ __device__ uint4optixTexFootprint2DGrad(unsigned long long tex,
unsigned int texInfo, float x, float y, float dPdx_x, float dPdx_y, float dPdy_x, float dPdy_y, bool
coarse, unsigned int *∗*singleMipLevel)

## 5.1.1 Detailed Description

OptiX Device API.

## 5.1.2 Function Documentation

## 5.1.2.1 optixContinuationCall( )

template<typename ReturnT, typename... ArgTypes>
static __forceinline__ __device__ ReturnT optixContinuationCall (
unsigned int *sbtIndex,*
ArgTypes... *args*) *[static]*

Creates a call to the continuation callable program at the specified SBT entry.

This will call the program that was specified in theOptixProgramGroupCallables
::entryFunctionNameCCin the module specified byOptixProgramGroupCallables::moduleCC.

The address of the SBT entry is calculated by:OptixShaderBindingTable::callablesRecordBase+
(OptixShaderBindingTable::callablesRecordStrideInBytes *∗* sbtIndex).

As opposed to direct callable programs, continuation callable programs are allowed to make secondary
optixTrace calls.

Behavior is undefined if there is no continuation callable program at the specified SBT entry.

Behavior is undefined if the number of arguments that are being passed in does not match the number
of parameters expected by the program that is called. In validation mode an exception will be
generated.

---

15

Parameters

| in | sbtIndex | The offset of the SBT entry of the continuation callable program to call relative to OptixShaderBindingTable::callablesRecordBase. |
| --- | --- | --- |
| in | args | The arguments to pass to the continuation callable program. |

## Available in RG, CH, MS, CC

## 5.1.2.2 optixDirectCall( )

template<typename ReturnT, typename... ArgTypes>
static __forceinline__ __device__ ReturnT optixDirectCall (
unsigned int *sbtIndex,*
ArgTypes... *args*) *[static]*

Creates a call to the direct callable program at the specified SBT entry.

This will call the program that was specified in theOptixProgramGroupCallables
::entryFunctionNameDCin the module specified byOptixProgramGroupCallables::moduleDC.

The address of the SBT entry is calculated by:OptixShaderBindingTable::callablesRecordBase+
(OptixShaderBindingTable::callablesRecordStrideInBytes *∗* sbtIndex).

Direct callable programs are allowed to call optixTrace, but any secondary trace calls invoked from
subsequently called CH, MS and callable programs will result an an error.

Behavior is undefined if there is no direct callable program at the specified SBT entry.

Behavior is undefined if the number of arguments that are being passed in does not match the number
of parameters expected by the program that is called. In validation mode an exception will be
generated.

## Parameters

| in | sbtIndex | The offset of the SBT entry of the direct callable program to call relative to OptixShaderBindingTable::callablesRecordBase. |
| --- | --- | --- |
| in | args | The arguments to pass to the direct callable program. |

## Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.3 optixGetAttribute_0( )

static __forceinline__ __device__ unsigned int optixGetAttribute_0 () *[static]*
Returns the attribute at the given slot index. There are up to 8 attributes available. The number of
attributes is configured withOptixPipelineCompileOptions::numAttributeValues.

Available in AH, CH

## 5.1.2.4 optixGetAttribute_1( )

static __forceinline__ __device__ unsigned int optixGetAttribute_1 () *[static]*

5.1.2.5 optixGetAttribute_2( )

static __forceinline__ __device__ unsigned int optixGetAttribute_2 () *[static]*

---

<u>16</u>

5.1.2.6 optixGetAttribute_3( )
static __forceinline__ __device__ unsigned int optixGetAttribute_3 () *[static]*
5.1.2.7 optixGetAttribute_4( )
static __forceinline__ __device__ unsigned int optixGetAttribute_4 () *[static]*
5.1.2.8 optixGetAttribute_5( )
static __forceinline__ __device__ unsigned int optixGetAttribute_5 () *[static]*
5.1.2.9 optixGetAttribute_6( )
static __forceinline__ __device__ unsigned int optixGetAttribute_6 () *[static]*
5.1.2.10 optixGetAttribute_7( )
static __forceinline__ __device__ unsigned int optixGetAttribute_7 () *[static]*
5.1.2.11 optixGetCatmullRomRocapsVertexData( )
static __forceinline__ __device__ void optixGetCatmullRomRocapsVertexData (
float4 *data[4]*) *[static]*

## 5.1.2.12 optixGetCatmullRomRocapsVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCatmullRomRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

## 5.1.2.13 optixGetCatmullRomVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetCatmullRomVertexData (
float4 *data[4]*) *[static]*
Returns the object space curve control vertex data of a CatmullRom spline curve in a Geometry
Acceleration Structure (GAS) at a given motion time.
data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.
Available in AH, CH

## 5.1.2.14 optixGetCatmullRomVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetCatmullRomVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*

---

<u>17</u>

## float4 data[4]) [static]

Deprecated. Call eitheroptixGetCatmullRomVertexData(float4 data[4])for current hit data, or
optixGetCatmullRomVertexDataFromHandle( )for random access sphere data.

Returns the object space curve control vertex data of a CatmullRom spline curve in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.15 optixGetCatmullRomVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCatmullRomVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

Returns the object space curve control vertex data of a CatmullRom spline curve in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.16 optixGetClusterId( )

static __forceinline__ __device__ unsigned int optixGetClusterId () *[static]*
Returns the user-provided cluster ID of the intersected CLAS of a hit.

Returns OPTIX_CLUSTER_ID_INVALID if the closest (or current) intersection is not a cluster.

see alsoOptixPipelineCompileOptions::allowClusteredGeometry

Available in AH, CH

## 5.1.2.17 optixGetCubicBezierRocapsVertexData( )

static __forceinline__ __device__ void optixGetCubicBezierRocapsVertexData (
float4 *data[4]*) *[static]*

---

<u>18</u>

## 5.1.2.18 optixGetCubicBezierRocapsVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCubicBezierRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

## 5.1.2.19 optixGetCubicBezierVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetCubicBezierVertexData (
float4 *data[4]*) *[static]*

Returns the object space curve control vertex data of a cubic Bezier curve in a Geometry Acceleration
Structure (GAS) at a given motion time.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

Available in AH, CH

## 5.1.2.20 optixGetCubicBezierVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetCubicBezierVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

Deprecated. Call eitheroptixGetCubicBezierVertexData(float4 data[4])for current hit data, or
optixGetCubicBezierVertexDataFromHandle( )for random access sphere data.

Returns the object space curve control vertex data of a cubic Bezier curve in a Geometry Acceleration
Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.21 optixGetCubicBezierVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCubicBezierVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*

---

<u>19</u>

float4 *data[4]*) *[static]*

Returns the object space curve control vertex data of a cubic Bezier curve in a Geometry Acceleration
Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.22 optixGetCubicBSplineRocapsVertexData( )

static __forceinline__ __device__ void optixGetCubicBSplineRocapsVertexData
(
float4 *data[4]*) *[static]*

## 5.1.2.23 optixGetCubicBSplineRocapsVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCubicBSplineRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

## 5.1.2.24 optixGetCubicBSplineVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetCubicBSplineVertexData (
float4 *data[4]*) *[static]*

Returns the object space curve control vertex data of a cubic BSpline curve in a Geometry Acceleration
Structure (GAS) at a given motion time.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

Available in AH, CH

## 5.1.2.25 optixGetCubicBSplineVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetCubicBSplineVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

Deprecated. Call eitheroptixGetCubicBSplineVertexData(float4 data[4])for current hit sphere data, or
optixGetCubicBSplineVertexDataFromHandle( )for random access sphere data.

Return the object space curve control vertex data of a cubic BSpline curve in a Geometry Acceleration

---

<u>20</u>

Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.26 optixGetCubicBSplineVertexDataFromHandle( )

static __forceinline__ __device__ void

optixGetCubicBSplineVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

Returns the object space curve control vertex data of a cubic BSpline curve in a Geometry Acceleration
Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.27 optixGetCurveParameter( )

static __forceinline__ __device__ float optixGetCurveParameter () *[static]*

Returns the curve parameter associated with the current intersection when using
OptixBuildInputCurveArrayobjects.

Available in AH, CH

## 5.1.2.28 optixGetExceptionCode( )

static __forceinline__ __device__ int optixGetExceptionCode () *[static]*
Returns the exception code.

Available in EX

## 5.1.2.29 optixGetExceptionDetail_0( )

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_0 ()
*[static]*

Returns the 32-bit exception detail at slot 0.

The behavior is undefined if the exception is not a user exception, or the used overload
optixThrowException( )did not provide the queried exception detail.

---

Available in EX

## 5.1.2.30 optixGetExceptionDetail_1( )

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_1 ()
*[static]*

Returns the 32-bit exception detail at slot 1.

See alsooptixGetExceptionDetail_0( )Available in EX

## 5.1.2.31 optixGetExceptionDetail_2( )

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_2 ()
*[static]*

Returns the 32-bit exception detail at slot 2.

See alsooptixGetExceptionDetail_0( )Available in EX

## 5.1.2.32 optixGetExceptionDetail_3( )

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_3 ()
*[static]*

Returns the 32-bit exception detail at slot 3.

See alsooptixGetExceptionDetail_0( )Available in EX

## 5.1.2.33 optixGetExceptionDetail_4( )

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_4 ()
*[static]*

Returns the 32-bit exception detail at slot 4.

## 5.1.2.34 optixGetExceptionDetail_5( )

See alsooptixGetExceptionDetail_0( )Available in EX

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_5 ()
*[static]*

Returns the 32-bit exception detail at slot 5.

See alsooptixGetExceptionDetail_0( )Available in EX

## 5.1.2.35 optixGetExceptionDetail_6( )

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_6 ()
*[static]*

Returns the 32-bit exception detail at slot 6.

See alsooptixGetExceptionDetail_0( )Available in EX

## 5.1.2.36 optixGetExceptionDetail_7( )

static __forceinline__ __device__ unsigned int optixGetExceptionDetail_7 ()
*[static]*

Returns the 32-bit exception detail at slot 7.

---

See alsooptixGetExceptionDetail_0( )Available in EX

## 5.1.2.37 optixGetExceptionLineInfo( )

static __forceinline__ __device__ char *∗* optixGetExceptionLineInfo () *[static]*

Returns a string that includes information about the source location that caused the current exception.

The source location is only available for user exceptions. Line information needs to be present in the
input PTX andOptixModuleCompileOptions::debugLevelmay not be set to OPTIX_COMPILE_
DEBUG_LEVEL_NONE.

Returns a NULL pointer if no line information is available.

Available in EX

## 5.1.2.38 optixGetGASMotionStepCount( )

static __forceinline__ __device__ unsigned int optixGetGASMotionStepCount (
OptixTraversableHandle *gas*) *[static]*

Returns the number of motion steps of a GAS (seeOptixMotionOptions)

Available in all OptiX program types

## 5.1.2.39 optixGetGASMotionTimeBegin( )

static __forceinline__ __device__ float optixGetGASMotionTimeBegin (
OptixTraversableHandle *gas*) *[static]*

Returns the motion begin time of a GAS (seeOptixMotionOptions)

Available in all OptiX program types

## 5.1.2.40 optixGetGASMotionTimeEnd( )

static __forceinline__ __device__ float optixGetGASMotionTimeEnd (
OptixTraversableHandle *gas*) *[static]*

Returns the motion end time of a GAS (seeOptixMotionOptions)

Available in all OptiX program types

## 5.1.2.41 optixGetGASPointerFromHandle( )

static __device__ __forceinline__CUdeviceptroptixGetGASPointerFromHandle (
OptixTraversableHandle *handle*) *[static]*

Returns a pointer to the geometry acceleration structure from its traversable handle.

Returns 0 if the traversable is not a geometry acceleration structure.

Available in all OptiX program types

## 5.1.2.42 optixGetGASTraversableHandle( )

static __forceinline__ __device__OptixTraversableHandle
optixGetGASTraversableHandle () *[static]*

Returns the traversable handle for the Geometry Acceleration Structure (GAS) containing the current
hit.

---

Available in IS, AH, CH

## 5.1.2.43 optixGetHitKind( )

static __forceinline__ __device__ unsigned int optixGetHitKind () *[static]*

Returns the 8 bit hit kind associated with the current hit.

UseoptixGetPrimitiveType( )to interpret the hit kind. For custom intersections (primitive type OPTIX_
PRIMITIVE_TYPE_CUSTOM), this is the 7-bit hitKind passed tooptixReportIntersection( ). Hit kinds
greater than 127 are reserved for built-in primitives.

Available in AH and CH

## 5.1.2.44 optixGetInstanceChildFromHandle( )

static __forceinline__ __device__OptixTraversableHandle
optixGetInstanceChildFromHandle (
OptixTraversableHandle *handle*) *[static]*

Returns child traversable handle from anOptixInstancetraversable.

Returns 0 if the traversable handle does not reference anOptixInstance.

Available in all OptiX program types

## 5.1.2.45 optixGetInstanceId( )

static __forceinline__ __device__ unsigned int optixGetInstanceId () *[static]*

Returns theOptixInstance::instanceIdof the instance within the top level acceleration structure
associated with the current intersection.

When building an acceleration structure usingOptixBuildInputInstanceArrayeachOptixInstancehas a
user supplied instanceId.OptixInstanceobjects reference another acceleration structure. During
traversal the acceleration structures are visited top down. In the IS and AH programs the
OptixInstance::instanceIdcorresponding to the most recently visitedOptixInstanceis returned when
callingoptixGetInstanceId( ). In CHoptixGetInstanceId( )returns theOptixInstance::instanceIdwhen
the hit was recorded with optixReportIntersection. In the case where there is noOptixInstancevisited,
optixGetInstanceId returns 0

Available in IS, AH, CH

## 5.1.2.46 optixGetInstanceIdFromHandle( )

static __forceinline__ __device__ unsigned int optixGetInstanceIdFromHandle
(

OptixTraversableHandle *handle*) *[static]*

Returns instanceId from anOptixInstancetraversable.

Returns 0 if the traversable handle does not reference anOptixInstance.

Available in all OptiX program types

## 5.1.2.47 optixGetInstanceIndex( )

static __forceinline__ __device__ unsigned int optixGetInstanceIndex ()
*[static]*

Returns the zero-based index of the instance within its instance acceleration structure associated with the current intersection.

In the IS and AH programs the index corresponding to the most recently visitedOptixInstanceis
returned when callingoptixGetInstanceIndex( ). In CHoptixGetInstanceIndex( )returns the index
when the hit was recorded with optixReportIntersection. In the case where there is noOptixInstance
visited, optixGetInstanceIndex returns 0

Available in IS, AH, CH

## 5.1.2.48 optixGetInstanceInverseTransformFromHandle( )

static __forceinline__ __device__ const float4 *∗*

optixGetInstanceInverseTransformFromHandle (
OptixTraversableHandle *handle*) *[static]*

Returns world-to-object transform from anOptixInstancetraversable.

Returns 0 if the traversable handle does not reference anOptixInstance.

Available in all OptiX program types

## 5.1.2.49 optixGetInstanceTransformFromHandle( )

static __forceinline__ __device__ const float4 *∗*

optixGetInstanceTransformFromHandle (

OptixTraversableHandle *handle*) *[static]*

Returns object-to-world transform from anOptixInstancetraversable.

Returns 0 if the traversable handle does not reference anOptixInstance.

Available in all OptiX program types

## 5.1.2.50 optixGetInstanceTraversableFromIAS( )

static __forceinline__ __device__OptixTraversableHandle

optixGetInstanceTraversableFromIAS (

OptixTraversableHandle *ias,*

unsigned int *instIdx*) *[static]*

Return the traversable handle of a given instance in an Instance Acceleration Structure (IAS)

To obtain instance traversables by index, the IAS must be built using the flag OPTIX_BUILD_FLAG_
ALLOW_RANDOM_INSTANCE_ACCESS.

Available in all OptiX program types

## 5.1.2.51 optixGetLaunchDimensions( )

static __forceinline__ __device__ uint3 optixGetLaunchDimensions () *[static]*
Available in any program, it returns the dimensions of the current launch specified by optixLaunch on
the host.

Available in all OptiX program types

## 5.1.2.52 optixGetLaunchIndex( )

static __forceinline__ __device__ uint3 optixGetLaunchIndex () *[static]*

---

Available in any program, it returns the current launch index within the launch dimensions specified
by optixLaunch on the host.

The raygen program is typically only launched once per launch index.

Available in all OptiX program types

## 5.1.2.53 optixGetLinearCurveVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetLinearCurveVertexData (
float4 *data[2]*) *[static]*

Returns the object space control vertex data of the currently intersected linear curve at the current ray
time.

Similar to the random access variant optixGetLinearCurveVertexDataFromHandle, but does not
require setting flag OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS when building the
corresponding GAS.

It is only valid to call this function if the return value of optixGetPrimitiveType(optixGetHitKind())
equals OPTIX_PRIMITIVE_TYPE_ROUND_LINEAR.

Available in AH, CH

## 5.1.2.54 optixGetLinearCurveVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetLinearCurveVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[2]*) *[static]*

Deprecated. Call eitheroptixGetLinearCurveVertexData(float4 data[2])for a current-hit data fetch, or
optixGetLinearCurveVertexDataFromHandle(...) for a random-access data fetch.

Returns the object space curve control vertex data of a linear curve in a Geometry Acceleration
Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.55 optixGetLinearCurveVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetLinearCurveVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,* float4 *data[2]*) *[static]*

Performs a random access fetch of the object space curve control vertex data of a linear curve in a
Geometry Acceleration Structure (GAS) at a given motion time.

To access vertex data of any curve, the GAS must be built using the flag OPTIX_BUILD_FLAG_
ALLOW_RANDOM_VERTEX_ACCESS. If only the vertex data of a currently intersected linear curve
is required, it is recommended to use function optixGetLinearCurveVertexData. A data fetch of the
currently hit primitive does NOT require building the corresponding GAS with flag OPTIX_BUILD_
FLAG_ALLOW_RANDOM_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.56 optixGetMatrixMotionTransformFromHandle( )

static __forceinline__ __device__ constOptixMatrixMotionTransform *∗*
optixGetMatrixMotionTransformFromHandle (
OptixTraversableHandle *handle*) *[static]*

Returns a pointer to aOptixMatrixMotionTransformfrom its traversable handle.

Returns 0 if the traversable is not of type OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_
TRANSFORM.

Available in all OptiX program types

## 5.1.2.57 optixGetObjectRayDirection( )

static __forceinline__ __device__ float3 optixGetObjectRayDirection ()
*[static]*

Returns the current object space ray direction based on the current transform stack.

Available in IS and AH

## 5.1.2.58 optixGetObjectRayOrigin( )

static __forceinline__ __device__ float3 optixGetObjectRayOrigin () *[static]*
Returns the current object space ray origin based on the current transform stack.

Available in IS and AH

## 5.1.2.59 optixGetObjectToWorldTransformMatrix( ) [1/2]

template<typename HitState >
static __forceinline__ __device__ void optixGetObjectToWorldTransformMatrix
(
const HitState & *hs,*
float *m[12]*) *[static]*

Returns the object-to-world transformation matrix resulting from the transformation list of the
templated hit object (see optixGetWorldToObjectTransformMatrix for example usage).

The cost of this function may be proportional to the size of the transformation list.

---

Available in IS, AH, CH

## 5.1.2.60 optixGetObjectToWorldTransformMatrix( ) [2/2]

static __forceinline__ __device__ void optixGetObjectToWorldTransformMatrix
(

float *m[12]*) *[static]*

Returns the object-to-world transformation matrix resulting from the current active transformation list.

The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.61 optixGetPayload_0( )

static __forceinline__ __device__ unsigned int optixGetPayload_0 () *[static]*

Returns the 32-bit payload at the given slot index. There are up to 32 attributes available. The number
of attributes is configured withOptixPipelineCompileOptions::numPayloadValuesor with
OptixPayloadTypeparameters set inOptixModuleCompileOptions.

Available in IS, AH, CH, MS

## 5.1.2.62 optixGetPayload_1( )

static __forceinline__ __device__ unsigned int optixGetPayload_1 () *[static]*

## 5.1.2.63 optixGetPayload_10( )

static __forceinline__ __device__ unsigned int optixGetPayload_10 () *[static]*

## 5.1.2.64 optixGetPayload_11( )

static __forceinline__ __device__ unsigned int optixGetPayload_11 () *[static]*

## 5.1.2.65 optixGetPayload_12( )

static __forceinline__ __device__ unsigned int optixGetPayload_12 () *[static]*

## 5.1.2.66 optixGetPayload_13( )

static __forceinline__ __device__ unsigned int optixGetPayload_13 () *[static]*

## 5.1.2.67 optixGetPayload_14( )

static __forceinline__ __device__ unsigned int optixGetPayload_14 () *[static]*

## 5.1.2.68 optixGetPayload_15( )

static __forceinline__ __device__ unsigned int optixGetPayload_15 () *[static]*

## 5.1.2.69 optixGetPayload_16( )

static __forceinline__ __device__ unsigned int optixGetPayload_16 () *[static]*

---

5.1.2.70 optixGetPayload_17()
static __forceinline__ __device__ unsigned int optixGetPayload_17 ( ) [static]

5.1.2.71 optixGetPayload_18()
static __forceinline__ __device__ unsigned int optixGetPayload_18 ( ) [static]

5.1.2.72 optixGetPayload_19()
static __forceinline__ __device__ unsigned int optixGetPayload_19 ( ) [static]

5.1.2.73 optixGetPayload_2()
static __forceinline__ __device__ unsigned int optixGetPayload_2 ( ) [static]

5.1.2.74 optixGetPayload_20()
static __forceinline__ __device__ unsigned int optixGetPayload_20 ( ) [static]

5.1.2.75 optixGetPayload_21()
static __forceinline__ __device__ unsigned int optixGetPayload_21 ( ) [static]

5.1.2.76 optixGetPayload_22()
static __forceinline__ __device__ unsigned int optixGetPayload_22 ( ) [static]

5.1.2.77 optixGetPayload_23()
static __forceinline__ __device__ unsigned int optixGetPayload_23 ( ) [static]

5.1.2.78 optixGetPayload_24()
static __forceinline__ __device__ unsigned int optixGetPayload_24 ( ) [static]

5.1.2.79 optixGetPayload_25()
static __forceinline__ __device__ unsigned int optixGetPayload_25 ( ) [static]

5.1.2.80 optixGetPayload_26()
static __forceinline__ __device__ unsigned int optixGetPayload_26 ( ) [static]

5.1.2.81 optixGetPayload_27()
static __forceinline__ __device__ unsigned int optixGetPayload_27 ( ) [static]

5.1.2.82 optixGetPayload_28()
static __forceinline__ __device__ unsigned int optixGetPayload_28 ( ) [static]

## 5.1.2.83 optixGetPayload_29()
static __forceinline__ __device__ unsigned int optixGetPayload_29 ( ) [static]

---

5.1.2.84 optixGetPayload_3( )

static __forceinline__ __device__ unsigned int optixGetPayload_3 () *[static]*

5.1.2.85 optixGetPayload_30( )

static __forceinline__ __device__ unsigned int optixGetPayload_30 () *[static]*

5.1.2.86 optixGetPayload_31( )

static __forceinline__ __device__ unsigned int optixGetPayload_31 () *[static]*

5.1.2.87 optixGetPayload_4( )

static __forceinline__ __device__ unsigned int optixGetPayload_4 () *[static]*

5.1.2.88 optixGetPayload_5( )

static __forceinline__ __device__ unsigned int optixGetPayload_5 () *[static]*

5.1.2.89 optixGetPayload_6( )

static __forceinline__ __device__ unsigned int optixGetPayload_6 () *[static]*

5.1.2.90 optixGetPayload_7( )

static __forceinline__ __device__ unsigned int optixGetPayload_7 () *[static]*

5.1.2.91 optixGetPayload_8( )

static __forceinline__ __device__ unsigned int optixGetPayload_8 () *[static]*

5.1.2.92 optixGetPayload_9( )

static __forceinline__ __device__ unsigned int optixGetPayload_9 () *[static]*

## 5.1.2.93 optixGetPrimitiveIndex( )

static __forceinline__ __device__ unsigned int optixGetPrimitiveIndex ()
*[static]*

For a givenOptixBuildInputTriangleArraythe number of primitives is defined as.

"(OptixBuildInputTriangleArray::indexBuffer == 0) ? OptixBuildInputTriangleArray::numVertices/3 :
OptixBuildInputTriangleArray::numIndexTriplets;".

For a givenOptixBuildInputCustomPrimitiveArraythe number of primitives is defined as numAabbs.

In IS and AH this corresponds to the currently intersected primitive.

The primitive index returns the index into the array of primitives plus the primitiveIndexOffset.

In CH this corresponds to the primitive index of the closest intersected primitive.

Available in IS, AH, CH

## 5.1.2.94 optixGetPrimitiveType( ) [1/2]

static __forceinline__ __device__OptixPrimitiveTypeoptixGetPrimitiveType (
) *[static]*

---

Function interpreting the hit kind associated with the current optixReportIntersection.

Available in AH, CH

## 5.1.2.95 optixGetPrimitiveType( ) [2/2]

static __forceinline__ __device__OptixPrimitiveTypeoptixGetPrimitiveType (
unsigned int *hitKind*) *[static]*

Function interpreting the result ofoptixGetHitKind( ).

Available in all OptiX program types

## 5.1.2.96 optixGetQuadraticBSplineRocapsVertexData( )

static __forceinline__ __device__ void
optixGetQuadraticBSplineRocapsVertexData (
float4 *data[3]*) *[static]*

## 5.1.2.97 optixGetQuadraticBSplineRocapsVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetQuadraticBSplineRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*

## 5.1.2.98 optixGetQuadraticBSplineVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetQuadraticBSplineVertexData (
float4 *data[3]*) *[static]*

Returns the object space curve control vertex data of a quadratic BSpline curve in a Geometry
Acceleration Structure (GAS) at a given motion time.

Available in AH, CH

## 5.1.2.99 optixGetQuadraticBSplineVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetQuadraticBSplineVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*

Returns the object space curve control vertex data of a quadratic BSpline curve in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

---

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.100 optixGetQuadraticBSplineVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetQuadraticBSplineVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*

Returns the object space curve control vertex data of a quadratic BSpline curve in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.101 optixGetRayFlags( )

static __forceinline__ __device__ unsigned int optixGetRayFlags () *[static]*

Returns the rayFlags passed into optixTrace.

Available in IS, AH, CH, MS

## 5.1.2.102 optixGetRayTime( )

static __forceinline__ __device__ float optixGetRayTime () *[static]*

Returns the rayTime passed into optixTrace.

Returns 0 if motion is disabled.

Available in IS, AH, CH, MS

## 5.1.2.103 optixGetRayTmax( )

static __forceinline__ __device__ float optixGetRayTmax () *[static]*

In IS and CH returns the current smallest reported hitT or the tmax passed into optixTrace if no hit has
been reported.

In AH returns the hitT value as passed in to optixReportIntersection

In MS returns the tmax passed into optixTrace

Available in IS, AH, CH, MS

---

## 5.1.2.104 optixGetRayTmin( )

static __forceinline__ __device__ float optixGetRayTmin () *[static]*

Returns the tmin passed into optixTrace.

Available in IS, AH, CH, MS

## 5.1.2.105 optixGetRayVisibilityMask( )

static __forceinline__ __device__ unsigned int optixGetRayVisibilityMask ()
*[static]*

Returns the visibilityMask passed into optixTrace.

Available in IS, AH, CH, MS

## 5.1.2.106 optixGetRemainingTraceDepth( )

static __forceinline__ __device__ unsigned int optixGetRemainingTraceDepth (
) *[static]*

If non-zero it is legal to call optixTrace or optixTraverse without triggering an OPTIX_EXCEPTION_
CODE_TRACE_DEPTH_EXCEEDED exception. In the case of optixTrace it represents the number of
recursive calls that are remaining and counts down.

Value is in the range of [0..OptixPipelineLinkOptions::maxTraceDepth], and maxTraceDepth has a
maximum value of 31.

Available in RG, CH, MS, CC, DC

## 5.1.2.107 optixGetRibbonNormal( ) [1/2]

static __forceinline__ __device__ float3 optixGetRibbonNormal (
float2 *ribbonParameters*) *[static]*

Return ribbon normal at intersection reported by optixReportIntersection.

Available in AH, CH

## 5.1.2.108 optixGetRibbonNormal( ) [2/2]

static __forceinline__ __device__ float3 optixGetRibbonNormal (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float2 *ribbonParameters*) *[static]*

Deprecated. Call eitheroptixGetRibbonNormal(float2 ribbonParameters)for current hit data, or
optixGetRibbonNormalFromHandle( )for random access.

Returns ribbon normal at intersection reported by optixReportIntersection.

Available in all OptiX program types

## 5.1.2.109 optixGetRibbonNormalFromHandle( )

static __forceinline__ __device__ float3 optixGetRibbonNormalFromHandle (

---

OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float2 *ribbonParameters*) *[static]*

Returns ribbon normal at intersection reported by optixReportIntersection.

Available in all OptiX program types

## 5.1.2.110 optixGetRibbonParameters( )

static __forceinline__ __device__ float2 optixGetRibbonParameters () *[static]*

Returns the ribbon parameters along directrix (length) and generator (width) of the current intersection
when usingOptixBuildInputCurveArrayobjects with curveType OPTIX_PRIMITIVE_TYPE_FLAT_
QUADRATIC_BSPLINE.

Available in AH, CH

## 5.1.2.111 optixGetRibbonVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetRibbonVertexData (
float4 *data[3]*) *[static]*

Returns the object space curve control vertex data of a ribbon (flat quadratic BSpline) in a Geometry
Acceleration Structure (GAS) at a given motion time.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

Available in AH, CH

## 5.1.2.112 optixGetRibbonVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetRibbonVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*

Deprecated. Call eitheroptixGetRibbonVertexData(float4 data[3])for current hit data, or
optixGetRibbonVertexDataFromHandle( )for random access.

Returns the object space curve control vertex data of a ribbon (flat quadratic BSpline) in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

---

## 5.1.2.113 optixGetRibbonVertexDataFromHandle( )

static __forceinline__ __device__ void optixGetRibbonVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*

Returns the object space curve control vertex data of a ribbon (flat quadratic BSpline) in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.114 optixGetSbtDataPointer( )

static __forceinline__ __device__CUdeviceptroptixGetSbtDataPointer ()
*[static]*

Returns the generic memory space pointer to the data region (past the header) of the currently active
SBT record corresponding to the current program.

Note that optixGetSbtDataPointer is not available in OptiX-enabled functions, because there is no SBT
entry associated with the function.

Available in RG, IS, AH, CH, MS, EX, DC, CC

## 5.1.2.115 optixGetSbtGASIndex( )

static __forceinline__ __device__ unsigned int optixGetSbtGASIndex () *[static]*
Returns the Sbt GAS index of the primitive associated with the current intersection.

In IS and AH this corresponds to the currently intersected primitive.

In CH this corresponds to the SBT GAS index of the closest intersected primitive.

Available in IS, AH, CH

## 5.1.2.116 optixGetSphereData( ) [1/2]

static __forceinline__ __device__ void optixGetSphereData (
float4 *data[1]*) *[static]*

Returns the object space sphere data of the currently intersected sphere at the current ray time.

Similar to the random access variant optixGetSphereDataFromHandle, but does not require setting flag
OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS when building the corresponding
GAS.

It is only valid to call this function if the return value of optixGetPrimitiveType(optixGetHitKind())
equals OPTIX_PRIMITIVE_TYPE_SPHERE.

Available in AH, CH

---

## 5.1.2.117 optixGetSphereData( ) [2/2]

static __forceinline__ __device__ void optixGetSphereData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[1]*) *[static]*

Deprecated. Call eitheroptixGetSphereData(float4 data[1])for current hit sphere data, or
optixGetSphereDataFromHandle( )for random access sphere data.

Returns the object space sphere data, center point and radius, in a Geometry Acceleration Structure
(GAS) at a given motion time.

To access sphere data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

data[0] = {x,y,z,w} with {x,y,z} the position of the sphere center and w the radius.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.118 optixGetSphereDataFromHandle( )

static __forceinline__ __device__ void optixGetSphereDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[1]*) *[static]*

Performs a random access fetch of the object space sphere data, center point and radius, in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data of any curve, the GAS must be built using the flag OPTIX_BUILD_FLAG_
ALLOW_RANDOM_VERTEX_ACCESS. If only the vertex data of a currently intersected sphere is
required, it is recommended to use function optixGetSphereData. A data fetch of the currently hit
primitive does NOT require building the corresponding GAS with flag OPTIX_BUILD_FLAG_ALLOW
_RANDOM_VERTEX_ACCESS.

data[0] = {x,y,z,w} with {x,y,z} the position of the sphere center and w the radius.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.119 optixGetSRTMotionTransformFromHandle( )

static __forceinline__ __device__ constOptixSRTMotionTransform *∗*
optixGetSRTMotionTransformFromHandle (
OptixTraversableHandle *handle*) *[static]*

Returns a pointer to aOptixSRTMotionTransformfrom its traversable handle.

---

Returns 0 if the traversable is not of type OPTIX_TRANSFORM_TYPE_SRT_MOTION_TRANSFORM.
Available in all OptiX program types

## 5.1.2.120 optixGetStaticTransformFromHandle( )

static __forceinline__ __device__ constOptixStaticTransform *∗*

optixGetStaticTransformFromHandle (

OptixTraversableHandle *handle*) *[static]*

Returns a pointer to aOptixStaticTransformfrom its traversable handle.

Returns 0 if the traversable is not of type OPTIX_TRANSFORM_TYPE_STATIC_TRANSFORM.

Available in all OptiX program types

## 5.1.2.121 optixGetTransformListHandle( )

static __forceinline__ __device__OptixTraversableHandle

optixGetTransformListHandle (

unsigned int *index*) *[static]*

Returns the traversable handle for a transform in the current transform list.

Available in IS, AH, CH

## 5.1.2.122 optixGetTransformListSize( )

static __forceinline__ __device__ unsigned int optixGetTransformListSize ()
*[static]*

Returns the number of transforms on the current transform list.

Available in IS, AH, CH

## 5.1.2.123 optixGetTransformTypeFromHandle( )

static __forceinline__ __device__OptixTransformType

optixGetTransformTypeFromHandle (

OptixTraversableHandle *handle*) *[static]*

Returns the transform type of a traversable handle from a transform list.

Available in all OptiX program types

## 5.1.2.124 optixGetTriangleBarycentrics( )

static __forceinline__ __device__ float2 optixGetTriangleBarycentrics ()
*[static]*

Convenience function that returns the first two attributes as floats.

When usingOptixBuildInputTriangleArrayobjects, during intersection with a triangle, the barycentric
coordinates of the hit are stored into the first two attribute registers.

Available in AH, CH

## 5.1.2.125 optixGetTriangleVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetTriangleVertexData (

---

<u>37</u>

float3 *data[3]*) *[static]*

Returns the object space triangle vertex positions of the currently intersected triangle at the current ray
time.

Similar to the random access variant optixGetTriangleVertexDataFromHandle, but does not require
setting flag OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS when building the
corresponding GAS.

It is only valid to call this function if the return value of optixGetPrimitiveType(optixGetHitKind())
equals OPTIX_PRIMITIVE_TYPE_TRIANGLE.

Available in AH, CH

## 5.1.2.126 optixGetTriangleVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetTriangleVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float3 *data[3]*) *[static]*

[DEPRECATED] Returns the object space triangle vertex positions of a given triangle in a Geometry
Acceleration Structure (GAS) at a given motion time. This function is deprecated, use
optixGetTriangleVertexDataFromHandle for random access triangle vertex data fetch or the overload
optixGetTriangleVertexData(float3 data[3])for a current triangle hit vertex data fetch.

To access vertex data, the GAS must be built using the flag OPTIX_BUILD_FLAG_ALLOW_RANDOM
_VERTEX_ACCESS.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

Available in all OptiX program types

## 5.1.2.127 optixGetTriangleVertexDataFromHandle( )

static __forceinline__ __device__ void optixGetTriangleVertexDataFromHandle
(

Performs a random access data fetch object space vertex position of a given triangle in a Geometry
Acceleration Structure (GAS) at a given motion time.

To access vertex data of any triangle, the GAS must be built using the flag OPTIX_BUILD_FLAG_
ALLOW_RANDOM_VERTEX_ACCESS. If only the vertex data of a currently intersected triangle is
required, it is recommended to use function optixGetTriangleVertexData. A data fetch of the currently
hit primitive does NOT require building the corresponding GAS with flag OPTIX_BUILD_FLAG_
ALLOW_RANDOM_VERTEX_ACCESS.

If motion is disabled viaOptixPipelineCompileOptions::usesMotionBlur, or the GAS does not contain
motion, the time parameter is ignored.

---

<u>38</u>

Available in all OptiX program types

## 5.1.2.128 optixGetWorldRayDirection( )

static __forceinline__ __device__ float3 optixGetWorldRayDirection () *[static]*
Returns the rayDirection passed into optixTrace.

May be more expensive to call in IS and AH than their object space counterparts, so effort should be
made to use the object space ray in those programs.

Available in IS, AH, CH, MS

## 5.1.2.129 optixGetWorldRayOrigin( )

static __forceinline__ __device__ float3 optixGetWorldRayOrigin () *[static]*
Returns the rayOrigin passed into optixTrace.

May be more expensive to call in IS and AH than their object space counterparts, so effort should be
made to use the object space ray in those programs.

Available in IS, AH, CH, MS

## 5.1.2.130 optixGetWorldToObjectTransformMatrix( ) [1/2]

template<typename HitState >

static __forceinline__ __device__ void optixGetWorldToObjectTransformMatrix
(

Returns the world-to-object transformation matrix resulting from the transformation list of the
templated hit object. Users may implement getRayTime, getTransformListSize, and
getTransformListHandle in their own structs, or inherit them from Optix[Incoming*|*
Outgoing]HitObject. Here is an example:

struct FixedTimeHitState :OptixIncomingHitObject{ float time; forceinline device float getRayTime() {
return time; } }; ... optixGetWorldToObjectTransformMatrix(FixedTimeHitState{ 0.4f }, m);

The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.131 optixGetWorldToObjectTransformMatrix( ) [2/2]

static __forceinline__ __device__ void optixGetWorldToObjectTransformMatrix
(

float *m[12]*) *[static]*

Returns the world-to-object transformation matrix resulting from the current active transformation list.
The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.132 optixHitObjectGetAttribute_0( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_0
() *[static]*

---

<u>39</u>

Return the attribute at the given slot index for the current outgoing hit object. There are up to 8
attributes available. The number of attributes is configured withOptixPipelineCompileOptions
::numAttributeValues.

Results are undefined if the hit object is a miss.

Available in RG, CH, MS, CC, DC

## 5.1.2.133 optixHitObjectGetAttribute_1( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_1
() *[static]*

## 5.1.2.134 optixHitObjectGetAttribute_2( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_2
() *[static]*

## 5.1.2.135 optixHitObjectGetAttribute_3( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_3
() *[static]*

## 5.1.2.136 optixHitObjectGetAttribute_4( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_4
() *[static]*

## 5.1.2.137 optixHitObjectGetAttribute_5( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_5
() *[static]*

## 5.1.2.138 optixHitObjectGetAttribute_6( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_6
() *[static]*

## 5.1.2.139 optixHitObjectGetAttribute_7( )

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_7
() *[static]*

## 5.1.2.140 optixHitObjectGetCatmullRomRocapsVertexData( )

static __forceinline__ __device__ void
optixHitObjectGetCatmullRomRocapsVertexData (
float4 *data[4]*) *[static]*

## 5.1.2.141 optixHitObjectGetCatmullRomVertexData( )

static __forceinline__ __device__ void optixHitObjectGetCatmullRomVertexData
(

## float4 data[4]) [static]

Returns the object space curve control vertex data of a CatmullRom spline curve for a valid outgoing

---

<u>40</u>

hit object.
data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.
It is only valid to call this function if the return value of
optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_ROUND_
CATMULLROM.

Available in RG, CH, MS, CC, DC

## 5.1.2.142 optixHitObjectGetClusterId( )

static __forceinline__ __device__ unsigned int optixHitObjectGetClusterId (
) *[static]*

Returns the user-provided cluster ID associated with the current outgoing hit object.

Returns OPTIX_CLUSTER_ID_INVALID if the closest intersection is not a cluster, or if the hit object is
a miss.

see alsoOptixPipelineCompileOptions::allowClusteredGeometry

Available in RG, CH, MS, CC, DC

## 5.1.2.143 optixHitObjectGetCubicBezierRocapsVertexData( )

static __forceinline__ __device__ void
optixHitObjectGetCubicBezierRocapsVertexData (
float4 *data[4]*) *[static]*

## 5.1.2.144 optixHitObjectGetCubicBezierVertexData( )

static __forceinline__ __device__ void
optixHitObjectGetCubicBezierVertexData (
float4 *data[4]*) *[static]*
Returns the object space curve control vertex data of a cubic Bezier curve for a valid outgoing hit object.
data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.
It is only valid to call this function if the return value of
optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_ROUND_
CUBIC_BEZIER.

Available in RG, CH, MS, CC, DC

## 5.1.2.145 optixHitObjectGetCubicBSplineRocapsVertexData( )

static __forceinline__ __device__ void
optixHitObjectGetCubicBSplineRocapsVertexData (
float4 *data[4]*) *[static]*
SeeoptixHitObjectGetCubicBSplineVertexDatafor further documentation.
It is only valid to call this function if the return value of
optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_ROUND_
CUBIC_BSPLINE_ROCAPS.
Available in RG, CH, MS, CC, DC

---

<u>41</u>

## 5.1.2.146 optixHitObjectGetCubicBSplineVertexData( )

static __forceinline__ __device__ void
optixHitObjectGetCubicBSplineVertexData (
float4 *data[4]*) *[static]*

Returns the object space curve control vertex data of a cubic BSpline curve for a valid outgoing hit
object.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

It is only valid to call this function if the return value of

optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_ROUND_
CUBIC_BSPLINE.

Available in RG, CH, MS, CC, DC

## 5.1.2.147 optixHitObjectGetCurveParameter( )

static __forceinline__ __device__ float optixHitObjectGetCurveParameter ()
*[static]*

Returns the curve parameter associated with the intersection of a curve.

This function is the hit object's equivalent tooptixGetCurveParameter( ). It is only valid to call this
function if the return value of optixGetPrimitiveType(optixHitObjectGetHitKind()) equals a primitive
type that can be used to build an AS withOptixBuildInputCurveArrayobjects.

Available in RG, CH, MS, CC, DC

## 5.1.2.148 optixHitObjectGetGASTraversableHandle( )

static __forceinline__ __device__OptixTraversableHandle
optixHitObjectGetGASTraversableHandle () *[static]*

Returns the traversable handle for the Geometry Acceleration Structure (GAS) associated with the
current outgoing hit object. Returns 0 if the hit object is not a hit.

Available in RG, CH, MS, CC, DC

## 5.1.2.149 optixHitObjectGetHitKind( )

static __forceinline__ __device__ unsigned int optixHitObjectGetHitKind ()
*[static]*

Returns the 8 bit hit kind associated with the current outgoing hit object.

Results are undefined if the hit object is a miss.

SeeoptixGetHitKind( ).

Available in RG, CH, MS, CC, DC

## 5.1.2.150 optixHitObjectGetInstanceId( )

static __forceinline__ __device__ unsigned int optixHitObjectGetInstanceId (
) *[static]*

Returns theOptixInstance::instanceIdof the instance within the top level acceleration structure
associated with the outgoing hit object.

Results are undefined if the hit object is a miss.

---

<u>42</u>

SeeoptixGetInstanceId( ).
Available in RG, CH, MS, CC, DC

## 5.1.2.151 optixHitObjectGetInstanceIndex( )

static __forceinline__ __device__ unsigned int
optixHitObjectGetInstanceIndex () *[static]*

Returns the zero-based index of the instance within its instance acceleration structure associated with
the outgoing hit object.

Results are undefined if the hit object is a miss.

SeeoptixGetInstanceIndex( ).

Available in RG, CH, MS, CC, DC

## 5.1.2.152 optixHitObjectGetLinearCurveVertexData( )

static __forceinline__ __device__ void
optixHitObjectGetLinearCurveVertexData (
float4 *data[2]*) *[static]*
Returns the object space control vertex data of the currently intersected linear curve for a valid
outgoing hit object. It is the hit object's pendant ofoptixGetLinearCurveVertexData(float4 data[2]).
It is only valid to call this function if the return value of
optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_ROUND_
LINEAR.

Available in RG, CH, MS, CC, DC

## 5.1.2.153 optixHitObjectGetObjectToWorldTransformMatrix( )

static __forceinline__ __device__ void

optixHitObjectGetObjectToWorldTransformMatrix (
float *m[12]*) *[static]*

Returns the object-to-world transformation matrix resulting from the transformation list of the current
outgoing hit object.

The cost of this function may be proportional to the size of the transformation list.

Available in RG, CH, MS, CC, DC

## 5.1.2.154 optixHitObjectGetPrimitiveIndex( )

static __forceinline__ __device__ unsigned int
optixHitObjectGetPrimitiveIndex () *[static]*
Return the primitive index associated with the current outgoing hit object.
Results are undefined if the hit object is a miss.
SeeoptixGetPrimitiveIndex( )for more details.
Available in RG, CH, MS, CC, DC

---

<u>43</u>

## 5.1.2.155 optixHitObjectGetQuadraticBSplineRocapsVertexData( )

static __forceinline__ __device__ void
optixHitObjectGetQuadraticBSplineRocapsVertexData (
float4 *data[3]*) *[static]*

## 5.1.2.156 optixHitObjectGetQuadraticBSplineVertexData( )

static __forceinline__ __device__ void

optixHitObjectGetQuadraticBSplineVertexData (
float4 *data[3]*) *[static]*

Returns the object space curve control vertex data of a quadratic BSpline curve for a valid outgoing hit
object.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

It is only valid to call this function if the return value of

optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_FLAT_
QUADRATIC_BSPLINE.

Available in RG, CH, MS, CC, DC

## 5.1.2.157 optixHitObjectGetRayFlags( )

static __forceinline__ __device__ unsigned int optixHitObjectGetRayFlags ()
*[static]*

Returns the rayFlags passed into optixTrace associated with the current outgoing hit object.

Available in RG, CH, MS, CC, DC

## 5.1.2.158 optixHitObjectGetRayTime( )

static __forceinline__ __device__ float optixHitObjectGetRayTime () *[static]*
Returns the rayTime passed into optixTraverse, optixMakeHitObject or optixMakeMissHitObject.

Returns 0 for nop hit objects or when motion is disabled.

Available in RG, CH, MS, CC, DC

## 5.1.2.159 optixHitObjectGetRayTmax( )

static __forceinline__ __device__ float optixHitObjectGetRayTmax () *[static]*
If the hit object is a hit, returns the smallest reported hitT.

If the hit object is a miss, returns the tmax passed into optixTraverse, optixMakeHitObject or
optixMakeMissHitObject.

Returns 0 for nop hit objects.

Available in RG, CH, MS, CC, DC

## 5.1.2.160 optixHitObjectGetRayTmin( )

static __forceinline__ __device__ float optixHitObjectGetRayTmin () *[static]*
Returns the tmin passed into optixTraverse, optixMakeHitObject or optixMakeMissHitObject.

Returns 0.0f for nop hit objects.

---

<u>44</u>

Available in RG, CH, MS, CC, DC

## 5.1.2.161 optixHitObjectGetRibbonNormal( )

static __forceinline__ __device__ float3 optixHitObjectGetRibbonNormal (
float2 *ribbonParameters*) *[static]*

Return ribbon normal at intersection reported by optixReportIntersection.

Available in RG, CH, MS, CC, DC

## 5.1.2.162 optixHitObjectGetRibbonParameters( )

static __forceinline__ __device__ float2 optixHitObjectGetRibbonParameters (
) *[static]*

Returns the ribbon parameters along directrix (length) and generator (width) of the current curve
intersection with primitive type OPTIX_PRIMITIVE_TYPE_FLAT_QUADRATIC_BSPLINE.

This function is the hit object's equivalent tooptixGetRibbonParameters( ). It is only valid to call this
function if the return value of optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_
PRIMITIVE_TYPE_FLAT_QUADRATIC_BSPLINE.

Available in RG, CH, MS, CC, DC

## 5.1.2.163 optixHitObjectGetRibbonVertexData( )

static __forceinline__ __device__ void optixHitObjectGetRibbonVertexData (
float4 *data[3]*) *[static]*

Returns the object space curve control vertex data of a ribbon (flat quadratic BSpline) for a valid
outgoing hit object.

data[i] = {x,y,z,w} with {x,y,z} the position and w the radius of control vertex i.

It is only valid to call this function if the return value of

optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_FLAT_
QUADRATIC_BSPLINE.

Available in RG, CH, MS, CC, DC

## 5.1.2.164 optixHitObjectGetSbtDataPointer( )

static __forceinline__ __device__CUdeviceptr

optixHitObjectGetSbtDataPointer () *[static]*

Device pointer address for the SBT associated with the hit or miss program for the current outgoing hit
object.

Returns 0 for nop hit objects.

Available in RG, CH, MS, CC, DC

## 5.1.2.165 optixHitObjectGetSbtGASIndex( )

static __forceinline__ __device__ unsigned int optixHitObjectGetSbtGASIndex
() *[static]*

Return the SBT GAS index of the closest intersected primitive associated with the current outgoing hit
object.

Results are undefined if the hit object is a miss.

---

SeeoptixGetSbtGASIndex( )for details on the version for the incoming hit object.

Available in RG, CH, MS, CC, DC

## 5.1.2.166 optixHitObjectGetSbtRecordIndex( )

static __forceinline__ __device__ unsigned int
optixHitObjectGetSbtRecordIndex () *[static]*

Returns the SBT record index associated with the hit or miss program for the current outgoing hit
object.

Returns 0 for nop hit objects.

## 5.1.2.167 optixHitObjectGetSphereData( )

Available in RG, CH, MS, CC, DC

static __forceinline__ __device__ void optixHitObjectGetSphereData (
float4 *data[1]*) *[static]*

Returns the object space sphere data of the currently intersected sphere for a valid outgoing hit object.
It is the hit object's pendant ofoptixGetSphereData(float4 data[1]).

It is only valid to call this function if the return value of

optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_SPHERE.

Available in RG, CH, MS, CC, DC

## 5.1.2.168 optixHitObjectGetTransformListHandle( )

static __forceinline__ __device__OptixTraversableHandle

optixHitObjectGetTransformListHandle (

unsigned int *index*) *[static]*

Returns the traversable handle for a transform in the current transform list associated with the
outgoing hit object.

Results are undefined if the hit object is a miss.

SeeoptixGetTransformListHandle( )

Available in RG, CH, MS, CC, DC

## 5.1.2.169 optixHitObjectGetTransformListSize( )

static __forceinline__ __device__ unsigned int
optixHitObjectGetTransformListSize () *[static]*

Returns the number of transforms associated with the current outgoing hit object's transform list.

Returns zero when there is no hit (miss and nop).

SeeoptixGetTransformListSize( )

Available in RG, CH, MS, CC, DC

## 5.1.2.170 optixHitObjectGetTraverseData( )

static __forceinline__ __device__ void optixHitObjectGetTraverseData (
OptixTraverseData *∗ data*) *[static]*

---

Serializes the current outgoing hit object which allows to recreate it at a later point using
optixMakeHitObject.

Parameters

| out | data |
| --- | --- |

Available in RG, CH, MS, CC, DC

## 5.1.2.171 optixHitObjectGetTriangleBarycentrics( )

static __forceinline__ __device__ float2
optixHitObjectGetTriangleBarycentrics () *[static]*

Returns the barycentric coordinates of the hit point on an intersected triangle.

This function is the hit object's equivalent tooptixGetTriangleBarycentrics( ). It is only valid to call this
function if the return value of optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_
PRIMITIVE_TYPE_TRIANGLE.

Available in RG, CH, MS, CC, DC

## 5.1.2.172 optixHitObjectGetTriangleVertexData( )

static __forceinline__ __device__ void optixHitObjectGetTriangleVertexData (
float3 *data[3]*) *[static]*

Returns the object space triangle vertex positions of the intersected triangle for a valid outgoing hit
object. It is the hit object's pendant ofoptixGetTriangleVertexData(float3 data[3]).

It is only valid to call this function if the return value of

optixGetPrimitiveType(optixHitObjectGetHitKind()) equals OPTIX_PRIMITIVE_TYPE_TRIANGLE.

Available in RG, CH, MS, CC, DC

## 5.1.2.173 optixHitObjectGetWorldRayDirection( )

static __forceinline__ __device__ float3 optixHitObjectGetWorldRayDirection
() *[static]*

Returns the rayDirection passed into optixTraverse, optixMakeHitObject or optixMakeMissHitObject.

Returns [0, 0, 0] for nop hit objects.

Available in RG, CH, MS, CC, DC

## 5.1.2.174 optixHitObjectGetWorldRayOrigin( )

static __forceinline__ __device__ float3 optixHitObjectGetWorldRayOrigin ()
*[static]*

Returns the rayOrigin passed into optixTraverse, optixMakeHitObject or optixMakeMissHitObject.

Returns [0, 0, 0] for nop hit objects.

Available in RG, CH, MS, CC, DC

---

<u>47</u>

## 5.1.2.175 optixHitObjectGetWorldToObjectTransformMatrix( )

static __forceinline__ __device__ void
optixHitObjectGetWorldToObjectTransformMatrix (
float *m[12]*) *[static]*

Returns the world-to-object transformation matrix resulting from the transformation list of the current
outgoing hit object.

The cost of this function may be proportional to the size of the transformation list.

Available in RG, CH, MS, CC, DC

## 5.1.2.176 optixHitObjectIsHit( )

static __forceinline__ __device__ bool optixHitObjectIsHit () *[static]*

Returns true if the current outgoing hit object contains a hit.

## 5.1.2.177 optixHitObjectIsMiss( )

Available in RG, CH, MS, CC, DC

static __forceinline__ __device__ bool optixHitObjectIsMiss () *[static]*

Returns true if the current outgoing hit object contains a miss.

Available in RG, CH, MS, CC, DC

## 5.1.2.178 optixHitObjectIsNop( )

static __forceinline__ __device__ bool optixHitObjectIsNop () *[static]*

Returns true if the current outgoing hit object contains neither a hit nor miss. If executed with
optixInvoke, no operation will result. An implied nop hit object is always assumed to exist even if
there are no calls such as optixTraverse to explicitly create one.

Available in RG, CH, MS, CC, DC

## 5.1.2.179 optixHitObjectSetSbtRecordIndex( )

static __forceinline__ __device__ void optixHitObjectSetSbtRecordIndex (
unsigned int *sbtRecordIndex*) *[static]*

Sets the SBT record index in the current outgoing hit object.

Available in RG, CH, MS, CC, DC

## 5.1.2.180 optixHitObjectTransformNormalFromObjectToWorldSpace( )

static __forceinline__ __device__ float3

optixHitObjectTransformNormalFromObjectToWorldSpace (
float3 *normal*) *[static]*

Transforms the normal using object-to-world transformation matrix resulting from the transformation
list of the current outgoing hit object.

The cost of this function may be proportional to the size of the transformation list.

Available in RG, CH, MS, CC, DC

---

<u>48</u>

## 5.1.2.181 optixHitObjectTransformNormalFromWorldToObjectSpace( )

static __forceinline__ __device__ float3
optixHitObjectTransformNormalFromWorldToObjectSpace (
float3 *normal*) *[static]*
Transforms the normal using world-to-object transformation matrix resulting from the transformation
list of the current outgoing hit object.
The cost of this function may be proportional to the size of the transformation list.
Available in RG, CH, MS, CC, DC

## 5.1.2.182 optixHitObjectTransformPointFromObjectToWorldSpace( )

static __forceinline__ __device__ float3
optixHitObjectTransformPointFromObjectToWorldSpace (
float3 *point*) *[static]*
Transforms the point using object-to-world transformation matrix resulting from the transformation
list of the current outgoing hit object.
The cost of this function may be proportional to the size of the transformation list.

Available in RG, CH, MS, CC, DC

## 5.1.2.183 optixHitObjectTransformPointFromWorldToObjectSpace( )

static __forceinline__ __device__ float3
optixHitObjectTransformPointFromWorldToObjectSpace (
float3 *point*) *[static]*
Transforms the point using world-to-object transformation matrix resulting from the transformation
list of the current outgoing hit object.
The cost of this function may be proportional to the size of the transformation list.

Available in RG, CH, MS, CC, DC

## 5.1.2.184 optixHitObjectTransformVectorFromObjectToWorldSpace( )

static __forceinline__ __device__ float3
optixHitObjectTransformVectorFromObjectToWorldSpace (
float3 *vec*) *[static]*
Transforms the vector using object-to-world transformation matrix resulting from the transformation
list of the current outgoing hit object.
The cost of this function may be proportional to the size of the transformation list.

Available in RG, CH, MS, CC, DC

## 5.1.2.185 optixHitObjectTransformVectorFromWorldToObjectSpace( )

static __forceinline__ __device__ float3
optixHitObjectTransformVectorFromWorldToObjectSpace (
float3 *vec*) *[static]*

Transforms the vector using world-to-object transformation matrix resulting from the transformation

---

49

list of the current outgoing hit object.

The cost of this function may be proportional to the size of the transformation list.

Available in RG, CH, MS, CC, DC

## 5.1.2.186 optixIgnoreIntersection( )

static __forceinline__ __device__ void optixIgnoreIntersection () *[static]*
Discards the hit, and returns control to the calling optixReportIntersection or built-in intersection
routine.

Available in AH

## 5.1.2.187 optixInvoke( ) [1/2]

template<typename... Payload>

static __forceinline__ __device__ void optixInvoke (
OptixPayloadTypeID *type,*
Payload &... *payload*) *[static]*

Invokes closesthit, miss or nop based on the current outgoing hit object. After execution the current
outgoing hit object will be set to nop. An implied nop hit object is always assumed to exist even if there
are no calls to optixTraverse, optixMakeMissHitObject, optixMakeHitObject or
optixMakeNopHitObject.

## Parameters

| in | type |  |
| --- | --- | --- |
| in,out | payload | up to 32 unsigned int values that hold the payload |

Available in RG, CH, MS, CC

## 5.1.2.188 optixInvoke( ) [2/2]

template<typename... Payload>
static __forceinline__ __device__ void optixInvoke (
Payload &... *payload*) *[static]*

Invokes closesthit, miss or nop based on the current outgoing hit object. After execution the current
outgoing hit object will be set to nop. An implied nop hit object is always assumed to exist even if there
are no calls to optixTraverse, optixMakeMissHitObject, optixMakeHitObject or
optixMakeNopHitObject.

## Parameters

| in,out | payload | up to 32 unsigned int values that hold the payload |
| --- | --- | --- |

Available in RG, CH, MS, CC

## 5.1.2.189 optixIsBackFaceHit( ) [1/2]

static __forceinline__ __device__ bool optixIsBackFaceHit () *[static]*
Function interpreting the hit kind associated with the current optixReportIntersection.

---

<u>50</u>

Available in AH, CH

## 5.1.2.190 optixIsBackFaceHit( ) [2/2]

static __forceinline__ __device__ bool optixIsBackFaceHit (
unsigned int *hitKind*) *[static]*

Function interpreting the result ofoptixGetHitKind( ).

Available in all OptiX program types

## 5.1.2.191 optixIsFrontFaceHit( ) [1/2]

static __forceinline__ __device__ bool optixIsFrontFaceHit () *[static]*

Function interpreting the hit kind associated with the current optixReportIntersection.

Available in AH, CH

## 5.1.2.192 optixIsFrontFaceHit( ) [2/2]

static __forceinline__ __device__ bool optixIsFrontFaceHit (
unsigned int *hitKind*) *[static]*

Function interpreting the result ofoptixGetHitKind( ).

Available in all OptiX program types

## 5.1.2.193 optixIsTriangleBackFaceHit( )

static __forceinline__ __device__ bool optixIsTriangleBackFaceHit () *[static]*
Convenience function interpreting the result ofoptixGetHitKind( ).

Available in AH, CH

## 5.1.2.194 optixIsTriangleFrontFaceHit( )

static __forceinline__ __device__ bool optixIsTriangleFrontFaceHit () *[static]*
Convenience function interpreting the result ofoptixGetHitKind( ).

Available in AH, CH

## 5.1.2.195 optixIsTriangleHit( )

static __forceinline__ __device__ bool optixIsTriangleHit () *[static]*

Convenience function interpreting the result ofoptixGetHitKind( ).

Available in AH, CH

## 5.1.2.196 optixMakeHitObject( )

static __forceinline__ __device__ void optixMakeHitObject (
OptixTraversableHandle *handle,*
float3 *rayOrigin,*
float3 *rayDirection,*
float *tmin,*
float *rayTime,*

---

51

unsigned int *rayFlags,*

OptixTraverseData *traverseData,*

constOptixTraversableHandle *∗ transforms,*

unsigned int *numTransforms*) *[static]*

Constructs an outgoing hit object from the hit object data provided. The traverseData needs to be
collected from a previous hit object usingoptixHitObjectGetTraverseData. This hit object will now
become the current outgoing hit object and will overwrite the current outgoing hit object.

## Parameters

| in | handle |  |
| --- | --- | --- |
| in | rayOrigin |  |
| in | rayDirection |  |
| in | tmin |  |
| in | rayTime |  |
| in | rayFlags | really only 16 bits, combination of OptixRayFlags |
| in | traverseData |  |
| in | transforms |  |
| in | numTransforms |  |

## Available in RG, CH, MS, CC

## 5.1.2.197 optixMakeMissHitObject( )

static __forceinline__ __device__ void optixMakeMissHitObject (

unsigned int *missSBTIndex,*

float3 *rayOrigin,*

float3 *rayDirection,*

float *tmin,*

float *tmax,*

float *rayTime,*

unsigned int *rayFlags*) *[static]*

Constructs an outgoing hit object from the miss information provided. The SBT record index is
explicitly specified as an argument. This hit object will now become the current outgoing hit object and
will overwrite the current outgoing hit object.

## Parameters

| in | missSBTIndex | specifies the miss program invoked on a miss |
| --- | --- | --- |
| in | rayOrigin |  |
| in | rayDirection |  |
| in | tmin |  |
| in | tmax |  |
| in | rayTime |  |
| in | rayFlags | really only 16 bits, combination of OptixRayFlags |

---

<u>52</u>

Available in RG, CH, MS, CC

## 5.1.2.198 optixMakeNopHitObject( )

static __forceinline__ __device__ void optixMakeNopHitObject () *[static]*

Constructs an outgoing hit object that when invoked does nothing (neither the miss nor the closest hit
shader will be invoked). This hit object will now become the current outgoing hit object and will
overwrite the current outgoing hit object. Accessors such asoptixHitObjectGetInstanceIdwill return 0
or 0 filled structs. OnlyoptixHitObjectIsNopwill return a non-zero result.

Available in RG, CH, MS, CC

## 5.1.2.199 optixReorder( ) [1/2]

static __forceinline__ __device__ void optixReorder () *[static]*

Reorder the current thread using the hit object only, ie without further coherence hints.

Available in RG

## 5.1.2.200 optixReorder( ) [2/2]

static __forceinline__ __device__ void optixReorder (
unsigned int *coherenceHint,*
unsigned int *numCoherenceHintBitsFromLSB*) *[static]*

Reorder the current thread using the current outgoing hit object and the coherence hint bits provided.
Note that the coherence hint will take away some of the bits used in the hit object for sorting, so care
should be made to reduce the number of hint bits as much as possible. Nop hit objects can use more
coherence hint bits. Bits are taken from the lowest significant bit range. The maximum value of
numCoherenceHintBitsFromLSB is implementation defined and can vary.

## Parameters

| in | coherenceHint |
| --- | --- |
| in | numCoherenceHintBitsFromLSB |

Available in RG

## 5.1.2.201 optixReportIntersection( ) [1/9]

$$
\begin{array}{l} \mathrm {s t a t i c} \quad \mathrm {f r o c e i n l i n e} _ {- -} \quad \mathrm {d e v i c e} _ {- -} \mathrm {b o o l} \mathrm {o p t i x R e p o r t I n t e r s e c t i o n} (\mathrm {f l o a t} \mathrm {h i t T}, \\ \mathrm {u n s s i g n e d i n t h i t K i n d}) [ \mathrm {s t a t i c} ] \\ \end{array}
$$

Reports an intersections (overload without attributes).

IfoptixGetRayTmin( ) <= hitT <=optixGetRayTmax( ), the any hit program associated with this
intersection program (via the SBT entry) is called.

The AH program can do one of three things:

1.call optixIgnoreIntersection - no hit is recorded, optixReportIntersection returns false

2.call optixTerminateRay - hit is recorded, optixReportIntersection does not return, no further
traversal occurs, and the associated closest hit program is called

3.neither - hit is recorded, optixReportIntersection returns true

---

<u>53</u>

hitKind - Only the 7 least significant bits should be written [0..127]. Any values above 127 are reserved
for built in intersection. The value can be queried withoptixGetHitKind( )in AH and CH.

The attributes specified with a0..a7 are available in the AH and CH programs. Note that the attributes
available in the CH program correspond to the closest recorded intersection. The number of attributes
in registers and memory can be configured in the pipeline.

## Parameters

| in | hitT |
| --- | --- |
| in | hitKind |

## Available in IS

## 5.1.2.202 optixReportIntersection( ) [2/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0*) *[static]*

Reports an intersection (overload with 1 attribute register).

See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.203 optixReportIntersection( ) [3/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1*) *[static]*

Reports an intersection (overload with 2 attribute registers).

See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.204 optixReportIntersection( ) [4/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2*) *[static]*

Reports an intersection (overload with 3 attribute registers).

See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.205 optixReportIntersection( ) [5/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*

---

<u>54</u>

unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3*) *[static]*
Reports an intersection (overload with 4 attribute registers).
See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.206 optixReportIntersection( ) [6/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3,*
unsigned int *a4*) *[static]*
Reports an intersection (overload with 5 attribute registers).

See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.207 optixReportIntersection( ) [7/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3,*
unsigned int *a4,*
unsigned int *a5*) *[static]*
Reports an intersection (overload with 6 attribute registers).
See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.208 optixReportIntersection( ) [8/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3,* unsigned int *a4,*
unsigned int *a5,*
unsigned int *a6*) *[static]*
Reports an intersection (overload with 7 attribute registers).
See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.209 optixReportIntersection( ) [9/9]

static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3,*
unsigned int *a4,*
unsigned int *a5,*
unsigned int *a6,*
unsigned int *a7*) *[static]*

Reports an intersection (overload with 8 attribute registers).

See alsooptixReportIntersection(float,unsigned int)Available in IS

## 5.1.2.210 optixSetPayload_0( )

static __forceinline__ __device__ void optixSetPayload_0 (
unsigned int *p*) *[static]*

Writes the 32-bit payload at the given slot index. There are up to 32 attributes available. The number of
attributes is configured withOptixPipelineCompileOptions::numPayloadValuesor with
OptixPayloadTypeparameters set inOptixModuleCompileOptions.

Available in IS, AH, CH, MS

## 5.1.2.211 optixSetPayload_1( )

static __forceinline__ __device__ void optixSetPayload_1 (
unsigned int *p*) *[static]*
5.1.2.212 optixSetPayload_10( )
static __forceinline__ __device__ void optixSetPayload_10 (
unsigned int *p*) *[static]*
5.1.2.213 optixSetPayload_11( )
static __forceinline__ __device__ void optixSetPayload_11 (
unsigned int *p*) *[static]*

---

## 5.1.2.214 optixSetPayload_12()
static __forceinline__ __device__ void optixSetPayload_12 (
unsigned int p ) [static]

5.1.2.215 optixSetPayload_13()
static __forceinline__ __device__ void optixSetPayload_13 (
unsigned int p ) [static]

5.1.2.216 optixSetPayload_14()
static __forceinline__ __device__ void optixSetPayload_14 (
unsigned int p ) [static]

5.1.2.217 optixSetPayload_15()
static __forceinline__ __device__ void optixSetPayload_15 (
unsigned int p ) [static]

5.1.2.218 optixSetPayload_16()
static __forceinline__ __device__ void optixSetPayload_16 (
unsigned int p ) [static]

5.1.2.219 optixSetPayload_17()
static __forceinline__ __device__ void optixSetPayload_17 (
unsigned int p ) [static]

5.1.2.220 optixSetPayload_18()
static __forceinline__ __device__ void optixSetPayload_18 (
unsigned int p ) [static]

5.1.2.221 optixSetPayload_19()
static __forceinline__ __device__ void optixSetPayload_19 (
unsigned int p ) [static]

5.1.2.222 optixSetPayload_2()
static __forceinline__ __device__ void optixSetPayload_2 (
unsigned int p ) [static]

5.1.2.223 optixSetPayload_20()
static __forceinline__ __device__ void optixSetPayload_20 (
unsigned int p ) [static]

## 5.1.2.224 optixSetPayload_21()
static __forceinline__ __device__ void optixSetPayload_21 (

---

## 5.1.2.225 optixSetPayload_22( )

static __forceinline__ __device__ void optixSetPayload_22 (
unsigned int *p*) *[static]*

## 5.1.2.226 optixSetPayload_23( )

static __forceinline__ __device__ void optixSetPayload_23 (
unsigned int *p*) *[static]*

## 5.1.2.227 optixSetPayload_24( )

static __forceinline__ __device__ void optixSetPayload_24 (
unsigned int *p*) *[static]*

## 5.1.2.228 optixSetPayload_25( )

static __forceinline__ __device__ void optixSetPayload_25 (
unsigned int *p*) *[static]*

## 5.1.2.229 optixSetPayload_26( )

static __forceinline__ __device__ void optixSetPayload_26 (
unsigned int *p*) *[static]*

## 5.1.2.230 optixSetPayload_27( )

static __forceinline__ __device__ void optixSetPayload_27 (
unsigned int *p*) *[static]*

## 5.1.2.231 optixSetPayload_28( )

static __forceinline__ __device__ void optixSetPayload_28 (
unsigned int *p*) *[static]*

## 5.1.2.232 optixSetPayload_29( )

static __forceinline__ __device__ void optixSetPayload_29 (
unsigned int *p*) *[static]*

## 5.1.2.233 optixSetPayload_3( )

static __forceinline__ __device__ void optixSetPayload_3 (
unsigned int *p*) *[static]*

## 5.1.2.234 optixSetPayload_30( )

static __forceinline__ __device__ void optixSetPayload_30 (
unsigned int *p*) *[static]*

---

## 5.1.2.235 optixSetPayload_31( )

static __forceinline__ __device__ void optixSetPayload_31 (
unsigned int *p*) *[static]*

5.1.2.236 optixSetPayload_4( )

static __forceinline__ __device__ void optixSetPayload_4 (
unsigned int *p*) *[static]*

5.1.2.237 optixSetPayload_5( )

static __forceinline__ __device__ void optixSetPayload_5 (
unsigned int *p*) *[static]*

5.1.2.238 optixSetPayload_6( )

static __forceinline__ __device__ void optixSetPayload_6 (
unsigned int *p*) *[static]*

5.1.2.239 optixSetPayload_7( )

static __forceinline__ __device__ void optixSetPayload_7 (
unsigned int *p*) *[static]*

5.1.2.240 optixSetPayload_8( )

static __forceinline__ __device__ void optixSetPayload_8 (
unsigned int *p*) *[static]*

## 5.1.2.241 optixSetPayload_9( )

static __forceinline__ __device__ void optixSetPayload_9 (
unsigned int *p*) *[static]*

## 5.1.2.242 optixSetPayloadTypes( )

static __forceinline__ __device__ void optixSetPayloadTypes (
unsigned int *typeMask*) *[static]*

Specify the supported payload types for a program.

The supported types are specified as a bitwise combination of payload types. (See
OptixPayloadTypeID) May only be called once per program.

Must be called at the top of the program.

Available in IS, AH, CH, MS

## 5.1.2.243 optixTerminateRay( )

static __forceinline__ __device__ void optixTerminateRay () *[static]*

Record the hit, stops traversal, and proceeds to CH.

Available in AH

---

## 5.1.2.244 optixTexFootprint2D( )

static __forceinline__ __device__ uint4 optixTexFootprint2D (

unsigned long long *tex,*
unsigned int *texInfo,*
float *x,*
float *y,*

unsigned int *∗ singleMipLevel*) *[static]*

optixTexFootprint2D calculates the footprint of a corresponding 2D texture fetch (non-mipmapped).
On Turing and subsequent architectures, a texture footprint instruction allows user programs to
determine the set of texels that would be accessed by an equivalent filtered texture lookup.

## Parameters

| in | tex | CUDA texture object (cast to 64-bit integer) |
| --- | --- | --- |
| in | texInfo | Texture info packed into 32-bit integer, described below. |
| in | x | Texture coordinate |
| in | y | Texture coordinate |
| out | singleMipLevel | Result indicating whether the footprint spans only a single miplevel. |

The texture info argument is a packed 32-bit integer with the following layout:

texInfo[31:29] = reserved (3 bits) texInfo[28:24] = miplevel count (5 bits) texInfo[23:20] = log2 of tile
width (4 bits) texInfo[19:16] = log2 of tile height (4 bits) texInfo[15:10] = reserved (6 bits) texInfo[9:8] =
horizontal wrap mode (2 bits) (CUaddress_mode) texInfo[7:6] = vertical wrap mode (2 bits)
(CUaddress_mode) texInfo[5] = mipmap filter mode (1 bit) (CUfilter_mode) texInfo[4:0] = maximum
anisotropy (5 bits)

Returns a 16-byte structure (as a uint4) that stores the footprint of a texture request at a particular
"granularity", which has the following layout:

struct Texture2DFootprint { unsigned long long mask; unsigned int tileY : 12; unsigned int reserved1 :
4; unsigned int dx : 3; unsigned int dy : 3; unsigned int reserved2 : 2; unsigned int granularity : 4;
unsigned int reserved3 : 4; unsigned int tileX : 12; unsigned int level : 4; unsigned int reserved4 : 16; };

The granularity indicates the size of texel groups that are represented by an 8x8 bitmask. For example,
a granularity of 12 indicates texel groups that are 128x64 texels in size. In a footprint call, The returned
granularity will either be the actual granularity of the result, or 0 if the footprint call was able to honor
the requested granularity (the usual case).

level is the mip level of the returned footprint. Two footprint calls are needed to get the complete
footprint when a texture call spans multiple mip levels.

mask is an 8x8 bitmask of texel groups that are covered, or partially covered, by the footprint. tileX and
tileY give the starting position of the mask in 8x8 texel-group blocks. For example, suppose a
granularity of 12 (128x64 texels), and tileX=3 and tileY=4. In this case, bit 0 of the mask (the low order
bit) corresponds to texel group coordinates (3*∗*8, 4*∗*8), and texel coordinates (3*∗*8*∗*128, 4*∗*8*∗*64), within
the specified mip level.

If nonzero, dx and dy specify a "toroidal rotation" of the bitmask. Toroidal rotation of a coordinate in
the mask simply means that its value is reduced by 8. Continuing the example from above, if dx=0 and
dy=0 the mask covers texel groups (3*∗*8, 4*∗*8) to (3*∗*8+7, 4*∗*8+7) inclusive. If, on the other hand, dx=2,
the rightmost 2 columns in the mask have their x coordinates reduced by 8, and similarly for dy.

---

See the OptiX SDK for sample code that illustrates how to unpack the result.

Available anywhere

## 5.1.2.245 optixTexFootprint2DGrad( )

static __forceinline__ __device__ uint4 optixTexFootprint2DGrad (
unsigned long long *tex,*
unsigned int *texInfo,*
float *x,*
float *y,*
float *dPdx_x,*
float *dPdx_y,*
float *dPdy_x,*
float *dPdy_y,*
bool *coarse,*
unsigned int *∗ singleMipLevel*) *[static]*

$$
x{,}
$$

optixTexFootprint2DGrad calculates the footprint of a corresponding 2D texture fetch (tex2DGrad)

## Parameters

| in | tex | CUDA texture object (cast to 64-bit integer) |
| --- | --- | --- |
| in | texInfo | Texture info packed into 32-bit integer, described below. |
| in | x | Texture coordinate |
| in | y | Texture coordinate |
| in | dPdx_x | Derivative of x coordinate, which determines level of detail. |
| in | dPdx_y | Derivative of x coordinate, which determines level of detail. |
| in | dPdy_x | Derivative of y coordinate, which determines level of detail. |
| in | dPdy_y | Derivative of y coordinate, which determines level of detail. |
| in | coarse | Requests footprint from coarse miplevel, when the footprint spans two levels. |
| out | singleMipLevel | Result indicating whether the footprint spans only a single miplevel. |

See alsooptixTexFootprint2D(unsigned long long,unsigned int,float,float,unsigned int*∗*)Available
anywhere

## 5.1.2.246 optixTexFootprint2DLod( )

static __forceinline__ __device__ uint4 optixTexFootprint2DLod (
unsigned long long *tex,*
unsigned int *texInfo,*
float *x,*
float *y,*
float *level,*
bool *coarse,*
unsigned int *∗ singleMipLevel*) *[static]* optixTexFootprint2DLod calculates the footprint of a corresponding 2D texture fetch (tex2DLod)

Parameters

| in | tex | CUDA texture object (cast to 64-bit integer) |
| --- | --- | --- |
| in | texInfo | Texture info packed into 32-bit integer, described below. |
| in | x | Texture coordinate |
| in | y | Texture coordinate |
| in | level | Level of detail(lod) |
| in | coarse | Requests footprint from coarse miplevel, when the footprint spans two levels. |
| out | singleMipLevel | Result indicating whether the footprint spans only a single miplevel. |

See alsooptixTexFootprint2D(unsigned long long,unsigned int,float,float,unsigned int*∗*)Available
anywhere

## 5.1.2.247 optixThrowException( ) [1/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode*) *[static]*

Throws a user exception with the given exception code (overload without exception details).

∧
The exception code must be in the range from 0 to 2 30 - 1. Up to 8 optional exception details can be
passed. They can be queried in the EX program usingoptixGetExceptionDetail_0( )to ..._8().

The exception details must not be used to encode pointers to the stack since the current stack is not
preserved in the EX program.

## Not available in EX

## Parameters

| in | exceptionCode | The exception code to be thrown. |
| --- | --- | --- |

Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.248 optixThrowException( ) [2/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0*) *[static]*

Throws a user exception with the given exception code (overload with 1 exception detail).

See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.249 optixThrowException( ) [3/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0,*
unsigned int *exceptionDetail1*) *[static]*

Throws a user exception with the given exception code (overload with 2 exception details).

---

See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.250 optixThrowException( ) [4/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0,*
unsigned int *exceptionDetail1,*
unsigned int *exceptionDetail2*) *[static]*
Throws a user exception with the given exception code (overload with 3 exception details).
See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.251 optixThrowException( ) [5/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0,*
unsigned int *exceptionDetail1,*
unsigned int *exceptionDetail2,*
unsigned int *exceptionDetail3*) *[static]*

Throws a user exception with the given exception code (overload with 4 exception details).

See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.252 optixThrowException( ) [6/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0,*
unsigned int *exceptionDetail1,*
unsigned int *exceptionDetail2,*
unsigned int *exceptionDetail3,*
unsigned int *exceptionDetail4*) *[static]*

Throws a user exception with the given exception code (overload with 5 exception details).

See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.253 optixThrowException( ) [7/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0,*
unsigned int *exceptionDetail1,*
unsigned int *exceptionDetail2,*
unsigned int *exceptionDetail3,*
unsigned int *exceptionDetail4,*
unsigned int *exceptionDetail5*) *[static]*

---

Throws a user exception with the given exception code (overload with 6 exception details).

See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.254 optixThrowException( ) [8/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0,*
unsigned int *exceptionDetail1,*
unsigned int *exceptionDetail2,*
unsigned int *exceptionDetail3,*
unsigned int *exceptionDetail4,*
unsigned int *exceptionDetail5,*
unsigned int *exceptionDetail6*) *[static]*

Throws a user exception with the given exception code (overload with 7 exception details).

See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.255 optixThrowException( ) [9/9]

static __forceinline__ __device__ void optixThrowException (
int *exceptionCode,*
unsigned int *exceptionDetail0,*
unsigned int *exceptionDetail1,*
unsigned int *exceptionDetail2,*
unsigned int *exceptionDetail3,*
unsigned int *exceptionDetail4,*
unsigned int *exceptionDetail5,*
unsigned int *exceptionDetail6,*
unsigned int *exceptionDetail7*) *[static]*
Throws a user exception with the given exception code (overload with 8 exception details).

See alsooptixThrowException(int)Available in RG, IS, AH, CH, MS, DC, CC

## 5.1.2.256 optixTrace( ) [1/2]

template<typename... Payload>
static __forceinline__ __device__ void optixTrace (
OptixPayloadTypeID *type,*
OptixTraversableHandle *handle,*
float3 *rayOrigin,*
float3 *rayDirection,*
float *tmin,*
float *tmax,*
float *rayTime,*
OptixVisibilityMask *visibilityMask,* unsigned int *rayFlags,*

unsigned int *SBToffset,*

unsigned int *SBTstride,*

unsigned int *missSBTIndex,*

Payload &... *payload*) *[static]*

Initiates a ray tracing query starting with the given traversable.

Parameters

| in | type |  |
| --- | --- | --- |
| in | handle |  |
| in | rayOrigin |  |
| in | rayDirection |  |
| in | tmin |  |
| in | tmax |  |
| in | rayTime |  |
| in | visibilityMask | really only 8 bits |
| in | rayFlags | really only 16 bits, combination of OptixRayFlags |
| in | SBToffset | really only 4 bits |
| in | SBTstride | really only 4 bits |
| in | missSBTIndex | specifies the miss program invoked on a miss |
| in,out | payload | up to 32 unsigned int values that hold the payload |

## Available in RG, CH, MS, CC

## 5.1.2.257 optixTrace( ) [2/2]

template<typename... Payload>

static __forceinline__ __device__ void optixTrace (
OptixTraversableHandle *handle,*
float3 *rayOrigin,*
float3 *rayDirection,*
float *tmin,*
float *tmax,*
float *rayTime,*
OptixVisibilityMask *visibilityMask,*
unsigned int *rayFlags,*
unsigned int *SBToffset,*
unsigned int *SBTstride,*
unsigned int *missSBTIndex,*
Payload &... *payload*) *[static]*

Initiates a ray tracing query starting with the given traversable.

---

## Parameters

| in | handle |  |
| --- | --- | --- |
| in | rayOrigin |  |
| in | rayDirection |  |
| in | tmin |  |
| in | tmax |  |
| in | rayTime |  |
| in | visibilityMask | really only 8 bits |
| in | rayFlags | really only 16 bits, combination of OptixRayFlags |
| in | SBToffset | really only 4 bits |
| in | SBTstride | really only 4 bits |
| in | missSBTIndex | specifies the miss program invoked on a miss |
| in,out | payload | up to 32 unsigned int values that hold the payload |

Available in RG, CH, MS, CC

## 5.1.2.258 optixTransformNormalFromObjectToWorldSpace( ) [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformNormalFromObjectToWorldSpace (
const HitState & *hs,*
float3 *normal*) *[static]*
Transforms the normal using object-to-world transformation matrix resulting from the transformation
list of the templated hit object (see optixGetWorldToObjectTransformMatrix for example usage).
The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.259 optixTransformNormalFromObjectToWorldSpace( ) [2/2]

static __forceinline__ __device__ float3
optixTransformNormalFromObjectToWorldSpace (
float3 *normal*) *[static]*
Transforms the normal using object-to-world transformation matrix resulting from the current active
transformation list.
The cost of this function may be proportional to the size of the transformation list.
Available in IS, AH, CH
5.1.2.260 optixTransformNormalFromWorldToObjectSpace( ) [1/2]
template<typename HitState >
static __forceinline__ __device__ float3
optixTransformNormalFromWorldToObjectSpace (
const HitState & *hs,* float3 *normal*) *[static]*

Transforms the normal using world-to-object transformation matrix resulting from the transformation
list of the templated hit object (see optixGetWorldToObjectTransformMatrix for example usage).

The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.261 optixTransformNormalFromWorldToObjectSpace( ) [2/2]

static __forceinline__ __device__ float3
optixTransformNormalFromWorldToObjectSpace (
float3 *normal*) *[static]*
Transforms the normal using world-to-object transformation matrix resulting from the current active
transformation list.
The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.262 optixTransformPointFromObjectToWorldSpace( ) [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformPointFromObjectToWorldSpace (
const HitState & *hs,*
float3 *point*) *[static]*
Transforms the point using object-to-world transformation matrix resulting from the transformation
list of the templated hit object (see optixGetWorldToObjectTransformMatrix for example usage).
The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.263 optixTransformPointFromObjectToWorldSpace( ) [2/2]

static __forceinline__ __device__ float3
optixTransformPointFromObjectToWorldSpace (
float3 *point*) *[static]*
Transforms the point using object-to-world transformation matrix resulting from the current active
transformation list.
The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.264 optixTransformPointFromWorldToObjectSpace( ) [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformPointFromWorldToObjectSpace (
const HitState & *hs,*
float3 *point*) *[static]*

---

Transforms the point using world-to-object transformation matrix resulting from the transformation
list of the templated hit object (see optixGetWorldToObjectTransformMatrix for example usage).

The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.265 optixTransformPointFromWorldToObjectSpace( ) [2/2]

static __forceinline__ __device__ float3
optixTransformPointFromWorldToObjectSpace (
float3 *point*) *[static]*
Transforms the point using world-to-object transformation matrix resulting from the current active
transformation list.
The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.266 optixTransformVectorFromObjectToWorldSpace( ) [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformVectorFromObjectToWorldSpace (
const HitState & *hs,*
float3 *vec*) *[static]*
Transforms the vector using object-to-world transformation matrix resulting from the transformation
list of the templated hit object (see optixGetWorldToObjectTransformMatrix for example usage).
The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

5.1.2.267 optixTransformVectorFromObjectToWorldSpace( ) [2/2]

static __forceinline__ __device__ float3
optixTransformVectorFromObjectToWorldSpace (
float3 *vec*) *[static]*
Transforms the vector using object-to-world transformation matrix resulting from the current active
transformation list.

The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.268 optixTransformVectorFromWorldToObjectSpace( ) [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformVectorFromWorldToObjectSpace (
const HitState & *hs,*
float3 *vec*) *[static]*
Transforms the vector using world-to-object transformation matrix resulting from the transformation
list of the templated hit object (see optixGetWorldToObjectTransformMatrix for example usage).

---

The cost of this function may be proportional to the size of the transformation list.

Available in IS, AH, CH

## 5.1.2.269 optixTransformVectorFromWorldToObjectSpace( ) [2/2]

static __forceinline__ __device__ float3
optixTransformVectorFromWorldToObjectSpace (
float3 *vec*) *[static]*
Transforms the vector using world-to-object transformation matrix resulting from the current active
transformation list.
The cost of this function may be proportional to the size of the transformation list.
Available in IS, AH, CH

## 5.1.2.270 optixTraverse( ) [1/2]

template<typename... Payload>
static __forceinline__ __device__ void optixTraverse (
OptixPayloadTypeID *type,*
OptixTraversableHandle *handle,*
float3 *rayOrigin,*
float3 *rayDirection,*
float *tmin,*
float *tmax,*
float *rayTime,*
OptixVisibilityMask *visibilityMask,*
unsigned int *rayFlags,*
unsigned int *SBToffset,*
unsigned int *SBTstride,*
unsigned int *missSBTIndex,*
Payload &... *payload*) *[static]*

Similar to optixTrace, but does not invoke closesthit or miss. Instead, it overwrites the current outgoing
hit object with the results of traversing the ray. The outgoing hit object may be invoked at some later
point with optixInvoke. The outgoing hit object can also be queried through various functions such as
optixHitObjectIsHit or optixHitObjectGetAttribute_0.

## Parameters

| in | type |  |
| --- | --- | --- |
| in | handle |  |
| in | rayOrigin |  |
| in | rayDirection |  |
| in | tmin |  |
| in | tmax |  |
| in | rayTime |  |
| in | visibilityMask | really only 8 bits |

---

## Parameters

| in | rayFlags | really only 16 bits, combination of OptixRayFlags |
| --- | --- | --- |
| in | SBToffset | really only 4 bits |
| in | SBTstride | really only 4 bits |
| in | missSBTIndex | specifies the miss program invoked on a miss |
| in,out | payload | up to 32 unsigned int values that hold the payload |

Available in RG, CH, MS, CC, DC

## 5.1.2.271 optixTraverse( ) [2/2]

template<typename... Payload>
static __forceinline__ __device__ void optixTraverse (
OptixTraversableHandle *handle,*
float3 *rayOrigin,*
float3 *rayDirection,*
float *tmin,*
float *tmax,*
float *rayTime,*
OptixVisibilityMask *visibilityMask,*
unsigned int *rayFlags,*
unsigned int *SBToffset,*
unsigned int *SBTstride,*
unsigned int *missSBTIndex,*
Payload &... *payload*) *[static]*

Similar to optixTrace, but does not invoke closesthit or miss. Instead, it overwrites the current outgoing
hit object with the results of traversing the ray. The outgoing hit object may be invoked at some later
point with optixInvoke. The outgoing hit object can also be queried through various functions such as
optixHitObjectIsHit or optixHitObjectGetAttribute_0.

## Parameters

| in | handle |  |
| --- | --- | --- |
| in | rayOrigin |  |
| in | rayDirection |  |
| in | tmin |  |
| in | tmax |  |
| in | rayTime |  |
| in | visibilityMask | really only 8 bits |
| in | rayFlags | really only 16 bits, combination of OptixRayFlags |
| in | SBToffset | really only 4 bits |
| in | SBTstride | really only 4 bits |
| in | missSBTIndex | specifies the miss program invoked on a miss |

---

## Parameters

| in,out | payload | up to 32 unsigned int values that hold the payload |
| --- | --- | --- |

Available in RG, CH, MS, CC, DC

5.1.2.272 optixUndefinedValue()

static __forceinline__ __device__ unsigned int optixUndefinedValue ( ) [static] Returns an undefined value.

Available anywhere

## 5.2 Cooperative Vector

## Classes

- class OptixCoopVec<T,N>

## Functions

- template <typename
  static __forceinline__

__device__ VecTOut optixCoopVecLoad (CUdeviceptr ptr)

- template <typename VecTC
static __forceinline__ __dev

out , typename T >
ce__ VecTOut optixCoopVecLoad (T *ptr)

- template <typename VecT >
  static __forceinline__ __device__ Vec

optixCoopVecExp2 (const VecT &vec)

- template <typename VecT >
  static __forceinline__ __device__ VecT optixCoopVecLog2 (const VecT &vec)

- template <typename VecT >
  static __forceinline__ __device_

VecT optixCoopVecTanh (const VecT &vec)

- template <typename VecTOut , typename VecTIn static __forceinline__ __device__ VecTOut optixG

> opVecCvt (const VecTIn &vec)

- template <typename VecT >
  static __forceinline__ __device__ VecT optixCoopVecMin (const VecT &vecA, const VecT &vecB)

- template <typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMin (const VecT &vecA, typename VecT ::value_type B)

- template <typename VecT >
  static __forceinline__ __device__ VecT optixCoopVecMax (const VecT &vecA, const VecT &vecB)

- template <typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMax (const VecT &vecA, typename VecT ::value_type B)

- template <typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMul (const VecT &vecA, const VecT &vecB)

- template <typename VecT >
static __forceinline__ __device__ VecT

tixCoopVecAdd (const VecT &vecA, const VecT &vecB)

- template <typename VecT >
static __forceinline__ __device__ VecT optixCoopVecSub (const VecT &vecA, const VecT &vecB)

- template <typename VecT >
static __forceinline__ __device__ VecT optixCoopVecStep (const VecT &vecA, const VecT &vecB)

- template <typename VecT >
static __forceinline__ __device__ VecT optixCoopVecFFMA (const VecT &vecA, const VecT &vecB, const VecT &vecC)

---

• template<typename VecTOut , typename VecTIn , OptixCoopVecElemType inputInterpretation,
OptixCoopVecMatrixLayout matrixLayout, bool transpose, unsigned int N, unsigned int K,
OptixCoopVecElemType matrixElementType, OptixCoopVecElemType biasElementType>
static __forceinline__ __device__ VecTOut optixCoopVecMatMul (const VecTIn &inputVector,
CUdeviceptr matrix, unsigned matrixOffsetInBytes, CUdeviceptr bias, unsigned
biasOffsetInBytes, unsigned rowColumnStrideInBytes=0)

• template<typename VecTOut , typename VecTIn , OptixCoopVecElemType inputInterpretation,
OptixCoopVecMatrixLayout matrixLayout, bool transpose, unsigned int N, unsigned int K,
OptixCoopVecElemType matrixElementType>
static __forceinline__ __device__ VecTOut optixCoopVecMatMul (const VecTIn &inputVector,
CUdeviceptr matrix, unsigned matrixOffsetInBytes, unsigned rowColumnStrideInBytes=0)

• template<typename VecTIn >
static __forceinline__ __device__ void optixCoopVecReduceSumAccumulate (const VecTIn
&inputVector, CUdeviceptr outputVector, unsigned offsetInBytes)

• template<typename VecTA , typename VecTB , OptixCoopVecMatrixLayout matrixLayout =
OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL>
static __forceinline__ __device__ void optixCoopVecOuterProductAccumulate (const VecTA
&vecA, const VecTB &vecB, CUdeviceptr outputMatrix, unsigned offsetInBytes, unsigned
rowColumnStrideInBytes=0)

• template<unsigned int N, unsigned int K, OptixCoopVecElemType elementType,
OptixCoopVecMatrixLayout layout = OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_
OPTIMAL, unsigned int rowColumnStrideInBytes = 0>
static __forceinline__ __device__ unsigned int optixCoopVecGetMatrixSize ()

## 5.2.1 Detailed Description

## 5.2.2 Function Documentation

## 5.2.2.1 optixCoopVecAdd()

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecAdd (
    const VecT & vecA,
    const VecT & vecB ) [static]

Available anywhere.

## 5.2.2.2 optixCoopVecCvt()

template<typename VecTOut , typename VecTIn >
static __forceinline__ __device__ VecTOut optixCoopVecCvt (
    const VecTIn & vec ) [static]

Convert from VecTIn to VecTOut. Not all conversions are supported, only integral to 16 or 32-bit
floating point.

Available anywhere

## 5.2.2.3 optixCoopVecExp2()

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecExp2 (
    const VecT & vec ) [static]

---

Following functions are designed to facilitate activation function evaluation between calls to
optixCoopVecMatMul. Utilizing only these functions on the activation vectors will typically improve
performance.

Available anywhere

## 5.2.2.4 optixCoopVecFFMA( )

template<typename VecT >

static __forceinline__ __device__ VecT optixCoopVecFFMA (
const VecT & *vecA,*
const VecT & *vecB,*
const VecT & *vecC*) *[static]*

Available anywhere.

## 5.2.2.5 optixCoopVecGetMatrixSize( )

template<unsigned int N, unsigned int K,OptixCoopVecElemTypeelementType,
OptixCoopVecMatrixLayoutlayout = OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_
OPTIMAL, unsigned int rowColumnStrideInBytes = 0*>*
static __forceinline__ __device__ unsigned int optixCoopVecGetMatrixSize ()
*[static]*

This function is intended strictly for matrix layouts that must be computed through the host API,
optixCoopVecMatrixComputeSize, but is needed on the device. For optimal performance the offsets to
each layer in a network should be constant, so this function can be used to facilitate calculating the
offset for subsequent layers in shader code. It can also be used for calculating the size of row and
column major matrices, but the rowColumnStrideInBytes template parameter must be specified, so
that it can be calculated during compilation.

For row and column ordered matrix layouts, when rowColumnStrideInBytes is 0, the stride will
assume tight packing.

Results will be rounded to the next multiple of 64 to make it easy to pack the matrices in memory and
have the correct alignment.

Results are in number of bytes, and should match the output of the host function
optixCoopVecMatrixComputeSize.

## Template Parameters

| N,K | dimensions of the matrix |
| --- | --- |
| elementType | Type of the matrix elements |
| layout | Layout of the matrix |

Available anywhere

## 5.2.2.6 optixCoopVecLoad( ) [1/2]

template<typename VecTOut >

static __forceinline__ __device__ VecTOut optixCoopVecLoad (
CUdeviceptr *ptr*) *[static]*

Load the vector from global memory. The memory address must be 16 byte aligned regardless of the type and number of elements in the vector.

Available anywhere

## 5.2.2.7 optixCoopVecLoad( ) [2/2]

template<typename VecTOut, typename T >

static __forceinline__ __device__ VecTOut optixCoopVecLoad (
T *∗ ptr*) *[static]*

Load the vector from global memory. The memory address must be 16 byte aligned regardless of the
type and number of elements in the vector.

Available anywhere

## 5.2.2.8 optixCoopVecLog2( )

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecLog2 (
const VecT & *vec*) *[static]*
Available anywhere.

## 5.2.2.9 optixCoopVecMatMul( ) [1/2]

template<typename VecTOut, typename VecTIn,OptixCoopVecElemType
inputInterpretation,OptixCoopVecMatrixLayoutmatrixLayout, bool transpose,
unsigned int N, unsigned int K,OptixCoopVecElemTypematrixElementType,
OptixCoopVecElemTypebiasElementType*>*

static __forceinline__ __device__ VecTOut optixCoopVecMatMul (

const VecTIn & *inputVector,*
CUdeviceptr *matrix,*
unsigned *matrixOffsetInBytes,*
CUdeviceptr *bias,*
unsigned *biasOffsetInBytes,*

unsigned *rowColumnStrideInBytes = 0*) *[static]*

Computes a vector matrix multiplication with an addition of a bias.

A * B + C = D
Does matrix * inputVector + bias = output
[NxK] [Kx1] [Nx1] = [Nx1]

Not all combinations of inputType and matrixElementType are supported. See the following table for
supported configurations.

|  |  |  |  |  |
| --- | --- | --- | --- | --- |
| FLOAT16 | FLOAT16 | FLOAT16 | FLOAT16 | FLOAT16 |
| FLOAT16 | FLOAT8_E4M3 | FLOAT8_E4M3 | FLOAT16 | FLOAT16 |
| FLOAT16 | FLOAT8_E5M4 | FLOAT8_E5M4 | FLOAT16 | FLOAT16 |
| FLOAT16 | UINT8/INT8 | UINT8/INT8 | UINT32/INT32 | UINT32/INT32 |
| FLOAT32 | UINT8/INT8 | UINT8/INT8 | UINT32/INT32 | UINT32/INT32 |
| UINT8/INT8 | UINT8/INT8 | UINT8/INT8 | UINT32/INT32 | UINT32/INT32 |

---

If either the input or matrix is signed, then the bias and output must also be signed.

When matrixElementType is OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E4M3 or OPTIX_COOP_
VEC_ELEM_TYPE_FLOAT8_E5M2 the matrixLayout must be either OPTIX_COOP_VEC_MATRIX_
LAYOUT_INFERENCING_OPTIMAL or OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_
OPTIMAL.

When the inputVector's element type does not match the inputInterpretation arithmetically casting is
performed on the input values to match the inputInterpretation.

If transpose is true, the matrix is treated as being stored transposed in memory (stored as KxN instead
of NxK). Set other parameters as if the matrix was not transposed in memory. Not all matrix element
types or matrix layouts support transpose. Only OPTIX_COOP_VEC_ELEM_TYPE_FLOAT16 is
supported. Only OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_OPTIMAL and OPTIX_
COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL are supported.

The bias pointer is assumed to not be null and may be dereferenced. If you wish to do the matrix
multiply without a bias then use the overloaded version of this function that does not take the bias.

For row and column ordered matrix layouts, the stride will assume tight packing when
rowColumnStrideInBytes is a constant immediate 0 (computed values or loaded from memory will not
work). Ignored for other matrix layouts. Value must be 16 byte aligned.

Template Parameters

| VecTOut | Type must match biasElementType and size must match N |
| --- | --- |
| VecTIn | Type must be i32,f16 or f32 type and size must match K |
| inputInterpretation | Must match matrixLayout |
| matrixLayout | The layout of the matrix in memory |
| transpose | Whether the data in memory for matrix is transposed from the specified layout |
| N | Must match VecTOut::size |
| K | Must match VecTIn::size |
| matrixElementType | Type of elements stored in memory |
| biasElementType | Type of elements stored in memory,must also match VecTOut::elementType |

## Parameters

| in | inputVector |  |
| --- | --- | --- |
| in | matrix | pointer to global memory. Array of NxK elements. 64 byte aligned.Must not be modified during use. |
| in | matrixOffsetInBytes | offset to start of matrix data.Using the same value for matrix with different offsets for all layers yields more efficient execution.64byte aligned. |
| in | bias | pointer to global memory. Array of N elements.16 byte aligned.Must not be modified during use. |
| in | biasOffsetInBytes | offset to start of bias data.Using the same value for bias with different offsets for all layers yields more efficient execution.16byte aligned. |
| in | rowColumnStrideInBytes | for row or column major matrix layouts,this identifies the stride between columns or rows. |

---

Available in all OptiX program types

## 5.2.2.10 optixCoopVecMatMul() [2/2]

template<typename VecTOut , typename VecTIn , OptixCoopVecElemType
inputInterpretation, OptixCoopVecMatrixLayout matrixLayout, bool transpose,
unsigned int N, unsigned int K, OptixCoopVecElemType matrixElementType>
static __forceinline__ __device__ VecTOut optixCoopVecMatMul (
    const VecTIn & inputVector,
    CUdeviceptr matrix,
    unsigned matrixOffsetInBytes,
    unsigned rowColumnStrideInBytes = 0 ) [static]

Same as optixCoopVecMatMul, but without the bias parameters.

## 5.2.2.11 optixCoopVecMax() [1/2]

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMax (
    const VecT & vecA,
    const VecT & vecB ) [static]

Available anywhere.

## 5.2.2.12 optixCoopVecMax() [2/2]

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMax (
    const VecT & vecA,
    typename VecT::value_type B ) [static]

Available anywhere.

## 5.2.2.13 optixCoopVecMin() [1/2]

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMin (
    const VecT & vecA,
    const VecT & vecB ) [static]

Available anywhere.

## 5.2.2.14 optixCoopVecMin() [2/2]

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMin (
    const VecT & vecA,
    typename VecT::value_type B ) [static]

---

## 5.2.2.15 optixCoopVecMul( )

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMul (
const VecT & *vecA,*
const VecT & *vecB*) *[static]*

Available anywhere.

## 5.2.2.16 optixCoopVecOuterProductAccumulate( )

template<typename VecTA, typename VecTB,OptixCoopVecMatrixLayout
matrixLayout = OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL*>*
static __forceinline__ __device__ void optixCoopVecOuterProductAccumulate (
const VecTA & *vecA,*
const VecTB & *vecB,*
CUdeviceptr *outputMatrix,*
unsigned *offsetInBytes,*
unsigned *rowColumnStrideInBytes = 0*) *[static]*

Produces a matrix outer product of the input vecA and vecB (vecA *∗* transpose(vecB)) and does a
component-wise atomic add reduction of the result into global memory starting *offsetInBytes* bytes after
*outputMatrix*. The dimentions of the matrix are [VecTA::size, VecTB::size]. VecTA::elementType, VecTB
::elementType and the element type of the matrix must be the same, no type conversion is performed.
The element type must be OPTIX_COOP_VEC_ELEM_TYPE_FLOAT16.

outputMatrix + offsetInBytes must be 4B aligned, but performance may be better with 128 byte
alignments.

The output matrix will be in matrixLayout layout, though currently only OPTIX_COOP_VEC_
MATRIX_LAYOUT_TRAINING_OPTIMAL layout is supported.

## Template Parameters

| VecTA | Type of vecA |
| --- | --- |
| VecTB | Type of vecB |
| matrixLayout | Layout of matrix stored in outputMatrix |

## Parameters

| in | vecA |  |
| --- | --- | --- |
| in | vecB |  |
| in | outputMatrix | pointer to global memory on the device, sum with offsetInBytes must be a multiple of 4 |
| in | offsetInBytes | offset in bytes from outputMatrix, sum with outputMatrix must be a multiple of 4 |
| in | rowColumnStrideInBytes | stride between rows or columns, zero takes natural stride, ignored for optimal layouts |

Available in all OptiX program types

---

## 5.2.2.17 optixCoopVecReduceSumAccumulate( )

template<typename VecTIn >
static __forceinline__ __device__ void optixCoopVecReduceSumAccumulate (
const VecTIn & *inputVector,*
CUdeviceptr *outputVector,*
unsigned *offsetInBytes*) *[static]*

Performs a component-wise atomic add reduction of the vector into global memory starting at
*offsetInBytes* bytes after *outputVector*.

VecTIn::elementType must be of type OPTIX_COOP_VEC_ELEM_TYPE_FLOAT16 or OPTIX_COOP_
VEC_ELEM_TYPE_FLOAT32 The memory backed by *outputVector* + *offsetInBytes* must be large enough
to accomodate VecTIn::size elements. The type of data in *outputVector* must match VecTIn
::elementType. No type conversion is performed. *outputVector* + *offsetInBytes* must be 4 byte aligned.

Template Parameters

<u>VecTIn</u> <u>Type of inputVector</u>

## Parameters

| in | inputVector |  |
| --- | --- | --- |
| in | outputVector | pointer to global memory on the device,sum with offsetInBytes must be a multiple of4 |
| in | offsetInBytes | offset in bytes from outputVector，sum with outputVector must be a multiple of4 |

Available in all OptiX program types

## 5.2.2.18 optixCoopVecStep( )

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecStep (
const VecT & *vecA,*
const VecT & *vecB*) *[static]*
Returns result[i] = (vecA[i] < vecB[i]) ? 0 : 1;.
Available anywhere

5.2.2.19 optixCoopVecSub( )

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecSub (
const VecT & *vecA,*
const VecT & *vecB*) *[static]*
Available anywhere.

## 5.2.2.20 optixCoopVecTanh( )

template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecTanh (

---

## const VecT & vec) [static]

Available anywhere.

## 5.3 Function Table

## Classes

•structOptixFunctionTable

## Macros

•#defineOPTIX_CONCATENATE_ABI_VERSION(prefix, macro)OPTIX_CONCATENATE_ABI_
VERSION_IMPL(prefix, macro)

•#defineOPTIX_CONCATENATE_ABI_VERSION_IMPL(prefix, macro) prefix ## _ ## macro

•#defineOPTIX_FUNCTION_TABLE_SYMBOLOPTIX_CONCATENATE_ABI_VERSION(g_
optixFunctionTable,OPTIX_ABI_VERSION)

## Typedefs

•typedef structOptixFunctionTableOptixFunctionTable

## Variables

•OptixFunctionTableOPTIX_FUNCTION_TABLE_SYMBOL

## 5.3.1 Detailed Description

OptiX Function Table.

## 5.3.2 Macro Definition Documentation

## 5.3.2.1 OPTIX_CONCATENATE_ABI_VERSION

#define OPTIX_CONCATENATE_ABI_VERSION(

## 5.3.2.2 OPTIX_CONCATENATE_ABI_VERSION_IMPL

#define OPTIX_CONCATENATE_ABI_VERSION_IMPL(

*prefix,*

*macro*) prefix ## _ ## macro

## 5.3.2.3 OPTIX_FUNCTION_TABLE_SYMBOL

#define OPTIX_FUNCTION_TABLE_SYMBOLOPTIX_CONCATENATE_ABI_VERSION(g_
optixFunctionTable,OPTIX_ABI_VERSION)

## 5.3.3 Typedef Documentation

## 5.3.3.1 OptixFunctionTable

typedef structOptixFunctionTableOptixFunctionTable

The function table containing all API functions.

SeeoptixInit( )andoptixInitWithHandle( ).

---

## 5.3.4 Variable Documentation

## 5.3.4.1 OPTIX_FUNCTION_TABLE_SYMBOL

OptixFunctionTableOPTIX_FUNCTION_TABLE_SYMBOL

If the stubs inoptix_stubs.hare used, then the function table needs to be defined in exactly one
translation unit. This can be achieved by including this header file in that translation unit.

Mixing multiple SDKs in a single application will result in symbol collisions. To enable different
compilation units to use different SDKs, use OPTIX_ENABLE_SDK_MIXING.

## 5.4 Host API

Modules

•Error handling

•Device context

•Pipelines

•Modules

•Tasks

•Program groups

•Launches

•Acceleration structures

•Cooperative Vector

•Denoiser

## 5.4.1 Detailed Description

OptiX Host API.

## 5.5 Error handling

Functions

•OPTIXAPIconst char *∗* optixGetErrorName(OptixResultresult)

•OPTIXAPIconst char *∗* optixGetErrorString(OptixResultresult)

## 5.5.1 Detailed Description

5.5.2 Function Documentation

## 5.5.2.1 optixGetErrorName( )

OPTIXAPIconst char *∗* optixGetErrorName (
OptixResult *result*)

Returns a string containing the name of an error code in the enum.

Output is a string representation of the enum. For example "OPTIX_SUCCESS" for OPTIX_SUCCESS
and "OPTIX_ERROR_INVALID_VALUE" for OPTIX_ERROR_INVALID_VALUE.

If the error code is not recognized, "Unrecognized OptixResult code" is returned.

## Parameters

| in | result | OptixResult enum to generate string name for |
| --- | --- | --- |

---

See alsooptixGetErrorString

## 5.5.2.2 optixGetErrorString( )

OPTIXAPIconst char *∗* optixGetErrorString (
OptixResult *result*)

Returns the description string for an error code.

Output is a string description of the enum. For example "Success" for OPTIX_SUCCESS and "Invalid
value" for OPTIX_ERROR_INVALID_VALUE.

If the error code is not recognized, "Unrecognized OptixResult code" is returned.

## Parameters

| in | result | OptixResult enum to generate string description for |
| --- | --- | --- |

See alsooptixGetErrorName

## 5.6 Device context

•OPTIXAPIOptixResultoptixDeviceContextCreate(CUcontext fromContext, const
OptixDeviceContextOptions *∗*options,OptixDeviceContext *∗*context)
•OPTIXAPIOptixResultoptixDeviceContextDestroy(OptixDeviceContextcontext)
•OPTIXAPIOptixResultoptixDeviceContextGetProperty(OptixDeviceContextcontext,
OptixDevicePropertyproperty, void *∗*value, size_t sizeInBytes)
•OPTIXAPIOptixResultoptixDeviceContextSetLogCallback(OptixDeviceContextcontext,
OptixLogCallbackcallbackFunction, void *∗*callbackData, unsigned int callbackLevel)
•OPTIXAPIOptixResultoptixDeviceContextSetCacheEnabled(OptixDeviceContextcontext, int
enabled)
•OPTIXAPIOptixResultoptixDeviceContextSetCacheLocation(OptixDeviceContextcontext,
const char *∗*location)
•OPTIXAPIOptixResultoptixDeviceContextSetCacheDatabaseSizes(OptixDeviceContextcontext,
size_t lowWaterMark, size_t highWaterMark)
•OPTIXAPIOptixResultoptixDeviceContextGetCacheEnabled(OptixDeviceContextcontext, int
*∗*enabled)
•OPTIXAPIOptixResultoptixDeviceContextGetCacheLocation(OptixDeviceContextcontext, char
*∗*location, size_t locationSize)
•OPTIXAPIOptixResultoptixDeviceContextGetCacheDatabaseSizes(OptixDeviceContext
context, size_t *∗*lowWaterMark, size_t *∗*highWaterMark)

5.6.1 Detailed Description

5.6.2 Function Documentation

5.6.2.1 optixDeviceContextCreate( )

OPTIXAPIOptixResultoptixDeviceContextCreate (
CUcontext *fromContext,*
constOptixDeviceContextOptions *∗ options,*
OptixDeviceContext *∗ context*)

---

Create a device context associated with the CUDA context specified with 'fromContext'.

If zero is specified for 'fromContext', OptiX will use the current CUDA context. The CUDA context
should be initialized before calling optixDeviceContextCreate.

## Parameters

| in | fromContext |
| --- | --- |
| in | options |
| out | context |

## Returns

•OPTIX_ERROR_CUDA_NOT_INITIALIZED If using zero for 'fromContext' and CUDA has
not been initialized yet on the calling thread.

•OPTIX_ERROR_CUDA_ERROR CUDA operation failed.

•OPTIX_ERROR_HOST_OUT_OF_MEMORY Heap allocation failed.

•OPTIX_ERROR_INTERNAL_ERROR Internal error

## 5.6.2.2 optixDeviceContextDestroy( )

OPTIXAPIOptixResultoptixDeviceContextDestroy (
OptixDeviceContext *context*)

Destroys all CPU and GPU state associated with the device.

It will attempt to block on CUDA streams that have launch work outstanding.

Any API objects, such as OptixModule and OptixPipeline, not already destroyed will be destroyed.

Thread safety: A device context must not be destroyed while it is still in use by concurrent API calls in
other threads.

## 5.6.2.3 optixDeviceContextGetCacheDatabaseSizes( )

OPTIXAPIOptixResultoptixDeviceContextGetCacheDatabaseSizes (
OptixDeviceContext *context,*
size_t *∗ lowWaterMark,*
size_t *∗ highWaterMark*)

Returns the low and high water marks for disk cache garbage collection. If the cache has been disabled
by setting the environment variable OPTIX_CACHE_MAXSIZE=0, this function will return 0 for the
low and high water marks.

## Parameters

| in | context | the device context |
| --- | --- | --- |
| out | lowWaterMark | the low water mark |
| out | highWaterMark | the high water mark |

## 5.6.2.4 optixDeviceContextGetCacheEnabled( )

OPTIXAPIOptixResultoptixDeviceContextGetCacheEnabled (
OptixDeviceContext *context,* int *∗ enabled*)

Indicates whether the disk cache is enabled or disabled.

## Parameters

| in | context | the device context |
| --- | --- | --- |
| out | enabled | 1 if enabled,0 if disabled |

## 5.6.2.5 optixDeviceContextGetCacheLocation( )

OPTIXAPIOptixResultoptixDeviceContextGetCacheLocation (
OptixDeviceContext *context,*
char *∗ location,*
size_t *locationSize*)

Returns the location of the disk cache. If the cache has been disabled by setting the environment
variable OPTIX_CACHE_MAXSIZE=0, this function will return an empy string.

## Parameters

| in | context | the device context |
| --- | --- | --- |
| out | location | directory of disk cache, null terminated if locationSize&gt;0 |
| in | locationSize | locationSize |

## 5.6.2.6 optixDeviceContextGetProperty( )

OPTIXAPIOptixResultoptixDeviceContextGetProperty (
OptixDeviceContext *context,*
OptixDeviceProperty *property,*
void *∗ value,*
size_t *sizeInBytes*)

Query properties of a device context.

## Parameters

| in | context | the device context to query the property for |
| --- | --- | --- |
| in | property | the property to query |
| out | value | pointer to the returned |
| in | sizeInBytes | size of output |

## 5.6.2.7 optixDeviceContextSetCacheDatabaseSizes( )

OPTIXAPIOptixResultoptixDeviceContextSetCacheDatabaseSizes (
OptixDeviceContext *context,*
size_t *lowWaterMark,*
size_t *highWaterMark*)

---

Sets the low and high water marks for disk cache garbage collection.

Garbage collection is triggered when a new entry is written to the cache and the current cache data size
plus the size of the cache entry that is about to be inserted exceeds the high water mark. Garbage
collection proceeds until the size reaches the low water mark. Garbage collection will always free
enough space to insert the new entry without exceeding the low water mark. Setting either limit to
zero will disable garbage collection. An error will be returned if both limits are non-zero and the high
water mark is smaller than the low water mark.

Note that garbage collection is performed only on writes to the disk cache. No garbage collection is
triggered on disk cache initialization or immediately when calling this function, but on subsequent
inserting of data into the database.

If the size of a compiled module exceeds the value configured for the high water mark and garbage
collection is enabled, the module will not be added to the cache and a warning will be added to the log.

The high water mark can be overridden with the environment variable OPTIX_CACHE_MAXSIZE.
The environment variable takes precedence over the function parameters. The low water mark will be
set to half the value of OPTIX_CACHE_MAXSIZE. Setting OPTIX_CACHE_MAXSIZE to 0 will disable
the disk cache, but will not alter the contents of the cache. Negative and non-integer values will be
ignored.

## Parameters

| in | context | the device context |
| --- | --- | --- |
| in | lowWaterMark | the low water mark |
| in | highWaterMark | the high water mark |

## 5.6.2.8 optixDeviceContextSetCacheEnabled( )

OPTIXAPIOptixResultoptixDeviceContextSetCacheEnabled (
OptixDeviceContext *context,*
int *enabled*)

Enables or disables the disk cache.

If caching was previously disabled, enabling it will attempt to initialize the disk cache database using
the currently configured cache location. An error will be returned if initialization fails.

Note that no in-memory cache is used, so no caching behavior will be observed if the disk cache is
disabled.

The cache can be disabled by setting the environment variable OPTIX_CACHE_MAXSIZE=0. The
environment variable takes precedence over this setting. See
optixDeviceContextSetCacheDatabaseSizesfor additional information.

Note that the disk cache can be disabled by the environment variable, but it cannot be enabled via the
environment if it is disabled via the API.

## Parameters

| in | context | the device context |
| --- | --- | --- |
| in | enabled | 1 to enabled,0 to disable |

---

## 5.6.2.9 optixDeviceContextSetCacheLocation( )

OPTIXAPIOptixResultoptixDeviceContextSetCacheLocation (
OptixDeviceContext *context,*
const char *∗ location*)

Sets the location of the disk cache.

The location is specified by a directory. This directory should not be used for other purposes and will
be created if it does not exist. An error will be returned if is not possible to create the disk cache at the
specified location for any reason (e.g., the path is invalid or the directory is not writable). Caching will
be disabled if the disk cache cannot be initialized in the new location. If caching is disabled, no error
will be returned until caching is enabled. If the disk cache is located on a network file share, behavior is
undefined.

The location of the disk cache can be overridden with the environment variable OPTIX_CACHE_
PATH. The environment variable takes precedence over this setting.

The default location depends on the operating system:

•Windows: LOCALAPPDATA%\NVIDIA\OptixCache

•Linux: /var/tmp/OptixCache_<username> (or /tmp/OptixCache_<username> if the first
choice is not usable), the underscore and username suffix are omitted if the username cannot be
obtained

•MacOS X: /Library/Application Support/NVIDIA/OptixCache

## Parameters

| in | context | the device context |
| --- | --- | --- |
| in | location | directory of disk cache |

## 5.6.2.10 optixDeviceContextSetLogCallback( )

OPTIXAPIOptixResultoptixDeviceContextSetLogCallback (

OptixDeviceContext *context,*

OptixLogCallback *callbackFunction,*

unsigned int *callbackLevel*)

Sets the current log callback method.

SeeOptixLogCallbackfor more details.

Thread safety: It is guaranteed that the callback itself (callbackFunction and callbackData) are updated
atomically. It is not guaranteed that the callback itself (callbackFunction and callbackData) and the
callbackLevel are updated atomically. It is unspecified when concurrent API calls using the same
context start to make use of the new callback method.

## Parameters

| in | context | the device context |
| --- | --- | --- |
| in | callbackFunction | the callback function to call |
| in | callbackData | pointer to data passed to callback function while invoking it |
| in | callbackLevel | callback level |

---

## 5.7 Pipelines

Functions

•OPTIXAPIOptixResultoptixPipelineCreate(OptixDeviceContextcontext, const
OptixPipelineCompileOptions *∗*pipelineCompileOptions, constOptixPipelineLinkOptions
*∗*pipelineLinkOptions, constOptixProgramGroup *∗*programGroups, unsigned int
numProgramGroups, char *∗*logString, size_t *∗*logStringSize,OptixPipeline *∗*pipeline)

•OPTIXAPIOptixResultoptixPipelineDestroy(OptixPipelinepipeline)

•OPTIXAPIOptixResultoptixPipelineSetStackSizeFromCallDepths(OptixPipelinepipeline,
unsigned int maxTraceDepth, unsigned int maxContinuationCallableDepth, unsigned int
maxDirectCallableDepthFromState, unsigned int maxDirectCallableDepthFromTraversal,
unsigned int maxTraversableGraphDepth)

•OPTIXAPIOptixResultoptixPipelineSetStackSize(OptixPipelinepipeline, unsigned int
directCallableStackSizeFromTraversal, unsigned int directCallableStackSizeFromState, unsigned
int continuationStackSize, unsigned int maxTraversableGraphDepth)

•OPTIXAPIOptixResultoptixPipelineSymbolMemcpyAsync(OptixPipelinepipeline, const char
*∗*name, void *∗*mem, size_t sizeInBytes, size_t offsetInBytes,OptixPipelineSymbolMemcpyKind
kind, CUstream stream)

## 5.7.1 Detailed Description

## 5.7.2 Function Documentation

## 5.7.2.1 optixPipelineCreate( )

OPTIXAPIOptixResultoptixPipelineCreate (
OptixDeviceContext *context,*
constOptixPipelineCompileOptions *∗ pipelineCompileOptions,*
constOptixPipelineLinkOptions *∗ pipelineLinkOptions,*
constOptixProgramGroup *∗ programGroups,*
unsigned int *numProgramGroups,*
char *∗ logString,*
size_t *∗ logStringSize,*
OptixPipeline *∗ pipeline*)

logString is an optional buffer that contains compiler feedback and errors. This information is also
passed to the context logger (if enabled), however it may be difficult to correlate output to the logger to
specific API invocations when using multiple threads. The output to logString will only contain
feedback for this specific invocation of this API call.

logStringSize as input should be a pointer to the number of bytes backing logString. Upon return it
contains the length of the log message (including the null terminator) which may be greater than the
input value. In this case, the log message will be truncated to fit into logString.

If logString or logStringSize are NULL, no output is written to logString. If logStringSize points to a
value that is zero, no output is written. This does not affect output to the context logger if enabled.

## Parameters

| in | context |  |
| --- | --- | --- |
| in | pipelineCompileOptions |  |
| in | pipelineLinkOptions |  |

---

## Parameters

| in | programGroups | array of ProgramGroup objects |
| --- | --- | --- |
| in | numProgramGroups | number of ProgramGroup objects |
| out | logString | Information will be written to this string. If logStringSize&gt;0 logString will be null terminated. |
| in,out | logStringSize |  |
| out | pipeline |  |

## 5.7.2.2 optixPipelineDestroy( )

OPTIXAPIOptixResultoptixPipelineDestroy (
OptixPipeline *pipeline*)

Thread safety: A pipeline must not be destroyed while it is still in use by concurrent API calls in other
threads.

## 5.7.2.3 optixPipelineSetStackSize( )

OPTIXAPIOptixResultoptixPipelineSetStackSize (
OptixPipeline *pipeline,*
unsigned int *directCallableStackSizeFromTraversal,*
unsigned int *directCallableStackSizeFromState,*
unsigned int *continuationStackSize,*
unsigned int *maxTraversableGraphDepth*)

Sets the stack sizes for a pipeline.

Users are encouraged to see the programming guide and the implementations of the helper functions
to understand how to construct the stack sizes based on their particular needs.

If this method is not used, an internal default implementation is used. The default implementation is
correct (but not necessarily optimal) as long as the maximum depth of call trees of CC programs is at
most 2, and no DC programs or motion transforms are used.

The maxTraversableGraphDepth responds to the maximal number of traversables visited when calling
trace. Every acceleration structure and motion transform count as one level of traversal. E.g., for a
simple IAS (instance acceleration structure) -*>* GAS (geometry acceleration structure) traversal graph,
the maxTraversableGraphDepth is two. For IAS -*>* MT (motion transform) -*>* GAS, the
maxTraversableGraphDepth is three. Note that it does not matter whether a IAS or GAS has motion or
not, it always counts as one. Launching optix with exceptions turned on (seeOPTIX_EXCEPTION_
FLAG_TRACE_DEPTH) will throw an exception if the specified maxTraversableGraphDepth is too
small.

## Parameters

| in | pipeline | The pipeline to configure the stack size for. |
| --- | --- | --- |
| in | directCallableStackSizeFromTraversal | The direct stack size requirement for direct callables invoked from IS or AH. |
| in | directCallableStackSizeFromState | The direct stack size requirement for direct callables invoked from RG, MS, or CH. |
| in | continuationStackSize | The continuation stack requirement. |

---

87

## Parameters

| in | maxTraversableGraphDepth | The maximum depth of a traversable graph passed to trace. |
| --- | --- | --- |

## 5.7.2.4 optixPipelineSetStackSizeFromCallDepths( )

OPTIXAPIOptixResultoptixPipelineSetStackSizeFromCallDepths (
OptixPipeline *pipeline,*
unsigned int *maxTraceDepth,*
unsigned int *maxContinuationCallableDepth,*
unsigned int *maxDirectCallableDepthFromState,*
unsigned int *maxDirectCallableDepthFromTraversal,*
unsigned int *maxTraversableGraphDepth*)

Sets the stack size for a pipeline based on the given depth parameters.

When the pipeline is created the stack sizes for a pipeline are configured based on the depth values that
were given in theOptixPipelineLinkOptions. This method allows to reconfigure the pipeline to new
values for the maximum trace depth and the maximum callable depths which includes a recalculation
of the stack sizes.

## Parameters

| in | pipeline | The pipeline to set the stack size for. |
| --- | --- | --- |
| in | maxTraceDepth | The maximum trace recursion depth. See OptixPipelineLinkOptions::maxTraceDepth. |
| in | maxContinuationCallableDepth | The maximum depth of continuation callable call graphs. See OptixPipelineLinkOptions::maxContinuationCallableDepth. |
| in | maxDirectCallableDepthFromState | The maximum depth of direct callable call graphs called from RG, CH, MS or CC. See OptixPipelineLinkOptions::maxDirectCallableDepthFromState. |
| in | maxDirectCallableDepthFromTraversal | The maximum depth of direct callable call graphs called from IS or AH. See OptixPipelineLinkOptions::maxDirectCallableDepthFromTraversal. |
| in | maxTraversableGraphDepth | The maximum depth of a traversable graph passed to trace. |

## 5.7.2.5 optixPipelineSymbolMemcpyAsync( )

OPTIXAPIOptixResultoptixPipelineSymbolMemcpyAsync (
OptixPipeline *pipeline,*
const char *∗ name,*
void *∗ mem,*
size_t *sizeInBytes,*
size_t *offsetInBytes,*

---

88

OptixPipelineSymbolMemcpyKind *kind,*
CUstream *stream*)

Copies data from or to a global symbol in the pipeline. Depending on the given kind of the copy
operation, the mem parameter acts as the source or the target of the operation. The sizeInBytes
parameter determines how many bytes are copied. The offsetInBytes parameter determines the offset
in bytes in the target memory.

## Parameters

| in | pipeline | The pipeline to get the symbol address from/to. |
| --- | --- | --- |
| in | name | The name of the symbol to copy data from/to. |
| in | mem | The memory where to copy data from/to. Depending on the kind of the copy operation this is either a host or a device pointer. |
| in | sizeInBytes | The amount of bytes to copy. |
| in | offsetInBytes | The offset in the symbol&#x27;s memory to copy the data from/to. |
| in | kind | A flag that determines the direction of the copy operation. |
| in | stream | The CUstream to execute the asynchronous operation in. |

## 5.8 Modules

## Functions

•OPTIXAPIOptixResultoptixModuleCreate(OptixDeviceContextcontext, const
OptixModuleCompileOptions *∗*moduleCompileOptions, constOptixPipelineCompileOptions
*∗*pipelineCompileOptions, const char *∗*input, size_t inputSize, char *∗*logString, size_t
*∗*logStringSize,OptixModule *∗*module)

•OPTIXAPIOptixResultoptixModuleCreateWithTasks(OptixDeviceContextcontext, const
OptixModuleCompileOptions *∗*moduleCompileOptions, constOptixPipelineCompileOptions
*∗*pipelineCompileOptions, const char *∗*input, size_t inputSize, char *∗*logString, size_t
*∗*logStringSize,OptixModule *∗*module,OptixTask *∗*firstTask)

•OPTIXAPIOptixResultoptixModuleGetCompilationState(OptixModulemodule,
OptixModuleCompileState *∗*state)

•OPTIXAPIOptixResultoptixModuleCancelCreation(OptixModulemodule,OptixCreationFlags
flags)

•OPTIXAPIOptixResultoptixDeviceContextCancelCreations(OptixDeviceContextcontext,
OptixCreationFlagsflags)

•OPTIXAPIOptixResultoptixModuleDestroy(OptixModulemodule)

•OPTIXAPIOptixResultoptixBuiltinISModuleGet(OptixDeviceContextcontext, const
OptixModuleCompileOptions *∗*moduleCompileOptions, constOptixPipelineCompileOptions
*∗*pipelineCompileOptions, constOptixBuiltinISOptions *∗*builtinISOptions,OptixModule
*∗*builtinModule)

## 5.8.1 Detailed Description

5.8.2 Function Documentation

## 5.8.2.1 optixBuiltinISModuleGet( )

OPTIXAPIOptixResultoptixBuiltinISModuleGet (
OptixDeviceContext *context,*

---

<u>89</u>

constOptixModuleCompileOptions *∗ moduleCompileOptions,*
constOptixPipelineCompileOptions *∗ pipelineCompileOptions,*
constOptixBuiltinISOptions *∗ builtinISOptions,*
OptixModule *∗ builtinModule*)

Returns a module containing the intersection program for the built-in primitive type specified by the
builtinISOptions. This module must be used as the moduleIS for theOptixProgramGroupHitgroupin
any SBT record for that primitive type. (The entryFunctionNameIS should be null.)

## 5.8.2.2 optixDeviceContextCancelCreations( )

OPTIXAPIOptixResultoptixDeviceContextCancelCreations (
OptixDeviceContext *context,*
OptixCreationFlags *flags*)

Used to cancel creation of all modules asssociated with an OptixDeviceContext. Conditionally blocks
(seeOptixCreationFlags)

Thread safety: Safe to call from any thread

## 5.8.2.3 optixModuleCancelCreation( )

OPTIXAPIOptixResultoptixModuleCancelCreation (
OptixModule *module,*
OptixCreationFlags *flags*)

Used to cancel task-based module creation. A canceled module will transition to OPTIX_MODULE_
COMPILE_STATE_IMPENDING_FAILURE if there are unfinished tasks that have been returned to the
user, or OPTIX_MODULE_COMPILE_STATE_FAILED if all returned tasks have finished executing, at
which point it should be treated as any other module that has failed compilation. The user may
continue executing tasks of a canceled module, they will simply return OPTIX_ERROR_CREATION_
CANCELED without performing any compilation and without creating new tasks.

Conditionally blocks (seeOptixCreationFlags)

Thread safety: Safe to call from any thread

## 5.8.2.4 optixModuleCreate( )

OPTIXAPIOptixResultoptixModuleCreate (
OptixDeviceContext *context,*
constOptixModuleCompileOptions *∗ moduleCompileOptions,*
constOptixPipelineCompileOptions *∗ pipelineCompileOptions,*
const char *∗ input,*
size_t *inputSize,*
char *∗ logString,*
size_t *∗ logStringSize,*
OptixModule *∗ module*)

Compiling programs into a module. These programs can be passed in as either PTX or OptiX-IR.
See the Programming Guide for details, as well as how to generate these encodings from CUDA
sources.

logString is an optional buffer that contains compiler feedback and errors. This information is also

---

90

passed to the context logger (if enabled), however it may be difficult to correlate output to the logger to
specific API invocations when using multiple threads. The output to logString will only contain
feedback for this specific invocation of this API call.

logStringSize as input should be a pointer to the number of bytes backing logString. Upon return it
contains the length of the log message (including the null terminator) which may be greater than the
input value. In this case, the log message will be truncated to fit into logString.

If logString or logStringSize are NULL, no output is written to logString. If logStringSize points to a
value that is zero, no output is written. This does not affect output to the context logger if enabled.

## Parameters

| in | context |  |
| --- | --- | --- |
| in | moduleCompileOptions |  |
| in | pipelineCompileOptions | All modules in a pipeline need to use the same values for the pipeline compile options. |
| in | input | Pointer to the input code. |
| in | inputSize | Parsing proceeds up to inputSize characters. Or, when reading PTX input,the first NUL byte,whichever occurs first. |
| out | logString | Information will be written to this string.If logStringSize&gt;0logStringwillbe null terminated. |
| in,out | logStringSize |  |
| out | module |  |

## Returns

OPTIX_ERROR_INVALID_VALUE - context is 0, moduleCompileOptions is 0,
pipelineCompileOptions is 0, input is 0, module is 0.

## 5.8.2.5 optixModuleCreateWithTasks( )

OPTIXAPIOptixResultoptixModuleCreateWithTasks (
OptixDeviceContext *context,*
constOptixModuleCompileOptions *∗ moduleCompileOptions,*
constOptixPipelineCompileOptions *∗ pipelineCompileOptions,*
const char *∗ input,*
size_t *inputSize,*
char *∗ logString,*
size_t *∗ logStringSize,*
OptixModule *∗ module,*
OptixTask *∗ firstTask*)

This function is designed to do just enough work to create the OptixTask return parameter and is
expected to be fast enough run without needing parallel execution. A single thread could generate all
the OptixTask objects for further processing in a work pool.

Options are similar tooptixModuleCreate( ), aside from the return parameter, firstTask.

The memory used to hold the input should be live until all tasks are finished.

---

<u>91</u>

It is illegal to calloptixModuleDestroy( )if any OptixTask objects are currently being executed. In that
case OPTIX_ERROR_ILLEGAL_DURING_TASK_EXECUTE will be returned.

If an invocation of optixTaskExecute fails, the OptixModule will be marked as OPTIX_MODULE_
COMPILE_STATE_IMPENDING_FAILURE if there are outstanding tasks or OPTIX_MODULE_
COMPILE_STATE_FAILURE if there are no outstanding tasks. Subsequent calls tooptixTaskExecute( )
may execute additional work to collect compilation errors generated from the input. Currently
executing tasks will not necessarily be terminated immediately but at the next opportunity.

Logging will continue to be directed to the logger installed with the OptixDeviceContext. If logString is
provided tooptixModuleCreateWithTasks( ), it will contain all the compiler feedback from all executed
tasks. The lifetime of the memory pointed to by logString should extend from calling
optixModuleCreateWithTasks( )to when the compilation state is either OPTIX_MODULE_COMPILE_
STATE_FAILURE or OPTIX_MODULE_COMPILE_STATE_COMPLETED. OptiX will not write to the
logString outside of execution ofoptixModuleCreateWithTasks( )oroptixTaskExecute( ). If the
compilation state is OPTIX_MODULE_COMPILE_STATE_IMPENDING_FAILURE and no further
execution ofoptixTaskExecute( )is performed the logString may be reclaimed by the application before
callingoptixModuleDestroy( ). The contents of logString will contain output from currently completed
tasks.

All OptixTask objects associated with a given OptixModule will be cleaned up when
optixModuleDestroy( )is called regardless of whether the compilation was successful or not. If the
compilation state is OPTIX_MODULE_COMPILE_STATE_IMPENDING_FAILURE, any unstarted
OptixTask objects do not need to be executed though there is no harm doing so.

See alsooptixModuleCreate

## 5.8.2.6 optixModuleDestroy( )

OPTIXAPIOptixResultoptixModuleDestroy (
OptixModule *module*)

Call for OptixModule objects created with optixModuleCreate and optixModuleDeserialize.

Modules must not be destroyed while they are still used by any program group.

Thread safety: A module must not be destroyed while it is still in use by concurrent API calls in other
threads.

## 5.8.2.7 optixModuleGetCompilationState( )

OPTIXAPIOptixResultoptixModuleGetCompilationState (
OptixModule *module,*
OptixModuleCompileState *∗ state*)

When creating a module with tasks, the current state of the module can be queried using this function.
Thread safety: Safe to call from any thread until optixModuleDestroy is called.

See alsooptixModuleCreateWithTasks

## 5.9 Tasks

Functions

•OPTIXAPIOptixResultoptixTaskExecute(OptixTasktask,OptixTask *∗*additionalTasks, unsigned
int maxNumAdditionalTasks, unsigned int *∗*numAdditionalTasksCreated)

•OPTIXAPIOptixResultoptixTaskGetSerializationKey(OptixTasktask, void *∗*key, size_t *∗*size)

•OPTIXAPIOptixResultoptixTaskSerializeOutput(OptixTasktask, void *∗*data, size_t *∗*size)

---

92

•OPTIXAPIOptixResultoptixTaskDeserializeOutput(OptixTasktask, const void *∗*data, size_t
size,OptixTask *∗*additionalTasks, unsigned int maxNumAdditionalTasks, unsigned int
*∗*numAdditionalTasksCreated)

## 5.9.1 Detailed Description

5.9.2 Function Documentation

## 5.9.2.1 optixTaskDeserializeOutput( )

OPTIXAPIOptixResultoptixTaskDeserializeOutput (
OptixTask *task,*
const void *∗ data,*
size_t *size,*
OptixTask *∗ additionalTasks,*
unsigned int *maxNumAdditionalTasks,*
unsigned int *∗ numAdditionalTasksCreated*)

Given the serialized task output, deserialize it and return potential new dependent tasks similar to
optixTaskExecute( ). CallingoptixTaskDeserializeOutput( )on a completed (either executed or
deserialized) task will return an error.

## Parameters

| in | task | the OptixTask which to be deserialized |
| --- | --- | --- |
| in | data | the deserialized task&#x27;s output |
| in | size | the size of the deserialized task&#x27;s output |
| in | additionalTasks | pointer to array of OptixTask objects to be filled in |
| in | maxNumAdditionalTasks | maximum number of additional OptixTask objects |
| out | numAdditionalTasksCreated | number of OptixTask objects created by OptiX and written into additionalTasks |

## 5.9.2.2 optixTaskExecute( )

OPTIXAPIOptixResultoptixTaskExecute (
OptixTask *task,*
OptixTask *∗ additionalTasks,*
unsigned int *maxNumAdditionalTasks,*
unsigned int *∗ numAdditionalTasksCreated*)

Each OptixTask should be executed withoptixTaskExecute( ). If additional parallel work is found, new
OptixTask objects will be returned in additionalTasks along with the number of additional tasks in
numAdditionalTasksCreated. The parameter additionalTasks should point to a user allocated array of
minimum size maxNumAdditionalTasks. OptiX can generate upto maxNumAdditionalTasks
additional tasks.

Each task can be executed in parallel and in any order.

Thread safety: Safe to call from any thread untiloptixModuleDestroy( )is called for any associated task.
See alsooptixModuleCreateWithTasks

---

93

Parameters

| in | task | the OptixTask to execute |
| --- | --- | --- |
| in | additionalTasks | pointer to array of OptixTask objects to be filled in |
| in | maxNumAdditionalTasks | maximum number of additional OptixTask objects |
| out | numAdditionalTasksCreated | number of OptixTask objects created by OptiX and written into additionalTasks |

## 5.9.2.3 optixTaskGetSerializationKey( )

OPTIXAPIOptixResultoptixTaskGetSerializationKey (
OptixTask *task,*
void *∗ key,*
size_t *∗ size*)

Retrieve the task's serialization key and its size. It is expected to call this function twice. Once to get the
size and once to retrieve the key after space for it has been allocated. If the size of the key will be zero,
the task will not be serializable and the task should be executed throughoptixTaskExecute( ).

## Parameters

| in | task | the OptixTask which key to retrieve |
| --- | --- | --- |
| out | key | characters representing the key without string-terminating&#x27;\0&#x27;. If nullptr, no output will be written |
| out | size | size of the key. Will be 0 for non-serializable tasks. |

## Returns

success

## 5.9.2.4 optixTaskSerializeOutput( )

OPTIXAPIOptixResultoptixTaskSerializeOutput (
OptixTask *task,*
void *∗ data,*
size_t *∗ size*)

Retrieve the serialized data of the task's output. It is expected to call this function twice. Once to get the
size and once to retrieve the data after space for it has been allocated. Calling
optixTaskSerializeOutput( )before callingoptixTaskExecute( )will return an error. Calling
optixTaskSerializeOutput( )after callingoptixTaskDeserializeOutput( )will return an error.

## Parameters

| in | task | the OptixTask which output data to retrieve |
| --- | --- | --- |
| out | data | allocated space big enough to hold the output. If nullptr, no output will be written |
| out | size | size of the data. Will be 0 for non-serializable tasks. |

---

94

## 5.10 Program groups

Functions

•OPTIXAPIOptixResultoptixProgramGroupGetStackSize(OptixProgramGroupprogramGroup,
OptixStackSizes *∗*stackSizes,OptixPipelinepipeline)

•OPTIXAPIOptixResultoptixProgramGroupCreate(OptixDeviceContextcontext, const
OptixProgramGroupDesc *∗*programDescriptions, unsigned int numProgramGroups, const
OptixProgramGroupOptions *∗*options, char *∗*logString, size_t *∗*logStringSize,
OptixProgramGroup *∗*programGroups)

•OPTIXAPIOptixResultoptixProgramGroupDestroy(OptixProgramGroupprogramGroup)

•OPTIXAPIOptixResultoptixSbtRecordPackHeader(OptixProgramGroupprogramGroup, void
*∗*sbtRecordHeaderHostPointer)

5.10.1 Detailed Description

5.10.2 Function Documentation

## 5.10.2.1 optixProgramGroupCreate( )

OPTIXAPIOptixResultoptixProgramGroupCreate (
OptixDeviceContext *context,*
constOptixProgramGroupDesc *∗ programDescriptions,*
unsigned int *numProgramGroups,*
constOptixProgramGroupOptions *∗ options,*
char *∗ logString,*
size_t *∗ logStringSize,*
OptixProgramGroup *∗ programGroups*)

logString is an optional buffer that contains compiler feedback and errors. This information is also
passed to the context logger (if enabled), however it may be difficult to correlate output to the logger to
specific API invocations when using multiple threads. The output to logString will only contain
feedback for this specific invocation of this API call.

logStringSize as input should be a pointer to the number of bytes backing logString. Upon return it
contains the length of the log message (including the null terminator) which may be greater than the
input value. In this case, the log message will be truncated to fit into logString.

If logString or logStringSize are NULL, no output is written to logString. If logStringSize points to a
value that is zero, no output is written. This does not affect output to the context logger if enabled.

Creates numProgramGroups OptiXProgramGroup objects from the specifiedOptixProgramGroupDesc
array. The size of the arrays must match.

Parameters

| in | context |  |
| --- | --- | --- |
| in | programDescriptions | N * OptixProgramGroupDesc |
| in | numProgramGroups | N |
| in | options |  |
| out | logString | Information will be written to this string. If logStringSize&gt;0 logString will be null terminated. |
| in,out | logStringSize |  |

---

## Parameters

| out | programGroups |  |
| --- | --- | --- |

## 5.10.2.2 optixProgramGroupDestroy( )

OPTIXAPIOptixResultoptixProgramGroupDestroy (
OptixProgramGroup *programGroup*)

Thread safety: A program group must not be destroyed while it is still in use by concurrent API calls in
other threads.

## 5.10.2.3 optixProgramGroupGetStackSize( )

OPTIXAPIOptixResultoptixProgramGroupGetStackSize (
OptixProgramGroup *programGroup,*
OptixStackSizes *∗ stackSizes,*
OptixPipeline *pipeline*)

Returns the stack sizes for the given program group. When programs in this programGroup are
relying on external functions, the corresponding stack sizes can only be correctly retrieved when all
functions are known after linking, i.e. when a pipeline has been created. When pipeline is set to
NULL, the stack size will be calculated excluding external functions. In this case a warning will be
issued if external functions are referenced by the OptixModule.

## Parameters

| in | programGroup | the program group |
| --- | --- | --- |
| out | stackSizes | the corresponding stack sizes |
| in | pipeline | considering the program group within the given pipeline, can be NULL |

## 5.10.2.4 optixSbtRecordPackHeader( )

OPTIXAPIOptixResultoptixSbtRecordPackHeader (
OptixProgramGroup *programGroup,*
void *∗ sbtRecordHeaderHostPointer*)

## Parameters

| in | programGroup | the program group containing the program(s) |
| --- | --- | --- |
| out | sbtRecordHeaderHostPointer | the result sbt record header |

## 5.11 Launches

## Functions

•OPTIXAPIOptixResultoptixLaunch(OptixPipelinepipeline, CUstream stream,CUdeviceptr
pipelineParams, size_t pipelineParamsSize, constOptixShaderBindingTable *∗*sbt, unsigned int
width, unsigned int height, unsigned int depth)

---

## 5.11.1 Detailed Description

5.11.2 Function Documentation

## 5.11.2.1 optixLaunch( )

OPTIXAPIOptixResultoptixLaunch (
OptixPipeline *pipeline,*
CUstream *stream,*
CUdeviceptr *pipelineParams,*
size_t *pipelineParamsSize,*
constOptixShaderBindingTable *∗ sbt,*
unsigned int *width,*
unsigned int *height,*
unsigned int *depth*)

Where the magic happens.

The stream and pipeline must belong to the same device context. Multiple launches may be issues in
parallel from multiple threads to different streams.

pipelineParamsSize number of bytes are copied from the device memory pointed to by pipelineParams
before launch. It is an error if pipelineParamsSize is greater than the size of the variable declared in
modules and identified byOptixPipelineCompileOptions::pipelineLaunchParamsVariableName. If the
launch params variable was optimized out or not found in the modules linked to the pipeline then the
pipelineParams and pipelineParamsSize parameters are ignored.

sbt points to the shader binding table, which defines shader groupings and their resources. See the SBT
spec.

## Parameters

| in | pipeline |  |
| --- | --- | --- |
| in | stream |  |
| in | pipelineParams |  |
| in | pipelineParamsSize |  |
| in | sbt |  |
| in | width | number of elements to compute |
| in | height | number of elements to compute |
| in | depth | number of elements to compute |

Thread safety: In the current implementation concurrent launches to the same pipeline are not
supported. Concurrent launches require separate OptixPipeline objects.

## 5.12 Acceleration structures

## Functions

•OPTIXAPIOptixResultoptixAccelComputeMemoryUsage(OptixDeviceContextcontext, const
OptixAccelBuildOptions *∗*accelOptions, constOptixBuildInput *∗*buildInputs, unsigned int
numBuildInputs,OptixAccelBufferSizes *∗*bufferSizes)

•OPTIXAPIOptixResultoptixAccelBuild(OptixDeviceContextcontext, CUstream stream, const

---

OptixAccelBuildOptions *accelOptions, const OptixBuildInput *buildInputs, unsigned int numBuildInputs, CUdeviceptr tempBuffer, size_t tempBufferSizeInBytes, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle, const OptixAccelEmitDesc *emittedProperties, unsigned int numEmittedProperties)

• OPTIXAPI OptixResult optixAccelGetRelocationInfo (OptixDeviceContext context, OptixTraversableHandle handle, OptixRelocationInfo *info)
• OPTIXAPI OptixResult optixCheckRelocationCompatibility (OptixDeviceContext context, const OptixRelocationInfo *info, int *compatible)
• OPTIXAPI OptixResult optixAccelRelocate (OptixDeviceContext context, CUstream stream, const OptixRelocationInfo *info, const OptixRelocateInput *relocateInputs, size_t numRelocateInputs, CUdeviceptr targetAccel, size_t targetAccelSizeInBytes, OptixTraversableHandle *targetHandle)
• OPTIXAPI OptixResult optixAccelCompact (OptixDeviceContext context, CUstream stream, OptixTraversableHandle inputHandle, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle)
• OPTIXAPI OptixResult optixAccelEmitProperty (OptixDeviceContext context, CUstream stream, OptixTraversableHandle handle, const OptixAccelEmitDesc *emittedProperty)
• OPTIXAPI OptixResult optixConvertPointerToTraversableHandle (OptixDeviceContext onDevice, CUdeviceptr pointer, OptixTraversableType traversableType, OptixTraversableHandle *traversableHandle)
• OPTIXAPI OptixResult optixOpacityMicromapArrayComputeMemoryUsage (OptixDeviceContext context, const OptixOpacityMicromapArrayBuildInput *buildInput, OptixMicromapBufferSizes *bufferSizes)
• OPTIXAPI OptixResult optixOpacityMicromapArrayBuild (OptixDeviceContext context, CUstream stream, const OptixOpacityMicromapArrayBuildInput *buildInput, const OptixMicromapBuffers *buffers)
• OPTIXAPI OptixResult optixOpacityMicromapArrayGetRelocationInfo (OptixDeviceContext context, CUdeviceptr opacityMicromapArray, OptixRelocationInfo *info)
• OPTIXAPI OptixResult optixOpacityMicromapArrayRelocate (OptixDeviceContext context, CUstream stream, const OptixRelocationInfo *info, CUdeviceptr targetOpacityMicromapArray, size_t targetOpacityMicromapArraySizeInBytes)
• OPTIXAPI OptixResult optixClusterAccelComputeMemoryUsage (OptixDeviceContext context, OptixClusterAccelBuildMode buildMode, const OptixClusterAccelBuildInput *buildInput, OptixAccelBufferSizes *bufferSizes)
• OPTIXAPI OptixResult optixClusterAccelBuild (OptixDeviceContext context, CUstream stream, const OptixClusterAccelBuildModeDesc *buildModeDesc, const OptixClusterAccelBuildInput *buildInput, CUdeviceptr argsArray, CUdeviceptr argsCount, unsigned int argsStrideInBytes)

## 5.12.1 Detailed Description

## 5.12.2 Function Documentation

## 5.12.2.1 optixAccelBuild()

OPTIXAPI OptixResult optixAccelBuild (
    OptixDeviceContext context,
    CUstream stream,
    const OptixAccelBuildOptions * accelOptions,
    const OptixBuildInput * buildInputs,
    unsigned int numBuildInputs,
    CUdeviceptr tempBuffer,
    size_t tempBufferSizeInBytes,

---

CUdeviceptr *outputBuffer,*
size_t *outputBufferSizeInBytes,*
OptixTraversableHandle *∗ outputHandle,*
constOptixAccelEmitDesc *∗ emittedProperties,*
unsigned int *numEmittedProperties*)

## Parameters

| in | context |  |
| --- | --- | --- |
| in | stream |  |
| in | accelOptions | accel options |
| in | buildInputs | an array of OptixBuildInput objects |
| in | numBuildInputs | must be&gt;=1 for GAS,and==1 for IAS |
| in | tempBuffer | must be a multiple of OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT |
| in | tempBufferSizeInBytes |  |
| in | outputBuffer | must be a multiple of OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT |
| in | outputBufferSizeInBytes |  |
| out | outputHandle |  |
| in | emittedProperties | types of requested properties and output buffers |
| in | numEmittedProperties | number of post-build properties to populate(may be zero) |

## 5.12.2.2 optixAccelCompact( )

OPTIXAPIOptixResultoptixAccelCompact (
OptixDeviceContext *context,*
CUstream *stream,*
OptixTraversableHandle *inputHandle,*
CUdeviceptr *outputBuffer,*
size_t *outputBufferSizeInBytes,*
OptixTraversableHandle *∗ outputHandle*)

After building an acceleration structure, it can be copied in a compacted form to reduce memory. In
order to be compacted, OPTIX_BUILD_FLAG_ALLOW_COMPACTION must be supplied in
OptixAccelBuildOptions::buildFlagspassed to optixAccelBuild.

'outputBuffer' is the pointer to where the compacted acceleration structure will be written. This pointer
must be a multiple of OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT.

The size of the memory specified in 'outputBufferSizeInBytes' should be at least the value computed
using the OPTIX_PROPERTY_TYPE_COMPACTED_SIZE that was reported during optixAccelBuild.

Parameters

| in | context |
| --- | --- |
| in | stream |
| in | inputHandle |
| in | outputBuffer |

---

## Parameters

| in | outputBufferSizeInBytes |
| --- | --- |
| out | outputHandle |

## 5.12.2.3 optixAccelComputeMemoryUsage( )

OPTIXAPIOptixResultoptixAccelComputeMemoryUsage (
OptixDeviceContext *context,*
constOptixAccelBuildOptions *∗ accelOptions,*
constOptixBuildInput *∗ buildInputs,*
unsigned int *numBuildInputs,*
OptixAccelBufferSizes *∗ bufferSizes*)

## Parameters

| in | context |  |
| --- | --- | --- |
| in | accelOptions | options for the accel build |
| in | buildInputs | an array of OptixBuildInput objects |
| in | numBuildInputs | number of elements in buildInputs (must be at least 1) |
| out | bufferSizes | fills in buffer sizes |

## 5.12.2.4 optixAccelEmitProperty( )

OPTIXAPIOptixResultoptixAccelEmitProperty (
OptixDeviceContext *context,*
CUstream *stream,*
OptixTraversableHandle *handle,*
constOptixAccelEmitDesc *∗ emittedProperty*)

Emit a single property after an acceleration structure was built. The result buffer of the '
emittedProperty' needs to be large enough to hold the requested property (.

See alsoOptixAccelPropertyType).

## Parameters

| in | context |  |
| --- | --- | --- |
| in | stream |  |
| in | handle |  |
| in | emittedProperty | type of requested property and output buffer |

## 5.12.2.5 optixAccelGetRelocationInfo( )

OPTIXAPIOptixResultoptixAccelGetRelocationInfo (
OptixDeviceContext *context,*
OptixTraversableHandle *handle,*

---

OptixRelocationInfo *∗ info*)

Obtain relocation information, stored inOptixRelocationInfo, for a given context and acceleration
structure's traversable handle.

The relocation information can be passed to optixCheckRelocationCompatibility to determine if an
acceleration structure, referenced by 'handle', can be relocated to a different device's memory space (see
optixCheckRelocationCompatibility).

When used with optixAccelRelocate, it provides data necessary for doing the relocation.

If the acceleration structure data associated with 'handle' is copied multiple times, the same
OptixRelocationInfocan also be used on all copies.

## Parameters

| in | context |
| --- | --- |
| in | handle |
| out | info |

## Returns

OPTIX_ERROR_INVALID_VALUE will be returned for traversable handles that are not from
acceleration structure builds.

## 5.12.2.6 optixAccelRelocate( )

OPTIXAPIOptixResultoptixAccelRelocate (
OptixDeviceContext *context,*
CUstream *stream,*
constOptixRelocationInfo *∗ info,*
constOptixRelocateInput *∗ relocateInputs,*
size_t *numRelocateInputs,*
CUdeviceptr *targetAccel,*
size_t *targetAccelSizeInBytes,*
OptixTraversableHandle *∗ targetHandle*)

optixAccelRelocate is called to update the acceleration structure after it has been relocated. Relocation
is necessary when the acceleration structure's location in device memory has changed.
optixAccelRelocate does not copy the memory. This function only operates on the relocated memory
whose new location is specified by 'targetAccel'. optixAccelRelocate also returns the new
OptixTraversableHandle associated with 'targetAccel'. The original memory (source) is not required to
be valid, only theOptixRelocationInfo.

Before calling optixAccelRelocate, optixCheckRelocationCompatibility should be called to ensure the
copy will be compatible with the destination device context.

The memory pointed to by 'targetAccel' should be allocated with the same size as the source
acceleration. Similar to the 'outputBuffer' used in optixAccelBuild, this pointer must be a multiple of
OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT.

The memory in 'targetAccel' must be allocated as long as the accel is in use.

The instance traversables referenced by an IAS and the micromaps referenced by a triangle GAS may
themselves require relocation. 'relocateInputs' and 'numRelocateInputs' should be used to specify the relocated traversables and micromaps. After relocation, the relocated accel will reference these
relocated traversables and micromaps instead of their sources. The number of relocate inputs
'numRelocateInputs' must match the number of build inputs 'numBuildInputs' used to build the source
accel. Relocation inputs correspond with build inputs used to build the source accel and should appear
in the same order (seeoptixAccelBuild). 'relocateInputs' and 'numRelocateInputs' may be zero,
preserving any references to traversables and micromaps from the source accel.

## Parameters

| in | context |
| --- | --- |
| in | stream |
| in | info |
| in | relocateInputs |
| in | numRelocateInputs |
| in | targetAccel |
| in | targetAccelSizeInBytes |
| out | targetHandle |

## 5.12.2.7 optixCheckRelocationCompatibility( )

OPTIXAPIOptixResultoptixCheckRelocationCompatibility (
OptixDeviceContext *context,*
constOptixRelocationInfo *∗ info,*
int *∗ compatible*)

Checks if an optix data structure built using another OptixDeviceContext (that was used to fill in 'info')
is compatible with the OptixDeviceContext specified in the 'context' parameter.

Any device is always compatible with itself.

Parameters

| in | context |  |
| --- | --- | --- |
| in | info |  |
| out | compatible | If OPTIX_SUCCESS is returned'compatible' will have the value of either:
•0: This context is not compatible with the optix data structure associated with'info'.
•1: This context is compatible. |

## 5.12.2.8 optixClusterAccelBuild( )

OPTIXAPIOptixResultoptixClusterAccelBuild (
OptixDeviceContext *context,*
CUstream *stream,*
constOptixClusterAccelBuildModeDesc *∗ buildModeDesc,*
constOptixClusterAccelBuildInput *∗ buildInput,*
CUdeviceptr *argsArray,*

---

CUdeviceptr *argsCount,*

unsigned int *argsStrideInBytes*)

Entry point to building one type of cluster objects: a CLAS, a Cluster template, or a GAS-over-CLAS.
This is an indirect build function: all build arguments are read from device memory, with only the
output location, type of build and limits passed on the host. This is a multi build function: more than
one object can be built at once, but only of one type. The supplied limits must bound the inputs (Args)
of all builds. Output buffer size constraints for implicit and explicit builds: implicit: The output and
temp buffer must be at least as big as reported by a corresponding

optixClusterAccelComputeMemoryUsage call. explicit: The output buffers must be at least as big as
reported by a corresponding optixClusterAccelBuild call with the getSize mode and all device data
supplied. The temp buffer must be at least as big as reported by a corresponding

optixClusterAccelComputeMemoryUsage call. getSize: No output buffer is used. The temp buffer
must be at least as big as reported by a corresponding optixClusterAccelComputeMemoryUsage call.
Consequently, calling optixClusterAccelBuild with the getSize mode and subsequently building with
the explicit mode is more memory efficient, but slower compared to building with the implicit mode.

## Parameters

| in | context |  |
| --- | --- | --- |
| in | stream |  |
| in | buildModeDesc | A single input, describes where to write data for the selected build mode |
| in | buildInput | A single input, describes the type of object to build and limits over all objects&#x27; arguments |
| in | argsArray | Pointer to arguments array in device memory, describes each object to build: OptixClusterAccelBuildInputTrianglesArgs when using OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLESOptixClusterAccelBuildInputTrianglesArgs when using OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLESOptixClusterAccelBuildInputGridsArgs when using OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_GRIDSOptixClusterAccelBuildInputTemplatesArgs when using OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TEMPLATESOptixClusterAccelBuildInputClustersArgs when using OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS |
| in | argsCount | Optional pointer to device memory, storing the number of objects to build, if null is provided, uses maxArgCount from buildInput |
| in | argsStrideInBytes | Optional stride of args objects, if null is provided, uses natural stride of Args type |

## 5.12.2.9 optixClusterAccelComputeMemoryUsage( )

OPTIXAPIOptixResultoptixClusterAccelComputeMemoryUsage (

OptixDeviceContext *context,*

OptixClusterAccelBuildMode *buildMode,*

$$
\ {\tt c o c s s t\ \ O p t i x C l u s t e r A c c e l B u u l l d I n p u t\ *\ b u i\ d2I n u u}{\tt*t},
$$

$$
\ \ \ \ \ \ \ \ \ \ \\\ \ \ \ \ \ \ \ \ {sf A}c{\sf c l l B u f f e r S i z e s}\ \ *\ \ {\ b u r r e r s i z e s}\ \ *\ {\it u}u{\it\ b u r r e r s i z e s}\ \ )
$$

Host side conservative memory computation for a subsequent optixClusterAccelBuild call with the
same build mode and input. For implicit builds, the output buffer size contains the required size for
holding all build outputs as specified in buildInput-*>*maxArgsCount. For explicit builds, the output buffer size contains the required size for holding a single build output. The temp buffer of any
optixClusterAccelBuild call must be at least as big as reported by
optixClusterAccelComputeMemoryUsage. optixClusterAccelComputeMemoryUsage always returns 0
forOptixAccelBufferSizes::tempUpdateSizeInBytes.

Parameters

| in | context |  |
| --- | --- | --- |
| in | buildMode | Select the kind of output target(implicit:single buffer,explicit:per-build buffers,getSize:compact size computation for future explicit builds) |
| in | buildInput | A single input,describes the type of object to build and limits over all objects&#x27; arguments |
| out | bufferSizes |  |

## 5.12.2.10 optixConvertPointerToTraversableHandle( )

OPTIXAPIOptixResultoptixConvertPointerToTraversableHandle (
OptixDeviceContext *onDevice,*
CUdeviceptr *pointer,*
OptixTraversableType *traversableType,*
OptixTraversableHandle *∗ traversableHandle*)

Parameters

| in | onDevice |  |
| --- | --- | --- |
| in | pointer | pointer to traversable allocated in OptixDeviceContext. This pointer must be a multiple of OPTIX_TRANSFORM_BYTE_ALIGNMENT |
| in | traversableType | Type of OptixTraversableHandle to create |
| out | traversableHandle | traversable handle. traversableHandle must be in host memory |

## 5.12.2.11 optixOpacityMicromapArrayBuild( )

OPTIXAPIOptixResultoptixOpacityMicromapArrayBuild (

OptixDeviceContext *context,*
CUstream *stream,*
constOptixOpacityMicromapArrayBuildInput *∗ buildInput,*
constOptixMicromapBuffers *∗ buffers*)

Construct an array of Opacity Micromaps.

Each triangle within an instance/GAS may reference one opacity micromap to give finer control over
∧
alpha behavior. A opacity micromap consists of a set of 4 N micro-triangles in a triangular uniform
barycentric grid. Multiple opacity micromaps are collected (built) into a opacity micromap array with
this function. Each geometry in a GAS may bind a single opacity micromap array and can use opacity
micromaps from that array only.

Each micro-triangle within a opacity micromap can be in one of four states: Transparent, Opaque,
Unknown-Transparent or Unknown-Opaque. During traversal, if a triangle with a opacity micromap
attached is intersected, the opacity micromap is queried to categorize the hit as either opaque,
unknown (alpha) or a miss. Geometry, ray or instance flags that modify the alpha/opaque behavior are applied *after* this opacity micromap query.

The opacity micromap query may operate in 2-state mode (alpha testing) or 4-state mode (AHS
culling), depending on the opacity micromap type and ray/instance flags. When operating in 2-state
mode, alpha hits will not be reported, and transparent and opaque hits must be accurate.

## Parameters

| in | context |  |
| --- | --- | --- |
| in | stream |  |
| in | buildInput | a single build input object referencing many opacity micromaps |
| in | buffers | the buffers used for build |

## 5.12.2.12 optixOpacityMicromapArrayComputeMemoryUsage( )

OPTIXAPIOptixResultoptixOpacityMicromapArrayComputeMemoryUsage (
OptixDeviceContext *context,*
constOptixOpacityMicromapArrayBuildInput *∗ buildInput,*
OptixMicromapBufferSizes *∗ bufferSizes*)

Determine the amount of memory necessary for a Opacity Micromap Array build.

## Parameters

| in | context |
| --- | --- |
| in | buildInput |
| out | bufferSizes |

## 5.12.2.13 optixOpacityMicromapArrayGetRelocationInfo( )

OPTIXAPIOptixResultoptixOpacityMicromapArrayGetRelocationInfo (
OptixDeviceContext *context,*
CUdeviceptr *opacityMicromapArray,*
OptixRelocationInfo *∗ info*)

Obtain relocation information, stored inOptixRelocationInfo, for a given context and opacity
micromap array.

The relocation information can be passed to optixCheckRelocationCompatibility to determine if a
opacity micromap array, referenced by buffers, can be relocated to a different device's memory space
(seeoptixCheckRelocationCompatibility).

When used with optixOpacityMicromapArrayRelocate, it provides data necessary for doing the
relocation.

If the opacity micromap array data associated with 'opacityMicromapArray' is copied multiple times,
the sameOptixRelocationInfocan also be used on all copies.

Parameters

| in | context |
| --- | --- |
| in | opacityMicromapArray |

---

<u>105</u>

## Parameters

| out | info |
| --- | --- |

## 5.12.2.14 optixOpacityMicromapArrayRelocate( )

OPTIXAPIOptixResultoptixOpacityMicromapArrayRelocate (
OptixDeviceContext *context,*
CUstream *stream,*
constOptixRelocationInfo *∗ info,*
CUdeviceptr *targetOpacityMicromapArray,*
size_t *targetOpacityMicromapArraySizeInBytes*)

optixOpacityMicromapArrayRelocate is called to update the opacity micromap array after it has been
relocated. Relocation is necessary when the opacity micromap array's location in device memory has
changed. optixOpacityMicromapArrayRelocate does not copy the memory. This function only
operates on the relocated memory whose new location is specified by 'targetOpacityMicromapArray'.
The original memory (source) is not required to be valid, only theOptixRelocationInfo.

Before calling optixOpacityMicromapArrayRelocate, optixCheckRelocationCompatibility should be
called to ensure the copy will be compatible with the destination device context.

The memory pointed to by 'targetOpacityMicromapArray' should be allocated with the same size as
the source opacity micromap array. Similar to the 'OptixMicromapBuffers::output' used in
optixOpacityMicromapArrayBuild, this pointer must be a multiple of OPTIX_OPACITY_MICROMAP_
ARRAY_BUFFER_BYTE_ALIGNMENT.

The memory in 'targetOpacityMicromapArray' must be allocated as long as the opacity micromap
array is in use.

Note that any Acceleration Structures build using the original memory (source) as input will still be
associated with this original memory. To associate an existing (possibly relocated) Acceleration
Structures with the relocated opacity micromap array, use optixAccelBuild to update the existing
Acceleration Structures (See OPTIX_BUILD_OPERATION_UPDATE)

## Parameters

| in | context |
| --- | --- |
| in | stream |
| in | info |
| in | targetOpacityMicromapArray |
| in | targetOpacityMicromapArraySizeInBytes |

## 5.13 Cooperative Vector

## Functions

•OPTIXAPIOptixResultoptixCoopVecMatrixConvert(OptixDeviceContextcontext, CUstream
stream, unsigned int numNetworks, constOptixNetworkDescription *∗*inputNetworkDescription,
CUdeviceptrinputNetworks, size_t inputNetworkStrideInBytes, constOptixNetworkDescription
*∗*outputNetworkDescription,CUdeviceptroutputNetworks, size_t outputNetworkStrideInBytes)

•OPTIXAPIOptixResultoptixCoopVecMatrixComputeSize(OptixDeviceContextcontext,
unsigned int N, unsigned int K,OptixCoopVecElemTypeelementType,

---

106

OptixCoopVecMatrixLayoutlayout, size_t rowColumnStrideInBytes, size_t *∗*sizeInBytes)

## 5.13.1 Detailed Description

## 5.13.2 Function Documentation

## 5.13.2.1 optixCoopVecMatrixComputeSize( )

OPTIXAPIOptixResultoptixCoopVecMatrixComputeSize (
OptixDeviceContext *context,*
unsigned int *N,*
unsigned int *K,*
OptixCoopVecElemType *elementType,*
OptixCoopVecMatrixLayout *layout,*
size_t *rowColumnStrideInBytes,*
size_t *∗ sizeInBytes*)

For row and column ordered matrix layouts, when *rowColumnStrideInBytes* is 0, the stride will assume
tight packing.

Results will be rounded to the next multiple of 64 to make it easy to pack the matrices in memory and
have the correct alignment.

## Parameters

| in | context |  |
| --- | --- | --- |
| in | elementType |  |
| in | N |  |
| in | K |  |
| in | layout |  |
| in | rowColumnStrideInBytes | Ignored for optimal layouts |
| out | sizeInBytes | Output size of the matrix in bytes |

## 5.13.2.2 optixCoopVecMatrixConvert( )

OPTIXAPIOptixResultoptixCoopVecMatrixConvert (
OptixDeviceContext *context,*
CUstream *stream,*
unsigned int *numNetworks,*
constOptixNetworkDescription *∗ inputNetworkDescription,*
CUdeviceptr *inputNetworks,*
size_t *inputNetworkStrideInBytes,*
constOptixNetworkDescription *∗ outputNetworkDescription,*
CUdeviceptr *outputNetworks,*
size_t *outputNetworkStrideInBytes*)

Convert matrices from one layout and or element type to another.

One use case is to convert a matrix in OPTIX_COOP_VEC_MATRIX_LAYOUT_ROW_MAJOR or

---

107

OPTIX_COOP_VEC_MATRIX_LAYOUT_COLUMN_MAJOR into OPTIX_COOP_VEC_MATRIX_
LAYOUT_INFERENCING_OPTIMAL.

The alignment base address + offset of each matrix needs to be a minimum of 64 bytes. This is similar
to the requirements ofoptixCoopVecMatMul.

Type conversion is possible, but is limited. If the input elementType and output elementType are not
equal, then one must be OPTIX_COOP_VEC_ELEM_TYPE_FLOAT32 or OPTIX_COOP_VEC_ELEM_
TYPE_FLOAT16 and the other must be a lower-precision floating-point type. If the output
elementType is OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E4M3 or OPTIX_COOP_VEC_ELEM_
TYPE_FLOAT8_E5M2, then the output layout must be OPTIX_COOP_VEC_MATRIX_LAYOUT_
INFERENCING_OPTIMAL or OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL.

Parameters

| in | context |  |
| --- | --- | --- |
| in | stream |  |
| in | numNetworks | number of networks to convert |
| in | inputNetworkDescription | description of the input network matrix topology(one per invocation) |
| in | inputNetworks | base pointer to array of matrices that match the input topology specified in network |
| in | inputNetworkStrideInBytes | number of bytes between input networks,ignored if numNetworks is one |
| in | outputNetworkDescription | description of the output network matrix topology(one per invocation) |
| in | outputNetworks | base pointer to array of matrices that match the output topology specified in network |
| in | outputNetworkStrideInBytes | number of bytes between output networks,ignored if numNetworks is one |

## 5.14 Denoiser

## Functions

•OPTIXAPIOptixResultoptixDenoiserCreate(OptixDeviceContextcontext,
OptixDenoiserModelKindmodelKind, constOptixDenoiserOptions *∗*options,OptixDenoiser
*∗*denoiser)

•OPTIXAPIOptixResultoptixDenoiserCreateWithUserModel(OptixDeviceContextcontext, const
void *∗*userData, size_t userDataSizeInBytes,OptixDenoiser *∗*denoiser)

•OPTIXAPIOptixResultoptixDenoiserDestroy(OptixDenoiserdenoiser)

•OPTIXAPIOptixResultoptixDenoiserComputeMemoryResources(constOptixDenoiserdenoiser,
unsigned int outputWidth, unsigned int outputHeight,OptixDenoiserSizes *∗*returnSizes)

•OPTIXAPIOptixResultoptixDenoiserSetup(OptixDenoiserdenoiser, CUstream stream,
unsigned int inputWidth, unsigned int inputHeight,CUdeviceptrdenoiserState, size_t
denoiserStateSizeInBytes,CUdeviceptrscratch, size_t scratchSizeInBytes)

•OPTIXAPIOptixResultoptixDenoiserInvoke(OptixDenoiserdenoiser, CUstream stream, const
OptixDenoiserParams *∗*params,CUdeviceptrdenoiserState, size_t denoiserStateSizeInBytes,
constOptixDenoiserGuideLayer *∗*guideLayer, constOptixDenoiserLayer *∗*layers, unsigned int
numLayers, unsigned int inputOffsetX, unsigned int inputOffsetY,CUdeviceptrscratch, size_t
scratchSizeInBytes)

---

<u>108</u>

•OPTIXAPIOptixResultoptixDenoiserComputeIntensity(OptixDenoiserdenoiser, CUstream
stream, constOptixImage2D *∗*inputImage,CUdeviceptroutputIntensity,CUdeviceptrscratch,
size_t scratchSizeInBytes)

•OPTIXAPIOptixResultoptixDenoiserComputeAverageColor(OptixDenoiserdenoiser,
CUstream stream, constOptixImage2D *∗*inputImage,CUdeviceptroutputAverageColor,
CUdeviceptrscratch, size_t scratchSizeInBytes)

5.14.1 Detailed Description

5.14.2 Function Documentation

## 5.14.2.1 optixDenoiserComputeAverageColor( )

OPTIXAPIOptixResultoptixDenoiserComputeAverageColor (
OptixDenoiser *denoiser,*
CUstream *stream,*
constOptixImage2D *∗ inputImage,*
CUdeviceptr *outputAverageColor,*
CUdeviceptr *scratch,*
size_t *scratchSizeInBytes*)

Compute average logarithmic for each of the first three channels for the given image. When denoising
tiles the intensity of the entire image should be computed, i.e. not per tile to get consistent results.

The size of scratch memory required can be queried withoptixDenoiserComputeMemoryResources.
data type unsigned char is not supported for 'inputImage', it must be 3 or 4 component half/float.

## Parameters

| in | denoiser |  |
| --- | --- | --- |
| in | stream |  |
| in | inputImage |  |
| out | outputAverageColor | three floats |
| in | scratch |  |
| in | scratchSizeInBytes |  |

## 5.14.2.2 optixDenoiserComputeIntensity( )

OPTIXAPIOptixResultoptixDenoiserComputeIntensity (
OptixDenoiser *denoiser,*
CUstream *stream,*
constOptixImage2D *∗ inputImage,*
CUdeviceptr *outputIntensity,*
CUdeviceptr *scratch,*
size_t *scratchSizeInBytes*)

Computes the logarithmic average intensity of the given image. The returned value 'outputIntensity' is
multiplied with the RGB values of the input image/tile in optixDenoiserInvoke if given in the
parameterOptixDenoiserParams::hdrIntensity(otherwise 'hdrIntensity' must be a null pointer). This is

---

<u>109</u>

useful for denoising HDR images which are very dark or bright. When denoising tiles the intensity of
the entire image should be computed, i.e. not per tile to get consistent results.

For each RGB pixel in the inputImage the intensity is calculated and summed if it is greater than 1e-8f:
intensity = log(r *∗* 0.212586f + g *∗* 0.715170f + b *∗* 0.072200f). The function returns 0.18 / exp(sum of
intensities / number of summed pixels). More details could be found in the Reinhard tonemapping
paper: http://www.cmap.polytechnique.fr/ *∼*peyre/cours/x2005signal/hdr_

photographic.pdf

The size of scratch memory required can be queried withoptixDenoiserComputeMemoryResources.
data type unsigned char is not supported for 'inputImage', it must be 3 or 4 component half/float.

Parameters

| in | denoiser |  |
| --- | --- | --- |
| in | stream |  |
| in | inputImage |  |
| out | outputIntensity | single float |
| in | scratch |  |
| in | scratchSizeInBytes |  |

## 5.14.2.3 optixDenoiserComputeMemoryResources( )

OPTIXAPIOptixResultoptixDenoiserComputeMemoryResources (
constOptixDenoiser *denoiser,*
unsigned int *outputWidth,*
unsigned int *outputHeight,*
OptixDenoiserSizes *∗ returnSizes*)

Computes the GPU memory resources required to execute the denoiser.

Memory for state and scratch buffers must be allocated with the sizes in 'returnSizes' and scratch
memory passed to optixDenoiserSetup, optixDenoiserInvoke, optixDenoiserComputeIntensity and
optixDenoiserComputeAverageColor. For tiled denoising an overlap area
('overlapWindowSizeInPixels') must be added to each tile on all sides which increases the amount of
memory needed to denoise a tile. In case of tiling use withOverlapScratchSizeInBytes for scratch
memory size. If only full resolution images are denoised, withoutOverlapScratchSizeInBytes can be
used which is always smaller than withOverlapScratchSizeInBytes.

'outputWidth' and 'outputHeight' is the dimension of the image to be denoised (without overlap in
case tiling is being used). 'outputWidth' and 'outputHeight' must be greater than or equal to the
dimensions passed to optixDenoiserSetup.

Parameters

| in | denoiser |
| --- | --- |
| in | outputWidth |
| in | outputHeight |
| out | returnSizes |

---

<u>110</u>

## 5.14.2.4 optixDenoiserCreate( )

OPTIXAPIOptixResultoptixDenoiserCreate (
OptixDeviceContext *context,*
OptixDenoiserModelKind *modelKind,*
constOptixDenoiserOptions *∗ options,*
OptixDenoiser *∗ denoiser*)

Creates a denoiser object with the given options, using built-in inference models.

'modelKind' selects the model used for inference. Inference for the built-in models can be guided
(giving hints to improve image quality) with albedo and normal vector images in the guide layer (see
'optixDenoiserInvoke'). Use of these images must be enabled in 'OptixDenoiserOptions'.

## Parameters

| in | context |
| --- | --- |
| in | modelKind |
| in | options |
| out | denoiser |

## 5.14.2.5 optixDenoiserCreateWithUserModel( )

OPTIXAPIOptixResultoptixDenoiserCreateWithUserModel (
OptixDeviceContext *context,*
const void *∗ userData,*
size_t *userDataSizeInBytes,*
OptixDenoiser *∗ denoiser*)

Creates a denoiser object with the given options, using a provided inference model.

'userData' and 'userDataSizeInBytes' provide a user model for inference. The memory passed in
userData will be accessed only during the invocation of this function and can be freed after it returns.
The user model must export only one weight set which determines both the model kind and the
required set of guide images.

## Parameters

| in | context |
| --- | --- |
| in | userData |
| in | userDataSizeInBytes |
| out | denoiser |

## 5.14.2.6 optixDenoiserDestroy( )

OPTIXAPIOptixResultoptixDenoiserDestroy (
OptixDenoiser *denoiser*)
Destroys the denoiser object and any associated host resources.

---

<u>111</u>

## 5.14.2.7 optixDenoiserInvoke( )

OPTIXAPIOptixResultoptixDenoiserInvoke (
OptixDenoiser *denoiser,*
CUstream *stream,*
constOptixDenoiserParams *∗ params,*
CUdeviceptr *denoiserState,*
size_t *denoiserStateSizeInBytes,*
constOptixDenoiserGuideLayer *∗ guideLayer,*
constOptixDenoiserLayer *∗ layers,*
unsigned int *numLayers,*
unsigned int *inputOffsetX,*
unsigned int *inputOffsetY,*
CUdeviceptr *scratch,*
size_t *scratchSizeInBytes*)

Invokes denoiser on a set of input data and produces at least one output image. State memory must be
available during the execution of the denoiser (or until optixDenoiserSetup is called with a new state
memory pointer). Scratch memory passed is used only for the duration of this function. Scratch and
state memory sizes must have a size greater than or equal to the sizes as returned by
optixDenoiserComputeMemoryResources.

'inputOffsetX' and 'inputOffsetY' are pixel offsets in the 'inputLayers' image specifying the beginning
of the image without overlap. When denoising an entire image without tiling there is no overlap and
'inputOffsetX' and 'inputOffsetY' must be zero. When denoising a tile which is adjacent to one of the
four sides of the entire image the corresponding offsets must also be zero since there is no overlap at
the side adjacent to the image border.

'guideLayer' provides additional information to the denoiser. When providing albedo and normal
vector guide images, the corresponding fields in the 'OptixDenoiserOptions' must be enabled, see
optixDenoiserCreate. 'guideLayer' must not be null. If a guide image in 'OptixDenoiserOptions' is not
enabled, the corresponding image in 'OptixDenoiserGuideLayer' is ignored.

If OPTIX_DENOISER_MODEL_KIND_TEMPORAL or OPTIX_DENOISER_MODEL_KIND_
TEMPORAL_AOV is selected, a 2d flow image must be given in 'OptixDenoiserGuideLayer'. It
describes for each pixel the flow from the previous to the current frame (a 2d vector in pixel space). The
denoised beauty/AOV of the previous frame must be given in 'previousOutput'. If this image is not
available in the first frame of a sequence, the noisy beauty/AOV from the first frame and zero flow
vectors could be given as a substitute. For non-temporal model kinds the flow image in
'OptixDenoiserGuideLayer' is ignored. 'previousOutput' and 'output' may refer to the same buffer if
tiling is not used, i.e. 'previousOutput' is first read by this function and later overwritten with the
denoised result. 'output' can be passed as 'previousOutput' to the next frame. In other model kinds
(not temporal) 'previousOutput' is ignored.

The beauty layer must be given as the first entry in 'layers'. In AOV type model kinds (OPTIX_
DENOISER_MODEL_KIND_AOV or in user defined models implementing kernel-prediction)
additional layers for the AOV images can be given. In each layer the noisy input image is given in
'input', the denoised output is written into the 'output' image. input and output images may refer to
the same buffer, with the restriction that the pixel formats must be identical for input and output when
the blend mode is selected (seeOptixDenoiserParams).

If OPTIX_DENOISER_MODEL_KIND_TEMPORAL or OPTIX_DENOISER_MODEL_KIND_
TEMPORAL_AOV is selected, the denoised image from the previous frame must be given in
'previousOutput' in the layer. 'previousOutput' and 'output' may refer to the same buffer if tiling is not

---

<u>112</u>

used, i.e. 'previousOutput' is first read by this function and later overwritten with the denoised result.
'output' can be passed as 'previousOutput' to the next frame. In addition,

'previousOutputInternalGuideLayer' and 'outputInternalGuideLayer' must both be allocated
regardless of tiling mode. The pixel format must be OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_
LAYER and the dimension must be identical to to the other input layers. In the first frame memory in
'previousOutputInternalGuideLayer' must either contain valid data from previous denoiser runs or set
to zero. In other model kinds (not temporal) 'previousOutput' and the internal guide layers are ignored.

If OPTIX_DENOISER_MODEL_KIND_TEMPORAL or OPTIX_DENOISER_MODEL_KIND_
TEMPORAL_AOV is selected, the normal vector guide image must be given as 3d vectors in camera
space. In the other models only the x and y channels are used and other channels are ignored.

## Parameters

| in | denoiser |
| --- | --- |
| in | stream |
| in | params |
| in | denoiserState |
| in | denoiserStateSizeInBytes |
| in | guideLayer |
| in | layers |
| in | numLayers |
| in | inputOffsetX |
| in | inputOffsetY |
| in | scratch |
| in | scratchSizeInBytes |

## 5.14.2.8 optixDenoiserSetup( )

OPTIXAPIOptixResultoptixDenoiserSetup (
OptixDenoiser *denoiser,*
CUstream *stream,*
unsigned int *inputWidth,*
unsigned int *inputHeight,*
CUdeviceptr *denoiserState,*
size_t *denoiserStateSizeInBytes,*
CUdeviceptr *scratch,*
size_t *scratchSizeInBytes*)

Initializes the state required by the denoiser.

'inputWidth' and 'inputHeight' must include overlap on both sides of the image if tiling is being used.
The overlap is returned byoptixDenoiserComputeMemoryResources. For subsequent calls to
optixDenoiserInvoke'inputWidth' and 'inputHeight' are the maximum dimensions of the input layers.
Dimensions of the input layers passed tooptixDenoiserInvokemay be different in each invocation
however they always must be smaller than 'inputWidth' and 'inputHeight' passed to
optixDenoiserSetup.

---

## Parameters

| in | denoiser |
| --- | --- |
| in | stream |
| in | inputWidth |
| in | inputHeight |
| in | denoiserState |
| in | denoiserStateSizeInBytes |
| in | scratch |
| in | scratchSizeInBytes |

## 5.15 Utilities

## Classes

•structOptixUtilDenoiserImageTile

## Macros

•#defineOPTIX_MICROMAP_INLINE_FUNCOPTIX_MICROMAP_FUNCinline

•#defineOPTIX_MICROMAP_FLOAT2_SUB(a, b) { a.x - b.x, a.y - b.y }

## Functions

•OPTIX_MICROMAP_INLINE_FUNCfloatoptix_impl::__uint_as_float(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCunsigned intoptix_impl::extractEvenBits(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCunsigned intoptix_impl::prefixEor(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCvoidoptix_impl::index2dbary(unsigned int index,
unsigned int &u, unsigned int &v, unsigned int &w)

•OPTIX_MICROMAP_INLINE_FUNCvoidoptix_impl::micro2bary(unsigned int index,
unsigned int subdivisionLevel, float2 &bary0, float2 &bary1, float2 &bary2)

•OPTIX_MICROMAP_INLINE_FUNCfloat2optix_impl::base2micro(const float2
&baseBarycentrics, const float2 microVertexBaseBarycentrics[3])

•OptixResultoptixUtilGetPixelStride(constOptixImage2D&image, unsigned int
&pixelStrideInBytes)

•OptixResultoptixUtilDenoiserSplitImage(constOptixImage2D&input, constOptixImage2D
&output, unsigned int overlapWindowSizeInPixels, unsigned int tileWidth, unsigned int
tileHeight, std::vector< OptixUtilDenoiserImageTile > &tiles)

•OptixResultoptixUtilDenoiserInvokeTiled(OptixDenoiserdenoiser, CUstream stream, const
OptixDenoiserParams *∗*params,CUdeviceptrdenoiserState, size_t denoiserStateSizeInBytes,
constOptixDenoiserGuideLayer *∗*guideLayer, constOptixDenoiserLayer *∗*layers, unsigned int
numLayers,CUdeviceptrscratch, size_t scratchSizeInBytes, unsigned int
overlapWindowSizeInPixels, unsigned int tileWidth, unsigned int tileHeight)

•OptixResultoptixUtilAccumulateStackSizes(OptixProgramGroupprogramGroup,
OptixStackSizes *∗*stackSizes,OptixPipelinepipeline)

•OptixResultoptixUtilComputeStackSizes(constOptixStackSizes *∗*stackSizes, unsigned int
maxTraceDepth, unsigned int maxCCDepth, unsigned int maxDCDepth, unsigned int
*∗*directCallableStackSizeFromTraversal, unsigned int *∗*directCallableStackSizeFromState,
unsigned int *∗*continuationStackSize)

---

• OptixResult optixUtilComputeStackSizesDCSplit (const OptixStackSizes *stackSizes, unsigned int dssDCFromTraversal, unsigned int dssDCFromState, unsigned int maxTraceDepth, unsigned int maxCCDepth, unsigned int maxDCDepthFromTraversal, unsigned int maxDCDepthFromState, unsigned int *directCallableStackSizeFromTraversal, unsigned int *directCallableStackSizeFromState, unsigned int *continuationStackSize)
• OptixResult optixUtilComputeStackSizesCssCCTree (const OptixStackSizes *stackSizes, unsigned int cssCCTree, unsigned int maxTraceDepth, unsigned int maxDCDepth, unsigned int *directCallableStackSizeFromTraversal, unsigned int *directCallableStackSizeFromState, unsigned int *continuationStackSize)
• OptixResult optixUtilComputeStackSizesSimplePathTracer (OptixProgramGroup programGroupRG, OptixProgramGroup programGroupMS1, const OptixProgramGroup *programGroupCH1, unsigned int programGroupCH1Count, OptixProgramGroup programGroupMS2, const OptixProgramGroup *programGroupCH2, unsigned int programGroupCH2Count, unsigned int *directCallableStackSizeFromTraversal, unsigned int *directCallableStackSizeFromState, unsigned int *continuationStackSize, OptixPipeline pipeline)
• OPTIXAPI OptixResult optixInitWithHandle (void **handlePtr)
• OPTIXAPI OptixResult optixInit (void)
• OPTIXAPI OptixResult optixUninitWithHandle (void *handle)

## 5.15.1 Detailed Description

OptiX Utilities.

## 5.15.2 Macro Definition Documentation

## 5.15.2.1 OPTIX_MICROMAP_FLOAT2_SUB

#define OPTIX_MICROMAP_FLOAT2_SUB(
a,
b ) { a.x - b.x, a.y - b.y }

## 5.15.2.2 OPTIX_MICROMAP_INLINE_FUNC

#define OPTIX_MICROMAP_INLINE_FUNC OPTIX_MICROMAP_FUNC inline

## 5.15.3 Function Documentation

## 5.15.3.1 __uint_as_float()

OPTIX_MICROMAP_INLINE_FUNC float optix_impl::__uint_as_float (
unsigned int x )

## 5.15.3.2 base2micro()

OPTIX_MICROMAP_INLINE_FUNC float2 optix_impl::base2micro (
const float2 & baseBarycentrics,
const float2 microVertexBaseBarycentrics[3])

## 5.15.3.3 extractEvenBits()

OPTIX_MICROMAP_INLINE_FUNC unsigned int optix_impl::extractEvenBits (
unsigned int x)

---

## 5.15.3.4 index2dbary( )

OPTIX_MICROMAP_INLINE_FUNCvoid optix_impl::index2dbary (
unsigned int *index,*
unsigned int & *u,*
unsigned int & *v,*
unsigned int & *w*)
5.15.3.5 micro2bary( )
OPTIX_MICROMAP_INLINE_FUNCvoid optix_impl::micro2bary (
unsigned int *index,*
unsigned int *subdivisionLevel,*
float2 & *bary0,*
float2 & *bary1,*
float2 & *bary2*)
5.15.3.6 optixInit( )
OPTIXAPIOptixResultoptixInit (
void) *[inline]*
Loads the OptiX library and initializes the function table used by the stubs below.
A variant ofoptixInitWithHandle( )that does not make the handle to the loaded library available.

## 5.15.3.7 optixInitWithHandle( )

OPTIXAPIOptixResultoptixInitWithHandle (
void *∗∗ handlePtr*) *[inline]*
Loads the OptiX library and initializes the function table used by the stubs below.
If handlePtr is not nullptr, an OS-specific handle to the library will be returned in *∗*handlePtr.
See alsooptixUninitWithHandle

## 5.15.3.8 optixUninitWithHandle( )

OPTIXAPIOptixResultoptixUninitWithHandle (
void *∗ handle*) *[inline]*

Unloads the OptiX library and zeros the function table used by the stubs below. Takes the handle
returned by optixInitWithHandle. All OptixDeviceContext objects must be destroyed before calling
this function, or the behavior is undefined.

See alsooptixInitWithHandle

## 5.15.3.9 optixUtilAccumulateStackSizes( )

OptixResultoptixUtilAccumulateStackSizes (
OptixProgramGroup *programGroup,*
OptixStackSizes *∗ stackSizes,*
OptixPipeline *pipeline*) *[inline]*

---

Retrieves direct and continuation stack sizes for each program in the program group and accumulates
the upper bounds in the correponding output variables based on the semantic type of the program.
Before the first invocation of this function with a given instance ofOptixStackSizes, the members of
that instance should be set to 0. If the programs rely on external functions, passing the current pipeline
will consider these as well. Otherwise, a null pointer can be passed instead. When external functions
are present, a warning will be issued for these cases.

## 5.15.3.10 optixUtilComputeStackSizes( )

OptixResultoptixUtilComputeStackSizes (
constOptixStackSizes *∗ stackSizes,*
unsigned int *maxTraceDepth,*
unsigned int *maxCCDepth,*
unsigned int *maxDCDepth,*
unsigned int *∗ directCallableStackSizeFromTraversal,*
unsigned int *∗ directCallableStackSizeFromState,*
unsigned int *∗ continuationStackSize*) *[inline]*

Computes the stack size values needed to configure a pipeline.

See the programming guide for an explanation of the formula.

## Parameters

| in | stackSizes | Accumulated stack sizes of all programs in the call graph. |
| --- | --- | --- |
| in | maxTraceDepth | Maximum depth of optixTrace() calls. |
| in | maxCCDepth | Maximum depth of calls trees of continuation callables. |
| in | maxDCDepth | Maximum depth of calls trees of direct callables. |
| out | directCallableStackSizeFromTraversal | Direct stack size requirement for direct callables invoked from IS or AH. |
| out | directCallableStackSizeFromState | Direct stack size requirement for direct callables invoked from RG, MS, or CH. |
| out | continuationStackSize | Continuation stack requirement. |

## 5.15.3.11 optixUtilComputeStackSizesCssCCTree( )

OptixResultoptixUtilComputeStackSizesCssCCTree (

constOptixStackSizes *∗ stackSizes,*
unsigned int *cssCCTree,*
unsigned int *maxTraceDepth,*
unsigned int *maxDCDepth,*
unsigned int *∗ directCallableStackSizeFromTraversal,*
unsigned int *∗ directCallableStackSizeFromState,*
unsigned int *∗ continuationStackSize*) *[inline]*

Computes the stack size values needed to configure a pipeline.

This variant is similar tooptixUtilComputeStackSizes( ), except that it expects the value cssCCTree instead of cssCC and maxCCDepth.

See programming guide for an explanation of the formula.

Parameters

| in | stackSizes | Accumulated stack sizes of all programs in the call graph. |
| --- | --- | --- |
| in | cssCCTree | Maximum stack size used by calls trees of continuation callables. |
| in | maxTraceDepth | Maximum depth of optixTrace() calls. |
| in | maxDCDepth | Maximum depth of calls trees of direct callables. |
| out | directCallableStackSizeFromTraversal | Direct stack size requirement for direct callables invoked from IS or AH. |
| out | directCallableStackSizeFromState | Direct stack size requirement for direct callables invoked from RG, MS, or CH. |
| out | continuationStackSize | Continuation stack requirement. |

## 5.15.3.12 optixUtilComputeStackSizesDCSplit( )

OptixResultoptixUtilComputeStackSizesDCSplit (

constOptixStackSizes *∗ stackSizes,*
unsigned int *dssDCFromTraversal,*
unsigned int *dssDCFromState,*
unsigned int *maxTraceDepth,*
unsigned int *maxCCDepth,*
unsigned int *maxDCDepthFromTraversal,*
unsigned int *maxDCDepthFromState,*
unsigned int *∗ directCallableStackSizeFromTraversal,*
unsigned int *∗ directCallableStackSizeFromState,*
unsigned int *∗ continuationStackSize*) *[inline]*

Computes the stack size values needed to configure a pipeline.

This variant is similar tooptixUtilComputeStackSizes( ), except that it expects the values dssDC and
maxDCDepth split by call site semantic.

See programming guide for an explanation of the formula.

Parameters

| in | stackSizes | Accumulated stack sizes of all programs in the call graph. |
| --- | --- | --- |
| in | dssDCFromTraversal | Accumulated direct stack size of all DC programs invoked from IS or AH. |
| in | dssDCFromState | Accumulated direct stack size of all DC programs invoked from RG, MS, or CH. |
| in | maxTraceDepth | Maximum depth of optixTrace() calls. |
| in | maxCCDepth | Maximum depth of calls trees of continuation callables. |

---

## Parameters

| in | maxDCDepthFromTraversal | Maximum depth of calls trees of direct callables invoked from IS or AH. |
| --- | --- | --- |
| in | maxDCDepthFromState | Maximum depth of calls trees of direct callables invoked from RG, MS, or CH. |
| out | directCallableStackSizeFromTraversal | Direct stack size requirement for direct callables invoked from IS or AH. |
| out | directCallableStackSizeFromState | Direct stack size requirement for direct callables invoked from RG, MS, or CH. |
| out | continuationStackSize | Continuation stack requirement. |

## 5.15.3.13 optixUtilComputeStackSizesSimplePathTracer( )

OptixResultoptixUtilComputeStackSizesSimplePathTracer (
OptixProgramGroup *programGroupRG,*
OptixProgramGroup *programGroupMS1,*
constOptixProgramGroup *∗ programGroupCH1,*
unsigned int *programGroupCH1Count,*
OptixProgramGroup *programGroupMS2,*
constOptixProgramGroup *∗ programGroupCH2,*
unsigned int *programGroupCH2Count,*
unsigned int *∗ directCallableStackSizeFromTraversal,*
unsigned int *∗ directCallableStackSizeFromState,*
unsigned int *∗ continuationStackSize,*
OptixPipeline *pipeline*) *[inline]*

Computes the stack size values needed to configure a pipeline.

This variant is a specialization ofoptixUtilComputeStackSizes( )for a simple path tracer with the
following assumptions: There are only two ray types, camera rays and shadow rays. There are only
RG, MS, and CH programs, and no AH, IS, CC, or DC programs. The camera rays invoke only the miss
and closest hit programs MS1 and CH1, respectively. The CH1 program might trace shadow rays,
which invoke only the miss and closest hit programs MS2 and CH2, respectively.

For flexibility, we allow for each of CH1 and CH2 not just one single program group, but an array of
programs groups, and compute the maximas of the stack size requirements per array.

See programming guide for an explanation of the formula.

If the programs rely on external functions, passing the current pipeline will consider these as well.
Otherwise, a null pointer can be passed instead. When external functions are present, a warning will be
issued for these cases.

## 5.15.3.14 optixUtilDenoiserInvokeTiled( )

OptixResultoptixUtilDenoiserInvokeTiled (
OptixDenoiser *denoiser,*
CUstream *stream,*
constOptixDenoiserParams *∗ params,*
CUdeviceptr *denoiserState,* size_t *denoiserStateSizeInBytes,*
constOptixDenoiserGuideLayer *∗ guideLayer,*
constOptixDenoiserLayer *∗ layers,*
unsigned int *numLayers,*
CUdeviceptr *scratch,*
size_t *scratchSizeInBytes,*
unsigned int *overlapWindowSizeInPixels,*
unsigned int *tileWidth,*
unsigned int *tileHeight*) *[inline]*

Run denoiser on input layers seeoptixDenoiserInvokeadditional parameters:

Runs the denoiser on the input layers on a single GPU and stream usingoptixDenoiserInvoke. If the
input layers' dimensions are larger than the specified tile size, the image is divided into tiles using
optixUtilDenoiserSplitImage, and multiple back-to-back invocations are performed in order to reuse
the scratch space. Multiple tiles can be invoked concurrently ifoptixUtilDenoiserSplitImageis used
directly and multiple scratch allocations for each concurrent invocation are used. The input parameters
are the same asoptixDenoiserInvokeexcept for the addition of the maximum tile size.

## Parameters

| in | denoiser |
| --- | --- |
| in | stream |
| in | params |
| in | denoiserState |
| in | denoiserStateSizeInBytes |
| in | guideLayer |
| in | layers |
| in | numLayers |
| in | scratch |
| in | scratchSizeInBytes |
| in | overlapWindowSizeInPixels |
| in | tileWidth |
| in | tileHeight |

## 5.15.3.15 optixUtilDenoiserSplitImage( )

OptixResultoptixUtilDenoiserSplitImage (
constOptixImage2D& *input,*
constOptixImage2D& *output,*
unsigned int *overlapWindowSizeInPixels,*
unsigned int *tileWidth,*
unsigned int *tileHeight,*
std::vector< OptixUtilDenoiserImageTile > & tiles) [inline]

Split image into 2D tiles given horizontal and vertical tile size.

---

## Parameters

| in | input | full resolution input image to be split |
| --- | --- | --- |
| in | output | full resolution output image |
| in | overlapWindowSizeInPixels | see OptixDenoiserSizes，optixDenoiserComputeMemoryResources |
| in | tileWidth | maximum width of tiles |
| in | tileHeight | maximum height of tiles |
| out | tiles | list of tiles covering the input image |

## 5.15.3.16 optixUtilGetPixelStride( )

OptixResultoptixUtilGetPixelStride (
constOptixImage2D& *image,*
unsigned int & *pixelStrideInBytes*) *[inline]*

Return pixel stride in bytes for the given pixel format if the pixelStrideInBytes member of the image is
zero. Otherwise return pixelStrideInBytes from the image.

## Parameters

| in | image | Image containing the pixel stride |
| --- | --- | --- |
| in | pixelStrideInBytes | Pixel stride in bytes |

## 5.15.3.17 prefixEor( )

OPTIX_MICROMAP_INLINE_FUNCunsigned int optix_impl::prefixEor (
unsigned int *x*)

## 5.16 Types

## Classes

•structOptixDeviceContextOptions

•structOptixOpacityMicromapUsageCount

•structOptixBuildInputOpacityMicromap

•structOptixRelocateInputOpacityMicromap

•structOptixBuildInputTriangleArray

•structOptixRelocateInputTriangleArray

•structOptixBuildInputCurveArray

•structOptixBuildInputSphereArray

•structOptixAabb

•structOptixBuildInputCustomPrimitiveArray

•structOptixBuildInputInstanceArray

•structOptixRelocateInputInstanceArray

•structOptixBuildInput

•structOptixRelocateInput

•structOptixInstance

•structOptixOpacityMicromapDesc

•structOptixOpacityMicromapHistogramEntry

---

• struct OptixOpacityMicromapArrayBuildInput
• struct OptixMicromapBufferSizes
• struct OptixMicromapBuffers
• struct OptixMotionOptions
• struct OptixAccelBuildOptions
• struct OptixAccelBufferSizes
• struct OptixAccelEmitDesc
• struct OptixRelocationInfo
• struct OptixStaticTransform
• struct OptixMatrixMotionTransform
• struct OptixSRTData
• struct OptixSRTMotionTransform
• struct OptixClusterAccelBuildModeDescImplicitDest
• struct OptixClusterAccelBuildModeDescExplicitDest
• struct OptixClusterAccelBuildModeDescGetSize
• struct OptixClusterAccelBuildInputTriangles
• struct OptixClusterAccelBuildInputGrids
• struct OptixClusterAccelBuildInputClusters
• struct OptixClusterAccelPrimitiveInfo
• struct OptixClusterAccelBuildInputTrianglesArgs
• struct OptixClusterAccelBuildInputGridsArgs
• struct OptixClusterAccelBuildInputTemplatesArgs
• struct OptixClusterAccelBuildInputClustersArgs
• struct OptixClusterAccelBuildInput
• struct OptixClusterAccelBuildModeDesc
• struct OptixImage2D
• struct OptixDenoiserOptions
• struct OptixDenoiserGuideLayer
• struct OptixDenoiserLayer
• struct OptixDenoiserParams
• struct OptixDenoiserSizes
• struct OptixTraverseData
• struct OptixModuleCompileBoundValueEntry
• struct OptixPayloadType
• struct OptixModuleCompileOptions
• struct OptixBuiltinISOptions
• struct OptixProgramGroupSingleModule
• struct OptixProgramGroupHitgroup
• struct OptixProgramGroupCallables
• struct OptixProgramGroupDesc
• struct OptixProgramGroupOptions
• struct OptixPipelineCompileOptions
• struct OptixPipelineLinkOptions
• struct OptixShaderBindingTable
• struct OptixStackSizes
• struct OptixCoopVecMatrixDescription
• struct OptixNetworkDescription

---

## Macros

• #define OPTIX_SBT_RECORD_HEADER_SIZE ((size_t)32)
• #define OPTIX_SBT_RECORD_ALIGNMENT 16ull
• #define OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT 128ull
• #define OPTIX_INSTANCE_BYTE_ALIGNMENT 16ull
• #define OPTIX_AABB_BUFFER_BYTE_ALIGNMENT 8ull
• #define OPTIX_GEOMETRY_TRANSFORM_BYTE_ALIGNMENT 16ull
• #define OPTIX_TRANSFORM_BYTE_ALIGNMENT 64ull
• #define OPTIX_OPACITY_MICROMAP_DESC_BUFFER_BYTE_ALIGNMENT 8ull
• #define OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT 0
• #define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_TYPE_COUNT 8
• #define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_VALUE_COUNT 32
• #define OPTIX_OPACITY_MICROMAP_STATE_TRANSPARENT (0)
• #define OPTIX_OPACITY_MICROMAP_STATE_OPAQUE (1)
• #define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_TRANSPARENT (2)
• #define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_OPAQUE (3)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_TRANSPARENT (-1)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_OPAQUE (-2)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_TRANSPARENT (-3)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_OPAQUE (-4)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_CLUSTER_SKIP_OPACITY_MICROMAP (-5)
• #define OPTIX_OPACITY_MICROMAP_ARRAY_BUFFER_BYTE_ALIGNMENT 128ull
• #define OPTIX_OPACITY_MICROMAP_MAX_SUBDIVISION_LEVEL 12

## Typedefs

• typedef unsigned long long CUdeviceptr
• typedef struct OptixDeviceContext_t * OptixDeviceContext
• typedef struct OptixModule_t * OptixModule
• typedef struct OptixProgramGroup_t * OptixProgramGroup
• typedef struct OptixPipeline_t * OptixPipeline
• typedef struct OptixDenoiser_t * OptixDenoiser
• typedef struct OptixTask_t * OptixTask
• typedef unsigned long long OptixTraversableHandle
• typedef unsigned int OptixVisibilityMask
• typedef enum OptixResult OptixResult
• typedef enum OptixDeviceProperty OptixDeviceProperty
• typedef void(* OptixLogCallback) (unsigned int level, const char *tag, const char *message, void *cbdata)
• typedef enum OptixDeviceContextValidationMode OptixDeviceContextValidationMode
• typedef struct OptixDeviceContextOptions OptixDeviceContextOptions
• typedef enum OptixPipelineSymbolMemcpyKind OptixPipelineSymbolMemcpyKind
• typedef enum OptixDevicePropertyShaderExecutionReorderingFlags OptixDevicePropertyShaderExecutionReorderingFlags
• typedef enum OptixDevicePropertyClusterAccelFlags OptixDevicePropertyClusterAccelFlags
• typedef enum OptixGeometryFlags OptixGeometryFlags
• typedef enum OptixHitKind OptixHitKind
• typedef enum OptixIndicesFormat OptixIndicesFormat

---

• typedef enum OptixVertexFormat OptixVertexFormat
• typedef enum OptixTransformFormat OptixTransformFormat
• typedef enum OptixOpacityMicromapFormat OptixOpacityMicromapFormat
• typedef enum OptixOpacityMicromapArrayIndexingMode
OptixOpacityMicromapArrayIndexingMode
• typedef struct OptixOpacityMicromapUsageCount OptixOpacityMicromapUsageCount
• typedef struct OptixBuildInputOpacityMicromap OptixBuildInputOpacityMicromap
• typedef struct OptixRelocateInputOpacityMicromap OptixRelocateInputOpacityMicromap
• typedef struct OptixBuildInputTriangleArray OptixBuildInputTriangleArray
• typedef struct OptixRelocateInputTriangleArray OptixRelocateInputTriangleArray
• typedef enum OptixPrimitiveType OptixPrimitiveType
• typedef enum OptixPrimitiveTypeFlags OptixPrimitiveTypeFlags
• typedef enum OptixCurveEndcapFlags OptixCurveEndcapFlags
• typedef struct OptixBuildInputCurveArray OptixBuildInputCurveArray
• typedef struct OptixBuildInputSphereArray OptixBuildInputSphereArray
• typedef struct OptixAabb OptixAabb
• typedef struct OptixBuildInputCustomPrimitiveArray OptixBuildInputCustomPrimitiveArray
• typedef struct OptixBuildInputInstanceArray OptixBuildInputInstanceArray
• typedef struct OptixRelocateInputInstanceArray OptixRelocateInputInstanceArray
• typedef enum OptixBuildInputType OptixBuildInputType
• typedef struct OptixBuildInput OptixBuildInput
• typedef struct OptixRelocateInput OptixRelocateInput
• typedef enum OptixInstanceFlags OptixInstanceFlags
• typedef struct OptixInstance OptixInstance
• typedef enum OptixBuildFlags OptixBuildFlags
• typedef enum OptixOpacityMicromapFlags OptixOpacityMicromapFlags
• typedef struct OptixOpacityMicromapDesc OptixOpacityMicromapDesc
• typedef struct OptixOpacityMicromapHistogramEntry OptixOpacityMicromapHistogramEntry
• typedef struct OptixOpacityMicromapArrayBuildInput OptixOpacityMicromapArrayBuildInput
• typedef struct OptixMicromapBufferSizes OptixMicromapBufferSizes
• typedef struct OptixMicromapBuffers OptixMicromapBuffers
• typedef enum OptixBuildOperation OptixBuildOperation
• typedef enum OptixMotionFlags OptixMotionFlags
• typedef struct OptixMotionOptions OptixMotionOptions
• typedef struct OptixAccelBuildOptions OptixAccelBuildOptions
• typedef struct OptixAccelBufferSizes OptixAccelBufferSizes
• typedef enum OptixAccelPropertyType OptixAccelPropertyType
• typedef struct OptixAccelEmitDesc OptixAccelEmitDesc
• typedef struct OptixRelocationInfo OptixRelocationInfo
• typedef struct OptixStaticTransform OptixStaticTransform
• typedef struct OptixMatrixMotionTransform OptixMatrixMotionTransform
• typedef struct OptixSRTData OptixSRTData
• typedef struct OptixSRTMotionTransform OptixSRTMotionTransform
• typedef enum OptixTraversableType OptixTraversableType
• typedef enum OptixClusterAccelBuildFlags OptixClusterAccelBuildFlags
• typedef enum OptixClusterAccelClusterFlags OptixClusterAccelClusterFlags
• typedef enum OptixClusterAccelPrimitiveFlags OptixClusterAccelPrimitiveFlags
• typedef enum OptixClusterAccelBuildType OptixClusterAccelBuildType
• typedef enum OptixClusterAccelBuildMode OptixClusterAccelBuildMode
• typedef enum OptixClusterAccelIndicesFormat OptixClusterAccelIndicesFormat

---

• typedef struct OptixClusterAccelBuildModeDescImplicitDest
OptixClusterAccelBuildModeDescImplicitDest
• typedef struct OptixClusterAccelBuildModeDescExplicitDest
OptixClusterAccelBuildModeDescExplicitDest
• typedef struct OptixClusterAccelBuildModeDescGetSize
OptixClusterAccelBuildModeDescGetSize
• typedef struct OptixClusterAccelBuildInputTriangles OptixClusterAccelBuildInputTriangles
• typedef struct OptixClusterAccelBuildInputGrids OptixClusterAccelBuildInputGrids
• typedef struct OptixClusterAccelBuildInputClusters OptixClusterAccelBuildInputClusters
• typedef struct OptixClusterAccelPrimitiveInfo OptixClusterAccelPrimitiveInfo
• typedef enum OptixClusterIDValues OptixClusterIDValues
• typedef struct OptixClusterAccelBuildInputTrianglesArgs OptixClusterAccelBuildInputTrianglesArgs
• typedef struct OptixClusterAccelBuildInputGridsArgs OptixClusterAccelBuildInputGridsArgs
• typedef struct OptixClusterAccelBuildInputTemplatesArgs OptixClusterAccelBuildInputTemplatesArgs
• typedef struct OptixClusterAccelBuildInputClustersArgs OptixClusterAccelBuildInputClustersArgs
• typedef struct OptixClusterAccelBuildInput OptixClusterAccelBuildInput
• typedef struct OptixClusterAccelBuildModeDesc OptixClusterAccelBuildModeDesc
• typedef enum OptixPixelFormat OptixPixelFormat
• typedef struct OptixImage2D OptixImage2D
• typedef enum OptixDenoiserModelKind OptixDenoiserModelKind
• typedef enum OptixDenoiserAlphaMode OptixDenoiserAlphaMode
• typedef struct OptixDenoiserOptions OptixDenoiserOptions
• typedef struct OptixDenoiserGuideLayer OptixDenoiserGuideLayer
• typedef enum OptixDenoiserAOVType OptixDenoiserAOVType
• typedef struct OptixDenoiserLayer OptixDenoiserLayer
• typedef struct OptixDenoiserParams OptixDenoiserParams
• typedef struct OptixDenoiserSizes OptixDenoiserSizes
• typedef enum OptixRayFlags OptixRayFlags
• typedef enum OptixTransformType OptixTransformType
• typedef struct OptixTraverseData OptixTraverseData
• typedef enum OptixTraversableGraphFlags OptixTraversableGraphFlags
• typedef enum OptixCompileOptimizationLevel OptixCompileOptimizationLevel
• typedef enum OptixCompileDebugLevel OptixCompileDebugLevel
• typedef enum OptixModuleCompileState OptixModuleCompileState
• typedef enum OptixCreationFlags OptixCreationFlags
• typedef struct OptixModuleCompileBoundValueEntry OptixModuleCompileBoundValueEntry
• typedef enum OptixPayloadTypeID OptixPayloadTypeID
• typedef enum OptixPayloadSemantics OptixPayloadSemantics
• typedef struct OptixPayloadType OptixPayloadType
• typedef struct OptixModuleCompileOptions OptixModuleCompileOptions
• typedef struct OptixBuiltinISOptions OptixBuiltinISOptions
• typedef enum OptixProgramGroupKind OptixProgramGroupKind
• typedef enum OptixProgramGroupFlags OptixProgramGroupFlags
• typedef struct OptixProgramGroupSingleModule OptixProgramGroupSingleModule
• typedef struct OptixProgramGroupHitgroup OptixProgramGroupHitgroup
• typedef struct OptixProgramGroupCallables OptixProgramGroupCallables
• typedef struct OptixProgramGroupDesc OptixProgramGroupDesc

---

• typedef struct OptixProgramGroupOptions OptixProgramGroupOptions
• typedef enum OptixExceptionCodes OptixExceptionCodes
• typedef enum OptixExceptionFlags OptixExceptionFlags
• typedef struct OptixPipelineCompileOptions OptixPipelineCompileOptions
• typedef struct OptixPipelineLinkOptions OptixPipelineLinkOptions
• typedef struct OptixShaderBindingTable OptixShaderBindingTable
• typedef struct OptixStackSizes OptixStackSizes
• typedef enum OptixDevicePropertyCoopVecFlags OptixDevicePropertyCoopVecFlags
• typedef enum OptixCoopVecElemType OptixCoopVecElemType
• typedef enum OptixCoopVecMatrixLayout OptixCoopVecMatrixLayout
• typedef struct OptixCoopVecMatrixDescription OptixCoopVecMatrixDescription
• typedef struct OptixNetworkDescription OptixNetworkDescription
• typedef enum OptixQueryFunctionTableOptions OptixQueryFunctionTableOptions
• typedef OptixResult() OptixQueryFunctionTable_t(int abiId, unsigned int numOptions,
OptixQueryFunctionTableOptions *, const void **, void *functionTable, size_t sizeOfTable)

## Enumerations

• enum OptixResult {
  OPTIX_SUCCESS = 0 ,
  OPTIX_ERROR_INVALID_VALUE = 7001 ,
  OPTIX_ERROR_HOST_OUT_OF_MEMORY = 7002 ,
  OPTIX_ERROR_INVALID_OPERATION = 7003 ,
  OPTIX_ERROR_FILE_IO_ERROR = 7004 ,
  OPTIX_ERROR_INVALID_FILE_FORMAT = 7005 ,
  OPTIX_ERROR_DISK_CACHE_INVALID_PATH = 7010 ,
  OPTIX_ERROR_DISK_CACHE_PERMISSION_ERROR = 7011 ,
  OPTIX_ERROR_DISK_CACHE_DATABASE_ERROR = 7012 ,
  OPTIX_ERROR_DISK_CACHE_INVALID_DATA = 7013 ,
  OPTIX_ERROR_LAUNCH_FAILURE = 7050 ,
  OPTIX_ERROR_INVALID_DEVICE_CONTEXT = 7051 ,
  OPTIX_ERROR_CUDA_NOT_INITIALIZED = 7052 ,
  OPTIX_ERROR_VALIDATION_FAILURE = 7053 ,
  OPTIX_ERROR_INVALID_INPUT = 7200 ,
  OPTIX_ERROR_INVALID_LAUNCH_PARAMETER = 7201 ,
  OPTIX_ERROR_INVALID_PAYLOAD_ACCESS = 7202 ,
  OPTIX_ERROR_INVALID_ATTRIBUTE_ACCESS = 7203 ,
  OPTIX_ERROR_INVALID_FUNCTION_USE = 7204 ,
  OPTIX_ERROR_INVALID_FUNCTION_ARGUMENTS = 7205 ,
  OPTIX_ERROR_PIPELINE_OUT_OF_CONSTANT_MEMORY = 7250 ,
  OPTIX_ERROR_PIPELINE_LINK_ERROR = 7251 ,
  OPTIX_ERRORILLEGAL_DURING_TASK_EXECUTE = 7270 ,
  OPTIX_ERROR_CREATION_CANCELED = 7290 ,
  OPTIX_ERROR_INTERNAL_COMPILER_ERROR = 7299 ,
  OPTIX_ERROR_DENOISER_MODEL_NOT_SET = 7300 ,
  OPTIX_ERROR_DENOISER_NOT_INITIALIZED = 7301 ,
  OPTIX_ERROR_NOT_COMPATIBLE = 7400 ,
  OPTIX_ERROR_PAYLOAD_TYPE_MISMATCH = 7500 ,
  OPTIX_ERROR_PAYLOAD_TYPE_RESOLUTION_FAILED = 7501 ,
  OPTIX_ERROR_PAYLOAD_TYPE_ID_INVALID = 7502 ,
  OPTIX_ERROR_NOT_SUPPORTED = 7800 ,
  OPTIX_ERROR_UNSUPPORTED_ABI_VERSION = 7801 ,
  OPTIX_ERROR_FUNCTION_TABLE_SIZE_MISMATCH = 7802 ,

---

OPTIX_ERROR_INVALID_ENTRY_FUNCTION_OPTIONS = 7803 ,
OPTIX_ERROR_LIBRARY_NOT_FOUND = 7804 ,
OPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND = 7805 ,
OPTIX_ERROR_LIBRARY_UNLOAD_FAILURE = 7806 ,
OPTIX_ERROR_DEVICE_OUT_OF_MEMORY = 7807 ,
OPTIX_ERROR_INVALID_POINTER = 7808 ,
OPTIX_ERROR_SYMBOL_NOT_FOUND = 7809 ,
OPTIX_ERROR_CUDA_ERROR = 7900 ,
OPTIX_ERROR_INTERNAL_ERROR = 7990 ,
OPTIX_ERROR_UNKNOWN = 7999 }

## • enum OptixDeviceProperty {
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRACE_DEPTH = 0x2001 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRAVERSABLE_GRAPH_DEPTH = 0x2002 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_PRIMITIVES_PER_GAS = 0x2003 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCES_PER_IAS = 0x2004 ,
    OPTIX_DEVICE_PROPERTY_RTCORE_VERSION = 0x2005 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCE_ID = 0x2006 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_NUM_BITS_INSTANCE_VISIBILITY_MASK = 0x2007 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_RECORDS_PER_GAS = 0x2008 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_OFFSET = 0x2009 ,
    OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING = 0x200A ,
    OPTIX_DEVICE_PROPERTY_COOP_VEC = 0x200B ,
    OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL = 0x2020 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_VERTICES = 0x2021 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_TRIANGLES = 0x2022 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_STRUCTURED_GRID_RESOLUTION = 0x2023 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_SBT_INDEX = 0x2024 ,
    OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTERS_PER_GAS = 0x2025 }
## • enum OptixDeviceContextValidationMode {
    OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_OFF = 0 ,
    OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_ALL = 0xFFFFFFF }
## • enum OptixPipelineSymbolMemcpyKind {
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_DEVICE = 0x21A0 ,
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_HOST = 0x21A1 ,
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_DEVICE = 0x21A2 ,
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_HOST = 0x21A3 }
• enum OptixDevicePropertyShaderExecutionReorderingFlags {
    OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING_FLAG_NONE = 0 ,
    OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING_FLAG_STANDARD = 1
    << 0 }
• enum OptixDevicePropertyClusterAccelFlags {
    OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_NONE = 0 ,
    OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_STANDARD = 1 << 0 }
## • enum OptixGeometryFlags {
    OPTIX_GEOMETRY_FLAG_NONE = 0 ,
    OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT = 1u << 0 ,
    OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL = 1u << 1 ,
    OPTIX_GEOMETRY_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1u << 2 }
## • enum OptixHitKind {
    OPTIX_HIT_KIND_TRIANGLE_FRONT_FACE = 0xFE ,
    OPTIX_HIT_KIND_TRIANGLE_BACK_FACE = 0xFF }
## • enum OptixIndicesFormat {
    OPTIX_INDICES_FORMAT_NONE = 0 ,

---

OPTIX_INDICES_FORMAT_UNSIGNED_BYTE3 = 0x2101 ,
OPTIX_INDICES_FORMAT_UNSIGNED_SHORT3 = 0x2102 ,
OPTIX_INDICES_FORMAT_UNSIGNED_INT3 = 0x2103 }

• enum OptixVertexFormat {
    OPTIX_VERTEX_FORMAT_NONE = 0 ,
    OPTIX_VERTEX_FORMAT_FLOAT3 = 0x2121 ,
    OPTIX_VERTEX_FORMAT_FLOAT2 = 0x2122 ,
    OPTIX_VERTEX_FORMAT_HALF3 = 0x2123 ,
    OPTIX_VERTEX_FORMAT_HALF2 = 0x2124 ,
    OPTIX_VERTEX_FORMAT_SNORM16_3 = 0x2125 ,
    OPTIX_VERTEX_FORMAT_SNORM16_2 = 0x2126 }
• enum OptixTransformFormat {
    OPTIX_TRANSFORM_FORMAT_NONE = 0 ,
    OPTIX_TRANSFORM_FORMAT_MATRIX_FLOAT12 = 0x21E1 }
• enum OptixOpacityMicromapFormat {
    OPTIX_OPACITY_MICROMAP_FORMAT_NONE = 0 ,
    OPTIX_OPACITY_MICROMAP_FORMAT_2_STATE = 1 ,
    OPTIX_OPACITY_MICROMAP_FORMAT_4_STATE = 2 }
• enum OptixOpacityMicromapArrayIndexingMode {
    OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_NONE = 0 ,
    OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_LINEAR = 1 ,
    OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_INDEXED = 2 }
• enum OptixPrimitiveType {
    OPTIX_PRIMITIVE_TYPE_CUSTOM = 0x2500 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE = 0x2501 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE = 0x2502 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_LINEAR = 0x2503 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM = 0x2504 ,
    OPTIX_PRIMITIVE_TYPE_FLAT_QUADRATIC_BSPLINE = 0x2505 ,
    OPTIX_PRIMITIVE_TYPE_SPHERE = 0x2506 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER = 0x2507 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE_ROCAPS = 0x2508 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE_ROCAPS = 0x2509 ,
    OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM_ROCAPS = 0x250A ,
    OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER_ROCAPS = 0x250B ,
    OPTIX_PRIMITIVE_TYPE_TRIANGLE = 0x2531 }
• enum OptixPrimitiveTypeFlags {
    OPTIX_PRIMITIVE_TYPE_FLAGS_CUSTOM = 1 << 0 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE = 1 << 1 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE = 1 << 2 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_LINEAR = 1 << 3 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM = 1 << 4 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_FLAT_QUADRATIC_BSPLINE = 1 << 5 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_SPHERE = 1 << 6 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER = 1 << 7 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE_ROCAPS = 1 << 8 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE_ROCAPS = 1 << 9 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM_ROCAPS = 1 << 10 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER_ROCAPS = 1 << 11 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE = 1 << 31 }
• enum OptixCurveEndcapFlags {
    OPTIX_CURVE_ENDCAP_DEFAULT = 0 ,
    OPTIX_CURVE_ENDCAP_ON = 1 << 0 }

---

• enum OptixBuildInputType {
  OPTIX_BUILD_INPUT_TYPE_TRIANGLES = 0x2141,
  OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES = 0x2142,
  OPTIX_BUILD_INPUT_TYPE_INSTANCES = 0x2143,
  OPTIX_BUILD_INPUT_TYPE_INSTANCE_POINTERS = 0x2144,
  OPTIX_BUILD_INPUT_TYPE_CURVES = 0x2145,
  OPTIX_BUILD_INPUT_TYPE_SPHERES = 0x2146}
## • enum OptixInstanceFlags {
  OPTIX_INSTANCE_FLAG_NONE = 0,
  OPTIX_INSTANCE_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1u << 0,
  OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING = 1u << 1,
  OPTIX_INSTANCE_FLAG_DISABLE_ANYHIT = 1u << 2,
  OPTIX_INSTANCE_FLAG_ENFORCE_ANYHIT = 1u << 3,
  OPTIX_INSTANCE_FLAG_FORCE_OPACITY_MICROMAP_2_STATE = 1u << 4,
  OPTIX_INSTANCE_FLAG_DISABLE_OPACITY_MICROMAPS = 1u << 5}
• enum OptixBuildFlags {
  OPTIX_BUILD_FLAG_NONE = 0,
  OPTIX_BUILD_FLAG_ALLOW_UPDATE = 1u << 0,
  OPTIX_BUILD_FLAG_ALLOW_COMPACTION = 1u << 1,
  OPTIX_BUILD_FLAG_PREFER_FAST_TRACE = 1u << 2,
  OPTIX_BUILD_FLAG_PREFER_FAST_BUILD = 1u << 3,
  OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS = 1u << 4,
  OPTIX_BUILD_FLAG_ALLOW_RANDOM_INSTANCE_ACCESS = 1u << 5,
  OPTIX_BUILD_FLAG_ALLOW_OPACITY_MICROMAP_UPDATE = 1u << 6,
  OPTIX_BUILD_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS = 1u << 7}
• enum OptixOpacityMicromapFlags {
  OPTIX_OPACITY_MICROMAP_FLAG_NONE = 0,
  OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_TRACE = 1 << 0,
  OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_BUILD = 1 << 1}
## • enum OptixBuildOperation {
  OPTIX_BUILD_OPERATION_BUILD = 0x2161,
  OPTIX_BUILD_OPERATION_UPDATE = 0x2162}
## • enum OptixMotionFlags {
  OPTIX_MOTION_FLAG_NONE = 0,
  OPTIX_MOTION_FLAG_START_VANISH = 1u << 0,
  OPTIX_MOTION_FLAG_END_VANISH = 1u << 1}
## • enum OptixAccelPropertyType {
  OPTIX_PROPERTY_TYPE_COMPACTED_SIZE = 0x2181,
  OPTIX_PROPERTY_TYPE_AABBS = 0x2182}
## • enum OptixTraversableType {
  OPTIX_TRAVERSABLE_TYPE_STATIC_TRANSFORM = 0x21C1,
  OPTIX_TRAVERSABLE_TYPE_MATRIX_MOTION_TRANSFORM = 0x21C2,
  OPTIX_TRAVERSABLE_TYPE_SRT_MOTION_TRANSFORM = 0x21C3}
## • enum OptixClusterAccelBuildFlags {
  OPTIX_CLUSTER_ACCEL_BUILD_FLAG_NONE = 0,
  OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_TRACE = 1 << 0,
  OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_BUILD = 1 << 1,
  OPTIX_CLUSTER_ACCEL_BUILD_FLAG_ALLOW_OPACITY_MICROMAPS = 1 << 2}
## • enum OptixClusterAccelClusterFlags {
  OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_NONE = 0,
  OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS = 1
<< 0}

---

• enum OptixClusterAccelPrimitiveFlags {
  OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_NONE = 0 ,
  OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1 << 0 ,
  OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_REQUIRE_SINGLE_ANYHIT_CALL = 1 << 1 ,
  OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_ANYHIT = 1 << 2 }
• enum OptixClusterAccelBuildType {
  OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS = 0x2545 ,
  OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLES = 0x2546 ,
  OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES = 0x2547 ,
  OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TEMPLATES = 0x2548 ,
  OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_GRIDS = 0x2549 }
• enum OptixClusterAccelBuildMode {
  OPTIX_CLUSTER_ACCEL_BUILD_MODE_IMPLICIT_DESTINATIONS = 0 ,
  OPTIX_CLUSTER_ACCEL_BUILD_MODE_EXPLICIT_DESTINATIONS = 1 ,
  OPTIX_CLUSTER_ACCEL_BUILD_MODE_GET_SIZES = 2 }
• enum OptixClusterAccelIndicesFormat {
  OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_8BIT = 1 ,
  OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_16BIT = 2 ,
  OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_32BIT = 4 }
• enum OptixClusterIDValues { OPTIX_CLUSTER_ID_INVALID = 0xFFFFFFFu }
• enum OptixPixelFormat {
  OPTIX_PIXEL_FORMAT_HALF1 = 0x220a ,
  OPTIX_PIXEL_FORMAT_HALF2 = 0x2207 ,
  OPTIX_PIXEL_FORMAT_HALF3 = 0x2201 ,
  OPTIX_PIXEL_FORMAT_HALF4 = 0x2202 ,
  OPTIX_PIXEL_FORMAT_FLOAT1 = 0x220b ,
  OPTIX_PIXEL_FORMAT_FLOAT2 = 0x2208 ,
  OPTIX_PIXEL_FORMAT_FLOAT3 = 0x2203 ,
  OPTIX_PIXEL_FORMAT_FLOAT4 = 0x2204 ,
  OPTIX_PIXEL_FORMAT_UCHAR3 = 0x2205 ,
  OPTIX_PIXEL_FORMAT_UCHAR4 = 0x2206 ,
  OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_LAYER = 0x2209 }
• enum OptixDenoiserModelKind {
  OPTIX_DENOISER_MODEL_KIND_AOV = 0x2324 ,
  OPTIX_DENOISER_MODEL_KIND_TEMPORAL_AOV = 0x2326 ,
  OPTIX_DENOISER_MODEL_KIND_UPSCALE2X = 0x2327 ,
  OPTIX_DENOISER_MODEL_KIND_TEMPORAL_UPSCALE2X = 0x2328 ,
  OPTIX_DENOISER_MODEL_KIND_LDR = 0x2322 ,
  OPTIX_DENOISER_MODEL_KIND_HDR = 0x2323 ,
  OPTIX_DENOISER_MODEL_KIND_TEMPORAL = 0x2325 }
• enum OptixDenoiserAlphaMode {
  OPTIX_DENOISER_ALPHA_MODE_COPY = 0 ,
  OPTIX_DENOISER_ALPHA_MODE_DENOISE = 1 }
• enum OptixDenoiserAOVType {
  OPTIX_DENOISER_AOV_TYPE_NONE = 0 ,
  OPTIX_DENOISER_AOV_TYPE_BEAUTY = 0x7000 ,
  OPTIX_DENOISER_AOV_TYPE_SPECULAR = 0x7001 ,
  OPTIX_DENOISER_AOV_TYPE_REFLECTION = 0x7002 ,
  OPTIX_DENOISER_AOV_TYPE_REFRACTION = 0x7003 ,
  OPTIX_DENOISER_AOV_TYPE_DIFFUSE = 0x7004 }
• enum OptixRayFlags {
  OPTIX_RAY_FLAG_NONE = 0u ,

---

OPTIX_RAY_FLAG_DISABLE_ANYHIT = 1u << 0,
OPTIX_RAY_FLAG_ENFORCE_ANYHIT = 1u << 1,
OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT = 1u << 2,
OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT = 1u << 3,
OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES = 1u << 4,
OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES = 1u << 5,
OPTIX_RAY_FLAG_CULL_DISABLED_ANYHIT = 1u << 6,
OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT = 1u << 7,
OPTIX_RAY_FLAG_FORCE_OPACITY_MICROMAP_2_STATE = 1u << 10 }

• enum OptixTransformType {
    OPTIX_TRANSFORM_TYPE_NONE = 0,
    OPTIX_TRANSFORM_TYPE_STATIC_TRANSFORM = 1,
    OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_TRANSFORM = 2,
    OPTIX_TRANSFORM_TYPE_SRT_MOTION_TRANSFORM = 3,
    OPTIX_TRANSFORM_TYPE_INSTANCE = 4 }
• enum OptixTraversableGraphFlags {
    OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_ANY = 0,
    OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS = 1u << 0,
    OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING = 1u << 1 }
• enum OptixCompileOptimizationLevel {
    OPTIX_COMPILE_OPTIMIZATION_DEFAULT = 0,
    OPTIX_COMPILE_OPTIMIZATION_LEVEL_0 = 0x2340,
    OPTIX_COMPILE_OPTIMIZATION_LEVEL_1 = 0x2341,
    OPTIX_COMPILE_OPTIMIZATION_LEVEL_2 = 0x2342,
    OPTIX_COMPILE_OPTIMIZATION_LEVEL_3 = 0x2343 }
• enum OptixCompileDebugLevel {
    OPTIX_COMPILE_DEBUG_LEVEL_DEFAULT = 0,
    OPTIX_COMPILE_DEBUG_LEVEL_NONE = 0x2350,
    OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL = 0x2351,
    OPTIX_COMPILE_DEBUG_LEVEL_MODERATE = 0x2353,
    OPTIX_COMPILE_DEBUG_LEVEL_FULL = 0x2352 }
• enum OptixModuleCompileState {
    OPTIX_MODULE_COMPILE_STATE_NOT_STARTED = 0x2360,
    OPTIX_MODULE_COMPILE_STATE_STARTED = 0x2361,
    OPTIX_MODULE_COMPILE_STATE_IMPENDING_FAILURE = 0x2362,
    OPTIX_MODULE_COMPILE_STATE_FAILED = 0x2363,
    OPTIX_MODULE_COMPILE_STATE_COMPLETED = 0x2364 }
• enum OptixCreationFlags {
    OPTIX_CREATION_FLAG_NONE = 0,
    OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE = 1 << 0 }
• enum OptixPayloadTypeID {
    OPTIX_PAYLOAD_TYPE_DEFAULT = 0,
    OPTIX_PAYLOAD_TYPE_ID_0 = (1 << 0u),
    OPTIX_PAYLOAD_TYPE_ID_1 = (1 << 1u),
    OPTIX_PAYLOAD_TYPE_ID_2 = (1 << 2u),
    OPTIX_PAYLOAD_TYPE_ID_3 = (1 << 3u),
    OPTIX_PAYLOAD_TYPE_ID_4 = (1 << 4u),
    OPTIX_PAYLOAD_TYPE_ID_5 = (1 << 5u),
    OPTIX_PAYLOAD_TYPE_ID_6 = (1 << 6u),
    OPTIX_PAYLOAD_TYPE_ID_7 = (1 << 7u)}
• enum OptixPayloadSemantics {
    OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_NONE = 0,
    OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ = 1u << 0,

---

OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_WRITE = 2u << 0 ,
OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ_WRITE = 3u << 0 ,
OPTIX_PAYLOAD_SEMANTICS_CH_NONE = 0 ,
OPTIX_PAYLOAD_SEMANTICS_CH_READ = 1u << 2 ,
OPTIX_PAYLOAD_SEMANTICS_CH_WRITE = 2u << 2 ,
OPTIX_PAYLOAD_SEMANTICS_CH_READ_WRITE = 3u << 2 ,
OPTIX_PAYLOAD_SEMANTICS_MS_NONE = 0 ,
OPTIX_PAYLOAD_SEMANTICS_MS_READ = 1u << 4 ,
OPTIX_PAYLOAD_SEMANTICS_MS_WRITE = 2u << 4 ,
OPTIX_PAYLOAD_SEMANTICS_MS_READ_WRITE = 3u << 4 ,
OPTIX_PAYLOAD_SEMANTICS_AH_NONE = 0 ,
OPTIX_PAYLOAD_SEMANTICS_AH_READ = 1u << 6 ,
OPTIX_PAYLOAD_SEMANTICS_AH_WRITE = 2u << 6 ,
OPTIX_PAYLOAD_SEMANTICS_AH_READ_WRITE = 3u << 6 ,
OPTIX_PAYLOAD_SEMANTICS_IS_NONE = 0 ,
OPTIX_PAYLOAD_SEMANTICS_IS_READ = 1u << 8 ,
OPTIX_PAYLOAD_SEMANTICS_IS_WRITE = 2u << 8 ,
OPTIX_PAYLOAD_SEMANTICS_IS_READ_WRITE = 3u << 8 }

• enum OptixProgramGroupKind {
OPTIX_PROGRAM_GROUP_KIND_RAYGEN = 0x2421 ,
OPTIX_PROGRAM_GROUP_KIND_MISS = 0x2422 ,
OPTIX_PROGRAM_GROUP_KIND_EXCEPTION = 0x2423 ,
OPTIX_PROGRAM_GROUP_KIND_HITGROUP = 0x2424 ,
OPTIX_PROGRAM_GROUP_KIND_CALLABLES = 0x2425 }
• enum OptixProgramGroupFlags { OPTIX_PROGRAM_GROUP_FLAGS_NONE = 0 }

• enum OptixExceptionCodes {
OPTIX_EXCEPTION_CODE_STACK_OVERFLOW = -1 ,
OPTIX_EXCEPTION_CODE_TRACE_DEPTH_EXCEEDED = -2 }

• enum OptixExceptionFlags {
OPTIX_EXCEPTION_FLAG_NONE = 0 ,
OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW = 1u << 0 ,
OPTIX_EXCEPTION_FLAG_TRACE_DEPTH = 1u << 1 ,
OPTIX_EXCEPTION_FLAG_USER = 1u << 2 }
• enum OptixDevicePropertyCoopVecFlags {
OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_NONE = 0 ,
OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_STANDARD = 1 << 0 }
• enum OptixCoopVecElemType {
OPTIX_COOP_VEC_ELEM_TYPE_UNKNOWN = 0x2A00 ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT16 = 0x2A01 ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT32 = 0x2A03 ,
OPTIX_COOP_VEC_ELEM_TYPE_UINT8 = 0x2A04 ,
OPTIX_COOP_VEC_ELEM_TYPE_INT8 = 0x2A05 ,
OPTIX_COOP_VEC_ELEM_TYPE_UINT32 = 0x2A08 ,
OPTIX_COOP_VEC_ELEM_TYPE_INT32 = 0x2A09 ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E4M3 = 0x2A0A ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E5M2 = 0x2A0B }
• enum OptixCoopVecMatrixLayout {
OPTIX_COOP_VEC_MATRIX_LAYOUT_ROW_MAJOR = 0x2A40 ,
OPTIX_COOP_VEC_MATRIX_LAYOUT_COLUMN_MAJOR = 0x2A41 ,
OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_OPTIMAL = 0x2A42 ,
OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL = 0x2A43 }
• enum OptixQueryFunctionTableOptions { OPTIX_QUERY_FUNCTION_TABLE_OPTION_
DUMMY = 0 }

---

## 5.16.1 Detailed Description

OptiX Types.

## 5.16.2 Macro Definition Documentation

## 5.16.2.1 OPTIX_AABB_BUFFER_BYTE_ALIGNMENT

#define OPTIX_AABB_BUFFER_BYTE_ALIGNMENT 8ull

Alignment requirement forOptixBuildInputCustomPrimitiveArray::aabbBuffers.

## 5.16.2.2 OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT

#define OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT 128ull

Alignment requirement for output and temporay buffers for acceleration structures.

## 5.16.2.3 OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_TYPE_COUNT

#define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_TYPE_COUNT 8

Maximum number of payload types allowed.

## 5.16.2.4 OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_VALUE_COUNT

#define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_VALUE_COUNT 32

Maximum number of payload values allowed.

## 5.16.2.5 OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT

#define OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT 0

Maximum number of registers allowed. Defaults to no explicit limit.

## 5.16.2.6 OPTIX_GEOMETRY_TRANSFORM_BYTE_ALIGNMENT

#define OPTIX_GEOMETRY_TRANSFORM_BYTE_ALIGNMENT 16ull

Alignment requirement forOptixBuildInputTriangleArray::preTransform.

## 5.16.2.7 OPTIX_INSTANCE_BYTE_ALIGNMENT

#define OPTIX_INSTANCE_BYTE_ALIGNMENT 16ull

Alignment requirement forOptixBuildInputInstanceArray::instances.

## 5.16.2.8 OPTIX_OPACITY_MICROMAP_ARRAY_BUFFER_BYTE_ALIGNMENT

#define OPTIX_OPACITY_MICROMAP_ARRAY_BUFFER_BYTE_ALIGNMENT 128ull

Alignment requirement for opacity micromap array buffers.

## 5.16.2.9 OPTIX_OPACITY_MICROMAP_DESC_BUFFER_BYTE_ALIGNMENT

#define OPTIX_OPACITY_MICROMAP_DESC_BUFFER_BYTE_ALIGNMENT 8ull

Alignment requirement forOptixOpacityMicromapArrayBuildInput::perMicromapDescBuffer.

---

## 5.16.2.10 OPTIX_OPACITY_MICROMAP_MAX_SUBDIVISION_LEVEL

#define OPTIX_OPACITY_MICROMAP_MAX_SUBDIVISION_LEVEL 12

Maximum subdivision level for opacity micromaps.

## 5.16.2.11 OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_CLUSTER_SKIP_OPACITY_MICROMAP

## #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_CLUSTER_SKIP_OPACITY_ MICROMAP (-5)

Predefined index to indicate that no opacity micromap applies for a triangle. The opaque/non-opaque
state is determined by the geometry flags, similar as for triangles in instances with the OPTIX_
INSTANCE_FLAG_DISABLE_OPACITY_MICROMAPS flag set. This special index is only available
for the opacity micromap index array supplied toOptixClusterAccelBuildInputTrianglesArgs. This
special index does NOT require the cluster to be built with OPTIX_CLUSTER_ACCEL_CLUSTER_
FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS.

## 5.16.2.12 OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_OPAQUE

## #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_OPAQUE (-2)

## 5.16.2.13 OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_TRANSPARENT

## #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_TRANSPARENT (-1)

Predefined index to indicate that a triangle in the BVH build doesn't have an associated opacity
micromap, and that it should revert to one of the four possible states for the full triangle.

## 5.16.2.14 OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_OPAQUE

#define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_OPAQUE (-4)

## 5.16.2.15 OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_TRANSPARENT

#define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_
TRANSPARENT (-3)

## 5.16.2.16 OPTIX_OPACITY_MICROMAP_STATE_OPAQUE

#define OPTIX_OPACITY_MICROMAP_STATE_OPAQUE (1)

## 5.16.2.17 OPTIX_OPACITY_MICROMAP_STATE_TRANSPARENT

#define OPTIX_OPACITY_MICROMAP_STATE_TRANSPARENT (0)

Opacity micromaps encode the states of microtriangles in either 1 bit (2-state) or 2 bits (4-state) using
the following values.

## 5.16.2.18 OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_OPAQUE

#define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_OPAQUE (3)

## 5.16.2.19 OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_TRANSPARENT

#define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_TRANSPARENT (2)

---

## 5.16.2.20 OPTIX_SBT_RECORD_ALIGNMENT

#define OPTIX_SBT_RECORD_ALIGNMENT 16ull

Alignment requirement for device pointers inOptixShaderBindingTable.

## 5.16.2.21 OPTIX_SBT_RECORD_HEADER_SIZE

#define OPTIX_SBT_RECORD_HEADER_SIZE ((size_t)32)

Size of the SBT record headers.

## 5.16.2.22 OPTIX_TRANSFORM_BYTE_ALIGNMENT

#define OPTIX_TRANSFORM_BYTE_ALIGNMENT 64ull

Alignment requirement forOptixStaticTransform,OptixMatrixMotionTransform,
OptixSRTMotionTransform.

## 5.16.3 Typedef Documentation

## 5.16.3.1 CUdeviceptr

typedef unsigned long longCUdeviceptr

## 5.16.3.2 OptixAabb

CUDA device pointer.

typedef structOptixAabbOptixAabb

## 5.16.3.3 OptixAccelBufferSizes

AABB inputs.

typedef structOptixAccelBufferSizesOptixAccelBufferSizes

Struct for querying builder allocation requirements.

Once queried the sizes should be used to allocate device memory of at least these sizes.

See alsooptixAccelComputeMemoryUsage( )

## 5.16.3.4 OptixAccelBuildOptions

typedef structOptixAccelBuildOptionsOptixAccelBuildOptions

Build options for acceleration structures.

## 5.16.3.5 OptixAccelEmitDesc

See alsooptixAccelComputeMemoryUsage( ),optixAccelBuild( )

typedef structOptixAccelEmitDescOptixAccelEmitDesc

Specifies a type and output destination for emitted post-build properties.

See alsooptixAccelBuild( )

## 5.16.3.6 OptixAccelPropertyType

typedef enumOptixAccelPropertyTypeOptixAccelPropertyType

---

Properties which can be emitted during acceleration structure build.

See alsoOptixAccelEmitDesc::type.

## 5.16.3.7 OptixBuildFlags

typedef enumOptixBuildFlagsOptixBuildFlags

Builder Options.

Used forOptixAccelBuildOptions::buildFlags. Can be or'ed together.

## 5.16.3.8 OptixBuildInput

typedef structOptixBuildInputOptixBuildInput

Build inputs.

All of them support motion and the size of the data arrays needs to match the number of motion steps
See alsooptixAccelComputeMemoryUsage( ),optixAccelBuild( )

## 5.16.3.9 OptixBuildInputCurveArray

typedef structOptixBuildInputCurveArrayOptixBuildInputCurveArray

Curve inputs.

A curve is a swept surface defined by a 3D spline curve and a varying width (radius). A curve (or
"strand") of degree d (3=cubic, 2=quadratic, 1=linear) is represented by N *>* d vertices and N width
values, and comprises N - d segments. Each segment is defined by d+1 consecutive vertices. Each
curve may have a different number of vertices.

OptiX describes the curve array as a list of curve segments. The primitive id is the segment number. It
is the user's responsibility to maintain a mapping between curves and curve segments. Each index
buffer entry i = indexBuffer[primid] specifies the start of a curve segment, represented by d+1
consecutive vertices in the vertex buffer, and d+1 consecutive widths in the width buffer. Width is
interpolated the same way vertices are interpolated, that is, using the curve basis.

Each curves build input has only one SBT record. To create curves with different materials in the same
BVH, use multiple build inputs.

See alsoOptixBuildInput::curveArray

## 5.16.3.10 OptixBuildInputCustomPrimitiveArray

typedef structOptixBuildInputCustomPrimitiveArray

OptixBuildInputCustomPrimitiveArray

Custom primitive inputs.

See alsoOptixBuildInput::customPrimitiveArray

## 5.16.3.11 OptixBuildInputInstanceArray

typedef structOptixBuildInputInstanceArrayOptixBuildInputInstanceArray
Instance and instance pointer inputs.

See alsoOptixBuildInput::instanceArray

---

## 5.16.3.12 OptixBuildInputOpacityMicromap

typedef structOptixBuildInputOpacityMicromapOptixBuildInputOpacityMicromap

## 5.16.3.13 OptixBuildInputSphereArray

typedef structOptixBuildInputSphereArrayOptixBuildInputSphereArray

Sphere inputs.

A sphere is defined by a center point and a radius. Each center point is represented by a vertex in the
vertex buffer. There is either a single radius for all spheres, or the radii are represented by entries in the
radius buffer.

The vertex buffers and radius buffers point to a host array of device pointers, one per motion step. Host
array size must match the number of motion keys as set inOptixMotionOptions(or an array of size 1 if
OptixMotionOptions::numKeysis set to 0 or 1). Each per motion key device pointer must point to an
array of vertices corresponding to the center points of the spheres, or an array of 1 or N radii. Format
OPTIX_VERTEX_FORMAT_FLOAT3 is used for vertices, OPTIX_VERTEX_FORMAT_FLOAT for radii.

See alsoOptixBuildInput::sphereArray

## 5.16.3.14 OptixBuildInputTriangleArray

typedef structOptixBuildInputTriangleArrayOptixBuildInputTriangleArray
Triangle inputs.

See alsoOptixBuildInput::triangleArray

## 5.16.3.15 OptixBuildInputType

typedef enumOptixBuildInputTypeOptixBuildInputType

Enum to distinguish the different build input types.

See alsoOptixBuildInput::type

## 5.16.3.16 OptixBuildOperation

typedef enumOptixBuildOperationOptixBuildOperation

Enum to specify the acceleration build operation.

Used inOptixAccelBuildOptions, which is then passed to optixAccelBuild and

optixAccelComputeMemoryUsage, this enum indicates whether to do a build or an update of the
acceleration structure.

Acceleration structure updates utilize the same acceleration structure, but with updated bounds.

Updates are typically much faster than builds, however, large perturbations can degrade the quality of
the acceleration structure.

See alsooptixAccelComputeMemoryUsage( ),optixAccelBuild( ),OptixAccelBuildOptions

## 5.16.3.17 OptixBuiltinISOptions

typedef structOptixBuiltinISOptionsOptixBuiltinISOptions

Specifies the options for retrieving an intersection program for a built-in primitive type. The primitive
type must not be OPTIX_PRIMITIVE_TYPE_CUSTOM.

See alsooptixBuiltinISModuleGet( )

---

## 5.16.3.18 OptixClusterAccelBuildFlags

typedef enumOptixClusterAccelBuildFlagsOptixClusterAccelBuildFlags
Host-side flags for all types of cluster builds.

## 5.16.3.19 OptixClusterAccelBuildInput

typedef structOptixClusterAccelBuildInputOptixClusterAccelBuildInput

## 5.16.3.20 OptixClusterAccelBuildInputClusters

typedef structOptixClusterAccelBuildInputClusters
OptixClusterAccelBuildInputClusters

## 5.16.3.21 OptixClusterAccelBuildInputClustersArgs

typedef structOptixClusterAccelBuildInputClustersArgs
OptixClusterAccelBuildInputClustersArgs

## 5.16.3.22 OptixClusterAccelBuildInputGrids

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS
builds.

typedef structOptixClusterAccelBuildInputGrids
OptixClusterAccelBuildInputGrids

## 5.16.3.23 OptixClusterAccelBuildInputGridsArgs

typedef structOptixClusterAccelBuildInputGridsArgs
OptixClusterAccelBuildInputGridsArgs

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_
GRIDS builds.

## 5.16.3.24 OptixClusterAccelBuildInputTemplatesArgs

typedef structOptixClusterAccelBuildInputTemplatesArgs
OptixClusterAccelBuildInputTemplatesArgs

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_
TEMPLATES builds.

## 5.16.3.25 OptixClusterAccelBuildInputTriangles

typedef structOptixClusterAccelBuildInputTriangles
OptixClusterAccelBuildInputTriangles

## 5.16.3.26 OptixClusterAccelBuildInputTrianglesArgs

typedef structOptixClusterAccelBuildInputTrianglesArgs
OptixClusterAccelBuildInputTrianglesArgs

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_
TRIANGLES builds and OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES
builds.

---

## 5.16.3.27 OptixClusterAccelBuildMode

typedef enumOptixClusterAccelBuildModeOptixClusterAccelBuildMode
Build mode for cluster builds.

## 5.16.3.28 OptixClusterAccelBuildModeDesc

typedef structOptixClusterAccelBuildModeDescOptixClusterAccelBuildModeDesc

## 5.16.3.29 OptixClusterAccelBuildModeDescExplicitDest

typedef structOptixClusterAccelBuildModeDescExplicitDest
OptixClusterAccelBuildModeDescExplicitDest

## 5.16.3.30 OptixClusterAccelBuildModeDescGetSize

typedef structOptixClusterAccelBuildModeDescGetSize
OptixClusterAccelBuildModeDescGetSize

## 5.16.3.31 OptixClusterAccelBuildModeDescImplicitDest

typedef structOptixClusterAccelBuildModeDescImplicitDest
OptixClusterAccelBuildModeDescImplicitDest

## 5.16.3.32 OptixClusterAccelBuildType

typedef enumOptixClusterAccelBuildTypeOptixClusterAccelBuildType

## 5.16.3.33 OptixClusterAccelClusterFlags

Build type for cluster builds - specifying the type of data input and output.

typedef enumOptixClusterAccelClusterFlagsOptixClusterAccelClusterFlags
Device-side flags for clusters builds.

## 5.16.3.34 OptixClusterAccelIndicesFormat

typedef enumOptixClusterAccelIndicesFormatOptixClusterAccelIndicesFormat

Helper enum where values match the byte count of the corresponding index format, allowing usage of
enum value when specifying byte count.

## 5.16.3.35 OptixClusterAccelPrimitiveFlags

typedef enumOptixClusterAccelPrimitiveFlagsOptixClusterAccelPrimitiveFlags

Device-side flags that specify per-primitive specific behavior Note the packing within the 32b struct
OptixClusterAccelPrimitiveInfo.

## 5.16.3.36 OptixClusterAccelPrimitiveInfo

## 5.16.3.37 OptixClusterIDValues

typedef structOptixClusterAccelPrimitiveInfoOptixClusterAccelPrimitiveInfo

typedef enumOptixClusterIDValuesOptixClusterIDValues

Reserved value for cluster IDs in Args.

---

## 5.16.3.38 OptixCompileDebugLevel

typedef enumOptixCompileDebugLevelOptixCompileDebugLevel
Debug levels.

See alsoOptixModuleCompileOptions::debugLevel

## 5.16.3.39 OptixCompileOptimizationLevel

typedef enumOptixCompileOptimizationLevelOptixCompileOptimizationLevel
Optimization levels.

See alsoOptixModuleCompileOptions::optLevel

## 5.16.3.40 OptixCoopVecElemType

## 5.16.3.41 OptixCoopVecMatrixDescription

typedef enumOptixCoopVecElemTypeOptixCoopVecElemType

typedef structOptixCoopVecMatrixDescriptionOptixCoopVecMatrixDescription
Each matrix's offset from the base address is expressed with offsetInBytes. This allows for non-uniform
matrices to be tightly packed.

The rowColumnStrideInBytes is ignored if the layout is either OPTIX_COOP_VEC_MATRIX_LAYOUT
_INFERENCING_OPTIMAL or OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL

## 5.16.3.42 OptixCoopVecMatrixLayout

typedef enumOptixCoopVecMatrixLayoutOptixCoopVecMatrixLayout

## 5.16.3.43 OptixCreationFlags

typedef enumOptixCreationFlagsOptixCreationFlags

Flags for canceling the creation of an OptiX object.

If OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE is set, the calling thread will block until
one of these conditions is met:

1.All executing object creation threads have processed the new state

2.The creation of the object has finished, in which case the new state will be ignored

If OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE is not set, any *CancelCreation* call will
return without blocking. Note that the cancel request may still be ignored if all creation threads finish
their tasks before they can process the new state.

See alsooptixModuleCancelCreation( ), #optixPipelineCancelCreations(),
optixDeviceContextCancelCreations( )

## 5.16.3.44 OptixCurveEndcapFlags

typedef enumOptixCurveEndcapFlagsOptixCurveEndcapFlags

Curve end cap types, for non-linear curves.

---

## 5.16.3.45 OptixDenoiser

typedef struct OptixDenoiser_t*∗* OptixDenoiser

Opaque type representing a denoiser instance.

## 5.16.3.46 OptixDenoiserAlphaMode

typedef enumOptixDenoiserAlphaModeOptixDenoiserAlphaMode

Alpha denoising mode.

See alsooptixDenoiserCreate( )

## 5.16.3.47 OptixDenoiserAOVType

typedef enumOptixDenoiserAOVTypeOptixDenoiserAOVType
AOV type used by the denoiser.

## 5.16.3.48 OptixDenoiserGuideLayer

typedef structOptixDenoiserGuideLayerOptixDenoiserGuideLayer
Guide layer for the denoiser.

See alsooptixDenoiserInvoke( )

## 5.16.3.49 OptixDenoiserLayer

typedef structOptixDenoiserLayerOptixDenoiserLayer

Input/Output layers for the denoiser.

See alsooptixDenoiserInvoke( )

## 5.16.3.50 OptixDenoiserModelKind

typedef enumOptixDenoiserModelKindOptixDenoiserModelKind
Model kind used by the denoiser.

See alsooptixDenoiserCreate

## 5.16.3.51 OptixDenoiserOptions

typedef structOptixDenoiserOptionsOptixDenoiserOptions

Options used by the denoiser.

See alsooptixDenoiserCreate( )

## 5.16.3.52 OptixDenoiserParams

Various parameters used by the denoiser.

typedef structOptixDenoiserParamsOptixDenoiserParams

See alsooptixDenoiserInvoke( )

optixDenoiserComputeIntensity( )

optixDenoiserComputeAverageColor( )

---

## 5.16.3.53 OptixDenoiserSizes

typedef structOptixDenoiserSizesOptixDenoiserSizes

Various sizes related to the denoiser.

See alsooptixDenoiserComputeMemoryResources( )

## 5.16.3.54 OptixDeviceContext

typedef struct OptixDeviceContext_t*∗* OptixDeviceContext

Opaque type representing a device context.

## 5.16.3.55 OptixDeviceContextOptions

typedef structOptixDeviceContextOptionsOptixDeviceContextOptions

Parameters used foroptixDeviceContextCreate( )

See alsooptixDeviceContextCreate( )

## 5.16.3.56 OptixDeviceContextValidationMode

typedef enumOptixDeviceContextValidationMode

OptixDeviceContextValidationMode

Validation mode settings.

When enabled, certain device code utilities will be enabled to provide as good debug and error
checking facilities as possible.

See alsooptixDeviceContextCreate( )

## 5.16.3.57 OptixDeviceProperty

typedef enumOptixDevicePropertyOptixDeviceProperty

Parameters used foroptixDeviceContextGetProperty( )

See alsooptixDeviceContextGetProperty( )

## 5.16.3.58 OptixDevicePropertyClusterAccelFlags

typedef enumOptixDevicePropertyClusterAccelFlags

OptixDevicePropertyClusterAccelFlags

Flags used to interpret the result ofoptixDeviceContextGetProperty( )and OPTIX_DEVICE_
PROPERTY_CLUSTER_ACCEL.

See alsooptixDeviceContextGetProperty( )

## 5.16.3.59 OptixDevicePropertyCoopVecFlags

typedef enumOptixDevicePropertyCoopVecFlagsOptixDevicePropertyCoopVecFlags

Flags used to interpret the result ofoptixDeviceContextGetProperty( )and OPTIX_DEVICE_
PROPERTY_COOP_VEC.

See alsooptixDeviceContextGetProperty( )

---

## 5.16.3.60 OptixDevicePropertyShaderExecutionReorderingFlags

typedef enumOptixDevicePropertyShaderExecutionReorderingFlags
OptixDevicePropertyShaderExecutionReorderingFlags

Flags used to interpret the result ofoptixDeviceContextGetProperty( )and OPTIX_DEVICE_
PROPERTY_SHADER_EXECUTION_REORDERING.

See alsooptixDeviceContextGetProperty( )

## 5.16.3.61 OptixExceptionCodes

typedef enumOptixExceptionCodesOptixExceptionCodes

The following values are used to indicate which exception was thrown.

## 5.16.3.62 OptixExceptionFlags

typedef enumOptixExceptionFlagsOptixExceptionFlags

See alsoOptixPipelineCompileOptions::exceptionFlags,OptixExceptionCodes

## 5.16.3.63 OptixGeometryFlags

## Exception flags.

typedef enumOptixGeometryFlagsOptixGeometryFlags

Flags used byOptixBuildInputTriangleArray::flags,OptixBuildInputSphereArray::flagsand
OptixBuildInputCustomPrimitiveArray::flags.

## 5.16.3.64 OptixHitKind

typedef enumOptixHitKindOptixHitKind

Legacy type: A subset of the hit kinds for built-in primitive intersections. It is preferred to use
optixGetPrimitiveType( ), together withoptixIsFrontFaceHit( )oroptixIsBackFaceHit( ).

See alsooptixGetHitKind( )

## 5.16.3.65 OptixImage2D

typedef structOptixImage2DOptixImage2D

Image descriptor used by the denoiser.

See alsooptixDenoiserInvoke( ),optixDenoiserComputeIntensity( )

## 5.16.3.66 OptixIndicesFormat

typedef enumOptixIndicesFormatOptixIndicesFormat

Format of indices used intOptixBuildInputTriangleArray::indexFormat.

## 5.16.3.67 OptixInstance

typedef structOptixInstanceOptixInstance

Instances.

See alsoOptixBuildInputInstanceArray::instances

---

## 5.16.3.68 OptixInstanceFlags

typedef enumOptixInstanceFlagsOptixInstanceFlags

Flags set on theOptixInstance::flags.

These can be or'ed together to combine multiple flags.

## 5.16.3.69 OptixLogCallback

typedef void(*∗* OptixLogCallback) (unsigned int level, const char *∗*tag, const
char *∗*message, void *∗*cbdata)

Type of the callback function used for log messages.

Parameters

| in | level | The log level indicates the severity of the message. See below for possible values. |
| --- | --- | --- |
| in | tag | A terse message category description (e.g., 'SCENE STAT') |
| in | message | Null terminated log message (without newline at the end). |
| in | cbdata | Callback data that was provided with the callback pointer. |

It is the users responsibility to ensure thread safety within this function.

The following log levels are defined.

0 disable Setting the callback level will disable all messages. The callback function will not be called in
this case. 1 fatal A non-recoverable error. The context and/or OptiX itself might no longer be in a
usable state. 2 error A recoverable error, e.g., when passing invalid call parameters. 3 warning Hints
that OptiX might not behave exactly as requested by the user or may perform slower than expected. 4
print Status or progress messages.

Higher levels might occur.

See alsooptixDeviceContextSetLogCallback( ),OptixDeviceContextOptions

## 5.16.3.70 OptixMatrixMotionTransform

typedef structOptixMatrixMotionTransformOptixMatrixMotionTransform

Represents a matrix motion transformation.

The device address of instances of this type must be a multiple of OPTIX_TRANSFORM_BYTE_
ALIGNMENT.

This struct, as defined here, handles only N=2 motion keys due to the fixed array length of its
transform member. The following example shows how to create instances for an arbitrary number N of
motion keys:

floatmatrixData[N][12];
...// setup matrixData
size_ttransformSizeInBytes =sizeof(OptixMatrixMotionTransform) + (N-2) * 12 *sizeof(float);
OptixMatrixMotionTransform* matrixMoptionTransform = (OptixMatrixMotionTransform*)
malloc(transformSizeInBytes);
memset(matrixMoptionTransform, 0, transformSizeInBytes);
...// setup other members of matrixMoptionTransform
matrixMoptionTransform->motionOptions.numKeys
memcpy(matrixMoptionTransform->transform, matrixData, N * 12 *sizeof(float));
...// copy matrixMoptionTransform to device memory
free(matrixMoptionTransform)

See alsooptixConvertPointerToTraversableHandle( )

---

## 5.16.3.71 OptixMicromapBuffers

typedef structOptixMicromapBuffersOptixMicromapBuffers

Buffer inputs for opacity micromap array builds.

## 5.16.3.72 OptixMicromapBufferSizes

typedef structOptixMicromapBufferSizesOptixMicromapBufferSizes

Conservative memory requirements for building a opacity micromap array.

## 5.16.3.73 OptixModule

typedef struct OptixModule_t*∗* OptixModule

Opaque type representing a module.

## 5.16.3.74 OptixModuleCompileBoundValueEntry

typedef structOptixModuleCompileBoundValueEntry

OptixModuleCompileBoundValueEntry

Struct for specifying specializations for pipelineParams as specified inOptixPipelineCompileOptions
::pipelineLaunchParamsVariableName.

The bound values are supposed to represent a constant value in the pipelineParams. OptiX will
attempt to locate all loads from the pipelineParams and correlate them to the appropriate bound value,
but there are cases where OptiX cannot safely or reliably do this. For example if the pointer to the
pipelineParams is passed as an argument to a non-inline function or the offset of the load to the
pipelineParams cannot be statically determined (e.g. accessed in a loop). No module should rely on the
value being specialized in order to work correctly. The values in the pipelineParams specified on
optixLaunch should match the bound value. If validation mode is enabled on the context, OptiX will
verify that the bound values specified matches the values in pipelineParams specified to optixLaunch.

These values are compiled in to the module as constants. Once the constants are inserted into the code,
an optimization pass will be run that will attempt to propagate the consants and remove unreachable
code.

If caching is enabled, changes in these values will result in newly compiled modules.

The pipelineParamOffset and sizeInBytes must be within the bounds of the pipelineParams variable.
OPTIX_ERROR_INVALID_VALUE will be returned from optixModuleCreate otherwise.

If more than one bound value overlaps or the size of a bound value is equal to 0, an OPTIX_ERROR_
INVALID_VALUE will be returned from optixModuleCreate.

The same set of bound values do not need to be used for all modules in a pipeline, but overlapping
values between modules must have the same value. OPTIX_ERROR_INVALID_VALUE will be
returned from optixPipelineCreate otherwise.

See alsoOptixModuleCompileOptions

## 5.16.3.75 OptixModuleCompileOptions

typedef structOptixModuleCompileOptionsOptixModuleCompileOptions

Compilation options for module.

See alsooptixModuleCreate( )

---

<u>145</u>

## 5.16.3.76 OptixModuleCompileState

typedef enumOptixModuleCompileStateOptixModuleCompileState

Module compilation state.

See alsooptixModuleGetCompilationState( ),optixModuleCreateWithTasks( )

## 5.16.3.77 OptixMotionFlags

typedef enumOptixMotionFlagsOptixMotionFlags

Enum to specify motion flags.

See alsoOptixMotionOptions::flags.

## 5.16.3.78 OptixMotionOptions

typedef structOptixMotionOptionsOptixMotionOptions

Motion options.

See alsoOptixAccelBuildOptions::motionOptions,OptixMatrixMotionTransform::motionOptions,
OptixSRTMotionTransform::motionOptions

## 5.16.3.79 OptixNetworkDescription

typedef structOptixNetworkDescriptionOptixNetworkDescription

## 5.16.3.80 OptixOpacityMicromapArrayBuildInput

typedef structOptixOpacityMicromapArrayBuildInput
OptixOpacityMicromapArrayBuildInput

Inputs to opacity micromap array construction.

## 5.16.3.81 OptixOpacityMicromapArrayIndexingMode

typedef enumOptixOpacityMicromapArrayIndexingMode
OptixOpacityMicromapArrayIndexingMode

indexing mode of triangles to opacity micromaps in an array, used in
OptixBuildInputOpacityMicromap.

## 5.16.3.82 OptixOpacityMicromapDesc

typedef structOptixOpacityMicromapDescOptixOpacityMicromapDesc
Opacity micromap descriptor.

## 5.16.3.83 OptixOpacityMicromapFlags

typedef enumOptixOpacityMicromapFlagsOptixOpacityMicromapFlags
Flags defining behavior of opacity micromaps in a opacity micromap array.

## 5.16.3.84 OptixOpacityMicromapFormat

typedef enumOptixOpacityMicromapFormatOptixOpacityMicromapFormat
Specifies whether to use a 2- or 4-state opacity micromap format.

---

<u>146</u>

## 5.16.3.85 OptixOpacityMicromapHistogramEntry

typedef structOptixOpacityMicromapHistogramEntry

OptixOpacityMicromapHistogramEntry

Opacity micromap histogram entry. Specifies how many opacity micromaps of a specific type are input
to the opacity micromap array build. Note that while this is similar to

OptixOpacityMicromapUsageCount, the histogram entry specifies how many opacity micromaps of a
specific type are combined into a opacity micromap array.

## 5.16.3.86 OptixOpacityMicromapUsageCount

typedef structOptixOpacityMicromapUsageCountOptixOpacityMicromapUsageCount

Opacity micromap usage count for acceleration structure builds. Specifies how many opacity
micromaps of a specific type are referenced by triangles when building the AS. Note that while this is
similar toOptixOpacityMicromapHistogramEntry, the usage count specifies how many opacity
micromaps of a specific type are referenced by triangles in the AS.

## 5.16.3.87 OptixPayloadSemantics

Semantic flags for a single payload word.

typedef enumOptixPayloadSemanticsOptixPayloadSemantics

Used to specify the semantics of a payload word per shader type. "read": Shader of this type may read
the payload word. "write": Shader of this type may write the payload word.

"trace_caller_write": Shaders may consume the value of the payload word passed to optixTrace by the
caller. "trace_caller_read": The caller to optixTrace may read the payload word after the call to
optixTrace.

Semantics can be bitwise combined. Combining "read" and "write" is equivalent to specifying "read_
write". A payload needs to be writable by the caller or at least one shader type. A payload needs to be
readable by the caller or at least one shader type after a being writable.

## 5.16.3.88 OptixPayloadType

typedef structOptixPayloadTypeOptixPayloadType

Specifies a single payload type.

## 5.16.3.89 OptixPayloadTypeID

typedef enumOptixPayloadTypeIDOptixPayloadTypeID

Payload type identifiers.

## 5.16.3.90 OptixPipeline

typedef struct OptixPipeline_t*∗* OptixPipeline

Opaque type representing a pipeline.

## 5.16.3.91 OptixPipelineCompileOptions

typedef structOptixPipelineCompileOptionsOptixPipelineCompileOptions

Compilation options for all modules of a pipeline.

Similar toOptixModuleCompileOptions, but these options here need to be equal for all modules of a

---

<u>147</u>

pipeline.

See alsooptixModuleCreate( ),optixPipelineCreate( )

## 5.16.3.92 OptixPipelineLinkOptions

typedef structOptixPipelineLinkOptionsOptixPipelineLinkOptions

Link options for a pipeline.

See alsooptixPipelineCreate( )

## 5.16.3.93 OptixPipelineSymbolMemcpyKind

typedef enumOptixPipelineSymbolMemcpyKindOptixPipelineSymbolMemcpyKind

Flags used to interpret the source and target of memory copies when using
optixPipelineSymbolMemcpyAsync( )

See alsooptixPipelineSymbolMemcpyAsync( )

## 5.16.3.94 OptixPixelFormat

typedef enumOptixPixelFormatOptixPixelFormat

Pixel formats used by the denoiser.

See alsoOptixImage2D::format

## 5.16.3.95 OptixPrimitiveType

typedef enumOptixPrimitiveTypeOptixPrimitiveType

Builtin primitive types.

## 5.16.3.96 OptixPrimitiveTypeFlags

typedef enumOptixPrimitiveTypeFlagsOptixPrimitiveTypeFlags

Builtin flags may be bitwise combined.

See alsoOptixPipelineCompileOptions::usesPrimitiveTypeFlags

## 5.16.3.97 OptixProgramGroup

typedef struct OptixProgramGroup_t*∗* OptixProgramGroup

Opaque type representing a program group.

## 5.16.3.98 OptixProgramGroupCallables

typedef structOptixProgramGroupCallablesOptixProgramGroupCallables

Program group representing callables.

Module and entry function name need to be valid for at least one of the two callables.

See also #OptixProgramGroupDesc::callables

## 5.16.3.99 OptixProgramGroupDesc

typedef structOptixProgramGroupDescOptixProgramGroupDesc

Descriptor for program groups.

---

<u>148</u>

## 5.16.3.100 OptixProgramGroupFlags

typedef enumOptixProgramGroupFlagsOptixProgramGroupFlags

Flags for program groups.

## 5.16.3.101 OptixProgramGroupHitgroup

typedef structOptixProgramGroupHitgroupOptixProgramGroupHitgroup

Program group representing the hitgroup.

For each of the three program types, module and entry function name might both be nullptr.

See alsoOptixProgramGroupDesc::hitgroup

## 5.16.3.102 OptixProgramGroupKind

typedef enumOptixProgramGroupKindOptixProgramGroupKind

## 5.16.3.103 OptixProgramGroupOptions

Distinguishes different kinds of program groups.

typedef structOptixProgramGroupOptionsOptixProgramGroupOptions

Program group options.

See alsooptixProgramGroupCreate( )

## 5.16.3.104 OptixProgramGroupSingleModule

typedef structOptixProgramGroupSingleModuleOptixProgramGroupSingleModule

Program group representing a single module.

Used for raygen, miss, and exception programs. In case of raygen and exception programs, module
and entry function name need to be valid. For miss programs, module and entry function name might
both be nullptr.

See alsoOptixProgramGroupDesc::raygen,OptixProgramGroupDesc::miss,OptixProgramGroupDesc
::exception

## 5.16.3.105 OptixQueryFunctionTable_t

typedefOptixResult() OptixQueryFunctionTable_t(int abiId, unsigned int
numOptions,OptixQueryFunctionTableOptions *∗*, const void *∗∗*, void
*∗*functionTable, size_t sizeOfTable)

Type of the function optixQueryFunctionTable( )

## 5.16.3.106 OptixQueryFunctionTableOptions

typedef enumOptixQueryFunctionTableOptionsOptixQueryFunctionTableOptions
Options that can be passed to optixQueryFunctionTable( )

## 5.16.3.107 OptixRayFlags

typedef enumOptixRayFlagsOptixRayFlags

Ray flags passed to the device functionoptixTrace( ). These affect the behavior of traversal per
invocation.

---

<u>149</u>

See alsooptixTrace( )

## 5.16.3.108 OptixRelocateInput

typedef structOptixRelocateInputOptixRelocateInput

Relocation inputs.

See alsooptixAccelRelocate( )

## 5.16.3.109 OptixRelocateInputInstanceArray

typedef structOptixRelocateInputInstanceArray
OptixRelocateInputInstanceArray

Instance and instance pointer inputs.

## 5.16.3.110 OptixRelocateInputOpacityMicromap

See alsoOptixRelocateInput::instanceArray

typedef structOptixRelocateInputOpacityMicromap
OptixRelocateInputOpacityMicromap

## 5.16.3.111 OptixRelocateInputTriangleArray

typedef structOptixRelocateInputTriangleArray
OptixRelocateInputTriangleArray

Triangle inputs.

See alsoOptixRelocateInput::triangleArray

## 5.16.3.112 OptixRelocationInfo

typedef structOptixRelocationInfoOptixRelocationInfo

Used to store information related to relocation of optix data structures.

See alsooptixOpacityMicromapArrayGetRelocationInfo( ),optixOpacityMicromapArrayRelocate( ),
optixAccelGetRelocationInfo( ),optixAccelRelocate( ),optixCheckRelocationCompatibility( )

## 5.16.3.113 OptixResult

typedef enumOptixResultOptixResult

Result codes returned from API functions.

All host side API functions return OptixResult with the exception of optixGetErrorName and
optixGetErrorString. When successful OPTIX_SUCCESS is returned. All return codes except for OPTIX
_SUCCESS should be assumed to be errors as opposed to a warning.

See alsooptixGetErrorName( ),optixGetErrorString( )

## 5.16.3.114 OptixShaderBindingTable

typedef structOptixShaderBindingTableOptixShaderBindingTable

Describes the shader binding table (SBT)

See alsooptixLaunch( )

---

<u>150</u>

## 5.16.3.115 OptixSRTData

typedef structOptixSRTDataOptixSRTData

Represents an SRT transformation.

An SRT transformation can represent a smooth rotation with fewer motion keys than a matrix
transformation. Each motion key is constructed from elements taken from a matrix S, a quaternion R,
and a translation T.
 

*sx a b pvx*
 
The scaling matrix *S* =  
 0 *sy c pvy* defines an affine transformation that can include scale,
0 0 *sz pvz*
shear, and a translation. The translation allows to define the pivot point for the subsequent rotation.
The quaternion R = [ qx, qy, qz, qw ] describes a rotation with angular component qw = cos(theta/2)
and other components [ qx, qy, qz ] = sin(theta/2) *∗* [ ax, ay, az ] where the axis [ ax, ay, az ] is
normalized.

$$
S={left[left\begin{matrix}{s x}&{a}&{b}&{p v x}\\ {0}&{s y}&{c}&{p v y}\\ {0}&{0}&{s z}&{p v z}\end{matrix}]}
$$

$$
\mathrm{q{}}\mathrm{{x},\,{\ q q}\mathrm\{{q,}}\mathrm{{q z}\ }mathrm={{s i n}}({\mathrm{t h}}e t t{/22}))*[\mathrm{{a x},\,{a y},\,{a z}\ }
$$

 
1 0 0 *tx*
 
The translation matrix *T* =  
0 1 0 *ty* defines another translation that is applied after the rotation.
0 0 1 *tz*
Typically, this translation includes the inverse translation from the matrix S to reverse the translation
for the pivot point for R.

$$
T=\left[\begin{matrix}{1}&{0}&{0}&{t x}\\ {0}&{1}&{0}&{t y}\\ {0}&{0}&{1}&{t z}\end{matrix}\right]
$$

To obtain the effective transformation at time t, the elements of the components of S, R, and T will be
interpolated linearly. The components are then multiplied to obtain the combined transformation C = T
∧
*∗* R *∗* S. The transformation C is the effective object-to-world transformations at time t, and C (-1) is the
effective world-to-object transformation at time t.

$$
*,mathrm{8,}\mathrm{{}}\mathrm{{S}}
$$

$$
\mathbb{C}^{\wedge}(-1)
$$

See alsoOptixSRTMotionTransform::srtData,optixConvertPointerToTraversableHandle( )

## 5.16.3.116 OptixSRTMotionTransform

typedef structOptixSRTMotionTransformOptixSRTMotionTransform

Represents an SRT motion transformation.

The device address of instances of this type must be a multiple of OPTIX_TRANSFORM_BYTE_
ALIGNMENT.

This struct, as defined here, handles only N=2 motion keys due to the fixed array length of its srtData
member. The following example shows how to create instances for an arbitrary number N of motion
keys:

OptixSRTDatasrtData[N];
...// setup srtData
size_ttransformSizeInBytes =sizeof(OptixSRTMotionTransform) + (N-2) *sizeof(OptixSRTData);
OptixSRTMotionTransform* srtMotionTransform = (OptixSRTMotionTransform*) malloc(transformSizeInBytes);
memset(srtMotionTransform, 0, transformSizeInBytes);
...// setup other members of srtMotionTransform
srtMotionTransform->motionOptions.numKeys= N;
memcpy(srtMotionTransform->srtData, srtData, N *sizeof(OptixSRTData));
...// copy srtMotionTransform to device memory
free(srtMotionTransform)

See alsooptixConvertPointerToTraversableHandle( )

## 5.16.3.117 OptixStackSizes

typedef structOptixStackSizesOptixStackSizes

Describes the stack size requirements of a program group.

---

<u>151</u>

See alsooptixProgramGroupGetStackSize( )

## 5.16.3.118 OptixStaticTransform

typedef structOptixStaticTransformOptixStaticTransform

Static transform.

The device address of instances of this type must be a multiple of OPTIX_TRANSFORM_BYTE_
ALIGNMENT.

See alsooptixConvertPointerToTraversableHandle( )

## 5.16.3.119 OptixTask

typedef struct OptixTask_t*∗* OptixTask

## 5.16.3.120 OptixTransformFormat

Opaque type representing a work task.

typedef enumOptixTransformFormatOptixTransformFormat

Format of transform used inOptixBuildInputTriangleArray::transformFormat.

## 5.16.3.121 OptixTransformType

typedef enumOptixTransformTypeOptixTransformType

Transform.

OptixTransformType is used by the device functionoptixGetTransformTypeFromHandle( )to
determine the type of the OptixTraversableHandle returned fromoptixGetTransformListHandle( ).

## 5.16.3.122 OptixTraversableGraphFlags

typedef enumOptixTraversableGraphFlagsOptixTraversableGraphFlags

Specifies the set of valid traversable graphs that may be passed to invocation ofoptixTrace( ). Flags
may be bitwise combined.

## 5.16.3.123 OptixTraversableHandle

typedef unsigned long longOptixTraversableHandle

Traversable handle.

## 5.16.3.124 OptixTraversableType

typedef enumOptixTraversableTypeOptixTraversableType

Traversable Handles.

See alsooptixConvertPointerToTraversableHandle( )

## 5.16.3.125 OptixTraverseData

typedef structOptixTraverseDataOptixTraverseData

Hit Object Struct to store the data collected in a hit object during traversal in an internal format using
optixHitObjectGetTraverseData( ). The hit object can be reconstructed using that data at a later
point with optixMakeHitObjectWithTraverseData( ).

---

152

## 5.16.3.126 OptixVertexFormat

typedef enumOptixVertexFormatOptixVertexFormat

Format of vertices used inOptixBuildInputTriangleArray::vertexFormat.

## 5.16.3.127 OptixVisibilityMask

typedef unsigned intOptixVisibilityMask

Visibility mask.

## 5.16.4 Enumeration Type Documentation

## 5.16.4.1 OptixAccelPropertyType

enumOptixAccelPropertyType

Properties which can be emitted during acceleration structure build.

See alsoOptixAccelEmitDesc::type.

Enumerator

| OPTIX_PROPERTY_TYPE_COMPACTED_SIZE | Size of a compacted acceleration structure. The device pointer points to a uint64. |
| --- | --- |
| OPTIX_PROPERTY_TYPE_AABBS | OptixAabb * numMotionSteps. |

## 5.16.4.2 OptixBuildFlags

enumOptixBuildFlags

Builder Options.

Used forOptixAccelBuildOptions::buildFlags. Can be or'ed together.

Enumerator

| OPTIX_BUILD_FLAG_NONE | No special flags set. |
| --- | --- |
| OPTIX_BUILD_FLAG_ALLOW_UPDATE | Allow updating the build with new vertex positions with subsequent calls to optixAccelBuild. |
| OPTIX_BUILD_FLAG_ALLOW_COMPACTION |  |
| OPTIX_BUILD_FLAG_PREFER_FAST_TRACE | This flag is mutually exclusive with OPTIX_BUILD_FLAG_PREFER_FAST_BUILD. |
| OPTIX_BUILD_FLAG_PREFER_FAST_BUILD | This flag is mutually exclusive with OPTIX_BUILD_FLAG_PREFER_FAST_TRACE. |

---

153

Enumerator

| OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS | Allow random access to build input vertices See optixGetTriangleVertexDataFromHandle optixGetLinearCurveVertexDataFromHandle optixGetQuadraticBSplineVertexDataFromHandle optixGetCubicBSplineVertexDataFromHandle optixGetCatmullRomVertexDataFromHandle optixGetCubicBezierVertexDataFromHandle optixGetQuadraticBSplineRocapsVertexDataFromHandle optixGetCubicBSplineRocapsVertexDataFromHandle optixGetCatmullRomRocapsVertexDataFromHandle optixGetCubicBezierRocapsVertexDataFromHandle optixGetRibbonVertexDataFromHandle optixGetRibbonNormalFromHandle optixGetSphereDataFromHandle. |
| --- | --- |
| OPTIX_BUILD_FLAG_ALLOW_RANDOM_INSTANCE_ACCESS | Allow random access to instances See optixGetInstanceTraversableFromIAS. |
| OPTIX_BUILD_FLAG_ALLOW_OPACITY_MICROMAP_UPDATE | Support updating the opacity micromap array and opacity micromap indices on refits. May increase AS size and may have a small negative impact on traversal performance. If this flag is absent, all opacity micromap inputs must remain unchanged between the initial AS builds and their subsequent refits. |
| OPTIX_BUILD_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS | If enabled, any instances referencing this GAS are allowed to disable the opacity micromap test through the DISABLE_OPACITY_MICROMAPS flag instance flag. Note that the GAS will not be optimized for the attached opacity micromap Arrays if this flag is set, which may result in reduced traversal performance. |

## 5.16.4.3 OptixBuildInputType

enumOptixBuildInputType

Enum to distinguish the different build input types.

See alsoOptixBuildInput::type

Enumerator

| OPTIX_BUILD_INPUT_TYPE_TRIANGLES | Triangle inputs. See alsoOptixBuildInputTriangleArray |
| --- | --- |
| OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES | Custom primitive inputs. See alsoOptixBuildInputCustomPrimitiveArray |

---

154

Enumerator

| OPTIX_BUILD_INPUT_TYPE_INSTANCES | Instance inputs. See alsoOptixBuildInputInstanceArray |
| --- | --- |
| OPTIX_BUILD_INPUT_TYPE_INSTANCE_POINTERS | Instance pointer inputs. See alsoOptixBuildInputInstanceArray |
| OPTIX_BUILD_INPUT_TYPE_CURVES | Curve inputs. See alsoOptixBuildInputCurveArray |
| OPTIX_BUILD_INPUT_TYPE_SPHERES | Sphere inputs. See alsoOptixBuildInputSphereArray |

## 5.16.4.4 OptixBuildOperation

enumOptixBuildOperation

Enum to specify the acceleration build operation.

Used inOptixAccelBuildOptions, which is then passed to optixAccelBuild and
optixAccelComputeMemoryUsage, this enum indicates whether to do a build or an update of the
acceleration structure.

Acceleration structure updates utilize the same acceleration structure, but with updated bounds.
Updates are typically much faster than builds, however, large perturbations can degrade the quality of
the acceleration structure.

See alsooptixAccelComputeMemoryUsage( ),optixAccelBuild( ),OptixAccelBuildOptions

Enumerator

| OPTIX_BUILD_OPERATION_BUILD | Perform a full build operation. |
| --- | --- |
| OPTIX_BUILD_OPERATION_UPDATE | Perform an update using new bounds. |

## 5.16.4.5 OptixClusterAccelBuildFlags

enumOptixClusterAccelBuildFlags

Host-side flags for all types of cluster builds.

Enumerator

| OPTIX_CLUSTER_ACCEL_BUILD_FLAG_NONE |
| --- |
| OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_TRACE |
| OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_BUILD |
| OPTIX_CLUSTER_ACCEL_BUILD_FLAG_ALLOW_OPACITY_MICROMAPS |

## 5.16.4.6 OptixClusterAccelBuildMode

enumOptixClusterAccelBuildMode

Build mode for cluster builds.

---

Enumerator

| OPTIX_CLUSTER_ACCEL_BUILD_MODE_IMPLICIT_DESTINATIONS | Fastest build, single output buffer, build outputs may have padding wrt each other. |
| --- | --- |
| OPTIX_CLUSTER_ACCEL_BUILD_MODE_EXPLICIT_DESTINATIONS | Compact build, application specifies output destination per Arg; requires Get Sizes build run beforehand. |
| OPTIX_CLUSTER_ACCEL_BUILD_MODE_GET_SIZES | Size computation for future explicit build; computes output sizes for all Args. |

## 5.16.4.7 OptixClusterAccelBuildType

enumOptixClusterAccelBuildType

Build type for cluster builds - specifying the type of data input and output.

Enumerator

OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS
OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLES
OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES
OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TEMPLATES
OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_GRIDS

## 5.16.4.8 OptixClusterAccelClusterFlags

enumOptixClusterAccelClusterFlags

Device-side flags for clusters builds.

Enumerator

| OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_NONE |  |
| --- | --- |
| OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS | Similar to the ALLOW_DISABLE_OPACITY_MICROMAPS build flag of regular triangle GAS builds. This flag is required if the CLAS is in an instance with the OPTIX_INSTANCE_FLAG_DISABLE_OPACITY_MICROMAPS flag set. |

## 5.16.4.9 OptixClusterAccelIndicesFormat

enumOptixClusterAccelIndicesFormat

Helper enum where values match the byte count of the corresponding index format, allowing usage of
enum value when specifying byte count.

Enumerator

OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_8BIT
OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_16BIT

---

Enumerator

<u>OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_32BIT</u>

## 5.16.4.10 OptixClusterAccelPrimitiveFlags

enumOptixClusterAccelPrimitiveFlags

Device-side flags that specify per-primitive specific behavior Note the packing within the 32b struct
OptixClusterAccelPrimitiveInfo.

Enumerator

| OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_NONE |
| --- |
| OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_TRIANGLE_FACE_CULLING |
| OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_REQUIRE_SINGLE_ANYHIT_CALL |
| OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_ANYHIT |

## 5.16.4.11 OptixClusterIDValues

enumOptixClusterIDValues

Reserved value for cluster IDs in Args.

Enumerator

<u>OPTIX_CLUSTER_ID_INVALID</u>

## 5.16.4.12 OptixCompileDebugLevel

enumOptixCompileDebugLevel

Debug levels.

See alsoOptixModuleCompileOptions::debugLevel

Enumerator

| OPTIX_COMPILE_DEBUG_LEVEL_DEFAULT | Default currently is minimal. |
| --- | --- |
| OPTIX_COMPILE_DEBUG_LEVEL_NONE | No debug information. |
| OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL | Generate information that does not impact performance. Note this replaces OPTIX_COMPILE_DEBUG_LEVEL_LINEINFO. |
| OPTIX_COMPILE_DEBUG_LEVEL_MODERATE | Generate some debug information with slight performance cost. |
| OPTIX_COMPILE_DEBUG_LEVEL_FULL | Generate full debug information. |

## 5.16.4.13 OptixCompileOptimizationLevel

enumOptixCompileOptimizationLevel

Optimization levels.

---

See alsoOptixModuleCompileOptions::optLevel

Enumerator

| OPTIX_COMPILE_OPTIMIZATION_DEFAULT | Default is to run all optimizations. |
| --- | --- |
| OPTIX_COMPILE_OPTIMIZATION_LEVEL_0 | No optimizations. |
| OPTIX_COMPILE_OPTIMIZATION_LEVEL_1 | Some optimizations. |
| OPTIX_COMPILE_OPTIMIZATION_LEVEL_2 | Most optimizations. |
| OPTIX_COMPILE_OPTIMIZATION_LEVEL_3 | All optimizations. |

## 5.16.4.14 OptixCoopVecElemType

enumOptixCoopVecElemType

Enumerator

| OPTIX_COOP_VEC_ELEM_TYPE_UNKNOWN |  |
| --- | --- |
| OPTIX_COOP_VEC_ELEM_TYPE_FLOAT16 | 16bit float |
| OPTIX_COOP_VEC_ELEM_TYPE_FLOAT32 | 32bit float |
| OPTIX_COOP_VEC_ELEM_TYPE_UINT8 | 8bit unsigned integer |
| OPTIX_COOP_VEC_ELEM_TYPE_INT8 | 8bit signed integer |
| OPTIX_COOP_VEC_ELEM_TYPE_UINT32 | 32bit unsigned integer |
| OPTIX_COOP_VEC_ELEM_TYPE_INT32 | 32bit signed integer |
| OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E4M3 | FLOAT8 type with 4 bits exponent,3bits mantissa.Only supported as the inputInterpretation and matrixElementType. |
| OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E5M2 | FLOAT8 type with 5 bits exponent,2bits mantissa.Only supported as the inputInterpretation and matrixElementType. |

## 5.16.4.15 OptixCoopVecMatrixLayout

enumOptixCoopVecMatrixLayout

Enumerator

| OPTIX_COOP_VEC_MATRIX_LAYOUT_ROW_MAJOR |
| --- |
| OPTIX_COOP_VEC_MATRIX_LAYOUT_COLUMN_MAJOR |
| OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_OPTIMAL |
| OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL |

## 5.16.4.16 OptixCreationFlags

enumOptixCreationFlags

Flags for canceling the creation of an OptiX object.

---

If OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE is set, the calling thread will block until
one of these conditions is met:

1.All executing object creation threads have processed the new state

2.The creation of the object has finished, in which case the new state will be ignored

If OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE is not set, any *CancelCreation* call will
return without blocking. Note that the cancel request may still be ignored if all creation threads finish
their tasks before they can process the new state.

See alsooptixModuleCancelCreation( ), #optixPipelineCancelCreations(),
optixDeviceContextCancelCreations( )

Enumerator

OPTIX_CREATION_FLAG_NONE

OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE

## 5.16.4.17 OptixCurveEndcapFlags

enumOptixCurveEndcapFlags

Curve end cap types, for non-linear curves.

Enumerator

| OPTIX_CURVE_ENDCAP_DEFAULT | Default end caps. Round end caps for linear, no end caps for quadratic/cubic. |
| --- | --- |
| OPTIX_CURVE_ENDCAP_ON | Flat end caps at both ends of quadratic/cubic curve segments.Not valid for linear. |

## 5.16.4.18 OptixDenoiserAlphaMode

enumOptixDenoiserAlphaMode

Alpha denoising mode.

See alsooptixDenoiserCreate( )

Enumerator

| OPTIX_DENOISER_ALPHA_MODE_COPY | Copy alpha(if present) from input layer, no denoising. |
| --- | --- |
| OPTIX_DENOISER_ALPHA_MODE_DENOISE | Denoise alpha. |

## 5.16.4.19 OptixDenoiserAOVType

enumOptixDenoiserAOVType

AOV type used by the denoiser.

---

Enumerator

| OPTIX_DENOISER_AOV_TYPE_NONE | Unspecified AOV type. |
| --- | --- |
| OPTIX_DENOISER_AOV_TYPE_BEAUTY |  |
| OPTIX_DENOISER_AOV_TYPE_SPECULAR |  |
| OPTIX_DENOISER_AOV_TYPE_REFLECTION |  |
| OPTIX_DENOISER_AOV_TYPE_REFRACTION |  |
| OPTIX_DENOISER_AOV_TYPE_DIFFUSE |  |

## 5.16.4.20 OptixDenoiserModelKind

enumOptixDenoiserModelKind

Model kind used by the denoiser.

See alsooptixDenoiserCreate

Enumerator

| OPTIX_DENOISER_MODEL_KIND_AOV | Built-in model for denoising single image. |
| --- | --- |
| OPTIX_DENOISER_MODEL_KIND_TEMPORAL_AOV | Built-in model for denoising image sequence,temporally stable. |
| OPTIX_DENOISER_MODEL_KIND_UPSCALE2X | Built-in model for denoising single image upscaling(supports AOVs). |
| OPTIX_DENOISER_MODEL_KIND_TEMPORAL_UPSCALE2X | Built-in model for denoising image sequenceupscaling,temporally stable(supports AOVs). |
| OPTIX_DENOISER_MODEL_KIND_LDR | Deprecated.Use OPTIX_DENOISER_MODEL_KIND_AOV.When used internally mapped toOPTIX_DENOISER_MODEL_KIND_AOV. |
| OPTIX_DENOISER_MODEL_KIND_HDR |  |
| OPTIX_DENOISER_MODEL_KIND_TEMPORAL | Deprecated.Use OPTIX_DENOISER_MODEL_KIND_TEMPORAL_AOV. |

## 5.16.4.21 OptixDeviceContextValidationMode

enumOptixDeviceContextValidationMode

Validation mode settings.

When enabled, certain device code utilities will be enabled to provide as good debug and error
checking facilities as possible.

See alsooptixDeviceContextCreate( )

Enumerator

OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_OFF
OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_ALL

---

## 5.16.4.22 OptixDeviceProperty

enumOptixDeviceProperty

Parameters used foroptixDeviceContextGetProperty( )

See alsooptixDeviceContextGetProperty( )

Enumerator

| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRACE_DEPTH | Maximum value for OptixPipelineLinkOptions::maxTraceDepth.sizeof(unsigned int) |
| --- | --- |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRAVERSABLE_GRAPH_DEPTH | Maximum value to pass into optixPipelineSetStackSize for parameter maxTraversableGraphDepth.sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_PRIMITIVES_PER_GAS | The maximum number of primitives (over all build inputs) as input to a single Geometry Acceleration Structure(GAS).sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCES_PER_IAS | The maximum number of instances (over all build inputs) as input to a single Instance Acceleration Structure(IAS).sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_RTCORE_VERSION | The RT core version supported by the device(0for no support,10 for version 1.0).sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCE_ID | The maximum value for OptixInstance::instanceId.sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_NUM_BITS_INSTANCE_VISIBILITY_MASK | The number of bits available for theOptixInstance::visibilityMask.Higher bits must be set to zero.sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_RECORDS_PER_GAS | The maximum number of instances that can be added to a single Instance AccelerationStructure(IAS).sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_OFFSET | The maximum summed value of OptixInstance::sbtOffset.Also the maximum summed value of sbt offsets of all ancestor instances of a GAS in a traversable graph.sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING | Returns a flag specifying capabilities of theoptixReorder()device function.See OptixDevicePropertyShaderExecutionReorderingFlags for documentation on the values that can be returned.sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_COOP_VEC | Returns a flag specifying whether cooperative vector support is enabled for this device.See OptixDevicePropertyCoopVecFlags for documentation on the values that can be returned.sizeof(unsigned int) |

---

Enumerator

| OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL | Returns a flag specifying support for cluster acceleration structure builds. See OptixDevicePropertyClusterAccelFlags for documentation on the values that can be returned. sizeof(unsigned int) |
| --- | --- |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_VERTICES | Returns a maximum unique vertices per cluster in a cluster acceleration structure (CLAS) build. sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_TRIANGLES | Returns a maximum triangles per cluster in a cluster acceleration structure (CLAS) build. sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_STRUCTURED_GRID_RESOLUTION | Returns a maximum resolution per cluster in a structured cluster acceleration (CLAS) structure build. sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_SBT_INDEX | Returns a maximum sbt index allowed in a cluster acceleration structure (CLAS) build. sizeof(unsigned int) |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTERS_PER_GAS | Returns the maximum number of clusters (CLAS) as input to a single Geometry Acceleration Structure (GAS). sizeof(unsigned int) |

## 5.16.4.23 OptixDevicePropertyClusterAccelFlags

enumOptixDevicePropertyClusterAccelFlags

Flags used to interpret the result ofoptixDeviceContextGetProperty( )and OPTIX_DEVICE_
PROPERTY_CLUSTER_ACCEL.

See alsooptixDeviceContextGetProperty( )

Enumerator

| OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_NONE | Cluster acceleration structure builds are not supported. |
| --- | --- |
| OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_STANDARD |  |

## 5.16.4.24 OptixDevicePropertyCoopVecFlags

enumOptixDevicePropertyCoopVecFlags

Flags used to interpret the result ofoptixDeviceContextGetProperty( )and OPTIX_DEVICE_
PROPERTY_COOP_VEC.

See alsooptixDeviceContextGetProperty( )

Enumerator

| OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_NONE | Any use of cooperative vector host APIs or device intrinsics will result in an error. |
| --- | --- |

---

Enumerator

| OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_STANDARD |  |
| --- | --- |

## 5.16.4.25 OptixDevicePropertyShaderExecutionReorderingFlags

enumOptixDevicePropertyShaderExecutionReorderingFlags

Flags used to interpret the result ofoptixDeviceContextGetProperty( )and OPTIX_DEVICE_
PROPERTY_SHADER_EXECUTION_REORDERING.

See alsooptixDeviceContextGetProperty( )

Enumerator

| OPTIX_DEVICE_PROPERTY_SHADER
EXECUTION_REORDERING_FLAG_NONE | optixReorder() acts as a no-op, and no thread reordering is performed. Note that it is still legal to call this device function; no errors will be generated. |
| --- | --- |
| OPTIX_DEVICE_PROPERTY_SHADER
EXECUTION_REORDERING_FLAG_STANDARD |  |

## 5.16.4.26 OptixExceptionCodes

enumOptixExceptionCodes

The following values are used to indicate which exception was thrown.

Enumerator

| OPTIX_EXCEPTION_CODE_STACK_OVERFLOW | Stack overflow of the continuation stack. no exception details. |
| --- | --- |
| OPTIX_EXCEPTION_CODE_TRACE_DEPTH_EXCEEDED | The trace depth is exceeded. no exception details. |

## 5.16.4.27 OptixExceptionFlags

enumOptixExceptionFlags

Exception flags.

See alsoOptixPipelineCompileOptions::exceptionFlags,OptixExceptionCodes

Enumerator

| OPTIX_EXCEPTION_FLAG_NONE | No exception are enabled. |
| --- | --- |

---

163

## Enumerator

| OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW | Enables exceptions check related to the continuation stack. This flag should be used when the application handles stack overflows in a user exception program as part of the normal flow of execution. For catching overflows during debugging and development, the device context validation mode should be used instead.See also OptixDeviceContextValidationMode |
| --- | --- |
| OPTIX_EXCEPTION_FLAG_TRACE_DEPTH | Enables exceptions check related to trace depth.This flag should be used when the application handles trace depth overflows in a user exception program as part of the normal flow of execution.For catching overflows during debugging and development,the device context validation mode should be used instead.See also OptixDeviceContextValidationMode |
| OPTIX_EXCEPTION_FLAG_USER | Enables user exceptions via optixThrowException().This flag must be specified for all modules in a pipeline if any module calls optixThrowException(). |

## 5.16.4.28 OptixGeometryFlags

enumOptixGeometryFlags

Flags used byOptixBuildInputTriangleArray::flags,OptixBuildInputSphereArray::flagsand
OptixBuildInputCustomPrimitiveArray::flags.

Enumerator

| OPTIX_GEOMETRY_FLAG_NONE | No flags set. |
| --- | --- |
| OPTIX_GEOMETRY_FLAG_DISABLEANYHIT | Disables the invocation of the anyhit program. Can be overridden by OPTIX_INSTANCE_FLAG_ENFORCE_ANYHIT and OPTIX_RAY_FLAG_ENFORCE_ANYHIT. |
| OPTIX_GEOMETRY_FLAG_REQUIRESINGLE_ANYHIT_CALL | If set, an intersection with the primitive will trigger one and only one invocation of the anyhit program. Otherwise,the anyhit program may be invoked more than once. |
| OPTIX_GEOMETRY_FLAG_DISABLETRIANGLE_FACE_CULLING | Prevent triangles from getting culled due to their orientation.Effectively ignores ray flagsOPTIX_RAY_FLAG_CULL_BACK_FACINGTRIANGLESandOPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES. |

## 5.16.4.29 OptixHitKind

enumOptixHitKind

Legacy type: A subset of the hit kinds for built-in primitive intersections. It is preferred to use

---

164

optixGetPrimitiveType( ), together withoptixIsFrontFaceHit( )oroptixIsBackFaceHit( ).

See alsooptixGetHitKind( )

Enumerator

| OPTIX_HIT_KIND_TRIANGLE_FRONT_FACE | Ray hit the triangle on the front face. |
| --- | --- |
| OPTIX_HIT_KIND_TRIANGLE_BACK_FACE | Ray hit the triangle on the back face. |

## 5.16.4.30 OptixIndicesFormat

enumOptixIndicesFormat

Format of indices used intOptixBuildInputTriangleArray::indexFormat.

Enumerator

| OPTIX_INDICES_FORMAT_NONE | No indices, this format must only be used in combination with triangle soups,i.e.,numIndexTriplets must be zero. |
| --- | --- |
| OPTIX_INDICES_FORMAT_UNSIGNED_BYTE3 | Three bytes. |
| OPTIX_INDICES_FORMAT_UNSIGNED_SHORT3 | Three shorts. |
| OPTIX_INDICES_FORMAT_UNSIGNED_INT3 | Three ints. |

## 5.16.4.31 OptixInstanceFlags

enumOptixInstanceFlags

Flags set on theOptixInstance::flags.

These can be or'ed together to combine multiple flags.

Enumerator

| OPTIX_INSTANCE_FLAG_NONE | No special flag set. |
| --- | --- |
| OPTIX_INSTANCE_FLAG_DISABLE_TRIANGLE_FACE_CULLING | Prevent triangles from getting culled due to their orientation. Effectively ignores ray flags OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES and OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES. |
| OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING | Flip triangle orientation. This affects front/backface culling as well as the reported face in case of a hit. |
| OPTIX_INSTANCE_FLAG_DISABLE_ANYHIT | Disable anyhit programs for all geometries of the instance. Can be overridden by OPTIX_RAY_FLAG_ENFORCE_ANYHIT. This flag is mutually exclusive with OPTIX_INSTANCE_FLAG_ENFORCE_ANYHIT. |

---

165

Enumerator

| OPTIX_INSTANCE_FLAG_ENFORCEANYHIT | Enables anyhit programs for all geometries of the instance. Overrides OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT Can be overridden by OPTIX_RAY_FLAG_DISABLE_ANYHIT. This flag is mutually exclusive with OPTIX_INSTANCE_FLAG_DISABLE_ANYHIT. |
| --- | --- |
| OPTIX_INSTANCE_FLAG_FORCE_OPACITYMICROMAP2_STATE | Force 4-state opacity micromaps to behave as 2-state opacity micromaps during traversal. |
| OPTIX_INSTANCE_FLAG_DISABLEOPACITYMICROMAPS | Don't perform opacity micromap query for this instance.Triangle GAS must be built with ALLOW_DISABLE_OPACITY_MICROMAPS for this to be valid.Clusters in a GAS must be build with OPTIX_CLUSTER_ACCELCLUSTER_FLAG_ALLOW_DISABLEOPACITY_MICROMAPS for this to be valid.This flag overrides FORCE_OPACTIYMIXROMAP2_STATE instance and ray flags. |

## 5.16.4.32 OptixModuleCompileState

enumOptixModuleCompileState

Module compilation state.

See alsooptixModuleGetCompilationState( ),optixModuleCreateWithTasks( )

Enumerator

| OPTIX_MODULE_COMPILE_STATE_NOT_STARTED | No OptixTask objects have started. |
| --- | --- |
| OPTIX_MODULE_COMPILE_STATE_STARTED | Started, but not all OptixTask objects have completed. No detected failures. |
| OPTIX_MODULE_COMPILE_STATE_IMPENDING_FAILURE | Not all OptixTask objects have completed, but at least one has failed. |
| OPTIX_MODULE_COMPILE_STATE_FAILED | All OptixTask objects have completed, and at least one has failed. |
| OPTIX_MODULE_COMPILE_STATE_COMPLETED | All OptixTask objects have completed.The OptixModule is ready to be used. |

## 5.16.4.33 OptixMotionFlags

enumOptixMotionFlags

Enum to specify motion flags.

See alsoOptixMotionOptions::flags.

Enumerator

| OPTIX_MOTION_FLAG_NONE |
| --- |
| OPTIX_MOTION_FLAG_START_VANISH |

---

166

Enumerator

<u>OPTIX_MOTION_FLAG_END_VANISH</u>

## 5.16.4.34 OptixOpacityMicromapArrayIndexingMode

enumOptixOpacityMicromapArrayIndexingMode

indexing mode of triangles to opacity micromaps in an array, used in
OptixBuildInputOpacityMicromap.

Enumerator

| OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_NONE | No opacity micromap is used. |
| --- | --- |
| OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_LINEAR | An implicit linear mapping of triangles to opacity micromaps in the opacity micromap array is used. triangle[i] will use opacityMicromapArray[i]. |
| OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_INDEXED | OptixBuildInputOpacityMicromap::indexBuffer provides a per triangle array of predefined indices and/or indices into OptixBuildInputOpacityMicromap ::opacityMicromapArray.See OptixBuildInputOpacityMicromap::indexBuffer for more details. |

## 5.16.4.35 OptixOpacityMicromapFlags

enumOptixOpacityMicromapFlags

Flags defining behavior of opacity micromaps in a opacity micromap array.

Enumerator

| OPTIX_OPACITY_MICROMAP_FLAG_NONE |  |
| --- | --- |
| OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_TRACE | This flag is mutually exclusive with OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_BUILD. |
| OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_BUILD | This flag is mutually exclusive with OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_TRACE. |

## 5.16.4.36 OptixOpacityMicromapFormat

enumOptixOpacityMicromapFormat

Specifies whether to use a 2- or 4-state opacity micromap format.

Enumerator

| OPTIX_OPACITY_MICROMAP_FORMAT_NONE | invalid format |
| --- | --- |

---

167

Enumerator

| OPTIX_OPACITY_MICROMAP_FORMAT_2_STATE | 0: Transparent, 1: Opaque |
| --- | --- |
| OPTIX_OPACITY_MICROMAP_FORMAT_4_STATE | 0: Transparent, 1: Opaque, 2: Unknown-Transparent, 3: Unknown-Opaque |

## 5.16.4.37 OptixPayloadSemantics

enumOptixPayloadSemantics

Semantic flags for a single payload word.

Used to specify the semantics of a payload word per shader type. "read": Shader of this type may read
the payload word. "write": Shader of this type may write the payload word.

"trace_caller_write": Shaders may consume the value of the payload word passed to optixTrace by the
caller. "trace_caller_read": The caller to optixTrace may read the payload word after the call to
optixTrace.

Semantics can be bitwise combined. Combining "read" and "write" is equivalent to specifying "read_
write". A payload needs to be writable by the caller or at least one shader type. A payload needs to be
readable by the caller or at least one shader type after a being writable.

Enumerator

| OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_NONE |
| --- |
| OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ |
| OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_CH_NONE |
| OPTIX_PAYLOAD_SEMANTICS_CH_READ |
| OPTIX_PAYLOAD_SEMANTICS_CH_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_CH_READ_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_MS_NONE |
| OPTIX_PAYLOAD_SEMANTICS_MS_READ |
| OPTIX_PAYLOAD_SEMANTICS_MS_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_MS_READ_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_AH_NONE |
| OPTIX_PAYLOAD_SEMANTICS_AH_READ |
| OPTIX_PAYLOAD_SEMANTICS_AH_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_AH_READ_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_IS_NONE |
| OPTIX_PAYLOAD_SEMANTICS_IS_READ |
| OPTIX_PAYLOAD_SEMANTICS_IS_WRITE |
| OPTIX_PAYLOAD_SEMANTICS_IS_READ_WRITE |

---

168

## 5.16.4.38 OptixPayloadTypeID

enumOptixPayloadTypeID

Payload type identifiers.

Enumerator

| OPTIX_PAYLOAD_TYPE_DEFAULT |  |
| --- | --- |
| OPTIX_PAYLOAD_TYPE_ID_0 |  |
| OPTIX_PAYLOAD_TYPE_ID_1 |  |
| OPTIX_PAYLOAD_TYPE_ID_2 |  |
| OPTIX_PAYLOAD_TYPE_ID_3 |  |
| OPTIX_PAYLOAD_TYPE_ID_4 |  |
| OPTIX_PAYLOAD_TYPE_ID_5 |  |
| OPTIX_PAYLOAD_TYPE_ID_6 |  |
| OPTIX_PAYLOAD_TYPE_ID_7 |  |

## 5.16.4.39 OptixPipelineSymbolMemcpyKind

enumOptixPipelineSymbolMemcpyKind

Flags used to interpret the source and target of memory copies when using
optixPipelineSymbolMemcpyAsync( )

See alsooptixPipelineSymbolMemcpyAsync( )

Enumerator

OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_DEVICE
OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_HOST
OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_DEVICE
OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_HOST

## 5.16.4.40 OptixPixelFormat

enumOptixPixelFormat

Pixel formats used by the denoiser.

See alsoOptixImage2D::format

Enumerator

| OPTIX_PIXEL_FORMAT_HALF1 | one half |
| --- | --- |
| OPTIX_PIXEL_FORMAT_HALF2 | two halfs,XY |
| OPTIX_PIXEL_FORMAT_HALF3 | three halfs,RGB |
| OPTIX_PIXEL_FORMAT_HALF4 | four halfs,RGBA |
| OPTIX_PIXEL_FORMAT_FLOAT1 | one float |
| OPTIX_PIXEL_FORMAT_FLOAT2 | two floats,XY |

---

169

Enumerator

| OPTIX_PIXEL_FORMAT_FLOAT3 | three floats，RGB |
| --- | --- |
| OPTIX_PIXEL_FORMAT_FLOAT4 | four floats，RGBA |
| OPTIX_PIXEL_FORMAT_UCHAR3 | three unsigned chars，RGB |
| OPTIX_PIXEL_FORMAT_UCHAR4 | four unsigned chars，RGBA |
| OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_LAYER | internal format |

## 5.16.4.41 OptixPrimitiveType

enumOptixPrimitiveType

Builtin primitive types.

Enumerator

| OPTIX_PRIMITIVE_TYPE_CUSTOM | Custom primitive. |
| --- | --- |
| OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE | B-spline curve of degree 2 with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE | B-spline curve of degree 3 with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_ROUND_LINEAR | Piecewise linear curve with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM | CatmullRom curve with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_FLAT_QUADRATIC_BSPLINE | B-spline curve of degree 2 with oriented, flat cross-section. |
| OPTIX_PRIMITIVE_TYPE_SPHERE | Sphere. |
| OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER | Bezier curve of degree 3 with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE_ROCAPS | B-spline curve of degree 2 with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE_ROCAPS | B-spline curve of degree 3 with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM_ROCAPS | CatmullRom curve with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER_ROCAPS | Bezier curve of degree 3 with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_TRIANGLE | Triangle. |

## 5.16.4.42 OptixPrimitiveTypeFlags

enumOptixPrimitiveTypeFlags

Builtin flags may be bitwise combined.

See alsoOptixPipelineCompileOptions::usesPrimitiveTypeFlags

---

170

Enumerator

| OPTIX_PRIMITIVE_TYPE_FLAGS_CUSTOM | Custom primitive. |
| --- | --- |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE | B-spline curve of degree 2 with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE | B-spline curve of degree 3 with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_LINEAR | Piecewise linear curve with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM | CatmullRom curve with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_FLAT_QUADRATIC_BSPLINE | B-spline curve of degree 2 with oriented, flat cross-section. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_SPHERE | Sphere. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER | Bezier curve of degree 3 with circular cross-section. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE_ROCAPS | B-spline curve of degree 2 with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE_ROCAPS | B-spline curve of degree 3 with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM_ROCAPS | CatmullRom curve with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER_ROCAPS | Bezier curve of degree 3 with circular cross-section, using rocaps intersection. |
| OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE | Triangle. |

## 5.16.4.43 OptixProgramGroupFlags

enumOptixProgramGroupFlags

Flags for program groups.

## Enumerator

| OPTIX_PROGRAM_GROUP_FLAGS_NONE | Currently there are no flags. |
| --- | --- |

## 5.16.4.44 OptixProgramGroupKind

enumOptixProgramGroupKind

Distinguishes different kinds of program groups.

Enumerator

| OPTIX_PROGRAM_GROUP_KIND_RAYGEN | Program group containing a raygen (RG) program. See alsoOptixProgramGroupSingleModule,OptixProgramGroupDesc:raygen |
| --- | --- |

---

171

Enumerator

| OPTIX_PROGRAM_GROUP_KIND_MISS | Program group containing a miss (MS) program. See also OptixProgramGroupSingleModule, OptixProgramGroupDesc::miss |
| --- | --- |
| OPTIX_PROGRAM_GROUP_KIND_EXCEPTION | Program group containing an exception (EX) program. See also OptixProgramGroupHitgroup, OptixProgramGroupDesc::exception |
| OPTIX_PROGRAM_GROUP_KIND_HITGROUP | Program group containing an intersection(IS), any hit(AH)，and/or closest hit(CH)program.See alsoOptixProgramGroupSingleModule，OptixProgramGroupDesc::hitgroup |
| OPTIX_PROGRAM_GROUP_KIND_CALLABLES | Program group containing a direct(DC)or continuation(CC)callable program.See alsoOptixProgramGroupCallables，OptixProgramGroupDesc::callables |

## 5.16.4.45 OptixQueryFunctionTableOptions

enumOptixQueryFunctionTableOptions

Options that can be passed to optixQueryFunctionTable( )

## Enumerator

OPTIX_QUERY_FUNCTION_TABLE_OPTION_DUMMY Placeholder (there are no options yet)

## 5.16.4.46 OptixRayFlags

enumOptixRayFlags

Ray flags passed to the device functionoptixTrace( ). These affect the behavior of traversal per
invocation.

See alsooptixTrace( )

Enumerator

| OPTIX_RAY_FLAG_NONE | No change from the behavior configured for the individual AS. |
| --- | --- |
| OPTIX_RAY_FLAG_DISABLE_ANYHIT | Disables anyhit programs for the ray. Overrides OPTIX_INSTANCE_FLAG_ENFORCE_ANYHIT. This flag is mutually exclusive with OPTIX_RAY_FLAG_ENFORCE_ANYHIT, OPTIX_RAY_FLAG_CULL_DISABLE_ANYHIT, OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT. |

---

172

Enumerator

| OPTIX_RAY_FLAG_ENFORCE_ANYHIT | Forces anyhit program execution for the ray. Overrides OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT as well as OPTIX_INSTANCE_FLAG_DISABLE_ANYHIT. This flag is mutually exclusive with OPTIX_RAY_FLAG_DISABLE_ANYHIT, OPTIX_RAY_FLAG_CULL_DISABLE_ANYHIT, OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT. |
| --- | --- |
| OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT | Terminates the ray after the first hit and executes the closesthit program of that hit. |
| OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT | Disables closesthit programs for the ray, but still executes miss program in case of a miss. |
| OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES | Do not intersect triangle back faces (respects a possible face change due to instance flag OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING). This flag is mutually exclusive with OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES. |
| OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES | Do not intersect triangle front faces (respects a possible face change due to instance flag OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING). This flag is mutually exclusive with OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES. |
| OPTIX_RAY_FLAG_CULL_DISABLED_ANYHIT | Do not intersect geometry which disables anyhit programs (due to setting geometry flag OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT or instance flag OPTIX_INSTANCE_FLAG_DISABLE_ANYHIT). This flag is mutually exclusive with OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT, OPTIX_RAY_FLAG_ENFORCE_ANYHIT, OPTIX_RAY_FLAG_DISABLE_ANYHIT. |
| OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT | Do not intersect geometry which have an enabled anyhit program (due to not setting geometry flag OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT or setting instance flag OPTIX_INSTANCE_FLAG_ENFORCE_ANYHIT). This flag is mutually exclusive with OPTIX_RAY_FLAG_CULL_DISABLE_ANYHIT, OPTIX_RAY_FLAG_ENFORCE_ANYHIT, OPTIX_RAY_FLAG_DISABLE_ANYHIT. |
| OPTIX_RAY_FLAG_FORCE_OPACITY_MICROMAP_2_STATE | Force 4-state opacity micromaps to behave as 2-state opacity micromaps during traversal. |

## 5.16.4.47 OptixResult

enumOptixResult

---

<u>173</u>

Result codes returned from API functions.

All host side API functions return OptixResult with the exception of optixGetErrorName and
optixGetErrorString. When successful OPTIX_SUCCESS is returned. All return codes except for OPTIX
_SUCCESS should be assumed to be errors as opposed to a warning.

See alsooptixGetErrorName( ),optixGetErrorString( )

Enumerator

| OPTIX_SUCCESS |
| --- |
| OPTIX_ERROR_INVALID_VALUE |
| OPTIX_ERROR_HOST_OUT_OF_MEMORY |
| OPTIX_ERROR_INVALID_OPERATION |
| OPTIX_ERROR_FILE_IO_ERROR |
| OPTIX_ERROR_INVALID_FILE_FORMAT |
| OPTIX_ERROR_DISK_CACHE_INVALID_PATH |
| OPTIX_ERROR_DISK_CACHE_PERMISSION_ERROR |
| OPTIX_ERROR_DISK_CACHE_DATABASE_ERROR |
| OPTIX_ERROR_DISK_CACHE_INVALID_DATA |
| OPTIX_ERROR_LAUNCH_FAILURE |
| OPTIX_ERROR_INVALID_DEVICE_CONTEXT |
| OPTIX_ERROR_CUDA_NOT_INITIALIZED |
| OPTIX_ERROR_VALIDATION_FAILURE |
| OPTIX_ERROR_INVALID_INPUT |
| OPTIX_ERROR_INVALID_LAUNCH_PARAMETER |
| OPTIX_ERROR_INVALID_PAYLOAD_ACCESS |
| OPTIX_ERROR_INVALID_ATTRIBUTE_ACCESS |
| OPTIX_ERROR_INVALID_FUNCTION_USE |
| OPTIX_ERROR_INVALID_FUNCTION_ARGUMENTS |
| OPTIX_ERROR_PIPELINE_OUT_OF_CONSTANT_MEMORY |
| OPTIX_ERROR_PIPELINE_LINK_ERROR |
| OPTIX_ERROR_ILLEGAL_DURING_TASK_EXECUTE |
| OPTIX_ERROR_CREATION_CANCELED |
| OPTIX_ERROR_INTERNAL_COMPILER_ERROR |
| OPTIX_ERROR_DENOISER_MODEL_NOT_SET |
| OPTIX_ERROR_DENOISER_NOT_INITIALIZED |
| OPTIX_ERROR_NOT_COMPATIBLE |
| OPTIX_ERROR_PAYLOAD_TYPE_MISMATCH |
| OPTIX_ERROR_PAYLOAD_TYPE_RESOLUTION_FAILED |
| OPTIX ERROR_PAYLOAD_TYPE_ID_INVALID |
| OPTIX ERROR_NOT_SUPPORTED |
| OPTIX ERROR_UNSUPPORTED_ABI_VERSION |

---

174

Enumerator

| OPTIX_ERROR_FUNCTION_TABLE_SIZE_MISMATCH |
| --- |
| OPTIX_ERROR_INVALID_ENTRY_FUNCTION_OPTIONS |
| OPTIX_ERROR_LIBRARY_NOT_FOUND |
| OPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND |
| OPTIX_ERROR_LIBRARY_UNLOAD_FAILURE |
| OPTIX_ERROR_DEVICE_OUT_OF_MEMORY |
| OPTIX_ERROR_INVALID_POINTER |
| OPTIX_ERROR_SYMBOL_NOT_FOUND |
| OPTIX_ERROR_CUDA_ERROR |
| OPTIX_ERROR_INTERNAL_ERROR |
| OPTIX_ERROR_UNKNOWN |

## 5.16.4.48 OptixTransformFormat

enumOptixTransformFormat

Format of transform used inOptixBuildInputTriangleArray::transformFormat.

Enumerator

| OPTIX_TRANSFORM_FORMAT_NONE | no transform, default for zero initialization |
| --- | --- |
| OPTIX_TRANSFORM_FORMAT_MATRIX_FLOAT12 | 3x4 row major affine matrix |

## 5.16.4.49 OptixTransformType

enumOptixTransformType

Transform.

OptixTransformType is used by the device functionoptixGetTransformTypeFromHandle( )to
determine the type of the OptixTraversableHandle returned fromoptixGetTransformListHandle( ).

Enumerator

| OPTIX_TRANSFORM_TYPE_NONE | Not a transformation. |
| --- | --- |
| OPTIX_TRANSFORM_TYPE_STATIC_TRANSFORM | See also OptixStaticTransform |
| OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_TRANSFORM | See also OptixMatrixMotionTransform |
| OPTIX_TRANSFORM_TYPE_SRT_MOTION_TRANSFORM | See also OptixSRTMotionTransform |
| OPTIX_TRANSFORM_TYPE_INSTANCE | See also OptixInstance |

## 5.16.4.50 OptixTraversableGraphFlags

enumOptixTraversableGraphFlags

Specifies the set of valid traversable graphs that may be passed to invocation ofoptixTrace( ). Flags

---

175

may be bitwise combined.

| OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_ANY | Used to signal that any traversable graphs is valid. This flag is mutually exclusive with all other flags. |
| --- | --- |
| OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS | Used to signal that a traversable graph of a single Geometry Acceleration Structure (GAS) without any transforms is valid. This flag may be combined with other flags except for OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_ANY. |
| OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING | Used to signal that a traversable graph of a single Instance Acceleration Structure (IAS) directly connected to Geometry Acceleration Structure (GAS) traversables without transform traversables in between is valid. This flag may be combined with other flags except for OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_ANY. |

## 5.16.4.51 OptixTraversableType

enumOptixTraversableType

Traversable Handles.

See alsooptixConvertPointerToTraversableHandle( )

Enumerator

| OPTIX_TRAVERSABLE_TYPE_STATIC_TRANSFORM | Static transforms. See also OptixStaticTransform |
| --- | --- |
| OPTIX_TRAVERSABLE_TYPE_MATRIX_MOTION_TRANSFORM | Matrix motion transform. See alsoOptixMatrixMotionTransform |
| OPTIX_TRAVERSABLE_TYPE_SRT_MOTION_TRANSFORM | SRT motion transform. See alsoOptixSRTMotionTransform |

## 5.16.4.52 OptixVertexFormat

enumOptixVertexFormat

Format of vertices used inOptixBuildInputTriangleArray::vertexFormat.

Enumerator

| OPTIX_VERTEX_FORMAT_NONE | No vertices. |
| --- | --- |
| OPTIX_VERTEX_FORMAT_FLOAT3 | Vertices are represented by three floats. |
| OPTIX_VERTEX_FORMAT_FLOAT2 | Vertices are represented by two floats. |
| OPTIX_VERTEX_FORMAT_HALF3 | Vertices are represented by three halfs. |
| OPTIX_VERTEX_FORMAT_HALF2 | Vertices are represented by two halfs. |

---

176

## Enumerator

| OPTIX_VERTEX_FORMAT_SNORM16_3 |  |
| --- | --- |
| OPTIX_VERTEX_FORMAT_SNORM16_2 |  |

## 6 Namespace Documentation

## 6.1 optix_impl Namespace Reference

## Functions

•static __forceinline__ __device__ float4optixAddFloat4(const float4 &a, const float4 &b)

•static __forceinline__ __device__ float4optixMulFloat4(const float4 &a, float b)

•static __forceinline__ __device__ uint4optixLdg(unsigned long long addr)
< >

•template class T
static __forceinline__ __device__ ToptixLoadReadOnlyAlign16(const T *∗*ptr)

•static __forceinline__ __device__ float4optixMultiplyRowMatrix(const float4 vec, const float4
m0, const float4 m1, const float4 m2)

•static __forceinline__ __device__ voidoptixGetMatrixFromSrt(float4 &m0, float4 &m1, float4
&m2, constOptixSRTData&srt)

•static __forceinline__ __device__ voidoptixInvertMatrix(float4 &m0, float4 &m1, float4 &m2)

•static __forceinline__ __device__ voidoptixLoadInterpolatedMatrixKey(float4 &m0, float4 &m1,
float4 &m2, const float4 *∗*matrix, const float t1)

•static __forceinline__ __device__ voidoptixLoadInterpolatedSrtKey(float4 &srt0, float4 &srt1,
float4 &srt2, float4 &srt3, const float4 *∗*srt, const float t1)

•static __forceinline__ __device__ voidoptixResolveMotionKey(float &localt, int &key, const
OptixMotionOptions&options, const float globalt)

•static __forceinline__ __device__ voidoptixGetInterpolatedTransformation(float4 &trf0, float4
&trf1, float4 &trf2, constOptixMatrixMotionTransform *∗*transformData, const float time)

•static __forceinline__ __device__ voidoptixGetInterpolatedTransformation(float4 &trf0, float4
&trf1, float4 &trf2, constOptixSRTMotionTransform *∗*transformData, const float time)

•static __forceinline__ __device__ voidoptixGetInterpolatedTransformationFromHandle(float4
&trf0, float4 &trf1, float4 &trf2, constOptixTraversableHandlehandle, const float time, const bool
objectToWorld)

•template<typename HitState >
static __forceinline__ __device__ voidoptixGetWorldToObjectTransformMatrix(const HitState
&hs, float4 &m0, float4 &m1, float4 &m2)

•template<typename HitState >
static __forceinline__ __device__ voidoptixGetObjectToWorldTransformMatrix(const HitState
&hs, float4 &m0, float4 &m1, float4 &m2)

•static __forceinline__ __device__ float3optixTransformPoint(const float4 &m0, const float4 &m1,
const float4 &m2, const float3 &p)

•static __forceinline__ __device__ float3optixTransformVector(const float4 &m0, const float4
&m1, const float4 &m2, const float3 &v)

•static __forceinline__ __device__ float3optixTransformNormal(const float4 &m0, const float4
&m1, const float4 &m2, const float3 &n)

•OPTIX_MICROMAP_INLINE_FUNCfloat__uint_as_float(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCunsigned intextractEvenBits(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCunsigned intprefixEor(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCvoidindex2dbary(unsigned int index, unsigned int &u,
unsigned int &v, unsigned int &w)

---

• OPTIX_MICROMAP_INLINE_FUNC void micro2bary (unsigned int index, unsigned int subdivisionLevel, float2 &bary0, float2 &bary1, float2 &bary2)
• OPTIX_MICROMAP_INLINE_FUNC float2 base2micro (const float2 &baseBarycentrics, const float2 microVertexBaseBarycentrics[3])

## 6.1.1 Function Documentation

## 6.1.1.1 optixAddFloat4()

static __forceinline__ __device__ float4 optix_impl::optixAddFloat4 (
    const float4 & a,
    const float4 & b ) [static]

## 6.1.1.2 optixGetInterpolatedTransformation() [1/2]

static __forceinline__ __device__ void optix_impl
## ::optixGetInterpolatedTransformation (
    float4 & trf0,
    float4 & trf1,
    float4 & trf2,
    const OptixMatrixMotionTransform * transformData,
    const float time ) [static]

6.1.1.3 optixGetInterpolatedTransformation() [2/2]

static __forceinline__ __device__ void optix_impl
## ::optixGetInterpolatedTransformation (
    float4 & trf0,
    float4 & trf1,
    float4 & trf2,
    const OptixSRTMotionTransform * transformData,
    const float time ) [static]

6.1.1.4 optixGetInterpolatedTransformationFromHandle()

static __forceinline__ __device__ void optix_impl
::optixGetInterpolatedTransformationFromHandle (
    float4 & trf0,
    float4 & trf1,
    float4 & trf2,
    const OptixTraversableHandle handle,
    const float time,
    const bool objectToWorld ) [static]

## 6.1.1.5 optixGetMatrixFromSrt()

static __forceinline__ __device__ void optix_impl::optixGetMatrixFromSrt (
    float4 & m0,

---

<u>178</u>

float4 & *m1,*
float4 & *m2,*
constOptixSRTData& *srt*) *[static]*

## 6.1.1.6 optixGetObjectToWorldTransformMatrix( )

template<typename HitState >
static __forceinline__ __device__ void optix_impl
::optixGetObjectToWorldTransformMatrix (
const HitState & *hs,*
float4 & *m0,*
float4 & *m1,*
float4 & *m2*) *[static]*

## 6.1.1.7 optixGetWorldToObjectTransformMatrix( )

template<typename HitState >
static __forceinline__ __device__ void optix_impl
::optixGetWorldToObjectTransformMatrix (
const HitState & *hs,*
float4 & *m0,*
float4 & *m1,*
float4 & *m2*) *[static]*
6.1.1.8 optixInvertMatrix( )
static __forceinline__ __device__ void optix_impl::optixInvertMatrix (
float4 & *m0,*
float4 & *m1,*
float4 & *m2*) *[static]*
6.1.1.9 optixLdg( )
static __forceinline__ __device__ uint4 optix_impl::optixLdg (
unsigned long long *addr*) *[static]*
6.1.1.10 optixLoadInterpolatedMatrixKey( )
static __forceinline__ __device__ void optix_impl
::optixLoadInterpolatedMatrixKey (
float4 & *m0,*
float4 & *m1,*
float4 & *m2,*
const float4 *∗ matrix,*
const float *t1*) *[static]*

---

<u>179</u>

## 6.1.1.11 optixLoadInterpolatedSrtKey( )

static __forceinline__ __device__ void optix_impl
::optixLoadInterpolatedSrtKey (
float4 & *srt0,*
float4 & *srt1,*
float4 & *srt2,*
float4 & *srt3,*
const float4 *∗ srt,*
const float *t1*) *[static]*
6.1.1.12 optixLoadReadOnlyAlign16( )
template<class T >
static __forceinline__ __device__ T optix_impl::optixLoadReadOnlyAlign16 (
const T *∗ ptr*) *[static]*
6.1.1.13 optixMulFloat4( )
static __forceinline__ __device__ float4 optix_impl::optixMulFloat4 (
const float4 & *a,*
float *b*) *[static]*
6.1.1.14 optixMultiplyRowMatrix( )
static __forceinline__ __device__ float4 optix_impl::optixMultiplyRowMatrix
(
const float4 *vec,*
const float4 *m0,*
const float4 *m1,*
const float4 *m2*) *[static]*
6.1.1.15 optixResolveMotionKey( )
static __forceinline__ __device__ void optix_impl::optixResolveMotionKey (
float & *localt,*
int & *key,*
constOptixMotionOptions& *options,*
const float *globalt*) *[static]*
6.1.1.16 optixTransformNormal( )
static __forceinline__ __device__ float3 optix_impl::optixTransformNormal (
const float4 & *m0,*
const float4 & *m1,*
const float4 & *m2,*
const float3 & *n*) *[static]*

---

## 6.1.1.17 optixTransformPoint()

static __forceinline__ __device__ float3 optix_impl::optixTransformPoint (
    const float4 & m0,
    const float4 & m1,
    const float4 & m2,
    const float3 & p ) [static]

6.1.1.18 optixTransformVector()

static __forceinline__ __device__ float3 optix_impl::optixTransformVector (
    const float4 & m0,
    const float4 & m1,
    const float4 & m2,
    const float3 & v ) [static]

## 6.2 optix_internal Namespace Reference

Classes
• struct TypePack

## 7 Class Documentation

## 7.1 OptixAabb Struct Reference
#include <optix_types.h>

## Public Attributes
• float minX
• float minY
• float minZ
• float maxX
• float maxY
• float maxZ

## 7.1.1 Detailed Description
AABB inputs.

## 7.1.2 Member Data Documentation

## 7.1.2.1 maxX
float OptixAabb::maxX
Upper extent in X direction.

## 7.1.2.2 maxY
float OptixAabb::maxY
Upper extent in Y direction.

---

<u>181</u>

7.1.2.3 maxZ

float OptixAabb::maxZ

Upper extent in Z direction.

7.1.2.4 minX

float OptixAabb::minX

Lower extent in X direction.

7.1.2.5 minY

float OptixAabb::minY

Lower extent in Y direction.

7.1.2.6 minZ

float OptixAabb::minZ

Lower extent in Z direction.

## 7.2 OptixAccelBufferSizes Struct Reference

## Public Attributes

#include <optix_types.h>

•size_toutputSizeInBytes

•size_ttempUpdateSizeInBytes

•size_ttempSizeInBytes

## 7.2.1 Detailed Description

Struct for querying builder allocation requirements.

Once queried the sizes should be used to allocate device memory of at least these sizes.

See alsooptixAccelComputeMemoryUsage( )

## 7.2.2 Member Data Documentation

7.2.2.1 outputSizeInBytes

size_t OptixAccelBufferSizes::outputSizeInBytes

The size in bytes required for the outputBuffer parameter to optixAccelBuild when doing a build
(OPTIX_BUILD_OPERATION_BUILD).

7.2.2.2 tempSizeInBytes

size_t OptixAccelBufferSizes::tempSizeInBytes

The size in bytes required for the tempBuffer paramter to optixAccelBuild when doing a build (OPTIX_
BUILD_OPERATION_BUILD).

7.2.2.3 tempUpdateSizeInBytes

size_t OptixAccelBufferSizes::tempUpdateSizeInBytes

---

<u>182</u>

The size in bytes required for the tempBuffer parameter to optixAccelBuild when doing an update
(OPTIX_BUILD_OPERATION_UPDATE). This value can be different than tempSizeInBytes used for a
full build. Only non-zero if OPTIX_BUILD_FLAG_ALLOW_UPDATE flag is set in
OptixAccelBuildOptions.

## 7.3 OptixAccelBuildOptions Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intbuildFlags

•OptixBuildOperationoperation

•OptixMotionOptionsmotionOptions

## 7.3.1 Detailed Description

Build options for acceleration structures.

See alsooptixAccelComputeMemoryUsage( ),optixAccelBuild( )

## 7.3.2 Member Data Documentation

7.3.2.1 buildFlags

unsigned int OptixAccelBuildOptions::buildFlags

Combinations of OptixBuildFlags.

7.3.2.2 motionOptions

OptixMotionOptionsOptixAccelBuildOptions::motionOptions

7.3.2.3 operation

Options for motion.

OptixBuildOperationOptixAccelBuildOptions::operation

If OPTIX_BUILD_OPERATION_UPDATE the output buffer is assumed to contain the result of a full
build with OPTIX_BUILD_FLAG_ALLOW_UPDATE set and using the same number of primitives. It
is updated incrementally to reflect the current position of the primitives. If a BLAS has been built with
OPTIX_BUILD_FLAG_ALLOW_OPACITY_MICROMAP_UPDATE, new opacity micromap arrays and
opacity micromap indices may be provided to the refit.

## 7.4 OptixAccelEmitDesc Struct Reference

#include <optix_types.h>

## Public Attributes

•CUdeviceptrresult

•OptixAccelPropertyTypetype

## 7.4.1 Detailed Description

Specifies a type and output destination for emitted post-build properties.

See alsooptixAccelBuild( )

---

<u>183</u>

## 7.4.2 Member Data Documentation

7.4.2.1 result

CUdeviceptrOptixAccelEmitDesc::result

Output buffer for the properties.

7.4.2.2 type

OptixAccelPropertyTypeOptixAccelEmitDesc::type

Requested property.

## 7.5 OptixBuildInput Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixBuildInputTypetype

•union {

charpad[1024]

OptixBuildInputTriangleArraytriangleArray

OptixBuildInputCurveArraycurveArray

OptixBuildInputSphereArraysphereArray

OptixBuildInputCustomPrimitiveArraycustomPrimitiveArray

OptixBuildInputInstanceArrayinstanceArray

};

## 7.5.1 Detailed Description

Build inputs.

All of them support motion and the size of the data arrays needs to match the number of motion steps
See alsooptixAccelComputeMemoryUsage( ),optixAccelBuild( )

## 7.5.2 Member Data Documentation

## 7.5.2.1

union {...} OptixBuildInput::@1

7.5.2.2 curveArray

OptixBuildInputCurveArrayOptixBuildInput::curveArray

Curve inputs.

7.5.2.3 customPrimitiveArray

OptixBuildInputCustomPrimitiveArrayOptixBuildInput::customPrimitiveArray
Custom primitive inputs.

7.5.2.4 instanceArray

OptixBuildInputInstanceArrayOptixBuildInput::instanceArray

---

<u>184</u>

Instance and instance pointer inputs.

7.5.2.5 pad

char OptixBuildInput::pad[1024]

7.5.2.6 sphereArray

OptixBuildInputSphereArrayOptixBuildInput::sphereArray

Sphere inputs.

7.5.2.7 triangleArray

OptixBuildInputTriangleArrayOptixBuildInput::triangleArray
Triangle inputs.

7.5.2.8 type

OptixBuildInputTypeOptixBuildInput::type

The type of the build input.

## 7.6 OptixBuildInputCurveArray Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixPrimitiveTypecurveType

•unsigned intnumPrimitives

•constCUdeviceptr *∗* vertexBuffers

•unsigned intnumVertices

•unsigned intvertexStrideInBytes

•constCUdeviceptr *∗* widthBuffers

•unsigned intwidthStrideInBytes

•constCUdeviceptr *∗* normalBuffers

•unsigned intnormalStrideInBytes

•CUdeviceptrindexBuffer

•unsigned intindexStrideInBytes

•unsigned intflag

•unsigned intprimitiveIndexOffset

•unsigned intendcapFlags

## 7.6.1 Detailed Description

Curve inputs.

A curve is a swept surface defined by a 3D spline curve and a varying width (radius). A curve (or
"strand") of degree d (3=cubic, 2=quadratic, 1=linear) is represented by N *>* d vertices and N width
values, and comprises N - d segments. Each segment is defined by d+1 consecutive vertices. Each
curve may have a different number of vertices.

OptiX describes the curve array as a list of curve segments. The primitive id is the segment number. It
is the user's responsibility to maintain a mapping between curves and curve segments. Each index
buffer entry i = indexBuffer[primid] specifies the start of a curve segment, represented by d+1

---

<u>7.6</u>

<u>185</u>

consecutive vertices in the vertex buffer, and d+1 consecutive widths in the width buffer. Width is
interpolated the same way vertices are interpolated, that is, using the curve basis.

Each curves build input has only one SBT record. To create curves with different materials in the same
BVH, use multiple build inputs.

See alsoOptixBuildInput::curveArray

## 7.6.2 Member Data Documentation

7.6.2.1 curveType

OptixPrimitiveTypeOptixBuildInputCurveArray::curveType

Curve degree and basis.

See alsoOptixPrimitiveType

7.6.2.2 endcapFlags

unsigned int OptixBuildInputCurveArray::endcapFlags

End cap flags, see OptixCurveEndcapFlags.

7.6.2.3 flag

unsigned int OptixBuildInputCurveArray::flag

Combination of OptixGeometryFlags describing the primitive behavior.

7.6.2.4 indexBuffer

## CUdeviceptrOptixBuildInputCurveArray::indexBuffer

Device pointer to array of unsigned ints, one per curve segment. This buffer is required (unlike for
OptixBuildInputTriangleArray). Each index is the start of degree+1 consecutive vertices in
vertexBuffers, and corresponding widths in widthBuffers and normals in normalBuffers. These define
a single segment. Size of array is numPrimitives.

7.6.2.5 indexStrideInBytes

## unsigned int OptixBuildInputCurveArray::indexStrideInBytes

Stride between indices. If set to zero, indices are assumed to be tightly packed and stride is
sizeof(unsigned int).

7.6.2.6 normalBuffers

constCUdeviceptr*∗* OptixBuildInputCurveArray::normalBuffers

Reserved for future use.

7.6.2.7 normalStrideInBytes

unsigned int OptixBuildInputCurveArray::normalStrideInBytes

Reserved for future use.

7.6.2.8 numPrimitives

unsigned int OptixBuildInputCurveArray::numPrimitives

---

<u>186</u>

Number of primitives. Each primitive is a polynomial curve segment.

7.6.2.9 numVertices

unsigned int OptixBuildInputCurveArray::numVertices

Number of vertices in each buffer in vertexBuffers.

7.6.2.10 primitiveIndexOffset

## unsigned int OptixBuildInputCurveArray::primitiveIndexOffset

Primitive index bias, applied inoptixGetPrimitiveIndex( ). Sum of primitiveIndexOffset and number of
primitives must not overflow 32bits.

7.6.2.11 vertexBuffers

constCUdeviceptr*∗* OptixBuildInputCurveArray::vertexBuffers

Pointer to host array of device pointers, one per motion step. Host array size must match number of
motion keys as set inOptixMotionOptions(or an array of size 1 ifOptixMotionOptions::numKeysis set
to 1). Each per-motion-key device pointer must point to an array of floats (the vertices of the curves).

7.6.2.12 vertexStrideInBytes

## unsigned int OptixBuildInputCurveArray::vertexStrideInBytes

Stride between vertices. If set to zero, vertices are assumed to be tightly packed and stride is
sizeof(float3).

7.6.2.13 widthBuffers

constCUdeviceptr*∗* OptixBuildInputCurveArray::widthBuffers

Parallel to vertexBuffers: a device pointer per motion step, each with numVertices float values,
specifying the curve width (radius) corresponding to each vertex.

7.6.2.14 widthStrideInBytes

unsigned int OptixBuildInputCurveArray::widthStrideInBytes

Stride between widths. If set to zero, widths are assumed to be tightly packed and stride is sizeof(float).

## 7.7 OptixBuildInputCustomPrimitiveArray Struct Reference

#include <optix_types.h>

## Public Attributes

•constCUdeviceptr *∗* aabbBuffers

•unsigned intnumPrimitives

•unsigned intstrideInBytes

•const unsigned int *∗* flags

•unsigned intnumSbtRecords

•CUdeviceptrsbtIndexOffsetBuffer

•unsigned intsbtIndexOffsetSizeInBytes

•unsigned intsbtIndexOffsetStrideInBytes

•unsigned intprimitiveIndexOffset

---

<u>187</u>

## 7.7.1 Detailed Description

Custom primitive inputs.

See alsoOptixBuildInput::customPrimitiveArray

## 7.7.2 Member Data Documentation

7.7.2.1 aabbBuffers

constCUdeviceptr*∗* OptixBuildInputCustomPrimitiveArray::aabbBuffers

Points to host array of device pointers to AABBs (typeOptixAabb), one per motion step. Host array
size must match number of motion keys as set inOptixMotionOptions(or an array of size 1 if
OptixMotionOptions::numKeysis set to 1). Each device pointer must be a multiple of OPTIX_AABB_
BUFFER_BYTE_ALIGNMENT.

7.7.2.2 flags

## const unsigned int∗ OptixBuildInputCustomPrimitiveArray::flags

Array of flags, to specify flags per sbt record, combinations of OptixGeometryFlags describing the
primitive behavior, size must match numSbtRecords.

7.7.2.3 numPrimitives

## unsigned int OptixBuildInputCustomPrimitiveArray::numPrimitives

Number of primitives in each buffer (i.e., per motion step) inOptixBuildInputCustomPrimitiveArray
::aabbBuffers.

7.7.2.4 numSbtRecords

unsigned int OptixBuildInputCustomPrimitiveArray::numSbtRecords

Number of sbt records available to the sbt index offset override.

7.7.2.5 primitiveIndexOffset

## unsigned int OptixBuildInputCustomPrimitiveArray::primitiveIndexOffset

Primitive index bias, applied inoptixGetPrimitiveIndex( ). Sum of primitiveIndexOffset and number of
primitive must not overflow 32bits.

7.7.2.6 sbtIndexOffsetBuffer

## CUdeviceptrOptixBuildInputCustomPrimitiveArray::sbtIndexOffsetBuffer

Device pointer to per-primitive local sbt index offset buffer. May be NULL. Every entry must be in
range [0,numSbtRecords-1]. Size needs to be the number of primitives.

7.7.2.7 sbtIndexOffsetSizeInBytes

unsigned int OptixBuildInputCustomPrimitiveArray::sbtIndexOffsetSizeInBytes
Size of type of the sbt index offset. Needs to be 0, 1, 2 or 4 (8, 16 or 32 bit).

7.7.2.8 sbtIndexOffsetStrideInBytes

unsigned int OptixBuildInputCustomPrimitiveArray

---

<u>188</u>

::sbtIndexOffsetStrideInBytes

Stride between the index offsets. If set to zero, the offsets are assumed to be tightly packed and the
stride matches the size of the type (sbtIndexOffsetSizeInBytes).

7.7.2.9 strideInBytes

unsigned int OptixBuildInputCustomPrimitiveArray::strideInBytes

Stride between AABBs (per motion key). If set to zero, the aabbs are assumed to be tightly packed and
the stride is assumed to be sizeof(OptixAabb). If non-zero, the value must be a multiple of OPTIX_
AABB_BUFFER_BYTE_ALIGNMENT.

## 7.8 OptixBuildInputInstanceArray Struct Reference

#include <optix_types.h>

## Public Attributes

•CUdeviceptrinstances

•unsigned intinstanceStride

## 7.8.1 Detailed Description

•unsigned intnumInstances

Instance and instance pointer inputs.

## 7.8.2 Member Data Documentation

See alsoOptixBuildInput::instanceArray

7.8.2.1 instances

CUdeviceptrOptixBuildInputInstanceArray::instances

IfOptixBuildInput::typeis OPTIX_BUILD_INPUT_TYPE_INSTANCE_POINTERS instances and aabbs
should be interpreted as arrays of pointers instead of arrays of structs.

This pointer must be a multiple of OPTIX_INSTANCE_BYTE_ALIGNMENT ifOptixBuildInput::type
is OPTIX_BUILD_INPUT_TYPE_INSTANCES. The array elements must be a multiple of OPTIX_
INSTANCE_BYTE_ALIGNMENT ifOptixBuildInput::typeis OPTIX_BUILD_INPUT_TYPE_
INSTANCE_POINTERS.

7.8.2.2 instanceStride

unsigned int OptixBuildInputInstanceArray::instanceStride

Only valid for OPTIX_BUILD_INPUT_TYPE_INSTANCE Defines the stride between instances. A
stride of 0 indicates a tight packing, i.e., stride = sizeof(OptixInstance)

7.8.2.3 numInstances

unsigned int OptixBuildInputInstanceArray::numInstances

Number of elements inOptixBuildInputInstanceArray::instances.

## 7.9 OptixBuildInputOpacityMicromap Struct Reference

#include <optix_types.h>

---

<u>7.9</u>

<u>189</u>

## Public Attributes

•OptixOpacityMicromapArrayIndexingModeindexingMode

•CUdeviceptropacityMicromapArray

•CUdeviceptrindexBuffer

•unsigned intindexSizeInBytes

•unsigned intindexStrideInBytes

•unsigned intindexOffset

•unsigned intnumMicromapUsageCounts

•constOptixOpacityMicromapUsageCount *∗* micromapUsageCounts

## 7.9.1 Member Data Documentation

7.9.1.1 indexBuffer

## CUdeviceptrOptixBuildInputOpacityMicromap::indexBuffer

int16 or int32 buffer specifying which opacity micromap index to use for each triangle. Instead of an
actual index, one of the predefined indices OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_
(FULLY_TRANSPARENT *|* FULLY_OPAQUE *|* FULLY_UNKNOWN_TRANSPARENT *|* FULLY_
UNKNOWN_OPAQUE) can be used to indicate that there is no opacity micromap for this particular
triangle but the triangle is in a uniform state and the selected behavior is applied to the entire triangle.
This buffer is required whenOptixBuildInputOpacityMicromap::indexingModeis OPTIX_OPACITY_
MICROMAP_ARRAY_INDEXING_MODE_INDEXED. Must be zero if

OptixBuildInputOpacityMicromap::indexingModeis OPTIX_OPACITY_MICROMAP_ARRAY_
INDEXING_MODE_LINEAR or OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_
NONE.

7.9.1.2 indexingMode

OptixOpacityMicromapArrayIndexingModeOptixBuildInputOpacityMicromap
::indexingMode

Indexing mode of triangle to opacity micromap array mapping.

7.9.1.3 indexOffset

## unsigned int OptixBuildInputOpacityMicromap::indexOffset

Constant offset to non-negative opacity micromap indices.

7.9.1.4 indexSizeInBytes

## unsigned int OptixBuildInputOpacityMicromap::indexSizeInBytes

0, 2 or 4 (unused, 16 or 32 bit) Must be non-zero whenOptixBuildInputOpacityMicromap
::indexingModeis OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_INDEXED.

7.9.1.5 indexStrideInBytes

## unsigned int OptixBuildInputOpacityMicromap::indexStrideInBytes

Opacity micromap index buffer stride. If set to zero, indices are assumed to be tightly packed and
stride is inferred fromOptixBuildInputOpacityMicromap::indexSizeInBytes.

7.9.1.6 micromapUsageCounts

constOptixOpacityMicromapUsageCount*∗* OptixBuildInputOpacityMicromap
::micromapUsageCounts

---

<u>190</u>

List of number of usages of opacity micromaps of format and subdivision combinations. Counts with
equal format and subdivision combination (duplicates) are added together.

7.9.1.7 numMicromapUsageCounts

unsigned int OptixBuildInputOpacityMicromap::numMicromapUsageCounts

Number ofOptixOpacityMicromapUsageCount.

7.9.1.8 opacityMicromapArray

## CUdeviceptrOptixBuildInputOpacityMicromap::opacityMicromapArray

Device pointer to a opacity micromap array used by this build input array. This buffer is required
whenOptixBuildInputOpacityMicromap::indexingModeis OPTIX_OPACITY_MICROMAP_ARRAY_
INDEXING_MODE_LINEAR or OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_
INDEXED. Must be zero ifOptixBuildInputOpacityMicromap::indexingModeis OPTIX_OPACITY_
MICROMAP_ARRAY_INDEXING_MODE_NONE.

## 7.10 OptixBuildInputSphereArray Struct Reference

#include <optix_types.h>

## Public Attributes

•constCUdeviceptr *∗* vertexBuffers

•unsigned intvertexStrideInBytes

•unsigned intnumVertices

•constCUdeviceptr *∗* radiusBuffers

•unsigned intradiusStrideInBytes

•intsingleRadius

•const unsigned int *∗* flags

•unsigned intnumSbtRecords

•CUdeviceptrsbtIndexOffsetBuffer

•unsigned intsbtIndexOffsetSizeInBytes

•unsigned intsbtIndexOffsetStrideInBytes

•unsigned intprimitiveIndexOffset

## 7.10.1 Detailed Description

Sphere inputs.

A sphere is defined by a center point and a radius. Each center point is represented by a vertex in the
vertex buffer. There is either a single radius for all spheres, or the radii are represented by entries in the
radius buffer.

The vertex buffers and radius buffers point to a host array of device pointers, one per motion step. Host
array size must match the number of motion keys as set inOptixMotionOptions(or an array of size 1 if
OptixMotionOptions::numKeysis set to 0 or 1). Each per motion key device pointer must point to an
array of vertices corresponding to the center points of the spheres, or an array of 1 or N radii. Format
OPTIX_VERTEX_FORMAT_FLOAT3 is used for vertices, OPTIX_VERTEX_FORMAT_FLOAT for radii.

See alsoOptixBuildInput::sphereArray

---

<u>191</u>

## 7.10.2 Member Data Documentation

7.10.2.1 flags

## const unsigned int∗ OptixBuildInputSphereArray::flags

Array of flags, to specify flags per sbt record, combinations of OptixGeometryFlags describing the
primitive behavior, size must match numSbtRecords.

7.10.2.2 numSbtRecords

## unsigned int OptixBuildInputSphereArray::numSbtRecords

Number of sbt records available to the sbt index offset override.

7.10.2.3 numVertices

## unsigned int OptixBuildInputSphereArray::numVertices

Number of vertices in each buffer in vertexBuffers.

7.10.2.4 primitiveIndexOffset

## unsigned int OptixBuildInputSphereArray::primitiveIndexOffset

Primitive index bias, applied inoptixGetPrimitiveIndex( ). Sum of primitiveIndexOffset and number of
primitives must not overflow 32bits.

7.10.2.5 radiusBuffers

constCUdeviceptr*∗* OptixBuildInputSphereArray::radiusBuffers

Parallel to vertexBuffers: a device pointer per motion step, each with numRadii float values, specifying
the sphere radius corresponding to each vertex.

7.10.2.6 radiusStrideInBytes

## unsigned int OptixBuildInputSphereArray::radiusStrideInBytes

Stride between radii. If set to zero, widths are assumed to be tightly packed and stride is sizeof(float).

7.10.2.7 sbtIndexOffsetBuffer

## CUdeviceptrOptixBuildInputSphereArray::sbtIndexOffsetBuffer

Device pointer to per-primitive local sbt index offset buffer. May be NULL. Every entry must be in
range [0,numSbtRecords-1]. Size needs to be the number of primitives.

7.10.2.8 sbtIndexOffsetSizeInBytes

Size of type of the sbt index offset. Needs to be 0, 1, 2 or 4 (8, 16 or 32 bit).

## unsigned int OptixBuildInputSphereArray::sbtIndexOffsetSizeInBytes

7.10.2.9 sbtIndexOffsetStrideInBytes

## unsigned int OptixBuildInputSphereArray::sbtIndexOffsetStrideInBytes

Stride between the sbt index offsets. If set to zero, the offsets are assumed to be tightly packed and the
stride matches the size of the type (sbtIndexOffsetSizeInBytes).

---

<u>192</u>

7.10.2.10 singleRadius

int OptixBuildInputSphereArray::singleRadius

Boolean value indicating whether a single radius per radius buffer is used, or the number of radii in
radiusBuffers equals numVertices.

7.10.2.11 vertexBuffers

constCUdeviceptr*∗* OptixBuildInputSphereArray::vertexBuffers

Pointer to host array of device pointers, one per motion step. Host array size must match number of
motion keys as set inOptixMotionOptions(or an array of size 1 ifOptixMotionOptions::numKeysis
set to 1). Each per-motion-key device pointer must point to an array of floats (the center points of the
spheres).

7.10.2.12 vertexStrideInBytes

unsigned int OptixBuildInputSphereArray::vertexStrideInBytes

Stride between vertices. If set to zero, vertices are assumed to be tightly packed and stride is
sizeof(float3).

## 7.11 OptixBuildInputTriangleArray Struct Reference

#include <optix_types.h>

## Public Attributes

•constCUdeviceptr *∗* vertexBuffers

•unsigned intnumVertices

•OptixVertexFormatvertexFormat

•unsigned intvertexStrideInBytes

•CUdeviceptrindexBuffer

•unsigned intnumIndexTriplets

•OptixIndicesFormatindexFormat

•unsigned intindexStrideInBytes

•CUdeviceptrpreTransform

•const unsigned int *∗* flags

•unsigned intnumSbtRecords

•CUdeviceptrsbtIndexOffsetBuffer

•unsigned intsbtIndexOffsetSizeInBytes

•unsigned intsbtIndexOffsetStrideInBytes

•unsigned intprimitiveIndexOffset

•OptixTransformFormattransformFormat

•OptixBuildInputOpacityMicromapopacityMicromap

## 7.11.1 Detailed Description

Triangle inputs.

See alsoOptixBuildInput::triangleArray

## 7.11.2 Member Data Documentation

7.11.2.1 flags

const unsigned int*∗* OptixBuildInputTriangleArray::flags

---

<u>193</u>

Array of flags, to specify flags per sbt record, combinations of OptixGeometryFlags describing the
primitive behavior, size must match numSbtRecords.

7.11.2.2 indexBuffer

## CUdeviceptrOptixBuildInputTriangleArray::indexBuffer

Optional pointer to array of 16 or 32-bit int triplets, one triplet per triangle. The minimum alignment
must match the natural alignment of the type as specified in the indexFormat, i.e., for OPTIX_INDICES
_FORMAT_UNSIGNED_INT3 4-byte and for OPTIX_INDICES_FORMAT_UNSIGNED_SHORT3 a
2-byte alignment.

7.11.2.3 indexFormat

## OptixIndicesFormatOptixBuildInputTriangleArray::indexFormat

7.11.2.4 indexStrideInBytes

See alsoOptixIndicesFormat

## unsigned int OptixBuildInputTriangleArray::indexStrideInBytes

7.11.2.5 numIndexTriplets

Stride between triplets of indices. If set to zero, indices are assumed to be tightly packed and stride is
inferred from indexFormat.

## unsigned int OptixBuildInputTriangleArray::numIndexTriplets

7.11.2.6 numSbtRecords

Size of array inOptixBuildInputTriangleArray::indexBuffer. For build, needs to be zero if indexBuffer
is nullptr.

unsigned int OptixBuildInputTriangleArray::numSbtRecords

Number of sbt records available to the sbt index offset override.

7.11.2.7 numVertices

unsigned int OptixBuildInputTriangleArray::numVertices

Number of vertices in each of buffer inOptixBuildInputTriangleArray::vertexBuffers.

7.11.2.8 opacityMicromap

OptixBuildInputOpacityMicromapOptixBuildInputTriangleArray

::opacityMicromap

Optional opacity micromap inputs.

7.11.2.9 preTransform

## CUdeviceptrOptixBuildInputTriangleArray::preTransform

Optional pointer to array of floats representing a 3x4 row major affine transformation matrix. This
pointer must be a multiple of OPTIX_GEOMETRY_TRANSFORM_BYTE_ALIGNMENT.

---

<u>194</u>

7.11.2.10 primitiveIndexOffset

unsigned int OptixBuildInputTriangleArray::primitiveIndexOffset

Primitive index bias, applied inoptixGetPrimitiveIndex( ). Sum of primitiveIndexOffset and number of
triangles must not overflow 32bits.

7.11.2.11 sbtIndexOffsetBuffer

CUdeviceptrOptixBuildInputTriangleArray::sbtIndexOffsetBuffer

Device pointer to per-primitive local sbt index offset buffer. May be NULL. Every entry must be in
range [0,numSbtRecords-1]. Size needs to be the number of primitives.

7.11.2.12 sbtIndexOffsetSizeInBytes

unsigned int OptixBuildInputTriangleArray::sbtIndexOffsetSizeInBytes

7.11.2.13 sbtIndexOffsetStrideInBytes

Size of type of the sbt index offset. Needs to be 0, 1, 2 or 4 (8, 16 or 32 bit).

unsigned int OptixBuildInputTriangleArray::sbtIndexOffsetStrideInBytes

Stride between the index offsets. If set to zero, the offsets are assumed to be tightly packed and the
stride matches the size of the type (sbtIndexOffsetSizeInBytes).

7.11.2.14 transformFormat

OptixTransformFormatOptixBuildInputTriangleArray::transformFormat

7.11.2.15 vertexBuffers

See alsoOptixTransformFormat

constCUdeviceptr*∗* OptixBuildInputTriangleArray::vertexBuffers

Points to host array of device pointers, one per motion step. Host array size must match the number of
motion keys as set inOptixMotionOptions(or an array of size 1 ifOptixMotionOptions::numKeysis
set to 0 or 1). Each per motion key device pointer must point to an array of vertices of the triangles in
the format as described by vertexFormat. The minimum alignment must match the natural alignment
of the type as specified in the vertexFormat, i.e., for OPTIX_VERTEX_FORMAT_FLOATX 4-byte, for all
others a 2-byte alignment. However, an 16-byte stride (and buffer alignment) is recommended for
vertices of format OPTIX_VERTEX_FORMAT_FLOAT3 for GAS build performance.

7.11.2.16 vertexFormat

OptixVertexFormatOptixBuildInputTriangleArray::vertexFormat

See alsoOptixVertexFormat

7.11.2.17 vertexStrideInBytes

unsigned int OptixBuildInputTriangleArray::vertexStrideInBytes

Stride between vertices. If set to zero, vertices are assumed to be tightly packed and stride is inferred
from vertexFormat.

---

## 7.12 OptixBuiltinISOptions Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixPrimitiveTypebuiltinISModuleType

•intusesMotionBlur

•unsigned intbuildFlags

•unsigned intcurveEndcapFlags

## 7.12.1 Detailed Description

Specifies the options for retrieving an intersection program for a built-in primitive type. The primitive
type must not be OPTIX_PRIMITIVE_TYPE_CUSTOM.

See alsooptixBuiltinISModuleGet( )

## 7.12.2 Member Data Documentation

7.12.2.1 buildFlags

unsigned int OptixBuiltinISOptions::buildFlags

Build flags, see OptixBuildFlags.

7.12.2.2 builtinISModuleType

OptixPrimitiveTypeOptixBuiltinISOptions::builtinISModuleType

7.12.2.3 curveEndcapFlags

unsigned int OptixBuiltinISOptions::curveEndcapFlags

End cap properties of curves, see OptixCurveEndcapFlags, 0 for non-curve types.

7.12.2.4 usesMotionBlur

int OptixBuiltinISOptions::usesMotionBlur

Boolean value indicating whether vertex motion blur is used (but not motion transform blur).

## 7.13 OptixClusterAccelBuildInput Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixClusterAccelBuildTypetype

•union {

OptixClusterAccelBuildInputTrianglestriangles

OptixClusterAccelBuildInputClustersclusters

OptixClusterAccelBuildInputGridsgrids

};

---

## 7.13.1 Member Data Documentation

7.13.1.1

union {...} OptixClusterAccelBuildInput::@5

7.13.1.2 clusters

OptixClusterAccelBuildInputClustersOptixClusterAccelBuildInput::clusters
Used for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS type builds.

7.13.1.3 grids

OptixClusterAccelBuildInputGridsOptixClusterAccelBuildInput::grids

7.13.1.4 triangles

Used for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_GRIDS type builds.

OptixClusterAccelBuildInputTrianglesOptixClusterAccelBuildInput::triangles

Used for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLES, OPTIX_
CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES, OPTIX_CLUSTER_ACCEL_
BUILD_TYPE_CLUSTERS_FROM_TEMPLATES type builds.

7.13.1.5 type

OptixClusterAccelBuildTypeOptixClusterAccelBuildInput::type

## 7.14 OptixClusterAccelBuildInputClusters Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixClusterAccelBuildFlagsflags

•unsigned intmaxArgCount

•unsigned intmaxTotalClusterCount

•unsigned intmaxClusterCountPerArg

## 7.14.1 Member Data Documentation

7.14.1.1 flags

OptixClusterAccelBuildFlagsOptixClusterAccelBuildInputClusters::flags

7.14.1.2 maxArgCount

unsigned int OptixClusterAccelBuildInputClusters::maxArgCount

Max number ofOptixClusterAccelBuildInputClustersArgsprovided at build time for OPTIX_
CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS.

7.14.1.3 maxClusterCountPerArg

unsigned int OptixClusterAccelBuildInputClusters::maxClusterCountPerArg

---

7.14.1.4 maxTotalClusterCount

unsigned int OptixClusterAccelBuildInputClusters::maxTotalClusterCount

## 7.15 OptixClusterAccelBuildInputClustersArgs Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intclusterHandlesCount

•unsigned intclusterHandlesBufferStrideInBytes

•CUdeviceptrclusterHandlesBuffer

## 7.15.1 Detailed Description

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS
builds.

## 7.15.2 Member Data Documentation

CUdeviceptrOptixClusterAccelBuildInputClustersArgs::clusterHandlesBuffer

7.15.2.1 clusterHandlesBuffer

The clusterHandlesBuffer can come directly from CLAS builds output via

OptixClusterAccelBuildModeDescImplicitDest::outputHandlesBufferor

OptixClusterAccelBuildModeDescExplicitDest::outputHandlesBuffer.

7.15.2.2 clusterHandlesBufferStrideInBytes

unsigned int OptixClusterAccelBuildInputClustersArgs
::clusterHandlesBufferStrideInBytes

7.15.2.3 clusterHandlesCount

unsigned int OptixClusterAccelBuildInputClustersArgs::clusterHandlesCount

Number of CLAS input to the BLAS build (size of the clusterHandles buffer)

## 7.16 OptixClusterAccelBuildInputGrids Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixClusterAccelBuildFlagsflags

•unsigned intmaxArgCount

•OptixVertexFormatvertexFormat

•unsigned intmaxSbtIndexValue

•unsigned intmaxWidth

•unsigned intmaxHeight

## 7.16.1 Member Data Documentation

7.16.1.1 flags

OptixClusterAccelBuildFlagsOptixClusterAccelBuildInputGrids::flags

---

7.16.1.2 maxArgCount

unsigned int OptixClusterAccelBuildInputGrids::maxArgCount

7.16.1.3 maxHeight

unsigned int OptixClusterAccelBuildInputGrids::maxHeight

The maximum number of edge segments along the height of any grid.

7.16.1.4 maxSbtIndexValue

## unsigned int OptixClusterAccelBuildInputGrids::maxSbtIndexValue

The maximum used SBT index over all clusters. This must include the base SBT offset (
::basePrimitiveInfo), any potential per primitive offset (::primitiveInfoBuffer), as well as a potential
offset at template instantiation (OptixClusterAccelBuildInputTemplatesArgs::sbtIndexOffset)

7.16.1.5 maxWidth

7.16.1.6 vertexFormat

unsigned int OptixClusterAccelBuildInputGrids::maxWidth

The maximum number of edge segments along the width of any grid.

OptixVertexFormatOptixClusterAccelBuildInputGrids::vertexFormat

## 7.17 OptixClusterAccelBuildInputGridsArgs Struct Reference

OptixVertexFormat (see documentation for supported formats)

## Public Attributes

#include <optix_types.h>

•unsigned intbaseClusterId

•unsigned intclusterFlags

•OptixClusterAccelPrimitiveInfobasePrimitiveInfo

•unsigned intpositionTruncateBitCount: 6

•unsigned intreserved: 26

•unsigned chardimensions[2]

•unsigned shortreserved2

## 7.17.1 Detailed Description

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_
GRIDS builds.

## 7.17.2 Member Data Documentation

7.17.2.1 baseClusterId

unsigned int OptixClusterAccelBuildInputGridsArgs::baseClusterId

32-bit user-defined ID, serves as a base value for the template and can be offset at template
instantiation (seeOptixClusterAccelBuildInputTemplatesArgs::clusterIdOffset)

---

7.17.2.2 basePrimitiveInfo

OptixClusterAccelPrimitiveInfoOptixClusterAccelBuildInputGridsArgs

::basePrimitiveInfo

Applied to all triangles in cluster.

7.17.2.3 clusterFlags

unsigned int OptixClusterAccelBuildInputGridsArgs::clusterFlags

7.17.2.4 dimensions

Combination of OptixClusterAccelClusterFlags.

unsigned char OptixClusterAccelBuildInputGridsArgs::dimensions[2]

Resolution of the 2D grid, max value per dimension can be queried.

7.17.2.5 positionTruncateBitCount

unsigned int OptixClusterAccelBuildInputGridsArgs::positionTruncateBitCount

SeeOptixClusterAccelBuildInputTrianglesArgs::positionTruncateBitCount.

7.17.2.6 reserved

7.17.2.7 reserved2

unsigned int OptixClusterAccelBuildInputGridsArgs::reserved

unsigned short OptixClusterAccelBuildInputGridsArgs::reserved2

## 7.18 OptixClusterAccelBuildInputTemplatesArgs Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intclusterIdOffset

•unsigned intsbtIndexOffset

•CUdeviceptrclusterTemplate

•CUdeviceptrvertexBuffer

•unsigned intvertexStrideInBytes

•unsigned intreserved

## 7.18.1 Detailed Description

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_
TEMPLATES builds.

## 7.18.2 Member Data Documentation

7.18.2.1 clusterIdOffset

unsigned int OptixClusterAccelBuildInputTemplatesArgs::clusterIdOffset

Offset applied to template baseClusterId, effective clusterId = clusterTemplate.baseClusterId +
clusterIdOffset. Either may be 0.

---

7.18.2.2 clusterTemplate

CUdeviceptrOptixClusterAccelBuildInputTemplatesArgs::clusterTemplate
Opaque pointer to the template.

7.18.2.3 reserved

unsigned int OptixClusterAccelBuildInputTemplatesArgs::reserved

7.18.2.4 sbtIndexOffset

unsigned int OptixClusterAccelBuildInputTemplatesArgs::sbtIndexOffset

Offset to base sbtIndex from template creation (which may define a constant or per-triangle base
sbtIndex), final sbt index is also limited to fit into 24b.

7.18.2.5 vertexBuffer

CUdeviceptrOptixClusterAccelBuildInputTemplatesArgs::vertexBuffer

The vertex data to use to instantiate the template; vertex order must match that of template creation.
For templates created from grids, see documentation.

7.18.2.6 vertexStrideInBytes

unsigned int OptixClusterAccelBuildInputTemplatesArgs::vertexStrideInBytes
Stride between elements in vertex buffer. Stride 0 -*>* natural stride.

## 7.19 OptixClusterAccelBuildInputTriangles Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixClusterAccelBuildFlagsflags

•unsigned intmaxArgCount

•OptixVertexFormatvertexFormat

•unsigned intmaxSbtIndexValue

•unsigned intmaxUniqueSbtIndexCountPerArg

•unsigned intmaxTriangleCountPerArg

•unsigned intmaxVertexCountPerArg

•unsigned intmaxTotalTriangleCount

•unsigned intmaxTotalVertexCount

•unsigned intminPositionTruncateBitCount

## 7.19.1 Member Data Documentation

7.19.1.1 flags

OptixClusterAccelBuildFlagsOptixClusterAccelBuildInputTriangles::flags

7.19.1.2 maxArgCount

unsigned int OptixClusterAccelBuildInputTriangles::maxArgCount

Max number ofOptixClusterAccelBuildInputTrianglesArgsprovided at build time for OPTIX_
CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLES and OPTIX_CLUSTER_ACCEL_

---

## BUILD_TYPE_TEMPLATES_FROM_TRIANGLES. Max number of

OptixClusterAccelBuildInputTemplatesArgsprovided at build time for OPTIX_CLUSTER_ACCEL_
BUILD_TYPE_CLUSTERS_FROM_TEMPLATES.

7.19.1.3 maxSbtIndexValue

## unsigned int OptixClusterAccelBuildInputTriangles::maxSbtIndexValue

The maximum used sbt index over all clusters; This must include the base sbt offset (
::basePrimitiveInfo), any potential per primitive offset (::primitiveInfoBuffer), as well as a potential
offset at template instantiation (OptixClusterAccelBuildInputTemplatesArgs::sbtIndexOffset)

7.19.1.4 maxTotalTriangleCount

## unsigned int OptixClusterAccelBuildInputTriangles::maxTotalTriangleCount

Optional, upper bound on the number of triangles over all Args, maxTriangleCountPerArg *∗*
maxArgCount otherwise.

7.19.1.5 maxTotalVertexCount

## unsigned int OptixClusterAccelBuildInputTriangles::maxTotalVertexCount

Optional, upper bound on the number of vertices over all Args, maxVertexCountPerArg *∗*
maxArgCount otherwise.

7.19.1.6 maxTriangleCountPerArg

unsigned int OptixClusterAccelBuildInputTriangles::maxTriangleCountPerArg

Upper bound on the number of triangles per Arg.

7.19.1.7 maxUniqueSbtIndexCountPerArg

unsigned int OptixClusterAccelBuildInputTriangles

::maxUniqueSbtIndexCountPerArg

Number of unique SBT indices per cluster. If the cluster has the same SBT index for all its triangles, this
value is 1.

7.19.1.8 maxVertexCountPerArg

## unsigned int OptixClusterAccelBuildInputTriangles::maxVertexCountPerArg

Upper bound on the number of vertices per Arg.

7.19.1.9 minPositionTruncateBitCount

unsigned int OptixClusterAccelBuildInputTriangles

::minPositionTruncateBitCount

Lower bound on the number of bits being truncated of the vertex positions.

7.19.1.10 vertexFormat

## OptixVertexFormatOptixClusterAccelBuildInputTriangles::vertexFormat

OptixVertexFormat (see documentation for supported formats)

---

## 7.20 OptixClusterAccelBuildInputTrianglesArgs Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intclusterId

•unsigned intclusterFlags

•unsigned inttriangleCount: 9

•unsigned intvertexCount: 9

•unsigned intpositionTruncateBitCount: 6

•unsigned intindexFormat: 4

•unsigned intopacityMicromapIndexFormat: 4

•OptixClusterAccelPrimitiveInfobasePrimitiveInfo

•unsigned shortindexBufferStrideInBytes

•unsigned shortvertexBufferStrideInBytes

•unsigned shortprimitiveInfoBufferStrideInBytes

•unsigned shortopacityMicromapIndexBufferStrideInBytes

•CUdeviceptrindexBuffer

•CUdeviceptrvertexBuffer

•CUdeviceptrprimitiveInfoBuffer

•CUdeviceptropacityMicromapArray

•CUdeviceptropacityMicromapIndexBuffer

•CUdeviceptrinstantiationBoundingBoxLimit

## 7.20.1 Detailed Description

Device data, args provided for OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_
TRIANGLES builds and OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES
builds.

## 7.20.2 Member Data Documentation

7.20.2.1 basePrimitiveInfo

OptixClusterAccelPrimitiveInfoOptixClusterAccelBuildInputTrianglesArgs

::basePrimitiveInfo

Applied to all triangles in cluster. Additional per triangle flags can be specified in PrimitiveInfoBuffer.

7.20.2.2 clusterFlags

unsigned int OptixClusterAccelBuildInputTrianglesArgs::clusterFlags

Combination of OptixClusterAccelClusterFlags.

7.20.2.3 clusterId

## unsigned int OptixClusterAccelBuildInputTrianglesArgs::clusterId

32-bit user-defined ID, for template creation acts as the baseClusterId and can be offset at template
instantiation (seeOptixClusterAccelBuildInputTemplatesArgs::clusterIdOffset)

7.20.2.4 indexBuffer

CUdeviceptrOptixClusterAccelBuildInputTrianglesArgs::indexBuffer

Triplets of vertex indices into vertexBuffer per triangle. Must contain 3 *∗* triangleCount indices.

---

7.20.2.5 indexBufferStrideInBytes

unsigned short OptixClusterAccelBuildInputTrianglesArgs

::indexBufferStrideInBytes

Stride between elements in index buffer. Stride 0 -*>* natural stride.

7.20.2.6 indexFormat

unsigned int OptixClusterAccelBuildInputTrianglesArgs::indexFormat

Can use OptixClusterAccelIndicesFormat as helper to set value: 1, 2, or 4 bytes-wide indices.

7.20.2.7 instantiationBoundingBoxLimit

CUdeviceptrOptixClusterAccelBuildInputTrianglesArgs

::instantiationBoundingBoxLimit

## Optional with OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES,

32-byte-aligned pointer toOptixAabb, one per cluster, limiting the extent of each cluster. Vertices
provided for template instantiation must not be outside the bounding box. Providing a bounding box
may improve compression (reduced CLAS size) as well as trace performance. Ignored for OPTIX_
CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLES.

7.20.2.8 opacityMicromapArray

CUdeviceptrOptixClusterAccelBuildInputTrianglesArgs::opacityMicromapArray

Optional, needs to be set if OMMs are used.

7.20.2.9 opacityMicromapIndexBuffer

CUdeviceptrOptixClusterAccelBuildInputTrianglesArgs

::opacityMicromapIndexBuffer

Optional, needs to be set if OMMs are used.

7.20.2.10 opacityMicromapIndexBufferStrideInBytes

unsigned short OptixClusterAccelBuildInputTrianglesArgs

::opacityMicromapIndexBufferStrideInBytes

Stride between elements in omm index buffer. Stride 0 -*>* natural stride.

7.20.2.11 opacityMicromapIndexFormat

unsigned int OptixClusterAccelBuildInputTrianglesArgs

::opacityMicromapIndexFormat

Can use OptixClusterAccelIndicesFormat as helper to set value: 1, 2, or 4 bytes-wide indices.

7.20.2.12 positionTruncateBitCount

unsigned int OptixClusterAccelBuildInputTrianglesArgs

::positionTruncateBitCount

Number of LSB in mantissa that are dropped (0 means don't drop any) for float32 positions. Other
formats are first converted to float32 before dropping bits. Builder will drop bits when building CLAS
/ instantiating cluster templates (no need to truncate the input before build).

---

7.20.2.13 primitiveInfoBuffer

CUdeviceptrOptixClusterAccelBuildInputTrianglesArgs::primitiveInfoBuffer

Optional, per primitive array ofOptixClusterAccelPrimitiveInfo.

7.20.2.14 primitiveInfoBufferStrideInBytes

unsigned short OptixClusterAccelBuildInputTrianglesArgs

::primitiveInfoBufferStrideInBytes

Stride between elements in primitive info buffer. Stride 0 -*>* natural stride.

7.20.2.15 triangleCount

unsigned int OptixClusterAccelBuildInputTrianglesArgs::triangleCount

7.20.2.16 vertexBuffer

Number of triangles for cluster / cluster template, max value can be queried.

## CUdeviceptrOptixClusterAccelBuildInputTrianglesArgs::vertexBuffer

vertexBuffer is mandatory when using OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_
TRIANGLES. Optional with OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_
TRIANGLES and when specified provide example "hint" vertices for templates; actual vertices are
specified at template instantiation. It is typically useful to provide vertices for template creation in
scenarios such as animation, where the relative locality of vertices is expected to be similar between the
template creation and instantiation.

7.20.2.17 vertexBufferStrideInBytes

## unsigned short OptixClusterAccelBuildInputTrianglesArgs

::vertexBufferStrideInBytes

Stride between elements in vertex buffer. Stride 0 -*>* natural stride.

7.20.2.18 vertexCount

unsigned int OptixClusterAccelBuildInputTrianglesArgs::vertexCount

Number of vertices shared by triangles for cluster / cluster template, max value can be queried.

## 7.21 OptixClusterAccelBuildModeDesc Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixClusterAccelBuildModemode

•union {

OptixClusterAccelBuildModeDescImplicitDestimplicitDest

OptixClusterAccelBuildModeDescExplicitDestexplicitDest

OptixClusterAccelBuildModeDescGetSizegetSize

};

---

<u>205</u>

## 7.21.1 Member Data Documentation

7.21.1.1

union {...} OptixClusterAccelBuildModeDesc::@7

7.21.1.2 explicitDest

OptixClusterAccelBuildModeDescExplicitDestOptixClusterAccelBuildModeDesc
::explicitDest

7.21.1.3 getSize

OptixClusterAccelBuildModeDescGetSizeOptixClusterAccelBuildModeDesc

::getSize

7.21.1.4 implicitDest

OptixClusterAccelBuildModeDescImplicitDestOptixClusterAccelBuildModeDesc
::implicitDest

7.21.1.5 mode

OptixClusterAccelBuildModeOptixClusterAccelBuildModeDesc::mode

## 7.22 OptixClusterAccelBuildModeDescExplicitDest Struct Reference

#include <optix_types.h>

## Public Attributes

•CUdeviceptrtempBuffer

•size_ttempBufferSizeInBytes

•CUdeviceptrdestAddressesBuffer

•unsigned intdestAddressesStrideInBytes

•CUdeviceptroutputHandlesBuffer

•unsigned intoutputHandlesStrideInBytes

•CUdeviceptroutputSizesBuffer

•unsigned intoutputSizesStrideInBytes

## 7.22.1 Member Data Documentation

7.22.1.1 destAddressesBuffer

CUdeviceptrOptixClusterAccelBuildModeDescExplicitDest::destAddressesBuffer

Entries must be aligned according to the output type.

7.22.1.2 destAddressesStrideInBytes

unsigned int OptixClusterAccelBuildModeDescExplicitDest

::destAddressesStrideInBytes

Minimum 8, Stride of 0 implies natural stride of 8B.

7.22.1.3 outputHandlesBuffer

CUdeviceptrOptixClusterAccelBuildModeDescExplicitDest::outputHandlesBuffer

---

<u>206</u>

TraversableHandle for GAS, pointer for cluster and template outputs, can be the same as
destAddresses in which case they will overwrite the input.

7.22.1.4 outputHandlesStrideInBytes

unsigned int OptixClusterAccelBuildModeDescExplicitDest

::outputHandlesStrideInBytes

Minimum 8, Stride of 0 implies natural stride of 8B.

7.22.1.5 outputSizesBuffer

CUdeviceptrOptixClusterAccelBuildModeDescExplicitDest::outputSizesBuffer

Optional, uint32 array (4 byte aligned)

7.22.1.6 outputSizesStrideInBytes

unsigned int OptixClusterAccelBuildModeDescExplicitDest

::outputSizesStrideInBytes

Minimum 4, Stride of 0 implies natural stride of 4B.

7.22.1.7 tempBuffer

CUdeviceptrOptixClusterAccelBuildModeDescExplicitDest::tempBuffer

128-byte aligned, see OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT

7.22.1.8 tempBufferSizeInBytes

size_t OptixClusterAccelBuildModeDescExplicitDest::tempBufferSizeInBytes

## 7.23 OptixClusterAccelBuildModeDescGetSize Struct Reference

#include <optix_types.h>

## Public Attributes

•CUdeviceptroutputSizesBuffer

•unsigned intoutputSizesStrideInBytes

•CUdeviceptrtempBuffer

•size_ttempBufferSizeInBytes

## 7.23.1 Member Data Documentation

7.23.1.1 outputSizesBuffer

CUdeviceptrOptixClusterAccelBuildModeDescGetSize::outputSizesBuffer

Mandatory, uint32 array (4 byte aligned)

7.23.1.2 outputSizesStrideInBytes

unsigned int OptixClusterAccelBuildModeDescGetSize::outputSizesStrideInBytes

Minimum 4, Stride of 0 implies natural stride of 4B.

---

<u>207</u>

7.23.1.3 tempBuffer

CUdeviceptrOptixClusterAccelBuildModeDescGetSize::tempBuffer
128-byte aligned, see OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT

7.23.1.4 tempBufferSizeInBytes

size_t OptixClusterAccelBuildModeDescGetSize::tempBufferSizeInBytes

## 7.24 OptixClusterAccelBuildModeDescImplicitDest Struct Reference

#include <optix_types.h>

## Public Attributes

•CUdeviceptroutputBuffer

•size_toutputBufferSizeInBytes

•size_ttempBufferSizeInBytes

•CUdeviceptrtempBuffer

•CUdeviceptroutputHandlesBuffer

•unsigned intoutputHandlesStrideInBytes

•CUdeviceptroutputSizesBuffer

•unsigned intoutputSizesStrideInBytes

## 7.24.1 Member Data Documentation

7.24.1.1 outputBuffer

CUdeviceptrOptixClusterAccelBuildModeDescImplicitDest::outputBuffer

alignment of outputBuffer must match result type. Clusters: 128 bytes Templates: 32 bytes GASes: 128
bytes, see OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT

7.24.1.2 outputBufferSizeInBytes

size_t OptixClusterAccelBuildModeDescImplicitDest::outputBufferSizeInBytes
size of outputHandlesBuffer is outputHandlesStrideInBytes *∗* number of inputs specified with either
argCount or maxArgCount

7.24.1.3 outputHandlesBuffer

CUdeviceptrOptixClusterAccelBuildModeDescImplicitDest::outputHandlesBuffer
TraversableHandle for GAS, pointer for cluster and template outputs.

7.24.1.4 outputHandlesStrideInBytes

unsigned int OptixClusterAccelBuildModeDescImplicitDest

::outputHandlesStrideInBytes

Minimum 8, Stride of 0 implies natural stride of 8B.

7.24.1.5 outputSizesBuffer

CUdeviceptrOptixClusterAccelBuildModeDescImplicitDest::outputSizesBuffer
Optional, uint32 array (4 byte aligned)

---

<u>208</u>

7.24.1.6 outputSizesStrideInBytes

unsigned int OptixClusterAccelBuildModeDescImplicitDest

::outputSizesStrideInBytes

Minimum 4, Stride of 0 implies natural stride of 4B.

7.24.1.7 tempBuffer

CUdeviceptrOptixClusterAccelBuildModeDescImplicitDest::tempBuffer

128-byte aligned, see OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT

7.24.1.8 tempBufferSizeInBytes

## 7.25 OptixClusterAccelPrimitiveInfo Struct Reference

#include <optix_types.h>

## Public Attributes

size_t OptixClusterAccelBuildModeDescImplicitDest::tempBufferSizeInBytes

•unsigned intsbtIndex: 24

•unsigned intreserved: 5

•unsigned intprimitiveFlags: 3

## 7.25.1 Member Data Documentation

7.25.1.1 primitiveFlags

unsigned int OptixClusterAccelPrimitiveInfo::primitiveFlags

Combination of OptixClusterAccelPrimitiveFlags.

7.25.1.2 reserved

unsigned int OptixClusterAccelPrimitiveInfo::reserved

7.25.1.3 sbtIndex

unsigned int OptixClusterAccelPrimitiveInfo::sbtIndex

## 7.26 OptixCoopVec< T, N > Class Template Reference

#include <optix_device.h>

•usingvalue_type= T

## Public Types

## Public Member Functions

•__forceinline__ __device__OptixCoopVec()

•__forceinline__ __device__OptixCoopVec(constvalue_type&val)

•__forceinline__ __device__ constvalue_type&operator[ ](unsigned int index) const

•__forceinline__ __device__value_type&operator[ ](unsigned int index)

•__forceinline__ __device__ constvalue_type *∗* data() const

•__forceinline__ __device__value_type *∗* data()

---

<u>209</u>

## Static Public Attributes

•static const unsigned intsize= N

## Protected Attributes

•value_typem_data[size]

## 7.26.1 Detailed Description

## template<typename T, unsigned int N>

class OptixCoopVec< T, N >

The API does not require the use of this class specifically, but it must define a certain interface as
spelled out by the public members of the class. Note that not all types of T are supported. Only 8 and
32 bit signed and unsigned integral types along with 16 and 32 bit floating point values.

## 7.26.2 Member Typedef Documentation

7.26.2.1 value_type

template<typename T, unsigned int N>
usingOptixCoopVec< T, N >::value_type = T

## 7.26.3 Constructor & Destructor Documentation

## 7.26.3.1 OptixCoopVec( ) [1/2]

template<typename T, unsigned int N>
__forceinline__ __device__OptixCoopVec< T, N >::OptixCoopVec() [inline]

## 7.26.3.2 OptixCoopVec( ) [2/2]

template<typename T, unsigned int N>

__forceinline__ __device__OptixCoopVec< T, N >::OptixCoopVec(
constvalue_type& *val*) *[inline]*

## 7.26.4 Member Function Documentation

## 7.26.4.1 data( ) [1/2]

template<typename T, unsigned int N>
__forceinline__ __device__value_type ∗ OptixCoopVec< T, N >::data ()

*[inline]*
7.26.4.2 data( ) [2/2]
template<typename T, unsigned int N>

__forceinline__ __device__ constvalue_type ∗ OptixCoopVec< T, N >::data (

) const *[inline]*

## 7.26.4.3 operator[]( ) [1/2]

template<typename T, unsigned int N>

---

<u>210</u>

__forceinline__ __device__value_type&OptixCoopVec< T, N >::operator[ ] (
unsigned int *index*) *[inline]*

## 7.26.4.4 operator[]( ) [2/2]

template<typename T, unsigned int N>
__forceinline__ __device__ constvalue_type&OptixCoopVec< T, N >
::operator[ ] (
unsigned int *index*) const *[inline]*

## 7.26.5 Member Data Documentation

7.26.5.1 m_data

template<typename T, unsigned int N>
value_typeOptixCoopVec< T, N >::m_data[size] [protected]

7.26.5.2 size

template<typename T, unsigned int N>
const unsigned intOptixCoopVec< T, N >::size = N [static]

## 7.27 OptixCoopVecMatrixDescription Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intN

•unsigned intK

•unsigned intoffsetInBytes

•OptixCoopVecElemTypeelementType

•OptixCoopVecMatrixLayoutlayout

•unsigned introwColumnStrideInBytes

•unsigned intsizeInBytes

## 7.27.1 Detailed Description

Each matrix's offset from the base address is expressed with offsetInBytes. This allows for non-uniform
matrices to be tightly packed.

The rowColumnStrideInBytes is ignored if the layout is either OPTIX_COOP_VEC_MATRIX_LAYOUT
_INFERENCING_OPTIMAL or OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL

## 7.27.2 Member Data Documentation

7.27.2.1 elementType

OptixCoopVecElemTypeOptixCoopVecMatrixDescription::elementType

## 7.27.2.2 K

unsigned int OptixCoopVecMatrixDescription::K

---

<u>211</u>

7.27.2.3 layout

OptixCoopVecMatrixLayoutOptixCoopVecMatrixDescription::layout

## 7.27.2.4 N

unsigned int OptixCoopVecMatrixDescription::N

7.27.2.5 offsetInBytes

unsigned int OptixCoopVecMatrixDescription::offsetInBytes

7.27.2.6 rowColumnStrideInBytes

7.27.2.7 sizeInBytes

unsigned int OptixCoopVecMatrixDescription::rowColumnStrideInBytes

## 7.28 OptixDenoiserGuideLayer Struct Reference

unsigned int OptixCoopVecMatrixDescription::sizeInBytes

#include <optix_types.h>

## Public Attributes

•OptixImage2Dalbedo

•OptixImage2Dnormal

•OptixImage2Dflow

•OptixImage2DpreviousOutputInternalGuideLayer

•OptixImage2DoutputInternalGuideLayer

•OptixImage2DflowTrustworthiness

## 7.28.1 Detailed Description

Guide layer for the denoiser.

See alsooptixDenoiserInvoke( )

## 7.28.2 Member Data Documentation

7.28.2.1 albedo

OptixImage2DOptixDenoiserGuideLayer::albedo

7.28.2.2 flow

OptixImage2DOptixDenoiserGuideLayer::flow

7.28.2.3 flowTrustworthiness

OptixImage2DOptixDenoiserGuideLayer::flowTrustworthiness

7.28.2.4 normal

OptixImage2DOptixDenoiserGuideLayer::normal

---

<u>212</u>

7.28.2.5 outputInternalGuideLayer

OptixImage2DOptixDenoiserGuideLayer::outputInternalGuideLayer

7.28.2.6 previousOutputInternalGuideLayer

OptixImage2DOptixDenoiserGuideLayer::previousOutputInternalGuideLayer

## 7.29 OptixDenoiserLayer Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixImage2Dinput

•OptixImage2DpreviousOutput

•OptixImage2Doutput

•OptixDenoiserAOVTypetype

## 7.29.1 Detailed Description

Input/Output layers for the denoiser.

See alsooptixDenoiserInvoke( )

## 7.29.2 Member Data Documentation

7.29.2.1 input

OptixImage2DOptixDenoiserLayer::input

7.29.2.2 output

OptixImage2DOptixDenoiserLayer::output

7.29.2.3 previousOutput

OptixImage2DOptixDenoiserLayer::previousOutput

7.29.2.4 type

OptixDenoiserAOVTypeOptixDenoiserLayer::type

## 7.30 OptixDenoiserOptions Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intguideAlbedo

•unsigned intguideNormal

•OptixDenoiserAlphaModedenoiseAlpha

## 7.30.1 Detailed Description

Options used by the denoiser.

See alsooptixDenoiserCreate( )

---

## 7.30.2 Member Data Documentation

7.30.2.1 denoiseAlpha

OptixDenoiserAlphaModeOptixDenoiserOptions::denoiseAlpha

alpha denoise mode

7.30.2.2 guideAlbedo

unsigned int OptixDenoiserOptions::guideAlbedo

7.30.2.3 guideNormal

## 7.31 OptixDenoiserParams Struct Reference

#include <optix_types.h>

unsigned int OptixDenoiserOptions::guideNormal

## Public Attributes

•CUdeviceptrhdrIntensity

•floatblendFactor

•CUdeviceptrhdrAverageColor

•unsigned inttemporalModeUsePreviousLayers

•floatflowMulX

•floatflowMulY

## 7.31.1 Detailed Description

Various parameters used by the denoiser.

See alsooptixDenoiserInvoke( )

optixDenoiserComputeIntensity( )

optixDenoiserComputeAverageColor( )

## 7.31.2 Member Data Documentation

7.31.2.1 blendFactor

float OptixDenoiserParams::blendFactor

blend factor. If set to 0 the output is 100% of the denoised input. If set to 1, the output is 100% of the
unmodified input. Values between 0 and 1 will linearly interpolate between the denoised and
unmodified input.

7.31.2.2 flowMulX

float OptixDenoiserParams::flowMulX

Multiplication factors for motion vectors (flow guide layer). When set to zero, motion vectors are not
scaled.

7.31.2.3 flowMulY

float OptixDenoiserParams::flowMulY

---

7.31.2.4 hdrAverageColor

CUdeviceptrOptixDenoiserParams::hdrAverageColor

this parameter is used when the OPTIX_DENOISER_MODEL_KIND_AOV model kind is set. average
log color of input image, separate for RGB channels (default null pointer). points to three floats. if set to
null, average log color will be calculated automatically. See hdrIntensity for tiling, this also applies
here.

7.31.2.5 hdrIntensity

CUdeviceptrOptixDenoiserParams::hdrIntensity

average log intensity of input image (default null pointer). points to a single float. if set to null,
autoexposure will be calculated automatically for the input image. Should be set to average log
intensity of the entire image at least if tiling is used to get consistent autoexposure for all tiles.

7.31.2.6 temporalModeUsePreviousLayers

## unsigned int OptixDenoiserParams::temporalModeUsePreviousLayers

In temporal modes this parameter must be set to 1 if previous layers (e.g.

previousOutputInternalGuideLayer) contain valid data. This is the case in the second and subsequent
frames of a sequence (for example after a change of camera angle). In the first frame of such a sequence
this parameter must be set to 0.

## 7.32 OptixDenoiserSizes Struct Reference

#include <optix_types.h>

## Public Attributes

•size_tstateSizeInBytes

•size_twithOverlapScratchSizeInBytes

•size_twithoutOverlapScratchSizeInBytes

•unsigned intoverlapWindowSizeInPixels

•size_tcomputeAverageColorSizeInBytes

•size_tcomputeIntensitySizeInBytes

•size_tinternalGuideLayerPixelSizeInBytes

## 7.32.1 Detailed Description

Various sizes related to the denoiser.

See alsooptixDenoiserComputeMemoryResources( )

## 7.32.2 Member Data Documentation

7.32.2.1 computeAverageColorSizeInBytes

size_t OptixDenoiserSizes::computeAverageColorSizeInBytes

Size of scratch memory passed tooptixDenoiserComputeAverageColor. The size is independent of the
tile/image resolution.

7.32.2.2 computeIntensitySizeInBytes

size_t OptixDenoiserSizes::computeIntensitySizeInBytes

---

Size of scratch memory passed tooptixDenoiserComputeIntensity. The size is independent of the
tile/image resolution.

7.32.2.3 internalGuideLayerPixelSizeInBytes

size_t OptixDenoiserSizes::internalGuideLayerPixelSizeInBytes

Number of bytes for each pixel in internal guide layers.

7.32.2.4 overlapWindowSizeInPixels

unsigned int OptixDenoiserSizes::overlapWindowSizeInPixels

7.32.2.5 stateSizeInBytes

Overlap on all four tile sides.

7.32.2.6 withoutOverlapScratchSizeInBytes

size_t OptixDenoiserSizes::stateSizeInBytes

Size of state memory passed tooptixDenoiserSetup,optixDenoiserInvoke.

7.32.2.7 withOverlapScratchSizeInBytes

size_t OptixDenoiserSizes::withoutOverlapScratchSizeInBytes

Size of scratch memory passed tooptixDenoiserSetup,optixDenoiserInvoke. No overlap added.

size_t OptixDenoiserSizes::withOverlapScratchSizeInBytes

## 7.33 OptixDeviceContextOptions Struct Reference

Size of scratch memory passed tooptixDenoiserSetup,optixDenoiserInvoke. Overlap added to
dimensions passed tooptixDenoiserComputeMemoryResources.

## Public Attributes

#include <optix_types.h>

•OptixLogCallbacklogCallbackFunction

•void *∗* logCallbackData

•intlogCallbackLevel

•OptixDeviceContextValidationModevalidationMode

## 7.33.1 Detailed Description

Parameters used foroptixDeviceContextCreate( )

See alsooptixDeviceContextCreate( )

## 7.33.2 Member Data Documentation

7.33.2.1 logCallbackData

void*∗* OptixDeviceContextOptions::logCallbackData

Pointer stored and passed to logCallbackFunction when a message is generated.

---

7.33.2.2 logCallbackFunction

OptixLogCallbackOptixDeviceContextOptions::logCallbackFunction

Function pointer used when OptiX wishes to generate messages.

7.33.2.3 logCallbackLevel

int OptixDeviceContextOptions::logCallbackLevel

Maximum callback level to generate message for (seeOptixLogCallback)

7.33.2.4 validationMode

OptixDeviceContextValidationModeOptixDeviceContextOptions::validationMode
Validation mode of context.

## 7.34 OptixFunctionTable Struct Reference

#include <optix_function_table.h>

## Public Attributes

Error handling

•const char *∗*(*∗* optixGetErrorName)(OptixResultresult)

•const char *∗*(*∗* optixGetErrorString)(OptixResultresult)

## Device context

•OptixResult(*∗* optixDeviceContextCreate)(CUcontext fromContext, const

OptixDeviceContextOptions *∗*options,OptixDeviceContext *∗*context)

•OptixResult(*∗* optixDeviceContextDestroy)(OptixDeviceContextcontext)

•OptixResult(*∗* optixDeviceContextGetProperty)(OptixDeviceContextcontext,
OptixDevicePropertyproperty, void *∗*value, size_t sizeInBytes)

•OptixResult(*∗* optixDeviceContextSetLogCallback)(OptixDeviceContextcontext,

OptixLogCallbackcallbackFunction, void *∗*callbackData, unsigned int callbackLevel)

•OptixResult(*∗* optixDeviceContextSetCacheEnabled)(OptixDeviceContextcontext, int enabled)

•OptixResult(*∗* optixDeviceContextSetCacheLocation)(OptixDeviceContextcontext, const char
*∗*location)

•OptixResult(*∗* optixDeviceContextSetCacheDatabaseSizes)(OptixDeviceContextcontext, size_t
lowWaterMark, size_t highWaterMark)

•OptixResult(*∗* optixDeviceContextGetCacheEnabled)(OptixDeviceContextcontext, int
*∗*enabled)

•OptixResult(*∗* optixDeviceContextGetCacheLocation)(OptixDeviceContextcontext, char
*∗*location, size_t locationSize)

•OptixResult(*∗* optixDeviceContextGetCacheDatabaseSizes)(OptixDeviceContextcontext, size_
t *∗*lowWaterMark, size_t *∗*highWaterMark)

## Modules

•OptixResult(*∗* optixModuleCreate)(OptixDeviceContextcontext, const

OptixModuleCompileOptions *∗*moduleCompileOptions, constOptixPipelineCompileOptions
*∗*pipelineCompileOptions, const char *∗*input, size_t inputSize, char *∗*logString, size_t
*∗*logStringSize,OptixModule *∗*module)

•OptixResult(*∗* optixModuleCreateWithTasks)(OptixDeviceContextcontext, const
OptixModuleCompileOptions *∗*moduleCompileOptions, constOptixPipelineCompileOptions
*∗*pipelineCompileOptions, const char *∗*input, size_t inputSize, char *∗*logString, size_t
*∗*logStringSize,OptixModule *∗*module,OptixTask *∗*firstTask)

---

• OptixResult(* optixModuleGetCompilationState)(OptixModule module,
OptixModuleCompileState *state)
• OptixResult(* optixModuleCancelCreation)(OptixModule module, OptixCreationFlags flags)
• OptixResult(* optixStub)(void)
• OptixResult(* optixDeviceContextCancelCreations)(OptixDeviceContext context,
OptixCreationFlags flags)
• OptixResult(* optixModuleDestroy)(OptixModule module)
• OptixResult(* optixBuiltinISModuleGet)(OptixDeviceContext context, const
OptixModuleCompileOptions *moduleCompileOptions, const OptixPipelineCompileOptions
*pipelineCompileOptions, const OptixBuiltinISOptions *builtinISOptions, OptixModule
*builtinModule)

## Tasks

• OptixResult(* optixTaskExecute)(OptixTask task, OptixTask *additionalTasks, unsigned int
maxNumAdditionalTasks, unsigned int *numAdditionalTasksCreated)
• OptixResult(* optixTaskGetSerializationKey)(OptixTask task, void *key, size_t *size)
• OptixResult(* optixTaskSerializeOutput)(OptixTask task, void *data, size_t *size)
• OptixResult(* optixTaskDeserializeOutput)(OptixTask task, const void *data, size_t size,
OptixTask *additionalTasks, unsigned int maxNumAdditionalTasks, unsigned int
*numAdditionalTasksCreated)

## Program groups

• OptixResult(* optixProgramGroupCreate)(OptixDeviceContext context, const
OptixProgramGroupDesc *programDescriptions, unsigned int numProgramGroups, const
OptixProgramGroupOptions *options, char *logString, size_t *logStringSize,
OptixProgramGroup *programGroups)
• OptixResult(* optixProgramGroupDestroy)(OptixProgramGroup programGroup)
• OptixResult(* optixProgramGroupGetStackSize)(OptixProgramGroup programGroup,
OptixStackSizes *stackSizes, OptixPipeline pipeline)

## Pipeline

• OptixResult(* optixPipelineCreate)(OptixDeviceContext context, const
OptixPipelineCompileOptions *pipelineCompileOptions, const OptixPipelineLinkOptions
*pipelineLinkOptions, const OptixProgramGroup *programGroups, unsigned int
numProgramGroups, char *logString, size_t *logStringSize, OptixPipeline *pipeline)
• OptixResult(* optixPipelineDestroy)(OptixPipeline pipeline)
• OptixResult(* optixPipelineSetStackSizeFromCallDepths)(OptixPipeline pipeline, unsigned int
maxTraceDepth, unsigned int maxContinuationCallableDepth, unsigned int
maxDirectCallableDepthFromState, unsigned int maxDirectCallableDepthFromTraversal,
unsigned int maxTraversableGraphDepth)
• OptixResult(* optixPipelineSetStackSize)(OptixPipeline pipeline, unsigned int
directCallableStackSizeFromTraversal, unsigned int directCallableStackSizeFromState,
unsigned int continuationStackSize, unsigned int maxTraversableGraphDepth)
• OptixResult(* optixPipelineSymbolMemcpyAsync)(OptixPipeline pipeline, const char *name,
void *mem, size_t sizeInBytes, size_t offsetInBytes, OptixPipelineSymbolMemcpyKind kind,
CUstream stream)

## Acceleration structures

• OptixResult(* optixAccelComputeMemoryUsage)(OptixDeviceContext context, const
OptixAccelBuildOptions *accelOptions, const OptixBuildInput *buildInputs, unsigned int
numBuildInputs, OptixAccelBufferSizes *bufferSizes)

---

• OptixResult(* optixAccelBuild)(OptixDeviceContext context, CUstream stream, const OptixAccelBuildOptions *accelOptions, const OptixBuildInput *buildInputs, unsigned int numBuildInputs, CUdeviceptr tempBuffer, size_t tempBufferSizeInBytes, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle, const OptixAccelEmitDesc *emittedProperties, unsigned int numEmittedProperties)
• OptixResult(* optixAccelGetRelocationInfo)(OptixDeviceContext context, OptixTraversableHandle handle, OptixRelocationInfo *info)
• OptixResult(* optixCheckRelocationCompatibility)(OptixDeviceContext context, const OptixRelocationInfo *info, int *compatible)
• OptixResult(* optixAccelRelocate)(OptixDeviceContext context, CUstream stream, const OptixRelocationInfo *info, const OptixRelocateInput *relocateInputs, size_t numRelocateInputs, CUdeviceptr targetAccel, size_t targetAccelSizeInBytes, OptixTraversableHandle *targetHandle)
• OptixResult(* optixAccelCompact)(OptixDeviceContext context, CUstream stream, OptixTraversableHandle inputHandle, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle)
• OptixResult(* optixAccelEmitProperty)(OptixDeviceContext context, CUstream stream, OptixTraversableHandle handle, const OptixAccelEmitDesc *emittedProperty)
• OptixResult(* optixConvertPointerToTraversableHandle)(OptixDeviceContext onDevice, CUdeviceptr pointer, OptixTraversableType traversableType, OptixTraversableHandle *traversableHandle)
• OptixResult(* optixOpacityMicromapArrayComputeMemoryUsage)(OptixDeviceContext context, const OptixOpacityMicromapArrayBuildInput *buildInput, OptixMicromapBufferSizes *bufferSizes)
• OptixResult(* optixOpacityMicromapArrayBuild)(OptixDeviceContext context, CUstream stream, const OptixOpacityMicromapArrayBuildInput *buildInput, const OptixMicromapBuffers *buffers)
• OptixResult(* optixOpacityMicromapArrayGetRelocationInfo)(OptixDeviceContext context, CUdeviceptr opacityMicromapArray, OptixRelocationInfo *info)
• OptixResult(* optixOpacityMicromapArrayRelocate)(OptixDeviceContext context, CUstream stream, const OptixRelocationInfo *info, CUdeviceptr targetOpacityMicromapArray, size_t targetOpacityMicromapArraySizeInBytes)
• OptixResult(* stub1)(void)
• OptixResult(* stub2)(void)
• OptixResult(* optixClusterAccelComputeMemoryUsage)(OptixDeviceContext context, OptixClusterAccelBuildMode buildMode, const OptixClusterAccelBuildInput *buildInput, OptixAccelBufferSizes *bufferSizes)
• OptixResult(* optixClusterAccelBuild)(OptixDeviceContext context, CUstream stream, const OptixClusterAccelBuildModeDesc *buildModeDesc, const OptixClusterAccelBuildInput *buildInput, CUdeviceptr argsArray, CUdeviceptr argsCount, unsigned int argsStrideInBytes)

## Launch

• OptixResult(* optixSbtRecordPackHeader)(OptixProgramGroup programGroup, void *sbtRecordHeaderHostPointer)
• OptixResult(* optixLaunch)(OptixPipeline pipeline, CUstream stream, CUdeviceptr pipelineParams, size_t pipelineParamsSize, const OptixShaderBindingTable *sbt, unsigned int width, unsigned int height, unsigned int depth)

## Cooperative Vector

• OptixResult(* optixCoopVecMatrixConvert)(OptixDeviceContext context, CUstream stream, unsigned int numNetworks, const OptixNetworkDescription *inputNetworkDescription, CUdeviceptr inputNetworks, size_t inputNetworkStrideInBytes, const OptixNetworkDescription *outputNetworkDescription, CUdeviceptr outputNetworks, size_t outputNetworkStrideInBytes)

---

• OptixResult(* optixCoopVecMatrixComputeSize)(OptixDeviceContext context, unsigned int N, unsigned int K, OptixCoopVecElemType elementType, OptixCoopVecMatrixLayout layout, size_t rowColumnStrideInBytes, size_t *sizeInBytes)

## Denoiser

• OptixResult(* optixDenoiserCreate)(OptixDeviceContext context, OptixDenoiserModelKind modelKind, const OptixDenoiserOptions *options, OptixDenoiser *returnHandle)
• OptixResult(* optixDenoiserDestroy)(OptixDenoiser handle)
• OptixResult(* optixDenoiserComputeMemoryResources)(const OptixDenoiser handle, unsigned int maximumInputWidth, unsigned int maximumInputHeight, OptixDenoiserSizes *returnSizes)
• OptixResult(* optixDenoiserSetup)(OptixDenoiser denoiser, CUstream stream, unsigned int inputWidth, unsigned int inputHeight, CUdeviceptr state, size_t stateSizeInBytes, CUdeviceptr scratch, size_t scratchSizeInBytes)
• OptixResult(* optixDenoiserInvoke)(OptixDenoiser denoiser, CUstream stream, const OptixDenoiserParams *params, CUdeviceptr denoiserState, size_t denoiserStateSizeInBytes, const OptixDenoiserGuideLayer *guideLayer, const OptixDenoiserLayer *layers, unsigned int numLayers, unsigned int inputOffsetX, unsigned int inputOffsetY, CUdeviceptr scratch, size_t scratchSizeInBytes)
• OptixResult(* optixDenoiserComputeIntensity)(OptixDenoiser handle, CUstream stream, const OptixImage2D *inputImage, CUdeviceptr outputIntensity, CUdeviceptr scratch, size_t scratchSizeInBytes)
• OptixResult(* optixDenoiserComputeAverageColor)(OptixDenoiser handle, CUstream stream, const OptixImage2D *inputImage, CUdeviceptr outputAverageColor, CUdeviceptr scratch, size_t scratchSizeInBytes)
• OptixResult(* optixDenoiserCreateWithUserModel)(OptixDeviceContext context, const void *data, size_t dataSizeInBytes, OptixDenoiser *returnHandle)

## 7.34.1 Detailed Description

The function table containing all API functions.

See optixInit() and optixInitWithHandle().

## 7.34.2 Member Data Documentation

## 7.34.2.1 optixAccelBuild

OptixResult(* OptixFunctionTable::optixAccelBuild) (OptixDeviceContext context, CUstream stream, const OptixAccelBuildOptions *accelOptions, const OptixBuildInput *buildInputs, unsigned int numBuildInputs, CUdeviceptr tempBuffer, size_t tempBufferSizeInBytes, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle, const OptixAccelEmitDesc *emittedProperties, unsigned int numEmittedProperties)

See optixAccelBuild().

## 7.34.2.2 optixAccelCompact

OptixResult(* OptixFunctionTable::optixAccelCompact) (OptixDeviceContext context, CUstream stream, OptixTraversableHandle inputHandle, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle)

See optixAccelCompact().

---

## 7.34.2.3 optixAccelComputeMemoryUsage

OptixResult(* OptixFunctionTable::optixAccelComputeMemoryUsage)
(OptixDeviceContext context, const OptixAccelBuildOptions *accelOptions,
const OptixBuildInput *buildInputs, unsigned int numBuildInputs,
OptixAccelBufferSizes *bufferSizes)

See optixAccelComputeMemoryUsage().

## 7.34.2.4 optixAccelEmitProperty

OptixResult(* OptixFunctionTable::optixAccelEmitProperty)
(OptixDeviceContext context, CUstream stream, OptixTraversableHandle handle,
const OptixAccelEmitDesc *emittedProperty)

See optixAccelComputeMemoryUsage().

## 7.34.2.5 optixAccelGetRelocationInfo

OptixResult(* OptixFunctionTable::optixAccelGetRelocationInfo)
(OptixDeviceContext context, OptixTraversableHandle handle,
OptixRelocationInfo *info)

See optixAccelGetRelocationInfo().

## 7.34.2.6 optixAccelRelocate

OptixResult(* OptixFunctionTable::optixAccelRelocate) (OptixDeviceContext
context, CUstream stream, const OptixRelocationInfo *info, const
OptixRelocateInput *relocateInputs, size_t numRelocateInputs, CUdeviceptr
targetAccel, size_t targetAccelSizeInBytes, OptixTraversableHandle
*targetHandle)

See optixAccelRelocate().

## 7.34.2.7 optixBuiltinISModuleGet

OptixResult(* OptixFunctionTable::optixBuiltinISModuleGet)
(OptixDeviceContext context, const OptixModuleCompileOptions
*moduleCompileOptions, const OptixPipelineCompileOptions
*pipelineCompileOptions, const OptixBuiltinISOptions *builtinISOptions,
OptixModule *builtinModule)

See optixBuiltinISModuleGet().

## 7.34.2.8 optixCheckRelocationCompatibility

OptixResult(* OptixFunctionTable::optixCheckRelocationCompatibility)
(OptixDeviceContext context, const OptixRelocationInfo *info, int
*compatible)

See optixCheckRelocationCompatibility().

## 7.34.2.9 optixClusterAccelBuild

OptixResult(* OptixFunctionTable::optixClusterAccelBuild)
(OptixDeviceContext context, CUstream stream, const
OptixClusterAccelBuildModeDesc *buildModeDesc, const
OptixClusterAccelBuildInput *buildInput, CUdeviceptr argsArray, CUdeviceptr

---

## argsCount, unsigned int argsStrideInBytes)

See optixClusterAccelBuild().

## 7.34.2.10 optixClusterAccelComputeMemoryUsage

OptixResult(* OptixFunctionTable::optixClusterAccelComputeMemoryUsage)
(OptixDeviceContext context, OptixClusterAccelBuildMode buildMode, const
OptixClusterAccelBuildInput *buildInput, OptixAccelBufferSizes *bufferSizes)

See optixClusterAccelComputeMemoryUsage().

## 7.34.2.11 optixConvertPointerToTraversableHandle

OptixResult(* OptixFunctionTable::optixConvertPointerToTraversableHandle)
(OptixDeviceContext onDevice, CUdeviceptr pointer, OptixTraversableType
traversableType, OptixTraversableHandle *traversableHandle)

See optixConvertPointerToTraversableHandle().

## 7.34.2.12 optixCoopVecMatrixComputeSize

OptixResult(* OptixFunctionTable::optixCoopVecMatrixComputeSize)
(OptixDeviceContext context, unsigned int N, unsigned int K,
OptixCoopVecElemType elementType, OptixCoopVecMatrixLayout layout, size_t
rowColumnStrideInBytes, size_t *sizeInBytes)

See optixCoopVecMatrixComputeSize().

## 7.34.2.13 optixCoopVecMatrixConvert

OptixResult(* OptixFunctionTable::optixCoopVecMatrixConvert)
(OptixDeviceContext context, CUstream stream, unsigned int numNetworks,
const OptixNetworkDescription *inputNetworkDescription, CUdeviceptr
inputNetworks, size_t inputNetworkStrideInBytes, const
OptixNetworkDescription *outputNetworkDescription, CUdeviceptr
outputNetworks, size_t outputNetworkStrideInBytes)

See optixCoopVecMatrixConvert().

## 7.34.2.14 optixDenoiserComputeAverageColor

OptixResult(* OptixFunctionTable::optixDenoiserComputeAverageColor)
(OptixDenoiser handle, CUstream stream, const OptixImage2D *inputImage,
CUdeviceptr outputAverageColor, CUdeviceptr scratch, size_t
scratchSizeInBytes)

See optixDenoiserComputeAverageColor().

## 7.34.2.15 optixDenoiserComputeIntensity

OptixResult(* OptixFunctionTable::optixDenoiserComputeIntensity)
(OptixDenoiser handle, CUstream stream, const OptixImage2D *inputImage,
CUdeviceptr outputIntensity, CUdeviceptr scratch, size_t scratchSizeInBytes)

See optixDenoiserComputeIntensity().

---

## 7.34.2.16 optixDenoiserComputeMemoryResources

OptixResult(* OptixFunctionTable::optixDenoiserComputeMemoryResources)
(const OptixDenoiser handle, unsigned int maximumInputWidth, unsigned int
maximumInputHeight, OptixDenoiserSizes *returnSizes)

See optixDenoiserComputeMemoryResources().

## 7.34.2.17 optixDenoiserCreate

OptixResult(* OptixFunctionTable::optixDenoiserCreate) (OptixDeviceContext
context, OptixDenoiserModelKind modelKind, const OptixDenoiserOptions
*options, OptixDenoiser *returnHandle)

See optixDenoiserCreate().

## 7.34.2.18 optixDenoiserCreateWithUserModel

OptixResult(* OptixFunctionTable::optixDenoiserCreateWithUserModel)
(OptixDeviceContext context, const void *data, size_t dataSizeInBytes,
OptixDenoiser *returnHandle)

See optixDenoiserCreateWithUserModel().

## 7.34.2.19 optixDenoiserDestroy

OptixResult(* OptixFunctionTable::optixDenoiserDestroy) (OptixDenoiser
handle)

See optixDenoiserDestroy().

## 7.34.2.20 optixDenoiserInvoke

OptixResult(* OptixFunctionTable::optixDenoiserInvoke) (OptixDenoiser
denoiser, CUstream stream, const OptixDenoiserParams *params, CUdeviceptr
denoiserState, size_t denoiserStateSizeInBytes, const
OptixDenoiserGuideLayer *guideLayer, const OptixDenoiserLayer *layers,
unsigned int numLayers, unsigned int inputOffsetX, unsigned int
inputOffsetY, CUdeviceptr scratch, size_t scratchSizeInBytes)

See optixDenoiserInvoke().

## 7.34.2.21 optixDenoiserSetup

OptixResult(* OptixFunctionTable::optixDenoiserSetup) (OptixDenoiser
denoiser, CUstream stream, unsigned int inputWidth, unsigned int
inputHeight, CUdeviceptr state, size_t stateSizeInBytes, CUdeviceptr
scratch, size_t scratchSizeInBytes)

See optixDenoiserSetup().

## 7.34.2.22 optixDeviceContextCancelCreations

OptixResult(* OptixFunctionTable::optixDeviceContextCancelCreations)
(OptixDeviceContext context, OptixCreationFlags flags)

See optixDeviceContextCancelCreations().

---

## 7.34.2.23 optixDeviceContextCreate

OptixResult(* OptixFunctionTable::optixDeviceContextCreate) (CUcontext fromContext, const OptixDeviceContextOptions *options, OptixDeviceContext *context)

See optixDeviceContextCreate().

## 7.34.2.24 optixDeviceContextDestroy

OptixResult(* OptixFunctionTable::optixDeviceContextDestroy)
(OptixDeviceContext context)

See optixDeviceContextDestroy().

## 7.34.2.25 optixDeviceContextGetCacheDatabaseSizes

OptixResult(* OptixFunctionTable::optixDeviceContextGetCacheDatabaseSizes)
(OptixDeviceContext context, size_t *lowWaterMark, size_t *highWaterMark)

See optixDeviceContextGetCacheDatabaseSizes().

## 7.34.2.26 optixDeviceContextGetCacheEnabled

OptixResult(* OptixFunctionTable::optixDeviceContextGetCacheEnabled)
(OptixDeviceContext context, int *enabled)

See optixDeviceContextGetCacheEnabled().

## 7.34.2.27 optixDeviceContextGetCacheLocation

OptixResult(* OptixFunctionTable::optixDeviceContextGetCacheLocation)
(OptixDeviceContext context, char *location, size_t locationSize)

See optixDeviceContextGetCacheLocation().

## 7.34.2.28 optixDeviceContextGetProperty

OptixResult(* OptixFunctionTable::optixDeviceContextGetProperty)
(OptixDeviceContext context, OptixDeviceProperty property, void *value, size _t sizeInBytes)

See optixDeviceContextGetProperty().

## 7.34.2.29 optixDeviceContextSetCacheDatabaseSizes

OptixResult(* OptixFunctionTable::optixDeviceContextSetCacheDatabaseSizes)
(OptixDeviceContext context, size_t lowWaterMark, size_t highWaterMark)

See optixDeviceContextSetCacheDatabaseSizes().

## 7.34.2.30 optixDeviceContextSetCacheEnabled

OptixResult(* OptixFunctionTable::optixDeviceContextSetCacheEnabled)
(OptixDeviceContext context, int enabled)

See optixDeviceContextSetCacheEnabled().

---

## 7.34.2.31 optixDeviceContextSetCacheLocation

OptixResult(* OptixFunctionTable::optixDeviceContextSetCacheLocation)
(OptixDeviceContext context, const char *location)

See optixDeviceContextSetCacheLocation().

## 7.34.2.32 optixDeviceContextSetLogCallback

OptixResult(* OptixFunctionTable::optixDeviceContextSetLogCallback)
(OptixDeviceContext context, OptixLogCallback callbackFunction, void
*callbackData, unsigned int callbackLevel)

See optixDeviceContextSetLogCallback().

## 7.34.2.33 optixGetErrorName

const char *(* OptixFunctionTable::optixGetErrorName) (OptixResult result)

See optixGetErrorName().

## 7.34.2.34 optixGetErrorString

const char *(* OptixFunctionTable::optixGetErrorString) (OptixResult result)

See optixGetErrorString().

## 7.34.2.35 optixLaunch

OptixResult(* OptixFunctionTable::optixLaunch) (OptixPipeline pipeline,
CUstream stream, CUdeviceptr pipelineParams, size_t pipelineParamsSize,
const OptixShaderBindingTable *sbt, unsigned int width, unsigned int height,
unsigned int depth)

See optixConvertPointerToTraversableHandle().

## 7.34.2.36 optixModuleCancelCreation

OptixResult(* OptixFunctionTable::optixModuleCancelCreation) (OptixModule
module, OptixCreationFlags flags)

See optixModuleCancelCreation().

## 7.34.2.37 optixModuleCreate

OptixResult(* OptixFunctionTable::optixModuleCreate) (OptixDeviceContext
context, const OptixModuleCompileOptions *moduleCompileOptions, const
OptixPipelineCompileOptions *pipelineCompileOptions, const char *input, size
_t inputSize, char *logString, size_t *logStringSize, OptixModule *module)

See optixModuleCreate().

## 7.34.2.38 optixModuleCreateWithTasks

OptixResult(* OptixFunctionTable::optixModuleCreateWithTasks)
(OptixDeviceContext context, const OptixModuleCompileOptions
*moduleCompileOptions, const OptixPipelineCompileOptions
*pipelineCompileOptions, const char *input, size_t inputSize, char
*logString, size_t *logStringSize, OptixModule *module, OptixTask

---

## *firstTask)
See optixModuleCreateWithTasks().

## 7.34.2.39 optixModuleDestroy
OptixResult(* OptixFunctionTable::optixModuleDestroy) (OptixModule module)
See optixModuleDestroy().

## 7.34.2.40 optixModuleGetCompilationState
OptixResult(* OptixFunctionTable::optixModuleGetCompilationState)
(OptixModule module, OptixModuleCompileState *state)
See optixModuleGetCompilationState().

## 7.34.2.41 optixOpacityMicromapArrayBuild
OptixResult(* OptixFunctionTable::optixOpacityMicromapArrayBuild)
(OptixDeviceContext context, CUstream stream, const
OptixOpacityMicromapArrayBuildInput *buildInput, const OptixMicromapBuffers
*buffers)
See optixOpacityMicromapArrayBuild().

## 7.34.2.42 optixOpacityMicromapArrayComputeMemoryUsage
OptixResult(* OptixFunctionTable
::optixOpacityMicromapArrayComputeMemoryUsage) (OptixDeviceContext context,
const OptixOpacityMicromapArrayBuildInput *buildInput,
OptixMicromapBufferSizes *bufferSizes)
See optixOpacityMicromapArrayComputeMemoryUsage().

## 7.34.2.43 optixOpacityMicromapArrayGetRelocationInfo
OptixResult(* OptixFunctionTable
::optixOpacityMicromapArrayGetRelocationInfo) (OptixDeviceContext context,
CUdeviceptr opacityMicromapArray, OptixRelocationInfo *info)
See optixOpacityMicromapArrayGetRelocationInfo().

## 7.34.2.44 optixOpacityMicromapArrayRelocate
OptixResult(* OptixFunctionTable::optixOpacityMicromapArrayRelocate)
(OptixDeviceContext context, CUstream stream, const OptixRelocationInfo
*info, CUdeviceptr targetOpacityMicromapArray, size_t
targetOpacityMicromapArraySizeInBytes)
See optixOpacityMicromapArrayRelocate().

## 7.34.2.45 optixPipelineCreate
OptixResult(* OptixFunctionTable::optixPipelineCreate) (OptixDeviceContext
context, const OptixPipelineCompileOptions *pipelineCompileOptions, const
OptixPipelineLinkOptions *pipelineLinkOptions, const OptixProgramGroup
*programGroups, unsigned int numProgramGroups, char *logString, size_t
*logStringSize, OptixPipeline *pipeline)

---

See optixPipelineCreate().

## 7.34.2.46 optixPipelineDestroy
OptixResult(* OptixFunctionTable::optixPipelineDestroy) (OptixPipeline
pipeline)
See optixPipelineDestroy().

## 7.34.2.47 optixPipelineSetStackSize
OptixResult(* OptixFunctionTable::optixPipelineSetStackSize) (OptixPipeline
pipeline, unsigned int directCallableStackSizeFromTraversal, unsigned int
directCallableStackSizeFromState, unsigned int continuationStackSize,
unsigned int maxTraversableGraphDepth)

See optixPipelineSetStackSize().

## 7.34.2.48 optixPipelineSetStackSizeFromCallDepths
OptixResult(* OptixFunctionTable::optixPipelineSetStackSizeFromCallDepths)
(OptixPipeline pipeline, unsigned int maxTraceDepth, unsigned int
maxContinuationCallableDepth, unsigned int maxDirectCallableDepthFromState,
unsigned int maxDirectCallableDepthFromTraversal, unsigned int
maxTraversableGraphDepth)

See optixPipelineSetStackSizeFromCallDepths().

## 7.34.2.49 optixPipelineSymbolMemcpyAsync
## OptixResult(* OptixFunctionTable::optixPipelineSymbolMemcpyAsync)
(OptixPipeline pipeline, const char *name, void *mem, size_t sizeInBytes,
size_t offsetInBytes, OptixPipelineSymbolMemcpyKind kind, CUstream stream)

See optixPipelineCreate().

## 7.34.2.50 optixProgramGroupCreate
OptixResult(* OptixFunctionTable::optixProgramGroupCreate)
(OptixDeviceContext context, const OptixProgramGroupDesc
*programDescriptions, unsigned int numProgramGroups, const
OptixProgramGroupOptions *options, char *logString, size_t *logStringSize,
OptixProgramGroup *programGroups)

See optixProgramGroupCreate().

## 7.34.2.51 optixProgramGroupDestroy
## OptixResult(* OptixFunctionTable::optixProgramGroupDestroy)
(OptixProgramGroup programGroup)

See optixProgramGroupDestroy().

## 7.34.2.52 optixProgramGroupGetStackSize
## OptixResult(* OptixFunctionTable::optixProgramGroupGetStackSize)
(OptixProgramGroup programGroup, OptixStackSizes *stackSizes, OptixPipeline
pipeline)

---

See optixProgramGroupGetStackSize().

## 7.34.2.53 optixSbtRecordPackHeader
OptixResult(* OptixFunctionTable::optixSbtRecordPackHeader)
(OptixProgramGroup programGroup, void *sbtRecordHeaderHostPointer)

See optixConvertPointerToTraversableHandle().

## 7.34.2.54 optixStub
OptixResult(* OptixFunctionTable::optixStub) (void)

See optixModuleCreate().

## 7.34.2.55 optixTaskDeserializeOutput
OptixResult(* OptixFunctionTable::optixTaskDeserializeOutput) (OptixTask
task, const void *data, size_t size, OptixTask *additionalTasks, unsigned
int maxNumAdditionalTasks, unsigned int *numAdditionalTasksCreated)

See optixTaskDeserializeOutput().

## 7.34.2.56 optixTaskExecute
OptixResult(* OptixFunctionTable::optixTaskExecute) (OptixTask task,
OptixTask *additionalTasks, unsigned int maxNumAdditionalTasks, unsigned int
*numAdditionalTasksCreated)

See optixTaskExecute().

## 7.34.2.57 optixTaskGetSerializationKey
OptixResult(* OptixFunctionTable::optixTaskGetSerializationKey) (OptixTask
task, void *key, size_t *size)

See optixTaskGetSerializationKey().

## 7.34.2.58 optixTaskSerializeOutput
OptixResult(* OptixFunctionTable::optixTaskSerializeOutput) (OptixTask task,
void *data, size_t *size)

See optixTaskSerializeOutput().

## 7.34.2.59 stub1
OptixResult(* OptixFunctionTable::stub1) (void)

See optixAccelComputeMemoryUsage().

## 7.34.2.60 stub2
OptixResult(* OptixFunctionTable::stub2) (void)

See optixAccelComputeMemoryUsage().

## 7.35 OptixImage2D Struct Reference
#include <optix_types.h>

---

<u>228</u>

## Public Attributes

•CUdeviceptrdata

•unsigned intwidth

•unsigned intheight

•unsigned introwStrideInBytes

•unsigned intpixelStrideInBytes

•OptixPixelFormatformat

## 7.35.1 Detailed Description

Image descriptor used by the denoiser.

See alsooptixDenoiserInvoke( ),optixDenoiserComputeIntensity( )

## 7.35.2 Member Data Documentation

7.35.2.1 data

CUdeviceptrOptixImage2D::data

7.35.2.2 format

Pointer to the actual pixel data.

OptixPixelFormatOptixImage2D::format

7.35.2.3 height

Pixel format.

unsigned int OptixImage2D::height

Height of the image (in pixels)

7.35.2.4 pixelStrideInBytes

unsigned int OptixImage2D::pixelStrideInBytes

Stride between subsequent pixels of the image (in bytes). If set to 0, dense packing (no gaps) is
assumed. For pixel format OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_LAYER it must be set to
OptixDenoiserSizes::internalGuideLayerPixelSizeInBytes.

7.35.2.5 rowStrideInBytes

unsigned int OptixImage2D::rowStrideInBytes

Stride between subsequent rows of the image (in bytes).

7.35.2.6 width

unsigned int OptixImage2D::width

Width of the image (in pixels)

## 7.36 OptixIncomingHitObject Struct Reference

#include <optix_device.h>

## Public Member Functions

---

•__forceinline__ __device__ floatgetRayTime() const

•__forceinline__ __device__ unsigned intgetTransformListSize() const

•__forceinline__ __device__OptixTraversableHandlegetTransformListHandle(unsigned int
index) const

## 7.36.1 Member Function Documentation

## 7.36.1.1 getRayTime( )

__forceinline__ __device__ float OptixIncomingHitObject::getRayTime ()
const *[inline]*

## 7.36.1.2 getTransformListHandle( )

__forceinline__ __device__OptixTraversableHandleOptixIncomingHitObject
::getTransformListHandle (

unsigned int *index*) const *[inline]*

## 7.36.1.3 getTransformListSize( )

__forceinline__ __device__ unsigned int OptixIncomingHitObject

::getTransformListSize () const *[inline]*

## 7.37 OptixInstance Struct Reference

#include <optix_types.h>

## Public Attributes

•floattransform[12]

•unsigned intinstanceId

•unsigned intsbtOffset

•unsigned intvisibilityMask

•unsigned intflags

•OptixTraversableHandletraversableHandle

•unsigned intpad[2]

## 7.37.1 Detailed Description

Instances.

See alsoOptixBuildInputInstanceArray::instances

## 7.37.2 Member Data Documentation

7.37.2.1 flags

unsigned int OptixInstance::flags

Any combination of OptixInstanceFlags is allowed.

7.37.2.2 instanceId

unsigned int OptixInstance::instanceId

Application supplied ID. The maximal ID can be queried using OPTIX_DEVICE_PROPERTY_LIMIT_
MAX_INSTANCE_ID.

---

7.37.2.3 pad

unsigned int OptixInstance::pad[2]

round up to 80-byte, to ensure 16-byte alignment

7.37.2.4 sbtOffset

unsigned int OptixInstance::sbtOffset

SBT record offset. In a traversable graph with multiple levels of instance acceleration structure (IAS)
objects, offsets are summed together. The maximal SBT offset can be queried using OPTIX_DEVICE_
PROPERTY_LIMIT_MAX_SBT_OFFSET.

7.37.2.5 transform

float OptixInstance::transform[12]

7.37.2.6 traversableHandle

affine object-to-world transformation as 3x4 matrix in row-major layout

OptixTraversableHandleOptixInstance::traversableHandle

Set with an OptixTraversableHandle.

7.37.2.7 visibilityMask

unsigned int OptixInstance::visibilityMask

Visibility mask. If rayMask & instanceMask == 0 the instance is culled. The number of available bits
can be queried using OPTIX_DEVICE_PROPERTY_LIMIT_NUM_BITS_INSTANCE_VISIBILITY_
MASK.

## 7.38 OptixMatrixMotionTransform Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixTraversableHandlechild

•OptixMotionOptionsmotionOptions

•unsigned intpad[3]

•floattransform[2][12]

## 7.38.1 Detailed Description

Represents a matrix motion transformation.

The device address of instances of this type must be a multiple of OPTIX_TRANSFORM_BYTE_
ALIGNMENT.

This struct, as defined here, handles only N=2 motion keys due to the fixed array length of its
transform member. The following example shows how to create instances for an arbitrary number N of
motion keys:

floatmatrixData[N][12];

...// setup matrixData
size_ttransformSizeInBytes =sizeof(OptixMatrixMotionTransform) + (N-2) * 12 *sizeof(float);
OptixMatrixMotionTransform* matrixMoptionTransform = (OptixMatrixMotionTransform*)
malloc(transformSizeInBytes);
memset(matrixMoptionTransform, 0, transformSizeInBytes);

---

...// setup other members of matrixMoptionTransform
matrixMoptionTransform->motionOptions.numKeys
memcpy(matrixMoptionTransform->transform, matrixData, N * 12 *sizeof(float));
...// copy matrixMoptionTransform to device memory
free(matrixMoptionTransform)

See alsooptixConvertPointerToTraversableHandle( )

## 7.38.2 Member Data Documentation

7.38.2.1 child

OptixTraversableHandleOptixMatrixMotionTransform::child

The traversable that is transformed by this transformation.

7.38.2.2 motionOptions

7.38.2.3 pad

OptixMotionOptionsOptixMatrixMotionTransform::motionOptions

The motion options for this transformation. Must have at least two motion keys.

unsigned int OptixMatrixMotionTransform::pad[3]

Padding to make the transformation 16 byte aligned.

7.38.2.4 transform

float OptixMatrixMotionTransform::transform[2][12]

## 7.39 OptixMicromapBuffers Struct Reference

Affine object-to-world transformation as 3x4 matrix in row-major layout.

## Public Attributes

#include <optix_types.h>

•CUdeviceptroutput

•size_toutputSizeInBytes

•CUdeviceptrtemp

•size_ttempSizeInBytes

## 7.39.1 Detailed Description

Buffer inputs for opacity micromap array builds.

## 7.39.2 Member Data Documentation

7.39.2.1 output

CUdeviceptrOptixMicromapBuffers::output

Output buffer.

7.39.2.2 outputSizeInBytes

size_t OptixMicromapBuffers::outputSizeInBytes

---

Output buffer size.

7.39.2.3 temp

CUdeviceptrOptixMicromapBuffers::temp

Temp buffer.

7.39.2.4 tempSizeInBytes

size_t OptixMicromapBuffers::tempSizeInBytes

Temp buffer size.

## 7.40 OptixMicromapBufferSizes Struct Reference

#include <optix_types.h>

## Public Attributes

•size_toutputSizeInBytes

•size_ttempSizeInBytes

## 7.40.1 Detailed Description

Conservative memory requirements for building a opacity micromap array.

## 7.40.2 Member Data Documentation

7.40.2.1 outputSizeInBytes

7.40.2.2 tempSizeInBytes

size_t OptixMicromapBufferSizes::outputSizeInBytes

size_t OptixMicromapBufferSizes::tempSizeInBytes

## 7.41 OptixModuleCompileBoundValueEntry Struct Reference

#include <optix_types.h>

## Public Attributes

•size_tpipelineParamOffsetInBytes

•size_tsizeInBytes

•const void *∗* boundValuePtr

•const char *∗* annotation

## 7.41.1 Detailed Description

Struct for specifying specializations for pipelineParams as specified inOptixPipelineCompileOptions
::pipelineLaunchParamsVariableName.

The bound values are supposed to represent a constant value in the pipelineParams. OptiX will
attempt to locate all loads from the pipelineParams and correlate them to the appropriate bound value,
but there are cases where OptiX cannot safely or reliably do this. For example if the pointer to the
pipelineParams is passed as an argument to a non-inline function or the offset of the load to the
pipelineParams cannot be statically determined (e.g. accessed in a loop). No module should rely on the value being specialized in order to work correctly. The values in the pipelineParams specified on
optixLaunch should match the bound value. If validation mode is enabled on the context, OptiX will
verify that the bound values specified matches the values in pipelineParams specified to optixLaunch.

These values are compiled in to the module as constants. Once the constants are inserted into the code,
an optimization pass will be run that will attempt to propagate the consants and remove unreachable
code.

If caching is enabled, changes in these values will result in newly compiled modules.

The pipelineParamOffset and sizeInBytes must be within the bounds of the pipelineParams variable.
OPTIX_ERROR_INVALID_VALUE will be returned from optixModuleCreate otherwise.

If more than one bound value overlaps or the size of a bound value is equal to 0, an OPTIX_ERROR_
INVALID_VALUE will be returned from optixModuleCreate.

The same set of bound values do not need to be used for all modules in a pipeline, but overlapping
values between modules must have the same value. OPTIX_ERROR_INVALID_VALUE will be
returned from optixPipelineCreate otherwise.

See alsoOptixModuleCompileOptions

## 7.41.2 Member Data Documentation

7.41.2.1 annotation

const char*∗* OptixModuleCompileBoundValueEntry::annotation

7.41.2.2 boundValuePtr

const void*∗* OptixModuleCompileBoundValueEntry::boundValuePtr

7.41.2.3 pipelineParamOffsetInBytes

size_t OptixModuleCompileBoundValueEntry::pipelineParamOffsetInBytes

7.41.2.4 sizeInBytes

size_t OptixModuleCompileBoundValueEntry::sizeInBytes

## 7.42 OptixModuleCompileOptions Struct Reference

#include <optix_types.h>

## Public Attributes

•intmaxRegisterCount

•OptixCompileOptimizationLeveloptLevel

•OptixCompileDebugLeveldebugLevel

•constOptixModuleCompileBoundValueEntry *∗* boundValues

•unsigned intnumBoundValues

•unsigned intnumPayloadTypes

•constOptixPayloadType *∗* payloadTypes

•OptixModulebaseModule

## 7.42.1 Detailed Description

Compilation options for module.

---

See alsooptixModuleCreate( )

## 7.42.2 Member Data Documentation

7.42.2.1 baseModule

OptixModuleOptixModuleCompileOptions::baseModule

If not nullptr, pointer to the base module for potential specialization.

7.42.2.2 boundValues

constOptixModuleCompileBoundValueEntry*∗* OptixModuleCompileOptions
::boundValues

Ingored if numBoundValues is set to 0.

7.42.2.3 debugLevel

OptixCompileDebugLevelOptixModuleCompileOptions::debugLevel
Generate debug information.

7.42.2.4 maxRegisterCount

int OptixModuleCompileOptions::maxRegisterCount

7.42.2.5 numBoundValues

Maximum number of registers allowed when compiling to SASS. Set to 0 for no explicit limit. May
vary within a pipeline.

unsigned int OptixModuleCompileOptions::numBoundValues

7.42.2.6 numPayloadTypes

## unsigned int OptixModuleCompileOptions::numPayloadTypes

The number of different payload types available for compilation. Must be zero if
OptixPipelineCompileOptions::numPayloadValuesis not zero.

7.42.2.7 optLevel

OptixCompileOptimizationLevelOptixModuleCompileOptions::optLevel
Optimization level. May vary within a pipeline.

7.42.2.8 payloadTypes

constOptixPayloadType*∗* OptixModuleCompileOptions::payloadTypes
Points to host array of payload type definitions, size must match numPayloadTypes.

## 7.43 OptixMotionOptions Struct Reference

#include <optix_types.h>

---

## Public Attributes

•unsigned shortnumKeys

•unsigned shortflags

•floattimeBegin

•floattimeEnd

## 7.43.1 Detailed Description

Motion options.

See alsoOptixAccelBuildOptions::motionOptions,OptixMatrixMotionTransform::motionOptions,
OptixSRTMotionTransform::motionOptions

## 7.43.2 Member Data Documentation

7.43.2.1 flags

unsigned short OptixMotionOptions::flags

Combinations ofOptixMotionFlags.

7.43.2.2 numKeys

unsigned short OptixMotionOptions::numKeys

7.43.2.3 timeBegin

If numKeys *>* 1, motion is enabled. timeBegin, timeEnd and flags are all ignored when motion is
disabled.

float OptixMotionOptions::timeBegin

Point in time where motion starts. Must be lesser than timeEnd.

7.43.2.4 timeEnd

float OptixMotionOptions::timeEnd

Point in time where motion ends. Must be greater than timeBegin.

## 7.44 OptixNetworkDescription Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixCoopVecMatrixDescription *∗* layers

•unsigned intnumLayers

## 7.44.1 Member Data Documentation

7.44.1.1 layers

OptixCoopVecMatrixDescription*∗* OptixNetworkDescription::layers

7.44.1.2 numLayers

unsigned int OptixNetworkDescription::numLayers

---

## 7.45 OptixOpacityMicromapArrayBuildInput Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intflags

•CUdeviceptrinputBuffer

•CUdeviceptrperMicromapDescBuffer

•unsigned intperMicromapDescStrideInBytes

•unsigned intnumMicromapHistogramEntries

•constOptixOpacityMicromapHistogramEntry *∗* micromapHistogramEntries

## 7.45.1 Detailed Description

7.45.2.1 flags

Inputs to opacity micromap array construction.

## 7.45.2 Member Data Documentation

unsigned int OptixOpacityMicromapArrayBuildInput::flags

7.45.2.2 inputBuffer

Applies to all opacity micromaps in array.

CUdeviceptrOptixOpacityMicromapArrayBuildInput::inputBuffer

7.45.2.3 micromapHistogramEntries

128B aligned base pointer for raw opacity micromap input data.

constOptixOpacityMicromapHistogramEntry*∗*

OptixOpacityMicromapArrayBuildInput::micromapHistogramEntries

Histogram over opacity micromaps of input format and subdivision combinations. Counts of entries
with equal format and subdivision combination (duplicates) are added together.

7.45.2.4 numMicromapHistogramEntries

unsigned int OptixOpacityMicromapArrayBuildInput

::numMicromapHistogramEntries

Number ofOptixOpacityMicromapHistogramEntry.

7.45.2.5 perMicromapDescBuffer

## CUdeviceptrOptixOpacityMicromapArrayBuildInput::perMicromapDescBuffer

OneOptixOpacityMicromapDescentry per opacity micromap. This device pointer must be a multiple
of OPTIX_OPACITY_MICROMAP_DESC_BYTE_ALIGNMENT.

7.45.2.6 perMicromapDescStrideInBytes

unsigned int OptixOpacityMicromapArrayBuildInput

::perMicromapDescStrideInBytes

Stride between OptixOpacityMicromapDescs in perOmDescBuffer. If set to zero, the opacity micromap
descriptors are assumed to be tightly packed and the stride is assumed to be sizeof(OptixOpacityMicromapDesc). This stride must be a multiple of OPTIX_OPACITY_MICROMAP
_DESC_BYTE_ALIGNMENT.

## 7.46 OptixOpacityMicromapDesc Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intbyteOffset

•unsigned shortsubdivisionLevel

•unsigned shortformat

## 7.46.1 Detailed Description

## 7.46.2 Member Data Documentation

Opacity micromap descriptor.

7.46.2.1 byteOffset

unsigned int OptixOpacityMicromapDesc::byteOffset

7.46.2.2 format

Byte offset to opacity micromap in data input buffer of opacity micromap array build.

unsigned short OptixOpacityMicromapDesc::format

7.46.2.3 subdivisionLevel

OptixOpacityMicromapFormat.

unsigned short OptixOpacityMicromapDesc::subdivisionLevel

∧
Number of micro-triangles is 4 level. Valid levels are [0, 12].

## 7.47 OptixOpacityMicromapHistogramEntry Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intcount

•unsigned intsubdivisionLevel

•OptixOpacityMicromapFormatformat

## 7.47.1 Detailed Description

Opacity micromap histogram entry. Specifies how many opacity micromaps of a specific type are input
to the opacity micromap array build. Note that while this is similar to

OptixOpacityMicromapUsageCount, the histogram entry specifies how many opacity micromaps of a
specific type are combined into a opacity micromap array.

## 7.47.2 Member Data Documentation

7.47.2.1 count

unsigned int OptixOpacityMicromapHistogramEntry::count

---

Number of opacity micromaps with the format and subdivision level that are input to the opacity
micromap array build.

7.47.2.2 format

OptixOpacityMicromapFormatOptixOpacityMicromapHistogramEntry::format
Opacity micromap format.

7.47.2.3 subdivisionLevel

unsigned int OptixOpacityMicromapHistogramEntry::subdivisionLevel
∧
Number of micro-triangles is 4 level. Valid levels are [0, 12].

## 7.48 OptixOpacityMicromapUsageCount Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intcount

•unsigned intsubdivisionLevel

•OptixOpacityMicromapFormatformat

## 7.48.1 Detailed Description

Opacity micromap usage count for acceleration structure builds. Specifies how many opacity
micromaps of a specific type are referenced by triangles when building the AS. Note that while this is
similar toOptixOpacityMicromapHistogramEntry, the usage count specifies how many opacity
micromaps of a specific type are referenced by triangles in the AS.

## 7.48.2 Member Data Documentation

7.48.2.1 count

unsigned int OptixOpacityMicromapUsageCount::count

Number of opacity micromaps with this format and subdivision level referenced by triangles in the
corresponding triangle build input at AS build time.

7.48.2.2 format

OptixOpacityMicromapFormatOptixOpacityMicromapUsageCount::format
opacity micromap format.

7.48.2.3 subdivisionLevel

unsigned int OptixOpacityMicromapUsageCount::subdivisionLevel

∧
Number of micro-triangles is 4 level. Valid levels are [0, 12].

## 7.49 OptixOutgoingHitObject Struct Reference

#include <optix_device.h>

## Public Member Functions

---

•__forceinline__ __device__ floatgetRayTime() const

•__forceinline__ __device__ unsigned intgetTransformListSize() const

•__forceinline__ __device__OptixTraversableHandlegetTransformListHandle(unsigned int
index) const

## 7.49.1 Member Function Documentation

## 7.49.1.1 getRayTime( )

__forceinline__ __device__ float OptixOutgoingHitObject::getRayTime ()
const *[inline]*

## 7.49.1.2 getTransformListHandle( )

__forceinline__ __device__OptixTraversableHandleOptixOutgoingHitObject
::getTransformListHandle (

unsigned int *index*) const *[inline]*

## 7.49.1.3 getTransformListSize( )

__forceinline__ __device__ unsigned int OptixOutgoingHitObject

::getTransformListSize () const *[inline]*

## 7.50 OptixPayloadType Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intnumPayloadValues

## 7.50.1 Detailed Description

•const unsigned int *∗* payloadSemantics

Specifies a single payload type.

## 7.50.2 Member Data Documentation

7.50.2.1 numPayloadValues

unsigned int OptixPayloadType::numPayloadValues

The number of 32b words the payload of this type holds.

7.50.2.2 payloadSemantics

const unsigned int*∗* OptixPayloadType::payloadSemantics

Points to host array of payload word semantics, size must match numPayloadValues.

## 7.51 OptixPipelineCompileOptions Struct Reference

#include <optix_types.h>

## Public Attributes

•intusesMotionBlur

---

•unsigned inttraversableGraphFlags

•intnumPayloadValues

•intnumAttributeValues

•unsigned intexceptionFlags

•const char *∗* pipelineLaunchParamsVariableName

•size_tpipelineLaunchParamsSizeInBytes

•unsigned intusesPrimitiveTypeFlags

•intallowOpacityMicromaps

•intallowClusteredGeometry

## 7.51.1 Detailed Description

Compilation options for all modules of a pipeline.

Similar toOptixModuleCompileOptions, but these options here need to be equal for all modules of a
pipeline.

See alsooptixModuleCreate( ),optixPipelineCreate( )

## 7.51.2 Member Data Documentation

7.51.2.1 allowClusteredGeometry

int OptixPipelineCompileOptions::allowClusteredGeometry

Boolean value indicating whether clusters (cluster acceleration structures) may be used. This value
MUST be set if clusters are present in the BVH, otherwise validation will return an error.

7.51.2.2 allowOpacityMicromaps

int OptixPipelineCompileOptions::allowOpacityMicromaps

Boolean value indicating whether opacity micromaps may be used.

7.51.2.3 exceptionFlags

unsigned int OptixPipelineCompileOptions::exceptionFlags

A bitmask of OptixExceptionFlags indicating which exceptions are enabled.

7.51.2.4 numAttributeValues

int OptixPipelineCompileOptions::numAttributeValues

How much storage, in 32b words, to make available for the attributes. The minimum number is 2.
Values below that will automatically be changed to 2. [2..8].

7.51.2.5 numPayloadValues

int OptixPipelineCompileOptions::numPayloadValues

How much storage, in 32b words, to make available for the payload, [0..32] Must be zero if
numPayloadTypes is not zero.

7.51.2.6 pipelineLaunchParamsSizeInBytes

size_t OptixPipelineCompileOptions::pipelineLaunchParamsSizeInBytes

Size of the variable pointed to by pipelineLaunchParamsVariableName. It will be a compiler error if
the size of the variable pointed to by pipelineLaunchParamsVariableName is not equal to this size.

---

7.51.2.7 pipelineLaunchParamsVariableName

const char*∗* OptixPipelineCompileOptions::pipelineLaunchParamsVariableName

The name of the pipeline parameter variable. If 0, no pipeline parameter will be available. This will be
ignored if the launch param variable was optimized out or was not found in the modules linked to the
pipeline.

7.51.2.8 traversableGraphFlags

unsigned int OptixPipelineCompileOptions::traversableGraphFlags

Traversable graph bitfield. See OptixTraversableGraphFlags.

7.51.2.9 usesMotionBlur

int OptixPipelineCompileOptions::usesMotionBlur

Boolean value indicating whether motion blur could be used.

7.51.2.10 usesPrimitiveTypeFlags

unsigned int OptixPipelineCompileOptions::usesPrimitiveTypeFlags

Bit field enabling primitive types. See OptixPrimitiveTypeFlags. Setting to zero corresponds to
enabling OPTIX_PRIMITIVE_TYPE_FLAGS_CUSTOM and OPTIX_PRIMITIVE_TYPE_FLAGS_
TRIANGLE.

## 7.52 OptixPipelineLinkOptions Struct Reference

#include <optix_types.h>

Public Attributes

•unsigned intmaxTraceDepth

•unsigned intmaxContinuationCallableDepth

•unsigned intmaxDirectCallableDepthFromState

•unsigned intmaxDirectCallableDepthFromTraversal

•unsigned intmaxTraversableGraphDepth

## 7.52.1 Detailed Description

Link options for a pipeline.

See alsooptixPipelineCreate( )

## 7.52.2 Member Data Documentation

7.52.2.1 maxContinuationCallableDepth

unsigned int OptixPipelineLinkOptions::maxContinuationCallableDepth

Maximum depth of continuation callable call graphs. 0 means that continuation callables will not take
part in the stack size calculation and can most likely not be called.

7.52.2.2 maxDirectCallableDepthFromState

unsigned int OptixPipelineLinkOptions::maxDirectCallableDepthFromState

Maximum depth of direct callable call graphs called from raygen, closesthit, miss or continuation callable programs. 0 means that direct callables will not take part in the default stack size calculation
for that part of the pipeline and can not be called from the programs mentioned above if the callable
needs any stack.

7.52.2.3 maxDirectCallableDepthFromTraversal

unsigned int OptixPipelineLinkOptions::maxDirectCallableDepthFromTraversal

Maximum depth of direct callable call graphs called from intersect or anyhit programs. 0 means that
direct callables will not take part in the default stack size calculation for that part of the pipeline and
can not be called from the programs mentioned above if the callable needs any stack.

7.52.2.4 maxTraceDepth

## unsigned int OptixPipelineLinkOptions::maxTraceDepth

7.52.2.5 maxTraversableGraphDepth

Maximum trace recursion depth. 0 means a ray generation program can be launched, but can't trace
any rays. The maximum allowed value is 31.

## unsigned int OptixPipelineLinkOptions::maxTraversableGraphDepth

The maximum depth of a traversable graph passed to trace. 0 means to take a default value based on
the traversableGraphFlags passed toOptixPipelineCompileOptions::traversableGraphFlags: OPTIX_
TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS means to take 1, otherwise 2 will be taken.

## 7.53 OptixProgramGroupCallables Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixModulemoduleDC

•const char *∗* entryFunctionNameDC

•OptixModulemoduleCC

•const char *∗* entryFunctionNameCC

## 7.53.1 Detailed Description

Program group representing callables.

Module and entry function name need to be valid for at least one of the two callables.

See also #OptixProgramGroupDesc::callables

## 7.53.2 Member Data Documentation

7.53.2.1 entryFunctionNameCC

const char*∗* OptixProgramGroupCallables::entryFunctionNameCC

Entry function name of the continuation callable (CC) program.

7.53.2.2 entryFunctionNameDC

const char*∗* OptixProgramGroupCallables::entryFunctionNameDC

Entry function name of the direct callable (DC) program.

---

7.53.2.3 moduleCC

OptixModuleOptixProgramGroupCallables::moduleCC

Module holding the continuation callable (CC) program.

7.53.2.4 moduleDC

OptixModuleOptixProgramGroupCallables::moduleDC
Module holding the direct callable (DC) program.

## 7.54 OptixProgramGroupDesc Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixProgramGroupKindkind

•unsigned intflags

•union {

OptixProgramGroupHitgrouphitgroup

OptixProgramGroupSingleModuleraygen

OptixProgramGroupSingleModulemiss

OptixProgramGroupSingleModuleexception

OptixProgramGroupCallablescallables

};

## 7.54.1 Detailed Description

Descriptor for program groups.

## 7.54.2 Member Data Documentation

## 7.54.2.1

union {...} OptixProgramGroupDesc::@9

7.54.2.2 callables

OptixProgramGroupCallablesOptixProgramGroupDesc::callables

See alsoOPTIX_PROGRAM_GROUP_KIND_CALLABLES

7.54.2.3 exception

OptixProgramGroupSingleModuleOptixProgramGroupDesc::exception

See alsoOPTIX_PROGRAM_GROUP_KIND_EXCEPTION

7.54.2.4 flags

unsigned int OptixProgramGroupDesc::flags

SeeOptixProgramGroupFlags.

---

7.54.2.5 hitgroup

OptixProgramGroupHitgroupOptixProgramGroupDesc::hitgroup

See alsoOPTIX_PROGRAM_GROUP_KIND_HITGROUP

7.54.2.6 kind

OptixProgramGroupKindOptixProgramGroupDesc::kind

The kind of program group.

7.54.2.7 miss

OptixProgramGroupSingleModuleOptixProgramGroupDesc::miss

See alsoOPTIX_PROGRAM_GROUP_KIND_MISS

7.54.2.8 raygen

OptixProgramGroupSingleModuleOptixProgramGroupDesc::raygen

See alsoOPTIX_PROGRAM_GROUP_KIND_RAYGEN

## 7.55 OptixProgramGroupHitgroup Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixModulemoduleCH

•const char *∗* entryFunctionNameCH

•OptixModulemoduleAH

•const char *∗* entryFunctionNameAH

•OptixModulemoduleIS

## 7.55.1 Detailed Description

•const char *∗* entryFunctionNameIS

Program group representing the hitgroup.

For each of the three program types, module and entry function name might both be nullptr.

See alsoOptixProgramGroupDesc::hitgroup

## 7.55.2 Member Data Documentation

7.55.2.1 entryFunctionNameAH

const char*∗* OptixProgramGroupHitgroup::entryFunctionNameAH

Entry function name of the any hit (AH) program.

7.55.2.2 entryFunctionNameCH

const char*∗* OptixProgramGroupHitgroup::entryFunctionNameCH

Entry function name of the closest hit (CH) program.

---

<u>245</u>

7.55.2.3 entryFunctionNameIS

const char*∗* OptixProgramGroupHitgroup::entryFunctionNameIS

Entry function name of the intersection (IS) program.

7.55.2.4 moduleAH

OptixModuleOptixProgramGroupHitgroup::moduleAH

Module holding the any hit (AH) program.

7.55.2.5 moduleCH

OptixModuleOptixProgramGroupHitgroup::moduleCH

Module holding the closest hit (CH) program.

7.55.2.6 moduleIS

OptixModuleOptixProgramGroupHitgroup::moduleIS

Module holding the intersection (Is) program.

## 7.56 OptixProgramGroupOptions Struct Reference

#include <optix_types.h>

## Public Attributes

•constOptixPayloadType *∗* payloadType

## 7.56.1 Detailed Description

Program group options.

See alsooptixProgramGroupCreate( )

## 7.56.2 Member Data Documentation

7.56.2.1 payloadType

constOptixPayloadType*∗* OptixProgramGroupOptions::payloadType

Specifies the payload type of this program group. All programs in the group must support the payload
type (Program support for a type is specified by calling.

See alsooptixSetPayloadTypesor otherwise all types specified in

OptixModuleCompileOptionsare supported). If a program is not available for the requested payload
type,optixProgramGroupCreatereturnsOPTIX_ERROR_PAYLOAD_TYPE_MISMATCH. If the
payloadTypeis left zero, a unique type is deduced. The payload type can be uniquely deduced if there
is exactly one payload type for which all programs in the group are available. If the payload type could
not be deduced uniquelyoptixProgramGroupCreatereturnsOPTIX_ERROR_PAYLOAD_TYPE_
RESOLUTION_FAILED.

## 7.57 OptixProgramGroupSingleModule Struct Reference

#include <optix_types.h>

---

<u>246</u>

## Public Attributes

•OptixModulemodule

•const char *∗* entryFunctionName

## 7.57.1 Detailed Description

Program group representing a single module.

Used for raygen, miss, and exception programs. In case of raygen and exception programs, module
and entry function name need to be valid. For miss programs, module and entry function name might
both be nullptr.

See alsoOptixProgramGroupDesc::raygen,OptixProgramGroupDesc::miss,OptixProgramGroupDesc
::exception

## 7.57.2 Member Data Documentation

7.57.2.1 entryFunctionName

const char*∗* OptixProgramGroupSingleModule::entryFunctionName

Entry function name of the single program.

7.57.2.2 module

## 7.58 OptixRelocateInput Struct Reference

OptixModuleOptixProgramGroupSingleModule::module

Module holding single program.

## Public Attributes

#include <optix_types.h>

•OptixBuildInputTypetype

•union {

OptixRelocateInputInstanceArrayinstanceArray

OptixRelocateInputTriangleArraytriangleArray

};

## 7.58.1 Detailed Description

Relocation inputs.

See alsooptixAccelRelocate( )

## 7.58.2 Member Data Documentation

union {...} OptixRelocateInput::@3

7.58.2.1

7.58.2.2 instanceArray

OptixRelocateInputInstanceArrayOptixRelocateInput::instanceArray

Instance and instance pointer inputs.

---

7.58.2.3 triangleArray

OptixRelocateInputTriangleArrayOptixRelocateInput::triangleArray
Triangle inputs.

7.58.2.4 type

OptixBuildInputTypeOptixRelocateInput::type

The type of the build input to relocate.

## 7.59 OptixRelocateInputInstanceArray Struct Reference

## Public Attributes

#include <optix_types.h>

•unsigned intnumInstances

## 7.59.1 Detailed Description

•CUdeviceptrtraversableHandles

Instance and instance pointer inputs.

See alsoOptixRelocateInput::instanceArray

## 7.59.2 Member Data Documentation

7.59.2.1 numInstances

## unsigned int OptixRelocateInputInstanceArray::numInstances

Number of elements inOptixRelocateInputInstanceArray::traversableHandles. Must match
OptixBuildInputInstanceArray::numInstancesof the source build input.

7.59.2.2 traversableHandles

## CUdeviceptrOptixRelocateInputInstanceArray::traversableHandles

These are the traversable handles of the instances (SeeOptixInstance::traversableHandle) These can be
used when also relocating the instances. No updates to the bounds are performed. Use
optixAccelBuild to update the bounds. 'traversableHandles' may be zero when the traversables are not
relocated (i.e. relocation of an IAS on the source device).

## 7.60 OptixRelocateInputOpacityMicromap Struct Reference

#include <optix_types.h>

## Public Attributes

•CUdeviceptropacityMicromapArray

## 7.60.1 Member Data Documentation

7.60.1.1 opacityMicromapArray

CUdeviceptrOptixRelocateInputOpacityMicromap::opacityMicromapArray

Device pointer to a relocated opacity micromap array used by the source build input array. May be zero when no micromaps where used in the source accel, or the referenced opacity micromaps don't
require relocation (for example relocation of a GAS on the source device).

## 7.61 OptixRelocateInputTriangleArray Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intnumSbtRecords

•OptixRelocateInputOpacityMicromapopacityMicromap

## 7.61.1 Detailed Description

Triangle inputs.

7.61.2.1 numSbtRecords

## 7.61.2 Member Data Documentation

See alsoOptixRelocateInput::triangleArray

## unsigned int OptixRelocateInputTriangleArray::numSbtRecords

Number of sbt records available to the sbt index offset override. Must match
OptixBuildInputTriangleArray::numSbtRecordsof the source build input.

7.61.2.2 opacityMicromap

OptixRelocateInputOpacityMicromapOptixRelocateInputTriangleArray
::opacityMicromap

Opacity micromap inputs.

## 7.62 OptixRelocationInfo Struct Reference

## Public Attributes

#include <optix_types.h>

•unsigned long longinfo[4]

## 7.62.1 Detailed Description

Used to store information related to relocation of optix data structures.

See alsooptixOpacityMicromapArrayGetRelocationInfo( ),optixOpacityMicromapArrayRelocate( ),
optixAccelGetRelocationInfo( ),optixAccelRelocate( ),optixCheckRelocationCompatibility( )

## 7.62.2 Member Data Documentation

7.62.2.1 info

Opaque data, used internally, should not be modified.

unsigned long long OptixRelocationInfo::info[4]

## 7.63 OptixShaderBindingTable Struct Reference

#include <optix_types.h>

---

## Public Attributes

•CUdeviceptrraygenRecord

•CUdeviceptrexceptionRecord

•CUdeviceptrmissRecordBase

•unsigned intmissRecordStrideInBytes

•unsigned intmissRecordCount

•CUdeviceptrhitgroupRecordBase

•unsigned inthitgroupRecordStrideInBytes

•unsigned inthitgroupRecordCount

•CUdeviceptrcallablesRecordBase

•unsigned intcallablesRecordStrideInBytes

•unsigned intcallablesRecordCount

## 7.63.1 Detailed Description

## Describes the shader binding table (SBT)

See alsooptixLaunch( )

## 7.63.2 Member Data Documentation

7.63.2.1 callablesRecordBase

## CUdeviceptrOptixShaderBindingTable::callablesRecordBase

Arrays of SBT records for callable programs. If the base address is not null, the stride and count must
not be zero. If the base address is null, then the count needs to zero. The base address and the stride
must be a multiple of OPTIX_SBT_RECORD_ALIGNMENT.

7.63.2.2 callablesRecordCount

## unsigned int OptixShaderBindingTable::callablesRecordCount

Arrays of SBT records for callable programs. If the base address is not null, the stride and count must
not be zero. If the base address is null, then the count needs to zero. The base address and the stride
must be a multiple of OPTIX_SBT_RECORD_ALIGNMENT.

7.63.2.3 callablesRecordStrideInBytes

## unsigned int OptixShaderBindingTable::callablesRecordStrideInBytes

Arrays of SBT records for callable programs. If the base address is not null, the stride and count must
not be zero. If the base address is null, then the count needs to zero. The base address and the stride
must be a multiple of OPTIX_SBT_RECORD_ALIGNMENT.

7.63.2.4 exceptionRecord

## CUdeviceptrOptixShaderBindingTable::exceptionRecord

Device address of the SBT record of the exception program. The address must be a multiple of OPTIX_
SBT_RECORD_ALIGNMENT.

---

7.63.2.5 hitgroupRecordBase

## CUdeviceptrOptixShaderBindingTable::hitgroupRecordBase

Arrays of SBT records for hit groups. The base address and the stride must be a multiple of OPTIX_SBT
_RECORD_ALIGNMENT.

7.63.2.6 hitgroupRecordCount

## unsigned int OptixShaderBindingTable::hitgroupRecordCount

Arrays of SBT records for hit groups. The base address and the stride must be a multiple of OPTIX_SBT
_RECORD_ALIGNMENT.

7.63.2.7 hitgroupRecordStrideInBytes

## unsigned int OptixShaderBindingTable::hitgroupRecordStrideInBytes

7.63.2.8 missRecordBase

Arrays of SBT records for hit groups. The base address and the stride must be a multiple of OPTIX_SBT
_RECORD_ALIGNMENT.

## CUdeviceptrOptixShaderBindingTable::missRecordBase

Arrays of SBT records for miss programs. The base address and the stride must be a multiple of OPTIX
_SBT_RECORD_ALIGNMENT.

7.63.2.9 missRecordCount

## unsigned int OptixShaderBindingTable::missRecordCount

Arrays of SBT records for miss programs. The base address and the stride must be a multiple of OPTIX
_SBT_RECORD_ALIGNMENT.

7.63.2.10 missRecordStrideInBytes

## unsigned int OptixShaderBindingTable::missRecordStrideInBytes

Arrays of SBT records for miss programs. The base address and the stride must be a multiple of OPTIX
_SBT_RECORD_ALIGNMENT.

7.63.2.11 raygenRecord

## CUdeviceptrOptixShaderBindingTable::raygenRecord

Device address of the SBT record of the ray gen program to start launch at. The address must be a
multiple of OPTIX_SBT_RECORD_ALIGNMENT.

## 7.64 OptixSRTData Struct Reference

#include <optix_types.h>

## Public Attributes

Parameters describing the SRT transformation

•floatsx

•floata

•floatb

•floatpvx

---

•floatsy
•floatc
•floatpvy
•floatsz
•floatpvz
•floatqx
•floatqy
•floatqz
•floatqw
•floattx
•floatty
•floattz

## 7.64.1 Detailed Description

Represents an SRT transformation.

An SRT transformation can represent a smooth rotation with fewer motion keys than a matrix
transformation. Each motion key is constructed from elements taken from a matrix S, a quaternion R,
and a translation T.
 

*sx a b pvx*
 
The scaling matrix *S* =  
 0 *sy c pvy* defines an affine transformation that can include scale,
0 0 *sz pvz*
shear, and a translation. The translation allows to define the pivot point for the subsequent rotation.

$$
S={left[left\begin{matrix}{s x}&{a}&{b}&{p v x}\\ {0}&{s y}&{c}&{p v y}\\ {0}&{0}&{s z}&{p v z}\end{matrix}]}right\,
$$

The quaternion R = [ qx, qy, qz, qw ] describes a rotation with angular component qw = cos(theta/2)
and other components [ qx, qy, qz ] = sin(theta/2) *∗* [ ax, ay, az ] where the axis [ ax, ay, az ] is
normalized.
 

$$
\tt{q W=\ s o((t h e t a/2)}
$$

$$
\tt{Q Z}=s(h e e{/2})*[\it X,\ a\ ,,3\it{Z}]
$$

1 0 0 *tx*
 
The translation matrix *T* =  
0 1 0 *ty* defines another translation that is applied after the rotation.
0 0 1 *tz*

$$
T=\left[\begin{matrix}{1}&{0}&{0}&{t x}\\ {0}&{1}&{0}&{t y}\\ {0}&{0}&{1}&{t z}\\ \end{matrix}\right]
$$

Typically, this translation includes the inverse translation from the matrix S to reverse the translation
for the pivot point for R.

To obtain the effective transformation at time t, the elements of the components of S, R, and T will be
interpolated linearly. The components are then multiplied to obtain the combined transformation C = T
∧
*∗* R *∗* S. The transformation C is the effective object-to-world transformations at time t, and C (-1) is the
effective world-to-object transformation at time t.

$$
\ *,mathrm{!}*\mathrm{S}.
$$

$$
\mathbb{C}^{\wedge}(-1)
$$

See alsoOptixSRTMotionTransform::srtData,optixConvertPointerToTraversableHandle( )

## 7.64.2 Member Data Documentation

7.64.2.1 a

float OptixSRTData::a

7.64.2.2 b

float OptixSRTData::b

$$
7.64.2.3\,\,\,\mathsf{c}
$$

float OptixSRTData::c

---

## 7.64.2.4  pvx
float OptixSRTData::pvx

7.64.2.5  pvy
float OptixSRTData::pvy

## 7.64.2.6  pvz
float OptixSRTData::pvz

7.64.2.7  qw
float OptixSRTData::qw

## 7.64.2.8  qx
float OptixSRTData::qx

## 7.64.2.9  qy
float OptixSRTData::qy

## 7.64.2.10  qz
float OptixSRTData::qz

## 7.64.2.11  sx
float OptixSRTData::sx

## 7.64.2.12  sy
float OptixSRTData::sy

## 7.64.2.13  sz
float OptixSRTData::sz

## 7.64.2.14  tx
float OptixSRTData::tx

## 7.64.2.15  ty
## float OptixSRTData::ty

7.64.2.16  tz
float OptixSRTData::tz

## 7.65 OptixSRTMotionTransform Struct Reference
#include <optix_types.h>

---

## Public Attributes

•OptixTraversableHandlechild
•OptixMotionOptionsmotionOptions
•unsigned intpad[3]
•OptixSRTDatasrtData[2]

## 7.65.1 Detailed Description

Represents an SRT motion transformation.

The device address of instances of this type must be a multiple of OPTIX_TRANSFORM_BYTE_
ALIGNMENT.

This struct, as defined here, handles only N=2 motion keys due to the fixed array length of its srtData
member. The following example shows how to create instances for an arbitrary number N of motion
keys:

OptixSRTDatasrtData[N];
...// setup srtData
size_ttransformSizeInBytes =sizeof(OptixSRTMotionTransform) + (N-2) *sizeof(OptixSRTData);
OptixSRTMotionTransform* srtMotionTransform = (OptixSRTMotionTransform*) malloc(transformSizeInBytes);
memset(srtMotionTransform, 0, transformSizeInBytes);
...// setup other members of srtMotionTransform
srtMotionTransform->motionOptions.numKeys= N;
memcpy(srtMotionTransform->srtData,srtData, N *sizeof(OptixSRTData));
...// copy srtMotionTransform to device memory
free(srtMotionTransform)

See alsooptixConvertPointerToTraversableHandle( )

## 7.65.2 Member Data Documentation

7.65.2.1 child

OptixTraversableHandleOptixSRTMotionTransform::child

The traversable transformed by this transformation.

7.65.2.2 motionOptions

OptixMotionOptionsOptixSRTMotionTransform::motionOptions

The motion options for this transformation Must have at least two motion keys.

7.65.2.3 pad

unsigned int OptixSRTMotionTransform::pad[3]

Padding to make the SRT data 16 byte aligned.

7.65.2.4 srtData

OptixSRTDataOptixSRTMotionTransform::srtData[2]

The actual SRT data describing the transformation.

## 7.66 OptixStackSizes Struct Reference

#include <optix_types.h>

---

## Public Attributes

•unsigned intcssRG

•unsigned intcssMS

•unsigned intcssCH

•unsigned intcssAH

•unsigned intcssIS

•unsigned intcssCC

•unsigned intdssDC

## 7.66.1 Detailed Description

Describes the stack size requirements of a program group.

See alsooptixProgramGroupGetStackSize( )

## 7.66.2 Member Data Documentation

7.66.2.1 cssAH

unsigned int OptixStackSizes::cssAH

Continuation stack size of AH programs in bytes.

7.66.2.2 cssCC

unsigned int OptixStackSizes::cssCC

Continuation stack size of CC programs in bytes.

7.66.2.3 cssCH

unsigned int OptixStackSizes::cssCH

Continuation stack size of CH programs in bytes.

7.66.2.4 cssIS

unsigned int OptixStackSizes::cssIS

Continuation stack size of IS programs in bytes.

7.66.2.5 cssMS

unsigned int OptixStackSizes::cssMS

Continuation stack size of MS programs in bytes.

7.66.2.6 cssRG

Continuation stack size of RG programs in bytes.

unsigned int OptixStackSizes::cssRG

7.66.2.7 dssDC

unsigned int OptixStackSizes::dssDC

Direct stack size of DC programs in bytes.

---

## 7.67 OptixStaticTransform Struct Reference

#include <optix_types.h>

## Public Attributes

•OptixTraversableHandlechild

•unsigned intpad[2]

•floattransform[12]

•floatinvTransform[12]

## 7.67.1 Detailed Description

Static transform.

The device address of instances of this type must be a multiple of OPTIX_TRANSFORM_BYTE_
ALIGNMENT.

See alsooptixConvertPointerToTraversableHandle( )

## 7.67.2 Member Data Documentation

7.67.2.1 child

OptixTraversableHandleOptixStaticTransform::child

The traversable transformed by this transformation.

7.67.2.2 invTransform

float OptixStaticTransform::invTransform[12]

7.67.2.3 pad

Affine world-to-object transformation as 3x4 matrix in row-major layout Must be the inverse of the
transform matrix.

unsigned int OptixStaticTransform::pad[2]

Padding to make the transformations 16 byte aligned.

7.67.2.4 transform

float OptixStaticTransform::transform[12]

Affine object-to-world transformation as 3x4 matrix in row-major layout.

## 7.68 OptixTraverseData Struct Reference

#include <optix_types.h>

## Public Attributes

•unsigned intdata[20]

## 7.68.1 Detailed Description

Hit Object Struct to store the data collected in a hit object during traversal in an internal format using
optixHitObjectGetTraverseData( ). The hit object can be reconstructed using that data at a later
point with optixMakeHitObjectWithTraverseData( ).

---

## 7.68.2 Member Data Documentation

7.68.2.1 data

unsigned int OptixTraverseData::data[20]

## 7.69 OptixUtilDenoiserImageTile Struct Reference

#include <optix_denoiser_tiling.h>

## Public Attributes

•OptixImage2Dinput

•OptixImage2Doutput

•unsigned intinputOffsetX

•unsigned intinputOffsetY

## 7.69.1 Detailed Description

Tile definition.

seeoptixUtilDenoiserSplitImage

## 7.69.2 Member Data Documentation

7.69.2.1 input

OptixImage2DOptixUtilDenoiserImageTile::input

7.69.2.2 inputOffsetX

unsigned int OptixUtilDenoiserImageTile::inputOffsetX

7.69.2.3 inputOffsetY

unsigned int OptixUtilDenoiserImageTile::inputOffsetY

7.69.2.4 output

OptixImage2DOptixUtilDenoiserImageTile::output

## 7.70 optix_internal::TypePack<... > Struct Template Reference

#include <optix_device_impl.h>

## 8 File Documentation

## 8.1 optix_device_impl.h File Reference

## Classes

•structoptix_internal::TypePack<... >

## Namespaces

•namespaceoptix_internal

---

## Macros

• #define OPTIX_DEFINE_optixGetAttribute_BODY(which)
• #define OPTIX_DEFINE_optixGetExceptionDetail_BODY(which)

## Functions

• template<typename... Payload>
  static __forceinline__ __device__ void optixTrace (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float rayTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)

• template<typename... Payload>
  static __forceinline__ __device__ void optixTraverse (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float rayTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)

• template<typename... Payload>
  static __forceinline__ __device__ void optixTrace (OptixPayloadTypeID type, OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float rayTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)

• static __forceinline__ __device__ void optixReorder (unsigned int coherenceHint, unsigned int numCoherenceHintBits)

• static __forceinline__ __device__ void optixReorder()

• template<typename... Payload>
  static __forceinline__ __device__ void optixInvoke (OptixPayloadTypeID type, Payload &... payload)

• static __forceinline__ __device__ void optixMakeHitObject (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float rayTime, unsigned int rayFlags, OptixTraverseData traverseData, const OptixTraversableHandle *transforms, unsigned int numTransforms)

• static __forceinline__ __device__ void optixMakeMissHitObject (unsigned int missSBTIndex, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float rayTime, unsigned int rayFlags)

• static __forceinline__ __device__ void optixMakeNopHitObject()

• static __forceinline__ __device__ void optixHitObjectGetTraverseData (OptixTraverseData *data)

• static __forceinline__ __device__ bool optixHitObjectIsHit()

• static __forceinline__ __device__ bool optixHitObjectIsMiss()

• static __forceinline__ __device__ bool optixHitObjectIsNop()

• static __forceinline__ __device__ unsigned int optixHitObjectGetInstanceId()

• static __forceinline__ __device__ unsigned int optixHitObjectGetInstanceIndex()

• static __forceinline__ __device__ unsigned int optixHitObjectGetPrimitiveIndex()

• static __forceline__ __device__ unsigned int optixHitObjectGetTransformListSize()

• static __forceline__ __device__ OptixTraversableHandle optixHitObjectGetTransformListHandle (unsigned int index)

• static __forceline__ __device__ unsigned int optixHitObjectGetSbtGASIndex ()

---

•static __forceinline__ __device__ unsigned intoptixHitObjectGetHitKind()

•static __forceinline__ __device__ float3optixHitObjectGetWorldRayOrigin()

•static __forceinline__ __device__ float3optixHitObjectGetWorldRayDirection()

•static __forceinline__ __device__ floatoptixHitObjectGetRayTmin()

•static __forceinline__ __device__ floatoptixHitObjectGetRayTmax()

•static __forceinline__ __device__ floatoptixHitObjectGetRayTime()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_0()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_1()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_2()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_3()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_4()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_5()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_6()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_7()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetSbtRecordIndex()

•static __forceinline__ __device__ voidoptixHitObjectSetSbtRecordIndex(unsigned int
sbtRecordIndex)

•static __forceinline__ __device__CUdeviceptroptixHitObjectGetSbtDataPointer()

•static __forceinline__ __device__OptixTraversableHandle

•static __forceinline__ __device__ unsigned intoptixHitObjectGetRayFlags()

optixHitObjectGetGASTraversableHandle()

•static __forceinline__ __device__ voidoptixSetPayload_0(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_1(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_3(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_2(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_4(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_5(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_6(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_7(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_8(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_9(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_10(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_11(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_12(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_13(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_14(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_15(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_16(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_17(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_18(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_20(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_19(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_21(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_22(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_23(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_24(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_25(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_26(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_27(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_28(unsigned int p)

---

• static __forceinline__ __device__ void optixSetPayload_29 (unsigned int p)
• static __forceline__ __device__ void optixSetPayload_30 (unsigned int p)
• static __forceline__ __device__ void optixSetPayload_31 (unsigned int p)
• static __forceline__ __device__ unsigned int optixGetPayload_0 ()
• static __forceline__ __device__ unsigned int optixGetPayload_1 ()
• static __forceline__ __device__ unsigned int optixGetPayload_2 ()
• static __forceline__ __device__ unsigned int optixGetPayload_3 ()
• static __forceline__ __device__ unsigned int optixGetPayload_4 ()
• static __forceline__ __device__ unsigned int optixGetPayload_5 ()
• static __forceline__ __device__ unsigned int optixGetPayload_6 ()
• static __forceline__ __device__ unsigned int optixGetPayload_7 ()
• static __forceline__ __device__ unsigned int optixGetPayload_8 ()
• static __forceline__ __device__ unsigned int optixGetPayload_9 ()
• static __forceline__ __device__ unsigned int optixGetPayload_10 ()
• static __forceline__ __device__ unsigned int optixGetPayload_11 ()
• static __forceline__ __device__ unsigned int optixGetPayload_12 ()
• static __forceline__ __device__ unsigned int optixGetPayload_13 ()
• static __forceline__ __device__ unsigned int optixGetPayload_14 ()
• static __forceline__ __device__ unsigned int optixGetPayload_15 ()
• static __forceline__ __device__ unsigned int optixGetPayload_16 ()
• static __forceline__ __device__ unsigned int optixGetPayload_17 ()
• static __forceline__ __device__ unsigned int optixGetPayload_18 ()
• static __forceline__ __device__ unsigned int optixGetPayload_19 ()
• static __forceline__ __device__ unsigned int optixGetPayload_20 ()
• static __forceline__ __device__ unsigned int optixGetPayload_21 ()
• static __forceline__ __device__ unsigned int optixGetPayload_22 ()
• static __forceline__ __device__ unsigned int optixGetPayload_23 ()
• static __forceline__ __device__ unsigned int optixGetPayload_24 ()
• static __forceline__ __device__ unsigned int optixGetPayload_25 ()
• static __forceline__ __device__ unsigned int optixGetPayload_26 ()
• static __forceline__ __device__ unsigned int optixGetPayload_27 ()
• static __forceline__ __device__ unsigned int optixGetPayload_28 ()
• static __forceline__ __device__ unsigned int optixGetPayload_29 ()
• static __forceline__ __device__ unsigned int optixGetPayload_30 ()
• static __forceline__ __device__ unsigned int optixGetPayload_31 ()
• static __forceline__ __device__ void optixSetPayloadTypes (unsigned int types)
• static __forceline__ __device__ unsigned int optixUndefinedValue ()
• __device__ __forceinline__ unsigned int optixGetRemainingTraceDepth ()
• static __forceline__ __device__ float3 optixGetWorldRayOrigin ()
• static __forceline__ __device__ float3 optixGetWorldRayDirection ()
• static __forceline__ __device__ float3 optixGetObjectRayOrigin ()
• static __forceline__ __device__ float3 optixGetObjectRayDirection ()
• static __forceline__ __device__ float optixGetRayTmin ()
• static __forceline__ __device__ float optixGetRayTmax ()
• static __forceline__ __device__ float optixGetRayTime ()
• static __forceline__ __device__ unsigned int optixGetRayFlags ()
• static __forceline__ __device__ unsigned int optixGetRayVisibilityMask ()
• static __forceline__ __device__ OptixTraversableHandle optixGetInstanceTraversableFromIAS
(OptixTraversableHandle ias, unsigned int instIdx)

---

•static __forceinline__ __device__ voidoptixGetTriangleVertexData(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float3 data[3])

•static __forceinline__ __device__ voidoptixGetTriangleVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float3
data[3])

•static __forceinline__ __device__ voidoptixGetTriangleVertexData(float3 data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetTriangleVertexData(float3 data[3])

•static __forceinline__ __device__ voidoptixGetLinearCurveVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[2])

•static __forceinline__ __device__ voidoptixGetLinearCurveVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[2])

•static __forceinline__ __device__ voidoptixGetLinearCurveVertexData(float4 data[2])

•static __forceinline__ __device__ voidoptixHitObjectGetLinearCurveVertexData(float4 data[2])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexData
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexData(float4 data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetQuadraticBSplineVertexData(float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineRocapsVertexData(float4
data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetQuadraticBSplineRocapsVertexData
(float4 data[3])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBSplineVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBSplineRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCatmullRomVertexData(float4 data[4])

---

•static __forceinline__ __device__ voidoptixGetCatmullRomRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCatmullRomRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBezierVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBezierRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetRibbonVertexData(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[3])

•static __forceinline__ __device__ voidoptixGetRibbonVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetRibbonVertexData(float4 data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetRibbonVertexData(float4 data[3])

•static __forceinline__ __device__ float3optixGetRibbonNormal(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float2 ribbonParameters)

•static __forceinline__ __device__ float3optixGetRibbonNormalFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float2
ribbonParameters)

•static __forceinline__ __device__ float3optixGetRibbonNormal(float2 ribbonParameters)

•static __forceinline__ __device__ float3optixHitObjectGetRibbonNormal(float2
ribbonParameters)

•static __forceinline__ __device__ voidoptixGetSphereData(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[1])

•static __forceinline__ __device__ voidoptixGetSphereDataFromHandle(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[1])

•static __forceinline__ __device__ voidoptixGetSphereData(float4 data[1])

•static __forceinline__ __device__ voidoptixHitObjectGetSphereData(float4 data[1])

•static __forceinline__ __device__OptixTraversableHandleoptixGetGASTraversableHandle()

•static __forceinline__ __device__ floatoptixGetGASMotionTimeBegin(OptixTraversableHandle
handle)

•static __forceinline__ __device__ floatoptixGetGASMotionTimeEnd(OptixTraversableHandle
handle)

•static __forceinline__ __device__ unsigned intoptixGetGASMotionStepCount
(OptixTraversableHandlehandle)

•template<typename HitState >

static __forceinline__ __device__ voidoptixGetWorldToObjectTransformMatrix(const HitState
&hs, float m[12])

•static __forceinline__ __device__ voidoptixGetWorldToObjectTransformMatrix(float m[12])

---

• static __forceinline__ __device__ void optixHitObjectGetWorldToObjectTransformMatrix (float m[12])
• template<typename HitState >
static __forceline__ __device__ void optixGetObjectToWorldTransformMatrix (const HitState &hs, float m[12])
• static __forceline__ __device__ void optixGetObjectToWorldTransformMatrix (float m[12])
• static __forceline__ __device__ void optixHitObjectGetObjectToWorldTransformMatrix (float m[12])
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformPointFromWorldToObjectSpace (const HitState &hs, float3 point)
• static __forceline__ __device__ float3 optixTransformPointFromWorldToObjectSpace (float3 point)
• static __forceline__ __device__ float3 optixHitObjectTransformPointFromWorldToObjectSpace (float3 point)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace (const HitState &hs, float3 vec)
• static __forceline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace (float3 vec)
• static __forceline__ __device__ float3 optixHitObjectTransformVectorFromWorldToObjectSpace (float3 vec)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformNormalFromWorldToObjectSpace (const HitState &hs, float3 normal)
• static __forceline__ __device__ float3 optixTransformNormalFromWorldToObjectSpace (float3 normal)
• static __forceline__ __device__ float3 optixHitObjectTransformNormalFromWorldToObjectSpace (float3 normal)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformPointFromObjectToWorldSpace (const HitState &hs, float3 point)
• static __forceline__ __device__ float3 optixTransformPointFromObjectToWorldSpace (float3 point)
• static __forceline__ __device__ float3 optixHitObjectTransformPointFromObjectToWorldSpace (float3 point)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace (const HitState &hs, float3 vec)
• static __forceline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace (float3 vec)
• static __forceline__ __device__ float3 optixHitObjectTransformVectorFromObjectToWorldSpace (float3 vec)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace (const HitState &hs, float3 normal)
• static __forceline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace (float3 normal)
• static __forceline__ __device__ float3 optixHitObjectTransformNormalFromObjectToWorldSpace (float3 normal)
• static __forceline__ __device__ unsigned int optixGetTransformListSize ()

---

<u>263</u>

•static __forceinline__ __device__OptixTraversableHandleoptixGetTransformListHandle
(unsigned int index)

•static __forceinline__ __device__OptixTransformTypeoptixGetTransformTypeFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ constOptixStaticTransform *∗*
optixGetStaticTransformFromHandle(OptixTraversableHandlehandle)

•static __forceinline__ __device__ constOptixSRTMotionTransform *∗*
optixGetSRTMotionTransformFromHandle(OptixTraversableHandlehandle)

•static __forceinline__ __device__ constOptixMatrixMotionTransform *∗*
optixGetMatrixMotionTransformFromHandle(OptixTraversableHandlehandle)

•static __forceinline__ __device__ unsigned intoptixGetInstanceIdFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__OptixTraversableHandleoptixGetInstanceChildFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ const float4 *∗* optixGetInstanceTransformFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ const float4 *∗* optixGetInstanceInverseTransformFromHandle
(OptixTraversableHandlehandle)

•static __device__ __forceinline__CUdeviceptroptixGetGASPointerFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5, unsigned int a6)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5, unsigned int a6, unsigned int a7)

•static __forceinline__ __device__ unsigned intoptixGetAttribute_0()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_1()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_2()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_3()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_4()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_5()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_6()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_7()

•static __forceinline__ __device__ voidoptixTerminateRay()

•static __forceinline__ __device__ voidoptixIgnoreIntersection()

•static __forceinline__ __device__ unsigned intoptixGetPrimitiveIndex()

---

• static __forceinline__ __device__ unsigned int optixGetClusterId ()
• static __forceline__ __device__ unsigned int optixHitObjectGetClusterId ()
• static __forceline__ __device__ unsigned int optixGetSbtGASIndex ()
• static __forceline__ __device__ unsigned int optixGetInstanceId ()
• static __forceline__ __device__ unsigned int optixGetInstanceIndex ()
• static __forceline__ __device__ unsigned int optixGetHitKind ()
• static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType (unsigned int hitKind)
• static __forceline__ __device__ bool optixIsBackFaceHit (unsigned int hitKind)
• static __forceline__ __device__ bool optixIsFrontFaceHit (unsigned int hitKind)
• static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType ()
• static __forceline__ __device__ bool optixIsBackFaceHit ()
• static __forceline__ __device__ bool optixIsFrontFaceHit ()
• static __forceline__ __device__ bool optixIsTriangleHit ()
• static __forceline__ __device__ bool optixIsTriangleFrontFaceHit ()
• static __forceline__ __device__ bool optixIsTriangleBackFaceHit ()
• static __forceline__ __device__ float optixGetCurveParameter ()
• static __forceline__ __device__ float optixHitObjectGetCurveParameter ()
• static __forceline__ __device__ float2 optixGetRibbonParameters ()
• static __forceline__ __device__ float2 optixHitObjectGetRibbonParameters ()
• static __forceline__ __device__ float2 optixGetTriangleBarycentrics ()
• static __forceline__ __device__ float2 optixHitObjectGetTriangleBarycentrics ()
• static __forceline__ __device__ uint3 optixGetLaunchIndex ()
• static __forceline__ __device__ uint3 optixGetLaunchDimensions ()
• static __forceline__ __device__ CUdeviceptr optixGetSbtDataPointer ()
• static __forceline__ __device__ void optixThrowException (int exceptionCode)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4, unsigned int exceptionDetail5, unsigned int exceptionDetail6)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4, unsigned int exceptionDetail5, unsigned int exceptionDetail6, unsigned int exceptionDetail7)
• static __forceline__ __device__ int optixGetExceptionCode ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_0 ()

---

• static __forceinline__ __device__ unsigned int optixGetExceptionDetail_1 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_2 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_3 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_4 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_5 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_6 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_7 ()
• static __forceline__ __device__ char * optixGetExceptionLineInfo ()
• template<typename ReturnT , typename... ArgTypes>
static __forceline__ __device__ ReturnT optixDirectCall (unsigned int sbtIndex, ArgTypes...
args)
• template<typename ReturnT , typename... ArgTypes>
static __forceline__ __device__ ReturnT optixContinuationCall (unsigned int sbtIndex,
ArgTypes... args)
• static __forceline__ __device__ uint4 optixTexFootprint2D (unsigned long long tex, unsigned
int texInfo, float x, float y, unsigned int *singleMipLevel)
• static __forceline__ __device__ uint4 optixTexFootprint2DGrad (unsigned long long tex,
unsigned int texInfo, float x, float y, float dPdx_x, float dPdx_y, float dPdy_x, float dPdy_y, bool
coarse, unsigned int *singleMipLevel)
• static __forceline__ __device__ uint4 optixTexFootprint2DLod (unsigned long long tex,
unsigned int texInfo, float x, float y, float level, bool coarse, unsigned int *singleMipLevel)

## 8.1.1 Detailed Description

OptiX public API.

Author
NVIDIA Corporation

OptiX public API Reference - Device side implementation

## 8.1.2 Macro Definition Documentation

## 8.1.2.1 OPTIX_DEFINE_optixGetAttribute_BODY

#define OPTIX_DEFINE_optixGetAttribute_BODY(
which )

Value:
unsigned int ret;
\ asm("call (%0), _optix_get_attribute_" #which ", ();" : "=r"(ret) :);
\ return ret;

## 8.1.2.2 OPTIX_DEFINE_optixGetExceptionDetail_BODY

#define OPTIX_DEFINE_optixGetExceptionDetail_BODY(
which )

Value:
unsigned int ret;
\ asm("call (%0), _optix_get_exception_detail_" #which ", ();" : "=r"(ret) :);
\ return ret;

---

## 8.1.3 Function Documentation

## 8.1.3.1 optixContinuationCall()

template<typename ReturnT , typename... ArgTypes>
static __forceinline__ __device__ ReturnT optixContinuationCall (
    unsigned int sbtIndex,
    ArgTypes... args ) [static]

## 8.1.3.2 optixDirectCall()

template<typename ReturnT , typename... ArgTypes>
static __forceinline__ __device__ ReturnT optixDirectCall (
    unsigned int sbtIndex,
    ArgTypes... args ) [static]

## 8.1.3.3 optixGetAttribute_0()
static __forceinline__ __device__ unsigned int optixGetAttribute_0 ( ) [static]

## 8.1.3.4 optixGetAttribute_1()
static __forceinline__ __device__ unsigned int optixGetAttribute_1 ( ) [static]

## 8.1.3.5 optixGetAttribute_2()
static __forceinline__ __device__ unsigned int optixGetAttribute_2 ( ) [static]

## 8.1.3.6 optixGetAttribute_3()
static __forceinline__ __device__ unsigned int optixGetAttribute_3 ( ) [static]

## 8.1.3.7 optixGetAttribute_4()
static __forceinline__ __device__ unsigned int optixGetAttribute_4 ( ) [static]

## 8.1.3.8 optixGetAttribute_5()
static __forceinline__ __device__ unsigned int optixGetAttribute_5 ( ) [static]

## 8.1.3.9 optixGetAttribute_6()
static __forceinline__ __device__ unsigned int optixGetAttribute_6 ( ) [static]

## 8.1.3.10 optixGetAttribute_7()
static __forceinline__ __device__ unsigned int optixGetAttribute_7 ( ) [static]

## 8.1.3.11 optixGetCatmullRomRocapsVertexData()
static __forceinline__ __device__ void optixGetCatmullRomRocapsVertexData (
    float4 data[4] ) [static]

---

<u>267</u>

## 8.1.3.12 optixGetCatmullRomRocapsVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCatmullRomRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*
8.1.3.13 optixGetCatmullRomVertexData( ) [1/2]
static __forceinline__ __device__ void optixGetCatmullRomVertexData (
float4 *data[4]*) *[static]*
8.1.3.14 optixGetCatmullRomVertexData( ) [2/2]
static __forceinline__ __device__ void optixGetCatmullRomVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*
8.1.3.15 optixGetCatmullRomVertexDataFromHandle( )
static __forceinline__ __device__ void
optixGetCatmullRomVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*
8.1.3.16 optixGetClusterId( )
static __forceinline__ __device__ unsigned int optixGetClusterId () *[static]*
8.1.3.17 optixGetCubicBezierRocapsVertexData( )
static __forceinline__ __device__ void optixGetCubicBezierRocapsVertexData (
float4 *data[4]*) *[static]*
8.1.3.18 optixGetCubicBezierRocapsVertexDataFromHandle( )
static __forceinline__ __device__ void
optixGetCubicBezierRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*

---

<u>268</u>

unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

## 8.1.3.19 optixGetCubicBezierVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetCubicBezierVertexData (
float4 *data[4]*) *[static]*

## 8.1.3.20 optixGetCubicBezierVertexData( ) [2/2]

static __forceinline__ __device__ void optixGetCubicBezierVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

## 8.1.3.21 optixGetCubicBezierVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCubicBezierVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

## 8.1.3.22 optixGetCubicBSplineRocapsVertexData( )

static __forceinline__ __device__ void optixGetCubicBSplineRocapsVertexData
(
float4 *data[4]*) *[static]*

## 8.1.3.23 optixGetCubicBSplineRocapsVertexDataFromHandle( )

static __forceinline__ __device__ void
optixGetCubicBSplineRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[4]*) *[static]*

## 8.1.3.24 optixGetCubicBSplineVertexData( ) [1/2]

static __forceinline__ __device__ void optixGetCubicBSplineVertexData (
float4 *data[4]*) *[static]*

---

## 8.1.3.25 optixGetCubicBSplineVertexData() [2/2]

static __forceinline__ __device__ void optixGetCubicBSplineVertexData (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float4 data[4] ) [static]

## 8.1.3.26 optixGetCubicBSplineVertexDataFromHandle()

static __forceinline__ __device__ void
optixGetCubicBSplineVertexDataFromHandle (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float4 data[4] ) [static]

## 8.1.3.27 optixGetCurveParameter()
static __forceinline__ __device__ float optixGetCurveParameter ( ) [static]

## 8.1.3.28 optixGetExceptionCode()
static __forceinline__ __device__ int optixGetExceptionCode ( ) [static]

## 8.1.3.29 optixGetExceptionDetail_0()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_0 ( )
[static]

## 8.1.3.30 optixGetExceptionDetail_1()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_1 ( )
[static]

## 8.1.3.31 optixGetExceptionDetail_2()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_2 ( )
[static]

## 8.1.3.32 optixGetExceptionDetail_3()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_3 ( )
[static]

## 8.1.3.33 optixGetExceptionDetail_4()
static __forceinline__ __device__ unsigned int optixGetExceptionDetail_4 ( )
[static]

---

270

8.1.3.34 optixGetExceptionDetail_5()

static __forceinline__ __device__ uns [static]

gned int optixGetExceptionDetail_5 ( )

8.1.3.35 optixGetExceptionDetail_6()

static __forceinline__
[static]

__device__ unsigned int optixGetExceptionDetail_6 ( )

8.1.3.36 optixGetExceptionDetail_7()

static __forceinline__ __device
[*static*]

__ unsigned int optixGetExceptionDetail_7 ( )

8.1.3.37 optixGetExceptionLineInfo()

static __forceinline__ __device__ char * optixGetExceptionLineInfo ( ) [static]

8.1.3.38 optixGetGASMotionStepCount()

static __forceinline__ __device__ unsigned int optixG
OptixTraversableHandle handle ) [static]

tGASMotionStepCount (

8.1.3.39 optixGetGASMotionTimeBegin()

static __forceinline__ _
OptixTraversab

device__ float optixGetGASMotionTimeBegin ( eHandle handle ) [static]

8.1.3.40 optixGetGASMotionTimeEnd()

```cpp
static __forceinline__ __device__ float optixGe
    OptixTraversableHandle handle ) [static]

GASMotionTimeEnd (

8.1.3.41 optixGetGASPointerFromHandle()

static __device__ __forcein
OptixTraversableH

```c
ine__ CUdeviceptr optixGetGASPointerFromHandle (
handle handle ) [static]

8.1.3.42 optixGetGASTraversableHandle()

static __forceinline__
optixGetGASTraversable

__device__ OptixTraversableHandle
andle ( ) [static]

8.1.3.43 optixGetHitKind()

static __forceinline__ __device__ unsigned int optixGetHitKind ( ) [static]

8.1.3.44 optixGetInstanceChildFromHandle()

| static | __forceinline__ | __device__ | OptixTraversableHandle |
| --- | --- | --- | --- |
| optixGetInstanceChildFromHandle( |  |  |  |
| OptixTraversableHandle handle ) [static] |  |  |  |

---

## 8.1.3.45 optixGetInstanceId()
static __forceinline__ __device__ unsigned int optixGetInstanceId ( ) [static]

## 8.1.3.46 optixGetInstanceIdFromHandle()
static __forceinline__ __device__ unsigned int optixGetInstanceIdFromHandle (
OptixTraversableHandle handle ) [static]

## 8.1.3.47 optixGetInstanceIndex()
static __forceinline__ __device__ unsigned int optixGetInstanceIndex ( )
[static]

## 8.1.3.48 optixGetInstanceInverseTransformFromHandle()
static __forceinline__ __device__ const float4 *
optixGetInstanceInverseTransformFromHandle (
OptixTraversableHandle handle ) [static]

## 8.1.3.49 optixGetInstanceTransformFromHandle()
static __forceinline__ __device__ const float4 *
optixGetInstanceTransformFromHandle (
OptixTraversableHandle handle ) [static]

## 8.1.3.50 optixGetInstanceTraversableFromIAS()
static __forceinline__ __device__ OptixTraversableHandle
optixGetInstanceTraversableFromIAS (
OptixTraversableHandle ias,
unsigned int instIdx ) [static]

## 8.1.3.51 optixGetLaunchDimensions()
static __forceinline__ __device__ uint3 optixGetLaunchDimensions ( ) [static]

## 8.1.3.52 optixGetLaunchIndex()
static __forceinline__ __device__ uint3 optixGetLaunchIndex ( ) [static]

## 8.1.3.53 optixGetLinearCurveVertexData() [1/2]
static __forceinline__ __device__ void optixGetLinearCurveVertexData (
float4 data[2]) [static]

## 8.1.3.54 optixGetLinearCurveVertexData() [2/2]
static __forceinline__ __device__ void optixGetLinearCurveVertexData (
OptixTraversableHandle gas,
unsigned int primIdx, unsigned int sbtGASIndex,
float time,
float4 data[2] ) [static]

## 8.1.3.55 optixGetLinearCurveVertexDataFromHandle()

static __forceinline__ __device__ void
optixGetLinearCurveVertexDataFromHandle (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float4 data[2] ) [static]

## 8.1.3.56 optixGetMatrixMotionTransformFromHandle()

static __forceinline__ __device__ const OptixMatrixMotionTransform *
optixGetMatrixMotionTransformFromHandle (
    OptixTraversableHandle handle ) [static]

8.1.3.57 optixGetObjectRayDirection()

static __forceinline__ __device__ float3 optixGetObjectRayDirection ( )
[static]

8.1.3.58 optixGetObjectRayOrigin()

static __forceinline__ __device__ float3 optixGetObjectRayOrigin ( ) [static]

8.1.3.59 optixGetObjectToWorldTransformMatrix() [1/2]

template<typename HitState >
static __forceinline__ __device__ void optixGetObjectToWorldTransformMatrix(
    const HitState & hs,
    float m[12] ) [static]

8.1.3.60 optixGetObjectToWorldTransformMatrix() [2/2]

static __forceinline__ __device__ void optixGetObjectToWorldTransformMatrix(
    float m[12] ) [static]

8.1.3.61 optixGetPayload_0()
static __forceinline__ __device__ unsigned int optixGetPayload_0 ( ) [static]

8.1.3.62 optixGetPayload_1()
static __forceinline__ __device__ unsigned int optixGetPayload_1 ( ) [static]

---

8.1.3.63 optixGetPayload_10()
static __forceinline__ __device__ unsigned int optixGetPayload_10 ( ) [static]

8.1.3.64 optixGetPayload_11()
static __forceinline__ __device__ unsigned int optixGetPayload_11 ( ) [static]

8.1.3.65 optixGetPayload_12()
static __forceinline__ __device__ unsigned int optixGetPayload_12 ( ) [static]

8.1.3.66 optixGetPayload_13()
static __forceinline__ __device__ unsigned int optixGetPayload_13 ( ) [static]

8.1.3.67 optixGetPayload_14()
static __forceinline__ __device__ unsigned int optixGetPayload_14 ( ) [static]

8.1.3.68 optixGetPayload_15()
static __forceinline__ __device__ unsigned int optixGetPayload_15 ( ) [static]

8.1.3.69 optixGetPayload_16()
static __forceinline__ __device__ unsigned int optixGetPayload_16 ( ) [static]

8.1.3.70 optixGetPayload_17()
static __forceinline__ __device__ unsigned int optixGetPayload_17 ( ) [static]

8.1.3.71 optixGetPayload_18()
static __forceinline__ __device__ unsigned int optixGetPayload_18 ( ) [static]

8.1.3.72 optixGetPayload_19()
static __forceinline__ __device__ unsigned int optixGetPayload_19 ( ) [static]

8.1.3.73 optixGetPayload_2()
static __forceinline__ __device__ unsigned int optixGetPayload_2 ( ) [static]

8.1.3.74 optixGetPayload_20()
static __forceinline__ __device__ unsigned int optixGetPayload_20 ( ) [static]

8.1.3.75 optixGetPayload_21()
static __forceinline__ __device__ unsigned int optixGetPayload_21 ( ) [static]

8.1.3.76 optixGetPayload_22()
static __forceinline__ __device__ unsigned int optixGetPayload_22 ( ) [static]

---

8.1.3.77 optixGetPayload_23()
static __forceinline__ __device__ unsigned int optixGetPayload_23 ( ) [static]

8.1.3.78 optixGetPayload_24()
static __forceinline__ __device__ unsigned int optixGetPayload_24 ( ) [static]

8.1.3.79 optixGetPayload_25()
static __forceinline__ __device__ unsigned int optixGetPayload_25 ( ) [static]

8.1.3.80 optixGetPayload_26()
static __forceinline__ __device__ unsigned int optixGetPayload_26 ( ) [static]

8.1.3.81 optixGetPayload_27()
static __forceinline__ __device__ unsigned int optixGetPayload_27 ( ) [static]

8.1.3.82 optixGetPayload_28()
static __forceinline__ __device__ unsigned int optixGetPayload_28 ( ) [static]

8.1.3.83 optixGetPayload_29()
static __forceinline__ __device__ unsigned int optixGetPayload_29 ( ) [static]

8.1.3.84 optixGetPayload_3()
static __forceinline__ __device__ unsigned int optixGetPayload_3 ( ) [static]

8.1.3.85 optixGetPayload_30()
static __forceinline__ __device__ unsigned int optixGetPayload_30 ( ) [static]

8.1.3.86 optixGetPayload_31()
static __forceinline__ __device__ unsigned int optixGetPayload_31 ( ) [static]

8.1.3.87 optixGetPayload_4()
static __forceinline__ __device__ unsigned int optixGetPayload_4 ( ) [static]

8.1.3.88 optixGetPayload_5()
static __forceinline__ __device__ unsigned int optixGetPayload_5 ( ) [static]

8.1.3.89 optixGetPayload_6()
static __forceinline__ __device__ unsigned int optixGetPayload_6 ( ) [static]

## 8.1.3.90 optixGetPayload_7()
static __forceinline__ __device__ unsigned int optixGetPayload_7 ( ) [static]

---

8.1.3.91 optixGetPayload_8( )
static __forceinline__ __device__ unsigned int optixGetPayload_8 () *[static]*
8.1.3.92 optixGetPayload_9( )
static __forceinline__ __device__ unsigned int optixGetPayload_9 () *[static]*
8.1.3.93 optixGetPrimitiveIndex( )
static __forceinline__ __device__ unsigned int optixGetPrimitiveIndex ()
*[static]*
8.1.3.94 optixGetPrimitiveType( ) [1/2]
static __forceinline__ __device__OptixPrimitiveTypeoptixGetPrimitiveType (
) *[static]*
8.1.3.95 optixGetPrimitiveType( ) [2/2]
static __forceinline__ __device__OptixPrimitiveTypeoptixGetPrimitiveType (
unsigned int *hitKind*) *[static]*
8.1.3.96 optixGetQuadraticBSplineRocapsVertexData( )
static __forceinline__ __device__ void
optixGetQuadraticBSplineRocapsVertexData (
float4 *data[3]*) *[static]*
8.1.3.97 optixGetQuadraticBSplineRocapsVertexDataFromHandle( )
static __forceinline__ __device__ void
optixGetQuadraticBSplineRocapsVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*
8.1.3.98 optixGetQuadraticBSplineVertexData( ) [1/2]
static __forceinline__ __device__ void optixGetQuadraticBSplineVertexData (
float4 *data[3]*) *[static]*
8.1.3.99 optixGetQuadraticBSplineVertexData( ) [2/2]
static __forceinline__ __device__ void optixGetQuadraticBSplineVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*

---

## 8.1.3.100 optixGetQuadraticBSplineVertexDataFromHandle()

static __forceinline__ __device__ void
optixGetQuadraticBSplineVertexDataFromHandle (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float4 data[3] ) [static]

8.1.3.101 optixGetRayFlags()
static __forceinline__ __device__ unsigned int optixGetRayFlags ( ) [static]

8.1.3.102 optixGetRayTime()
static __forceinline__ __device__ float optixGetRayTime ( ) [static]

8.1.3.103 optixGetRayTmax()
static __forceinline__ __device__ float optixGetRayTmax ( ) [static]

8.1.3.104 optixGetRayTmin()
static __forceinline__ __device__ float optixGetRayTmin ( ) [static]

8.1.3.105 optixGetRayVisibilityMask()
static __forceinline__ __device__ unsigned int optixGetRayVisibilityMask ( ) [static]

8.1.3.106 optixGetRibbonNormal() [1/2]
static __forceinline__ __device__ float3 optixGetRibbonNormal (
    float2 ribbonParameters ) [static]

8.1.3.107 optixGetRibbonNormal() [2/2]
static __forceinline__ __device__ float3 optixGetRibbonNormal (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float2 ribbonParameters ) [static]

8.1.3.108 optixGetRibbonNormalFromHandle()
static __forceinline__ __device__ float3 optixGetRibbonNormalFromHandle (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex, float *time,*
float2 *ribbonParameters*) *[static]*
8.1.3.109 optixGetRibbonParameters( )
static __forceinline__ __device__ float2 optixGetRibbonParameters () *[static]*
8.1.3.110 optixGetRibbonVertexData( ) [1/2]
static __forceinline__ __device__ void optixGetRibbonVertexData (
float4 *data[3]*) *[static]*
8.1.3.111 optixGetRibbonVertexData( ) [2/2]
static __forceinline__ __device__ void optixGetRibbonVertexData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*
8.1.3.112 optixGetRibbonVertexDataFromHandle( )
static __forceinline__ __device__ void optixGetRibbonVertexDataFromHandle (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,*
float4 *data[3]*) *[static]*
8.1.3.113 optixGetSbtDataPointer( )
static __forceinline__ __device__CUdeviceptroptixGetSbtDataPointer ()
*[static]*
8.1.3.114 optixGetSbtGASIndex( )
static __forceinline__ __device__ unsigned int optixGetSbtGASIndex () *[static]*
8.1.3.115 optixGetSphereData( ) [1/2]
static __forceinline__ __device__ void optixGetSphereData (
float4 *data[1]*) *[static]*
8.1.3.116 optixGetSphereData( ) [2/2]
static __forceinline__ __device__ void optixGetSphereData (
OptixTraversableHandle *gas,*
unsigned int *primIdx,*
unsigned int *sbtGASIndex,*
float *time,* float4 data[1] ) [static]

## 8.1.3.117 optixGetSphereDataFromHandle()

static __forceinline__ __device__ void optixGetSphereDataFromHandle (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float4 data[1]) [static]

## 8.1.3.118 optixGetSRTMotionTransformFromHandle()

static __forceinline__ __device__ const OptixSRTMotionTransform *
optixGetSRTMotionTransformFromHandle (
    OptixTraversableHandle handle) [static]

## 8.1.3.119 optixGetStaticTransformFromHandle()

static __forceinline__ __device__ const OptixStaticTransform *
optixGetStaticTransformFromHandle (
    OptixTraversableHandle handle) [static]

## 8.1.3.120 optixGetTransformListHandle()

static __forceinline__ __device__ OptixTraversableHandle
optixGetTransformListHandle (
    unsigned int index) [static]

## 8.1.3.121 optixGetTransformListSize()

static __forceinline__ __device__ unsigned int optixGetTransformListSize ( )
[static]

## 8.1.3.122 optixGetTransformTypeFromHandle()

static __forceinline__ __device__ OptixTransformType
optixGetTransformTypeFromHandle (
    OptixTraversableHandle handle) [static]

## 8.1.3.123 optixGetTriangleBarycentrics()

static __forceinline__ __device__ float2 optixGetTriangleBarycentrics ( )
[static]

## 8.1.3.124 optixGetTriangleVertexData() [1/2]

static __forceinline__ __device__ void optixGetTriangleVertexData (
    float3 data[3]) [static]

---

## 8.1.3.125 optixGetTriangleVertexData() [2/2]

static __forceinline__ __device__ void optixGetTriangleVertexData (
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float3 data[3] ) [static]

## 8.1.3.126 optixGetTriangleVertexDataFromHandle()

static __forceinline__ __device__ void optixGetTriangleVertexDataFromHandle(
    OptixTraversableHandle gas,
    unsigned int primIdx,
    unsigned int sbtGASIndex,
    float time,
    float3 data[3] ) [static]

## 8.1.3.127 optixGetWorldRayDirection()
static __forceinline__ __device__ float3 optixGetWorldRayDirection ( ) [static]

8.1.3.128 optixGetWorldRayOrigin()
static __forceinline__ __device__ float3 optixGetWorldRayOrigin ( ) [static]

## 8.1.3.129 optixGetWorldToObjectTransformMatrix() [1/2]

template<typename HitState >
static __forceinline__ __device__ void optixGetWorldToObjectTransformMatrix(
    const HitState & hs,
    float m[12] ) [static]

## 8.1.3.130 optixGetWorldToObjectTransformMatrix() [2/2]

static __forceinline__ __device__ void optixGetWorldToObjectTransformMatrix(
    float m[12] ) [static]

## 8.1.3.131 optixHitObjectGetAttribute_0()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_0 ( ) [static]

8.1.3.132 optixHitObjectGetAttribute_l()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_1 ( ) [static]

---

## 8.1.3.133 optixHitObjectGetAttribute_2()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_2
( ) [static]

## 8.1.3.134 optixHitObjectGetAttribute_3()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_3
( ) [static]

## 8.1.3.135 optixHitObjectGetAttribute_4()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_4
( ) [static]

## 8.1.3.136 optixHitObjectGetAttribute_5()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_5
( ) [static]

## 8.1.3.137 optixHitObjectGetAttribute_6()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_6
( ) [static]

## 8.1.3.138 optixHitObjectGetAttribute_7()
static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_7
( ) [static]

## 8.1.3.139 optixHitObjectGetCatmullRomRocapsVertexData()

static __forceinline__ __device__ void
optixHitObjectGetCatmullRomRocapsVertexData (
float4 data[4] ) [static]

## 8.1.3.140 optixHitObjectGetCatmullRomVertexData()

static __forceinline__ __device__ void optixHitObjectGetCatmullRomVertexData (
float4 data[4] ) [static]

## 8.1.3.141 optixHitObjectGetClusterId()

static __forceinline__ __device__ unsigned int optixHitObjectGetClusterId (
) [static]

## 8.1.3.142 optixHitObjectGetCubicBezierRocapsVertexData()

static __forceinline__ __device__ void
optixHitObjectGetCubicBezierRocapsVertexData (
float4 data[4] ) [static]

---

## 8.1.3.143 optixHitObjectGetCubicBezierVertexData()

static __forceinline__ __device__ void
optixHitObjectGetCubicBezierVertexData (
float4 data[4] ) [static]

## 8.1.3.144 optixHitObjectGetCubicBSplineRocapsVertexData()

static __forceinline__ __device__ void
optixHitObjectGetCubicBSplineRocapsVertexData (
float4 data[4] ) [static]

8.1.3.145 optixHitObjectGetCubicBSplineVertexData()

static __forceinline__ __device__ void
optixHitObjectGetCubicBSplineVertexData (
float4 data[4] ) [static]

8.1.3.146 optixHitObjectGetCurveParameter()

static __forceinline__ __device__ float optixHitObjectGetCurveParameter ( )
[static]

## 8.1.3.147 optixHitObjectGetGASTraversableHandle()

static __forceinline__ __device__ OptixTraversableHandle
optixHitObjectGetGASTraversableHandle ( ) [static]

## 8.1.3.148 optixHitObjectGetHitKind()

static __forceinline__ __device__ unsigned int optixHitObjectGetHitKind ( )
[static]

## 8.1.3.149 optixHitObjectGetInstanceId()

static __forceinline__ __device__ unsigned int optixHitObjectGetInstanceId ( )
[static]

## 8.1.3.150 optixHitObjectGetInstanceIndex()

static __forceinline__ __device__ unsigned int
optixHitObjectGetInstanceIndex ( ) [static]

## 8.1.3.151 optixHitObjectGetLinearCurveVertexData()

static __forceinline__ __device__ void
optixHitObjectGetLinearCurveVertexData (
float4 data[2] ) [static]

## 8.1.3.152 optixHitObjectGetObjectToWorldTransformMatrix()

static __forceinline__ __device__ void
optixHitObjectGetObjectToWorldTransformMatrix ( float m[12] ) [static]

## 8.1.3.153 optixHitObjectGetPrimitiveIndex()
static __forceinline__ __device__ unsigned int
optixHitObjectGetPrimitiveIndex ( ) [static]

## 8.1.3.154 optixHitObjectGetQuadraticBSplineRocapsVertexData()

static __forceinline__ __device__ void
optixHitObjectGetQuadraticBSplineRocapsVertexData (
float4 data[3]) [static]

## 8.1.3.155 optixHitObjectGetQuadraticBSplineVertexData()

static __forceinline__ __device__ void
optixHitObjectGetQuadraticBSplineVertexData (
float4 data[3]) [static]

## 8.1.3.156 optixHitObjectGetRayFlags()

static __forceinline__ __device__ unsigned int optixHitObjectGetRayFlags ( )
[static]

8.1.3.157 optixHitObjectGetRayTime()

static __forceinline__ __device__ float optixHitObjectGetRayTime ( ) [static]

8.1.3.158 optixHitObjectGetRayTmax()

static __forceinline__ __device__ float optixHitObjectGetRayTmax ( ) [static]

8.1.3.159 optixHitObjectGetRayTmin()

static __forceinline__ __device__ float optixHitObjectGetRayTmin ( ) [static]

8.1.3.160 optixHitObjectGetRibbonNormal()

static __forceinline__ __device__ float3 optixHitObjectGetRibbonNormal (
float2 ribbonParameters) [static]

## 8.1.3.161 optixHitObjectGetRibbonParameters()

static __forceinline__ __device__ float2 optixHitObjectGetRibbonParameters ( )
[static]

## 8.1.3.162 optixHitObjectGetRibbonVertexData()

static __forceinline__ __device__ void optixHitObjectGetRibbonVertexData (
float4 data[3]) [static]

## 8.1.3.163 optixHitObjectGetSbtDataPointer()

static __forceinline__ __device__ CUdeviceptr optixHitObjectGetSbtDataPointer ( ) [static]

## 8.1.3.164 optixHitObjectGetSbtGASIndex()
static __forceinline__ __device__ unsigned int optixHitObjectGetSbtGASIndex
( ) [static]

## 8.1.3.165 optixHitObjectGetSbtRecordIndex()
static __forceinline__ __device__ unsigned int
optixHitObjectGetSbtRecordIndex ( ) [static]

## 8.1.3.166 optixHitObjectGetSphereData()
static __forceinline__ __device__ void optixHitObjectGetSphereData (
float4 data[1] ) [static]

## 8.1.3.167 optixHitObjectGetTransformListHandle()
static __forceinline__ __device__ OptixTraversableHandle
optixHitObjectGetTransformListHandle (
unsigned int index ) [static]

## 8.1.3.168 optixHitObjectGetTransformListSize()
static __forceinline__ __device__ unsigned int
optixHitObjectGetTransformListSize ( ) [static]

## 8.1.3.169 optixHitObjectGetTraverseData()
static __forceinline__ __device__ void optixHitObjectGetTraverseData (
OptixTraverseData * data ) [static]

## 8.1.3.170 optixHitObjectGetTriangleBarycentrics()
static __forceinline__ __device__ float2
optixHitObjectGetTriangleBarycentrics ( ) [static]

## 8.1.3.171 optixHitObjectGetTriangleVertexData()
static __forceinline__ __device__ void optixHitObjectGetTriangleVertexData (
float3 data[3] ) [static]

## 8.1.3.172 optixHitObjectGetWorldRayDirection()
static __forceinline__ __device__ float3 optixHitObjectGetWorldRayDirection
( ) [static]

## 8.1.3.173 optixHitObjectGetWorldRayOrigin()
static __forceinline__ __device__ float3 optixHitObjectGetWorldRayOrigin ( )
[static]

---

## 8.1.3.174 optixHitObjectGetWorldToObjectTransformMatrix()

static __forceinline__ __device__ void
optixHitObjectGetWorldToObjectTransformMatrix (
float m[12] ) [static]

8.1.3.175 optixHitObjectIsHit()
static __forceinline__ __device__ bool optixHitObjectIsHit ( ) [static]

8.1.3.176 optixHitObjectIsMiss()
static __forceinline__ __device__ bool optixHitObjectIsMiss ( ) [static]

8.1.3.177 optixHitObjectIsNop()
static __forceinline__ __device__ bool optixHitObjectIsNop ( ) [static]

8.1.3.178 optixHitObjectSetSbtRecordIndex()

static __forceinline__ __device__ void optixHitObjectSetSbtRecordIndex (
unsigned int sbtRecordIndex ) [static]

8.1.3.179 optixHitObjectTransformNormalFromObjectToWorldSpace()

static __forceinline__ __device__ float3
optixHitObjectTransformNormalFromObjectToWorldSpace (
float3 normal ) [static]

8.1.3.180 optixHitObjectTransformNormalFromWorldToObjectSpace()

static __forceinline__ __device__ float3
optixHitObjectTransformNormalFromWorldToObjectSpace (
float3 normal ) [static]

8.1.3.181 optixHitObjectTransformPointFromObjectToWorldSpace()

static __forceinline__ __device__ float3
optixHitObjectTransformPointFromObjectToWorldSpace (
float3 point ) [static]

8.1.3.182 optixHitObjectTransformPointFromWorldToObjectSpace()

static __forceinline__ __device__ float3
optixHitObjectTransformPointFromWorldToObjectSpace (
float3 point ) [static]

8.1.3.183 optixHitObjectTransformVectorFromObjectToWorldSpace()

static __forceinline__ __device__ float3
optixHitObjectTransformVectorFromObjectToWorldSpace (
float3 vec ) [static]

---

8.1.3.184 optixHitObjectTransformVectorFromWorldToObjectSpace()

static __forceinline__ __device__ float3
optixHitObjectTransformVectorFromWorldToObjectSpace (
float3 vec ) [static]

8.1.3.185 optixIgnoreIntersection()
static __forceinline__ __device__ void optixIgnoreIntersection ( ) [static]

8.1.3.186 optixInvoke() [1/2]

template<typename... Payload>
static __forceinline__ __device__ void optixInvoke (
OptixPayloadTypeID type,
Payload &... payload ) [static]

8.1.3.187 optixInvoke() [2/2]

template<typename... Payload>
static __forceinline__ __device__ void optixInvoke (
Payload &... payload ) [static]

8.1.3.188 optixIsBackFaceHit() [1/2]
static __forceinline__ __device__ bool optixIsBackFaceHit ( ) [static]

8.1.3.189 optixIsBackFaceHit() [2/2]
static __forceinline__ __device__ bool optixIsBackFaceHit (
unsigned int hitKind ) [static]

8.1.3.190 optixIsFrontFaceHit() [1/2]
static __forceinline__ __device__ bool optixIsFrontFaceHit ( ) [static]

8.1.3.191 optixIsFrontFaceHit() [2/2]
static __forceinline__ __device__ bool optixIsFrontFaceHit (
unsigned int hitKind ) [static]

8.1.3.192 optixIsTriangleBackFaceHit()
static __forceinline__ __device__ bool optixIsTriangleBackFaceHit ( ) [static]

8.1.3.193 optixIsTriangleFrontFaceHit()
static __forceinline__ __device__ bool optixIsTriangleFrontFaceHit ( ) [static]

8.1.3.194 optixIsTriangleHit()
static __forceinline__ __device__ bool optixIsTriangleHit ( ) [static]

---

## 8.1.3.195 optixMakeHitObject()

static __forceinline__ __device__ void optixMakeHitObject (
    OptixTraversableHandle handle,
    float3 rayOrigin,
    float3 rayDirection,
    float tmin,
    float rayTime,
    unsigned int rayFlags,
    OptixTraverseData traverseData,
    const OptixTraversableHandle * transforms,
    unsigned int numTransforms ) [static]

8.1.3.196 optixMakeMissHitObject()

static __forceinline__ __device__ void optixMakeMissHitObject (
    unsigned int missSBTIndex,
    float3 rayOrigin,
    float3 rayDirection,
    float tmin,
    float tmax,
    float rayTime,
    unsigned int rayFlags ) [static]

8.1.3.197 optixMakeNopHitObject()

static __forceinline__ __device__ void optixMakeNopHitObject ( ) [static]

8.1.3.198 optixReorder() [1/2]
static __forceinline__ __device__ void optixReorder ( ) [static]

8.1.3.199 optixReorder() [2/2]
static __forceinline__ __device__ void optixReorder (
    unsigned int coherenceHint,
    unsigned int numCoherenceHintBits ) [static]

8.1.3.200 optixReportIntersection() [1/9]
static __forceinline__ __device__ bool optixReportIntersection (
    float hitT,
    unsigned int hitKind ) [static]

8.1.3.201 optixReportIntersection() [2/9]
static __forceinline__ __device__ bool optixReportIntersection (
    float hitT,
    unsigned int hitKind,

---

## 8.1.3.202 optixReportIntersection( ) [3/9]

unsigned int *a0*) *[static]*
static __forceinline__ __device__ bool optixReportIntersection (
float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1) [static]
8.1.3.203 optixReportIntersection( ) [4/9]
static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1*,
unsigned int *a2*) *[static]*
8.1.3.204 optixReportIntersection( ) [5/9]
static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3*) *[static]*
8.1.3.205 optixReportIntersection( ) [6/9]
static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3,*
unsigned int *a4*) *[static]*
8.1.3.206 optixReportIntersection( ) [7/9]
static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,* unsigned int *a2,*
unsigned int *a3,*
unsigned int *a4,*
unsigned int *a5*) *[static]*

## 8.1.3.207 optixReportIntersection( ) [8/9]

static __forceinline__ __device__ bool optixReportIntersection (
float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1,
unsigned int a2,
unsigned int a3,
unsigned int a4,
unsigned int a5,
unsigned int a6) [static]
8.1.3.208 optixReportIntersection( ) [9/9]
static __forceinline__ __device__ bool optixReportIntersection (
float *hitT,*
unsigned int *hitKind,*
unsigned int *a0,*
unsigned int *a1,*
unsigned int *a2,*
unsigned int *a3,*
unsigned int *a4,*
unsigned int *a5,*
unsigned int *a6*,
unsigned int *a7*) *[static]*
8.1.3.209 optixSetPayload_0( )
static __forceinline__ __device__ void optixSetPayload_0 (
unsigned int *p*) *[static]*
8.1.3.210 optixSetPayload_1( )
static __forceinline__ __device__ void optixSetPayload_1 (
unsigned int *p*) *[static]*
8.1.3.211 optixSetPayload_10( )
static __forceinline__ __device__ void optixSetPayload_10 (
unsigned int *p*) *[static]*

---

8.1.3.212 optixSetPayload_11()
static __forceinline__ __device__ void optixSetPayload_11 (
unsigned int p ) [static]

8.1.3.213 optixSetPayload_12()
static __forceinline__ __device__ void optixSetPayload_12 (
unsigned int p ) [static]

8.1.3.214 optixSetPayload_13()
static __forceinline__ __device__ void optixSetPayload_13 (
unsigned int p ) [static]

8.1.3.215 optixSetPayload_14()
static __forceinline__ __device__ void optixSetPayload_14 (
unsigned int p ) [static]

8.1.3.216 optixSetPayload_15()
static __forceinline__ __device__ void optixSetPayload_15 (
unsigned int p ) [static]

8.1.3.217 optixSetPayload_16()
static __forceinline__ __device__ void optixSetPayload_16 (
unsigned int p ) [static]

8.1.3.218 optixSetPayload_17()
static __forceinline__ __device__ void optixSetPayload_17 (
unsigned int p ) [static]

8.1.3.219 optixSetPayload_18()
static __forceinline__ __device__ void optixSetPayload_18 (
unsigned int p ) [static]

8.1.3.220 optixSetPayload_19()
static __forceinline__ __device__ void optixSetPayload_19 (
unsigned int p ) [static]

8.1.3.221 optixSetPayload_2()
static __forceinline__ __device__ void optixSetPayload_2 (
unsigned int p ) [static]

8.1.3.222 optixSetPayload_20()
static __forceinline__ __device__ void optixSetPayload_20 (

---

## 8.1.3.223 optixSetPayload_21( )

unsigned int *p*) *[static]*

static __forceinline__ __device__ void optixSetPayload_21 (
unsigned int *p*) *[static]*

8.1.3.224 o*p*tixSetPayload_22( )

static __forceinline__ __device__ void optixSetPayload_22 (

unsigned int *p*) *[static]*
8.1.3.225 optixSetPayload_23( )

static __forceinline__ __device__ void optixSetPayload_23 (

unsigned int *p*) *[static]*
8.1.3.226 optixSetPayload_24( )

static __forceinline__ __device__ void optixSetPayload_24 (
unsigned int *p*) *[static]*

8.1.3.227 o*p*tixSetPayload_25( )

static __forceinline__ __device__ void optixSetPayload_25 (

unsigned int *p*) *[static]*
8.1.3.228 optixSetPayload_26( )

static __forceinline__ __device__ void optixSetPayload_26 (
unsigned int *p*) *[static]*

8.1.3.229 optixSetPayload_27( )

static __forceinline__ __device__ void optixSetPayload_27 (
unsigned int *p*) *[static]*

8.1.3.230 o*p*tixSetPayload_28( )

static __forceinline__ __device__ void optixSetPayload_28 (

unsigned int *p*) *[static]*
8.1.3.231 optixSetPayload_29( )

static __forceinline__ __device__ void optixSetPayload_29 (
unsigned int *p*) *[static]*

8.1.3.232 o*p*tixSetPayload_3( )

static __forceinline__ __device__ void optixSetPayload_3 (

unsigned int *p*) *[static]*

---

8.1.3.233 optixSetPayload_30()
static __forceinline__ __device__ void optixSetPayload_30 (
unsigned int p ) [static]

8.1.3.234 optixSetPayload_31()
static __forceinline__ __device__ void optixSetPayload_31 (
unsigned int p ) [static]

8.1.3.235 optixSetPayload_4()
static __forceinline__ __device__ void optixSetPayload_4 (
unsigned int p ) [static]

8.1.3.236 optixSetPayload_5()
static __forceinline__ __device__ void optixSetPayload_5 (
unsigned int p ) [static]

8.1.3.237 optixSetPayload_6()
static __forceinline__ __device__ void optixSetPayload_6 (
unsigned int p ) [static]

8.1.3.238 optixSetPayload_7()
static __forceinline__ __device__ void optixSetPayload_7 (
unsigned int p ) [static]

8.1.3.239 optixSetPayload_8()
static __forceinline__ __device__ void optixSetPayload_8 (
unsigned int p ) [static]

8.1.3.240 optixSetPayload_9()
static __forceinline__ __device__ void optixSetPayload_9 (
unsigned int p ) [static]

8.1.3.241 optixSetPayloadTypes()
static __forceinline__ __device__ void optixSetPayloadTypes (
unsigned int types ) [static]

8.1.3.242 optixTerminateRay()
static __forceinline__ __device__ void optixTerminateRay ( ) [static]

8.1.3.243 optixTexFootprint2D()
static __forceinline__ __device__ uint4 optixTexFootprint2D (
unsigned long long tex, unsigned int texInfo,
float x,
float y,
unsigned int * singleMipLevel ) [static]

## 8.1.3.244 optixTexFootprint2DGrad()

static __forceinline__ __device__ uint4 optixTexFootprint2DGrad (
    unsigned long long tex,
    unsigned int texInfo,
    float x,
    float y,
    float dPdx_x,
    float dPdx_y,
    float dPdy_x,
    float dPdy_y,
    bool coarse,
    unsigned int * singleMipLevel ) [static]

## 8.1.3.245 optixTexFootprint2DLod()

static __forceinline__ __device__ uint4 optixTexFootprint2DLod (
    unsigned long long tex,
    unsigned int texInfo,
    float x,
    float y,
    float level,
    bool coarse,
    unsigned int * singleMipLevel ) [static]

## 8.1.3.246 optixThrowException() [1/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode ) [static]

## 8.1.3.247 optixThrowException() [2/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0 ) [static]

## 8.1.3.248 optixThrowException() [3/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0,
    unsigned int exceptionDetail1 ) [static]

---

## 8.1.3.249 optixThrowException() [4/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0,
    unsigned int exceptionDetail1,
    unsigned int exceptionDetail2 ) [static]

## 8.1.3.250 optixThrowException() [5/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0,
    unsigned int exceptionDetail1,
    unsigned int exceptionDetail2,
    unsigned int exceptionDetail3 )
[static]

## 8.1.3.251 optixThrowException() [6/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0,
    unsigned int exceptionDetail1,
    unsigned int exceptionDetail2,
    unsigned int exceptionDetail3,
    unsigned int exceptionDetail4 )
[static]

## 8.1.3.252 optixThrowException() [7/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0,
    unsigned int exceptionDetail1,
    unsigned int exceptionDetail2,
    unsigned int exceptionDetail3,
    unsigned int exceptionDetail4,
    unsigned int exceptionDetail5 )
[static]

## 8.1.3.253 optixThrowException() [8/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0,
    unsigned int exceptionDetail1,
    unsigned int exceptionDetail2,
    unsigned int exceptionDetail3,
    unsigned int exceptionDetail4, unsigned int exceptionDetail5,
unsigned int exceptionDetail6 ) [static]

## 8.1.3.254 optixThrowException() [9/9]

static __forceinline__ __device__ void optixThrowException (
    int exceptionCode,
    unsigned int exceptionDetail0,
    unsigned int exceptionDetail1,
    unsigned int exceptionDetail2,
    unsigned int exceptionDetail3,
    unsigned int exceptionDetail4,
    unsigned int exceptionDetail5,
    unsigned int exceptionDetail6,
    unsigned int exceptionDetail7 ) [static]

## 8.1.3.255 optixTrace() [1/2]

template<typename... Payload>
static __forceinline__ __device__ void optixTrace (
    OptixPayloadTypeID type,
    OptixTraversableHandle handle,
    float3 rayOrigin,
    float3 rayDirection,
    float tmin,
    float tmax,
    float rayTime,
    OptixVisibilityMask visibilityMask,
    unsigned int rayFlags,
    unsigned int SBToffset,
    unsigned int SBTstride,
    unsigned int missSBTIndex,
    Payload &... payload ) [static]

## 8.1.3.256 optixTrace() [2/2]

template<typename... Payload>
static __forceinline__ __device__ void optixTrace (
    OptixTraversableHandle handle,
    float3 rayOrigin,
    float3 rayDirection,
    float tmin,
    float tmax,
    float rayTime,
    OptixVisibilityMask visibilityMask, unsigned int rayFlags,
unsigned int SBToffset,
unsigned int SBTstride,
unsigned int missSBTIndex,
Payload &... payload ) [static]

## 8.1.3.257 optixTransformNormalFromObjectToWorldSpace() [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
## optixTransformNormalFromObjectToWorldSpace (
const HitState & hs,
float3 normal ) [static]

8.1.3.258 optixTransformNormalFromObjectToWorldSpace() [2/2]

static __forceinline__ __device__ float3
optixTransformNormalFromObjectToWorldSpace (
float3 normal ) [static]

## 8.1.3.259 optixTransformNormalFromWorldToObjectSpace() [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
## optixTransformNormalFromWorldToObjectSpace (
const HitState & hs,
float3 normal ) [static]

8.1.3.260 optixTransformNormalFromWorldToObjectSpace() [2/2]

static __forceinline__ __device__ float3
optixTransformNormalFromWorldToObjectSpace (
float3 normal ) [static]

## 8.1.3.261 optixTransformPointFromObjectToWorldSpace() [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
## optixTransformPointFromObjectToWorldSpace (
const HitState & hs,
float3 point ) [static]

8.1.3.262 optixTransformPointFromObjectToWorldSpace() [2/2]

static __forceinline__ __device__ float3
optixTransformPointFromObjectToWorldSpace (
float3 point ) [static]

---

## 8.1.3.263 optixTransformPointFromWorldToObjectSpace() [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformPointFromWorldToObjectSpace (
    const HitState & hs,
    float3 point ) [static]

8.1.3.264 optixTransformPointFromWorldToObjectSpace() [2/2]

static __forceinline__ __device__ float3
optixTransformPointFromWorldToObjectSpace (
    float3 point ) [static]

8.1.3.265 optixTransformVectorFromObjectToWorldSpace() [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformVectorFromObjectToWorldSpace (
    const HitState & hs,
    float3 vec ) [static]

8.1.3.266 optixTransformVectorFromObjectToWorldSpace() [2/2]

static __forceinline__ __device__ float3
optixTransformVectorFromObjectToWorldSpace (
    float3 vec ) [static]

8.1.3.267 optixTransformVectorFromWorldToObjectSpace() [1/2]

template<typename HitState >
static __forceinline__ __device__ float3
optixTransformVectorFromWorldToObjectSpace (
    const HitState & hs,
    float3 vec ) [static]

8.1.3.268 optixTransformVectorFromWorldToObjectSpace() [2/2]

static __forceinline__ __device__ float3
optixTransformVectorFromWorldToObjectSpace (
    float3 vec ) [static]

8.1.3.269 optixTraverse() [1/2]

template<typename... Payload>
static __forceinline__ __device__ void optixTraverse (
    OptixPayloadTypeID type,
    OptixTraversableHandle handle,
    float3 rayOrigin, float3 *rayDirection,*
float *tmin,*
float *tmax,*
float *rayTime,*
OptixVisibilityMask *visibilityMask,*
unsigned int *rayFlags,*
unsigned int *SBToffset,*
unsigned int *SBTstride,*
unsigned int *missSBTIndex,*
Payload &... *payload*) *[static]*

## 8.1.3.270 optixTraverse( ) [2/2]

template<typename... Payload>

static __forceinline__ __device__ void optixTraverse (
OptixTraversableHandle *handle,*
float3 *rayOrigin,*
float3 *rayDirection,*
float *tmin,*
float *tmax,*
float *rayTime,*
OptixVisibilityMask *visibilityMask,*
unsigned int *rayFlags,*
unsigned int *SBToffset,*
unsigned int *SBTstride,*
unsigned int *missSBTIndex,*
Payload &... *payload*) *[static]*

## 8.1.3.271 optixUndefinedValue( )

static __forceinline__ __device__ unsigned int optixUndefinedValue () *[static]*

8.2 optix_device_impl.h

Go to the documentation of this file.

1/*
2* SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
3* SPDX-License-Identifier: LicenseRef-NvidiaProprietary
4*
5* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
6* property and proprietary rights in and to this material, related
7* documentation and any modifications thereto. Any use, reproduction,
8* disclosure or distribution of this material and related documentation
9* without an express license agreement from NVIDIA CORPORATION or
10* its affiliates is strictly prohibited.
11*/
20#if!defined(__OPTIX_INCLUDE_INTERNAL_HEADERS__)
21#error("optix_device_impl.h is an internal header file and must not be used directly. Please use
optix_device.h or optix.h instead.")
22#endif
23
24#ifndef OPTIX_OPTIX_DEVICE_IMPL_H

---

```c
#define OPTIX_OPTIX_DEVICE_IMPL_H

#include "internal/optix_device_impl_transformations.h"

#ifndef __CUDACC_RTC__
#include <initializer_list>
#include <type_traits>
#endif

namespace optix_internal {
template <typename...>
struct TypePack();
} // namespace optix_internal

template <typename... Payload>
static __forceinline__ __device__ void optixTrace(OptixTraversableHandle handle,
                                                float3            rayOrigin,
                                                float3            rayDirection,
                                                float            tmin,
                                                float            tmax,
                                                float            rayTime,
                                                OptixVisibilityMask    visibilityMask,
                                                unsigned int      rayFlags,
                                                unsigned int      SBTooffset,
                                                unsigned int      SBStride,
                                                unsigned int      missSBIIndex,
                                                Payload&...     payload)
{
    static_assert(sizeof(...(Payload)) <= 32, "Only up to 32 payload values are allowed.");
    // std::is_same compares each type in the two TypePacks to make sure that all types are unsigned int.
    // TypePack 1   unsigned int    T0    T1    T2 ... Tn-1    Tn
    // TypePack 2   T0    T1    T2    T3 ... Tn        unsigned int
#ifdef __CUDACC_RTC__
    static_assert(std::is_same<optix_internal::TypePack>unsigned int, Payload...>,
optix_internal::TypePack<Payload,..., unsigned int>::value,
                "All payload parameters need to be unsigned int.");
#endif

    OptixPayloadTypeID type = OPTIX_PAYLOAD_TYPE_DEFAULT;
    float          ox = rayOrigin.x, oy = rayOrigin.y, oz = rayOrigin.z;
    float          dx = rayDirection.x, dy = rayDirection.y, dz = rayDirection.z;
    unsigned int p[33]       = { 0, payload... };
    int             payloadSize = (int)sizeof(...(Payload));
    asm volatile(
        "call"

("%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31)",
        "_optix_trace_typeD_32"

("%32,%33,%34,%35,%36,%37,%38,%39,%40,%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56,%57,%58",
        "59%,60%,61%,62%,63%,64%,65%,66%,67%,68%,69%,70%,71%,72%,73%,74%,75%,76%,77%,78%,79%,80);"
        : "r=(p[11])," "f=(p[2])," "r=(p[3])," "r=(p[4])," "r=(p[5])," "r=(p[6])," "r=(p[7]),
        "r=(p[8])," "f=(p[9])," "r=(p[10])," "r=(p[11])," "r=(p[12])," "r=(p[13])," "r=(p[14]),
        "r=(p[15])," "r=(p[16])," "r=(p[17])," "r=(p[18])," "r=(p[19])," "r=(p[20])," "r=(p[21]),
        "r=(p[22])," "r=(p[23])," "r=(p[24])," "r=(p[25])," "r=(p[26])," "r=(p[27])," "r=(p[28]),
        "r=(p[29])," "r=(p[30])," "r=(p[31])," "r=(p[32])",
        : "r(type)," "I("(handle)," f(ox)," f(foy)," f(foz)," f(fdx)," f(fdy)," f(fdz),"

---

}

template <typename... Payload>
static __forceinline__ __device__ void optixTraverse(OptixTraversableHandle handle,
                                 float3            rayOrigin,
                                 float3            rayDirection,
                                 float3            tmin,
                                 float3            tmax,
                                 float3            rayTime,
                                 OptixVisibilityMask  visibilityMask,
                                 unsigned int      rayFlags,
                                 unsigned int      SBToffset,
                                 unsigned int      SBTstride,
                                 unsigned int      missSBTIndex,
                                 Payload&... payload)
{
    static_assert(sizeof(...(Payload) <= 32, "Only up to 32 payload values are allowed.");
    // std::is_same compares each type in the two TypePacks to make sure that all types are unsigned int.
    // TypePack 1    unsigned int   T0     T1     T2 ... Tn-1     Tn
    // TypePack 2    T0     T1     T2     T3 ... Tn    unsigned int
#ifndef __CUDACC_RTC__
    static_assert(std::is_same<optix_internal>::TypePack<unsigned int, Payload>...
optix_internal::TypePack<Payload,..., unsigned int>::value,
                    "All payload parameters need to be unsigned int.");
#endif

    OptixPayloadTypeID type = OPTIX_PAYLOAD_TYPE_DEFAULT;
    float             ox = rayOrigin.x, oy = rayOrigin.y, oz = rayOrigin.z;
    float             dx = rayDirection.x, dy = rayDirection.y, dz = rayDirection.z;
    unsigned int p[33]       = {0, payload...};
    int                 payloadSize = (int)sizeof(...(Payload));
    asm volatile(
        "call"
    );

    OptixPayloadTypeID type = OPTIX_PAYLOAD_TYPE_DEFAULT;
    float             ox = rayOrigin.x, oy = rayOrigin.y, oz = rayOrigin.z;
    float             dx = rayDirection.x, dy = rayDirection.y, dz = rayDirection.z;
    unsigned int p[33]       = {0, payload...};
    int                 payloadSize = (int)sizeof(...(Payload));
    asm volatile(
        "call"
    );

    (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15, %16, %17, %18, %19, %20, %21, %22, %23, %24, %25, %26, %27, %28, %29, %30, %31),"
    "_optix_hitobject_traverse,"

    (%32, %33, %34, %35, %36, %37, %38, %39, %40, %41, %42, %43, %44, %45, %46, %47, %48, %49, %50, %51, %52, %53, %54, %55, %56, %57, %58, %59, %60, %61, %62, %63, %64, %65, %66, %67, %68, %69, %70, %71, %72, %73, %74, %75, %76, %77, %78, %79, %80);"
    : ="r(p[1])", ="r(p[2])", ="r(p[3])", ="r(p[4])", ="r(p[5])", ="r(p[6])", ="r(p[7])",
    : ="r(p[8])", ="r(p[9])", ="r(p[10])", ="r(p[11])", ="r(p[12])", ="r(p[13])", ="r(p[14])",
    : ="r(p[15])", ="r(p[16])", ="r(p[17])", ="r(p[18])", ="r(p[19])", ="r(p[20])", ="r(p[21])",
    : ="r(p[22])", ="r(p[23])", ="r(p[24])

---

```cpp
                                        unsigned int        SBToffset,
                                        unsigned int        SBtstride,
                                        unsigned int        missSBTIndex,
                                        Payload&...      payload)
{
    // std::is_same compares each type in the two TypePacks to make sure that all types are unsigned int.
    // TypePack 1   unsigned int     T0    T1    T2 ...    Tn-1    Tn
    // TypePack 2   T0    T1    T2 ...    Tn    unsigned int
    static_assert(sizeof(...(Payload) <= 32, "Only up to 32 payload values are allowed.");
#ifndef __CUDACC_RTC__
    static_assert(std::is_same<optix_internal::TypePack<unsigned int, Payload>>,
optix_internal::TypePack,..., unsigned int>::value,
                "All payload parameters need to be unsigned int.");
#endif

    float    ox = rayOrigin.x, oy = rayOrigin.y, oz = rayOrigin.z;
    float    dx = rayDirection.x, dy = rayDirection.y, dz = rayDirection.z;
    unsigned int p[33]       = {0, payload...};
    int      payloadSize = (int)sizeof(...(Payload));

    asm volatile(
        "call"

("%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%"
    "29,%30,%31),"
    "_optix_trace_typeD_32,"

("%32,%33,%34,%35,%36,%37,%38,%39,%40,%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56,%57,%58"
    "59,%60,%61,%62,%63,%64,%65,%66,%67,%68,%69,%70,%71,%72,%73,%74,%75,%76,%77,%78,%79,%80);"
    : =r("p[1]"), =r("p[2]"), =r("p[3]"), =r("p[4]"), =r("p[5]"), =r("p[6]"), =r("p[7]"),
    =r("p[8]"), =r("p[9]"), =r("p[10]"), =r("p[11]"), =r("p[12]"), =r("p[13]"), =r("p[14]"),
    =r("p[15]"), =r("p[16]"), =r("p[17]"), =r("p[18]"), =r("p[19]"), =r("p[20]"), =r("p[21]"),
    =r("p[22]"), =r("p[23]"), =r("p[24]"), =r("p[25]"), =r("p[26]"), =r("p[27]"), =r("p[28]"),
    =r("p[29]"), =r("p[30]"), =r("p[31]"), =r("p[32]")
    : r("type"), l("handle"), f("ox"), f("oy"), f("oz"), f("dx"), f("dy"), f("dz"), f("tmin"),
    f("tmax"), f("rayTime"), r("visibilityMask"), r("rayFlags"), r("SBToffset"), r("SBtstride"),
    r("missSBTIndex"), r("payloadSize"), r("p[1]"), r("p[2]"), r("p[3]"), r("p[4]"), r("p[5]"),
    r("p[6]"), r("p[7]"), r("p[8]"), r("p[9]"), r("p[10]"), r("p[11]"), r("p[12]"), r("p[13]"),
    r("p[14]"), r("p[15]"),

(void)std::initializer_list<unsigned int>{index, (payload = p[index++])...};

---

```cpp
optix_internal::TypePack<Payload,..., unsigned int*>::value,
                    "All payload parameters need to be unsigned int.");
#endif

float        ox = rayOrigin.x, oy = rayOrigin.y, oz = rayOrigin.z;
float        dx = rayDirection.x, dy = rayDirection.y, dz = rayDirection.z;
unsigned int p[33]         = {0, payload...};
int           payloadSize = (int)sizeof(...(Payload));
asm volatile(
    "call"
("%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31)",
    "_optix_hitobject_traverse,"

("%32,%33,%34,%35,%36,%37,%38,%39,%40,%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56,%57,%58",
    "59,%60,%61,%62,%63,%64,%65,%66,%67,%68,%69,%70,%71,%72,%73,%74,%75,%76,%77,%78,%79,%80)";
    : ="r[p[1]]", ="r[p[2]]", ="r[p[3]]", ="r[p[4]]", ="r[p[5]]", ="r[p[6]]", ="r[p[7]]",
    : ="r[p[8]]", ="r[p[9]]", ="r[p[10]]", ="r[p[11]]", ="r[p[12]]", ="r[p[13]]", ="r[p[14]]",
    : ="r[p[15]]", ="r[p[16]]", ="r[p[17]]", ="r[p[18]]", ="r[p[19]]", ="r[p[20]]", ="r[p[21]]",
    : ="r[p[22]]", ="r[p[23]]", ="r[p[24]]", ="r[p[25]]", ="r[p[26]]", ="r[p[27]]", ="r[p[28]]",
    : ="r[p[29]]", ="r[p[30]]", ="r[p[31]]", ="r[p[32]]"
    : r("type"), l("handle"), f("ox"), f("oy"), f("oz"), f("dx"), f("dy"), f("dz"), f("tmin"),
    f("tmax"), f("rayTime"), f("visibilityMask"), f("rayFlags"), f("SBToFFSET"), f("SBTstride"),
    f("missSBTIndex"), r("payloadSize"), r("p[1]"), r("p[2]"), r("p[3]"), r("p[4]"), r("p[5]"),
    r("p[6]"), r("p[7]"), r("p[8]"), r("p[9]"), r("p10"), r("p11"), r("p12"), r("p13"),
    r("p14]), r("p15"), r("p16"), r("p17"), r("p18"), r("p19"), r("p20"),
    r("p21"), r("p22"), r("p23"), r("p24"), r("p25"), r("p26"), r("p27"),
    r("p28"), r("p29"), r("p30"), r("p31"), r("p32")
    );
unsigned int index = 1;
(void)std::initializer_list<unsigned int>(index, (payload = p[index++])..);

static __forceinline__ __device__ void optixReorder(unsigned int coherenceHint, unsigned int numCoherenceHintBits)
{
    asm volatile(
        "call"
        "()","_
        "_optix_hitobject_reorder,

277// std::is_same compares each type in the two TypePacks to make sure that all types are unsigned int.

---

```cpp
    static_assert(sizeof...(Payload) <= 32, "Only up to 32 payload values are allowed.");
#ifndef __CUDACC_RTC__
    static_assert(std::is_same<optix_internal>::TypePack<unsigned int, Payload>...
,optix_internal::TypePack<Payload,..., unsigned int>::value,
        "All payload parameters need to be unsigned int.");
#endif

    unsigned int p[33]         = {0, payload...};
    int                     payloadSize = (int)sizeof...(Payload);

    asm volatile(
        "call"

("%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%"
        "29,%30,%31),"
        "_optix_hitobject_invoke,"

("%32,%33,%34,%35,%36,%37,%38,%39,%40,%41,%42,%43,%44,%45,%46,%47,%48,%49,%50,%51,%52,%53,%54,%55,%56,%57,%58"
        "59,%60,%61,%62,%63,%64,%65);",
        : =r("p[1]), =r("p[2]), =r("p[3]), =r("p[4]), =r("p[5]), =r("p[6]), =r("p[7]),
        =r("p[8]), =r("p[9]), =r("p[10]), =r("p[11]), =r("p[12]), =r("p[13]), =r("p[14]),
        =r("p[15]), =r("p[16]), =r("p[17]), =r("p[18]), =r("p[19]), =r("p[20]), =r("p[21]),
        =r("p[22]), =r("p[23]), =r("p[24]), =r("p[25]), =r("p[26]), =r("p[27]), =r("p[28]),
        =r("p[29]), =r("p[30]), =r("p[31]), =r("p[32])
        : r("type"), r("payloadSize"), r("p[1]), r("p[2]),
        r("p[3]), r("p[4]), r("p[5]), r("p[6]), r("p[7]), r("p[8]), r("p[9]), r("p[10]),
        r("p[11]), r("p[12]), r("p[13]), r("p[14]), r("p[15]), r("p[16]), r("p[17]),
        r("p[18]), r("p[19]), r("p[20]), r("p[21]), r("p[22]), r("p[23]), r("p[24]),
        r("p[25]), r("p[26]), r("p[27]), r("p[28]), r("p[29]), r("p[30]), r("p[31]), r("p[32])
        );

    unsigned int index = 1;
    (void)std::initializer_list<unsigned int>(index, (payload = p[index++])...);

}

template <typename... Payload>
static __forceinline__ __device__ void optixInvoke(Payload&... payload)
{
    // std::is_same compares each type in the two TypePacks to make sure that all types are unsigned int.
    // TypePack 1     unsigned int   T0      T1      T2 ... Tn-1       Tn
    // TypePack 2     T0      T1      T2      T3 ... Tn       unsigned int
    static_assert(sizeof...(Payload) <= 32, "Only up to 32 payload values are allowed.");
#ifndef __CUDAACC_RTC__
    static_assert(std::is_same<optix_internal>::TypePack<unsigned int, Payload>...
,optix_in

---

"r"(p[3]), "r"(p[4]), "r"(p[5]), "r"(p[6]), "r"(p[7]), "r"(p[8]), "r"(p[9]), "r"(p[10]),
"r"(p[11]), "r"(p[12]), "r"(p[13]), "r"(p[14]), "r"(p[15]), "r"(p[16]), "r"(p[17]),
"r"(p[18]), "r"(p[19]), "r"(p[20]), "r"(p[21]), "r"(p[22]), "r"(p[23]), "r"(p[24]),
"r"(p[25]), "r"(p[26]), "r"(p[27]), "r"(p[28]), "r"(p[29]), "r"(p[30]), "r"(p[31]), "r"(p[32])
:);

unsigned int index = 1;
(void)std::initializer_list<unsigned int>{index, (payload = p[index++])...};

static __forceinline__ __device__ void optixMakeHitObject(OptixTraversableHandle handle,
float3 rayOrigin,
float3 rayDirection,
float tmin,
float rayTime,
unsigned int rayFlags,
OptixTraverseData traverseData,
const OptixTraversableHandle* transforms,
unsigned int numTransforms)
{
float ox = rayOrigin.x, oy = rayOrigin.y, oz = rayOrigin.z;
float dx = rayDirection.x, dy = rayDirection.y, dz = rayDirection.z;

asm volatile(
"call"
"(),"

 "_optix_hitobject_make_with_traverse_data_v2,"

("%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29
:
: "l"(handle), "f"(ox), "f"(oy), "f"(oz), "f"(dx), "f"(dy), "f"(dz), "f"(tmin), "f"(rayTime),
"r"(rayFlags),
"r"(traverseData.data[0]), "r"(traverseData.data[1]), "r"(traverseData.data[2]),
"r"(traverseData.data[3]), "r"(traverseData.data[4]), "r"(traverseData.data[5]),
"r"(traverseData.data[6]), "r"(traverseData.data[7]), "r"(traverseData.data[8]),
"r"(traverseData.data[9]), "r"(traverseData.data[10]), "r"(traverseData.data[11]),
"r"(traverseData.data[12]), "r"(traverseData.data[13]), "r"(traverseData.data[14]),
"r"(traverseData.data[15]), "r"(traverseData.data[16]), "r"(traverseData.data[17]),
"r"(traverseData.data[18]), "r"(traverseData.data[19]), "l"(transforms), "r"(numTransforms)
:
}

static __forceinline__ __device__ void optixMakeMissHitObject(unsigned int missSBTIndex,
float3 rayOrigin,
float3 rayDirection,
float tmin,
float tmax,
float rayTime,
unsigned int rayFlags)
{
float ox = rayOrigin.x, oy = rayOrigin.y, oz = rayOrigin.z;
float dx = rayDirection.x, dy = rayDirection.y, dz = rayDirection.z;

asm volatile(
"call"
"(),"

 "_optix_hitobject_make_miss_v2,"

("%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10);"
:
: "r"(missSBTIndex), "f"(ox), "f"(oy), "f"(oz), "f"(dx), "f"(dy), "f"(dz), "f(tmin)",
"f"(tmax), "f"(rayTime), "r"(rayFlags)
:
}

static __forceinline__ __device__ void optixMakeNopHitObject()
{
asm volatile(

---

"call"
"(),"

 "_optix_hitobject_make_nop,"
"();"

:
:
:

):
}

static __forceinline__ __device__ void optixHitObjectGetTraverseData(OptixTraverseData* data)
{
    asm volatile(
        "call"
        "(%0,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19),"
        "_optix_hitobject_get_traverse_data,"
        "();"
        : "=r"(data->data[0]), "=r"(data->data[1]), "=r"(data->data[2]), "=r"(data->data[3]),
"=r"(data->data[4]),
        "=r"(data->data[5]), "=r"(data->data[6]), "=r"(data->data[7]), "=r"(data->data[8]),
"=r"(data->data[9]),
        "=r"(data->data[10]), "=r"(data->data[11]), "=r"(data->data[12]), "=r"(data->data[13]),
"=r"(data->data[14]),
        "=r"(data->data[15]), "=r"(data->data[16]), "=r"(data->data[17]), "=r"(data->data[18]),
"=r"(data->data[19])
        :
        :);
}

static __forceinline__ __device__ bool optixHitObjectIsHit()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_is_hit,"
        "();"
        : "=r"(result)
        :
        :);
    return result;
}

static __forceinline__ __device__ bool optixHitObjectIsMiss()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_is_miss,"
        "();"
        : "=r"(result)
        :
        :);
    return result;
}

static __forceinline__ __device__ bool optixHitObjectIsNop()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_is_nop,"
        "();"
        : "=r"(result)
        :
        :);
    return result;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetInstanceId()
{
    unsigned int result;

---

asm volatile(
    "call (%0), _optix_hitobject_get_instance_id,"
    "();"
    : "=r"(result)
    :
    :);
return result;

static __forceinline__ __device__ unsigned int optixHitObjectGetInstanceIndex()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_get_instance_idx,"
        "();"
        : "=r"(result)
        :
        :);
return result;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetPrimitiveIndex()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_get_primitive_idx,"
        "();"
        : "=r"(result)
        :
        :);
return result;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetTransformListSize()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_get_transform_list_size,"
        "();"
        : "=r"(result)
        :
        :);
return result;
}

static __forceinline__ __device__ OptixTraversableHandle optixHitObjectGetTransformListHandle(unsigned
int index)
{
    unsigned long long result;
    asm volatile(
        "call (%0), _optix_hitobject_get_transform_list_handle,"
        "(%1);"
        : "=l"(result)
        : "r"(index)
        :);
return result;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetSbtGASIndex()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_get_sbt_gas_idx,"
        "();"
        : "=r"(result)
        :
        :);

---

return result;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetHitKind()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_get_hitkind,"
        "();"
        : "=r"(result)
        :
        :);
    return result;
}

static __forceinline__ __device__ float3 optixHitObjectGetWorldRayOrigin()
{
    float x, y, z;
    asm volatile(
        "call (%0), _optix_hitobject_get_world_ray_origin_x,"
        "();"
        : "=f"(x)
        :
        :);
    asm volatile(
        "call (%0), _optix_hitobject_get_world_ray_origin_y,"
        "();"
        : "=f"(y)
        :
        :);
    asm volatile(
        "call (%0), _optix_hitobject_get_world_ray_origin_z,"
        "();"
        : "=f"(z)
        :
        :);
    return make_float3(x, y, z);
}

static __forceinline__ __device__ float3 optixHitObjectGetWorldRayDirection()
{
    float x, y, z;
    asm volatile(
        "call (%0), _optix_hitobject_get_world_ray_direction_x,"
        "();"
        : "=f"(x)
        :
        :);
    asm volatile(
        "call (%0), _optix_hitobject_get_world_ray_direction_y,"
        "();"
        : "=f"(y)
        :
        :);
    asm volatile(
        "call (%0), _optix_hitobject_get_world_ray_direction_z,"
        "();"
        : "=f"(z)
        :
        :);
    return make_float3(x, y, z);
}

static __forceinline__ __device__ float optixHitObjectGetRayTmin()
{
    float result;
    asm volatile(

---

"call (%0), _optix_hitobject_get_ray_tmin,"
"();"
: "=f"(result)
:
:);
return result;
}

static __forceinline__ __device__ float optixHitObjectGetRayTmax()
{
float result;
asm volatile(
    "call (%0), _optix_hitobject_get_ray_tmax,"
    "();"
    : "=f"(result)
    :
:);
return result;
}

static __forceinline__ __device__ float optixHitObjectGetRayTime()
{
float result;
asm volatile(
    "call (%0), _optix_hitobject_get_ray_time,"
    "();"
    : "=f"(result)
    :
:);
return result;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_0()
{
unsigned int ret;
asm volatile(
    "call (%0), _optix_hitobject_get_attribute,"
    "(%1);"
    : "=r"(ret)
    : "r"(0)
    :);
return ret;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_1()
{
unsigned int ret;
asm volatile(
    "call (%0), _optix_hitobject_get_attribute,"
    "(%1);"
    : "=r"(ret)
    : "r"(1)
    :);
return ret;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_2()
{
unsigned int ret;
asm volatile(
    "call (%0), _optix_hitobject_get_attribute,"
    "(%1);"
    : "=r"(ret)
    : "r"(2)
    :);
return ret;
} static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_3()
{
    unsigned int ret;
    asm volatile(
        "call (%0), _optix_hitobject_get_attribute,"
        "(%1);"
        : "=r"(ret)
        : "r"(3)
        :);
    return ret;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_4()
{
    unsigned int ret;
    asm volatile(
        "call (%0), _optix_hitobject_get_attribute,"
        "(%1);"
        : "=r"(ret)
        : "r"(4)
        :);
    return ret;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_5()
{
    unsigned int ret;
    asm volatile(
        "call (%0), _optix_hitobject_get_attribute,"
        "(%1);"
        : "=r"(ret)
        : "r"(5)
        :);
    return ret;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_6()
{
    unsigned int ret;
    asm volatile(
        "call (%0), _optix_hitobject_get_attribute,"
        "(%1);"
        : "=r"(ret)
        : "r"(6)
        :);
    return ret;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_7()
{
    unsigned int ret;
    asm volatile(
        "call (%0), _optix_hitobject_get_attribute,"
        "(%1);"
        : "=r"(ret)
        : "r"(7)
        :);
    return ret;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetSbtRecordIndex()
{
    unsigned int result;
    asm volatile(
        "call (%0), _optix_hitobject_get_sbt_record_index,"
        "();"
    }
}

---

: "=r"(result)
:
):
return result;
}

static __forceinline__ __device__ void optixHitObjectSetSbtRecordIndex(unsigned int sbtRecordIndex)
{
    asm volatile(
        "call (), _optix_hitobject_set_sbt_record_index,"
        "(%0);"
        :
        : "r"(sbtRecordIndex)
        :);
}

static __forceinline__ __device__ CUdeviceptr optixHitObjectGetSbtDataPointer()
{
    unsigned long long ptr;
    asm volatile(
        "call (%0), _optix_hitobject_get_sbt_data_pointer,"
        "();"
        : "=l"(ptr)
        :
        :);
    return ptr;
}

static __forceinline__ __device__ OptixTraversableHandle optixHitObjectGetGASTraversableHandle()
{
    unsigned long long handle;
    asm("call (%0), _optix_hitobject_get_gas_traversable_handle, ();" : "=l"(handle) :);
    return (OptixTraversableHandle)handle;
}

static __forceinline__ __device__ unsigned int optixHitObjectGetRayFlags()
{
    unsigned int u0;
    asm("call (%0), _optix_hitobject_get_ray_flags, ();" : "=r"(u0) :);
    return u0;
}

static __forceinline__ __device__ void optixSetPayload_0(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(0), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_1(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(1), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_2(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(2), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_3(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(3), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_4(unsigned int p)
{ asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(4), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_5(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(5), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_6(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(6), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_7(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(7), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_8(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(8), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_9(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(9), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_10(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(10), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_11(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(11), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_12(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(12), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_13(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(13), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_14(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(14), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_15(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(15), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_16(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(16), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_17(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(17), "r"(p) :);
} static __forceinline__ __device__ void optixSetPayload_18(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(18), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_19(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(19), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_20(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(20), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_21(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(21), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_22(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(22), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_23(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(23), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_24(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(24), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_25(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(25), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_26(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(26), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_27(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(27), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_28(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(28), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_29(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(29), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_30(unsigned int p)
{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(30), "r"(p) :);
}

static __forceinline__ __device__ void optixSetPayload_31(unsigned int p)

---

{
    asm volatile("call _optix_set_payload, (%0, %1);" : : "r"(31), "r"(p) :);
}

static __forceinline__ __device__ unsigned int optixGetPayload_0()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(0) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_1()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(1) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_2()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(2) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_3()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(3) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_4()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(4) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_5()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(5) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_6()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(6) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_7()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(7) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_8()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : :=r"(result) : "r"(8) :);
    return result;
} static __forceinline__ __device__ unsigned int optixGetPayload_9()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(9) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_10()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(10) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_11()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(11) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_12()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(12) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_13()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(13) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_14()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(14) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_15()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(15) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_16()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(16) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_17()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(17) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_18()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(18) :);

---

return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_19()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(19) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_20()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(20) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_21()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(21) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_22()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(22) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_23()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(23) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_24()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(24) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_25()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(25) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_26()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(26) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_27()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(27) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_28()

---

{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(28) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_29()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(29) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_30()
{
    unsigned int result;
    asm volatile("call (%0), _optix_get_payload, (%1);" : "=r"(result) : "r"(30) :);
    return result;
}

static __forceinline__ __device__ unsigned int optixGetPayload_31()
{
    unsigned int result;
    asm volatile("call _optix_set_payload_types, (%0);" : : "r"(types) :);
}

static __forceinline__ __device__ unsigned int optixUndefinedValue()
{
    unsigned int u0;
    asm("call (%0), _optix_undef_value, ();" : "=r"(u0) :);
    return u0;
}

__device__ __forceinline__ unsigned int optixGetRemainingTraceDepth()
{
    unsigned int result;
    asm (
        "call (%0), _optix_get_remaining_trace_depth,
        "();"
        : "=r"(result)
        :
        :);
    return result;
}

static __forceinline__ __device__ float3 optixGetWorldRayOrigin()
{
    float f0, f1, f2;
    asm("call (%0), _optix_get_world_ray_origin_x, ();" : "=f"(f0) :);
    asm("call (%0), _optix_get_world_ray_origin_y, ();" : "=f"(f1) :);
    asm("call (%0), _optix_get_world_ray_origin_z, ();" : "=f"(f2) :);
    return make_float3(f0, f1, f2);
}

static __forceinline__ __device__ float3 optixGetWorldRayDirection()
{
    float f0, f1, f2;
    asm("call (%0), _optix_get_world_ray_direction_x, ();" : "=f"(f0) :);
    asm("call (%0), _optix_get_world_ray_direction_y, ();" : "=f"(f1) :);
    asm("call (%0), _optix_get_world_ray_direction_z, ();" : "=f"(f2) :);
    return make_float3(f0, f1, f2);

---

}

static __forceinline__ __device__ float3 optixGetObjectRayOrigin()
{
    float f0, f1, f2;
    asm("call (%0), _optix_get_object_ray_origin_x, ();" : "=f"(f0) :);
    asm("call (%0), _optix_get_object_ray_origin_y, ();" : "=f"(f1) :);
    asm("call (%0), _optix_get_object_ray_origin_z, ();" : "=f"(f2) :);
    return make_float3(f0, f1, f2);
}

static __forceinline__ __device__ float3 optixGetObjectRayDirection()
{
    float f0, f1, f2;
    asm("call (%0), _optix_get_object_ray_direction_x, ();" : "=f"(f0) :);
    asm("call (%0), _optix_get_object_ray_direction_y, ();" : "=f"(f1) :);
    asm("call (%0), _optix_get_object_ray_direction_z, ();" : "=f"(f2) :);
    return make_float3(f0, f1, f2);
}

static __forceinline__ __device__ float optixGetRayTmin()
{
    float f0;
    asm("call (%0), _optix_get_ray_tmin, ();" : "=f"(f0) :);
    return f0;
}

static __forceinline__ __device__ float optixGetRayTmax()
{
    float f0;
    asm("call (%0), _optix_get_ray_tmax, ();" : "=f"(f0) :);
    return f0;
}

static __forceinline__ __device__ float optixGetRayTime()
{
    float f0;
    asm("call (%0), _optix_get_ray_time, ();" : "=f"(f0) :);
    return f0;
}

static __forceinline__ __device__ unsigned int optixGetRayFlags()
{
    unsigned int u0;
    asm("call (%0), _optix_get_ray_flags, ();" : "=r"(u0) :);
    return u0;
}

static __forceinline__ __device__ unsigned int optixGetRayVisibilityMask()
{
    unsigned int u0;
    asm("call (%0), _optix_get_ray_visibility_mask, ();" : "=r"(u0) :);
    return u0;
}

static __forceinline__ __device__ OptixTraversableHandle
optixGetInstanceTraversableFromIAS(OptixTraversableHandle ias,

unsigned int
instIdx)
{
    unsigned long long handle;
    asm("call (%0), _optix_get_instance_traversable_from_ias, (%1, %2);"
        : "=l"(handle) : "l"(ias), "r"(instIdx));
    return (OptixTraversableHandle)handle;
}

---

```c
static __forceinline__ __device__ void optixGetTriangleVertexData(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float                 time,
                                                   float3                  data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8), _optix_get_triangle_vertex_data, "
        "(%9, %10, %11, %12);"
        : "=f((data[0].x), " =f((data[0].y), " =f((data[0].z), " =f((data[1].x), " =f((data[1].y),
        " =f((data[1].z), " =f((data[2].x), " =f((data[2].y), " =f((data[2].z)
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetTriangleVertexDataFromHandle(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float                 time,
                                                   float3                  data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8), _optix_get_triangle_vertex_data_from_handle, "
        "(%9, %10, %11, %12);"
        : "=f((data[0].x), " =f((data[0].y), " =f((data[0].z), " =f((data[1].x), " =f((data[1].y),
        " =f((data[1].z), " =f((data[2].x), " =f((data[2].y), " =f((data[2].z)
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetTriangleVertexData(float3 data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8), _optix_get_triangle_vertex_data_current_hit, "
        "();"
        : "=f((data[0].x), " =f((data[0].y), " =f((data[0].z), " =f((data[1].x), " =f((data[1].y),
        " =f((data[1].z), " =f((data[2].x), " =f((data[2].y), " =f((data[2].z)
        :);
}

static __forceinline__ __device__ void optixHitObjectGetTriangleVertexData(float3 data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8), _optix_hitobject_get_triangle_vertex_data, "
        "();"
        : "=f((data[0].x), " =f((data[0].y), " =f((data[0].z), " =f((data[1].x), " =f((data[1].y),
        " =f((data[1].z), " =f((data[2].x), " =f((data[2].y), " =f((data[2].z)
        :);
}

static __forceinline__ __device__ void optixGetLinearCurveVertexData(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float                 time,
                                                   float4                  data[2])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7), _optix_get_linear_curve_vertex_data, "
        "(%8, %9, %10, %11);"
        : "=f((data[0].x), " =f((data[0].y), " =f((data[0].z), " =f((data[0].w),
        " =f((data[1].x), " =f((data[1

---

```cpp
float        time,
float4       data[2])

{
  asm("call (%0, %1, %2, %3, %4, %5, %6, %7), _optix_get_linear_curve_vertex_data_from_handle, "
    "(%8, %9, %10, %11);"
    : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w),
    "=f(data[1].x), "=f(data[1].y), "=f(data[1].z), "=f(data[1].w)
    : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f(time)
    :");
}

static __forceinline__ __device__ void optixGetLinearCurveVertexData(float4 data[2])

{
  asm("call (%0, %1, %2, %3, %4, %5, %6, %7), _optix_get_linear_curve_vertex_data_current_hit, "
    ":)"
    : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w),
    "=f(data[1].x), "=f(data[1].y), "=f(data[1].z), "=f(data[1].w)
    :);
}

static __forceinline__ __device__ void optixHitObjectGetLinearCurveVertexData(float4 data[2])

{
  asm("call (%0, %1, %2, %3, %4, %5, %6, %7), _optix_hitobject_get_linear_curve_vertex_data, "
    ":)"
    : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w),
    "=f(data[1].x), "=f(data[1].y), "=f(data[1].z), "=f(data[1].w)
    :);
}

static __forceinline__ __device__ void optixGetQuadraticBSplineVertexData(OptixTraversableHandle gas,
                                                   unsigned int   primIdx,
                                                   unsigned int     sbtGASIndex,
                                                   float           time,
                                                   float4          data[3])

{
  asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
  optix_get_quadratic_bspline_vertex_data, "
    "(%12, %13, %14, %15);"
    : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w),
    "=f(data[1].x), "=f(data[1].y), "=f(data[1].z), "=f(data[1].w),
    "=f(data[2].x), "=f(data[2].y), "=f(data[2].z), "=f(data[2].w)
    : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f(time)
    :);
}

static __forceinline__ __device__ void optixGetQuadraticBSplineVertexDataFromHandle(OptixTraversableHandle gas,
                                                   unsigned int   primIdx,
                                                   unsigned int     sbtGASIndex,
                                                   float           time,
                                                   float4          data[3])

{
  asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
  optix_get_quadratic_bspline_vertex_data_from_handle, "
    "(%12, %13, %14, %15);"
    : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w),
    "=f(data[1].x), "=f(data[1].y), "=f(data[1].z), "=f(data[1].w),
    "=f(data[2].x), "=f(data[2].y), "=f(data[2].z), "=f(data[

---

```c
optix_get_quadratic_bspline_vertex_data_current_hit, "
    "();"
    : =f(data[0].x), =f(data[0].y), =f(data[0].z), =f(data[0].w),
        =f(data[1].x), =f(data[1].y), =f(data[1].z), =f(data[1].w),
        =f(data[2].x), =f(data[2].y), =f(data[2].z), =f(data[2].w)
    :);
}

static __forceinline__ __device__ void optixHitObjectGetQuadraticBSplineVertexData(float4 data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
optix_hitobject_get_quadratic_bspline_vertex_data,
    "()");
    : =f(data[0].x), =f(data[0].y), =f(data[0].z), =f(data[0].w),
        =f(data[1].x), =f(data[1].y), =f(data[1].z), =f(data[1].w),
        =f(data[2].x), =f(data[2].y), =f(data[2].z), =f(data[2].w)
    :);
}

static __forceinline__ __device__ void
optixGetQuadraticBSplineRocapsVertexDataFromHandle(OptixTraversableHandle gas,

                                        unsigned int primIdx,
                                        unsigned int
sbtGASIndex,

                                        float      time,
                                        float4       data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
optix_get_quadratic_bspline_rocaps_vertex_data_from_handle,
    "(%12, %13, %14, %15)";
    : =f(data[0].x), =f(data[0].y), =f(data[0].z), =f(data[0].w), =f(data[1].x),
(data[1].y),
    =f(data[1].z), =f(data[1].w), =f(data[2].x), =f(data[2].y), =f(data[2].z),
=f(data[2].w)
    : "1(gas), r"(primIdx), "r"(sbtGASIndex), "f"(time)
    :);
}

static __forceinline__ __device__ void optixGetQuadraticBSplineRocapsVertexData(float4 data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
optix_get_quadratic_bspline_rocaps_vertex_data_current_hit,
    "()");
    : =f(data[0].x), =f(data[0].y), =f(data[0].z), =f(data[0].w), =f(data[1].x),
=f(data[1].y),
    =f(data[1].z), =f(data[1].w), =f(data[2].x), =f(data[2].y), =f(data[2].z),
=f(data[2].w)
    :);
}

static __forceinline__ __device__ void optixHitObjectGetQuadraticBSplineRocapsVertexData(float4 data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
optix_hitobject_get_quadratic_bspline_rocaps_vertex_data,
    "()");
    : =f(data[0].x), =f(data[0].y), =f(data[0].z), =f(data[0].w), =f(data[1].x),
=f(data[1].y),
    =f(data[1].z), =f(data[1].w), =f(data[2].x), =f(data[2].y), =f(data[2].z),
=f(data[2].w)
    :);
}

static __forceinline__ __device__ void optixGetCubicBSplineVertexData(OptixTraversableHandle gas

---

```c
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubicBSpline_vertex_data, "
        "(%16, %17, %18, %19);"
        : =f("data[0].x", =f("data[0].y"), =f("data[0].z"), =f("data[0].w"),
            =f("data[1].x"), =f("data[1].y"), =f("data[1].z"), =f("data[1].w"),
            =f("data[2].x"), =f("data[2].y"), =f("data[2].z"), =f("data[2].w"),
            =f("data[3].x"), =f("data[3].y"), =f("data[3].z"), =f("data[3].w")
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetCubicBSplineVertexDataFromHandle(OptixTraversableHandle gas,
                                                          unsigned int           primIdx,
                                                          unsigned int           sbtGASIndex,
                                                          float                 time,
                                                          float4                  data[4])

{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubicBSpline_vertex_data_from_handle, "
        "(%16, %17, %18, %19);"
        : =f("data[0].x", =f("data[0].y"), =f("data[0].z"), =f("data[0].w"),
            =f("data[1].x"), =f("data[1].y"), =f("data[1].z"), =f("data[1].w"),
            =f("data[2].x"), =f("data[2].y"), =f("data[2].z"), =f("data[2].w"),
            =f("data[3].x"), =f("data[3].y"), =f("data[3].z"), =f("data[3].w")
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetCubicBSplineVertexData(float4 data[4])

{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubicBSpline_vertex_data_current_hit, "
        (");
        : =f("data[0].x", =f("data[0].y"), =f("data[0].z"), =f("data[0].w"),
            =f("data[1].x"), =f("data[1].y"), =f("data[1].z"), =f("data[1].w"),
            =f("data[2].x"), =f("data[2].y"), =f("data[2].z"), =f("data[2].w"),
            =f("data[3].x"), =f("data[3].y"), =f("data[3].z"), =f("data[3].w")
        :);
}

static __forceinline__ __device__ void optixHitObjectGetCubicBSplineVertexData(float4 data[4])

{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_hitobject_get_cubicBSpline_vertex_data, "
        ();
        : =f("data[0].x", =f("data[0].y"), =f("data[0].z"), =f("data[0].

---

<u>321</u>

```c
        "=f((data[2].z), "=f((data[2].w), "=f((data[3].x), "=f((data[3].y), "=f((data[3].z),
=f(data[3].w)
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetCubicBSplineRocapsVertexData(float4 data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubic_bspline_rocaps_vertex_data_current_hit,
        "();"
        : "=f((data[0].x), "=f((data[0].y), "=f((data[0].z), "=f((data[0].w), "=f((data[1].x),
        "=f((data[1].y), "=f((data[1].z), "=f((data[1].w), "=f((data[2].x), "=f((data[2].y),
        "=f((data[2].z), "=f((数据[2].w), "=f((数据[3].x), "=f((数据[3].y), "=f((数据[3].z),
=f(data[3].w)
        :);
}

static __forceinline__ __device__ void optixHitObjectGetCubicBSplineRocapsVertexData(float4 data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_hitobject_get_cubic_bspline_rocaps_vertex_data,
        "();"
        : "=f((data[0].x), "=f((data[0].y), "=f((数据[0].z), "=f((数据[0].w), "=f((数据[1].x),
        "=f((数据[1].y), "=f((数据[1].z), "=f((数据[1].w), "=f((数据[2].x), "=f((数据[2].y),
        "=f((数据[2].z), "=f((数据[2].w), "=f((数据[3].x), "=f((数据[3].y), "=f((数据[3].z),
=f(data[3].w)
        :);
}

static __forceinline__ __device__ void optixGetCatmullRomVertexData(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float               time,
                                                   float4                data[4])

{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_catmullrom_vertex_data,
        "(%16, %17, %18, %19);"
        : "=f((数据[0].x), "=f((数据[0].y), "=f((数据[0].z), "=f((数据[0].w), "=f((数据[1].x),
        "=f((数据[1].y), "=f((数据[1].z), "=f((数据[1].w), "=f((数据[2].x), "=f((数据[2].y),
        "=f((数据[2].z), "=f((数据[2].w), "=f((数据[3].x), "=f((数据[3].y), "=f((数据[3].z),
=f(data[3].w)
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetCatmullRomVertexDataFrom

---

<u>322</u>

```cpp
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "__optix_get_catmullrom_vertex_data_current_hit, "
        "(");
    : =f("data[0].x), =f("data[0].y), =f("data[0].z), =f("data[0].w), =f("data[1].x),
        =f("data[1].y), =f("data[1].z), =f("data[1].w), =f("data[2].x), =f("data[2].y),
        =f("data[2].z), =f("data[2].w), =f("data[3].x), =f("data[3].y), =f("data[3].z)",
=f("data[3].w)
        :);
}

static __forceinline__ __device__ void optixHitObjectGetCatmullRomVertexData(float4 data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "__optix_hitobject_get_catmullrom_vertex_data,
        "(");
    : =f("data[0].x), =f("data[0].y), =f("data[0].z), =f("data[0].w), =f("data[1].x),
        =f("data[1].y), =f("data[1].z), =f("data[1].w), =f("data[2].x), =f("data[2].y),
        =f("data[2].z), =f("data[2].w), =f("data[3].x), =f("data[3].y), =f("data[3].z)",
=f("data[3].w)
        :);
}

static __forceinline__ __device__ void optixGetCatmullRomRocapsVertexDataFromHandle(OptixTraversableHandle gas,
                                                          unsigned int
primIdx,
                                                          unsigned int
sbtGASIndex,

                                                          float           time
                                                          float4
data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "__optix_get_catmullrom_rocaps_vertex_data_from_handle,
        "(%16, %17, %18, %19);"
    : =f("data[0].x), =f("data[0].y), =f("data[0].z), =f("data[0].w), =f("data[1].x),
        =f("data[1].y), =f("data[1].z), =f("data[1].w), =f("data[2].x), =f("data[2].y),
        =f("data[2].z), =f("data[2].w), =f("data[3].x), =f("data[3].y), =f("data[3].z)",
=f("data[3].w)
    : l("gas), r"(primIdx), r"(sbtGASIndex), f"(time)
    :);
}

static __forceinline__ __device__ void optixGetCatmullRomRocapsVertexData(float4 data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "__optix_get_catmullrom_rocaps_vertex_data_hit,
        "(");
    : =f("data[0].x), =f("data[0].y), =f("数据[0].z), =f("数据[0].w), =f("数据[1].x),
        =f("数据[1].y), =f("数据[1].z), =f("数据[1].w),

---

<u>323</u>

```c
static __forceinline__ __device__ void optixGetCubicBezierVertexData(OptixTraversableHandle gas,
                                                   unsigned int       primIdx,
                                                   unsigned int       sbtGASIndex,
                                                   float           time,
                                                   float4          data[4])

{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubic_bezier_vertex_data, "
        "(%16, %17, %18, %19);"
        : "=f"(data[0].x), "=f"(data[0].y), "=f"(data[0].z), "=f"(data[0].w), "=f"(data[1].x),
        "=f"(data[1].y), "=f"(data[1].z), "=f"(data[1].w), "=f"(data[2].x), "=f"(data[2].y),
        "=f"(data[2].z), "=f"(data[2].w), "=f"(data[3].x), "=f"(data[3].y), "=f"(data[3].z),
        "=f"(data[3].w)
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetCubicBezierVertexDataFromHandle(OptixTraversableHandle gas,
                                                   unsigned int       primIdx,
                                                   unsigned int       sbtGASIndex,
                                                   float           time,
                                                   float4          data[4])

{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubic_bezier_vertex_data_from_handle, "
        "(%16, %17, %18, %19);"
        : "=f"(data[0].x), "=f"(data[0].y), "=f"(数据[0].z), "=f"(数据[0].w), "=f"(数据[1].x),
        "=f"(数据[1].y), "=f"(数据[1].z), "=f"(数据[1].w), "=f"(数据[2].x), "=f"(数据[2].y),
        "=f"(数据[2].z), "=f"(数据[2].w), "=f"(数据[3].x), "=f"(数据[3].y), "=f"(数据[3].z),
        "=f"(数据[3].w)
        : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
        :);
}

static __forceinline__ __device__ void optixGetCubicBezierVertexData(float4 data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubic_bezier_vertex_data_current_hit, "
        "();"
        : "=f"(数据[0].x), "=f"(数据[0].y), "=f"(数据[0].z), "=f"(数据[0].w), "=f"(数据[1].x),
        "=f"(数据[1].y), "=f"(数据[1].z), "=f"(数据[1].w), "=f"(数据[2].x), "=f"(数据[2].y),
        "=f"(数据[2].z), "=f"(数据[2].w), "=f"(数据[3].x), "=f"(数据[3].y), "=f"(数据[3].z),
        "=f"(数据[3].w)
        :);
}

static __forceinline__ __device__ void optixHitObjectGetCubicBezierVertexData(float4 data[4])
{

---

<u>324</u>

```c
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubic_bezier_rocaps_vertex_data_from_handle,
        "(%16, %17, %18, %19);"
    : "f(data[0].x), "f(data[0].y), "f(data[0].z), "f(data[0].w), "f(data[1].x),
        "f(data[1].y), "f(data[1].z), "f(data[1].w), "f(data[2].x), "f(data[2].y),
        "f(data[2].z), "f(data[2].w), "f(data[3].x), "f(data[3].y), "f(data[3].z),
=f(data[3].w)
    : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f"(time)
    :);
}

static __forceinline__ __device__ void optixGetCubicBezierRocapsVertexData(float4 data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_get_cubic_bezier_rocaps_vertex_data_current_hit,
        "();
    : "f(data[0].x), "f(data[0].y), "f(data[0].z), "f(data[0].w), "f(data[1].x),
        "f(data[1].y), "f(data[1].z), "f(data[1].w), "f(data[2].x), "f(data[2].y),
        "f(data[2].z), "f(data[2].w), "f(data[3].x), "f(data[3].y), "f(data[3].z),
=f(data[3].w)
    :);
}

static __forceinline__ __device__ void optixHitObjectGetCubicBezierRocapsVertexData(float4 data[4])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12, %13, %14, %15), "
        "_optix_hitobject_get_cubic_bezier_rocaps_vertex_data,
        "();
    : "f(data[0].x), "f(data[0].y), "f(data[0].z), "f(data[0].w), "f(data[1].x),
        "f(data[1].y), "f(data[1].z), "f(data[1].w), "f(data[2].x), "f(data[2].y),
        "f(data[2].z), "f(data[2].w), "f(data[3].x), "f(data[3].y), "f(data[3].z),
=f(data[3].w)
    :);
}

static __forceinline__ __device__ void optixGetRibbonVertexData(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float               time,
                                                   float4                data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11), _optix_get_ribbon_vertex_data, "
        "(%12, %13, %14, %15);"
    : "f(data[0].x), "f(data[0].y), "f(data[0].z), "f(data[0].w), "f(data[1].x),
        "f(data[1].y), "f(data[1].z), "f(data[1].w), "f(data[2].x), "f(data[2].y), "f(data[2].z),
        "f(data[2].w)
    : "l"(gas), "r"(primIdx), "r"(sbtGASIndex), "f

---

<u>325</u>

1773static__forceinline__ __device__ float3optixGetRibbonNormal(OptixTraversableHandlegas,

```c
static __forceinline__ __device__ void optixGetRibbonVertexData(float4 data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
_optix_get_ribbon_vertex_data_current_hit,
" ();"
    : "=f(data[0].x), =f(data[0].y), =f(data[0].z), =f(data[0].w), =f(data[1].x),
=f(data[1].y),
    "=f(data[1].z), =f(data[1].w), =f(data[2].x), =f(data[2].y), =f(data[2].z),
=f(data[2].w)
    :);
}

static __forceinline__ __device__ void optixHitObjectGetRibbonVertexData(float4 data[3])
{
    asm("call (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11),
_optix_hitobject_get_ribbon_vertex_data,
" ();"
    : "=f(data[0].x), =f(data[0].y), =f(data[0].z), =f(data[0].w), =f(data[1].x),
=f(data[1].y),
    "=f(data[1].z), =f(data[1].w), =f(data[2].x), =f(data[2].y), =f(data[2].z),
=f(data[2].w)
    :);
}

static __forceinline__ __device__ float3 optixGetRibbonNormal(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float               time,
                                                   float2                ribbonParameters)
{
    float3 normal;
    asm("call (%0, %1, %2), _optix_get_ribbon_normal, "
        "(%3, %4, %5, %6, %7, %8);"
        : "=f(normal.x), =f(normal.y), =f(normal.z)
        : "l(gas), r"(primIdx), "r(sbtGASIndex), "f(time),
        "f(ribbonParameters.x), "f(ribbonParameters.y)
        :);
    return normal;
}

static __forceinline__ __device__ float3 optixGetRibbonNormalFromHandle(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float               time,
                                                   float2                ribbonParameters)
{
    float3 normal;
    asm("call (%0, %1, %2), _optix_get_ribbon_normal_from_handle, "
        "(%3, %4, %5, %6, %7, %8);"
        : "=f(normal.x), =f(normal.y), =f(normal.z)
        : "l(gas), r"(primIdx), "r(sbtGASIndex), "f(time),
        "f(ribbonParameters.x), "f(ribbonParameters.y)
        :);
    return normal;
}

static __forceinline__ __device__ float3 optixGetRibbonNormal(float2 ribbonParameters)
{
    float3 normal;
    asm("call (%0, %1, %2), _optix_get_ribbon_normal_current_hit, "
        "(%3, %4);"
        : "=f(normal.x), =f(normal.y), =f(normal.z)
        : "f(ribbonParameters.x), "f(ribbonParameters.y)
        :);
    return normal;
}

1789static__forceinline__ __device__ float3optixGetRibbonNormalFromHandle(OptixTraversableHandlegas,

---

<u>326</u>

```csharp
static __forceinline__ __device__ float3 optixHitObjectGetRibbonNormal(float2 ribbonParameters)
{
    float3 normal;
    asm("call (%0, %1, %2), _optix_hitobject_get_ribbon_normal, "
        "(%3, %4);"
        : "=f(normal.x), "=f(normal.y), "=f(normal.z)
        : "=f(ribbonParameters.x), "=f(ribbonParameters.y)
        :);
    return normal;
}

static __forceinline__ __device__ void optixGetSphereData(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float                 time,
                                                   float4                data[1])
{
    asm("call (%0, %1, %2, %3), "
        "_optix_get_sphere_data, "
        "(%4, %5, %6, %7);"
        : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w)
        : "=l(gas), "=r(primIdx), "=r(sbtGASIndex), "=f(time)
        :);
}

static __forceinline__ __device__ void optixGetSphereDataFromHandle(OptixTraversableHandle gas,
                                                   unsigned int           primIdx,
                                                   unsigned int           sbtGASIndex,
                                                   float                 time,
                                                   float4                data[1])
{
    asm("call (%0, %1, %2, %3), "
        "_optix_get_sphere_data_from_handle, "
        "(%4, %5, %6, %7);"
        : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w)
        : "=l(gas), "=r(primIdx), "=r(sbtGASIndex), "=f(time)
        :);
}

static __forceinline__ __device__ void optixGetSphereData(float4 data[1])
{
    asm("call (%0, %1, %2, %3), "
        "_optix_get_sphere_data_current_hit, "
        "();"
        : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w)
        :);
}

static __forceinline__ __device__ void optixHitObjectGetSphereData(float4 data[1])
{
    asm("call (%0, %1, %2, %3), "
        "_optix_hitobject_get_sphere_data, "
        "();"
        : "=f(data[0].x), "=f(data[0].y), "=f(data[0].z), "=f(data[0].w)
        :);
}

static __forceinline__ __device__ OptixTraversableHandle optixGetGASTraversableHandle()
{
    unsigned long long handle;
    asm("call (%0), _optix_get_gas_traversable_handle, ();;" : "=l(handle) :);
    return (OptixTraversableHandle)handle;
}

static __forceinline__ __device__ float optixGetGASMotionTimeBegin(OptixTraversableHandle handle)
{ float f0;
asm("call (%0), _optix_get_gas_motion_time_begin, (%1);" : "=f"(f0) : "l"(handle) :);
return f0;
}

static __forceinline__ __device__ float optixGetGASMotionTimeEnd(OptixTraversableHandle handle)
{
    float f0;
    asm("call (%0), _optix_get_gas_motion_time_end, (%1);" : "=f"(f0) : "l"(handle) :);
    return f0;
}

static __forceinline__ __device__ unsigned int optixGetGASMotionStepCount(OptixTraversableHandle handle)
{
    unsigned int u0;
    asm("call (%0), _optix_get_gas_motion_step_count, (%1);" : "=r"(u0) : "l"(handle) :);
    return u0;
}

template<typename HitState>
static __forceinline__ __device__ void optixGetWorldToObjectTransformMatrix(const HitState& hs, float
m[12])
{
    if(hs.getTransformListSize() == 0)
    {
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        m[4] = 0.0f;
        m[5] = 1.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;
        return;
    }

    float4 m0, m1, m2;
    optix_impl::optixGetWorldToObjectTransformMatrix(hs, m0, m1, m2);
    m[0] = m0.x;
    m[1] = m0.y;
    m[2] = m0.z;
    m[3] = m0.w;
    m[4] = m1.x;
    m[5] = m1.y;
    m[6] = m1.z;
    m[7] = m1.w;
    m[8] = m2.x;
    m[9] = m2.y;
    m[10] = m2.z;
    m[11] = m2.w;
}

static __forceinline__ __device__ void optixGetWorldToObjectTransformMatrix(float m[12])
{
    optixGetWorldToObjectTransformMatrix(OptixIncomingHitObject{}, m);
}

static __forceinline__ __device__ void optixHitObjectGetWorldToObjectTransformMatrix(float m[12])
{
    optixGetWorldToObjectTransformMatrix(OptixOutgoingHitObject{}, m);
}

template<typename HitState> static __forceinline__ __device__ void optixGetObjectToWorldTransformMatrix(const HitState& hs, float
m[12])
{
    if(hs.getTransformListSize() == 0)
    {
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        m[4] = 0.0f;
        m[5] = 1.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;
        return;
    }

    float4 m0, m1, m2;
    optix_impl::optixGetObjectToWorldTransformMatrix(hs, m0, m1, m2);
    m[0] = m0.x;
    m[1] = m0.y;
    m[2] = m0.z;
    m[3] = m0.w;
    m[4] = m1.x;
    m[5] = m1.y;
    m[6] = m1.z;
    m[7] = m1.w;
    m[8] = m2.x;
    m[9] = m2.y;
    m[10] = m2.z;
    m[11] = m2.w;
}

static __forceinline__ __device__ void optixGetObjectToWorldTransformMatrix(float m[12])
{
    optixGetObjectToWorldTransformMatrix(OptixIncomingHitObject{}, m);
}

static __forceinline__ __device__ void optixHitObjectGetObjectToWorldTransformMatrix(float m[12])
{
    optixGetObjectToWorldTransformMatrix(OptixOutgoingHitObject{}, m);
}

template<typename HitState>
static __forceinline__ __device__ float3 optixTransformPointFromWorldToObjectSpace(const HitState& hs,
float3 point)
{
    if(hs.getTransformListSize() == 0)
        return point;

    float4 m0, m1, m2;
    optix_impl::optixGetWorldToObjectTransformMatrix(hs, m0, m1, m2);
    return optix_impl::optixTransformPoint(m0, m1, m2, point);
}

static __forceinline__ __device__ float3 optixTransformPointFromWorldToObjectSpace(float3 point)
{
    return optixTransformPointFromWorldToObjectSpace(OptixIncomingHitObject{}, point);
}

static __forceinline__ __device__ float3 optixHitObjectTransformPointFromWorldToObjectSpace(float3
point)
{
    return optixTransformPointFromWorldToObjectSpace(OptixOutgoingHitObject{}, point);

NVIDIA OptiX 9.1 API

---

}

template<typename HitState>
static __forceinline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace(const HitState& hs,
float3 vec)
{
    if(hs.getTransformListSize() == 0)
        return vec;

    float4 m0, m1, m2;
    optix_impl::optixGetWorldToObjectTransformMatrix(hs, m0, m1, m2);
    return optix_impl::optixTransformVector(m0, m1, m2, vec);
}

static __forceinline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace(float3 vec)
{
    return optixTransformVectorFromWorldToObjectSpace(OptixIncomingHitObject{}, vec);
}

static __forceinline__ __device__ float3 optixHitObjectTransformVectorFromWorldToObjectSpace(float3 vec)
{
    return optixTransformVectorFromWorldToObjectSpace(OptixOutgoingHitObject{}, vec);
}

template<typename HitState>
static __forceinline__ __device__ float3 optixTransformNormalFromWorldToObjectSpace(const HitState& hs,
float3 normal)
{
    if(hs.getTransformListSize() == 0)
        return normal;

    float4 m0, m1, m2;
    optix_impl::optixGetObjectToWorldTransformMatrix(hs, m0, m1, m2); // inverse of
optixGetWorldToObjectTransformMatrix()
    return optix_impl::optixTransformNormal(m0, m1, m2, normal);
}

static __forceinline__ __device__ float3 optixTransformNormalFromWorldToObjectSpace(float3 normal)
{
    return optixTransformNormalFromWorldToObjectSpace(OptixIncomingHitObject{}, normal);
}

static __forceinline__ __device__ float3 optixHitObjectTransformNormalFromWorldToObjectSpace(float3
normal)
{
    return optixTransformNormalFromWorldToObjectSpace(OptixOutgoingHitObject{}, normal);
}

template<typename HitState>
static __forceinline__ __device__ float3 optixTransformPointFromObjectToWorldSpace(const HitState& hs,
float3 point)
{
    if(hs.getTransformListSize() == 0)
        return point;

    float4 m0, m1, m2;
    optix_impl::optixGetObjectToWorldTransformMatrix(hs, m0, m1, m2);
    return optix_impl::optixTransformPoint(m0, m1, m2, point);
}

static __forceinline__ __device__ float3 optixTransformPointFromObjectToWorldSpace(float3 point)
{
    return optixTransformPointFromObjectToWorldSpace(OptixIncomingHitObject{}, point);
}

static __forceinline__ __device__ float3 optixHitObjectTransformPointFromObjectToWorldSpace(float3
point)

---

8.2 optix_device_impl.h

{
    return optixTransformPointFromObjectToWorldSpace(OptixOutgoingHitObject{}, point);
}

template<typename HitState>
static __forceinline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace(const HitState& hs,
float3 vec)
{
    if(hs.getTransformListSize() == 0)
        return vec;

    float4 m0, m1, m2;
    optix_impl::optixGetObjectToWorldTransformMatrix(hs, m0, m1, m2);
    return optix_impl::optixTransformVector(m0, m1, m2, vec);
}

static __forceinline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace(float3 vec)
{
    return optixTransformVectorFromObjectToWorldSpace(OptixIncomingHitObject{}, vec);
}

static __forceinline__ __device__ float3 optixHitObjectTransformVectorFromObjectToWorldSpace(float3 vec)
{
    return optixTransformVectorFromObjectToWorldSpace(OptixOutgoingHitObject{}, vec);
}

template<typename HitState>
static __forceinline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace(const HitState& hs,
float3 normal)
{
    if(hs.getTransformListSize() == 0)
        return normal;

    float4 m0, m1, m2;
    optix_impl::optixGetWorldToObjectTransformMatrix(hs, m0, m1, m2); // inverse of
optixGetObjectToWorldTransformMatrix()
    return optix_impl::optixTransformNormal(m0, m1, m2, normal);
}

static __forceinline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace(float3 normal)
{
    return optixTransformNormalFromObjectToWorldSpace(OptixIncomingHitObject{}, normal);
}

static __forceinline__ __device__ float3 optixHitObjectTransformNormalFromObjectToWorldSpace(float3
normal)
{
    return optixTransformNormalFromObjectToWorldSpace(OptixOutgoingHitObject{}, normal);
}

static __forceinline__ __device__ unsigned int optixGetTransformListSize()
{
    unsigned int u0;
    asm("call (%0), _optix_get_transform_list_size, ();;" : "=r"(u0) :);
    return u0;
}

static __forceinline__ __device__ OptixTraversableHandle optixGetTransformListHandle(unsigned int index)
{
    unsigned long long u0;
    asm("call (%0), _optix_get_transform_list_handle, (%1);;" : "=l"(u0) : "r"(index) :);
    return u0;
}

static __forceinline__ __device__ OptixTransformType
optixGetTransformTypeFromHandle(OptixTraversableHandle handle)
{ int i0;
asm("call (%0), _optix_get_transform_type_from_handle, (%1);" : "=r"(i0) : "l"(handle) :);
return (OptixTransformType)i0;

static __forceinline__ __device__ const OptixStaticTransform*
optixGetStaticTransformFromHandle(OptixTraversableHandle handle)
{
    unsigned long long ptr;
    asm("call (%0), _optix_get_static_transform_from_handle, (%1);" : "=l"(ptr) : "l"(handle) :);
    return (const OptixStaticTransform*)ptr;
}

static __forceinline__ __device__ const OptixSRTMotionTransform*
optixGetSRTMotionTransformFromHandle(OptixTraversableHandle handle)
{
    unsigned long long ptr;
    asm("call (%0), _optix_get_srt_motion_transform_from_handle, (%1);" : "=l"(ptr) : "l"(handle) :);
    return (const OptixSRTMotionTransform*)ptr;
}

static __forceinline__ __device__ const OptixMatrixMotionTransform*
optixGetMatrixMotionTransformFromHandle(OptixTraversableHandle handle)
{
    unsigned long long ptr;
    asm("call (%0), _optix_get_matrix_motion_transform_from_handle, (%1);" : "=l"(ptr) : "l"(handle) :);
    return (const OptixMatrixMotionTransform*)ptr;
}

static __forceinline__ __device__ unsigned int optixGetInstanceIdFromHandle(OptixTraversableHandle handle)
{
    int i0;
    asm("call (%0), _optix_get_instance_id_from_handle, (%1);" : "=r"(i0) : "l"(handle) :);
    return i0;
}

static __forceinline__ __device__ OptixTraversableHandle
optixGetInstanceChildFromHandle(OptixTraversableHandle handle)
{
    unsigned long long i0;
    asm("call (%0), _optix_get_instance_child_from_handle, (%1);" : "=l"(i0) : "l"(handle) :);
    return (OptixTraversableHandle)i0;
}

static __forceinline__ __device__ const float4*
optixGetInstanceTransformFromHandle(OptixTraversableHandle handle)
{
    unsigned long long ptr;
    asm("call (%0), _optix_get_instance_transform_from_handle, (%1);" : "=l"(ptr) : "l"(handle) :);
    return (const float4*)ptr;
}

static __forceinline__ __device__ const float4*
optixGetInstanceInverseTransformFromHandle(OptixTraversableHandle handle)
{
    unsigned long long ptr;
    asm("call (%0), _optix_get_instance_inverse_transform_from_handle, (%1);" : "=l"(ptr) : "l"(handle) :);
    return (const float4*)ptr;
}

static __device__ __forceinline__ CUdeviceptr optixGetGASPointerFromHandle(OptixTraversableHandle handle)
{
    unsigned long long ptr;
    asm("call (%0), _optix_get_gas_ptr_from_handle, (%1);" : "=l"(ptr) : "l"(handle) :);

NVIDIA OptiX 9.1 API return (CUdeviceptr)ptr;
}
static __forceinline__ __device__ bool optixReportIntersection(float hitT, unsigned int hitKind)
{
    int ret;
    asm volatile(
        "call (%0), _optix_report_intersection_0"
        ", (%1, %2);"
        : "=r"(ret)
        : "f"(hitT), "r"(hitKind)
        :);
    return ret;
}

static __forceinline__ __device__ bool optixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0)
{
    int ret;
    asm volatile(
        "call (%0), _optix_report_intersection_1"
        ", (%1, %2, %3);"
        : "=r"(ret)
        : "f"(hitT), "r"(hitKind), "r"(a0)
        :);
    return ret;
}

static __forceinline__ __device__ bool optixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1)
{
    int ret;
    asm volatile(
        "call (%0), _optix_report_intersection_2"
        ", (%1, %2, %3, %4);"
        : "=r"(ret)
        : "f"(hitT), "r"(hitKind), "r"(a0), "r"(a1)
        :);
    return ret;
}

static __forceinline__ __device__ bool optixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2)
{
    int ret;
    asm volatile(
        "call (%0), _optix_report_intersection_3"
        ", (%1, %2, %3, %4, %5);"
        : "=r"(ret)
        : "f"(hitT), "r"(hitKind), "r"(a0), "r"(a1), "r"(a2)
        :);
    return ret;
}

static __forceinline__ __device__ bool optixReportIntersection(float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1,
unsigned int a2,
unsigned int a3)
{
    int ret;
    asm volatile(
        "call (%0), _optix_report_intersection_4"
        ", (%1, %2, %3, %4, %5, %6);"
        : "=r"(ret)
        : "f"(hitT), "r"(hitKind), "r"(a0), "r"(a1), "r"(a2), "r"(a3)
        :);
}

---

<u>333</u>

2257returnret;
2258}

2259
2260static__forceinline__ __device__booloptixReportIntersection(floathitT,

2261
2262
2263
2264
2265
2266
2267 {
2268intret;

unsignedinthitKind,
unsignedinta0,
unsignedinta1,
unsignedinta2,
unsignedinta3,
unsignedinta4)

2269asmvolatile(
2270"call (%0), _optix_report_intersection_5"
2271", (%1, %2, %3, %4, %5, %6, %7);"

2272 :"=r"(ret)

2273 : "f"(hitT), "r"(hitKind), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4)

2274 :);
2275returnret;
2276}

2277
2278static__forceinline__ __device__booloptixReportIntersection(floathitT,
2279 unsignedinthitKind,

2280
2281
2282
2283
2284
2285
2286 {
2287intret;
2288asmvolatile(

unsignedinta0,
unsignedinta1,
unsignedinta2,
unsignedinta3,
unsignedinta4,
unsignedinta5)

```python
"call (%0), _optix_report_intersection_6"
", (%1, %2, %3, %4, %5, %6, %7, %8);"
: "=r"(ret)
: "f"(hitT), "r"(hitKind), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5)
:);

2294returnret;
2295}
2296

2297static__forceinline__ __device__booloptixReportIntersection(floathitT,

2298
2299
2300
2301
2302
2303
2304
2305
2306 {
2307intret;
2308asmvolatile(

unsignedinthitKind,
unsignedinta0,
unsignedinta1,
unsignedinta2,
unsignedinta3,
unsignedinta4,
unsignedinta5,
unsignedinta6)

2309"call (%0), _optix_report_intersection_7"
2310", (%1, %2, %3, %4, %5, %6, %7, %8, %9);"

2311 :"=r"(ret)

2312 : "f"(hitT), "r"(hitKind), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"

2313 :);
2314returnret;
2315}
2316

2317static__forceinline__ __device__booloptixReportIntersection(floathitT,
2318 unsignedinthitKind,

2319
2320
2321
2322
2323

unsignedinta0,
unsignedinta1,
unsignedinta2,
unsignedinta3,
unsignedinta4, unsigned int a5,
unsigned int a6,
unsigned int a7)

{
    int ret;
    asm volatile(
        "call (%0), _optix_report_intersection_8"
        ", (%1, %2, %3, %4, %5, %6, %7, %8, %9, %10);"
        : "=r"(ret)
        : "f"(hitT), "r"(hitKind), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        :);
    return ret;
}

#define OPTIX_DEFINE_optixGetAttribute_BODY(which)

unsigned int ret;

asm("call (%0), _optix_get_attribute_" #which ", ();;" : "=r"(ret) :);

return ret;

static __forceinline__ __device__ unsigned int optixGetAttribute_0()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(0);
}

static __forceinline__ __device__ unsigned int optixGetAttribute_1()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(1);
}

static __forceinline__ __device__ unsigned int optixGetAttribute_2()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(2);
}

static __forceinline__ __device__ unsigned int optixGetAttribute_3()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(3);
}

static __forceinline__ __device__ unsigned int optixGetAttribute_4()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(4);
}

static __forceinline__ __device__ unsigned int optixGetAttribute_5()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(5);
}

static __forceinline__ __device__ unsigned int optixGetAttribute_6()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(6);
}

static __forceinline__ __device__ unsigned int optixGetAttribute_7()
{
    OPTIX_DEFINE_optixGetAttribute_BODY(7);
}

#undef OPTIX_DEFINE_optixGetAttribute_BODY

static __forceinline__ __device__ void optixTerminateRay()
{
    asm volatile("call _optix_terminate_ray, ();");
}

---

}

static __forceinline__ __device__ void optixIgnoreIntersection()
{
    asm volatile("call _optix_ignore_intersection, ();");
}

static __forceline__ __device__ unsigned int optixGetPrimitiveIndex()
{
    unsigned int u0;
    asm("call (%0), _optix_read_primitive_idx, ();" : "=r"(u0) :);
    return u0;
}

static __forceline__ __device__ unsigned int optixGetClusterId()
{
    unsigned int u0;
    asm("call (%0), _optix_get_cluster_id, ();" : "=r"(u0) :);
    return u0;
}

static __forceline__ __device__ unsigned int optixHitObjectGetClusterId()
{
    unsigned int u0;
    asm("call (%0), _optix_hitobject_get_cluster_id, ();" : "=r"(u0) :);
    return u0;
}

static __forceline__ __device__ unsigned int optixGetSbtGASIndex()
{
    unsigned int u0;
    asm("call (%0), _optix_read_sbt_gas_idx, ();" : "=r"(u0) :);
    return u0;
}

static __forceline__ __device__ unsigned int optixGetInstanceId()
{
    unsigned int u0;
    asm("call (%0), _optix_read_instance_id, ();" : "=r"(u0) :);
    return u0;
}

static __forceline__ __device__ unsigned int optixGetInstanceIndex()
{
    unsigned int u0;
    asm("call (%0), _optix_read_instance_idx, ();" : "=r"(u0) :);
    return u0;
}

static __forceline__ __device__ unsigned int optixGetHitKind()
{
    unsigned int u0;
    asm("call (%0), _optix_get_hit_kind, ();" : "=r"(u0) :);
    return u0;
}

static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType(unsigned int hitKind)
{
    unsigned int u0;
    asm("call (%0), _optix_get_primitive_type_from_hit_kind, (%1);" : "=r"(u0) : "r"(hitKind));
    return (OptixPrimitiveType)u0;
}

static __forceline__ __device__ bool optixIsBackFaceHit(unsigned int hitKind)
{
    unsigned int u0;
    asm("call (%0), _optix_get_backface_from_hit_kind, (%1);" : "=r"(u0) : "r"(hitKind));

---

return (u0 == 0x1);
}

static __forceinline__ __device__ bool optixIsFrontFaceHit(unsigned int hitKind)
{
    return !optixIsBackFaceHit(hitKind);
}

static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType()
{
    return optixGetPrimitiveType(optixGetHitKind());
}

static __forceline__ __device__ bool optixIsBackFaceHit()
{
    return optixIsBackFaceHit(optixGetHitKind());
}

static __forceline__ __device__ bool optixIsFrontFaceHit()
{
    return optixIsFrontFaceHit(optixGetHitKind());
}

static __forceline__ __device__ bool optixIsTriangleHit()
{
    return optixIsTriangleFrontFaceHit() || optixIsTriangleBackFaceHit();
}

static __forceline__ __device__ bool optixIsTriangleFrontFaceHit()
{
    return optixGetHitKind() == OPTIX_HIT_KIND_TRIANGLE_FRONT_FACE;
}

static __forceline__ __device__ bool optixIsTriangleBackFaceHit()
{
    return optixGetHitKind() == OPTIX_HIT_KIND_TRIANGLE_BACK_FACE;
}

static __forceline__ __device__ float optixGetCurveParameter()
{
    float f0;
    asm("call (%0), _optix_get_curve_parameter, ();" : "=f"(f0) :);
    return f0;
}

static __forceline__ __device__ float optixHitObjectGetCurveParameter()
{
    float f0;
    asm("call (%0), _optix_hitobject_get_curve_parameter, ();" : "=f"(f0) :);
    return f0;
}

static __forceline__ __device__ float2 optixGetRibbonParameters()
{
    float f0, f1;
    asm("call (%0, %1), _optix_get_ribbon_parameters, ();" : "=f"(f0), "=f"(f1) :);
    return make_float2(f0, f1);
}

static __forceline__ __device__ float2 optixHitObjectGetRibbonParameters()
{
    float f0, f1;
    asm("call (%0, %1), _optix_hitobject_get_ribbon_parameters, ();" : "=f"(f0), "=f"(f1) :);
    return make_float2(f0, f1);
}

---

<u>337</u>

2522
2523static__forceinline__ __device__ float2optixGetTriangleBarycentrics( )
2524 {
2525floatf0, f1;
2526asm("call (%0, %1), _optix_get_triangle_barycentrics, ();":"=f"(f0),"=f"(f1) :);
2527returnmake_float2(f0, f1);
2528}
2529
2530static__forceinline__ __device__ float2optixHitObjectGetTriangleBarycentrics( )
2531 {
2532floatf0, f1;
2533asm("call (%0, %1), _optix_hitobject_get_triangle_barycentrics, ();":"=f"(f0),"=f"(f1) :);
2534returnmake_float2(f0, f1);
2535}
2536
2537static__forceinline__ __device__ uint3optixGetLaunchIndex( )
2538 {
2539unsignedintu0, u1, u2;
2540asm("call (%0), _optix_get_launch_index_x, ();":"=r"(u0) :);
2541asm("call (%0), _optix_get_launch_index_y, ();":"=r"(u1) :);
2542asm("call (%0), _optix_get_launch_index_z, ();":"=r"(u2) :);
2543returnmake_uint3(u0, u1, u2);
2544}
2545
2546static__forceinline__ __device__ uint3optixGetLaunchDimensions( )
2547 {
2548unsignedintu0, u1, u2;
2549asm("call (%0), _optix_get_launch_dimension_x, ();":"=r"(u0) :);
2550asm("call (%0), _optix_get_launch_dimension_y, ();":"=r"(u1) :);
2551asm("call (%0), _optix_get_launch_dimension_z, ();":"=r"(u2) :);
2552returnmake_uint3(u0, u1, u2);
2553}
2554
2555static__forceinline__ __device__CUdeviceptroptixGetSbtDataPointer( )
2556 {
2557unsignedlonglongptr;
2558asm("call (%0), _optix_get_sbt_data_ptr_64, ();":"=l"(ptr) :);
2559return(CUdeviceptr)ptr;
2560}
2561
2562static__forceinline__ __device__voidoptixThrowException(intexceptionCode)
2563 {
2564asmvolatile(
2565"call _optix_throw_exception_0, (%0);"
2566 :/* no return value */
2567 :"r"(exceptionCode)
2568 :);
2569}
2570
2571static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0)
2572 {
2573asmvolatile(
2574"call _optix_throw_exception_1, (%0, %1);"
2575 :/* no return value */
2576 :"r"(exceptionCode),"r"(exceptionDetail0)
2577 :);
2578}
2579
2580static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0,unsignedintexceptionDetail1)
2581 {
2582asmvolatile(
2583"call _optix_throw_exception_2, (%0, %1, %2);"
2584 :/* no return value */
2585 :"r"(exceptionCode),"r"(exceptionDetail0),"r"(exceptionDetail1)
2586 :);

---

<u>338</u>

2587}
2588
2589static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0,unsignedintexceptionDetail1,unsignedintexceptionDetail2)
2590 {
2591asmvolatile(
2592"call _optix_throw_exception_3, (%0, %1, %2, %3);"
2593 :/* no return value */
2594 :"r"(exceptionCode),"r"(exceptionDetail0),"r"(exceptionDetail1),"r"(exceptionDetail2)
2595 :);
2596}
2597
2598static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0,unsignedintexceptionDetail1,unsignedintexceptionDetail2,unsignedint
exceptionDetail3)
2599 {
2600asmvolatile(
2601"call _optix_throw_exception_4, (%0, %1, %2, %3, %4);"
2602 :/* no return value */
2603 :"r"(exceptionCode),"r"(exceptionDetail0),"r"(exceptionDetail1),"r"(exceptionDetail2),
"r"(exceptionDetail3)
2604 :);
2605}
2606
2607static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0,unsignedintexceptionDetail1,unsignedintexceptionDetail2,unsignedint
exceptionDetail3,unsignedintexceptionDetail4)
2608 {
2609asmvolatile(
2610"call _optix_throw_exception_5, (%0, %1, %2, %3, %4, %5);"
2611 :/* no return value */
2612 :"r"(exceptionCode),"r"(exceptionDetail0),"r"(exceptionDetail1),"r"(exceptionDetail2),
"r"(exceptionDetail3),"r"(exceptionDetail4)
2613 :);
2614}
2615
2616static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0,unsignedintexceptionDetail1,unsignedintexceptionDetail2,unsignedint
exceptionDetail3,unsignedintexceptionDetail4,unsignedintexceptionDetail5)
2617 {
2618asmvolatile(
2619"call _optix_throw_exception_6, (%0, %1, %2, %3, %4, %5, %6);"
2620 :/* no return value */
2621 :"r"(exceptionCode),"r"(exceptionDetail0),"r"(exceptionDetail1),"r"(exceptionDetail2),
"r"(exceptionDetail3),"r"(exceptionDetail4),"r"(exceptionDetail5)
2622 :);
2623}
2624
2625static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0,unsignedintexceptionDetail1,unsignedintexceptionDetail2,unsignedint
exceptionDetail3,unsignedintexceptionDetail4,unsignedintexceptionDetail5,unsignedint
exceptionDetail6)
2626 {
2627asmvolatile(
2628"call _optix_throw_exception_7, (%0, %1, %2, %3, %4, %5, %6, %7);"
2629 :/* no return value */
2630 :"r"(exceptionCode),"r"(exceptionDetail0),"r"(exceptionDetail1),"r"(exceptionDetail2),
"r"(exceptionDetail3),"r"(exceptionDetail4),"r"(exceptionDetail5),"r"(exceptionDetail6)
2631 :);
2632}
2633
2634static__forceinline__ __device__voidoptixThrowException(intexceptionCode,unsignedint
exceptionDetail0,unsignedintexceptionDetail1,unsignedintexceptionDetail2,unsignedint
exceptionDetail3,unsignedintexceptionDetail4,unsignedintexceptionDetail5,unsignedint
exceptionDetail6,unsignedintexceptionDetail7)
2635 {
2636asmvolatile(

---

<u>339</u>

2637"call _optix_throw_exception_8, (%0, %1, %2, %3, %4, %5, %6, %7, %8);"
2638 :/* no return value */
2639 :"r"(exceptionCode),"r"(exceptionDetail0),"r"(exceptionDetail1),"r"(exceptionDetail2),
"r"(exceptionDetail3),"r"(exceptionDetail4),"r"(exceptionDetail5),"r"(exceptionDetail6),
"r"(exceptionDetail7)
2640 :);
2641}
2642
2643static__forceinline__ __device__intoptixGetExceptionCode( )
2644 {
2645ints0;
2646asm("call (%0), _optix_get_exception_code, ();":"=r"(s0) :);
2647returns0;
2648}
2649
2650#define OPTIX_DEFINE_optixGetExceptionDetail_BODY(which)
*\*
2651unsigned int ret;
*\*
2652asm("call (%0), _optix_get_exception_detail_"#which ", ();" : "=r"(ret) :);
*\*
2653 return ret;
2654
2655static__forceinline__ __device__unsignedintoptixGetExceptionDetail_0( )
2656 {
2657OPTIX_DEFINE_optixGetExceptionDetail_BODY(0);
2658}
2659
2660static__forceinline__ __device__unsignedintoptixGetExceptionDetail_1( )
2661 {
2662OPTIX_DEFINE_optixGetExceptionDetail_BODY(1);
2663}
2664
2665static__forceinline__ __device__unsignedintoptixGetExceptionDetail_2( )
2666 {
2667OPTIX_DEFINE_optixGetExceptionDetail_BODY(2);
2668}
2669
2670static__forceinline__ __device__unsignedintoptixGetExceptionDetail_3( )
2671 {
2672OPTIX_DEFINE_optixGetExceptionDetail_BODY(3);
2673}
2674
2675static__forceinline__ __device__unsignedintoptixGetExceptionDetail_4( )
2676 {
2677OPTIX_DEFINE_optixGetExceptionDetail_BODY(4);
2678}
2679
2680static__forceinline__ __device__unsignedintoptixGetExceptionDetail_5( )
2681 {
2682OPTIX_DEFINE_optixGetExceptionDetail_BODY(5);
2683}
2684
2685static__forceinline__ __device__unsignedintoptixGetExceptionDetail_6( )
2686 {
2687OPTIX_DEFINE_optixGetExceptionDetail_BODY(6);
2688}
2689
2690static__forceinline__ __device__unsignedintoptixGetExceptionDetail_7( )
2691 {
2692OPTIX_DEFINE_optixGetExceptionDetail_BODY(7);
2693}
2694
2695#undef OPTIX_DEFINE_optixGetExceptionDetail_BODY
2696
2697
2698static__forceinline__ __device__char*optixGetExceptionLineInfo( )

---

<u>340</u>

2699 {

2700unsignedlonglongptr;
2701asm("call (%0), _optix_get_exception_line_info, ();":"=l"(ptr) :);
2702return(char*)ptr;

2703}
2704
2705template<typenameReturnT,typename... ArgTypes>

2706static__forceinline__ __device__ ReturnToptixDirectCall(unsignedintsbtIndex, ArgTypes... args)

2707 {

2708unsignedlonglongfunc;
2709asm("call (%0), _optix_call_direct_callable,(%1);":"=l"(func) :"r"(sbtIndex) :);
2710usingfuncT = ReturnT (*)(ArgTypes...);

2711 funcT call = (funcT)(func);
2712returncall(args...);
2713}
2714

2715template<typenameReturnT,typename... ArgTypes>
2716static__forceinline__ __device__ ReturnToptixContinuationCall(unsignedintsbtIndex, ArgTypes... args)
2717 {

2718unsignedlonglongfunc;

2719asm("call (%0), _optix_call_continuation_callable,(%1);":"=l"(func) :"r"(sbtIndex) :);
2720usingfuncT = ReturnT (*)(ArgTypes...);

2721 funcT call = (funcT)(func);
2722returncall(args...);
2723}

2724
2725static__forceinline__ __device__ uint4optixTexFootprint2D(unsignedlonglongtex,unsignedint
texInfo,floatx,floaty,unsignedint* singleMipLevel)

2726 {
2727 uint4 result;

2728unsignedlonglongresultPtr =reinterpret_cast<unsignedlonglong>(&result);
2729unsignedlonglongsingleMipLevelPtr =reinterpret_cast<unsignedlonglong>(singleMipLevel);
2730// Cast float args to integers, because the intrinics take.b32 arguments when compiled to PTX.
2731asmvolatile(

2732"call _optix_tex_footprint_2d_v2"
2733", (%0, %1, %2, %3, %4, %5);"
2734 :

2735 :"l"(tex),"r"(texInfo),"r"(__float_as_uint(x)),"r"(__float_as_uint(y)),

2736"l"(singleMipLevelPtr),"l"(resultPtr)
2737 :);
2738returnresult;
2739}
2740

2741static__forceinline__ __device__ uint4optixTexFootprint2DGrad(unsignedlonglongtex,

2742
2743
2744
2745
2746
2747
2748
2749
2750
2751 {

unsignedinttexInfo,
floatx,
floaty,
floatdPdx_x,
floatdPdx_y,
floatdPdy_x,
floatdPdy_y,
boolcoarse,
unsignedint* singleMipLevel)

2752 uint4 result;
2753unsignedlonglongresultPtr =reinterpret_cast<unsignedlonglong>(&result);
2754unsignedlonglongsingleMipLevelPtr =reinterpret_cast<unsignedlonglong>(singleMipLevel);
2755// Cast float args to integers, because the intrinics take.b32 arguments when compiled to PTX.
2756asmvolatile(

2757"call _optix_tex_footprint_2d_grad_v2"

2758", (%0, %1, %2, %3, %4, %5, %6, %7, %8, %9, %10);"
2759 :
2760 :"l"(tex),"r"(texInfo),"r"(__float_as_uint(x)),"r"(__float_as_uint(y)),
2761"r"(__float_as_uint(dPdx_x)),"r"(__float_as_uint(dPdx_y)),"r"(__float_as_uint(dPdy_x)),
2762"r"(__float_as_uint(dPdy_y)),"r"(static_cast<unsignedint>(coarse)),"l"(singleMipLevelPtr),
"l"(resultPtr)

2763 :);

---

<u>341</u>

2764
2765returnresult;
2766}
2767
2768static__forceinline__ __device__ uint4
2769optixTexFootprint2DLod(unsignedlonglongtex,unsignedinttexInfo,floatx,floaty,floatlevel,bool
coarse,unsignedint* singleMipLevel)
2770 {
2771 uint4 result;
2772unsignedlonglongresultPtr =reinterpret_cast<unsignedlonglong>(&result);
2773unsignedlonglongsingleMipLevelPtr =reinterpret_cast<unsignedlonglong>(singleMipLevel);
2774// Cast float args to integers, because the intrinics take.b32 arguments when compiled to PTX.
2775asmvolatile(
2776"call _optix_tex_footprint_2d_lod_v2"
2777", (%0, %1, %2, %3, %4, %5, %6, %7);"
2778 :
2779 :"l"(tex),"r"(texInfo),"r"(__float_as_uint(x)),"r"(__float_as_uint(y)),
2780"r"(__float_as_uint(level)),"r"(static_cast<unsignedint>(coarse)),"l"(singleMipLevelPtr),
"l"(resultPtr)
2781 :);
2782returnresult;
2783}
2784
2785#endif// OPTIX_OPTIX_DEVICE_IMPL_H

## 8.3 optix_device_impl_transformations.h File Reference

## Namespaces

•namespaceoptix_impl

## Functions

•static __forceinline__ __device__ float4optix_impl::optixAddFloat4(const float4 &a, const float4
&b)

•static __forceinline__ __device__ float4optix_impl::optixMulFloat4(const float4 &a, float b)

•static __forceinline__ __device__ uint4optix_impl::optixLdg(unsigned long long addr)

•template<class T >

static __forceinline__ __device__ Toptix_impl::optixLoadReadOnlyAlign16(const T *∗*ptr)

•static __forceinline__ __device__ float4optix_impl::optixMultiplyRowMatrix(const float4 vec,
const float4 m0, const float4 m1, const float4 m2)

•static __forceinline__ __device__ voidoptix_impl::optixGetMatrixFromSrt(float4 &m0, float4
&m1, float4 &m2, constOptixSRTData&srt)

•static __forceinline__ __device__ voidoptix_impl::optixInvertMatrix(float4 &m0, float4 &m1,
float4 &m2)

•static __forceinline__ __device__ voidoptix_impl::optixLoadInterpolatedMatrixKey(float4 &m0,
float4 &m1, float4 &m2, const float4 *∗*matrix, const float t1)

•static __forceinline__ __device__ voidoptix_impl::optixLoadInterpolatedSrtKey(float4 &srt0,
float4 &srt1, float4 &srt2, float4 &srt3, const float4 *∗*srt, const float t1)

•static __forceinline__ __device__ voidoptix_impl::optixResolveMotionKey(float &localt, int
&key, constOptixMotionOptions&options, const float globalt)

•static __forceinline__ __device__ voidoptix_impl::optixGetInterpolatedTransformation(float4
&trf0, float4 &trf1, float4 &trf2, constOptixMatrixMotionTransform *∗*transformData, const float
time)

•static __forceinline__ __device__ voidoptix_impl::optixGetInterpolatedTransformation(float4
&trf0, float4 &trf1, float4 &trf2, constOptixSRTMotionTransform *∗*transformData, const float
time)

---

<u>342</u>

•static __forceinline__ __device__ voidoptix_impl

::optixGetInterpolatedTransformationFromHandle(float4 &trf0, float4 &trf1, float4 &trf2, const
OptixTraversableHandlehandle, const float time, const bool objectToWorld)

## •template<typename HitState >

static __forceinline__ __device__ voidoptix_impl::optixGetWorldToObjectTransformMatrix
(const HitState &hs, float4 &m0, float4 &m1, float4 &m2)

•template<typename HitState >

static __forceinline__ __device__ voidoptix_impl::optixGetObjectToWorldTransformMatrix
(const HitState &hs, float4 &m0, float4 &m1, float4 &m2)

•static __forceinline__ __device__ float3optix_impl::optixTransformPoint(const float4 &m0, const
float4 &m1, const float4 &m2, const float3 &p)

•static __forceinline__ __device__ float3optix_impl::optixTransformVector(const float4 &m0,
const float4 &m1, const float4 &m2, const float3 &v)

•static __forceinline__ __device__ float3optix_impl::optixTransformNormal(const float4 &m0,
const float4 &m1, const float4 &m2, const float3 &n)

## 8.3.1 Detailed Description

OptiX public API.

Author

NVIDIA Corporation

OptiX public API Reference - Device side implementation for transformation helper functions.

8.4 optix_device_impl_transformations.h

Go to the documentation of this file.

1/*
2* SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
3* SPDX-License-Identifier: LicenseRef-NvidiaProprietary
4*
5* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
6* property and proprietary rights in and to this material, related
7* documentation and any modifications thereto. Any use, reproduction,
8* disclosure or distribution of this material and related documentation
9* without an express license agreement from NVIDIA CORPORATION or
10* its affiliates is strictly prohibited.
11*/
20#if!defined(__OPTIX_INCLUDE_INTERNAL_HEADERS__)
21#error("optix_device_impl_transformations.h is an internal header file and must not be used directly.
Please use optix_device.h or optix.h instead.")
22#endif
23
24#ifndef OPTIX_OPTIX_DEVICE_IMPL_TRANSFORMATIONS_H
25#define OPTIX_OPTIX_DEVICE_IMPL_TRANSFORMATIONS_H
26
27namespaceoptix_impl{
28
29static__forceinline__ __device__ float4optixAddFloat4(constfloat4& a,constfloat4& b)
30 {
31returnmake_float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
32}
33
34static__forceinline__ __device__ float4optixMulFloat4(constfloat4& a,floatb)
35 {
36returnmake_float4(a.x * b, a.y * b, a.z * b, a.w * b);
37}
38

---

<u>343</u>

39static__forceinline__ __device__ uint4optixLdg(unsignedlonglongaddr)
40 {
41constuint4* ptr;
42asmvolatile("cvta.to.global.u64 %0, %1;":"=l"(ptr) :"l"(addr));
43 uint4 ret;
44asmvolatile("ld.global.v4.u32 {%0,%1,%2,%3}, [%4];"
45 :"=r"(ret.x),"=r"(ret.y),"=r"(ret.z),"=r"(ret.w)
46 :"l"(ptr));
47returnret;
48}
49
50template<classT>
51static__forceinline__ __device__ ToptixLoadReadOnlyAlign16(constT* ptr)
52 {
53// Debug mode may keep this temporary variable
54// If T does not enforce 16B alignment, v may not be 16B aligned and storing the loaded data from ptr
fails
55 __align__(16) T v;
56for(unsignedintofs = 0; ofs < (unsignedint)sizeof(T); ofs += 16)
57 *(uint4*)((char*)&v + ofs) =optixLdg((unsignedlonglong)((char*)ptr + ofs));
58returnv;
59}
60
61// Multiplies the row vector vec with the 3x4 matrix with rows m0, m1, and m2
62static__forceinline__ __device__ float4optixMultiplyRowMatrix(constfloat4 vec,constfloat4 m0,const
float4 m1,constfloat4 m2)
63 {
64 float4 result;
65
66 result.x = vec.x * m0.x + vec.y * m1.x + vec.z * m2.x;
67 result.y = vec.x * m0.y + vec.y * m1.y + vec.z * m2.y;
68 result.z = vec.x * m0.z + vec.y * m1.z + vec.z * m2.z;
69 result.w = vec.x * m0.w + vec.y * m1.w + vec.z * m2.w + vec.w;
70
71returnresult;
72}
73
74// Converts the SRT transformation srt into a 3x4 matrix with rows m0, m1, and m2
75static__forceinline__ __device__voidoptixGetMatrixFromSrt(float4& m0, float4& m1, float4& m2,const
OptixSRTData& srt)
76 {
77// assumed to be normalized
78constfloat4 q = {srt.qx, srt.qy, srt.qz, srt.qw};
79
80constfloatsqw = q.w * q.w;
81constfloatsqx = q.x * q.x;
82constfloatsqy = q.y * q.y;
83constfloatsqz = q.z * q.z;
84
85constfloatxy = q.x * q.y;
86constfloatzw = q.z * q.w;
87constfloatxz = q.x * q.z;
88constfloatyw = q.y * q.w;
89constfloatyz = q.y * q.z;
90constfloatxw = q.x * q.w;
91
92 m0.x = (sqx-sqy-sqz + sqw);
93 m0.y = 2.0f * (xy-zw);
94 m0.z = 2.0f * (xz + yw);
95
96 m1.x = 2.0f * (xy + zw);
97 m1.y = (-sqx + sqy-sqz + sqw);
98 m1.z = 2.0f * (yz-xw);
99
100 m2.x = 2.0f * (xz-yw);
101 m2.y = 2.0f * (yz + xw);
102 m2.z = (-sqx-sqy + sqz + sqw);

---

<u>344</u>

103
104 m0.w = m0.x * srt.pvx+ m0.y * srt.pvy+ m0.z * srt.pvz+ srt.tx;
105 m1.w = m1.x * srt.pvx+ m1.y * srt.pvy+ m1.z * srt.pvz+ srt.ty;
106 m2.w = m2.x * srt.pvx+ m2.y * srt.pvy+ m2.z * srt.pvz+ srt.tz;
107
108 m0.z = m0.x * srt.b+ m0.y * srt.c+ m0.z * srt.sz;
109 m1.z = m1.x * srt.b+ m1.y * srt.c+ m1.z * srt.sz;
110 m2.z = m2.x * srt.b+ m2.y * srt.c+ m2.z * srt.sz;
111
112 m0.y = m0.x * srt.a+ m0.y * srt.sy;
113 m1.y = m1.x * srt.a+ m1.y * srt.sy;
114 m2.y = m2.x * srt.a+ m2.y * srt.sy;
115
116 m0.x = m0.x * srt.sx;
117 m1.x = m1.x * srt.sx;
118 m2.x = m2.x * srt.sx;
119}
120
121// Inverts a 3x4 matrix in place
122static__forceinline__ __device__voidoptixInvertMatrix(float4& m0, float4& m1, float4& m2)
123 {
124constfloatdet3 =
125 m0.x * (m1.y * m2.z-m1.z * m2.y)-m0.y * (m1.x * m2.z-m1.z * m2.x) + m0.z * (m1.x * m2.ym1.y * m2.x);
126
127constfloatinv_det3 = 1.0f / det3;
128
129floatinv3[3][3];
130 inv3[0][0] = inv_det3 * (m1.y * m2.z-m2.y * m1.z);
131 inv3[0][1] = inv_det3 * (m0.z * m2.y-m2.z * m0.y);
132 inv3[0][2] = inv_det3 * (m0.y * m1.z-m1.y * m0.z);
133
134 inv3[1][0] = inv_det3 * (m1.z * m2.x-m2.z * m1.x);
135 inv3[1][1] = inv_det3 * (m0.x * m2.z-m2.x * m0.z);
136 inv3[1][2] = inv_det3 * (m0.z * m1.x-m1.z * m0.x);
137
138 inv3[2][0] = inv_det3 * (m1.x * m2.y-m2.x * m1.y);
139 inv3[2][1] = inv_det3 * (m0.y * m2.x-m2.y * m0.x);
140 inv3[2][2] = inv_det3 * (m0.x * m1.y-m1.x * m0.y);
141
142constfloatb[3] = {m0.w, m1.w, m2.w};
143
144 m0.x = inv3[0][0];
145 m0.y = inv3[0][1];
146 m0.z = inv3[0][2];
147 m0.w =-inv3[0][0] * b[0]-inv3[0][1] * b[1]-inv3[0][2] * b[2];
148
149 m1.x = inv3[1][0];
150 m1.y = inv3[1][1];
151 m1.z = inv3[1][2];
152 m1.w =-inv3[1][0] * b[0]-inv3[1][1] * b[1]-inv3[1][2] * b[2];
153
154 m2.x = inv3[2][0];
155 m2.y = inv3[2][1];
156 m2.z = inv3[2][2];
157 m2.w =-inv3[2][0] * b[0]-inv3[2][1] * b[1]-inv3[2][2] * b[2];
158}
159
160static__forceinline__ __device__voidoptixLoadInterpolatedMatrixKey(float4& m0, float4& m1, float4&
m2,constfloat4* matrix,constfloatt1)
161 {
162 m0 =optixLoadReadOnlyAlign16(&matrix[0]);
163 m1 =optixLoadReadOnlyAlign16(&matrix[1]);
164 m2 =optixLoadReadOnlyAlign16(&matrix[2]);
165
166// The conditional prevents concurrent loads leading to spills
167if(t1 > 0.0f)

---

168 {
169constfloatt0 = 1.0f-t1;
170 m0 =optixAddFloat4(optixMulFloat4(m0, t0),optixMulFloat4(optixLoadReadOnlyAlign16(&matrix[3]),
t1));
171 m1 =optixAddFloat4(optixMulFloat4(m1, t0),optixMulFloat4(optixLoadReadOnlyAlign16(&matrix[4]),
t1));
172 m2 =optixAddFloat4(optixMulFloat4(m2, t0),optixMulFloat4(optixLoadReadOnlyAlign16(&matrix[5]),
t1));
173}
174}
175
176static__forceinline__ __device__voidoptixLoadInterpolatedSrtKey(float4& srt0,
177 float4& srt1,
178 float4& srt2,
179 float4& srt3,
180 constfloat4* srt,
181 constfloatt1)
182 {
183 srt0 =optixLoadReadOnlyAlign16(&srt[0]);
184 srt1 =optixLoadReadOnlyAlign16(&srt[1]);
185 srt2 =optixLoadReadOnlyAlign16(&srt[2]);
186 srt3 =optixLoadReadOnlyAlign16(&srt[3]);
187
188// The conditional prevents concurrent loads leading to spills
189if(t1 > 0.0f)
190 {
191constfloatt0 = 1.0f-t1;
192 srt0 =optixAddFloat4(optixMulFloat4(srt0, t0),optixMulFloat4(optixLoadReadOnlyAlign16(&srt[4]),
t1));
193 srt1 =optixAddFloat4(optixMulFloat4(srt1, t0),optixMulFloat4(optixLoadReadOnlyAlign16(&srt[5]),
t1));
194 srt2 =optixAddFloat4(optixMulFloat4(srt2, t0),optixMulFloat4(optixLoadReadOnlyAlign16(&srt[6]),
t1));
195 srt3 =optixAddFloat4(optixMulFloat4(srt3, t0),optixMulFloat4(optixLoadReadOnlyAlign16(&srt[7]),
t1));
196
197floatinv_length = 1.f / sqrt(srt2.y * srt2.y + srt2.z * srt2.z + srt2.w * srt2.w + srt3.x *
srt3.x);
198 srt2.y *= inv_length;
199 srt2.z *= inv_length;
200 srt2.w *= inv_length;
201 srt3.x *= inv_length;
202}
203}
204
205static__forceinline__ __device__voidoptixResolveMotionKey(float& localt,int& key,const
OptixMotionOptions& options,constfloatglobalt)
206 {
207constfloattimeBegin = options.timeBegin;
208constfloattimeEnd = options.timeEnd;
209constfloatnumIntervals = (float)(options.numKeys-1);
210
211// No need to check the motion flags. If data originates from a valid transform list handle, then
globalt is in
212// range, or vanish flags are not set.
213
214// should be NaN or in [0,numIntervals]
215floattime = max(0.f, min(numIntervals, numIntervals * __fdividef(globalt-timeBegin, timeEndtimeBegin)));
216
217// catch NaN (for example when timeBegin=timeEnd)
218if(time!= time)
219 time = 0.f;
220
221constfloatfltKey = fminf(floorf(time), numIntervals-1);
222
223 localt = time-fltKey;

---

224 key = (int)fltKey;
225}

226
227// Returns the interpolated transformation matrix for a particular matrix motion transformation and point
in time.
228static__forceinline__ __device__voidoptixGetInterpolatedTransformation(float4&
trf0,

229
230
231
transformData,
232
233 {
234// Compute key and intra key time
235floatkeyTime;

float4& trf1,
float4& trf2,
constOptixMatrixMotionTransform*
constfloattime)

236intkey;
237optixResolveMotionKey(keyTime, key,optixLoadReadOnlyAlign16(transformData).motionOptions, time);

238

239// Get pointer to left key
240constfloat4* transform = (constfloat4*)(&transformData->transform[key][0]);
241

242// Load and interpolate matrix keys

243optixLoadInterpolatedMatrixKey(trf0, trf1, trf2, transform, keyTime);

244}

245
246// Returns the interpolated transformation matrix for a particular SRT motion transformation and point in
time.
247static__forceinline__ __device__voidoptixGetInterpolatedTransformation(float4&
trf0,

248
249
250
transformData,
251
252 {
253// Compute key and intra key time
254floatkeyTime;
255intkey;

float4& trf1,
float4& trf2,
constOptixSRTMotionTransform*
constfloattime)

256optixResolveMotionKey(keyTime, key,optixLoadReadOnlyAlign16(transformData).motionOptions, time);

257

258// Get pointer to left key
259constfloat4* dataPtr =reinterpret_cast<constfloat4*>(&transformData->srtData[key]);
260

261// Load and interpolated SRT keys
262 float4 data[4];

263optixLoadInterpolatedSrtKey(data[0], data[1], data[2], data[3], dataPtr, keyTime);
264
265OptixSRTDatasrt = {data[0].x, data[0].y, data[0].z, data[0].w, data[1].x, data[1].y, data[1].z,
data[1].w,
266 data[2].x, data[2].y, data[2].z, data[2].w, data[3].x, data[3].y, data[3].z,
data[3].w};

267
268// Convert SRT into a matrix

269optixGetMatrixFromSrt(trf0, trf1, trf2, srt);

270}

271
272// Returns the interpolated transformation matrix for a particular traversable handle and point in time.
273static__forceinline__ __device__voidoptixGetInterpolatedTransformationFromHandle(float4&
trf0,

274
trf1,
275
trf2,
276
OptixTraversableHandlehandle,
277
time,

float4&
float4&
const
constfloat

---

278 constboolobjectToWorld)
279 {
280constOptixTransformTypetype =optixGetTransformTypeFromHandle(handle);
281
282if(type ==OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_TRANSFORM|| type ==
OPTIX_TRANSFORM_TYPE_SRT_MOTION_TRANSFORM)
283 {
284if(type ==OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_TRANSFORM)
285 {
286constOptixMatrixMotionTransform* transformData =
optixGetMatrixMotionTransformFromHandle(handle);
287optixGetInterpolatedTransformation(trf0, trf1, trf2, transformData, time);
288}
289else
290 {
291constOptixSRTMotionTransform* transformData =optixGetSRTMotionTransformFromHandle(handle);
292optixGetInterpolatedTransformation(trf0, trf1, trf2, transformData, time);
293}
294
295if(!objectToWorld)
296optixInvertMatrix(trf0, trf1, trf2);
297}
298elseif(type ==OPTIX_TRANSFORM_TYPE_INSTANCE|| type ==OPTIX_TRANSFORM_TYPE_STATIC_TRANSFORM)
299 {
300constfloat4* transform;
301
302if(type ==OPTIX_TRANSFORM_TYPE_INSTANCE)
303 {
304 transform = (objectToWorld)?optixGetInstanceTransformFromHandle(handle) :
305optixGetInstanceInverseTransformFromHandle(handle);
306}
307else
308 {
309constOptixStaticTransform* traversable =optixGetStaticTransformFromHandle(handle);
310 transform = (constfloat4*)((objectToWorld)? traversable->transform:
traversable->invTransform);
311}
312
313 trf0 =optixLoadReadOnlyAlign16(&transform[0]);
314 trf1 =optixLoadReadOnlyAlign16(&transform[1]);
315 trf2 =optixLoadReadOnlyAlign16(&transform[2]);
316}
317else
318 {
319 trf0 = {1.0f, 0.0f, 0.0f, 0.0f};
320 trf1 = {0.0f, 1.0f, 0.0f, 0.0f};
321 trf2 = {0.0f, 0.0f, 1.0f, 0.0f};
322}
323}
324
325// Returns the world-to-object transformation matrix resulting from the transform stack and ray time of
the given hit object.
326template<typenameHitState>
327static__forceinline__ __device__voidoptixGetWorldToObjectTransformMatrix(constHitState& hs, float4&
m0, float4& m1, float4& m2)
328 {
329constunsignedintsize = hs.getTransformListSize();
330constfloattime = hs.getRayTime();
331
332#pragma unroll 1
333for(unsignedinti = 0; i < size; ++i)
334 {
335OptixTraversableHandlehandle = hs.getTransformListHandle(i);
336
337 float4 trf0, trf1, trf2;
338optixGetInterpolatedTransformationFromHandle(trf0, trf1, trf2, handle, time,/*objectToWorld*/
false);

---

339
340if(i == 0)
341 {
342 m0 = trf0;
343 m1 = trf1;
344 m2 = trf2;
345}
346else
347 {
348// m := trf * m
349 float4 tmp0 = m0, tmp1 = m1, tmp2 = m2;
350 m0 =optixMultiplyRowMatrix(trf0, tmp0, tmp1, tmp2);
351 m1 =optixMultiplyRowMatrix(trf1, tmp0, tmp1, tmp2);
352 m2 =optixMultiplyRowMatrix(trf2, tmp0, tmp1, tmp2);
353}
354}
355}
356
357// Returns the object-to-world transformation matrix resulting from the transform stack and ray time of
the given hit object.
358template<typenameHitState>
359static__forceinline__ __device__voidoptixGetObjectToWorldTransformMatrix(constHitState& hs, float4&
m0, float4& m1, float4& m2)
360 {
361constintsize = hs.getTransformListSize();
362constfloattime = hs.getRayTime();
363
364#pragma unroll 1
365for(inti = size-1; i >= 0;--i)
366 {
367OptixTraversableHandlehandle = hs.getTransformListHandle(i);
368
369 float4 trf0, trf1, trf2;
370optixGetInterpolatedTransformationFromHandle(trf0, trf1, trf2, handle, time,/*objectToWorld*/
true);
371
372if(i == size-1)
373 {
374 m0 = trf0;
375 m1 = trf1;
376 m2 = trf2;
377}
378else
379 {
380// m := trf * m
381 float4 tmp0 = m0, tmp1 = m1, tmp2 = m2;
382 m0 =optixMultiplyRowMatrix(trf0, tmp0, tmp1, tmp2);
383 m1 =optixMultiplyRowMatrix(trf1, tmp0, tmp1, tmp2);
384 m2 =optixMultiplyRowMatrix(trf2, tmp0, tmp1, tmp2);
385}
386}
387}
388
389// Multiplies the 3x4 matrix with rows m0, m1, m2 with the point p.
390static__forceinline__ __device__ float3optixTransformPoint(constfloat4& m0,constfloat4& m1,const
float4& m2,constfloat3& p)
391 {
392 float3 result;
393 result.x = m0.x * p.x + m0.y * p.y + m0.z * p.z + m0.w;
394 result.y = m1.x * p.x + m1.y * p.y + m1.z * p.z + m1.w;
395 result.z = m2.x * p.x + m2.y * p.y + m2.z * p.z + m2.w;
396returnresult;
397}
398
399// Multiplies the 3x3 linear submatrix of the 3x4 matrix with rows m0, m1, m2 with the vector v.
400static__forceinline__ __device__ float3optixTransformVector(constfloat4& m0,constfloat4& m1,const
float4& m2,constfloat3& v)

---

401 {
402 float3 result;
403 result.x = m0.x * v.x + m0.y * v.y + m0.z * v.z;
404 result.y = m1.x * v.x + m1.y * v.y + m1.z * v.z;
405 result.z = m2.x * v.x + m2.y * v.y + m2.z * v.z;
406returnresult;
407}
408
409// Multiplies the transpose of the 3x3 linear submatrix of the 3x4 matrix with rows m0, m1, m2 with the
normal n.
410// Note that the given matrix is supposed to be the inverse of the actual transformation matrix.
411static__forceinline__ __device__ float3optixTransformNormal(constfloat4& m0,constfloat4& m1,const
float4& m2,constfloat3& n)
412 {
413 float3 result;
414 result.x = m0.x * n.x + m1.x * n.y + m2.x * n.z;
415 result.y = m0.y * n.x + m1.y * n.y + m2.y * n.z;
416 result.z = m0.z * n.x + m1.z * n.y + m2.z * n.z;
417returnresult;
418}
419
420}// namespace optix_impl
421
422#endif// OPTIX_OPTIX_DEVICE_IMPL_TRANSFORMATIONS_H

## 8.5 optix_micromap_impl.h File Reference

## Namespaces

## Macros

•namespaceoptix_impl

•#defineOPTIX_MICROMAP_FUNC

•#defineOPTIX_MICROMAP_INLINE_FUNCOPTIX_MICROMAP_FUNCinline

•#defineOPTIX_MICROMAP_FLOAT2_SUB(a, b) { a.x - b.x, a.y - b.y }

## Functions

•OPTIX_MICROMAP_INLINE_FUNCfloatoptix_impl::__uint_as_float(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCunsigned intoptix_impl::extractEvenBits(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCunsigned intoptix_impl::prefixEor(unsigned int x)

•OPTIX_MICROMAP_INLINE_FUNCvoidoptix_impl::index2dbary(unsigned int index,
unsigned int &u, unsigned int &v, unsigned int &w)

•OPTIX_MICROMAP_INLINE_FUNCvoidoptix_impl::micro2bary(unsigned int index,
unsigned int subdivisionLevel, float2 &bary0, float2 &bary1, float2 &bary2)

•OPTIX_MICROMAP_INLINE_FUNCfloat2optix_impl::base2micro(const float2
&baseBarycentrics, const float2 microVertexBaseBarycentrics[3])

## 8.5.1 Detailed Description

OptiX micromap helper functions.

Author

NVIDIA Corporation

## 8.5.2 Macro Definition Documentation

## 8.5.2.1 OPTIX_MICROMAP_FUNC

#define OPTIX_MICROMAP_FUNC

---

8.6 optix_micromap_impl.h

Go to the documentation of this file.

1/*
2* SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES.
3* SPDX-License-Identifier: BSD-3-Clause
4*
5* Redistribution and use in source and binary forms, with or without
6* modification, are permitted provided that the following conditions are met:
7*
8* 1. Redistributions of source code must retain the above copyright notice, this
9* list of conditions and the following disclaimer.
10*
11* 2. Redistributions in binary form must reproduce the above copyright notice,
12* this list of conditions and the following disclaimer in the documentation
13* and/or other materials provided with the distribution.
14*
15* 3. Neither the name of the copyright holder nor the names of its
16* contributors may be used to endorse or promote products derived from
17* this software without specific prior written permission.
18*
19* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
20* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
21* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
22* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
23* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
24* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
25* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
26* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
27* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
28* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
29*/
30
31
38#ifndef OPTIX_OPTIX_MICROMAP_IMPL_H
39#define OPTIX_OPTIX_MICROMAP_IMPL_H
40
41#ifndef OPTIX_MICROMAP_FUNC
42#ifdef __CUDACC__
43#define OPTIX_MICROMAP_FUNC __device__
44#else
45#define OPTIX_MICROMAP_FUNC
46#endif
47#endif
48
49namespaceoptix_impl{
50
55#define OPTIX_MICROMAP_INLINE_FUNC OPTIX_MICROMAP_FUNC inline
56
57#ifdef __CUDACC__
58// the device implementation of __uint_as_float is declared in cuda_runtime.h
59#else
60// the host implementation of __uint_as_float
61OPTIX_MICROMAP_INLINE_FUNCfloat__uint_as_float(unsignedintx)
62 {
63union{floatf;unsignedinti;} var;
64 var.i = x;
65returnvar.f;
66}
67#endif
68
69// Extract even bits
70OPTIX_MICROMAP_INLINE_FUNCunsignedintextractEvenBits(unsignedintx)
71 {
72 x &= 0x55555555;
73 x = (x | (x » 1)) & 0x33333333;
74 x = (x | (x » 2)) & 0x0f0f0f0f;

---

75 x = (x | (x » 4)) & 0x00ff00ff;
76 x = (x | (x » 8)) & 0x0000ffff;
77returnx;
78}
79
80
81// Calculate exclusive prefix or (log(n) XOR’s and SHF’s)
82OPTIX_MICROMAP_INLINE_FUNCunsignedintprefixEor(unsignedintx)
83 {
84 x ^= x » 1;
85 x ^= x » 2;
86 x ^= x » 4;
87 x ^= x » 8;
88returnx;
89}
90
91// Convert distance along the curve to discrete barycentrics
92OPTIX_MICROMAP_INLINE_FUNCvoidindex2dbary(unsignedintindex,unsignedint& u,unsignedint& v,unsigned
int& w)
93 {
94unsignedintb0 =extractEvenBits(index);
95unsignedintb1 =extractEvenBits(index » 1);
96
97unsignedintfx =prefixEor(b0);
98unsignedintfy =prefixEor(b0 & ~b1);
99
100unsignedintt = fy ^ b1;
101
102 u = (fx & ~t) | (b0 & ~t) | (~b0 & ~fx & t);
103 v = fy ^ b0;
104 w = (~fx & ~t) | (b0 & ~t) | (~b0 & fx & t);
105}
106
107// Compute barycentrics of a sub or micro triangle wrt a base triangle. The order of the returned
108// bary0, bary1, bary2 matters and allows for using this function for sub triangles and the
109// conversion from sub triangle to base triangle barycentric space
110OPTIX_MICROMAP_INLINE_FUNCvoidmicro2bary(unsignedintindex,unsignedintsubdivisionLevel, float2&
bary0, float2& bary1, float2& bary2)
111 {
112if(subdivisionLevel == 0)
113 {
114 bary0 = { 0, 0};
115 bary1 = { 1, 0};
116 bary2 = { 0, 1};
117return;
118}
119
120unsignedintiu, iv, iw;
121index2dbary(index, iu, iv, iw);
122
123// we need to only look at "level" bits
124 iu = iu & ((1 « subdivisionLevel)-1);
125 iv = iv & ((1 « subdivisionLevel)-1);
126 iw = iw & ((1 « subdivisionLevel)-1);
127
128intyFlipped = (iu & 1) ^ (iv & 1) ^ (iw & 1) ^ 1;
129
130intxFlipped = ((0x8888888888888888ull ^ 0xf000f000f000f000ull ^ 0xffff000000000000ull) » index) & 1;
131 xFlipped ^= ((0x8888888888888888ull ^ 0xf000f000f000f000ull ^ 0xffff000000000000ull) » (index »
6)) & 1;
132
133constfloatlevelScale =__uint_as_float((127u-subdivisionLevel) « 23);
134
135// scale the barycentic coordinate to the global space/scale
136floatdu = 1.f * levelScale;
137floatdv = 1.f * levelScale;
138

---

139// scale the barycentic coordinate to the global space/scale
140floatu = (float)iu * levelScale;
141floatv = (float)iv * levelScale;
142
143// c d
144// x-----x
145// / *\* /
146// / *\* /
147// x-----x
148// a b
149//
150//!xFlipped &&!yFlipped: abc
151//!xFlipped && yFlipped: cdb
152// xFlipped &&!yFlipped: bac
153// xFlipped && yFlipped: dcb
154
155 bary0 = { u + xFlipped * du, v + yFlipped * dv};
156 bary1 = { u + (1-xFlipped) * du, v + yFlipped * dv};
157 bary2 = { u + yFlipped * du, v + (1-yFlipped) * dv};
158}
159
160// avoid any conflicts due to multiple definitions
161#define OPTIX_MICROMAP_FLOAT2_SUB(a,b) { a.x-b.x, a.y-b.y}
162
163// Compute barycentrics for micro triangle from base barycentrics
164OPTIX_MICROMAP_INLINE_FUNCfloat2base2micro(constfloat2& baseBarycentrics,constfloat2
microVertexBaseBarycentrics[3])
165 {
166 float2 baryV0P =OPTIX_MICROMAP_FLOAT2_SUB(baseBarycentrics, microVertexBaseBarycentrics[0]);
167 float2 baryV0V1 =OPTIX_MICROMAP_FLOAT2_SUB(microVertexBaseBarycentrics[1],
microVertexBaseBarycentrics[0]);
168 float2 baryV0V2 =OPTIX_MICROMAP_FLOAT2_SUB(microVertexBaseBarycentrics[2],
microVertexBaseBarycentrics[0]);
169
170floatrdetA = 1.f / (baryV0V1.x * baryV0V2.y-baryV0V1.y * baryV0V2.x);
171 float4 A = { baryV0V2.y,-baryV0V2.x,-baryV0V1.y, baryV0V1.x};
172
173 float2 localUV;
174 localUV.x = rdetA * (baryV0P.x * A.x + baryV0P.y * A.y);
175 localUV.y = rdetA * (baryV0P.x * A.z + baryV0P.y * A.w);
176
177returnlocalUV;
178}
179#undef OPTIX_MICROMAP_FLOAT2_SUB
180// end group optix_utilities
182
183}// namespace optix_impl
184
185#endif// OPTIX_OPTIX_MICROMAP_IMPL_H
8.7 optix.h File Reference
Macros

•#defineOPTIX_VERSION90100

## 8.7.1 Detailed Description

OptiX public API header.

---

## Author

## NVIDIA Corporation

Includes the host api if compiling host code, includes the cuda api if compiling device code. For the
math library routines include optix_math.h

## 8.7.2 Macro Definition Documentation

## 8.7.2.1 OPTIX_VERSION

8.8 optix.h

#define OPTIX_VERSION 90100
The OptiX version.
•major = OPTIX_VERSION/10000
•minor = (OPTIX_VERSION%10000)/100
•micro = OPTIX_VERSION%100
Go to the documentation of this file.
1
2/*
3* SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
4* SPDX-License-Identifier: LicenseRef-NvidiaProprietary
5*
6* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
7* property and proprietary rights in and to this material, related
8* documentation and any modifications thereto. Any use, reproduction,
9* disclosure or distribution of this material and related documentation
10* without an express license agreement from NVIDIA CORPORATION or
11* its affiliates is strictly prohibited.
12*/
19
20#ifndef OPTIX_OPTIX_H
21#define OPTIX_OPTIX_H
22
28#define OPTIX_VERSION 90100
29
30
31#ifdef __CUDACC__
32#include "optix_device.h"
33#else
34#include "optix_host.h"
35#endif
36
37
38#endif// OPTIX_OPTIX_H

## 8.9 optix_denoiser_tiling.h File Reference

## Classes

•structOptixUtilDenoiserImageTile

## Functions

•OptixResultoptixUtilGetPixelStride(constOptixImage2D&image, unsigned int
&pixelStrideInBytes)

•OptixResultoptixUtilDenoiserSplitImage(constOptixImage2D&input, constOptixImage2D
&output, unsigned int overlapWindowSizeInPixels, unsigned int tileWidth, unsigned int
tileHeight, std::vector< OptixUtilDenoiserImageTile > &tiles)

---

•OptixResultoptixUtilDenoiserInvokeTiled(OptixDenoiserdenoiser, CUstream stream, const
OptixDenoiserParams *∗*params,CUdeviceptrdenoiserState, size_t denoiserStateSizeInBytes,
constOptixDenoiserGuideLayer *∗*guideLayer, constOptixDenoiserLayer *∗*layers, unsigned int
numLayers,CUdeviceptrscratch, size_t scratchSizeInBytes, unsigned int
overlapWindowSizeInPixels, unsigned int tileWidth, unsigned int tileHeight)

## 8.9.1 Detailed Description

OptiX public API header.

## Author

## NVIDIA Corporation

8.10 optix_denoiser_tiling.h

Go to the documentation of this file.

1/*
2* SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights
3* SPDX-License-Identifier: BSD-3-Clause
4*
5* Redistribution and use in source and binary forms, with or without
6* modification, are permitted provided that the following conditions are met:
7*
8* 1. Redistributions of source code must retain the above copyright notice, this
9* list of conditions and the following disclaimer.
10*
11* 2. Redistributions in binary form must reproduce the above copyright notice,
12* this list of conditions and the following disclaimer in the documentation
13* and/or other materials provided with the distribution.
14*
15* 3. Neither the name of the copyright holder nor the names of its
16* contributors may be used to endorse or promote products derived from
17* this software without specific prior written permission.
18*
19* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
20* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
21* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
22* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
23* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
24* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
25* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
26* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
27* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
28* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
29*/
30
34
35#ifndef OPTIX_DENOISER_TILING_H
36#define OPTIX_DENOISER_TILING_H
37
38#include <optix.h>
39
40#include <algorithm>
41#include <vector>
42
43#ifdef __cplusplus
44extern"C"{
45#endif
46
55structOptixUtilDenoiserImageTile
56 {
57// input tile image
58OptixImage2Dinput;

---

// output tile image
OptixImage2D output;

// overlap offsets, parameters for #optixUtilDenoiserInvoke
unsigned int inputOffsetX;
unsigned int inputOffsetY;
};

inline OptixResult optixUtilGetPixelStride(const OptixImage2D& image, unsigned int& pixelStrideInBytes)
{
    pixelStrideInBytes = image.pixelStrideInBytes;
    if(pixelStrideInBytes == 0)
    {
        switch(image.format)
        {
            case OPTIX_PIXEL_FORMAT_HALF1:
                pixelStrideInBytes = 1 * sizeof(short);
                break;
            case OPTIX_PIXEL_FORMAT_HALF2:
                pixelStrideInBytes = 2 * sizeof(short);
                break;
            case OPTIX_PIXEL_FORMAT_HALF3:
                pixelStrideInBytes = 3 * sizeof(short);
                break;
            case OPTIX_PIXEL_FORMAT_HALF4:
                pixelStrideInBytes = 4 * sizeof(short);
                break;
            case OPTIX_PIXEL_FORMAT_FLOAT1:
                pixelStrideInBytes = 1 * sizeof(float);
                break;
            case OPTIX_PIXEL_FORMAT_FLOAT2:
                pixelStrideInBytes = 2 * sizeof(float);
                break;
            case OPTIX_PIXEL_FORMAT_FLOAT3:
                pixelStrideInBytes = 3 * sizeof(float);
                break;
            case OPTIX_PIXEL_FORMAT_FLOAT4:
                pixelStrideInBytes = 4 * sizeof(float);
                break;
            case OPTIX_PIXEL_FORMAT_UCHAR3:
                pixelStrideInBytes = 3 * sizeof(char);
                break;
            case OPTIX_PIXEL_FORMAT_UCHAR4:
                pixelStrideInBytes = 4 * sizeof(char);
                break;
            case OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_LAYER:
                return OPTIX_ERROR_INVALID_VALUE;
                break;
        }
    }
    return OPTIX_SUCCESS;
}

inline OptixResult optixUtilDenoiserSplitImage(
                                    const OptixImage2D&                 input,
                                    const OptixImage2D&                 output,
                                    unsigned int                          overlapWindowSizeInPixels,
                                    unsigned int                          tileWidth,
                                    unsigned int                          tileHeight,
                                    std::vector<OptixUtilDenoiserImageTile>&   tiles)
{
    if(tileWidth == 0 || tileHeight == 0)
        return OPTIX_ERROR_INVALID_VALUE;

    unsigned int inPixelStride, outPixelStride;
    if(const OptixResult res = optixUtilGetPixelStride(input, inPixelStride))

---

142returnres;
143if(constOptixResultres =optixUtilGetPixelStride(output, outPixelStride))
144returnres;
145
146intinp_w = std::min(tileWidth + 2 * overlapWindowSizeInPixels, input.width);
147intinp_h = std::min(tileHeight + 2 * overlapWindowSizeInPixels, input.height);
148intinp_y = 0, copied_y = 0;
149
150intupscaleX = output.width/ input.width;
151intupscaleY = output.height/ input.height;
152
153do
154 {
155intinputOffsetY = inp_y == 0? 0 : std::max((int)overlapWindowSizeInPixels, inp_h-
((int)input.height-inp_y));
156intcopy_y = inp_y == 0? std::min(input.height, tileHeight + overlapWindowSizeInPixels)
157 std::min(tileHeight, input.height-copied_y);
158
159intinp_x = 0, copied_x = 0;
160do
161 {
162intinputOffsetX = inp_x == 0? 0 : std::max((int)overlapWindowSizeInPixels, inp_w-
((int)input.width-inp_x));
163intcopy_x = inp_x == 0? std::min(input.width, tileWidth + overlapWindowSizeInPixels) :
164 std::min(tileWidth, input.width-copied_x);
165
166OptixUtilDenoiserImageTiletile;
167 tile.input.data= input.data+ (size_t)(inp_y-inputOffsetY) *
input.rowStrideInBytes
168 + (size_t)(inp_x-inputOffsetX) * inPixelStride;
169 tile.input.width= inp_w;
170 tile.input.height= inp_h;
171 tile.input.rowStrideInBytes= input.rowStrideInBytes;
172 tile.input.pixelStrideInBytes= input.pixelStrideInBytes;
173 tile.input.format= input.format;
174
175 tile.output.data= output.data+ (size_t)(upscaleY * inp_y) *
output.rowStrideInBytes
176 + (size_t)(upscaleX * inp_x) * outPixelStride;
177 tile.output.width= upscaleX * copy_x;
178 tile.output.height= upscaleY * copy_y;
179 tile.output.rowStrideInBytes= output.rowStrideInBytes;
180 tile.output.pixelStrideInBytes= output.pixelStrideInBytes;
181 tile.output.format= output.format;
182
183 tile.inputOffsetX= inputOffsetX;
184 tile.inputOffsetY= inputOffsetY;
185
186 tiles.push_back(tile);
187
188 inp_x += inp_x == 0? tileWidth + overlapWindowSizeInPixels : tileWidth;
189 copied_x += copy_x;
190}while(inp_x <static_cast<int>(input.width));
191
192 inp_y += inp_y == 0? tileHeight + overlapWindowSizeInPixels : tileHeight;
193 copied_y += copy_y;
194}while(inp_y <static_cast<int>(input.height));
195
196returnOPTIX_SUCCESS;
197}
198
202
209
225inlineOptixResultoptixUtilDenoiserInvokeTiled(
226OptixDenoiserdenoiser,
227 CUstream stream,
228constOptixDenoiserParams* params,

---

CUdeviceptr denoiserState,
size_t denoiserStateSizeInBytes,
const OptixDenoiserGuideLayer* guideLayer,
const OptixDenoiserLayer* layers,
unsigned int numLayers,
CUdeviceptr scratch,
size_t scratchSizeInBytes,
unsigned int overlapWindowSizeInPixels,
unsigned int tileWidth,
unsigned int tileHeight)

{
if(!guideLayer || !layers)
return OPTIX_ERROR_INVALID_VALUE;

const unsigned int upscale = numLayers > 0 && layers[0].previousOutput.width == 2 *
layers[0].input.width ? 2 : 1;

std::vector<std::vector<OptixUtilDenoiserImageTile> tiles(numLayers);
std::vector<std::vector<OptixUtilDenoiserImageTile> prevTiles(numLayers);
for(unsigned int l = 0; l < numLayers; l++)
{
if(const OptixResult res = optixUtilDenoiserSplitImage(layers[l].input, layers[l].output,
overlapWindowSizeInPixels,
tileWidth, tileHeight, tiles[l]))
return res;

if(layers[l].previousOutput.data)
{
OptixImage2D dummyOutput = layers[l].previousOutput;
if(const OptixResult res = optixUtilDenoiserSplitImage(layers[l].previousOutput, dummyOutput,
upscale * overlapWindowSizeInPixels,
upscale * tileWidth, upscale * tileHeight,
prevTiles[l]))
return res;
}
}

std::vector<OptixUtilDenoiserImageTile> albedoTiles;
if(guideLayer->albedo.data)
{
OptixImage2D dummyOutput = guideLayer->albedo;
if(const OptixResult res = optixUtilDenoiserSplitImage(guideLayer->albedo, dummyOutput,
overlapWindowSizeInPixels,
tileWidth, tileHeight, albedoTiles))
return res;
}

std::vector<OptixUtilDenoiserImageTile> normalTiles;
if(guideLayer->normal.data)
{
OptixImage2D dummyOutput = guideLayer->normal;
if(const OptixResult res = optixUtilDenoiserSplitImage(guideLayer->normal, dummyOutput,
overlapWindowSizeInPixels,
tileWidth, tileHeight, normalTiles))
return res;
}

std::vector<OptixUtilDenoiserImageTile> flowTiles;
if(guideLayer->flow.data)
{
OptixImage2D dummyOutput = guideLayer->flow;
if(const OptixResult res = optixUtilDenoiserSplitImage(guideLayer->flow, dummyOutput,
overlapWindowSizeInPixels,
tileWidth, tileHeight, flowTiles))
return res;
}
NVIDIA OptiX 9.1 API std::vector<OptixUtilDenoiserImageTile> flowTrustTiles;
if(guideLayer->flowTrustworthiness.data)
{
    OptixImage2D dummyOutput = guideLayer->flowTrustworthiness;
    if(const OptixResult res = optixUtilDenoiserSplitImage(guideLayer->flowTrustworthiness,
dummyOutput,
                                 overlapWindowSizeInPixels,
                                 tileWidth, tileHeight, flowTrustTiles))
        return res;
}

std::vector<OptixUtilDenoiserImageTile> internalGuideLayerTiles;
if(guideLayer->previousOutputInternalGuideLayer.data && guideLayer->outputInternalGuideLayer.data)
{
    if(const OptixResult res =
optixUtilDenoiserSplitImage(guideLayer->previousOutputInternalGuideLayer,
                                 guideLayer->outputInternalGuideLayer,
                                 upscale * overlapWindowSizeInPixels,
                                 upscale * tileWidth, upscale * tileHeight,
internalGuideLayerTiles))
        return res;
}

for(size_t t = 0; t < tiles[0].size(); t++)
{
    std::vector<OptixDenoiserLayer> tlayers;
    for(unsigned int l = 0; l < numLayers; l++)
    {
        OptixDenoiserLayer layer = {};
        layer.input = (tiles[l])[t].input;
        layer.output = (tiles[l])[t].output;
        if(layers[l].previousOutput.data)
            layer.previousOutput = (prevTiles[l])[t].input;
        layer.type = layers[l].type;
        tlayers.push_back(layer);
    }

    OptixDenoiserGuideLayer gl = {};
    if(guideLayer->albedo.data)
        gl.albedo = albedoTiles[t].input;

    if(guideLayer->normal.data)
        gl.normal = normalTiles[t].input;

    if(guideLayer->flow.data)
        gl.flow = flowTiles[t].input;

    if(guideLayer->flowTrustworthiness.data)
        gl.flowTrustworthiness = flowTrustTiles[t].input;

    if(guideLayer->previousOutputInternalGuideLayer.data)
        gl.previousOutputInternalGuideLayer = internalGuideLayerTiles[t].input;

    if(guideLayer->outputInternalGuideLayer.data)
        gl.outputInternalGuideLayer = internalGuideLayerTiles[t].output;

    if(const OptixResult res =
        optixDenoiserInvoke(denoiser, stream, params, denoiserState, denoiserStateSizeInBytes,
                                  &gl, &tlayers[0], numLayers,
                                  (tiles[0])[t].inputOffsetX, (tiles[0])[t].inputOffsetY,
                                  scratch, scratchSizeInBytes))
        return res;
    }
    return OPTIX_SUCCESS;
}

// end group optix_utilities

---

## 8.11 optix_device.h File Reference

## Classes
• struct OptixIncomingHitObject
• struct OptixOutgoingHitObject
• class OptixCoopVec<T, N>

## Macros
• #define __OPTIX_INCLUDE_INTERNAL_HEADERS__
• #define OPTIX_INCLUDE_COOPERATIVE_VECTOR_UNSET
• #define OPTIX_INCLUDE_COOPERATIVE_VECTOR 1

## Functions
• template<typename... Payload>
static __forceinline__ __device__ void optixTrace (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixTraverse (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int missSBTIndex, Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixTrace (OptixPayloadTypeID type, OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixTraverse (OptixPayloadTypeID type, OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float raYTime, OptixVisibilityMask visibilityMask, unsigned int rayFlags, unsigned int SBToffset, unsigned int SBTstride, unsigned int missSBTIndex, Payload &... payload)
• static __forceinline__ __device__ void optixReorder (unsigned int coherenceHint, unsigned int numCoherenceHintBitsFromLSB)
• static __forceinline__ __device__ void optixReorder ()
• template<typename... Payload>
static __forceinline__ __device__ void optixInvoke (Payload &... payload)
• template<typename... Payload>
static __forceinline__ __device__ void optixInvoke (OptixPayloadTypeID type, Payload &... payload)
• static __forceinline__ __device__ void optixMakeHitObject (OptixTraversableHandle handle, float3 rayOrigin, float3 rayDirection, float tmin, float raYTime, unsigned int rayFlags, OptixTraverseData traverseData, const OptixTraversableHandle *transforms, unsigned int numTransforms)

---

•static __forceinline__ __device__ voidoptixMakeMissHitObject(unsigned int missSBTIndex,

float3 rayOrigin, float3 rayDirection, float tmin, float tmax, float rayTime, unsigned int rayFlags)

•static __forceinline__ __device__ voidoptixMakeNopHitObject()

•static __forceinline__ __device__ voidoptixHitObjectGetTraverseData(OptixTraverseData *∗*data)

•static __forceinline__ __device__ booloptixHitObjectIsHit()

•static __forceinline__ __device__ booloptixHitObjectIsMiss()

•static __forceinline__ __device__ booloptixHitObjectIsNop()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetSbtRecordIndex()

•static __forceinline__ __device__ voidoptixHitObjectSetSbtRecordIndex(unsigned int
sbtRecordIndex)

•static __forceinline__ __device__OptixTraversableHandle

optixHitObjectGetGASTraversableHandle()

•static __forceinline__ __device__ voidoptixSetPayload_0(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_1(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_3(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_2(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_4(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_5(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_6(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_7(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_8(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_9(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_10(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_11(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_12(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_13(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_14(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_15(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_16(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_17(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_18(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_19(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_20(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_21(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_22(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_23(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_24(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_25(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_26(unsigned int p)
•static __forceinline__ __device__ voidoptixSetPayload_27(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_29(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_28(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_30(unsigned int p)

•static __forceinline__ __device__ voidoptixSetPayload_31(unsigned int p)

•static __forceinline__ __device__ unsigned intoptixGetPayload_0()

•static __forceinline__ __device__ unsigned intoptixGetPayload_1()

•static __forceinline__ __device__ unsigned intoptixGetPayload_2()

•static __forceinline__ __device__ unsigned intoptixGetPayload_3()

•static __forceinline__ __device__ unsigned intoptixGetPayload_4()

•static __forceinline__ __device__ unsigned intoptixGetPayload_5()

---

• static __forceinline__ __device__ unsigned int optixGetPayload_6 ()
• static __forceline__ __device__ unsigned int optixGetPayload_7 ()
• static __forceline__ __device__ unsigned int optixGetPayload_8 ()
• static __forceline__ __device__ unsigned int optixGetPayload_9 ()
• static __forceline__ __device__ unsigned int optixGetPayload_10 ()
• static __forceline__ __device__ unsigned int optixGetPayload_11 ()
• static __forceline__ __device__ unsigned int optixGetPayload_12 ()
• static __forceline__ __device__ unsigned int optixGetPayload_13 ()
• static __forceline__ __device__ unsigned int optixGetPayload_14 ()
• static __forceline__ __device__ unsigned int optixGetPayload_15 ()
• static __forceline__ __device__ unsigned int optixGetPayload_16 ()
• static __forceline__ __device__ unsigned int optixGetPayload_17 ()
• static __forceline__ __device__ unsigned int optixGetPayload_18 ()
• static __forceline__ __device__ unsigned int optixGetPayload_19 ()
• static __forceline__ __device__ unsigned int optixGetPayload_20 ()
• static __forceline__ __device__ unsigned int optixGetPayload_21 ()
• static __forceline__ __device__ unsigned int optixGetPayload_22 ()
• static __forceline__ __device__ unsigned int optixGetPayload_23 ()
• static __forceline__ __device__ unsigned int optixGetPayload_24 ()
• static __forceline__ __device__ unsigned int optixGetPayload_25 ()
• static __forceline__ __device__ unsigned int optixGetPayload_26 ()
• static __forceline__ __device__ unsigned int optixGetPayload_27 ()
• static __forceline__ __device__ unsigned int optixGetPayload_28 ()
• static __forceline__ __device__ unsigned int optixGetPayload_29 ()
• static __forceline__ __device__ unsigned int optixGetPayload_30 ()
• static __forceline__ __device__ unsigned int optixGetPayload_31 ()
• static __forceline__ __device__ void optixSetPayloadTypes (unsigned int typeMask)
• static __forceline__ __device__ unsigned int optixUndefinedValue ()
• static __forceline__ __device__ unsigned int optixGetRemainingTraceDepth ()
• static __forceline__ __device__ float3 optixGetWorldRayOrigin ()
• static __forceline__ __device__ float3 optixHitObjectGetWorldRayOrigin ()
• static __forceline__ __device__ float3 optixGetWorldRayDirection ()
• static __forceline__ __device__ float3 optixHitObjectGetWorldRayDirection ()
• static __forceline__ __device__ float3 optixGetObjectRayOrigin ()
• static __forceline__ __device__ float3 optixGetObjectRayDirection ()
• static __forceline__ __device__ float optixGetRayTmin ()
• static __forceline__ __device__ float optixHitObjectGetRayTmin ()
• static __forceline__ __device__ float optixGetRayTmax ()
• static __forceline__ __device__ float optixHitObjectGetRayTmax ()
• static __forceline__ __device__ float optixGetRayTime ()
• static __forceline__ __device__ float optixHitObjectGetRayTime ()
• static __forceline__ __device__ unsigned int optixGetRayFlags ()
• static __forceline__ __device__ unsigned int optixHitObjectGetRayFlags ()
• static __forceline__ __device__ unsigned int optixGetRayVisibilityMask ()
• static __forceline__ __device__ OptixTraversableHandle optixGetInstanceTraversableFromIAS
(OptixTraversableHandle ias, unsigned int instIdx)
• static __forceline__ __device__ void optixGetTriangleVertexData (OptixTraversableHandle gas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float3 data[3])
• static __forceline__ __device__ void optixGetTriangleVertexDataFromHandle
(OptixTraversableHandle gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float3 data[3])

---

•static __forceinline__ __device__ voidoptixGetTriangleVertexData(float3 data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetTriangleVertexData(float3 data[3])

•static __forceinline__ __device__ voidoptixGetLinearCurveVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[2])

•static __forceinline__ __device__ voidoptixGetLinearCurveVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[2])

•static __forceinline__ __device__ voidoptixGetLinearCurveVertexData(float4 data[2])

•static __forceinline__ __device__ voidoptixHitObjectGetLinearCurveVertexData(float4 data[2])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexData
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineVertexData(float4 data[3])

•static __forceinline__ __device__ voidoptixGetQuadraticBSplineRocapsVertexData(float4
data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetQuadraticBSplineVertexData(float4
data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetQuadraticBSplineRocapsVertexData
(float4 data[3])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBSplineRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBSplineVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBSplineRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCatmullRomRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCatmullRomVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCatmullRomRocapsVertexData(float4
data[4])

---

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexData(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierRocapsVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixGetCubicBezierRocapsVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBezierVertexData(float4 data[4])

•static __forceinline__ __device__ voidoptixHitObjectGetCubicBezierRocapsVertexData(float4
data[4])

•static __forceinline__ __device__ voidoptixGetRibbonVertexData(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[3])

•static __forceinline__ __device__ voidoptixGetRibbonVertexDataFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4
data[3])

•static __forceinline__ __device__ voidoptixGetRibbonVertexData(float4 data[3])

•static __forceinline__ __device__ voidoptixHitObjectGetRibbonVertexData(float4 data[3])

•static __forceinline__ __device__ float3optixGetRibbonNormal(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float2 ribbonParameters)

•static __forceinline__ __device__ float3optixGetRibbonNormalFromHandle
(OptixTraversableHandlegas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float2
ribbonParameters)

•static __forceinline__ __device__ float3optixGetRibbonNormal(float2 ribbonParameters)

•static __forceinline__ __device__ float3optixHitObjectGetRibbonNormal(float2
ribbonParameters)

•static __forceinline__ __device__ voidoptixGetSphereData(OptixTraversableHandlegas,
unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[1])

•static __forceinline__ __device__ voidoptixGetSphereDataFromHandle(OptixTraversableHandle
gas, unsigned int primIdx, unsigned int sbtGASIndex, float time, float4 data[1])

•static __forceinline__ __device__ voidoptixGetSphereData(float4 data[1])

•static __forceinline__ __device__ voidoptixHitObjectGetSphereData(float4 data[1])

•static __forceinline__ __device__OptixTraversableHandleoptixGetGASTraversableHandle()

•static __forceinline__ __device__ floatoptixGetGASMotionTimeBegin(OptixTraversableHandle
gas)

•static __forceinline__ __device__ floatoptixGetGASMotionTimeEnd(OptixTraversableHandle
gas)

•static __forceinline__ __device__ unsigned intoptixGetGASMotionStepCount
(OptixTraversableHandlegas)

•static __forceinline__ __device__ voidoptixGetWorldToObjectTransformMatrix(float m[12])

•static __forceinline__ __device__ voidoptixGetObjectToWorldTransformMatrix(float m[12])

•static __forceinline__ __device__ float3optixTransformPointFromWorldToObjectSpace(float3
point)

•static __forceinline__ __device__ float3optixTransformVectorFromWorldToObjectSpace(float3
vec)

•static __forceinline__ __device__ float3optixTransformNormalFromWorldToObjectSpace(float3
normal)

•static __forceinline__ __device__ float3optixTransformPointFromObjectToWorldSpace(float3
point)

---

• static __forceinline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace (float3 vec)
• static __forceline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace (float3 normal)
• static __forceline__ __device__ void optixHitObjectGetWorldToObjectTransformMatrix (float m[12])
• static __forceline__ __device__ void optixHitObjectGetObjectToWorldTransformMatrix (float m[12])
• static __forceline__ __device__ float3 optixHitObjectTransformPointFromWorldToObjectSpace (float3 point)
• static __forceline__ __device__ float3 optixHitObjectTransformVectorFromWorldToObjectSpace (float3 vec)
• static __forceline__ __device__ float3 optixHitObjectTransformNormalFromWorldToObjectSpace (float3 normal)
• static __forceline__ __device__ float3 optixHitObjectTransformPointFromObjectToWorldSpace (float3 point)
• static __forceline__ __device__ float3 optixHitObjectTransformVectorFromObjectToWorldSpace (float3 vec)
• static __forceline__ __device__ float3 optixHitObjectTransformNormalFromObjectToWorldSpace (float3 normal)
• template<typename HitState >
static __forceline__ __device__ void optixGetWorldToObjectTransformMatrix (const HitState &hs, float m[12])
• template<typename HitState >
static __forceline__ __device__ void optixGetObjectToWorldTransformMatrix (const HitState &hs, float m[12])
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformPointFromWorldToObjectSpace (const HitState &hs, float3 point)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace (const HitState &hs, float3 vec)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformNormalFromWorldToObjectSpace (const HitState &hs, float3 normal)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformPointFromObjectToWorldSpace (const HitState &hs, float3 point)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace (const HitState &hs, float3 vec)
• template<typename HitState >
static __forceline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace (const HitState &hs, float3 normal)
• static __forceline__ __device__ unsigned int optixGetTransformListSize ()
• static __forceline__ __device__ unsigned int optixHitObjectGetTransformListSize ()
• static __forceline__ __device__ OptixTraversableHandle optixGetTransformListHandle (unsigned int index)
• static __forceline__ __device__ OptixTraversableHandle optixHitObjectGetTransformListHandle (unsigned int index)
• static __forceline__ __device__ OptixTransformType optixGetTransformTypeFromHandle (OptixTraversableHandle handle)

---

•static __forceinline__ __device__ constOptixStaticTransform *∗*
optixGetStaticTransformFromHandle(OptixTraversableHandlehandle)

•static __forceinline__ __device__ constOptixSRTMotionTransform *∗*
optixGetSRTMotionTransformFromHandle(OptixTraversableHandlehandle)

•static __forceinline__ __device__ constOptixMatrixMotionTransform *∗*
optixGetMatrixMotionTransformFromHandle(OptixTraversableHandlehandle)

•static __forceinline__ __device__ unsigned intoptixGetInstanceIdFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__OptixTraversableHandleoptixGetInstanceChildFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ const float4 *∗* optixGetInstanceTransformFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ const float4 *∗* optixGetInstanceInverseTransformFromHandle
(OptixTraversableHandlehandle)

•static __device__ __forceinline__CUdeviceptroptixGetGASPointerFromHandle
(OptixTraversableHandlehandle)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5, unsigned int a6)

•static __forceinline__ __device__ booloptixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int
a5, unsigned int a6, unsigned int a7)

•static __forceinline__ __device__ unsigned intoptixGetAttribute_0()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_1()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_2()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_3()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_4()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_5()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_7()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_0()

•static __forceinline__ __device__ unsigned intoptixGetAttribute_6()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_1()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_2()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_3()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_4()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_5()

•static __forceinline__ __device__ unsigned intoptixHitObjectGetAttribute_6()

---

• static __forceinline__ __device__ unsigned int optixHitObjectGetAttribute_7 ()
• static __forceline__ __device__ void optixTerminateRay ()
• static __forceline__ __device__ void optixIgnoreIntersection ()
• static __forceline__ __device__ unsigned int optixGetPrimitiveIndex ()
• static __forceline__ __device__ unsigned int optixGetClusterId ()
• static __forceline__ __device__ unsigned int optixHitObjectGetClusterId ()
• static __forceline__ __device__ unsigned int optixHitObjectGetPrimitiveIndex ()
• static __forceline__ __device__ unsigned int optixGetSbtGASIndex ()
• static __forceline__ __device__ unsigned int optixHitObjectGetSbtGASIndex ()
• static __forceline__ __device__ unsigned int optixGetInstanceId ()
• static __forceline__ __device__ unsigned int optixHitObjectGetInstanceId ()
• static __forceline__ __device__ unsigned int optixGetInstanceIndex ()
• static __forceline__ __device__ unsigned int optixHitObjectGetInstanceIndex ()
• static __forceline__ __device__ unsigned int optixGetHitKind ()
• static __forceline__ __device__ unsigned int optixHitObjectGetHitKind ()
• static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType (unsigned int hitKind)
• static __forceline__ __device__ bool optixIsFrontFaceHit (unsigned int hitKind)
• static __forceline__ __device__ bool optixIsBackFaceHit (unsigned int hitKind)
• static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType ()
• static __forceline__ __device__ bool optixIsFrontFaceHit ()
• static __forceline__ __device__ bool optixIsBackFaceHit ()
• static __forceline__ __device__ bool optixIsTriangleHit ()
• static __forceline__ __device__ bool optixIsTriangleFrontFaceHit ()
• static __forceline__ __device__ bool optixIsTriangleBackFaceHit ()
• static __forceline__ __device__ float2 optixGetTriangleBarycentrics ()
• static __forceline__ __device__ float2 optixHitObjectGetTriangleBarycentrics ()
• static __forceline__ __device__ float optixGetCurveParameter ()
• static __forceline__ __device__ float optixHitObjectGetCurveParameter ()
• static __forceline__ __device__ float2 optixGetRibbonParameters ()
• static __forceline__ __device__ float2 optixHitObjectGetRibbonParameters ()
• static __forceline__ __device__ uint3 optixGetLaunchIndex ()
• static __forceline__ __device__ uint3 optixGetLaunchDimensions ()
• static __forceline__ __device__ CUdeviceptr optixGetSbtDataPointer ()
• static __forceline__ __device__ CUdeviceptr optixHitObjectGetSbtDataPointer ()
• static __forceline__ __device__ void optixThrowException (int exceptionCode)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4, unsigned int exceptionDetail5)

---

• static __forceinline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4, unsigned int exceptionDetail5, unsigned int exceptionDetail6)
• static __forceline__ __device__ void optixThrowException (int exceptionCode, unsigned int exceptionDetail0, unsigned int exceptionDetail1, unsigned int exceptionDetail2, unsigned int exceptionDetail3, unsigned int exceptionDetail4, unsigned int exceptionDetail5, unsigned int exceptionDetail6, unsigned int exceptionDetail7)
• static __forceline__ __device__ int optixGetExceptionCode ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_0 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_1 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_2 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_3 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_4 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_5 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_6 ()
• static __forceline__ __device__ unsigned int optixGetExceptionDetail_7 ()
• static __forceline__ __device__ char * optixGetExceptionLineInfo ()
• template<typename ReturnT , typename... ArgTypes>
static __forceline__ __device__ ReturnT optixDirectCall (unsigned int sbtIndex, ArgTypes...
args)
• template<typename ReturnT , typename... ArgTypes>
static __forceline__ __device__ ReturnT optixContinuationCall (unsigned int sbtIndex,
ArgTypes...
args)
• static __forceline__ __device__ uint4 optixTexFootprint2D (unsigned long long tex, unsigned int texInfo, float x, float y, unsigned int *singleMipLevel)
• static __forceline__ __device__ uint4 optixTexFootprint2DLod (unsigned long long tex, unsigned int texInfo, float x, float y, float level, bool coarse, unsigned int *singleMipLevel)
• static __forceline__ __device__ uint4 optixTexFootprint2DGrad (unsigned long long tex, unsigned int texInfo, float x, float y, float dPdx_x, float dPdx_y, float dPdy_x, float dPdy_y, bool coarse, unsigned int *singleMipLevel)
• template<typename VecTOut >
static __forceline__ __device__ VecTOut optixCoopVecLoad (CUdeviceptr ptr)
• template<typename VecTOut , typename T >
static __forceline__ __device__ VecTOut optixCoopVecLoad (T *ptr)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecExp2 (const VecT &vec)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecLog2 (const VecT &vec)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecTanh (const VecT &vec)
• template<typename VecTOut , typename VecTIn >
static __forceline__ __device__ VecTOut optixCoopVecCvt (const VecTIn &vec)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecMin (const VecT &vecA, const VecT &vecB)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecMin (const VecT &vecA, typename VecT ::value_type B)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecMax (const VecT &vecA, const VecT &vecB)

---

• template<typename VecT >
static __forceinline__ __device__ VecT optixCoopVecMax (const VecT &vecA, typename VecT
::value_type B)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecMul (const VecT &vecA, const VecT &vecB)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecAdd (const VecT &vecA, const VecT &vecB)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecSub (const VecT &vecA, const VecT &vecB)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecStep (const VecT &vecA, const VecT &vecB)
• template<typename VecT >
static __forceline__ __device__ VecT optixCoopVecFFMA (const VecT &vecA, const VecT
&vecB, const VecT &vecC)
• template<typename VecTOut , typename VecTIn , OptixCoopVecElemType inputInterpretation,
OptixCoopVecMatrixLayout matrixLayout, bool transpose, unsigned int N, unsigned int K,
OptixCoopVecElemType matrixElementType, OptixCoopVecElemType biasElementType>
static __forceline__ __device__ VecTOut optixCoopVecMatMul (const VecTIn &inputVector,
CUdeviceptr matrix, unsigned matrixOffsetInBytes, CUdeviceptr bias, unsigned
biasOffsetInBytes, unsigned rowColumnStrideInBytes=0)
• template<typename VecTOut , typename VecTIn , OptixCoopVecElemType inputInterpretation,
OptixCoopVecMatrixLayout matrixLayout, bool transpose, unsigned int N, unsigned int K,
OptixCoopVecElemType matrixElementType>
static __forceline__ __device__ VecTOut optixCoopVecMatMul (const VecTIn &inputVector,
CUdeviceptr matrix, unsigned matrixOffsetInBytes, unsigned rowColumnStrideInBytes=0)
• template<typename VecTIn >
static __forceline__ __device__ void optixCoopVecReduceSumAccumulate (const VecTIn
&inputVector, CUdeviceptr outputVector, unsigned offsetInBytes)
• template<typename VecTA , typename VecTB , OptixCoopVecMatrixLayout matrixLayout =
OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL>
static __forceline__ __device__ void optixCoopVecOuterProductAccumulate (const VecTA
&vecA, const VecTB &vecB, CUdeviceptr outputMatrix, unsigned offsetInBytes, unsigned
rowColumnStrideInBytes=0)
• template<unsigned int N, unsigned int K, OptixCoopVecElemType elementType,
OptixCoopVecMatrixLayout layout = OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_
OPTIMAL, unsigned int rowColumnStrideInBytes = 0>
static __forceline__ __device__ unsigned int optixCoopVecGetMatrixSize ()

## 8.11.1 Detailed Description
OptiX public API header.

## Author
## NVIDIA Corporation

OptiX public API Reference - Device API declarations

## 8.11.2 Macro Definition Documentation

## 8.11.2.1 __OPTIX_INCLUDE_INTERNAL_HEADERS__
#define __OPTIX_INCLUDE_INTERNAL_HEADERS__

---

## 8.11.2.2 OPTIX_INCLUDE_COOPERATIVE_VECTOR

#define OPTIX_INCLUDE_COOPERATIVE_VECTOR 1

## 8.11.2.3 OPTIX_INCLUDE_COOPERATIVE_VECTOR_UNSET

## #define OPTIX_INCLUDE_COOPERATIVE_VECTOR_UNSET

8.12 optix_device.h

Go to the documentation of this file.
1/*
2* SPDX-FileCopyrightText: Copyright (c) 2010-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
3* SPDX-License-Identifier: LicenseRef-NvidiaProprietary
4*
5* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
6* property and proprietary rights in and to this material, related
7* documentation and any modifications thereto. Any use, reproduction,
8* disclosure or distribution of this material and related documentation
9* without an express license agreement from NVIDIA CORPORATION or
10* its affiliates is strictly prohibited.
11*/
17
18#ifndef OPTIX_OPTIX_DEVICE_H
19#define OPTIX_OPTIX_DEVICE_H
20
21#if defined(__cplusplus) && (__cplusplus < 201103L) &&!defined(_WIN32)
22#error Device code for OptiX requires at least C++11. Consider adding "--std c++11"to the nvcc
command-line.
23#endif
24
25#include "optix_types.h"
26
29
51template<typename... Payload>
52static__forceinline__ __device__voidoptixTrace(OptixTraversableHandlehandle,
53 float3 rayOrigin,
54 float3 rayDirection,
55floattmin,
56floattmax,
57floatrayTime,
58OptixVisibilityMaskvisibilityMask,
59unsignedintrayFlags,
60unsignedintSBToffset,
61unsignedintSBTstride,
62unsignedintmissSBTIndex,
63 Payload&... payload);
64
84template<typename... Payload>
85static__forceinline__ __device__voidoptixTraverse(OptixTraversableHandlehandle,
86 float3 rayOrigin,
87 float3 rayDirection,
88 floattmin,
89 floattmax,
90 floatrayTime,
91 OptixVisibilityMaskvisibilityMask,
92 unsignedintrayFlags,
93 unsignedintSBToffset,
94 unsignedintSBTstride,
95 unsignedintmissSBTIndex,
96 Payload&... payload);
97
115template<typename... Payload>
116static__forceinline__ __device__voidoptixTrace(OptixPayloadTypeIDtype,

---

118
119
120
121
122
123
124
125
126
127
128
129
150
151
152
153
154
155
156
157
158
159
160
161
162
163
164
175
numC
176
180
181
190
191
192
202
203
204
220
221
222
223
224
225
226
227
228
229
243
244
245
246
247
248
249
250
258
259
266
267
271
272
276
277
283
284
291

|  | float3 | rayOrigin, |
| --- | --- | --- |
|  | float3 | rayDirection, |
|  | float | tmin, |
|  | float | tmax, |
|  | float | rayTime, |
|  | OptixVisibilityMask | visibilityMask, |
|  | unsigned int | rayFlags, |
|  | unsigned int | SBToffset, |
|  | unsigned int | SBTstride, |
|  | unsigned int | missSBTIndex, |
|  | Payload&amp;... | payload); |
| template &lt;typename... Payload&gt; |  |  |
| static __forceinline__ __device__ void optixTraverse(OptixPayloadTypeID type, |  |  |
|  | OptixTraversableHandle handle, |  |
|  | float3 | rayOrigin, |
| --- | --- | --- |
|  | float3 | rayDirection, |
|  | float | tmin, |
|  | float | tmax, |
|  | float | rayTime, |
|  | OptixVisibilityMask | visibilityMask, |
|  | unsigned int | rayFlags, |
|  | unsigned int | SBToffset, |
|  | unsigned int | SBTstride, |
|  | unsigned int | missSBTIndex, |
|  | Payload&amp;... payload); |  |

175 static __forceinline__ __device__ void optixReor numCoherenceHintBitsFromLSB);

r(unsigned int coherenceHint, unsigned int

180 static __forceinline__ __device__ void optixReorder();

190 template <typename... Payload>

191 static __forceinline__ __device__ void optixInvoke(Payload&... payload);

202 template <typename... Payload>

203 static __forceinline__ __device__ void optixInvoke(OptixPayloadTypeID type, Payload&... payload);

| 220 | static __forceinline__ __device__ void optixMakeHitObject(OptixTraversableHandle | handle, |
| --- | --- | --- |
| 221 |  | float3 rayOrigin, |
| 222 |  | float3 rayDirection, |
| 223 |  | float tmin, |
| 224 |  | float rayTime, |
| 225 |  | unsigned int rayFlags, |
| 226 |  | OptixTraverseData traverseData, |
| 227 |  | const OptixTraversableHandle* transforms, |
| 228 |  | unsigned int numTransforms) |

243 static __forceinline__ __device__ void optixMakeMissHitObject(unsigned int missSBTIndex,

| 243 | static __forceinline__ __device__ void optixMakeMissHitObject(unsigned int missSBTIndex, |
| --- | --- |
| 244 | float3 rayOrigin, |
| 245 | float3 rayDirection, |
| 246 | float tmin, |
| 247 | float tmax, |
| 248 | float rayTime, |
| 249 | unsigned int rayFlags); |

258 static __forceinline__ __device__ void optixMakeNopHitObject();

266 static __forceinline__ __device__ void optixHitObjectGetTraverseData(OptixTraverseData* data);

271 static __forceinline__ __device__ bool optixHitObjectIsHit();

276 static __forceinline__ __device__ bool optixHitObjectIsMiss();

283 static __forceinline__ __device__ bool optixHitObjectIsNop();

291 static __forceinline__ __device__ unsigned int optixHitObjectGetSbtRecordIndex();

---

| static | __forceinline__ | __device__ | OptixTraversableHandle optixHitObjectGet |
| --- | --- | --- | --- |
| static | __forceinline__ | __device__ | void optixSetPayload_0(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_1(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_2(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_3(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_4(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_5(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_6(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_7(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_8(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_9(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_10(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_11(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_12(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_13(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_14(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_15(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_16(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_17(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_18(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_19(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_20(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_21(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_22(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_23(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_24(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_25(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_26(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_27(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_28(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_29(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_30(unsigned int p); |
| static | __forceinline__ | __device__ | void optixSetPayload_31(unsigned int p); |

| 348 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_0(); |
| --- | --- | --- | --- | --- | --- |
| 349 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_1(); |
| 350 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_2(); |
| 351 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_3(); |
| 352 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_4(); |
| 353 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_5(); |
| 354 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_6(); |
| 355 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_7(); |
| 356 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_8(); |
| 357 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_9(); |
| 358 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_10(); |
| 359 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_11(); |
| 360 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_12(); |
| 361 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_13(); |
| 362 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_14(); |
| 363 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_15(); |
| 364 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_16(); |
| 365 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_17(); |
| 366 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_18(); |
| 367 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_19(); |
| 368 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_20(); |
| 369 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_21(); |
| 370 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_22(); |
| 371 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_23(); |
| 372 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_24(); |
| 373 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_25(); |
| 374 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_26(); |
| 375 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_27(); |
| 376 | static | __forceinline__ | __device__ | unsigned int | optixGetPayload_28(); |

---

377static__forceinline__ __device__unsignedintoptixGetPayload_29();
378static__forceinline__ __device__unsignedintoptixGetPayload_30();
379static__forceinline__ __device__unsignedintoptixGetPayload_31();
380
389static__forceinline__ __device__voidoptixSetPayloadTypes(unsignedinttypeMask);
390
394static__forceinline__ __device__unsignedintoptixUndefinedValue();
395
404static__forceinline__ __device__unsignedintoptixGetRemainingTraceDepth();
405
412static__forceinline__ __device__ float3optixGetWorldRayOrigin();
413
419static__forceinline__ __device__ float3optixHitObjectGetWorldRayOrigin();
420
427static__forceinline__ __device__ float3optixGetWorldRayDirection();
428
434static__forceinline__ __device__ float3optixHitObjectGetWorldRayDirection();
435
439static__forceinline__ __device__ float3optixGetObjectRayOrigin();
440
444static__forceinline__ __device__ float3optixGetObjectRayDirection();
445
449static__forceinline__ __device__floatoptixGetRayTmin();
450
456static__forceinline__ __device__floatoptixHitObjectGetRayTmin();
457
466static__forceinline__ __device__floatoptixGetRayTmax();
467
476static__forceinline__ __device__floatoptixHitObjectGetRayTmax();
477
483static__forceinline__ __device__floatoptixGetRayTime();
484
490static__forceinline__ __device__floatoptixHitObjectGetRayTime();
491
495static__forceinline__ __device__unsignedintoptixGetRayFlags();
496
500static__forceinline__ __device__unsignedintoptixHitObjectGetRayFlags();
501
505static__forceinline__ __device__unsignedintoptixGetRayVisibilityMask();
506
513static__forceinline__ __device__OptixTraversableHandle
optixGetInstanceTraversableFromIAS(OptixTraversableHandleias,unsignedintinstIdx);
514
527static__forceinline__ __device__voidoptixGetTriangleVertexData(OptixTraversableHandlegas,
528 unsignedintprimIdx,
529 unsignedintsbtGASIndex,
530 floattime,
531 float3 data[3]);
532
546static__forceinline__ __device__voidoptixGetTriangleVertexDataFromHandle(OptixTraversableHandlegas,
547 unsignedintprimIdx,
548 unsignedintsbtGASIndex,
549 floattime,
550 float3 data[3]);
551
560static__forceinline__ __device__voidoptixGetTriangleVertexData(float3 data[3]);
561
568static__forceinline__ __device__voidoptixHitObjectGetTriangleVertexData(float3 data[3]);
569
570
586static__forceinline__ __device__voidoptixGetLinearCurveVertexData(OptixTraversableHandlegas,
587 unsignedintprimIdx,
588 unsignedintsbtGASIndex,
589 floattime,
590 float4 data[2]);
591
607static__forceinline__ __device__voidoptixGetLinearCurveVertexDataFromHandle(OptixTraversableHandle

---

| gas, |  |  |
| --- | --- | --- |
|  | unsigned int | primIdx, |
|  | unsigned int | sbtGASIndex, |
|  | float | time, |
|  | float4 | data[2]); |
| static __forceinline__ __device__ void optixGetLinearCurveVertexData(float4 data[2]); |  |  |
| static __forceline__ __device__ void optixHitObjectGetLinearCurveVertexData(float4 data[2]); |  |  |
| static __forceline__ __device__ void optixGetQuadraticBSplineVertexData(OptixTraversableHandle gas, | unsigned int | primIdx, |
|  | unsigned int | sbtGASIndex, |
|  | float | time, |
|  | float4 | data[3]); |
| static __forceline__ __device__ void optixGetQuadraticBSplineVertexDataFromHandle(OptixTraversableHandle gas, | unsigned int | primIdx, |
|  | unsigned int | sbtGASIndex, |
|  | float | time, |
|  | float4 | data[3]); |
| static __forceline__ __device__ void optixGetQuadraticBSplineRocapsVertexDataFromHandle(OptixTraversableHandle gas, | unsigned int | primIdx, |
|  | unsigned int | sbtGASIndex, |
|  | float | time, |
|  | float4 | data[3]); |
| static __forceline__ __device__ void optixGetQuadraticBSplineVertexData(float4 data[3]); |  |  |
|  | unsigned int | primIdx, |
|  | unsigned int | sbtGASIndex, |
|  | float | time, |
|  | float4 | data[4]); |
| static __forceline__ __device__ void optixGetCubicBSplineVertexDataFromHandle(OptixTraversableHandle gas, | unsigned int | primIdx, |
|  | unsigned int | sbtGASIndex, |
|  | float | time, |
|  | float4 | data[4]); |
| static __forceline__ __device__ void optixGetCubicBSplineRocapsVertexDataFromHandle(OptixTraversableHandle gas, | unsigned int | primIdx, |
|  | unsigned int | sbtGASIndex, |
|  | float | time, |
|  | float4 | data[4]); |
| static __forceline__ __device__ void optixGetCubicBSplineVertexData(float4 data[4]); |  |  |
|  | unsigned int | primIdx, |
|  | sbtGASIndex, |  |
|  | float | time, |
|  | float4 | data[4]); |

---

|  | unsigned int | sbtGASIndex, |
| --- | --- | --- |
|  | float | time, |
|  | float4 | data[4]); |
| static __forceinline__ __device__ void optixGetCatmullRomVertexDataFromHandle(OptixTraversableHandle gas, |  |  |
|  |  | unsigned int |
|  |  | primIdx, |
|  |  | unsigned int |
|  |  | sbtGASIndex, |
|  |  | float |
|  |  | time, |
|  |  | float4 |
| static __forceinline__ __device__ void optixGetCatmullRomRocapsVertexDataFromHandle(OptixTraversableHandle gas, |  |  |
|  |  | unsigned int |
|  |  | primIdx, |
|  |  | unsigned int |
|  |  | sbtGASIndex, |
|  |  | float |
|  |  | time, |
|  |  | float4 |
| static __forceinline__ __device__ void optixGetCatmullRomVertexData(float4 data[4]); |  |  |
| static __forceinline__ __device__ void optixGetCatmullRomRocapsVertexData(float4 data[4]); |  |  |
| static __forceinline__ __device__ void optixHitObjectGetCatmullRomVertexData(float4 data[4]); |  |  |
| static __forceinline__ __device__ void optixHitObjectGetCatmullRomRocapsVertexData(float4 data[4]); |  |  |
| static __forceinline__ __device__ void optixGetCubicBezierVertexData(OptixTraversableHandle gas, |  |  |
|  |  | unsigned int |
|  |  | primIdx, |
|  |  | unsigned int |
|  |  | sbtGASIndex, |
|  |  | float |
|  |  | time, |
|  |  | float4 |
| static __forceinline__ __device__ void optixGetCubicBezierVertexDataFromHandle(OptixTraversableHandle gas, |  |  |
|  |  | unsigned int |
|  |  | primIdx, |
|  |  | unsigned int |
|  |  | sbtGASIndex, |
|  |  | float |
|  |  | time, |
|  |  | float4 |
| static __forceinline__ __device__ void optixGetRibbonVertexDataFromHandle(OptixTraversableHandle gas, |  |  |
|  |  | unsigned int |
|  |  | primIdx, |
|  |  | unsigned int |
|  |  | sbtGASIndex, |
|  |  | float |
|  |  | time, |
|  |  | float4 |
| static __forceinline__ __device__ void optixGetRibbonVertexData(float4 data[3]); |  |  |
| static __forceinline__ __device__ void optixHitObjectGetRibbonVertexData(float4 data[3]); |  |  |
| static __forceinline__ __device__ float3 optixGetRibbonNormal(OptixTraversableHandle gas, |  |  |

---

<u>8.12 optix_device.h</u>
unsignedintprimIdx, unsignedintsbtGASIndex, floattime, float2 ribbonParameters); 962 966static__forceinline__ __device__ float3optixGetRibbonNormalFromHandle(OptixTraversableHandlegas, 967 unsignedintprimIdx, 968 unsignedintsbtGASIndex, 969 floattime, 970 float2 ribbonParameters); 971 975static__forceinline__ __device__ float3optixGetRibbonNormal(float2 ribbonParameters); 976 980static__forceinline__ __device__ float3optixHitObjectGetRibbonNormal(float2 ribbonParameters); 981 997static__forceinline__ __device__voidoptixGetSphereData(OptixTraversableHandlegas, 998 unsignedintprimIdx, 999 unsignedintsbtGASIndex, 1000 floattime, 1001 float4 data[1]); 1002 1018static__forceinline__ __device__voidoptixGetSphereDataFromHandle(OptixTraversableHandlegas, 1019 unsignedintprimIdx, 1020 unsignedintsbtGASIndex, 1021 floattime, 1022 float4 data[1]); 1023 1032static__forceinline__ __device__voidoptixGetSphereData(float4 data[1]); 1033 1040static__forceinline__ __device__voidoptixHitObjectGetSphereData(float4 data[1]); 1041 1046static__forceinline__ __device__OptixTraversableHandleoptixGetGASTraversableHandle(); 1047 1051static__forceinline__ __device__floatoptixGetGASMotionTimeBegin(OptixTraversableHandlegas); 1052 1056static__forceinline__ __device__floatoptixGetGASMotionTimeEnd(OptixTraversableHandlegas); 1057 1061static__forceinline__ __device__unsignedintoptixGetGASMotionStepCount(OptixTraversableHandlegas); 1062 1069static__forceinline__ __device__voidoptixGetWorldToObjectTransformMatrix(floatm[12]); 1070 1077static__forceinline__ __device__voidoptixGetObjectToWorldTransformMatrix(floatm[12]); 1078 1085static__forceinline__ __device__ float3optixTransformPointFromWorldToObjectSpace(float3 point); 1086 1093static__forceinline__ __device__ float3optixTransformVectorFromWorldToObjectSpace(float3 vec); 1094 1101static__forceinline__ __device__ float3optixTransformNormalFromWorldToObjectSpace(float3 normal); 1102 1109static__forceinline__ __device__ float3optixTransformPointFromObjectToWorldSpace(float3 point); 1110 1117static__forceinline__ __device__ float3optixTransformVectorFromObjectToWorldSpace(float3 vec); 1118 1125static__forceinline__ __device__ float3optixTransformNormalFromObjectToWorldSpace(float3 normal); 1126 1133static__forceinline__ __device__voidoptixHitObjectGetWorldToObjectTransformMatrix(floatm[12]); 1134 1141static__forceinline__ __device__voidoptixHitObjectGetObjectToWorldTransformMatrix(floatm[12]); 1142 1149static__forceinline__ __device__ float3optixHitObjectTransformPointFromWorldToObjectSpace(float3 point); 1150 1157static__forceinline__ __device__ float3optixHitObjectTransformVectorFromWorldToObjectSpace(float3 vec);

1165static__forceinline__ __device__ float3optixHitObjectTransformNormalFromWorldToObjectSpace(float3 normal);

NVIDIA OptiX 9.1 API

---

1166
static __forceinline__ __device__ float3 optixHitObjectTransformPointFromObjectToWorldSpace(float3
point);
1174
static __forceline__ __device__ float3 optixHitObjectTransformVectorFromObjectToWorldSpace(float3
vec);
1182
static __forceline__ __device__ float3 optixHitObjectTransformNormalFromObjectToWorldSpace(float3
normal);
1190
template <typename HitState>
static __forceline__ __device__ void optixGetWorldToObjectTransformMatrix(const HitState& hs, float
m[12]);
1207
template <typename HitState>
static __forceline__ __device__ void optixGetObjectToWorldTransformMatrix(const HitState& hs, float
m[12]);
1216
template <typename HitState>
static __forceline__ __device__ float3 optixTransformPointFromWorldToObjectSpace(const HitState& hs,
float3 point);
1225
template <typename HitState>
static __forceline__ __device__ float3 optixTransformVectorFromWorldToObjectSpace(const HitState& hs,
float3 vec);
1234
template <typename HitState>
static __forceline__ __device__ float3 optixTransformNormalFromWorldToObjectSpace(const HitState& hs,
float3 normal);
1243
template <typename HitState>
static __forceline__ __device__ float3 optixTransformPointFromObjectToWorldSpace(const HitState& hs,
float3 point);
1252
template <typename HitState>
static __forceline__ __device__ float3 optixTransformVectorFromObjectToWorldSpace(const HitState& hs,
float3 vec);
1261
template <typename HitState>
static __forceline__ __device__ float3 optixTransformNormalFromObjectToWorldSpace(const HitState& hs,
float3 normal);
1270
static __forceline__ __device__ unsigned int optixGetTransformListSize();
1275
static __forceline__ __device__ unsigned int optixHitObjectGetTransformListSize();
1285
static __forceline__ __device__ OptixTraversableHandle optixGetTransformListHandle(unsigned int
index);
1290
static __forceline__ __device__ OptixTraversableHandle optixHitObjectGetTransformListHandle(unsigned
int index);
1300
struct OptixIncomingHitObject
{
    __forceline__ __device__ float getRayTime()const { return optixGetRayTime(); }
    __forceline__ __device__ unsigned int getTransformListSize()const { return
optixGetTransformListSize(); }
    __forceline__ __device__ OptixTraversableHandle getTransformListHandle(unsigned int index)const
{
        return optixGetTransformListHandle(index);
    }
};
1310
struct OptixOutgoingHitObject
{
    __forceline__ __device__ float getRayTime()const { return optixHitObjectGetRayTime(); }
    __forceline__ __device__ unsigned int getTransformListSize()const

NVIDIA OptiX 9.1 API

---

{
    return optixHitObjectGetTransformListSize();
}
__forceinline__ __device__ OptixTraversableHandle getTransformListHandle(unsigned int index)const
{
    return optixHitObjectGetTransformListHandle(index);
}
};

static __forceline__ __device__ OptixTransformType
optixGetTransformTypeFromHandle(OptixTraversableHandle handle);

static __forceline__ __device__ const OptixStaticTransform*
optixGetStaticTransformFromHandle(OptixTraversableHandle handle);

static __forceline__ __device__ const OptixSRTMotionTransform*
optixGetSRTMotionTransformFromHandle(OptixTraversableHandle handle);

static __forceline__ __device__ const OptixMatrixMotionTransform*
optixGetMatrixMotionTransformFromHandle(OptixTraversableHandle handle);

static __forceline__ __device__ unsigned int optixGetInstanceIdFromHandle(OptixTraversableHandle
handle);

static __forceline__ __device__ OptixTraversableHandle
optixGetInstanceChildFromHandle(OptixTraversableHandle handle);

static __forceline__ __device__ const float4*
optixGetInstanceTransformFromHandle(OptixTraversableHandle handle);

static __forceline__ __device__ const float4*
optixGetInstanceInverseTransformFromHandle(OptixTraversableHandle handle);

static __device__ __forceinline__ CUdeviceptr optixGetGASPointerFromHandle(OptixTraversableHandle
handle);

static __forceinline__ __device__ bool optixReportIntersection(float hitT, unsigned int hitKind);

static __forceinline__ __device__ bool optixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0);

static __forceinline__ __device__ bool optixReportIntersection(float hitT, unsigned int hitKind,
unsigned int a0, unsigned int a1);

static __forceinline__ __device__ bool optixReportIntersection(float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1,
unsigned int a2,
unsigned int a3);

static __forceinline__ __device__ bool optixReportIntersection(float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1,
unsigned int a2,
unsigned int a3,
unsigned int a4);

static __forceinline__ __device__ bool optixReportIntersection(float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1,
unsigned int a2,
unsigned int a3, unsigned int a4,
unsigned int a5);

static __forceinline__ __device__ bool optixReportIntersection(float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1,
unsigned int a2,
unsigned int a3,
unsigned int a4,
unsigned int a5,
unsigned int a6);

static __forceinline__ __device__ bool optixReportIntersection(float hitT,
unsigned int hitKind,
unsigned int a0,
unsigned int a1,
unsigned int a2,
unsigned int a3,
unsigned int a4,
unsigned int a5,
unsigned int a6,
unsigned int a7);

static __forceline__ __device__ unsigned int optixGetAttribute_0();
static __forceline__ __device__ unsigned int optixGetAttribute_1();
static __forceline__ __device__ unsigned int optixGetAttribute_2();
static __forceline__ __device__ unsigned int optixGetAttribute_3();
static __forceline__ __device__ unsigned int optixGetAttribute_4();
static __forceline__ __device__ unsigned int optixGetAttribute_5();
static __forceline__ __device__ unsigned int optixGetAttribute_6();
static __forceline__ __device__ unsigned int optixGetAttribute_7();

static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_0();
static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_1();
static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_2();
static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_3();
static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_4();
static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_5();
static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_6();
static __forceline__ __device__ unsigned int optixHitObjectGetAttribute_7();

static __forceline__ __device__ void optixTerminateRay();

static __forceline__ __device__ void optixIgnoreIntersection();

static __forceline__ __device__ unsigned int optixGetPrimitiveIndex();

static __forceline__ __device__ unsigned int optixGetClusterId();

static __forceline__ __device__ unsigned int optixHitObjectGetClusterId();

static __forceline__ __device__ unsigned int optixHitObjectGetPrimitiveIndex();

static __forceline__ __device__ unsigned int optixGetSbtGASIndex();

static __forceline__ __device__ unsigned int optixHitObjectGetSbtGASIndex();

static __forceline__ __device__ unsigned int optixGetInstanceId();

static __forceline__ __device__ unsigned int optixHitObjectGetInstanceId();

---

static __forceinline__ __device__ unsigned int optixGetInstanceIndex();

static __forceline__ __device__ unsigned int optixHitObjectGetInstanceIndex();

static __forceline__ __device__ unsigned int optixGetHitKind();

static __forceline__ __device__ unsigned int optixHitObjectGetHitKind();

static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType(unsigned int hitKind);

static __forceline__ __device__ bool optixIsFrontFaceHit(unsigned int hitKind);

static __forceline__ __device__ bool optixIsBackFaceHit(unsigned int hitKind);

static __forceline__ __device__ OptixPrimitiveType optixGetPrimitiveType();

static __forceline__ __device__ bool optixIsFrontFaceHit();

static __forceline__ __device__ bool optixIsBackFaceHit();

static __forceline__ __device__ bool optixIsTriangleHit();

static __forceline__ __device__ bool optixIsTriangleFrontFaceHit();

static __forceline__ __device__ bool optixIsTriangleBackFaceHit();

static __forceline__ __device__ float2 optixGetTriangleBarycentrics();

static __forceline__ __device__ float2 optixHitObjectGetTriangleBarycentrics();

static __forceline__ __device__ float optixGetCurveParameter();

static __forceline__ __device__ float optixHitObjectGetCurveParameter();

static __forceline__ __device__ float2 optixGetRibbonParameters();

static __forceline__ __device__ float2 optixHitObjectGetRibbonParameters();

static __forceline__ __device__ uint3 optixGetLaunchIndex();

static __forceline__ __device__ uint3 optixGetLaunchDimensions();

static __forceline__ __device__ CUdeviceptr optixGetSbtDataPointer();

static __forceline__ __device__ CUdeviceptr optixHitObjectGetSbtDataPointer();

static __forceline__ __device__ void optixThrowException(int exceptionCode);

static __forceline__ __device__ void optixThrowException(int exceptionCode, unsigned int
exceptionDetail0);

static __forceline__ __device__ void optixThrowException(int exceptionCode,
unsigned int exceptionDetail0,
unsigned int exceptionDetail1);

static __forceline__ __device__ void optixThrowException(int exceptionCode,
unsigned int exceptionDetail0,
unsigned int exceptionDetail1,
unsigned int exceptionDetail2);

static __forceline__ __device__ void optixThrowException(int exceptionCode,
unsigned int exceptionDetail0,
unsigned int exceptionDetail1,
unsigned int exceptionDetail2,
unsigned int exceptionDetail3);

---

static __forceinline__ __device__ void optixThrowException(int exceptionCode,
unsigned int exceptionDetail0,
unsigned int exceptionDetail1,
unsigned int exceptionDetail2,
unsigned int exceptionDetail3,
unsigned int exceptionDetail4);

static __forceline__ __device__ void optixThrowException(int exceptionCode,
unsigned int exceptionDetail0,
unsigned int exceptionDetail1,
unsigned int exceptionDetail2,
unsigned int exceptionDetail3,
unsigned int exceptionDetail4,
unsigned int exceptionDetail5);

static __forceline__ __device__ void optixThrowException(int exceptionCode,
unsigned int exceptionDetail0,
unsigned int exceptionDetail1,
unsigned int exceptionDetail2,
unsigned int exceptionDetail3,
unsigned int exceptionDetail4,
unsigned int exceptionDetail5,
unsigned int exceptionDetail6);

static __forceline__ __device__ void optixThrowException(int exceptionCode,
unsigned int exceptionDetail0,
unsigned int exceptionDetail1,
unsigned int exceptionDetail2,
unsigned int exceptionDetail3,
unsigned int exceptionDetail4,
unsigned int exceptionDetail5,
unsigned int exceptionDetail6,
unsigned int exceptionDetail7);

static __forceline__ __device__ int optixGetExceptionCode();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_0();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_1();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_2();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_3();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_4();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_5();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_6();

static __forceline__ __device__ unsigned int optixGetExceptionDetail_7();

static __forceline__ __device__ char* optixGetExceptionLineInfo();

template <typename ReturnT, typename... ArgTypes>
static __forceline__ __device__ ReturnT optixDirectCall(unsigned int sbtIndex, ArgTypes... args);

template <typename ReturnT, typename... ArgTypes>
static __forceline__ __device__ ReturnT optixContinuationCall(unsigned int sbtIndex, ArgTypes... args);

static __forceline__ __device__ uint4 optixTexFootprint2D(unsigned long long tex, unsigned int
texInfo, float x, float y, unsigned int* singleMipLevel);

---

2113static__forceinline__ __device__ uint4
2114optixTexFootprint2DLod(unsignedlonglongtex,unsignedinttexInfo,floatx,floaty,floatlevel,bool
coarse,unsignedint* singleMipLevel);
2115
2130static__forceinline__ __device__ uint4optixTexFootprint2DGrad(unsignedlonglongtex,
2131 unsignedinttexInfo,
2132 floatx,
2133 floaty,
2134 floatdPdx_x,
2135 floatdPdx_y,
2136 floatdPdy_x,
2137 floatdPdy_y,
2138 boolcoarse,
2139 unsignedint* singleMipLevel);
2140// end group optix_device_api
2142
2143#define __OPTIX_INCLUDE_INTERNAL_HEADERS__
2144
2145#include "internal/optix_device_impl.h"
2146
2147
2148// If you manually define OPTIX_INCLUDE_COOPERATIVE_VECTOR to override the default behavior, you must
2149// set it to 0 or 1 and not simply define it with no value (which will default it have a value of 0).
2150#ifndef OPTIX_INCLUDE_COOPERATIVE_VECTOR
2151# define OPTIX_INCLUDE_COOPERATIVE_VECTOR_UNSET
2152# define OPTIX_INCLUDE_COOPERATIVE_VECTOR 1
2153#endif
2154
2155#if OPTIX_INCLUDE_COOPERATIVE_VECTOR
2161
2166template<typenameVecTOut>
2167static__forceinline__ __device__ VecTOutoptixCoopVecLoad(CUdeviceptrptr);
2172template<typenameVecTOut,typenameT>
2173static__forceinline__ __device__ VecTOutoptixCoopVecLoad(T* ptr);
2174
2175
2181template<typenameVecT>
2182static__forceinline__ __device__ VecToptixCoopVecExp2(constVecT& vec);
2185template<typenameVecT>
2186static__forceinline__ __device__ VecToptixCoopVecLog2(constVecT& vec);
2189template<typenameVecT>
2190static__forceinline__ __device__ VecToptixCoopVecTanh(constVecT& vec);
2195template<typenameVecTOut,typenameVecTIn>
2196static__forceinline__ __device__ VecTOutoptixCoopVecCvt(constVecTIn& vec);
2199template<typenameVecT>
2200static__forceinline__ __device__ VecToptixCoopVecMin(constVecT& vecA,constVecT& vecB);
2203template<typenameVecT>
2204static__forceinline__ __device__ VecToptixCoopVecMin(constVecT& vecA,typenameVecT::value_type B);
2207template<typenameVecT>
2208static__forceinline__ __device__ VecToptixCoopVecMax(constVecT& vecA,constVecT& vecB);
2211template<typenameVecT>
2212static__forceinline__ __device__ VecToptixCoopVecMax(constVecT& vecA,typenameVecT::value_type B);
2215template<typenameVecT>
2216static__forceinline__ __device__ VecToptixCoopVecMul(constVecT& vecA,constVecT& vecB);
2219template<typenameVecT>
2220static__forceinline__ __device__ VecToptixCoopVecAdd(constVecT& vecA,constVecT& vecB);
2223template<typenameVecT>
2224static__forceinline__ __device__ VecToptixCoopVecSub(constVecT& vecA,constVecT& vecB);
2228template<typenameVecT>
2229static__forceinline__ __device__ VecToptixCoopVecStep(constVecT& vecA,constVecT& vecB);
2232template<typenameVecT>
2233static__forceinline__ __device__ VecToptixCoopVecFFMA(constVecT& vecA,constVecT& vecB,constVecT&
vecC);
2234
2300template<
2301typenameVecTOut,
2302typenameVecTIn,

---

OptixCoopVecElemType inputInterpretation,
OptixCoopVecMatrixLayout matrixLayout,
bool transpose,
unsigned int N,
unsigned int K,
OptixCoopVecElemType matrixElementType,
OptixCoopVecElemType biasElementType>
static __forceinline__ __device__ VecTOut optixCoopVecMatMul(const VecTIn& inputVector,
CUdeviceptr matrix, // 64 byte aligned,
Array of KxN elements
unsigned matrixOffsetInBytes, // 64 byte
aligned
CUdeviceptr bias, // 16 byte aligned, Array
of N elements
unsigned biasOffsetInBytes, // 16 byte
aligned
unsigned rowColumnStrideInBytes = 0);

template <typename VecTOut, typename VecTIn, OptixCoopVecElemType inputInterpretation,
OptixCoopVecMatrixLayout matrixLayout, bool transpose, unsigned int N, unsigned int K, OptixCoopVecElemType
matrixElementType>
static __forceinline__ __device__ VecTOut optixCoopVecMatMul(const VecTIn& inputVector,
CUdeviceptr matrix, // 64 byte aligned,
Array of KxN elements
unsigned matrixOffsetInBytes, // 64 byte aligned
unsigned rowColumnStrideInBytes = 0);

template <typename VecTIn>
static __forceinline__ __device__ void optixCoopVecReduceSumAccumulate(const VecTIn& inputVector,
CUdeviceptr outputVector,
unsigned offsetInBytes);

template <typename VecTA, typename VecTB, OptixCoopVecMatrixLayout matrixLayout =
OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL>
static __forceinline__ __device__ void optixCoopVecOuterProductAccumulate(const VecTA& vecA,
const VecTB& vecB,
CUdeviceptr outputMatrix,
unsigned offsetInBytes,
unsigned
rowColumnStrideInBytes = 0);

template <unsigned int N, unsigned int K, OptixCoopVecElemType elementType, OptixCoopVecMatrixLayout
layout = OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_OPTIMAL, unsigned int rowColumnStrideInBytes = 0>
static __forceinline__ __device__ unsigned int optixCoopVecGetMatrixSize();

template <typename T, unsigned int N>
class OptixCoopVec
{
public:
    static const unsigned int size = N;
    using value_type             = T;

    __forceinline__ __device__ OptixCoopVec() {}
    __forceinline__ __device__ OptixCoopVec(const value_type& val)
    {
        for(unsigned int i = 0; i < size; ++i)
            m_data[i]       = val;
    }
    __forceinline__ __device__ const value_type& operator[](unsigned int index)const { return
m_data[index]; }
    __forceinline__ __device__ value_type& operator[](unsigned int index) { return m_data[index]; }

    __forceinline__ __device__ const value_type* data()const { return m_data; }
    __forceinline__ __device__ value_type* data() { return m_data; }

protected:
    value_type m_data[size];
}

NVIDIA OptiX 9.1 API

---

2426};
2427// end group optix_device_api
2429
2430#include "internal/optix_device_impl_coop_vec.h"
2431
2432#endif// OPTIX_INCLUDE_COOPERATIVE_VECTOR
2433
2434#ifdef OPTIX_INCLUDE_COOPERATIVE_VECTOR_UNSET
2435# undef OPTIX_INCLUDE_COOPERATIVE_VECTOR
2436# undef OPTIX_INCLUDE_COOPERATIVE_VECTOR_UNSET
2437#endif
2438
2439
2440#endif// OPTIX_OPTIX_DEVICE_H

## 8.13 optix_function_table.h File Reference

## Classes

•structOptixFunctionTable

## Macros

•#defineOPTIX_ABI_VERSION118

•#defineOPTIX_CONCATENATE_ABI_VERSION(prefix, macro)OPTIX_CONCATENATE_ABI_
VERSION_IMPL(prefix, macro)

•#defineOPTIX_CONCATENATE_ABI_VERSION_IMPL(prefix, macro) prefix ## _ ## macro

•#defineOPTIX_FUNCTION_TABLE_SYMBOLOPTIX_CONCATENATE_ABI_VERSION(g_
optixFunctionTable,OPTIX_ABI_VERSION)

## Typedefs

•typedef structOptixFunctionTableOptixFunctionTable

## 8.13.1 Detailed Description

Author

OptiX public API header.

NVIDIA Corporation

## 8.13.2 Macro Definition Documentation

## 8.13.2.1 OPTIX_ABI_VERSION

#define OPTIX_ABI_VERSION 118

The OptiX ABI version.

8.14 optix_function_table.h

Go to the documentation of this file.

2* SPDX-FileCopyrightText: Copyright (c) 2019-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

3* SPDX-License-Identifier: LicenseRef-NvidiaProprietary
4*

5* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual

6* property and proprietary rights in and to this material, related

7* documentation and any modifications thereto. Any use, reproduction,

---

8* disclosure or distribution of this material and related documentation
9* without an express license agreement from NVIDIA CORPORATION or
10* its affiliates is strictly prohibited.
11*/
15
16#ifndef OPTIX_OPTIX_FUNCTION_TABLE_H
17#define OPTIX_OPTIX_FUNCTION_TABLE_H
18
20#define OPTIX_ABI_VERSION 118
21
22#ifndef OPTIX_DEFINE_ABI_VERSION_ONLY
23
24#include "optix_types.h"
25
26#if!defined(OPTIX_DONT_INCLUDE_CUDA)
27// If OPTIX_DONT_INCLUDE_CUDA is defined, cuda driver types must be defined through other
28// means before including optix headers.
29#include <cuda.h>
30#endif
31
32#ifdef __cplusplus
33extern"C"{
34#endif
35
38
46typedefstructOptixFunctionTable
47 {
49//@ {
50
52constchar* (*optixGetErrorName)(OptixResultresult);
53
55constchar* (*optixGetErrorString)(OptixResultresult);
56
57//@}
59//@ {
60
62OptixResult(*optixDeviceContextCreate)(CUcontext fromContext,constOptixDeviceContextOptions*
options,OptixDeviceContext* context);
63
65OptixResult(*optixDeviceContextDestroy)(OptixDeviceContextcontext);
66
68OptixResult(*optixDeviceContextGetProperty)(OptixDeviceContextcontext,OptixDeviceProperty
property,void* value,size_tsizeInBytes);
69
71OptixResult(*optixDeviceContextSetLogCallback)(OptixDeviceContextcontext,
72 OptixLogCallbackcallbackFunction,
73 void* callbackData,
74 unsignedintcallbackLevel);
75
77OptixResult(*optixDeviceContextSetCacheEnabled)(OptixDeviceContextcontext,intenabled);
78
80OptixResult(*optixDeviceContextSetCacheLocation)(OptixDeviceContextcontext,constchar* location);
81
83OptixResult(*optixDeviceContextSetCacheDatabaseSizes)(OptixDeviceContextcontext,size_t
lowWaterMark,size_thighWaterMark);
84
86OptixResult(*optixDeviceContextGetCacheEnabled)(OptixDeviceContextcontext,int* enabled);
87
89OptixResult(*optixDeviceContextGetCacheLocation)(OptixDeviceContextcontext,char* location,size_t
locationSize);
90
92OptixResult(*optixDeviceContextGetCacheDatabaseSizes)(OptixDeviceContextcontext,size_t*
lowWaterMark,size_t* highWaterMark);
93
94//@}
96//@ {
97

---

OptixResult (*optixModuleCreate)(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const char* input,
size_t inputSize,
char* logString,
size_t* logStringSize,
OptixModule* module);

OptixResult (*optixModuleCreateWithTasks)(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const char* input,
size_t inputSize,
char* logString,
size_t* logStringSize,
OptixModule* module,
OptixTask* firstTask);

OptixResult (*optixModuleGetCompilationState)(OptixModule module, OptixModuleCompileState* state);

OptixResult (*optixModuleCancelCreation)(OptixModule module, OptixCreationFlags flags);

OptixResult (*optixStub)(void);

OptixResult (*optixDeviceContextCancelCreations)(OptixDeviceContext context, OptixCreationFlags
flags);

OptixResult (*optixModuleDestroy)(OptixModule module);

OptixResult(*optixBuiltinISModuleGet)(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const OptixBuiltinISOptions* builtinISOptions,
OptixModule* builtinModule);

//@ }
//@ {

OptixResult (*optixTaskExecute)(OptixTask task,
OptixTask* additionalTasks,
unsigned int maxNumAdditionalTasks,
unsigned int* numAdditionalTasksCreated);

OptixResult (*optixTaskGetSerializationKey)(OptixTask task, void* key, size_t* size);

OptixResult (*optixTaskSerializeOutput)(OptixTask task, void* data, size_t* size);

OptixResult (*optixTaskDeserializeOutput)(OptixTask task,
const void* data,
size_t size,
OptixTask* additionalTasks,
unsigned int maxNumAdditionalTasks,
unsigned int* numAdditionalTasksCreated);

//@ }
//@ {

OptixResult (*optixProgramGroupCreate)(OptixDeviceContext context,
const OptixProgramGroupDesc* programDescriptions,
unsigned int numProgramGroups,
const OptixProgramGroupOptions* options,
char* logString,
size_t* logStringSize,
OptixProgramGroup* programGroups);

OptixResult (*optixProgramGroupDestroy)(OptixProgramGroup programGroup);

---

OptixResult (*optixProgramGroupGetStackSize)(OptixProgramGroup programGroup, OptixStackSizes*
stackSizes, OptixPipeline pipeline);

//@ }
//@ {

OptixResult (*optixPipelineCreate)(OptixDeviceContext context,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const OptixPipelineLinkOptions* pipelineLinkOptions,
const OptixProgramGroup* programGroups,
unsigned int numProgramGroups,
char* logString,
size_t* logStringSize,
OptixPipeline* pipeline);

OptixResult (*optixPipelineDestroy)(OptixPipeline pipeline);

OptixResult (*optixPipelineSetStackSizeFromCallDepths)(OptixPipeline pipeline,
unsigned int maxTraceDepth,
unsigned int maxContinuationCallableDepth,
unsigned int maxDirectCallableDepthFromState,
unsigned int maxDirectCallableDepthFromTraversal,
unsigned int maxTraversableGraphDepth);

OptixResult (*optixPipelineSetStackSize)(OptixPipeline pipeline,
unsigned int directCallableStackSizeFromTraversal,
unsigned int directCallableStackSizeFromState,
unsigned int continuationStackSize,
unsigned int maxTraversableGraphDepth);

OptixResult (*optixPipelineSymbolMemcpyAsync)(OptixPipeline pipeline,
const char* name,
void* mem,
size_t sizeInBytes,
size_t offsetInBytes,
OptixPipelineSymbolMemcpyKind kind,
CUstream stream);

//@ }
//@ {

OptixResult (*optixAccelComputeMemoryUsage)(OptixDeviceContext context,
const OptixAccelBuildOptions* accelOptions,
const OptixBuildInput* buildInputs,
unsigned int numBuildInputs,
OptixAccelBufferSizes* bufferSizes);

OptixResult (*optixAccelBuild)(OptixDeviceContext context,
CUstream stream,
const OptixAccelBuildOptions* accelOptions,
const OptixBuildInput* buildInputs,
unsigned int numBuildInputs,
CUdeviceptr tempBuffer,
size_t tempBufferSizeInBytes,
CUdeviceptr outputBuffer,
size_t outputBufferSizeInBytes,
OptixTraversableHandle* outputHandle,
const OptixAccelEmitDesc* emittedProperties,
unsigned int numEmittedProperties);

OptixResult (*optixAccelGetRelocationInfo)(OptixDeviceContext context, OptixTraversableHandle
handle, OptixRelocationInfo* info);

OptixResult (*optixCheckRelocationCompatibility)(OptixDeviceContext context,
const OptixRelocationInfo* info,

---

255
256
258 0
259
260
261
262
263
264
265
266
267
269 0
270
271
272
273
274
275
276 0
277
278
279
280
282 0
283
284
285
286
288 0
context,
289
buildInput
290
bufferSize
291
293 0
294
295
296
297
299 0
300
301
302
304 0
305
306
307
308
target0pa
309
310 0
311 0
312
314 0
315
316
317
318
320 0
321
322
323
324
325
326

OptixResult (*optixAccelRelocate)(OptixDeviceContext context,
CUstream stream,
const OptixRelocationInfo* info,
const OptixRelocateInput* relocateInputs,
size_t numRelocateInputs,
CUdeviceptr targetAccel,
size_t targetAccelSizeInBytes,
OptixTraversableHandle* targetHandle);

| OptixResult (*optixConvertPointerToTraversableHandle)(OptixDeviceContext | onDevice, |
| --- | --- |
| CUdeviceptr | pointer, |
| OptixTraversableType | traversableType, |
| OptixTraversableHandle* | traversableHandle); |

OptixResult (*optixAccelCompact)(OptixDeviceContext context,
CUstream stream,
OptixTraversableHandle inputHandle,
CUdeviceptr outputBuffer,
size_t outputBufferSizeInBytes,
OptixTraversableHandle* outputHandle);

OptixResult (*optixAccelEmitProperty)(OptixDeviceContext context,
    CUstream stream,
    OptixTraversableHandle handle,
    const OptixAccelEmitDesc* emittedProperty);

288 OptixResult (*optixOpacityMicromapArrayComputeMemoryUsage)(OptixDeviceContext

const OptixOpacityMicromapArrayBuildInput*

OptixMicromapBufferSizes*

| 293 | OptixResult (*optixOpacityMicromapArrayBuild)(OptixDeviceContext | context, |
| --- | --- | --- |
| 294 | CUstream | stream, |
| 295 | const OptixOpacityMicromapArrayBuildInput* | buildInput, |
| 296 | const OptixMicromapBuffers* | buffers); |

OptixResult (*optixOpacityMicromapArrayGetRelocationInfo)(OptixDeviceContext context,
CUdeviceptr opacityMicromapArray,
OptixRelocationInfo* info);

targetOpacityMicromapArraySizeInBytes);

314 OptixResult (*optixClusterAccelCo

315

316

317

uteMemoryUsage)(OptixDeviceContext context,
OptixClusterAccelBuildMode buildMode,
const OptixClusterAccelBuildInput* buildInput,
OptixAccelBufferSizes* bufferSizes);

| 320 | OptixResult (*optixClusterAccelBuild)(OptixDeviceContext context, |  |
| --- | --- | --- |
| 321 | CUstream | stream, |
| 322 | const OptixClusterAccelBuildModeDesc* | buildModeDesc, |
| 323 | const OptixClusterAccelBuildInput* | buildInput, |
| 324 | CUdeviceptr | argsArray, |
| 325 | CUdeviceptr | argsCount, |
| 326 | unsigned int | argsStrideInBytes); |

---

//@ }
//@ {

OptixResult (*optixSbtRecordPackHeader)(OptixProgramGroup programGroup, void*
sbtRecordHeaderHostPointer);

OptixResult (*optixLaunch)(OptixPipeline pipeline,
CUstream stream,
CUdeviceptr pipelineParams,
size_t pipelineParamsSize,
const OptixShaderBindingTable* sbt,
unsigned int width,
unsigned int height,
unsigned int depth);

//@ }
//@ {

OptixResult (*optixCoopVecMatrixConvert)(OptixDeviceContext context,
CUstream stream,
unsigned int numNetworks,
const OptixNetworkDescription* inputNetworkDescription,
CUdeviceptr inputNetworks,
size_t inputNetworkStrideInBytes,
const OptixNetworkDescription* outputNetworkDescription,
CUdeviceptr outputNetworks,
size_t outputNetworkStrideInBytes);

OptixResult (*optixCoopVecMatrixComputeSize)(OptixDeviceContext context,
unsigned int N,
unsigned int K,
OptixCoopVecElemType elementType,
OptixCoopVecMatrixLayout layout,
size_t rowColumnStrideInBytes,
size_t* sizeInBytes);

//@ }
//@ {

OptixResult (*optixDenoiserCreate)(OptixDeviceContext context, OptixDenoiserModelKind modelKind,
const OptixDenoiserOptions* options, OptixDenoiser* returnHandle);

OptixResult (*optixDenoiserDestroy)(OptixDenoiser handle);

OptixResult (*optixDenoiserComputeMemoryResources)(const OptixDenoiser handle,
unsigned int maximumInputWidth,
unsigned int maximumInputHeight,
OptixDenoiserSizes* returnSizes);

OptixResult (*optixDenoiserSetup)(OptixDenoiser denoiser,
CUstream stream,
unsigned int inputWidth,
unsigned int inputHeight,
CUdeviceptr state,
size_t stateSizeInBytes,
CUdeviceptr scratch,
size_t scratchSizeInBytes);

OptixResult (*optixDenoiserInvoke)(OptixDenoiser denoiser,
CUstream stream,
const OptixDenoiserParams* params,
CUdeviceptr denoiserState,
size_t denoiserStateSizeInBytes,
const OptixDenoiserGuideLayer * guideLayer,
const OptixDenoiserLayer * layers,
unsigned int numLayers,

---

404unsignedintinputOffsetX,
405unsignedintinputOffsetY,
406CUdeviceptrscratch,
407size_tscratchSizeInBytes);
408
410OptixResult(*optixDenoiserComputeIntensity)(OptixDenoiserhandle,
411 CUstream stream,
412constOptixImage2D* inputImage,
413CUdeviceptroutputIntensity,
414CUdeviceptrscratch,
415size_tscratchSizeInBytes);
416
418OptixResult(*optixDenoiserComputeAverageColor)(OptixDenoiserhandle,
419 CUstream stream,
420 constOptixImage2D* inputImage,
421 CUdeviceptroutputAverageColor,
422 CUdeviceptrscratch,
423 size_tscratchSizeInBytes);
424
426OptixResult(*optixDenoiserCreateWithUserModel)(OptixDeviceContextcontext,constvoid* data,size_t
dataSizeInBytes,OptixDenoiser* returnHandle);
427//@}
428
429}OptixFunctionTable;
430
431// define global function table variable with ABI specific name.
432#define OPTIX_CONCATENATE_ABI_VERSION(prefix, macro) OPTIX_CONCATENATE_ABI_VERSION_IMPL(prefix, macro)
433#define OPTIX_CONCATENATE_ABI_VERSION_IMPL(prefix, macro) prefix ## _ ## macro
434#define OPTIX_FUNCTION_TABLE_SYMBOL OPTIX_CONCATENATE_ABI_VERSION(g_optixFunctionTable,
OPTIX_ABI_VERSION)
435// end group optix_function_table
437
438#ifdef __cplusplus
439}
440#endif
441
442#endif/* OPTIX_DEFINE_ABI_VERSION_ONLY */
443
444#endif/* OPTIX_OPTIX_FUNCTION_TABLE_H */

## 8.15 optix_function_table_definition.h File Reference

## Variables

•OptixFunctionTableOPTIX_FUNCTION_TABLE_SYMBOL

## 8.15.1 Detailed Description

OptiX public API header.

Author

NVIDIA Corporation

8.16 optix_function_table_definition.h

Go to the documentation of this file.
1/*

2* SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
3* SPDX-License-Identifier: BSD-3-Clause

5* Redistribution and use in source and binary forms, with or without

6* modification, are permitted provided that the following conditions are met:
7*

---

8* 1. Redistributions of source code must retain the above copyright notice, this
9* list of conditions and the following disclaimer.
10*
11* 2. Redistributions in binary form must reproduce the above copyright notice,
12* this list of conditions and the following disclaimer in the documentation
13* and/or other materials provided with the distribution.
14*
15* 3. Neither the name of the copyright holder nor the names of its
16* contributors may be used to endorse or promote products derived from
17* this software without specific prior written permission.
18*
19* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
20* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
21* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
22* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
23* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
24* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
25* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
26* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
27* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
28* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
29*/
30
34
35#ifndef OPTIX_OPTIX_FUNCTION_TABLE_DEFINITION_H
36#define OPTIX_OPTIX_FUNCTION_TABLE_DEFINITION_H
37
38#include "optix_function_table.h"
39
40#ifdef __cplusplus
41extern"C"{
42#endif
43
51OptixFunctionTableOPTIX_FUNCTION_TABLE_SYMBOL;
52// end group optix_function_table
54
55#ifdef __cplusplus
56}
57#endif
58
59#endif// OPTIX_OPTIX_FUNCTION_TABLE_DEFINITION_H

## 8.17 optix_host.h File Reference

## Macros

•#defineOPTIXAPI

## Functions

•OPTIXAPIconst char *∗* optixGetErrorName(OptixResultresult)

•OPTIXAPIconst char *∗* optixGetErrorString(OptixResultresult)

•OPTIXAPIOptixResultoptixDeviceContextCreate(CUcontext fromContext, const
OptixDeviceContextOptions *∗*options,OptixDeviceContext *∗*context)

•OPTIXAPIOptixResultoptixDeviceContextDestroy(OptixDeviceContextcontext)

•OPTIXAPIOptixResultoptixDeviceContextGetProperty(OptixDeviceContextcontext,
OptixDevicePropertyproperty, void *∗*value, size_t sizeInBytes)

•OPTIXAPIOptixResultoptixDeviceContextSetLogCallback(OptixDeviceContextcontext,
OptixLogCallbackcallbackFunction, void *∗*callbackData, unsigned int callbackLevel)

•OPTIXAPIOptixResultoptixDeviceContextSetCacheEnabled(OptixDeviceContextcontext, int
enabled)

•OPTIXAPIOptixResultoptixDeviceContextSetCacheLocation(OptixDeviceContextcontext,
const char *∗*location)

---

• OPTIXAPI OptixResult optixDeviceContextSetCacheDatabaseSizes (OptixDeviceContext context, size_t lowWaterMark, size_t highWaterMark)
• OPTIXAPI OptixResult optixDeviceContextGetCacheEnabled (OptixDeviceContext context, int *enabled)
• OPTIXAPI OptixResult optixDeviceContextGetCacheLocation (OptixDeviceContext context, char *location, size_t locationSize)
• OPTIXAPI OptixResult optixDeviceContextGetCacheDatabaseSizes (OptixDeviceContext context, size_t *lowWaterMark, size_t *highWaterMark)
• OPTIXAPI OptixResult optixPipelineCreate (OptixDeviceContext context, const OptixPipelineCompileOptions *pipelineCompileOptions, const OptixPipelineLinkOptions *pipelineLinkOptions, const OptixProgramGroup *programGroups, unsigned int numProgramGroups, char *logString, size_t *logStringSize, OptixPipeline *pipeline)
• OPTIXAPI OptixResult optixPipelineDestroy (OptixPipeline pipeline)
• OPTIXAPI OptixResult optixPipelineSetStackSizeFromCallDepths (OptixPipeline pipeline, unsigned int maxTraceDepth, unsigned int maxContinuationCallableDepth, unsigned int maxDirectCallableDepthFromState, unsigned int maxDirectCallableDepthFromTraversal, unsigned int maxTraversableGraphDepth)
• OPTIXAPI OptixResult optixPipelineSetStackSize (OptixPipeline pipeline, unsigned int directCallableStackSizeFromTraversal, unsigned int directCallableStackSizeFromState, unsigned int continuationStackSize, unsigned int maxTraversableGraphDepth)
• OPTIXAPI OptixResult optixPipelineSymbolMemcpyAsync (OptixPipeline pipeline, const char *name, void *mem, size_t sizeInBytes, size_t offsetInBytes, OptixPipelineSymbolMemcpyKind kind, CUstream stream)
• OPTIXAPI OptixResult optixModuleCreate (OptixDeviceContext context, const OptixModuleCompileOptions *moduleCompileOptions, const OptixPipelineCompileOptions *pipelineCompileOptions, const char *input, size_t inputSize, char *logString, size_t *logStringSize, OptixModule *module)
• OPTIXAPI OptixResult optixModuleCreateWithTasks (OptixDeviceContext context, const OptixModuleCompileOptions *moduleCompileOptions, const OptixPipelineCompileOptions *pipelineCompileOptions, const char *input, size_t inputSize, char *logString, size_t *logStringSize, OptixModule *module, OptixTask *firstTask)
• OPTIXAPI OptixResult optixModuleGetCompilationState (OptixModule module, OptixModuleCompileState *state)
• OPTIXAPI OptixResult optixModuleCancelCreation (OptixModule module, OptixCreationFlags flags)
• OPTIXAPI OptixResult optixDeviceContextCancelCreations (OptixDeviceContext context, OptixCreationFlags flags)
• OPTIXAPI OptixResult optixModuleDestroy (OptixModule module)
• OPTIXAPI OptixResult optixBuiltinISModuleGet (OptixDeviceContext context, const OptixModuleCompileOptions *moduleCompileOptions, const OptixPipelineCompileOptions *pipelineCompileOptions, const OptixBuiltinISOptions *builtinISOptions, OptixModule *builtinModule)
• OPTIXAPI OptixResult optixTaskExecute (OptixTask task, OptixTask *additionalTasks, unsigned int maxNumAdditionalTasks, unsigned int *numAdditionalTasksCreated)
• OPTIXAPI OptixResult optixTaskGetSerializationKey (OptixTask task, void *key, size_t *size)
• OPTIXAPI OptixResult optixTaskSerializeOutput (OptixTask task, void *data, size_t *size)
• OPTIXAPI OptixResult optixTaskDeserializeOutput (OptixTask task, const void *data, size_t size, OptixTask *additionalTasks, unsigned int maxNumAdditionalTasks, unsigned int *numAdditionalTasksCreated)
• OPTIXAPI OptixResult optixProgramGroupGetStackSize (OptixProgramGroup programGroup, OptixStackSizes *stackSizes, OptixPipeline pipeline)

---

• OPTIXAPI OptixResult optixProgramGroupCreate (OptixDeviceContext context, const OptixProgramGroupDesc *programDescriptions, unsigned int numProgramGroups, const OptixProgramGroupOptions *options, char *logString, size_t *logStringSize, OptixProgramGroup *programGroups)
• OPTIXAPI OptixResult optixProgramGroupDestroy (OptixProgramGroup programGroup)
• OPTIXAPI OptixResult optixSbtRecordPackHeader (OptixProgramGroup programGroup, void *sbtRecordHeaderHostPointer)
• OPTIXAPI OptixResult optixLaunch (OptixPipeline pipeline, CUstream stream, CUdeviceptr pipelineParams, size_t pipelineParamsSize, const OptixShaderBindingTable *sbt, unsigned int width, unsigned int height, unsigned int depth)
• OPTIXAPI OptixResult optixAccelComputeMemoryUsage (OptixDeviceContext context, const OptixAccelBuildOptions *accelOptions, const OptixBuildInput *buildInputs, unsigned int numBuildInputs, OptixAccelBufferSizes *bufferSizes)
• OPTIXAPI OptixResult optixAccelBuild (OptixDeviceContext context, CUstream stream, const OptixAccelBuildOptions *accelOptions, const OptixBuildInput *buildInputs, unsigned int numBuildInputs, CUdeviceptr tempBuffer, size_t tempBufferSizeInBytes, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle, const OptixAccelEmitDesc *emittedProperties, unsigned int numEmittedProperties)
• OPTIXAPI OptixResult optixAccelGetRelocationInfo (OptixDeviceContext context, OptixTraversableHandle handle, OptixRelocationInfo *info)
• OPTIXAPI OptixResult optixCheckRelocationCompatibility (OptixDeviceContext context, const OptixRelocationInfo *info, int *compatible)
• OPTIXAPI OptixResult optixAccelRelocate (OptixDeviceContext context, CUstream stream, const OptixRelocationInfo *info, const OptixRelocateInput *relocateInputs, size_t numRelocateInputs, CUdeviceptr targetAccel, size_t targetAccelSizeInBytes, OptixTraversableHandle *targetHandle)
• OPTIXAPI OptixResult optixAccelCompact (OptixDeviceContext context, CUstream stream, OptixTraversableHandle inputHandle, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle *outputHandle)
• OPTIXAPI OptixResult optixAccelEmitProperty (OptixDeviceContext context, CUstream stream, OptixTraversableHandle handle, const OptixAccelEmitDesc *emittedProperty)
• OPTIXAPI OptixResult optixConvertPointerToTraversableHandle (OptixDeviceContext onDevice, CUdeviceptr pointer, OptixTraversableType traversableType, OptixTraversableHandle *traversableHandle)
• OPTIXAPI OptixResult optixOpacityMicromapArrayComputeMemoryUsage (OptixDeviceContext context, const OptixOpacityMicromapArrayBuildInput *buildInput, OptixMicromapBufferSizes *bufferSizes)
• OPTIXAPI OptixResult optixOpacityMicromapArrayBuild (OptixDeviceContext context, CUstream stream, const OptixOpacityMicromapArrayBuildInput *buildInput, const OptixMicromapBuffers *buffers)
• OPTIXAPI OptixResult optixOpacityMicromapArrayGetRelocationInfo (OptixDeviceContext context, CUdeviceptr opacityMicromapArray, OptixRelocationInfo *info)
• OPTIXAPI OptixResult optixOpacityMicromapArrayRelocate (OptixDeviceContext context, CUstream stream, const OptixRelocationInfo *info, CUdeviceptr targetOpacityMicromapArray, size_t targetOpacityMicromapArraySizeInBytes)
• OPTIXAPI OptixResult optixClusterAccelComputeMemoryUsage (OptixDeviceContext context, OptixClusterAccelBuildMode buildMode, const OptixClusterAccelBuildInput *buildInput, OptixAccelBufferSizes *bufferSizes)
• OPTIXAPI OptixResult optixClusterAccelBuild (OptixDeviceContext context, CUstream stream, const OptixClusterAccelBuildModeDesc *buildModeDesc, const OptixClusterAccelBuildInput *buildInput, CUdeviceptr argsArray, CUdeviceptr argsCount, unsigned int argsStrideInBytes)
• OPTIXAPI OptixResult optixCoopVecMatrixConvert (OptixDeviceContext context, CUstream stream, unsigned int numNetworks, const OptixNetworkDescription *inputNetworkDescription,

---

CUdeviceptrinputNetworks, size_t inputNetworkStrideInBytes, constOptixNetworkDescription
*∗*outputNetworkDescription,CUdeviceptroutputNetworks, size_t outputNetworkStrideInBytes)

•OPTIXAPIOptixResultoptixCoopVecMatrixComputeSize(OptixDeviceContextcontext,
unsigned int N, unsigned int K,OptixCoopVecElemTypeelementType,
OptixCoopVecMatrixLayoutlayout, size_t rowColumnStrideInBytes, size_t *∗*sizeInBytes)

•OPTIXAPIOptixResultoptixDenoiserCreate(OptixDeviceContextcontext,
OptixDenoiserModelKindmodelKind, constOptixDenoiserOptions *∗*options,OptixDenoiser
*∗*denoiser)

•OPTIXAPIOptixResultoptixDenoiserCreateWithUserModel(OptixDeviceContextcontext, const
void *∗*userData, size_t userDataSizeInBytes,OptixDenoiser *∗*denoiser)

•OPTIXAPIOptixResultoptixDenoiserDestroy(OptixDenoiserdenoiser)

•OPTIXAPIOptixResultoptixDenoiserComputeMemoryResources(constOptixDenoiserdenoiser,
unsigned int outputWidth, unsigned int outputHeight,OptixDenoiserSizes *∗*returnSizes)

•OPTIXAPIOptixResultoptixDenoiserSetup(OptixDenoiserdenoiser, CUstream stream,
unsigned int inputWidth, unsigned int inputHeight,CUdeviceptrdenoiserState, size_t
denoiserStateSizeInBytes,CUdeviceptrscratch, size_t scratchSizeInBytes)

•OPTIXAPIOptixResultoptixDenoiserInvoke(OptixDenoiserdenoiser, CUstream stream, const
OptixDenoiserParams *∗*params,CUdeviceptrdenoiserState, size_t denoiserStateSizeInBytes,
constOptixDenoiserGuideLayer *∗*guideLayer, constOptixDenoiserLayer *∗*layers, unsigned int
numLayers, unsigned int inputOffsetX, unsigned int inputOffsetY,CUdeviceptrscratch, size_t
scratchSizeInBytes)

•OPTIXAPIOptixResultoptixDenoiserComputeIntensity(OptixDenoiserdenoiser, CUstream
stream, constOptixImage2D *∗*inputImage,CUdeviceptroutputIntensity,CUdeviceptrscratch,
size_t scratchSizeInBytes)

•OPTIXAPIOptixResultoptixDenoiserComputeAverageColor(OptixDenoiserdenoiser,
CUstream stream, constOptixImage2D *∗*inputImage,CUdeviceptroutputAverageColor,
CUdeviceptrscratch, size_t scratchSizeInBytes)

## 8.17.1 Detailed Description

## OptiX public API header.

## Author

## NVIDIA Corporation

OptiX host include file – includes the host api if compiling host code. For the math library routines
include optix_math.h

## 8.17.2 Macro Definition Documentation

## 8.17.2.1 OPTIXAPI

#define OPTIXAPI

Mixing multiple SDKs in a single application will result in symbol collisions. To enable different
compilation units to use different SDKs, use OPTIX_ENABLE_SDK_MIXING.

8.18 optix_host.h

Go to the documentation of this file.

1/*

2* SPDX-FileCopyrightText: Copyright (c) 2010-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

3* SPDX-License-Identifier: LicenseRef-NvidiaProprietary
4*

---

5* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
6* property and proprietary rights in and to this material, related
7* documentation and any modifications thereto. Any use, reproduction,
8* disclosure or distribution of this material and related documentation
9* without an express license agreement from NVIDIA CORPORATION or
10* its affiliates is strictly prohibited.
11*/
18
19#ifndef OPTIX_OPTIX_HOST_H
20#define OPTIX_OPTIX_HOST_H
21
24#ifndef OPTIXAPI
25# ifdef OPTIX_ENABLE_SDK_MIXING
26# define OPTIXAPI static
27# else// OPTIX_ENABLE_SDK_MIXING
28# ifdef __cplusplus
29# define OPTIXAPI extern "C"
30# else// __cplusplus
31# define OPTIXAPI
32# endif// __cplusplus
33# endif// OPTIX_ENABLE_SDK_MIXING
34#endif// OPTIXAPI
35
36#include "optix_types.h"
37#if!defined(OPTIX_DONT_INCLUDE_CUDA)
38// If OPTIX_DONT_INCLUDE_CUDA is defined, cuda driver types must be defined through other
39// means before including optix headers.
40#include <cuda.h>
41#endif
42
43#ifdef NV_MODULE_OPTIX
44// This is a mechanism to include <g_nvconfig.h> in driver builds only and translate any nvconfig macro to
a custom OPTIX-specific macro, that can also be used in SDK builds/installs
45#include <exp/misc/optix_nvconfig_translate.h>// includes <g_nvconfig.h>
46#endif// NV_MODULE_OPTIX
47
48
51
55
66OPTIXAPIconstchar*optixGetErrorName(OptixResultresult);
67
78OPTIXAPIconstchar*optixGetErrorString(OptixResultresult);
79
84
103OPTIXAPIOptixResultoptixDeviceContextCreate(CUcontext fromContext,constOptixDeviceContextOptions*
options,OptixDeviceContext* context);
104
113OPTIXAPIOptixResultoptixDeviceContextDestroy(OptixDeviceContextcontext);
114
121OPTIXAPIOptixResultoptixDeviceContextGetProperty(OptixDeviceContextcontext,OptixDeviceProperty
property,void* value,size_tsizeInBytes);
122
136OPTIXAPIOptixResultoptixDeviceContextSetLogCallback(OptixDeviceContextcontext,
137 OptixLogCallbackcallbackFunction,
138 void* callbackData,
139 unsignedintcallbackLevel);
140
159OPTIXAPIOptixResultoptixDeviceContextSetCacheEnabled(OptixDeviceContextcontext,intenabled);
160
181OPTIXAPIOptixResultoptixDeviceContextSetCacheLocation(OptixDeviceContextcontext,constchar*
location);
182
210OPTIXAPIOptixResultoptixDeviceContextSetCacheDatabaseSizes(OptixDeviceContextcontext,size_t
lowWaterMark,size_thighWaterMark);
211
216OPTIXAPIOptixResultoptixDeviceContextGetCacheEnabled(OptixDeviceContextcontext,int* enabled);
223OPTIXAPIOptixResultoptixDeviceContextGetCacheLocation(OptixDeviceContextcontext,char* location, size_t locationSize;

OPTIXAPI OptixResult optixDeviceContextGetCacheDatabaseSizes(OptixDeviceContext context, size_t*
lowWaterMark, size_t* highWaterMark);

OPTIXAPI OptixResult optixPipelineCreate(OptixDeviceContext context,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const OptixPipelineLinkOptions* pipelineLinkOptions,
const OptixProgramGroup* programGroups,
unsigned int numProgramGroups,
char* logString,
size_t* logStringSize,
OptixPipeline* pipeline);

OPTIXAPI OptixResult optixPipelineDestroy(OptixPipeline pipeline);

OPTIXAPI OptixResult optixPipelineSetStackSizeFromCallDepths(OptixPipeline pipeline,
unsigned int maxTraceDepth,
unsigned int maxContinuationCallableDepth,
unsigned int maxDirectCallableDepthFromState,
unsigned int maxDirectCallableDepthFromTraversal,
unsigned int maxTraversableGraphDepth);

OPTIXAPI OptixResult optixPipelineSetStackSize(OptixPipeline pipeline,
unsigned int directCallableStackSizeFromTraversal,
unsigned int directCallableStackSizeFromState,
unsigned int continuationStackSize,
unsigned int maxTraversableGraphDepth);

OPTIXAPI OptixResult optixPipelineSymbolMemcpyAsync(OptixPipeline pipeline,
const char* name,
void* mem,
size_t sizeInBytes,
size_t offsetInBytes,
OptixPipelineSymbolMemcpyKind kind,
CUstream stream);

OPTIXAPI OptixResult optixModuleCreate(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const char* input,
size_t inputSize,
char* logString,
size_t* logStringSize,
OptixModule* module);

OPTIXAPI OptixResult optixModuleCreateWithTasks(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const char* input,
size_t inputSize,
char* logString,
size_t* logStringSize,
OptixModule* module,
OptixTask* firstTask);

OPTIXAPI OptixResult optixModuleGetCompilationState(OptixModule module, OptixModuleCompileState* state);

OPTIXAPI OptixResult optixModuleCancelCreation(OptixModule module, OptixCreationFlags flags);

OPTIXAPI OptixResult optixDeviceContextCancelCreations(OptixDeviceContext context, OptixCreationFlags
flags);

OPTIXAPI OptixResult optixModuleDestroy(OptixModule module);

---

OPTIXAPI OptixResult optixBuiltinISModuleGet(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const OptixBuiltinISOptions* builtinISOptions,
OptixModule* builtinModule);

OPTIXAPI OptixResult optixTaskExecute(OptixTask task,
OptixTask* additionalTasks,
unsigned int maxNumAdditionalTasks,
unsigned int* numAdditionalTasksCreated);

OPTIXAPI OptixResult optixTaskGetSerializationKey(OptixTask task, void* key, size_t* size);

OPTIXAPI OptixResult optixTaskSerializeOutput(OptixTask task, void* data, size_t* size);

OPTIXAPI OptixResult optixTaskDeserializeOutput(OptixTask task,
const void* data,
size_t size,
OptixTask* additionalTasks,
unsigned int maxNumAdditionalTasks,
unsigned int* numAdditionalTasksCreated);

OPTIXAPI OptixResult optixProgramGroupGetStackSize(OptixProgramGroup programGroup, OptixStackSizes*
stackSizes, OptixPipeline pipeline);

OPTIXAPI OptixResult optixProgramGroupCreate(OptixDeviceContext context,
const OptixProgramGroupDesc* programDescriptions,
unsigned int numProgramGroups,
const OptixProgramGroupOptions* options,
char* logString,
size_t* logStringSize,
OptixProgramGroup* programGroups);

OPTIXAPI OptixResult optixProgramGroupDestroy(OptixProgramGroup programGroup);

OPTIXAPI OptixResult optixSbtRecordPackHeader(OptixProgramGroup programGroup, void*
sbtRecordHeaderHostPointer);

OPTIXAPI OptixResult optixLaunch(OptixPipeline pipeline,
CUstream stream,
CUdeviceptr pipelineParams,
size_t pipelineParamsSize,
const OptixShaderBindingTable* sbt,
unsigned int width,
unsigned int height,
unsigned int depth);

OPTIXAPI OptixResult optixAccelComputeMemoryUsage(OptixDeviceContext context,
const OptixAccelBuildOptions* accelOptions,
const OptixBuildInput* buildInputs,
unsigned int numBuildInputs,
OptixAccelBufferSizes* bufferSizes);

OPTIXAPI OptixResult optixAccelBuild(OptixDeviceContext context,
CUstream stream,
const OptixAccelBuildOptions* accelOptions,
const OptixBuildInput* buildInputs,
unsigned int numBuildInputs,
CUdeviceptr tempBuffer,
size_t tempBufferSizeInBytes,
CUdeviceptr outputBuffer,
size_t outputBufferSizeInBytes,

---

397

|  | OptixTraversableHandle* | outputHandle, |
| --- | --- | --- |
|  | const OptixAccelEmitDesc* | emittedProperties, |
|  | unsigned int | numEmittedProperties); |
| OPTIXAPI OptixResult optixAccelGetRelocationInfo(OptixDeviceContext context, OptixTraversableHandle handle, OptixRelocationInfo* info); |  |  |
| OPTIXAPI OptixResult optixCheckRelocationCompatibility(OptixDeviceContext context, const OptixRelocationInfo* info, int* compatible); |  |  |
| OPTIXAPI OptixResult optixAccelRelocate(OptixDeviceContext context, CUstream stream, |  |  |
|  | const OptixRelocationInfo* info, | const OptixRelocationInput* relocateInputs, |
|  | size_t | numRelocateInputs, |
|  | CUdeviceptr | targetAccel, |
|  | size_t | targetAccelSizeInBytes, |
|  | OptixTraversableHandle* | targetHandle); |
| OPTIXAPI OptixResult optixAccelCompact(OptixDeviceContext context, CUstream stream, |  |  |
|  | optixTraversableHandle inputHandle, | optixDeviceptr outputBuffer, |
|  | size_t | outputBufferSizeInBytes, |
|  | OptixTraversableHandle* outputHandle); |  |
| OPTIXAPI OptixResult optixAccelEmitProperty(OptixDeviceContext context, CUstream stream, |  |  |
|  | optixTraversableHandle handle, | const OptixAccelEmitDesc* emittedProperty); |
| OPTIXAPI OptixResult optixConvertPointerToTraversableHandle(OptixDeviceContext onDevice, CUdeviceptr pointer, |  |  |
|  | optixTraversableType traversableType, | optixTraversableHandle traversableHandle); |
| OPTIXAPI OptixResult optixOpacityMicromapArrayComputeMemoryUsage(OptixDeviceContext context, |  |  |
|  | const OptixOpacityMicromapArrayBuildInput* buildInput, | OptixMicromapBufferSizes* bufferSizes); |
| OPTIXAPI OptixResult optixOpacityMicromapArrayBuild(OptixDeviceContext context, CUstream stream, |  |  |
|  | const OptixOpacityMicromapArrayBuildInput* buildInput, | const OptixMicromapBuffers* buffers); |
| OPTIXAPI OptixResult optixOpacityMicromapArrayGetRelocationInfo(OptixDeviceContext context, CUdeviceptr opacityMicromapArray, |  |  |
|  | OptixRelocationInfo* info); |  |
| OPTIXAPI OptixResult optixOpacityMicromapArrayRelocate(OptixDeviceContext context, CUstream stream, |  |  |
|  | const OptixRelocationInfo* info, | const OptixRelocationInfo* info, |
|  | CUdeviceptr targetOpacityMicromapArray, | size_t |
| targetOpacityMicromapArraySizeInBytes); |  |  |
| OPTIXAPI OptixResult optixClusterAccelComputeMemoryUsage(OptixDeviceContext context, |  |  |
|  | OptixClusterAccelBuildMode buildMode, | const OptixClusterAccelBuildInput* buildInput, |
|  | OptixAccelBufferSizes* bufferSizes); |  |
| OPTIXAPI OptixResult optixClusterAccelBuild(OptixDeviceContext context, |  |  |
|  | CUstream stream, |  |

---

398

| OPTIXAPI OptixResult optixCoopVecMatrixConvert(OptixDeviceContext | context,
CUstream | stream,
unsigned int | numNetworks,
const OptixNetworkDescription* inputNetworkDescription,
CUdeviceptr | inputNetworks,
size_t | inputNetworkStrideInBytes
const OptixNetworkDescription* outputNetworkDescription,
CUdeviceptr | outputNetworks,
size_t | outputNetworkStrideInBytes) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| OPTIXAPI OptixResult optixCoopVecMatrixComputeSize(OptixDeviceContext | context,
unsigned int | N,
unsigned int | K,
OptixCoopVecElemType | elementType,
OptixCoopVecMatrixLayout | layout,
size_t | rowColumnStrideInBytes,
size_t* | sizeInBytes) |
| OPTIXAPI OptixResult optixDenoiserCreate(OptixDeviceContext | context,
OptixDenoiserModelKind | modelKind, |  |  |  |  |  |
| OPTIXAPI OptixResult optixDenoiserCreateWithUserModel(OptixDeviceContext | context,
const void* | userData, |  |  |  |  |  |
| OPTIXAPI OptixResult optixDenoiserDestroy(OptixDenoiser denoiser) |  |  |  |  |  |  |  |
| OPTIXAPI OptixResult optixDenoiserComputeMemoryResources(const OptixDenoiser denoiser, |  |  |  |  |  |  |  |
|  | unsigned int | outputWidth, |  |  |  |  |  |
|  | unsigned int | outputHeight, |  |  |  |  |  |
|  | OptixDenoiserSizes* returnSizes) |  |  |  |  |  |  |
| OPTIXAPI OptixResult optixDenoiserSetup(OptixDenoiser denoiser, |  |  |  |  |  |  |  |
| CUstream | stream, |  |  |  |  |  |  |
| unsigned int | inputWidth, |  |  |  |  |  |  |
| unsigned int | inputHeight, |  |  |  |  |  |  |
| CUdeviceptr | denoiserState, |  |  |  |  |  |  |
| size_t | denoiserStateSizeInBytes, |  |  |  |  |  |  |
| CUdeviceptr | scratch, |  |  |  |  |  |  |
| size_t | scratchSizeInBytes) |  |  |  |  |  |  |
| OPTIXAPI OptixResult optixDenoiserInvoke(OptixDenoiser |  | denoiser, |  |  |  |  |  |
| CUstream | stream, |  |  |  |  |  |  |
| const OptixDenoiserParams* | params, |  |  |  |  |  |  |
| CUdeviceptr | denoiserState, |  |  |  |  |  |  |
| size_t | denoiserStateSizeInBytes, |  |  |  |  |  |  |
| const OptixDenoiserGuideLayer* | guideLayer, |  |  |  |  |  |  |
| const OptixDenoiserLayer* | layers, |  |  |  |  |  |  |
| unsigned int | numLayers, |  |  |  |  |  |  |

---

<u>399</u>

1169unsignedintinputOffsetX,
1170unsignedintinputOffsetY,
1171CUdeviceptrscratch,
1172size_tscratchSizeInBytes);
1173
1197OPTIXAPIOptixResultoptixDenoiserComputeIntensity(OptixDenoiserdenoiser,
1198 CUstream stream,
1199constOptixImage2D* inputImage,
1200CUdeviceptroutputIntensity,
1201CUdeviceptrscratch,
1202size_tscratchSizeInBytes);
1203
1218OPTIXAPIOptixResultoptixDenoiserComputeAverageColor(OptixDenoiserdenoiser,
1219 CUstream stream,
1220 constOptixImage2D* inputImage,
1221 CUdeviceptroutputAverageColor,
1222 CUdeviceptrscratch,
1223 size_tscratchSizeInBytes);
1224
1226
1227#include "optix_function_table.h"
1228
1229#endif// OPTIX_OPTIX_HOST_H

## 8.19 optix_micromap.h File Reference

## Functions

•OPTIX_MICROMAP_INLINE_FUNCvoidoptixMicromapIndexToBaseBarycentrics(unsigned
int micromapTriangleIndex, unsigned int subdivisionLevel, float2 &baseBarycentrics0, float2
&baseBarycentrics1, float2 &baseBarycentrics2)

•OPTIX_MICROMAP_INLINE_FUNCfloat2optixBaseBarycentricsToMicroBarycentrics(float2
baseBarycentrics, float2 microVertexBaseBarycentrics[3])

## 8.19.1 Detailed Description

## Author

NVIDIA Corporation

OptiX micromap helper functions.

## 8.19.2 Function Documentation

OptiX micromap helper functions. Useable on either host or device.

## 8.19.2.1 optixBaseBarycentricsToMicroBarycentrics( )

OPTIX_MICROMAP_INLINE_FUNCfloat2 optixBaseBarycentricsToMicroBarycentrics (
float2 *baseBarycentrics,*
float2 *microVertexBaseBarycentrics[3]*)

Maps barycentrics in the space of the base triangle to barycentrics of a micro triangle. The vertices of
the micro triangle are defined by its barycentrics in the space of the base triangle. These can be queried
for a DMM hit by using optixGetMicroTriangleBarycentricsData().

## 8.19.2.2 optixMicromapIndexToBaseBarycentrics( )

OPTIX_MICROMAP_INLINE_FUNCvoid optixMicromapIndexToBaseBarycentrics (

---

400

unsigned int *micromapTriangleIndex,*

unsigned int *subdivisionLevel,*

float2 & *baseBarycentrics0,*

float2 & *baseBarycentrics1,*

float2 & *baseBarycentrics2*)

Converts a micromap triangle index to the three base-triangle barycentric coordinates of the
micro-triangle vertices in the base triangle. The base triangle is the triangle that the micromap is
applied to. Note that for displaced micro-meshes this function can be used to compute a UV mapping
from sub triangle to base triangle.

## Parameters

| in | micromapTriangleIndex | Index of a micro- or sub triangle within a micromap. |
| --- | --- | --- |
| in | subdivisionLevel | Number of subdivision levels of the micromap or number of subdivision levels being considered (for sub triangles). |
| out | baseBarycentrics0 | Barycentric coordinates in the space of the base triangle of vertex 0 of the micromap triangle. |
| out | baseBarycentrics1 | Barycentric coordinates in the space of the base triangle of vertex 1 of the micromap triangle. |
| out | baseBarycentrics2 | Barycentric coordinates in the space of the base triangle of vertex 2 of the micromap triangle. |

8.20 optix_micromap.h

Go to the documentation of this file.

1/*
2* SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
3* SPDX-License-Identifier: BSD-3-Clause
4*
5* Redistribution and use in source and binary forms, with or without
6* modification, are permitted provided that the following conditions are met:
7*
8* 1. Redistributions of source code must retain the above copyright notice, this
9* list of conditions and the following disclaimer.
10*
11* 2. Redistributions in binary form must reproduce the above copyright notice,
12* this list of conditions and the following disclaimer in the documentation
13* and/or other materials provided with the distribution.
14*
15* 3. Neither the name of the copyright holder nor the names of its
16* contributors may be used to endorse or promote products derived from
17* this software without specific prior written permission.
18*
19* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
20* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
21* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
22* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
23* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
24* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
25* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
26* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
27* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
28* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
29*/
30
39#ifndef OPTIX_OPTIX_MICROMAP_H
40#define OPTIX_OPTIX_MICROMAP_H

---

<u>401</u>

41

42#if!defined(OPTIX_DONT_INCLUDE_CUDA)
43// If OPTIX_DONT_INCLUDE_CUDA is defined, cuda driver type float2 must be defined through other
44// means before including optix headers.

45#include <vector_types.h>
46#endif
47#include "internal/optix_micromap_impl.h"

48
58OPTIX_MICROMAP_INLINE_FUNCvoidoptixMicromapIndexToBaseBarycentrics(unsignedintmicromapTriangleIndex,

59
60
61
62
63 {

unsignedintsubdivisionLevel,
float2& baseBarycentrics0,
float2& baseBarycentrics1,
float2& baseBarycentrics2)

64optix_impl::micro2bary(micromapTriangleIndex, subdivisionLevel, baseBarycentrics0, baseBarycentrics1,

baseBarycentrics2);
65}

66
70OPTIX_MICROMAP_INLINE_FUNCfloat2optixBaseBarycentricsToMicroBarycentrics(float2 baseBarycentrics,
71 float2 microVertexBaseBarycentrics[3])

72 {

73returnoptix_impl::base2micro(baseBarycentrics, microVertexBaseBarycentrics);

74}
75
76#endif// OPTIX_OPTIX_MICROMAP_H

## 8.21 optix_stack_size.h File Reference

## Functions

•OptixResultoptixUtilAccumulateStackSizes(OptixProgramGroupprogramGroup,
OptixStackSizes *∗*stackSizes,OptixPipelinepipeline)

•OptixResultoptixUtilComputeStackSizes(constOptixStackSizes *∗*stackSizes, unsigned int
maxTraceDepth, unsigned int maxCCDepth, unsigned int maxDCDepth, unsigned int
*∗*directCallableStackSizeFromTraversal, unsigned int *∗*directCallableStackSizeFromState,
unsigned int *∗*continuationStackSize)

•OptixResultoptixUtilComputeStackSizesDCSplit(constOptixStackSizes *∗*stackSizes, unsigned
int dssDCFromTraversal, unsigned int dssDCFromState, unsigned int maxTraceDepth, unsigned
int maxCCDepth, unsigned int maxDCDepthFromTraversal, unsigned int
maxDCDepthFromState, unsigned int *∗*directCallableStackSizeFromTraversal, unsigned int
*∗*directCallableStackSizeFromState, unsigned int *∗*continuationStackSize)

•OptixResultoptixUtilComputeStackSizesCssCCTree(constOptixStackSizes *∗*stackSizes,
unsigned int cssCCTree, unsigned int maxTraceDepth, unsigned int maxDCDepth, unsigned int
*∗*directCallableStackSizeFromTraversal, unsigned int *∗*directCallableStackSizeFromState,
unsigned int *∗*continuationStackSize)

•OptixResultoptixUtilComputeStackSizesSimplePathTracer(OptixProgramGroup
programGroupRG,OptixProgramGroupprogramGroupMS1, constOptixProgramGroup
*∗*programGroupCH1, unsigned int programGroupCH1Count,OptixProgramGroup
programGroupMS2, constOptixProgramGroup *∗*programGroupCH2, unsigned int
programGroupCH2Count, unsigned int *∗*directCallableStackSizeFromTraversal, unsigned int
*∗*directCallableStackSizeFromState, unsigned int *∗*continuationStackSize,OptixPipelinepipeline)

## 8.21.1 Detailed Description

OptiX public API header.

Author

NVIDIA Corporation

---

<u>402</u>

8.22 optix_stack_size.h

Go to the documentation of this file.

1/*
2* SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
3* SPDX-License-Identifier: BSD-3-Clause
4*
5* Redistribution and use in source and binary forms, with or without
6* modification, are permitted provided that the following conditions are met:
7*
8* 1. Redistributions of source code must retain the above copyright notice, this
9* list of conditions and the following disclaimer.
10*
11* 2. Redistributions in binary form must reproduce the above copyright notice,
12* this list of conditions and the following disclaimer in the documentation
13* and/or other materials provided with the distribution.
14*
15* 3. Neither the name of the copyright holder nor the names of its
16* contributors may be used to endorse or promote products derived from
17* this software without specific prior written permission.
18*
19* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
20* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
21* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
22* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
23* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
24* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
25* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
26* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
27* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
28* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
29*/
30
34
35#ifndef OPTIX_OPTIX_STACK_SIZE_H
36#define OPTIX_OPTIX_STACK_SIZE_H
37
38#include "optix.h"
39
40#include <algorithm>
41#include <cstring>
42
43#ifdef __cplusplus
44extern"C"{
45#endif
46
56inlineOptixResultoptixUtilAccumulateStackSizes(OptixProgramGroupprogramGroup,OptixStackSizes*
stackSizes,OptixPipelinepipeline)
57 {
58if(!stackSizes)
59returnOPTIX_ERROR_INVALID_VALUE;
60
61OptixStackSizeslocalStackSizes;
62OptixResultresult =optixProgramGroupGetStackSize(programGroup, &localStackSizes, pipeline);
63if(result!=OPTIX_SUCCESS)
64returnresult;
65
66 stackSizes->cssRG= std::max(stackSizes->cssRG, localStackSizes.cssRG);
67 stackSizes->cssMS= std::max(stackSizes->cssMS, localStackSizes.cssMS);
68 stackSizes->cssCH= std::max(stackSizes->cssCH, localStackSizes.cssCH);
69 stackSizes->cssAH= std::max(stackSizes->cssAH, localStackSizes.cssAH);
70 stackSizes->cssIS= std::max(stackSizes->cssIS, localStackSizes.cssIS);
71 stackSizes->cssCC= std::max(stackSizes->cssCC, localStackSizes.cssCC);
72 stackSizes->dssDC= std::max(stackSizes->dssDC, localStackSizes.dssDC);
73
74returnOPTIX_SUCCESS;
75}

---

<u>403</u>

76
90inlineOptixResultoptixUtilComputeStackSizes(constOptixStackSizes* stackSizes,
91unsignedintmaxTraceDepth,
92unsignedintmaxCCDepth,
93unsignedintmaxDCDepth,
94unsignedint* directCallableStackSizeFromTraversal,
95unsignedint* directCallableStackSizeFromState,
96unsignedint* continuationStackSize)
97 {
98if(!stackSizes)
99returnOPTIX_ERROR_INVALID_VALUE;
100
101constunsignedintcssRG = stackSizes->cssRG;
102constunsignedintcssMS = stackSizes->cssMS;
103constunsignedintcssCH = stackSizes->cssCH;
104constunsignedintcssAH = stackSizes->cssAH;
105constunsignedintcssIS = stackSizes->cssIS;
106constunsignedintcssCC = stackSizes->cssCC;
107constunsignedintdssDC = stackSizes->dssDC;
108
109if(directCallableStackSizeFromTraversal)
110 *directCallableStackSizeFromTraversal = maxDCDepth * dssDC;
111if(directCallableStackSizeFromState)
112 *directCallableStackSizeFromState = maxDCDepth * dssDC;
113
114// upper bound on continuation stack used by call trees of continuation callables
115unsignedintcssCCTree = maxCCDepth * cssCC;
116
117// upper bound on continuation stack used by CH or MS programs including the call tree of
118// continuation callables
119unsignedintcssCHOrMSPlusCCTree = std::max(cssCH, cssMS) + cssCCTree;
120
121// clang-format off
122if(continuationStackSize)
123 *continuationStackSize
124 = cssRG + cssCCTree
125 + (std::max(maxTraceDepth, 1u)-1) * cssCHOrMSPlusCCTree
126 + std::min(maxTraceDepth, 1u) * std::max(cssCHOrMSPlusCCTree, cssIS + cssAH);
127// clang-format on
128
129returnOPTIX_SUCCESS;
130}
131
155inlineOptixResultoptixUtilComputeStackSizesDCSplit(constOptixStackSizes* stackSizes,
156 unsignedintdssDCFromTraversal,
157 unsignedintdssDCFromState,
158 unsignedintmaxTraceDepth,
159 unsignedintmaxCCDepth,
160 unsignedintmaxDCDepthFromTraversal,
161 unsignedintmaxDCDepthFromState,
162 unsignedint*
directCallableStackSizeFromTraversal,
163unsignedint* directCallableStackSizeFromState,
164 unsignedint* continuationStackSize)
165 {
166if(!stackSizes)
167returnOPTIX_ERROR_INVALID_VALUE;
168
169constunsignedintcssRG = stackSizes->cssRG;
170constunsignedintcssMS = stackSizes->cssMS;
171constunsignedintcssCH = stackSizes->cssCH;
172constunsignedintcssAH = stackSizes->cssAH;
173constunsignedintcssIS = stackSizes->cssIS;
174constunsignedintcssCC = stackSizes->cssCC;
175// use dssDCFromTraversal and dssDCFromState instead of stackSizes->dssDC
176
177if(directCallableStackSizeFromTraversal)

---

<u>404</u>

178 *directCallableStackSizeFromTraversal = maxDCDepthFromTraversal * dssDCFromTraversal;
179if(directCallableStackSizeFromState)
180 *directCallableStackSizeFromState = maxDCDepthFromState * dssDCFromState;
181
182// upper bound on continuation stack used by call trees of continuation callables
183unsignedintcssCCTree = maxCCDepth * cssCC;
184
185// upper bound on continuation stack used by CH or MS programs including the call tree of
186// continuation callables
187unsignedintcssCHOrMSPlusCCTree = std::max(cssCH, cssMS) + cssCCTree;
188
189// clang-format off
190if(continuationStackSize)
191 *continuationStackSize
192 = cssRG + cssCCTree
193 + (std::max(maxTraceDepth, 1u)-1) * cssCHOrMSPlusCCTree
194 + std::min(maxTraceDepth, 1u) * std::max(cssCHOrMSPlusCCTree, cssIS + cssAH);
195// clang-format on
196
197returnOPTIX_SUCCESS;
198}
199
216inlineOptixResultoptixUtilComputeStackSizesCssCCTree(constOptixStackSizes* stackSizes,
217 unsignedintcssCCTree,
218 unsignedintmaxTraceDepth,
219 unsignedintmaxDCDepth,
220 unsignedint*
directCallableStackSizeFromTraversal,
221unsignedint* directCallableStackSizeFromState,
222 unsignedint* continuationStackSize)
223 {
224if(!stackSizes)
225returnOPTIX_ERROR_INVALID_VALUE;
226
227constunsignedintcssRG = stackSizes->cssRG;
228constunsignedintcssMS = stackSizes->cssMS;
229constunsignedintcssCH = stackSizes->cssCH;
230constunsignedintcssAH = stackSizes->cssAH;
231constunsignedintcssIS = stackSizes->cssIS;
232// use cssCCTree instead of stackSizes->cssCC and maxCCDepth
233constunsignedintdssDC = stackSizes->dssDC;
234
235if(directCallableStackSizeFromTraversal)
236 *directCallableStackSizeFromTraversal = maxDCDepth * dssDC;
237if(directCallableStackSizeFromState)
238 *directCallableStackSizeFromState = maxDCDepth * dssDC;
239
240// upper bound on continuation stack used by CH or MS programs including the call tree of
241// continuation callables
242unsignedintcssCHOrMSPlusCCTree = std::max(cssCH, cssMS) + cssCCTree;
243
244// clang-format off
245if(continuationStackSize)
246 *continuationStackSize
247 = cssRG + cssCCTree
248 + (std::max(maxTraceDepth, 1u)-1) * cssCHOrMSPlusCCTree
249 + std::min(maxTraceDepth, 1u) * std::max(cssCHOrMSPlusCCTree, cssIS + cssAH);
250// clang-format on
251
252returnOPTIX_SUCCESS;
253}
254
270inlineOptixResultoptixUtilComputeStackSizesSimplePathTracer(OptixProgramGroupprogramGroupRG,
271 OptixProgramGroupprogramGroupMS1,
272 constOptixProgramGroup* programGroupCH1,
273 unsignedintprogramGroupCH1Count,
274 OptixProgramGroupprogramGroupMS2,

---

<u>405</u>

275 constOptixProgramGroup* programGroupCH2,
276 unsignedintprogramGroupCH2Count,
277 unsignedint*
directCallableStackSizeFromTraversal,
278 unsignedint* directCallableStackSizeFromState,
279 unsignedint* continuationStackSize,
280 OptixPipelinepipeline)
281 {
282if(!programGroupCH1 && (programGroupCH1Count > 0))
283returnOPTIX_ERROR_INVALID_VALUE;
284if(!programGroupCH2 && (programGroupCH2Count > 0))
285returnOPTIX_ERROR_INVALID_VALUE;
286
287OptixResultresult;
288
289OptixStackSizesstackSizesRG = {};
290 result =optixProgramGroupGetStackSize(programGroupRG, &stackSizesRG, pipeline);
291if(result!=OPTIX_SUCCESS)
292returnresult;
293
294OptixStackSizesstackSizesMS1 = {};
295 result =optixProgramGroupGetStackSize(programGroupMS1, &stackSizesMS1,
pipeline);
296if(result!=OPTIX_SUCCESS)
297returnresult;
298
299OptixStackSizesstackSizesCH1 = {};
300for(unsignedinti = 0; i < programGroupCH1Count; ++i)
301 {
302 result =optixUtilAccumulateStackSizes(programGroupCH1[i], &stackSizesCH1, pipeline);
303if(result!=OPTIX_SUCCESS)
304returnresult;
305}
306
307OptixStackSizesstackSizesMS2 = {};
308 result =optixProgramGroupGetStackSize(programGroupMS2, &stackSizesMS2,
pipeline);
309if(result!=OPTIX_SUCCESS)
310returnresult;
311
312OptixStackSizesstackSizesCH2 = {};
313 memset(&stackSizesCH2, 0,sizeof(OptixStackSizes));
314for(unsignedinti = 0; i < programGroupCH2Count; ++i)
315 {
316 result =optixUtilAccumulateStackSizes(programGroupCH2[i], &stackSizesCH2, pipeline);
317if(result!=OPTIX_SUCCESS)
318returnresult;
319}
320
321constunsignedintcssRG = stackSizesRG.cssRG;
322constunsignedintcssMS1 = stackSizesMS1.cssMS;
323constunsignedintcssCH1 = stackSizesCH1.cssCH;
324constunsignedintcssMS2 = stackSizesMS2.cssMS;
325constunsignedintcssCH2 = stackSizesCH2.cssCH;
326// no AH, IS, CC, or DC programs
327
328if(directCallableStackSizeFromTraversal)
329 *directCallableStackSizeFromTraversal = 0;
330if(directCallableStackSizeFromState)
331 *directCallableStackSizeFromState = 0;
332
333if(continuationStackSize)
334 *continuationStackSize = cssRG + std::max(cssMS1, cssCH1 + std::max(cssMS2, cssCH2));
335
336returnOPTIX_SUCCESS;
337}
338// end group optix_utilities

---

<u>406</u>

340
341#ifdef __cplusplus
342}
343#endif
344
345#endif// OPTIX_OPTIX_STACK_SIZE_H

## 8.23 optix_stubs.h File Reference

## Macros

•#defineWIN32_LEAN_AND_MEAN1

## Functions

•static void *∗* optixLoadWindowsDllFromName(const char *∗*optixDllName)

•static void *∗* optixLoadWindowsDll()

•OPTIXAPIOptixResultoptixInitWithHandle(void *∗∗*handlePtr)

•OPTIXAPIOptixResultoptixInit(void)

•OPTIXAPIOptixResultoptixUninitWithHandle(void *∗*handle)

## Variables

•OptixFunctionTableOPTIX_FUNCTION_TABLE_SYMBOL

## 8.23.1 Detailed Description

Author

OptiX public API header.

NVIDIA Corporation

## 8.23.2 Macro Definition Documentation

## 8.23.2.1 WIN32_LEAN_AND_MEAN

#define WIN32_LEAN_AND_MEAN 1

## 8.23.3 Function Documentation

## 8.23.3.1 optixLoadWindowsDll( )

static void *∗* optixLoadWindowsDll () *[static]*

## 8.23.3.2 optixLoadWindowsDllFromName( )

static void *∗* optixLoadWindowsDllFromName (

const char *∗ optixDllName*) *[static]*

8.24 optix_stubs.h

Go to the documentation of this file.

1/*

2* SPDX-FileCopyrightText: Copyright (c) 2019-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.

3* SPDX-License-Identifier: BSD-3-Clause
4*

---

<u>407</u>

5* Redistribution and use in source and binary forms, with or without
6* modification, are permitted provided that the following conditions are met:
7*
8* 1. Redistributions of source code must retain the above copyright notice, this
9* list of conditions and the following disclaimer.
10*
11* 2. Redistributions in binary form must reproduce the above copyright notice,
12* this list of conditions and the following disclaimer in the documentation
13* and/or other materials provided with the distribution.
14*
15* 3. Neither the name of the copyright holder nor the names of its
16* contributors may be used to endorse or promote products derived from
17* this software without specific prior written permission.
18*
19* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
20* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
21* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
22* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
23* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
24* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
25* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
26* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
27* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
28* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
29*/
30
34
35#ifndef OPTIX_OPTIX_STUBS_H
36#define OPTIX_OPTIX_STUBS_H
37
38#include "optix_function_table.h"
39
40#ifdef _WIN32
41#ifndef WIN32_LEAN_AND_MEAN
42#define WIN32_LEAN_AND_MEAN 1
43#endif
44#include <windows.h>
45// The cfgmgr32 header is necessary for interrogating driver information in the registry.
46// For convenience the library is also linked in automatically using the #pragma command.
47#include <cfgmgr32.h>
48#pragma comment(lib, "Cfgmgr32.lib")
49#include <string.h>
50#else
51#include <dlfcn.h>
52#endif
53
56#ifndef OPTIXAPI
57# ifdef OPTIX_ENABLE_SDK_MIXING
58# define OPTIXAPI static
59# else// OPTIX_ENABLE_SDK_MIXING
60# ifdef __cplusplus
61# define OPTIXAPI extern "C"
62# else// __cplusplus
63# define OPTIXAPI
64# endif// __cplusplus
65# endif// OPTIX_ENABLE_SDK_MIXING
66#endif// OPTIXAPI
67
68#ifdef __cplusplus
69extern"C"{
70#endif
71
72// The function table needs to be defined in exactly one translation unit. This can be
73// achieved by including optix_function_table_definition.h in that translation unit.
74externOptixFunctionTableOPTIX_FUNCTION_TABLE_SYMBOL;
75
76#ifdef __cplusplus

---

<u>408</u>

77}
78#endif
79
80#ifdef _WIN32
81#if defined(_MSC_VER)
82// Visual Studio produces warnings suggesting strcpy and friends being replaced with _s
83// variants. All the string lengths and allocation sizes have been calculated and should
84// be safe, so we are disabling this warning to increase compatibility.
85#pragma warning(push)
86#pragma warning(disable : 4996)
87#endif
88staticvoid*optixLoadWindowsDllFromName(constchar* optixDllName)
89 {
90void* handle = NULL;
91
92// Try the bare dll name first. This picks it up in the local path, followed by
93// standard Windows paths.
94 handle = LoadLibraryA((LPSTR)optixDllName);
95if(handle)
96returnhandle;
97// If we don’t find it in the default dll search path, try the system paths
98
99// Get the size of the path first, then allocate
100unsignedintsize = GetSystemDirectoryA(NULL, 0);
101if(size == 0)
102 {
103// Couldn’t get the system path size, so bail
104returnNULL;
105}
106size_tpathSize = size + 1 + strlen(optixDllName);
107char* systemPath = (char*)malloc(pathSize);
108if(systemPath == NULL)
109returnNULL;
110if(GetSystemDirectoryA(systemPath, size)!= size-1)
111 {
112// Something went wrong
113 free(systemPath);
114returnNULL;
115}
116 strcat(systemPath,"*\\*");
117 strcat(systemPath, optixDllName);
118 handle = LoadLibraryA(systemPath);
119 free(systemPath);
120if(handle)
121returnhandle;
122
123// If we didn’t find it, go looking in the register store. Since nvoptix.dll doesn’t
124// have its own registry entry, we are going to look for the opengl driver which lives
125// next to nvoptix.dll. 0 (null) will be returned if any errors occured.
126
127staticconstchar* deviceInstanceIdentifiersGUID ="{4d36e968-e325-11ce-bfc1-08002be10318}";
128constULONG flags = CM_GETIDLIST_FILTER_CLASS |
CM_GETIDLIST_FILTER_PRESENT;
129 ULONG deviceListSize = 0;
130if(CM_Get_Device_ID_List_SizeA(&deviceListSize, deviceInstanceIdentifiersGUID, flags)!= CR_SUCCESS)
131 {
132returnNULL;
133}
134char* deviceNames = (char*)malloc(deviceListSize);
135if(deviceNames == NULL)
136returnNULL;
137if(CM_Get_Device_ID_ListA(deviceInstanceIdentifiersGUID, deviceNames, deviceListSize, flags))
138 {
139 free(deviceNames);
140returnNULL;
141}
142 DEVINST devID = 0;

---

<u>409</u>

143char* dllPath = NULL;
144
145// Continue to the next device if errors are encountered.
146for(char* deviceName = deviceNames; *deviceName; deviceName += strlen(deviceName) + 1)
147 {
148if(CM_Locate_DevNodeA(&devID, deviceName, CM_LOCATE_DEVNODE_NORMAL)!= CR_SUCCESS)
149 {
150continue;
151}
152 HKEY regKey = 0;
153if(CM_Open_DevNode_Key(devID, KEY_QUERY_VALUE, 0, RegDisposition_OpenExisting, &regKey,
CM_REGISTRY_SOFTWARE)!= CR_SUCCESS)
154 {
155continue;
156}
157constchar* valueName ="OpenGLDriverName";
158 DWORD valueSize = 0;
159 LSTATUS ret = RegQueryValueExA(regKey, valueName, NULL, NULL, NULL, &valueSize);
160if(ret!= ERROR_SUCCESS)
161 {
162 RegCloseKey(regKey);
163continue;
164}
165char* regValue = (char*)malloc(valueSize);
166if(regValue == NULL)
167 {
168 RegCloseKey(regKey);
169continue;
170}
171 ret = RegQueryValueExA(regKey, valueName, NULL, NULL, (LPBYTE)regValue, &valueSize);
172if(ret!= ERROR_SUCCESS)
173 {
174 free(regValue);
175 RegCloseKey(regKey);
176continue;
177}
178// Strip the opengl driver dll name from the string then create a new string with
179// the path and the nvoptix.dll name
180for(inti = (int)valueSize-1; i >= 0 && regValue[i]!=’*\\*’;--i)
181 regValue[i] =’*\*0’;
182size_tnewPathSize = strlen(regValue) + strlen(optixDllName) + 1;
183 dllPath = (char*)malloc(newPathSize);
184if(dllPath == NULL)
185 {
186 free(regValue);
187 RegCloseKey(regKey);
188continue;
189}
190 strcpy(dllPath, regValue);
191 strcat(dllPath, optixDllName);
192 free(regValue);
193 RegCloseKey(regKey);
194 handle = LoadLibraryA((LPCSTR)dllPath);
195 free(dllPath);
196if(handle)
197break;
198}
199 free(deviceNames);
200returnhandle;
201}
202#if defined(_MSC_VER)
203#pragma warning(pop)
204#endif
205
206staticvoid*optixLoadWindowsDll( )
207 {
208returnoptixLoadWindowsDllFromName("nvoptix.dll");

---

<u>410</u>

209}
210#endif
211
214
224OPTIXAPIinlineOptixResultoptixInitWithHandle(void** handlePtr)
225 {
226// Make sure these functions get initialized to zero in case the DLL and function
227// table can’t be loaded
228OPTIX_FUNCTION_TABLE_SYMBOL.optixGetErrorName= 0;
229OPTIX_FUNCTION_TABLE_SYMBOL.optixGetErrorString= 0;
230
231if(!handlePtr)
232returnOPTIX_ERROR_INVALID_VALUE;
233
234#ifdef _WIN32
235 *handlePtr =optixLoadWindowsDll();
236if(!*handlePtr)
237returnOPTIX_ERROR_LIBRARY_NOT_FOUND;
238
239void* symbol = (void*)GetProcAddress((HMODULE)*handlePtr,"optixQueryFunctionTable");
240if(!symbol)
241returnOPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND;
242#else
243 *handlePtr = dlopen("libnvoptix.so.1", RTLD_NOW);
244if(!*handlePtr)
245returnOPTIX_ERROR_LIBRARY_NOT_FOUND;
246
247void* symbol = dlsym(*handlePtr,"optixQueryFunctionTable");
248if(!symbol)
249returnOPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND;
250#endif
251
252OptixQueryFunctionTable_t* optixQueryFunctionTable = (OptixQueryFunctionTable_t*)symbol;
253
254returnoptixQueryFunctionTable(OPTIX_ABI_VERSION, 0, 0, 0, &OPTIX_FUNCTION_TABLE_SYMBOL,
sizeof(OPTIX_FUNCTION_TABLE_SYMBOL));
255}
256
260OPTIXAPIinlineOptixResultoptixInit(void)
261 {
262void* handle;
263returnoptixInitWithHandle(&handle);
264}
265
271OPTIXAPIinlineOptixResultoptixUninitWithHandle(void* handle)
272 {
273if(!handle)
274returnOPTIX_ERROR_INVALID_VALUE;
275#ifdef _WIN32
276if(!FreeLibrary((HMODULE)handle))
277returnOPTIX_ERROR_LIBRARY_UNLOAD_FAILURE;
278#else
279if(dlclose(handle))
280returnOPTIX_ERROR_LIBRARY_UNLOAD_FAILURE;
281#endif
282OptixFunctionTableempty
283#ifdef __cplusplus
284 {}
285#else
286 = { 0}
287#endif
288;
289OPTIX_FUNCTION_TABLE_SYMBOL= empty;
290returnOPTIX_SUCCESS;
291}
292
293// end group optix_utilities

---

<u>411</u>

295
296#ifndef OPTIX_DOXYGEN_SHOULD_SKIP_THIS
297
298// Stub functions that forward calls to the corresponding function pointer in the function
299
300OPTIXAPIinlineconstchar*optixGetErrorName(OptixResultresult)
301 {
302if(OPTIX_FUNCTION_TABLE_SYMBOL.optixGetErrorName)
303returnOPTIX_FUNCTION_TABLE_SYMBOL.optixGetErrorName(result);
304
305// If the DLL and symbol table couldn’t be loaded, provide a set of error strings
306// suitable for processing errors related to the DLL loading.
307switch(result)
308 {
309caseOPTIX_SUCCESS:
310return"OPTIX_SUCCESS";
311caseOPTIX_ERROR_INVALID_VALUE:
312return"OPTIX_ERROR_INVALID_VALUE";
313caseOPTIX_ERROR_UNSUPPORTED_ABI_VERSION:
314return"OPTIX_ERROR_UNSUPPORTED_ABI_VERSION";
315caseOPTIX_ERROR_FUNCTION_TABLE_SIZE_MISMATCH:
316return"OPTIX_ERROR_FUNCTION_TABLE_SIZE_MISMATCH";
317caseOPTIX_ERROR_INVALID_ENTRY_FUNCTION_OPTIONS:
318return"OPTIX_ERROR_INVALID_ENTRY_FUNCTION_OPTIONS";
319caseOPTIX_ERROR_LIBRARY_NOT_FOUND:
320return"OPTIX_ERROR_LIBRARY_NOT_FOUND";
321caseOPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND:
322return"OPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND";
323caseOPTIX_ERROR_LIBRARY_UNLOAD_FAILURE:
324return"OPTIX_ERROR_LIBRARY_UNLOAD_FAILURE";
325default:
326return"Unknown OptixResult code";
327}
328}
329
330OPTIXAPIinlineconstchar*optixGetErrorString(OptixResultresult)
331 {
332if(OPTIX_FUNCTION_TABLE_SYMBOL.optixGetErrorString)
333returnOPTIX_FUNCTION_TABLE_SYMBOL.optixGetErrorString(result);
334
335// If the DLL and symbol table couldn’t be loaded, provide a set of error strings
336// suitable for processing errors related to the DLL loading.
337switch(result)
338 {
339caseOPTIX_SUCCESS:
340return"Success";
341caseOPTIX_ERROR_INVALID_VALUE:
342return"Invalid value";
343caseOPTIX_ERROR_UNSUPPORTED_ABI_VERSION:
344return"Unsupported ABI version";
345caseOPTIX_ERROR_FUNCTION_TABLE_SIZE_MISMATCH:
346return"Function table size mismatch";
347caseOPTIX_ERROR_INVALID_ENTRY_FUNCTION_OPTIONS:
348return"Invalid options to entry function";
349caseOPTIX_ERROR_LIBRARY_NOT_FOUND:
350return"Library not found";
351caseOPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND:
352return"Entry symbol not found";
353caseOPTIX_ERROR_LIBRARY_UNLOAD_FAILURE:
354return"Library could not be unloaded";
355default:
356return"Unknown OptixResult code";
357}
358}
359
360OPTIXAPIinlineOptixResultoptixDeviceContextCreate(CUcontext fromContext,const
OptixDeviceContextOptions* options,OptixDeviceContext* context)

---

412

| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextCreate(fromContext, options, context);
} |
| --- |
| OPTIXAPI inline OptixResult optixDeviceContextDestroy(OptixDeviceContext context) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextDestroy(context);
} |
| OPTIXAPI inline OptixResult optixDeviceContextGetProperty(OptixDeviceContext context,
OptixDeviceProperty property, void* value, size_t sizeInBytes) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextGetProperty(context, property, value,
sizeInBytes);
} |
| OPTIXAPI inline OptixResult optixDeviceContextSetLogCallback(OptixDeviceContext context,
OptixLogCallback callbackFunction,
void* callbackData,
unsigned int callbackLevel) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextSetLogCallback(context, callbackFunction,
callbackData, callbackLevel);
} |
| OPTIXAPI inline OptixResult optixDeviceContextSetCacheEnabled(OptixDeviceContext context, int enabled) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextSetCacheEnabled(context, enabled);
} |
| OPTIXAPI inline OptixResult optixDeviceContextSetCacheLocation(OptixDeviceContext context, const char*
location) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextSetCacheLocation(context, location);
} |
| OPTIXAPI inline OptixResult optixDeviceContextGetCacheEnabled(OptixDeviceContext context, int* enabled) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextGetCacheEnabled(context, enabled);
} |
| OPTIXAPI inline OptixResult optixDeviceContextGetCacheLocation(OptixDeviceContext context, char*
location, size_t locationSize) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextGetCacheLocation(context, location,
locationSize);
} |
| OPTIXAPI inline OptixResult optixDeviceContextGetCacheDatabaseSizes(OptixDeviceContext context, size_t*
lowWaterMark, size_t* highWaterMark) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextGetCacheDatabaseSizes(context, lowWaterMark,
highWaterMark);
} |
| OPTIXAPI inline OptixResult optixModuleCreate(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const char* input,
size_t inputSize, |

---

char* logString,
size_t* logStringSize,
OptixModule* module)

{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixModuleCreate(context, moduleCompileOptions,
pipelineCompileOptions, input,
inputSize, logString, logStringSize, module);
}

OPTIXAPI inline OptixResult optixModuleCreateWithTasks(OptixDeviceContext context,
const OptixModuleCompileOptions*
moduleCompileOptions,
const OptixPipelineCompileOptions*
pipelineCompileOptions,
const char* input,
size_t inputSize,
char* logString,
size_t* logStringSize,
OptixModule* module,
OptixTask* firstTask)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixModuleCreateWithTasks(context, moduleCompileOptions,
pipelineCompileOptions, input,
inputSize, logString, logStringSize,
module, firstTask);
}

OPTIXAPI inline OptixResult optixModuleGetCompilationState(OptixModule module, OptixModuleCompileState*
state)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixModuleGetCompilationState(module, state);
}

OPTIXAPI inline OptixResult optixModuleCancelCreation(OptixModule module, OptixCreationFlags flags)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixModuleCancelCreation(module, flags);
}

OPTIXAPI inline OptixResult optixDeviceContextCancelCreations(OptixDeviceContext context,
OptixCreationFlags flags)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDeviceContextCancelCreations(context, flags);
}

OPTIXAPI inline OptixResult optixModuleDestroy(OptixModule module)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixModuleDestroy(module);
}

OPTIXAPI inline OptixResult optixBuiltinISModuleGet(OptixDeviceContext context,
const OptixModuleCompileOptions* moduleCompileOptions,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const OptixBuiltinISOptions* builtinISOptions,
OptixModule* builtinModule)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixBuiltinISModuleGet(context, moduleCompileOptions,
pipelineCompileOptions,
builtinISOptions, builtinModule);
}

OPTIXAPI inline OptixResult optixTaskExecute(OptixTask task,
OptixTask* additionalTasks,
unsigned int maxNumAdditionalTasks,
unsigned int* numAdditionalTasksCreated)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixTaskExecute(task, additionalTasks, maxNumAdditionalTasks, numAdditionalTasksCreated);
}

OPTIXAPI inline OptixResult optixTaskGetSerializationKey(OptixTask task, void* key, size_t* size)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixTaskGetSerializationKey(task, key, size);
}

OPTIXAPI inline OptixResult optixTaskSerializeOutput(OptixTask task, void* data, size_t* size)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixTaskSerializeOutput(task, data, size);
}

OPTIXAPI inline OptixResult optixTaskDeserializeOutput(OptixTask task,
const void* data,
size_t size,
OptixTask* additionalTasks,
unsigned int maxNumAdditionalTasks,
unsigned int* numAdditionalTasksCreated)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixTaskDeserializeOutput(task, data, size, additionalTasks,
maxNumAdditionalTasks,
numAdditionalTasksCreated);
}

OPTIXAPI inline OptixResult optixProgramGroupCreate(OptixDeviceContext context,
const OptixProgramGroupDesc* programDescriptions,
unsigned int numProgramGroups,
const OptixProgramGroupOptions options,
char* logString,
size_t* logStringSize,
OptixProgramGroup* programGroups)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixProgramGroupCreate(context, programDescriptions,
numProgramGroups, options,
logString, logStringSize, programGroups);
}

OPTIXAPI inline OptixResult optixProgramGroupDestroy(OptixProgramGroup programGroup)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixProgramGroupDestroy(programGroup);
}

OPTIXAPI inline OptixResult optixProgramGroupGetStackSize(OptixProgramGroup programGroup,
OptixStackSizes* stackSizes, OptixPipeline pipeline)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixProgramGroupGetStackSize(programGroup, stackSizes, pipeline);
}

OPTIXAPI inline OptixResult optixPipelineCreate(OptixDeviceContext context,
const OptixPipelineCompileOptions* pipelineCompileOptions,
const OptixPipelineLinkOptions* pipelineLinkOptions,
const OptixProgramGroup* programGroups,
unsigned int numProgramGroups,
char* logString,
size_t* logStringSize,
OptixPipeline* pipeline)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixPipelineCreate(context, pipelineCompileOptions,
pipelineLinkOptions, programGroups,
numProgramGroups, logString, logStringSize,
pipeline);
}

OPTIXAPI inline OptixResult optixPipelineDestroy(OptixPipeline pipeline)
{
    return OPTIX_FUNCTION_TABLE_SYMBOL.optixPipelineDestroy(pipeline);

---

| OPTIXAPI inline OptixResult optixPipelineSetStackSizeFromCallDepths(OptixPipeline pipeline, unsigned int maxTraceDepth, unsigned int maxContinuationCallableDepth, unsigned int maxDirectCallableDepthFromState, unsigned int maxDirectCallableDepthFromTraversal, unsigned int maxTraversableGraphDepth) |
| --- |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixPipelineSetStackSizeFromCallDepths(pipeline, maxTraceDepth, maxContinuationCallableDepth, maxDirectCallableDepthFromState, maxDirectCallableDepthFromTraversal, maxTraversableGraphDepth);
} |
| OPTIXAPI inline OptixResult optixPipelineSetStackSize(OptixPipeline pipeline, unsigned int directCallableStackSizeFromTraversal, unsigned int directCallableStackSizeFromState, unsigned int continuationStackSize, unsigned int maxTraversableGraphDepth) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixPipelineSetStackSize(pipeline, directCallableStackSizeFromTraversal, directCallableStackSizeFromState, continuationStackSize, maxTraversableGraphDepth);
} |
| OPTIXAPI inline OptixResult optixPipelineSymbolMemcpyAsync(OptixPipeline pipeline, const char* name, void* mem, size_t sizeInBytes, size_t offsetInBytes, OptixPipelineSymbolMemcpyKind kind, CUstream stream) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixPipelineSymbolMemcpyAsync(pipeline, name, mem, sizeInBytes, offsetInBytes, kind, stream);
} |
| OPTIXAPI inline OptixResult optixAccelComputeMemoryUsage(OptixDeviceContext context, const OptixAccelBuildOptions* accelOptions, const OptixBuildInput* buildInputs, unsigned int numBuildInputs, OptixAccelBufferSizes* bufferSizes) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixAccelComputeMemoryUsage(context, accelOptions, buildInputs, numBuildInputs, bufferSizes);
} |
| OPTIXAPI inline OptixResult optixAccelBuild(OptixDeviceContext context, CUstream stream, const OptixAccelBuildOptions* accelOptions, const OptixBuildInput* buildInputs, unsigned int numBuildInputs, CUdeviceptr tempBuffer, size_t tempBufferSizeInBytes, CUdeviceptr outputBuffer, size_t outputBufferSizeInBytes, OptixTraversableHandle* outputHandle, const OptixAccelEmitDesc* emittedProperties, unsigned int numEmittedProperties) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixAccelBuild(context, stream, accelOptions, buildInputs, |

---

numBuildInputs, tempBuffer,

tempBufferSizeInBytes, outputBuffer,
outputBufferSizeInBytes,

outputHandle, emittedProperties, numEmittedProperties);

}

OPTIXAPI inline OptixResult optixAccelGetRelocationInfo(OptixDeviceContext context,
OptixTraversableHandle handle, OptixRelocationInfo* info)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixAccelGetRelocationInfo(context, handle, info);
}

OPTIXAPI inline OptixResult optixCheckRelocationCompatibility(OptixDeviceContext context, const
OptixRelocationInfo* info, int* compatible)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixCheckRelocationCompatibility(context, info, compatible);
}

OPTIXAPI inline OptixResult optixAccelRelocate(OptixDeviceContext context,
CUstream stream,
const OptixRelocationInfo* info,
const OptixRelocateInput* relocateInputs,
size_t numRelocateInputs,
CUdeviceptr targetAccel,
size_t targetAccelSizeInBytes,
OptixTraversableHandle* targetHandle)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixAccelRelocate(context, stream, info, relocateInputs,
numRelocateInputs,
targetAccel, targetAccelSizeInBytes, targetHandle);
}

OPTIXAPI inline OptixResult optixAccelCompact(OptixDeviceContext context,
CUstream stream,
OptixTraversableHandle inputHandle,
CUdeviceptr outputBuffer,
size_t outputBufferSizeInBytes,
OptixTraversableHandle* outputHandle)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixAccelCompact(context, stream, inputHandle, outputBuffer,
outputBufferSizeInBytes, outputHandle);
}

OPTIXAPI inline OptixResult optixAccelEmitProperty(OptixDeviceContext context,
CUstream stream,
OptixTraversableHandle handle,
const OptixAccelEmitDesc* emittedProperty)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixAccelEmitProperty(context, stream, handle, emittedProperty);
}

OPTIXAPI inline OptixResult optixConvertPointerToTraversableHandle(OptixDeviceContext onDevice,
CUdeviceptr pointer,
OptixTraversableType traversableType,
OptixTraversableHandle* traversableHandle)
{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixConvertPointerToTraversableHandle(onDevice, pointer,
traversableType, traversableHandle);
}

OPTIXAPI inline OptixResult optixOpacityMicromapArrayComputeMemoryUsage(OptixDeviceContext context,
const
OptixOpacityMicromapArrayBuildInput* buildInput,
OptixMicromapBufferSizes* bufferSizes)
{

---

| return OPTIX_FUNCTION_TABLE_SYMBOL.optixOpacityMicromapArrayComputeMemoryUsage(context, buildInput, bufferSizes); |
| --- |
| OPTIXAPI inline OptixResult optixOpacityMicromapArrayBuild(OptixDeviceContext context, CUstream stream, const OptixOpacityMicromapArrayBuildInput* buildInput, const OptixMicromapBuffers* buffers) |
| return OPTIX_FUNCTION_TABLE_SYMBOL.optixOpacityMicromapArrayBuild(context, stream, buildInput, buffers); |
| OPTIXAPI inline OptixResult optixOpacityMicromapArrayGetRelocationInfo(OptixDeviceContext context, CUdeviceptr opacityMicromapArray, OptixRelocationInfo* info) |
| return OPTIX_FUNCTION_TABLE_SYMBOL.optixOpacityMicromapArrayGetRelocationInfo(context, opacityMicromapArray, info); |
| OPTIXAPI inline OptixResult optixOpacityMicromapArrayRelocate(OptixDeviceContext context, CUstream stream, const OptixRelocationInfo* info, CUdeviceptr targetOpacityMicromapArray, size_t targetOpacityMicromapArraySizeInBytes) |
| return OPTIX_FUNCTION_TABLE_SYMBOL.optixOpacityMicromapArrayRelocate(context, stream, info, targetOpacityMicromapArray, targetOpacityMicromapArraySizeInBytes); |
| OPTIXAPI inline OptixResult optixClusterAccelComputeMemoryUsage(OptixDeviceContext context, OptixClusterAccelBuildMode buildMode, const OptixClusterAccelBuildInput* buildInput, OptixAccelBufferSizes* bufferSizes) |
| return OPTIX_FUNCTION_TABLE_SYMBOL.optixClusterAccelComputeMemoryUsage(context, buildMode, buildInput, bufferSizes); |
| OPTIXAPI inline OptixResult optixClusterAccelBuild(OptixDeviceContext context, CUstream stream, const OptixClusterAccelBuildModeDesc* buildModeDesc, const OptixClusterAccelBuildInput* buildInput, CUdeviceptr argsArray, CUdeviceptr argsCount, unsigned int argsStrideInBytes) |
| return OPTIX_FUNCTION_TABLE_SYMBOL.optixClusterAccelBuild(context, stream, buildModeDesc, buildInput, argsArray, argsCount, argsStrideInBytes); |
| OPTIXAPI inline OptixResult optixSbtRecordPackHeader(OptixProgramGroup programGroup, void* sbtRecordHeaderHostPointer) |
| return OPTIX_FUNCTION_TABLE_SYMBOL.optixSbtRecordPackHeader(programGroup, sbtRecordHeaderHostPointer); |
| OPTIXAPI inline OptixResult optixLaunch(OptixPipeline pipeline, |

---

|  | CUstream | stream, |
| --- | --- | --- |
|  | CUdeviceptr | pipelineParams, |
|  | size_t | pipelineParamsSize, |
|  | const OptixShaderBindingTable* sbt, | unsigned int width, |
|  | unsigned int height, | unsigned int depth) |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixLaunch(pipeline, stream, pipelineParams, pipelineParamsSize, sbt, width, height, depth);
} |  |  |
| OPTIXAPI inline OptixResult optixCoopVecMatrixConvert(OptixDeviceContext context,CUstream stream,unsigned int numNetworks,const OptixNetworkDescription* inputNetworkDescription,CUdeviceptr inputNetworks,size_t inputNetworkStrideInBytes,const OptixNetworkDescription* outputNetworkDescription,CUdeviceptr outputNetworks,size_t outputNetworkStrideInBytes) |  |  |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixCoopVecMatrixConvert(context, stream, numNetworks, inputNetworkDescription,inputNetworks, inputNetworkStrideInBytes,outputNetworkDescription,outputNetworks, outputNetworkStrideInBytes);}
} |  |  |
| OPTIXAPI inline OptixResult optixCoopVecMatrixComputeSize(OptixDeviceContext context,unsigned int N,unsigned int K,OptixCoopVecElemType elementType,OptixCoopVecMatrixLayout layout,size_t rowColumnStrideInBytes,size_t* sizeInBytes) |  |  |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixCoopVecMatrixComputeSize(context, N, K, elementType, layout,rowColumnStrideInBytes,sizeInBytes);}
} |  |  |
| OPTIXAPI inline OptixResult optixDenoiserCreate(OptixDeviceContext context,OptixDenoiserModelKind modelKind,const OptixDenoiserOptions* options,OptixDenoiser* returnHandle) |  |  |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserCreate(context, modelKind, options, returnHandle);}
} |  |  |
| OPTIXAPI inline OptixResult optixDenoiserCreateWithUserModel(OptixDeviceContext context,const void* data,size_t dataSizeInBytes,OptixDenoiser* returnHandle) |  |  |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserCreateWithUserModel(context, data, dataSizeInBytes,returnHandle);}
} |  |  |
| OPTIXAPI inline OptixResult optixDenoiserDestroy(OptixDenoiser handle) |  |  |
| {
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserDestroy(handle);}
} |  |  |
| OPTIXAPI inline OptixResult optixDenoiserComputeMemoryResources(const OptixDenoiser handle,unsigned int maximumInputWidth,unsigned int maximumInputHeight,OptixDenoiserSizes* returnSizes) |  |  |

---

{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserComputeMemoryResources(handle, maximumInputWidth,
maximumInputHeight, returnSizes);
}

OPTIXAPI inline OptixResult optixDenoiserSetup(OptixDenoiser denoiser,

CUstream stream,
unsigned int inputWidth,
unsigned int inputHeight,
CUdeviceptr denoiserState,
size_t denoiserStateSizeInBytes,
CUdeviceptr scratch,
size_t scratchSizeInBytes)

{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserSetup(denoiser, stream, inputWidth, inputHeight,
denoiserState,
denoiserStateSizeInBytes, scratch,
scratchSizeInBytes);
}

OPTIXAPI inline OptixResult optixDenoiserInvoke(OptixDenoiser handle,

CUstream stream,
const OptixDenoiserParams* params,
CUdeviceptr denoiserData,
size_t denoiserDataSize,
const OptixDenoiserGuideLayer* guideLayer,
const OptixDenoiserLayer* layers,
unsigned int numLayers,
unsigned int inputOffsetX,
unsigned int inputOffsetY,
CUdeviceptr scratch,
size_t scratchSizeInBytes)

{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserInvoke(handle, stream, params, denoiserData,
denoiserDataSize,
guideLayer, layers, numLayers, inputOffsetX,
inputOffsetY,
scratch, scratchSizeInBytes);
}

OPTIXAPI inline OptixResult optixDenoiserComputeIntensity(OptixDenoiser handle,

CUstream stream,
const OptixImage2D* inputImage,
CUdeviceptr outputIntensity,
CUdeviceptr scratch,
size_t scratchSizeInBytes)

{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserComputeIntensity(handle, stream, inputImage,
outputIntensity,
scratch, scratchSizeInBytes);
}

OPTIXAPI inline OptixResult optixDenoiserComputeAverageColor(OptixDenoiser handle,

CUstream stream,
const OptixImage2D* inputImage,
CUdeviceptr outputAverageColor,
CUdeviceptr scratch,
size_t scratchSizeInBytes)

{
return OPTIX_FUNCTION_TABLE_SYMBOL.optixDenoiserComputeAverageColor(handle, stream, inputImage,
outputAverageColor,
scratch, scratchSizeInBytes);
}

#endif // OPTIX_DOXYGEN_SHOULD_SKIP_THIS

#endif // OPTIX_OPTIX_STUBS_H

---

## 8.25 optix_types.h File Reference

## Classes
• struct OptixDeviceContextOptions
• struct OptixOpacityMicromapUsageCount
• struct OptixBuildInputOpacityMicromap
• struct OptixRelocateInputOpacityMicromap
• struct OptixBuildInputTriangleArray
• struct OptixRelocateInputTriangleArray
• struct OptixBuildInputCurveArray
• struct OptixBuildInputSphereArray
• struct OptixAabb
• struct OptixBuildInputCustomPrimitiveArray
• struct OptixBuildInputInstanceArray
• struct OptixRelocateInputInstanceArray
• struct OptixBuildInput
• struct OptixRelocateInput
• struct OptixInstance
• struct OptixOpacityMicromapDesc
• struct OptixOpacityMicromapHistogramEntry
• struct OptixOpacityMicromapArrayBuildInput
• struct OptixMicromapBufferSizes
• struct OptixMicromapBuffers
• struct OptixMotionOptions
• struct OptixAccelBuildOptions
• struct OptixAccelBufferSizes
• struct OptixAccelEmitDesc
• struct OptixRelocationInfo
• struct OptixStaticTransform
• struct OptixMatrixMotionTransform
• struct OptixSRTData
• struct OptixSRTMotionTransform
• struct OptixClusterAccelBuildModeDescImplicitDest
• struct OptixClusterAccelBuildModeDescExplicitDest
• struct OptixClusterAccelBuildModeDescGetSize
• struct OptixClusterAccelBuildInputTriangles
• struct OptixClusterAccelBuildInputGrids
• struct OptixClusterAccelBuildInputClusters
• struct OptixClusterAccelPrimitiveInfo
• struct OptixClusterAccelBuildInputTrianglesArgs
• struct OptixClusterAccelBuildInputGridsArgs
• struct OptixClusterAccelBuildInputTemplatesArgs
• struct OptixClusterAccelBuildInputClustersArgs
• struct OptixClusterAccelBuildInput
• struct OptixClusterAccelBuildModeDesc
• struct OptixImage2D
• struct OptixDenoiserOptions
• struct OptixDenoiserGuideLayer
• struct OptixDenoiserLayer
• struct OptixDenoiserParams

---

• struct OptixDenoiserSizes
• struct OptixTraverseData
• struct OptixModuleCompileBoundValueEntry
• struct OptixPayloadType
• struct OptixModuleCompileOptions
• struct OptixBuiltinISOptions
• struct OptixProgramGroupSingleModule
• struct OptixProgramGroupHitgroup
• struct OptixProgramGroupCallables
• struct OptixProgramGroupDesc
• struct OptixProgramGroupOptions
• struct OptixPipelineCompileOptions
• struct OptixPipelineLinkOptions
• struct OptixShaderBindingTable
• struct OptixStackSizes
• struct OptixCoopVecMatrixDescription
• struct OptixNetworkDescription

## Macros

• #define OPTIX_SBT_RECORD_HEADER_SIZE ((size_t)32)
• #define OPTIX_SBT_RECORD_ALIGNMENT 16ull
• #define OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT 128ull
• #define OPTIX_INSTANCE_BYTE_ALIGNMENT 16ull
• #define OPTIX_AABB_BUFFER_BYTE_ALIGNMENT 8ull
• #define OPTIX_GEOMETRY_TRANSFORM_BYTE_ALIGNMENT 16ull
• #define OPTIX_TRANSFORM_BYTE_ALIGNMENT 64ull
• #define OPTIX_OPACITY_MICROMAP_DESC_BUFFER_BYTE_ALIGNMENT 8ull
• #define OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT 0
• #define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_TYPE_COUNT 8
• #define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_VALUE_COUNT 32
• #define OPTIX_OPACITY_MICROMAP_STATE_TRANSPARENT (0)
• #define OPTIX_OPACITY_MICROMAP_STATE_OPAQUE (1)
• #define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_TRANSPARENT (2)
• #define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_OPAQUE (3)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_TRANSPARENT (-1)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_OPAQUE (-2)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_TRANSPARENT (-3)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_OPAQUE (-4)
• #define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_CLUSTER_SKIP_OPACITY_MICROMAP (-5)
• #define OPTIX_OPACITY_MICROMAP_ARRAY_BUFFER_BYTE_ALIGNMENT 128ull
• #define OPTIX_OPACITY_MICROMAP_MAX_SUBDIVISION_LEVEL 12

---

## Typedefs

• typedef unsigned long long CUdeviceptr
• typedef struct OptixDeviceContext_t * OptixDeviceContext
• typedef struct OptixModule_t * OptixModule
• typedef struct OptixProgramGroup_t * OptixProgramGroup
• typedef struct OptixPipeline_t * OptixPipeline
• typedef struct OptixDenoiser_t * OptixDenoiser
• typedef struct OptixTask_t * OptixTask
• typedef unsigned long long OptixTraversableHandle
• typedef unsigned int OptixVisibilityMask
• typedef enum OptixResult OptixResult
• typedef enum OptixDeviceProperty OptixDeviceProperty
• typedef void(* OptixLogCallback) (unsigned int level, const char *tag, const char *message, void *cbdata)
• typedef enum OptixDeviceContextValidationMode OptixDeviceContextValidationMode
• typedef struct OptixDeviceContextOptions OptixDeviceContextOptions
• typedef enum OptixPipelineSymbolMemcpyKind OptixPipelineSymbolMemcpyKind
• typedef enum OptixDevicePropertyShaderExecutionReorderingFlags
OptixDevicePropertyShaderExecutionReorderingFlags
• typedef enum OptixDevicePropertyClusterAccelFlags OptixDevicePropertyClusterAccelFlags
• typedef enum OptixGeometryFlags OptixGeometryFlags
• typedef enum OptixHitKind OptixHitKind
• typedef enum OptixIndicesFormat OptixIndicesFormat
• typedef enum OptixVertexFormat OptixVertexFormat
• typedef enum OptixTransformFormat OptixTransformFormat
• typedef enum OptixOpacityMicromapFormat OptixOpacityMicromapFormat
• typedef enum OptixOpacityMicromapArrayIndexingMode
OptixOpacityMicromapArrayIndexingMode
• typedef struct OptixOpacityMicromapUsageCount OptixOpacityMicromapUsageCount
• typedef struct OptixBuildInputOpacityMicromap OptixBuildInputOpacityMicromap
• typedef struct OptixRelocateInputOpacityMicromap OptixRelocateInputOpacityMicromap
• typedef struct OptixBuildInputTriangleArray OptixBuildInputTriangleArray
• typedef struct OptixRelocateInputTriangleArray OptixRelocateInputTriangleArray
• typedef enum OptixPrimitiveType OptixPrimitiveType
• typedef enum OptixPrimitiveTypeFlags OptixPrimitiveTypeFlags
• typedef enum OptixCurveEndcapFlags OptixCurveEndcapFlags
• typedef struct OptixBuildInputCurveArray OptixBuildInputCurveArray
• typedef struct OptixBuildInputSphereArray OptixBuildInputSphereArray
• typedef struct OptixAabb OptixAabb
• typedef struct OptixBuildInputCustomPrimitiveArray OptixBuildInputCustomPrimitiveArray
• typedef struct OptixBuildInputInstanceArray OptixBuildInputInstanceArray
• typedef struct OptixRelocateInputInstanceArray OptixRelocateInputInstanceArray
• typedef enum OptixBuildInputType OptixBuildInputType
• typedef struct OptixBuildInput OptixBuildInput
• typedef struct OptixRelocateInput OptixRelocateInput
• typedef enum OptixInstanceFlags OptixInstanceFlags
• typedef struct OptixInstance OptixInstance
• typedef enum OptixBuildFlags OptixBuildFlags
• typedef enum OptixOpacityMicromapFlags OptixOpacityMicromapFlags
• typedef struct OptixOpacityMicromapDesc OptixOpacityMicromapDesc

---

• typedef struct OptixOpacityMicromapHistogramEntry OptixOpacityMicromapHistogramEntry
• typedef struct OptixOpacityMicromapArrayBuildInput OptixOpacityMicromapArrayBuildInput
• typedef struct OptixMicromapBufferSizes OptixMicromapBufferSizes
• typedef struct OptixMicromapBuffers OptixMicromapBuffers
• typedef enum OptixBuildOperation OptixBuildOperation
• typedef enum OptixMotionFlags OptixMotionFlags
• typedef struct OptixMotionOptions OptixMotionOptions
• typedef struct OptixAccelBuildOptions OptixAccelBuildOptions
• typedef struct OptixAccelBufferSizes OptixAccelBufferSizes
• typedef enum OptixAccelPropertyType OptixAccelPropertyType
• typedef struct OptixAccelEmitDesc OptixAccelEmitDesc
• typedef struct OptixRelocationInfo OptixRelocationInfo
• typedef struct OptixStaticTransform OptixStaticTransform
• typedef struct OptixMatrixMotionTransform OptixMatrixMotionTransform
• typedef struct OptixSRTData OptixSRTData
• typedef struct OptixSRTMotionTransform OptixSRTMotionTransform
• typedef enum OptixTraversableType OptixTraversableType
• typedef enum OptixClusterAccelBuildFlags OptixClusterAccelBuildFlags
• typedef enum OptixClusterAccelClusterFlags OptixClusterAccelClusterFlags
• typedef enum OptixClusterAccelPrimitiveFlags OptixClusterAccelPrimitiveFlags
• typedef enum OptixClusterAccelBuildType OptixClusterAccelBuildType
• typedef enum OptixClusterAccelBuildMode OptixClusterAccelBuildMode
• typedef enum OptixClusterAccelIndicesFormat OptixClusterAccelIndicesFormat
• typedef struct OptixClusterAccelBuildModeDescImplicitDest
OptixClusterAccelBuildModeDescImplicitDest
• typedef struct OptixClusterAccelBuildModeDescExplicitDest
OptixClusterAccelBuildModeDescExplicitDest
• typedef struct OptixClusterAccelBuildModeDescGetSize
OptixClusterAccelBuildModeDescGetSize
• typedef struct OptixClusterAccelBuildInputTriangles OptixClusterAccelBuildInputTriangles
• typedef struct OptixClusterAccelBuildInputGrids OptixClusterAccelBuildInputGrids
• typedef struct OptixClusterAccelBuildInputClusters OptixClusterAccelBuildInputClusters
• typedef struct OptixClusterAccelPrimitiveInfo OptixClusterAccelPrimitiveInfo
• typedef enum OptixClusterIDValues OptixClusterIDValues
• typedef struct OptixClusterAccelBuildInputTrianglesArgs OptixClusterAccelBuildInputTrianglesArgs
• typedef struct OptixClusterAccelBuildInputTemplatesArgs OptixClusterAccelBuildInputTemplatesArgs
• typedef struct OptixClusterAccelBuildInputClustersArgs OptixClusterAccelBuildInputClustersArgs
• typedef struct OptixClusterAccelBuildInput OptixClusterAccelBuildInput
• typedef struct OptixClusterAccelBuildModeDesc OptixClusterAccelBuildModeDesc
• typedef enum OptixPixelFormat OptixPixelFormat
• typedef struct OptixImage2D OptixImage2D
• typedef enum OptixDenoiserModelKind OptixDenoiserModelKind
• typedef enum OptixDenoiserAlphaMode OptixDenoiserAlphaMode
• typedef struct OptixDenoiserOptions OptixDenoiserOptions
• typedef struct OptixDenoiserGuideLayer OptixDenoiserGuideLayer
• typedef enum OptixDenoiserAOVType OptixDenoiserAOVType

---

• typedef struct OptixDenoiserLayer OptixDenoiserLayer
• typedef struct OptixDenoiserParams OptixDenoiserParams
• typedef struct OptixDenoiserSizes OptixDenoiserSizes
• typedef enum OptixRayFlags OptixRayFlags
• typedef enum OptixTransformType OptixTransformType
• typedef struct OptixTraverseData OptixTraverseData
• typedef enum OptixTraversableGraphFlags OptixTraversableGraphFlags
• typedef enum OptixCompileOptimizationLevel OptixCompileOptimizationLevel
• typedef enum OptixCompileDebugLevel OptixCompileDebugLevel
• typedef enum OptixModuleCompileState OptixModuleCompileState
• typedef enum OptixCreationFlags OptixCreationFlags
• typedef struct OptixModuleCompileBoundValueEntry OptixModuleCompileBoundValueEntry
• typedef enum OptixPayloadTypeID OptixPayloadTypeID
• typedef enum OptixPayloadSemantics OptixPayloadSemantics
• typedef struct OptixPayloadType OptixPayloadType
• typedef struct OptixModuleCompileOptions OptixModuleCompileOptions
• typedef struct OptixBuiltinISOptions OptixBuiltinISOptions
• typedef enum OptixProgramGroupKind OptixProgramGroupKind
• typedef enum OptixProgramGroupFlags OptixProgramGroupFlags
• typedef struct OptixProgramGroupSingleModule OptixProgramGroupSingleModule
• typedef struct OptixProgramGroupHitgroup OptixProgramGroupHitgroup
• typedef struct OptixProgramGroupCallables OptixProgramGroupCallables
• typedef struct OptixProgramGroupDesc OptixProgramGroupDesc
• typedef struct OptixProgramGroupOptions OptixProgramGroupOptions
• typedef enum OptixExceptionCodes OptixExceptionCodes
• typedef enum OptixExceptionFlags OptixExceptionFlags
• typedef struct OptixPipelineCompileOptions OptixPipelineCompileOptions
• typedef struct OptixPipelineLinkOptions OptixPipelineLinkOptions
• typedef struct OptixShaderBindingTable OptixShaderBindingTable
• typedef struct OptixStackSizes OptixStackSizes
• typedef enum OptixDevicePropertyCoopVecFlags OptixDevicePropertyCoopVecFlags
• typedef enum OptixCoopVecElemType OptixCoopVecElemType
• typedef enum OptixCoopVecMatrixLayout OptixCoopVecMatrixLayout
• typedef struct OptixCoopVecMatrixDescription OptixCoopVecMatrixDescription
• typedef struct OptixNetworkDescription OptixNetworkDescription
• typedef enum OptixQueryFunctionTableOptions OptixQueryFunctionTableOptions
• typedef OptixResult() OptixQueryFunctionTable_t(int abiId, unsigned int numOptions,
OptixQueryFunctionTableOptions *, const void **, void *functionTable, size_t sizeOfTable)

## Enumerations

• enum OptixResult {
  OPTIX_SUCCESS = 0 ,
  OPTIX_ERROR_INVALID_VALUE = 7001 ,
  OPTIX_ERROR_HOST_OUT_OF_MEMORY = 7002 ,
  OPTIX_ERROR_INVALID_OPERATION = 7003 ,
  OPTIX_ERROR_FILE_IO_ERROR = 7004 ,
  OPTIX_ERROR_INVALID_FILE_FORMAT = 7005 ,
  OPTIX_ERROR_DISK_CACHE_INVALID_PATH = 7010 ,
  OPTIX_ERROR_DISK_CACHE_PERMISSION_ERROR = 7011 ,
  OPTIX_ERROR_DISK_CACHE_DATABASE_ERROR = 7012 ,

---

OPTIX_ERROR_DISK_CACHE_INVALID_DATA = 7013,
OPTIX_ERROR_LAUNCH_FAILURE = 7050,
OPTIX_ERROR_INVALID_DEVICE_CONTEXT = 7051,
OPTIX_ERROR_CUDA_NOT_INITIALIZED = 7052,
OPTIX_ERROR_VALIDATION_FAILURE = 7053,
OPTIX_ERROR_INVALID_INPUT = 7200,
OPTIX_ERROR_INVALID_LAUNCH_PARAMETER = 7201,
OPTIX_ERROR_INVALID_PAYLOAD_ACCESS = 7202,
OPTIX_ERROR_INVALID_ATTRIBUTE_ACCESS = 7203,
OPTIX_ERROR_INVALID_FUNCTION_USE = 7204,
OPTIX_ERROR_INVALID_FUNCTION_ARGUMENTS = 7205,
OPTIX_ERROR_PIPELINE_OUT_OF_CONSTANT_MEMORY = 7250,
OPTIX_ERROR_PIPELINE_LINK_ERROR = 7251,
OPTIX_ERROR_ILLEGAL_DURING_TASK_EXECUTE = 7270,
OPTIX_ERROR_CREATION_CANCELED = 7290,
OPTIX_ERROR_INTERNAL_COMPILER_ERROR = 7299,
OPTIX_ERROR_DENOISER_MODEL_NOT_SET = 7300,
OPTIX_ERROR_DENOISER_NOT_INITIALIZED = 7301,
OPTIX_ERROR_NOT_COMPATIBLE = 7400,
OPTIX_ERROR_PAYLOAD_TYPE_MISMATCH = 7500,
OPTIX_ERROR_PAYLOAD_TYPE_RESOLUTION_FAILED = 7501,
OPTIX_ERROR_PAYLOAD_TYPE_ID_INVALID = 7502,
OPTIX_ERROR_NOT_SUPPORTED = 7800,
OPTIX_ERROR_UNSUPPORTED_ABI_VERSION = 7801,
OPTIX_ERROR_FUNCTION_TABLE_SIZE_MISMATCH = 7802,
OPTIX_ERROR_INVALID_ENTRY_FUNCTION_OPTIONS = 7803,
OPTIX_ERROR_LIBRARY_NOT_FOUND = 7804,
OPTIX_ERROR_ENTRY_SYMBOL_NOT_FOUND = 7805,
OPTIX_ERROR_LIBRARY_UNLOAD_FAILURE = 7806,
OPTIX_ERROR_DEVICE_OUT_OF_MEMORY = 7807,
OPTIX_ERROR_INVALID_POINTER = 7808,
OPTIX_ERROR_SYMBOL_NOT_FOUND = 7809,
OPTIX_ERROR_CUDA_ERROR = 7900,
OPTIX_ERROR_INTERNAL_ERROR = 7990,
OPTIX_ERROR_UNKNOWN = 7999 }

## • enum OptixDeviceProperty {
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRACE_DEPTH = 0x2001,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRAVERSABLE_GRAPH_DEPTH = 0x2002,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_PRIMITIVES_PER_GAS = 0x2003,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCE_PER_IAS = 0x2004,
  OPTIX_DEVICE_PROPERTY_RTCORE_VERSION = 0x2005,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCE_ID = 0x2006,
  OPTIX_DEVICE_PROPERTY_LIMIT_NUM_BITS_INSTANCE_VISIBILITY_MASK = 0x2007,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_RECORDS_PER_GAS = 0x2008,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_OFFSET = 0x2009,
  OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING = 0x200A,
  OPTIX_DEVICE_PROPERTY_COOP_VEC = 0x200B,
  OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL = 0x2020,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_VERTICES = 0x2021,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_TRIANGLES = 0x2022,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_STRUCTURED_GRID_RESOLUTION = 0x2023,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_SBT_INDEX = 0x2024,
  OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTERS_PER_GAS = 0x2025}

---

• enum OptixDeviceContextValidationMode {
  OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_OFF = 0 ,
  OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_ALL = 0xFFFFFFF }
• enum OptixPipelineSymbolMemcpyKind {
  OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_DEVICE = 0x21A0 ,
  OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_HOST = 0x21A1 ,
  OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_DEVICE = 0x21A2 ,
  OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_HOST = 0x21A3 }
## • enum OptixDevicePropertyShaderExecutionReorderingFlags {
  OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING_FLAG_NONE = 0 ,
  OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING_FLAG_STANDARD = 1
  << 0 }
• enum OptixDevicePropertyClusterAccelFlags {
  OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_NONE = 0 ,
  OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_STANDARD = 1 << 0 }
## • enum OptixGeometryFlags {
  OPTIX_GEOMETRY_FLAG_NONE = 0 ,
  OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT = 1u << 0 ,
  OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL = 1u << 1 ,
  OPTIX_GEOMETRY_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1u << 2 }
## • enum OptixHitKind {
  OPTIX_HIT_KIND_TRIANGLE_FRONT_FACE = 0xFE ,
  OPTIX_HIT_KIND_TRIANGLE_BACK_FACE = 0xFF }
## • enum OptixIndicesFormat {
  OPTIX_INDICES_FORMAT_NONE = 0 ,
  OPTIX_INDICES_FORMAT_UNSIGNED_BYTE3 = 0x2101 ,
  OPTIX_INDICES_FORMAT_UNSIGNED_SHORT3 = 0x2102 ,
  OPTIX_INDICES_FORMAT_UNSIGNED_INT3 = 0x2103 }
## • enum OptixVertexFormat {
  OPTIX_VERTEX_FORMAT_NONE = 0 ,
  OPTIX_VERTEX_FORMAT_FLOAT3 = 0x2121 ,
  OPTIX_VERTEX_FORMAT_FLOAT2 = 0x2122 ,
  OPTIX_VERTEX_FORMAT_HALF3 = 0x2123 ,
  OPTIX_VERTEX_FORMAT_HALF2 = 0x2124 ,
  OPTIX_VERTEX_FORMAT_SNORM16_3 = 0x2125 ,
  OPTIX_VERTEX_FORMAT_SNORM16_2 = 0x2126 }
## • enum OptixTransformFormat {
  OPTIX_TRANSFORM_FORMAT_NONE = 0 ,
  OPTIX_TRANSFORM_FORMAT_MATRIX_FLOAT12 = 0x21E1 }
## • enum OptixOpacityMicromapFormat {
  OPTIX_OPACITY_MICROMAP_FORMAT_NONE = 0 ,
  OPTIX_OPACITY_MICROMAP_FORMAT_2_STATE = 1 ,
  OPTIX_OPACITY_MICROMAP_FORMAT_4_STATE = 2 }
## • enum OptixOpacityMicromapArrayIndexingMode {
  OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_NONE = 0 ,
  OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_LINEAR = 1 ,
  OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_INDEXED = 2 }
## • enum OptixPrimitiveType {
  OPTIX_PRIMITIVE_TYPE_CUSTOM = 0x2500 ,
  OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE = 0x2501 ,
  OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE = 0x2502 ,
  OPTIX_PRIMITIVE_TYPE_ROUND_LINEAR = 0x2503 ,

---

OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM = 0x2504 ,
OPTIX_PRIMITIVE_TYPE_FLAT_QUADRATIC_BSPLINE = 0x2505 ,
OPTIX_PRIMITIVE_TYPE_SPHERE = 0x2506 ,
OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER = 0x2507 ,
OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE_ROCAPS = 0x2508 ,
OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE_ROCAPS = 0x2509 ,
OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM_ROCAPS = 0x250A ,
OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER_ROCAPS = 0x250B ,
OPTIX_PRIMITIVE_TYPE_TRIANGLE = 0x2531 }

## • enum OptixPrimitiveTypeFlags {
    OPTIX_PRIMITIVE_TYPE_FLAGS_CUSTOM = 1 << 0 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE = 1 << 1 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE = 1 << 2 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_LINEAR = 1 << 3 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM = 1 << 4 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_FLAT_QUADRATIC_BSPLINE = 1 << 5 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_SPHERE = 1 << 6 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER = 1 << 7 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE_ROCAPS = 1 << 8 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE_ROCAPS = 1 << 9 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM_ROCAPS = 1 << 10 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER_ROCAPS = 1 << 11 ,
    OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE = 1 << 31 }
• enum OptixCurveEndcapFlags {
    OPTIX_CURVE_ENDCAP_DEFAULT = 0 ,
    OPTIX_CURVE_ENDCAP_ON = 1 << 0 }
• enum OptixBuildInputType {
    OPTIX_BUILD_INPUT_TYPE_TRIANGLES = 0x2141 ,
    OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES = 0x2142 ,
    OPTIX_BUILD_INPUT_TYPE_INSTANCES = 0x2143 ,
    OPTIX_BUILD_INPUT_TYPE_INSTANCE_POINTERS = 0x2144 ,
    OPTIX_BUILD_INPUT_TYPE_CURVES = 0x2145 ,
    OPTIX_BUILD_INPUT_TYPE_SPHERES = 0x2146 }
• enum OptixInstanceFlags {
    OPTIX_INSTANCE_FLAG_NONE = 0 ,
    OPTIX_INSTANCE_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1u << 0 ,
    OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING = 1u << 1 ,
    OPTIX_INSTANCE_FLAG_DISABLE_ANYHIT = 1u << 2 ,
    OPTIX_INSTANCE_FLAG_ENFORCE_ANYHIT = 1u << 3 ,
    OPTIX_INSTANCE_FLAG_FORCE_OPACITY_MICROMAP_2_STATE = 1u << 4 ,
    OPTIX_INSTANCE_FLAG_DISABLE_OPACITY_MICROMAPS = 1u << 5 }
• enum OptixBuildFlags {
    OPTIX_BUILD_FLAG_NONE = 0 ,
    OPTIX_BUILD_FLAG_ALLOW_UPDATE = 1u << 0 ,
    OPTIX_BUILD_FLAG_ALLOW_COMPACTION = 1u << 1 ,
    OPTIX_BUILD_FLAG_PREFER_FAST_TRACE = 1u << 2 ,
    OPTIX_BUILD_FLAG_PREFER_FAST_BUILD = 1u << 3 ,
    OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS = 1u << 4 ,
    OPTIX_BUILD_FLAG_ALLOW_RANDOM_INSTANCE_ACCESS = 1u << 5 ,
    OPTIX_BUILD_FLAG_ALLOW_OPACITY_MICROMAP_UPDATE = 1u << 6 ,
    OPTIX_BUILD_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS = 1u << 7 }
• enum OptixOpacityMicromapFlags {
    OPTIX_OPACITY_MICROMAP_FLAG_NONE = 0 ,

---

OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_TRACE = 1 << 0 ,
OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_BUILD = 1 << 1 }

## • enum OptixBuildOperation {
    OPTIX_BUILD_OPERATION_BUILD = 0x2161 ,
    OPTIX_BUILD_OPERATION_UPDATE = 0x2162 }
## • enum OptixMotionFlags {
    OPTIX_MOTION_FLAG_NONE = 0 ,
    OPTIX_MOTION_FLAG_START_VANISH = 1u << 0 ,
    OPTIX_MOTION_FLAG_END_VANISH = 1u << 1 }
## • enum OptixAccelPropertyType {
    OPTIX_PROPERTY_TYPE_COMPACTED_SIZE = 0x2181 ,
    OPTIX_PROPERTY_TYPE_AABBS = 0x2182 }
## • enum OptixTraversableType {
    OPTIX_TRAVERSABLE_TYPE_STATIC_TRANSFORM = 0x21C1 ,
    OPTIX_TRAVERSABLE_TYPE_MATRIX_MOTION_TRANSFORM = 0x21C2 ,
    OPTIX_TRAVERSABLE_TYPE_SRT_MOTION_TRANSFORM = 0x21C3 }
## • enum OptixClusterAccelBuildFlags {
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_NONE = 0 ,
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_TRACE = 1 << 0 ,
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_BUILD = 1 << 1 ,
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_ALLOW_OPACITY_MICROMAPS = 1 << 2 }
## • enum OptixClusterAccelClusterFlags {
    OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_NONE = 0 ,
    OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS = 1 << 0 }
## • enum OptixClusterAccelPrimitiveFlags {
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_NONE = 0 ,
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1 << 0 ,
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_REQUIRE_SINGLE_ANYHIT_CALL = 1 << 1 ,
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_ANYHIT = 1 << 2 }
## • enum OptixClusterAccelBuildType {
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS = 0x2545 ,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLES = 0x2546 ,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES = 0x2547 ,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TEMPLATES = 0x2548 ,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_GRIDS = 0x2549 }
## • enum OptixClusterAccelBuildMode {
    OPTIX_CLUSTER_ACCEL_BUILD_MODE_IMPLICIT_DESTINATIONS = 0 ,
    OPTIX_CLUSTER_ACCEL_BUILD_MODE_EXPLICIT_DESTINATIONS = 1 ,
    OPTIX_CLUSTER_ACCEL_BUILD_MODE_GET_SIZES = 2 }
## • enum OptixClusterAccelIndicesFormat {
    OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_8BIT = 1 ,
    OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_16BIT = 2 ,
    OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_32BIT = 4 }
• enum OptixClusterIDValues { OPTIX_CLUSTER_ID_INVALID = 0xFFFFFFFu }
## • enum OptixPixelFormat {
    OPTIX_PIXEL_FORMAT_HALF1 = 0x220a ,
    OPTIX_PIXEL_FORMAT_HALF2 = 0x2207 ,
    OPTIX_PIXEL_FORMAT_HALF3 = 0x2201 ,
    OPTIX_PIXEL_FORMAT_HALF4 = 0x2202 ,
    OPTIX_PIXEL_FORMAT_FLOAT1 = 0x220b ,

---

OPTIX_PIXEL_FORMAT_FLOAT2 = 0x2208 ,
OPTIX_PIXEL_FORMAT_FLOAT3 = 0x2203 ,
OPTIX_PIXEL_FORMAT_FLOAT4 = 0x2204 ,
OPTIX_PIXEL_FORMAT_UCHAR3 = 0x2205 ,
OPTIX_PIXEL_FORMAT_UCHAR4 = 0x2206 ,
OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_LAYER = 0x2209 }

## • enum OptixDenoiserModelKind {
OPTIX_DENOISER_MODEL_KIND_AOV = 0x2324 ,
OPTIX_DENOISER_MODEL_KIND_TEMPORAL_AOV = 0x2326 ,
OPTIX_DENOISER_MODEL_KIND_UPSCALE2X = 0x2327 ,
OPTIX_DENOISER_MODEL_KIND_TEMPORAL_UPSCALE2X = 0x2328 ,
OPTIX_DENOISER_MODEL_KIND_LDR = 0x2322 ,
OPTIX_DENOISER_MODEL_KIND_HDR = 0x2323 ,
OPTIX_DENOISER_MODEL_KIND_TEMPORAL = 0x2325 }

## • enum OptixDenoiserAlphaMode {
OPTIX_DENOISER_ALPHA_MODE_COPY = 0 ,
OPTIX_DENOISER_ALPHA_MODE_DENOISE = 1 }

• enum OptixDenoiserAOVType {
OPTIX_DENOISER_AOV_TYPE_NONE = 0 ,
OPTIX_DENOISER_AOV_TYPE_BEAUTY = 0x7000 ,
OPTIX_DENOISER_AOV_TYPE_SPECULAR = 0x7001 ,
OPTIX_DENOISER_AOV_TYPE_REFLECTION = 0x7002 ,
OPTIX_DENOISER_AOV_TYPE_REFRACTION = 0x7003 ,
OPTIX_DENOISER_AOV_TYPE_DIFFUSE = 0x7004 }

• enum OptixRayFlags {
OPTIX_RAY_FLAG_NONE = 0u ,
OPTIX_RAY_FLAG_DISABLE_ANYHIT = 1u << 0 ,
OPTIX_RAY_FLAG_ENFORCE_ANYHIT = 1u << 1 ,
OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT = 1u << 2 ,
OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT = 1u << 3 ,
OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES = 1u << 4 ,
OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES = 1u << 5 ,
OPTIX_RAY_FLAG_CULL_DISABLED_ANYHIT = 1u << 6 ,
OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT = 1u << 7 ,
OPTIX_RAY_FLAG_FORCE_OPACITY_MICROMAP_2_STATE = 1u << 10 }

## • enum OptixTransformType {
OPTIX_TRANSFORM_TYPE_NONE = 0 ,
OPTIX_TRANSFORM_TYPE_STATIC_TRANSFORM = 1 ,
OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_TRANSFORM = 2 ,
OPTIX_TRANSFORM_TYPE_SRT_MOTION_TRANSFORM = 3 ,
OPTIX_TRANSFORM_TYPE_INSTANCE = 4 }

• enum OptixTraversableGraphFlags {
OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_ANY = 0 ,
OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS = 1u << 0 ,
OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING = 1u << 1 }

• enum OptixCompileOptimizationLevel {
OPTIX_COMPILE_OPTIMIZATION_DEFAULT = 0 ,
OPTIX_COMPILE_OPTIMIZATION_LEVEL_0 = 0x2340 ,
OPTIX_COMPILE_OPTIMIZATION_LEVEL_1 = 0x2341 ,
OPTIX_COMPILE_OPTIMIZATION_LEVEL_2 = 0x2342 ,
OPTIX_COMPILE_OPTIMIZATION_LEVEL_3 = 0x2343 }

• enum OptixCompileDebugLevel {
OPTIX_COMPILE_DEBUG_LEVEL_DEFAULT = 0 ,

---

OPTIX_COMPILE_DEBUG_LEVEL_NONE = 0x2350 ,
OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL = 0x2351 ,
OPTIX_COMPILE_DEBUG_LEVEL_MODERATE = 0x2353 ,
OPTIX_COMPILE_DEBUG_LEVEL_FULL = 0x2352 }

• enum OptixModuleCompileState {
    OPTIX_MODULE_COMPILE_STATE_NOT_STARTED = 0x2360 ,
    OPTIX_MODULE_COMPILE_STATE_STARTED = 0x2361 ,
    OPTIX_MODULE_COMPILE_STATE_IMPENDING_FAILURE = 0x2362 ,
    OPTIX_MODULE_COMPILE_STATE_FAILED = 0x2363 ,
    OPTIX_MODULE_COMPILE_STATE_COMPLETED = 0x2364 }

• enum OptixCreationFlags {
    OPTIX_CREATION_FLAG_NONE = 0 ,
    OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE = 1 << 0 }

• enum OptixPayloadTypeID {
    OPTIX_PAYLOAD_TYPE_DEFAULT = 0 ,
    OPTIX_PAYLOAD_TYPE_ID_0 = (1 << 0u) ,
    OPTIX_PAYLOAD_TYPE_ID_1 = (1 << 1u) ,
    OPTIX_PAYLOAD_TYPE_ID_2 = (1 << 2u) ,
    OPTIX_PAYLOAD_TYPE_ID_3 = (1 << 3u) ,
    OPTIX_PAYLOAD_TYPE_ID_4 = (1 << 4u) ,
    OPTIX_PAYLOAD_TYPE_ID_5 = (1 << 5u) ,
    OPTIX_PAYLOAD_TYPE_ID_6 = (1 << 6u) ,
    OPTIX_PAYLOAD_TYPE_ID_7 = (1 << 7u) }

• enum OptixPayloadSemantics {
    OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_NONE = 0 ,
    OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ = 1u << 0 ,
    OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_WRITE = 2u << 0 ,
    OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ_WRITE = 3u << 0 ,
    OPTIX_PAYLOAD_SEMANTICS_CH_NONE = 0 ,
    OPTIX_PAYLOAD_SEMANTICS_CH_READ = 1u << 2 ,
    OPTIX_PAYLOAD_SEMANTICS_CH_WRITE = 2u << 2 ,
    OPTIX_PAYLOAD_SEMANTICS_CH_READ_WRITE = 3u << 2 ,
    OPTIX_PAYLOAD_SEMANTICS_MS_NONE = 0 ,
    OPTIX_PAYLOAD_SEMANTICS_MS_READ = 1u << 4 ,
    OPTIX_PAYLOAD_SEMANTICS_MS_WRITE = 2u << 4 ,
    OPTIX_PAYLOAD_SEMANTICS_MS_READ_WRITE = 3u << 4 ,
    OPTIX_PAYLOAD_SEMANTICS_AH_NONE = 0 ,
    OPTIX_PAYLOAD_SEMANTICS_AH_READ = 1u << 6 ,
    OPTIX_PAYLOAD_SEMANTICS_AH_WRITE = 2u << 6 ,
    OPTIX_PAYLOAD_SEMANTICS_AH_READ_WRITE = 3u << 6 ,
    OPTIX_PAYLOAD_SEMANTICS_IS_NONE = 0 ,
    OPTIX_PAYLOAD_SEMANTICS_IS_READ = 1u << 8 ,
    OPTIX_PAYLOAD_SEMANTICS_IS_WRITE = 2u << 8 ,
    OPTIX_PAYLOAD_SEMANTICS_IS_READ_WRITE = 3u << 8 }

• enum OptixProgramGroupKind {
    OPTIX_PROGRAM_GROUP_KIND_RAYGEN = 0x2421 ,
    OPTIX_PROGRAM_GROUP_KIND_MISS = 0x2422 ,
    OPTIX_PROGRAM_GROUP_KIND_EXCEPTION = 0x2423 ,
    OPTIX_PROGRAM_GROUP_KIND_HITGROUP = 0x2424 ,
    OPTIX_PROGRAM_GROUP_KIND_CALLABLES = 0x2425 }

• enum OptixProgramGroupFlags { OPTIX_PROGRAM_GROUP_FLAGS_NONE = 0 }

• enum OptixExceptionCodes {
    OPTIX_EXCEPTION_CODE_STACK_OVERFLOW = -1 ,

---

OPTIX_EXCEPTION_CODE_TRACE_DEPTH_EXCEEDED= -2 }
•enumOptixExceptionFlags{
OPTIX_EXCEPTION_FLAG_NONE= 0 ,
OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW= 1u << 0 ,
OPTIX_EXCEPTION_FLAG_TRACE_DEPTH= 1u << 1 ,
OPTIX_EXCEPTION_FLAG_USER= 1u << 2 }
•enumOptixDevicePropertyCoopVecFlags{
OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_NONE= 0 ,
OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_STANDARD= 1 << 0 }
•enumOptixCoopVecElemType{
OPTIX_COOP_VEC_ELEM_TYPE_UNKNOWN= 0x2A00 ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT16= 0x2A01 ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT32= 0x2A03 ,
OPTIX_COOP_VEC_ELEM_TYPE_UINT8= 0x2A04 ,
OPTIX_COOP_VEC_ELEM_TYPE_INT8= 0x2A05 ,
OPTIX_COOP_VEC_ELEM_TYPE_UINT32= 0x2A08 ,
OPTIX_COOP_VEC_ELEM_TYPE_INT32= 0x2A09 ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E4M3= 0x2A0A ,
OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E5M2= 0x2A0B }
•enumOptixCoopVecMatrixLayout{
OPTIX_COOP_VEC_MATRIX_LAYOUT_ROW_MAJOR= 0x2A40 ,
OPTIX_COOP_VEC_MATRIX_LAYOUT_COLUMN_MAJOR= 0x2A41 ,
OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_OPTIMAL= 0x2A42 ,
OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL= 0x2A43 }
•enumOptixQueryFunctionTableOptions{OPTIX_QUERY_FUNCTION_TABLE_OPTION_
DUMMY= 0 }

## 8.25.1 Detailed Description

OptiX public API header.

## Author

## NVIDIA Corporation

OptiX types include file – defines types and enums used by the API.

8.26 optix_types.h

Go to the documentation of this file.

1
2/*
3* SPDX-FileCopyrightText: Copyright (c) 2019-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
4* SPDX-License-Identifier: LicenseRef-NvidiaProprietary
5*
6* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
7* property and proprietary rights in and to this material, related
8* documentation and any modifications thereto. Any use, reproduction,
9* disclosure or distribution of this material and related documentation
10* without an express license agreement from NVIDIA CORPORATION or
11* its affiliates is strictly prohibited.
12*/
19
20#ifndef OPTIX_OPTIX_TYPES_H
21#define OPTIX_OPTIX_TYPES_H
22
23#if!defined(__CUDACC_RTC__)

---

24#include <stddef.h>/* for size_t */
25#endif
26
27#ifdef NV_MODULE_OPTIX
28// This is a mechanism to include <g_nvconfig.h> in driver builds only and translate any nvconfig macro to
a custom OPTIX-specific macro, that can also be used in SDK builds/installs
29#include <exp/misc/optix_nvconfig_translate.h>// includes <g_nvconfig.h>
30#endif// NV_MODULE_OPTIX
31
32
35
40// This typedef should match the one in cuda.h in order to avoid compilation errors.
41#if defined(_WIN64) || defined(__LP64__)
43typedefunsignedlonglongCUdeviceptr;
44#else
46typedefunsignedintCUdeviceptr;
47#endif
48
50typedefstructOptixDeviceContext_t*OptixDeviceContext;
51
53typedefstructOptixModule_t*OptixModule;
54
56typedefstructOptixProgramGroup_t*OptixProgramGroup;
57
59typedefstructOptixPipeline_t*OptixPipeline;
60
62typedefstructOptixDenoiser_t*OptixDenoiser;
63
65typedefstructOptixTask_t*OptixTask;
66
68typedefunsignedlonglongOptixTraversableHandle;
69
71typedefunsignedintOptixVisibilityMask;
72
74#define OPTIX_SBT_RECORD_HEADER_SIZE ((size_t)32)
75
77#define OPTIX_SBT_RECORD_ALIGNMENT 16ull
78
80#define OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT 128ull
81
83#define OPTIX_INSTANCE_BYTE_ALIGNMENT 16ull
84
86#define OPTIX_AABB_BUFFER_BYTE_ALIGNMENT 8ull
87
89#define OPTIX_GEOMETRY_TRANSFORM_BYTE_ALIGNMENT 16ull
90
92#define OPTIX_TRANSFORM_BYTE_ALIGNMENT 64ull
93
95#define OPTIX_OPACITY_MICROMAP_DESC_BUFFER_BYTE_ALIGNMENT 8ull
96
98#define OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT 0
99
101#define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_TYPE_COUNT 8
102
104#define OPTIX_COMPILE_DEFAULT_MAX_PAYLOAD_VALUE_COUNT 32
105
108#define OPTIX_OPACITY_MICROMAP_STATE_TRANSPARENT (0)
109#define OPTIX_OPACITY_MICROMAP_STATE_OPAQUE (1)
110#define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_TRANSPARENT (2)
111#define OPTIX_OPACITY_MICROMAP_STATE_UNKNOWN_OPAQUE (3)
112
115#define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_TRANSPARENT (-1)
116#define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_OPAQUE (-2)
117#define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_TRANSPARENT (-3)
118#define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_FULLY_UNKNOWN_OPAQUE (-4)
123#define OPTIX_OPACITY_MICROMAP_PREDEFINED_INDEX_CLUSTER_SKIP_OPACITY_MICROMAP (-5)
124

---

| typedef enum OptixDeviceProperty |
| --- |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRACE_DEPTH = 0x2001, |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_TRAVERSABLE_GRAPH_DEPTH = 0x2002, |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_PRIMITIVES_PER_GAS = 0x2003, |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCES_PER_IAS = 0x2004, |
| OPTIX_DEVICE_PROPERTY_RTCORE_VERSION = 0x2005, |
| OPTIX_DEVICE_PROPERTY_LIMIT_MAX_INSTANCE_ID = 0x2006, |
| OPTIX_DEVICE_PROPERTY_LIMIT_NUM_BITS_INSTANCE_VISIBILITY_MASK = 0x2007, |

---

OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_RECORDS_PER_GAS = 0x2008,

OPTIX_DEVICE_PROPERTY_LIMIT_MAX_SBT_OFFSET = 0x2009,

OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING = 0x200A,

OPTIX_DEVICE_PROPERTY_COOP_VEC = 0x200B,

OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL = 0x2020,

OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_VERTICES = 0x2021,

OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_TRIANGLES = 0x2022,

OPTIX_DEVICE_PROPERTY_LIMIT_MAX_STRUCTURED_GRID_RESOLUTION = 0x2023,

OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTER_SBT_INDEX = 0x2024,

OPTIX_DEVICE_PROPERTY_LIMIT_MAX_CLUSTERS_PER_GAS = 0x2025,
} OptixDeviceProperty;

typedef void (*OptixLogCallback)(unsigned int level, const char* tag, const char* message, void* cbdata);

typedef enum OptixDeviceContextValidationMode
{
    OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_OFF = 0,
    OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_ALL = 0xFFFFFFFF
} OptixDeviceContextValidationMode;

typedef struct OptixDeviceContextOptions
{
    OptixLogCallback logCallbackFunction;
    void* logCallbackData;
    int logCallbackLevel;
    OptixDeviceContextValidationMode validationMode;
} OptixDeviceContextOptions;

typedef enum OptixPipelineSymbolMemcpyKind
{
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_DEVICE = 0x21A0,
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_FROM_HOST   = 0x21A1,
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_DEVICE  = 0x21A2,
    OPTIX_PIPELINE_SYMBOL_MEMCPY_KIND_TO_HOST     = 0x21A3,
} OptixPipelineSymbolMemcpyKind;

typedef enum OptixDevicePropertyShaderExecutionReorderingFlags
{
    OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING_FLAG_NONE      = 0,

    // Standard thread reordering is supported
    OPTIX_DEVICE_PROPERTY_SHADER_EXECUTION_REORDERING_FLAG_STANDARD = 1 « 0,
} OptixDevicePropertyShaderExecutionReorderingFlags;

typedef enum OptixDevicePropertyClusterAccelFlags
{
    OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_NONE      = 0,

    // Cluster acceleration structure builds are supported.
    OPTIX_DEVICE_PROPERTY_CLUSTER_ACCEL_FLAG_STANDARD = 1 « 0,
} OptixDevicePropertyClusterAccelFlags;

typedef enum OptixGeometryFlags
{
    OPTIX_GEOMETRY_FLAG_NONE = 0,

    OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT = 1u « 0,
}

NVIDIA OptiX 9.1 API

---

OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL = 1u « 1,

OPTIX_GEOMETRY_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1u « 2,
} OptixGeometryFlags;

typedef enum OptixHitKind
{
    OPTIX_HIT_KIND_TRIANGLE_FRONT_FACE = 0xFE,
    OPTIX_HIT_KIND_TRIANGLE_BACK_FACE = 0xFF
} OptixHitKind;

typedef enum OptixIndicesFormat
{
    OPTIX_INDICES_FORMAT_NONE = 0,
    OPTIX_INDICES_FORMAT_UNSIGNED_BYTE3 = 0x2101,
    OPTIX_INDICES_FORMAT_UNSIGNED_SHORT3 = 0x2102,
    OPTIX_INDICES_FORMAT_UNSIGNED_INT3 = 0x2103
} OptixIndicesFormat;

typedef enum OptixVertexFormat
{
    OPTIX_VERTEX_FORMAT_NONE        = 0,
    OPTIX_VERTEX_FORMAT_FLOAT3      = 0x2121,
    OPTIX_VERTEX_FORMAT_FLOAT2      = 0x2122,
    OPTIX_VERTEX_FORMAT_HALF3       = 0x2123,
    OPTIX_VERTEX_FORMAT_HALF2       = 0x2124,
    OPTIX_VERTEX_FORMAT_SNORM16_3   = 0x2125,
    OPTIX_VERTEX_FORMAT_SNORM16_2   = 0x2126
} OptixVertexFormat;

typedef enum OptixTransformFormat
{
    OPTIX_TRANSFORM_FORMAT_NONE          = 0,
    OPTIX_TRANSFORM_FORMAT_MATRIX_FLOAT12 = 0x21E1,
} OptixTransformFormat;

typedef enum OptixOpacityMicromapFormat
{
    OPTIX_OPACITY_MICROMAP_FORMAT_NONE = 0,
    OPTIX_OPACITY_MICROMAP_FORMAT_2_STATE = 1,
    OPTIX_OPACITY_MICROMAP_FORMAT_4_STATE = 2,
} OptixOpacityMicromapFormat;

typedef enum OptixOpacityMicromapArrayIndexingMode
{
    OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_NONE = 0,
    OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_LINEAR = 1,
    OPTIX_OPACITY_MICROMAP_ARRAY_INDEXING_MODE_INDEXED = 2,
} OptixOpacityMicromapArrayIndexingMode;

typedef struct OptixOpacityMicromapUsageCount
{
    unsigned int count;
    unsigned int subdivisionLevel;
    OptixOpacityMicromapFormat format;
} OptixOpacityMicromapUsageCount;

typedef struct OptixBuildInputOpacityMicromap
{
    OptixOpacityMicromapArrayIndexingMode indexingMode;

    CUdeviceptr opacityMicromapArray;

    CUdeviceptr indexBuffer;

    unsigned int indexSizeInBytes;

---

unsigned int indexStrideInBytes;

unsigned int indexOffset;

unsigned int numMicromapUsageCounts;
const OptixOpacityMicromapUsageCount* micromapUsageCounts;
} OptixBuildInputOpacityMicromap;

typedef struct OptixRelocateInputOpacityMicromap
{
    CUdeviceptr  opacityMicromapArray;
} OptixRelocateInputOpacityMicromap;


typedef struct OptixBuildInputTriangleArray
{
    const CUdeviceptr* vertexBuffers;

    unsigned int numVertices;

    OptixVertexFormat vertexFormat;

    unsigned int vertexStrideInBytes;

    CUdeviceptr indexBuffer;

    unsigned int numIndexTriplets;

    OptixIndicesFormat indexFormat;

    unsigned int indexStrideInBytes;

    CUdeviceptr preTransform;

    const unsigned int* flags;

    unsigned int numSbtRecords;

    CUdeviceptr sbtIndexOffsetBuffer;

    unsigned int sbtIndexOffsetSizeInBytes;

    unsigned int sbtIndexOffsetStrideInBytes;

    unsigned int primitiveIndexOffset;

    OptixTransformFormat transformFormat;

    OptixBuildInputOpacityMicromap opacityMicromap;

} OptixBuildInputTriangleArray;

typedef struct OptixRelocateInputTriangleArray
{
    unsigned int numSbtRecords;

    OptixRelocateInputOpacityMicromap opacityMicromap;
} OptixRelocateInputTriangleArray;

typedef enum OptixPrimitiveType
{
    OPTIX_PRIMITIVE_TYPE_CUSTOM = 0x2500,
    OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE = 0x2501,
    OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE = 0x2502,
    OPTIX_PRIMITIVE_TYPE_ROUND_LINEAR = 0x2503,

---

OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM = 0x2504,
OPTIX_PRIMITIVE_TYPE_FLAT_QUADRATIC_BSPLINE = 0x2505,
OPTIX_PRIMITIVE_TYPE_SPHERE = 0x2506,
OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER = 0x2507,
OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE_ROCAPS = 0x2508,
OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE_ROCAPS = 0x2509,
OPTIX_PRIMITIVE_TYPE_ROUND_CATMULLROM_ROCAPS = 0x250A,
OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BEZIER_ROCAPS = 0x250B,
OPTIX_PRIMITIVE_TYPE_TRIANGLE = 0x2531,
} OptixPrimitiveType;

typedef enum OptixPrimitiveTypeFlags
{
    OPTIX_PRIMITIVE_TYPE_FLAGS_CUSTOM = 1 « 0,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE = 1 « 1,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE = 1 « 2,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_LINEAR = 1 « 3,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM = 1 « 4,
    OPTIX_PRIMITIVE_TYPE_FLAGS_FLAT_QUADRATIC_BSPLINE = 1 « 5,
    OPTIX_PRIMITIVE_TYPE_FLAGS_SPHERE = 1 « 6,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER = 1 « 7,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_QUADRATIC_BSPLINE_ROCAPS = 1 « 8,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BSPLINE_ROCAPS = 1 « 9,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CATMULLROM_ROCAPS = 1 « 10,
    OPTIX_PRIMITIVE_TYPE_FLAGS_ROUND_CUBIC_BEZIER_ROCAPS = 1 « 11,
    OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE = 1 « 31,
} OptixPrimitiveTypeFlags;

typedef enum OptixCurveEndcapFlags
{
    OPTIX_CURVE_ENDCAP_DEFAULT = 0,
    OPTIX_CURVE_ENDCAP_ON = 1 « 0,
} OptixCurveEndcapFlags;

typedef struct OptixBuildInputCurveArray
{
    OptixPrimitiveType curveType;
    unsigned int numPrimitives;

    const CUdeviceptr* vertexBuffers;
    unsigned int numVertices;
    unsigned int vertexStrideInBytes;

    const CUdeviceptr* widthBuffers;
    unsigned int widthStrideInBytes;

    const CUdeviceptr* normalBuffers;
    unsigned int normalStrideInBytes;

    CUdeviceptr indexBuffer;
    unsigned int indexStrideInBytes;

    unsigned int flag;

    unsigned int primitiveIndexOffset;

    unsigned int endcapFlags;
} OptixBuildInputCurveArray;

typedef struct OptixBuildInputSphereArray
{
    const CUdeviceptr* vertexBuffers;

    unsigned int vertexStrideInBytes;
    unsigned int numVertices;

    const CUdeviceptr* radiusBuffers;

---

unsigned int radiusStrideInBytes;
int singleRadius;

const unsigned int* flags;

unsigned int numSbtRecords;
CUdeviceptr sbtIndexOffsetBuffer;
unsigned int sbtIndexOffsetSizeInBytes;
unsigned int sbtIndexOffsetStrideInBytes;

unsigned int primitiveIndexOffset;
} OptixBuildInputSphereArray;

typedef struct OptixAabb
{
    float minX;
    float minY;
    float minZ;
    float maxX;
    float maxY;
    float maxZ;
} OptixAabb;

typedef struct OptixBuildInputCustomPrimitiveArray
{
    const CUdeviceptr* aabbBuffers;

    unsigned int numPrimitives;

    unsigned int strideInBytes;

    const unsigned int* flags;

    unsigned int numSbtRecords;

    CUdeviceptr sbtIndexOffsetBuffer;

    unsigned int sbtIndexOffsetSizeInBytes;

    unsigned int sbtIndexOffsetStrideInBytes;

    unsigned int primitiveIndexOffset;
} OptixBuildInputCustomPrimitiveArray;

typedef struct OptixBuildInputInstanceArray
{
    CUdeviceptr instances;

    unsigned int numInstances;

    unsigned int instanceStride;
} OptixBuildInputInstanceArray;

typedef struct OptixRelocateInputInstanceArray
{
    unsigned int numInstances;

    CUdeviceptr traversableHandles;

} OptixRelocateInputInstanceArray;

typedef enum OptixBuildInputType
{
    OPTIX_BUILD_INPUT_TYPE_TRIANGLES = 0x2141,
    OPTIX_BUILD_INPUT_TYPE_CUSTOM_PRIMITIVES = 0x2142,
    OPTIX_BUILD_INPUT_TYPE_INSTANCES = 0x2143,
    OPTIX_BUILD_INPUT_TYPE_INSTANCE_POINTERS = 0x2144,

---

OPTIX_BUILD_INPUT_TYPE_CURVES = 0x2145,
OPTIX_BUILD_INPUT_TYPE_SPHERES = 0x2146
} OptixBuildInputType;

typedef struct OptixBuildInput
{
    OptixBuildInputType type;

    union
    {
        char pad[1024];
        OptixBuildInputTriangleArray triangleArray;
        OptixBuildInputCurveArray curveArray;
        OptixBuildInputSphereArray sphereArray;
        OptixBuildInputCustomPrimitiveArray customPrimitiveArray;
        OptixBuildInputInstanceArray instanceArray;
    };
} OptixBuildInput;

typedef struct OptixRelocateInput
{
    OptixBuildInputType type;

    union
    {
        OptixRelocateInputInstanceArray instanceArray;

        OptixRelocateInputTriangleArray triangleArray;
    };
} OptixRelocateInput;

typedef enum OptixInstanceFlags
{
    OPTIX_INSTANCE_FLAG_NONE = 0,

    OPTIX_INSTANCE_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1u « 0,

    OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING = 1u « 1,

    OPTIX_INSTANCE_FLAG_DISABLE_ANYHIT = 1u « 2,

    OPTIX_INSTANCE_FLAG_ENFORCE_ANYHIT = 1u « 3,

    OPTIX_INSTANCE_FLAG_FORCE_OPACITY_MICROMAP_2_STATE = 1u « 4,
    OPTIX_INSTANCE_FLAG_DISABLE_OPACITY_MICROMAPS = 1u « 5,
}

OptixInstanceFlags;

typedef struct OptixInstance
{
    float transform[12];

    unsigned int instanceId;

    unsigned int sbtOffset;

    unsigned int visibilityMask;

    unsigned int flags;

    OptixTraversableHandle traversableHandle;

    unsigned int pad[2];
} OptixInstance;

---

typedef enum OptixBuildFlags
{
    OPTIX_BUILD_FLAG_NONE = 0,

    OPTIX_BUILD_FLAG_ALLOW_UPDATE = 1u « 0,

    OPTIX_BUILD_FLAG_ALLOW_COMPACTION = 1u « 1,

    OPTIX_BUILD_FLAG_PREFER_FAST_TRACE = 1u « 2,

    OPTIX_BUILD_FLAG_PREFER_FAST_BUILD = 1u « 3,

    OPTIX_BUILD_FLAG_ALLOW_RANDOM_VERTEX_ACCESS = 1u « 4,

    OPTIX_BUILD_FLAG_ALLOW_RANDOM_INSTANCE_ACCESS = 1u « 5,

    OPTIX_BUILD_FLAG_ALLOW_OPACITY_MICROMAP_UPDATE = 1u « 6,

    OPTIX_BUILD_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS = 1u « 7,
} OptixBuildFlags;

typedef enum OptixOpacityMicromapFlags
{
    OPTIX_OPACITY_MICROMAP_FLAG_NONE = 0,

    OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_TRACE = 1 « 0,

    OPTIX_OPACITY_MICROMAP_FLAG_PREFER_FAST_BUILD = 1 « 1,
} OptixOpacityMicromapFlags;

typedef struct OptixOpacityMicromapDesc
{
    unsigned int byteOffset;
    unsigned short subdivisionLevel;
    unsigned short format;
} OptixOpacityMicromapDesc;

typedef struct OptixOpacityMicromapHistogramEntry
{
    unsigned int count;
    unsigned int subdivisionLevel;
    OptixOpacityMicromapFormat format;
} OptixOpacityMicromapHistogramEntry;

typedef struct OptixOpacityMicromapArrayBuildInput
{
    unsigned int flags;

    CUdeviceptr inputBuffer;

    CUdeviceptr perMicromapDescBuffer;

    unsigned int perMicromapStrideInBytes;

    unsigned int numMicromapHistogramEntries;
    const OptixOpacityMicromapHistogramEntry* micromapHistogramEntries;
} OptixOpacityMicromapArrayBuildInput;

typedef struct OptixMicromapBufferSizes
{
    size_t outputSizeInBytes;
    size_t tempSizeInBytes;
} OptixMicromapBufferSizes;

typedef struct OptixMicromapBuffers
{

---

CUdeviceptr output;
size_t outputSizeInBytes;
CUdeviceptr temp;
size_t tempSizeInBytes;
} OptixMicromapBuffers;

typedef enum OptixBuildOperation
{
    OPTIX_BUILD_OPERATION_BUILD = 0x2161,
    OPTIX_BUILD_OPERATION_UPDATE = 0x2162,
} OptixBuildOperation;

typedef enum OptixMotionFlags
{
    OPTIX_MOTION_FLAG_NONE          = 0,
    OPTIX_MOTION_FLAG_START_VANISH = 1u « 0,
    OPTIX_MOTION_FLAG_END_VANISH   = 1u « 1
} OptixMotionFlags;

typedef struct OptixMotionOptions
{
    unsigned short numKeys;

    unsigned short flags;

    float timeBegin;

    float timeEnd;
} OptixMotionOptions;

typedef struct OptixAccelBuildOptions
{
    unsigned int buildFlags;

    OptixBuildOperation operation;

    OptixMotionOptions motionOptions;
} OptixAccelBuildOptions;

typedef struct OptixAccelBufferSizes
{
    size_t outputSizeInBytes;

    size_t tempSizeInBytes;

    size_t tempUpdateSizeInBytes;
} OptixAccelBufferSizes;

typedef enum OptixAccelPropertyType
{
    OPTIX_PROPERTY_TYPE_COMPACTED_SIZE = 0x2181,

    OPTIX_PROPERTY_TYPE_AABBS = 0x2182,
} OptixAccelPropertyType;

typedef struct OptixAccelEmitDesc
{
    CUdeviceptr result;

    OptixAccelPropertyType type;
} OptixAccelEmitDesc;

typedef struct OptixRelocationInfo
{
    unsigned long long info[4];
} OptixRelocationInfo;

---

typedef struct OptixStaticTransform
{
    OptixTraversableHandle child;

    unsigned int pad[2];

    float transform[12];

    float invTransform[12];
} OptixStaticTransform;

typedef struct OptixMatrixMotionTransform
{
    OptixTraversableHandle child;

    OptixMotionOptions motionOptions;

    unsigned int pad[3];

    float transform[2][12];
} OptixMatrixMotionTransform;

//     [ sx   a   b  pvx ]
// S = [ 0 sy   c  pvy ]
//     [ 0 0 sz  pvz ]
//     [ 1 0 0 tx ]
// T = [ 0 1 0 ty ]
//     [ 0 0 1 tz ]

typedef struct OptixSRTData
{
    float sx, a, b, pvx, sy, c, pvy, sz, pvz, qx, qy, qz, qw, tx, ty, tz;
} OptixSRTData;


typedef struct OptixSRTMotionTransform
{
    OptixTraversableHandle child;

    OptixMotionOptions motionOptions;

    unsigned int pad[3];

    OptixSRTData srtData[2];
} OptixSRTMotionTransform;

typedef enum OptixTraversableType
{
    OPTIX_TRAVERSABLE_TYPE_STATIC_TRANSFORM = 0x21C1,
    OPTIX_TRAVERSABLE_TYPE_MATRIX_MOTION_TRANSFORM = 0x21C2,
    OPTIX_TRAVERSABLE_TYPE_SRT_MOTION_TRANSFORM = 0x21C3,
} OptixTraversableType;


typedef enum OptixClusterAccelBuildFlags
{
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_NONE             = 0,
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_TRACE      = 1 « 0,
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_PREFER_FAST_BUILD      = 1 « 1,
    OPTIX_CLUSTER_ACCEL_BUILD_FLAG_ALLOW_OPACITY_MICROMAPS = 1 « 2
} OptixClusterAccelBuildFlags;

typedef enum OptixClusterAccelClusterFlags
{
    OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_NONE             = 0,
    OPTIX_CLUSTER_ACCEL_CLUSTER_FLAG_ALLOW_DISABLE_OPACITY_MICROMAPS = 1 « 0,

NVIDIA OptiX 9.1 API

---

} OptixClusterAccelClusterFlags;

typedef enum OptixClusterAccelPrimitiveFlags
{
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_NONE = 0,
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_TRIANGLE_FACE_CULLING = 1 « 0,
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_REQUIRE_SINGLE_ANYHIT_CALL = 1 « 1,
    OPTIX_CLUSTER_ACCEL_PRIMITIVE_FLAG_DISABLE_ANYHIT = 1 « 2,
} OptixClusterAccelPrimitiveFlags;

typedef enum OptixClusterAccelBuildType
{
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_GASES_FROM_CLUSTERS = 0x2545,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TRIANGLES = 0x2546,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_TRIANGLES = 0x2547,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_CLUSTERS_FROM_TEMPLATES = 0x2548,
    OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_GRIDS = 0x2549
} OptixClusterAccelBuildType;

typedef enum OptixClusterAccelBuildMode
{
    OPTIX_CLUSTER_ACCEL_BUILD_MODE_IMPLICIT_DESTINATIONS = 0,
    OPTIX_CLUSTER_ACCEL_BUILD_MODE_EXPLICIT_DESTINATIONS = 1,
    OPTIX_CLUSTER_ACCEL_BUILD_MODE_GET_SIZES = 2
} OptixClusterAccelBuildMode;

typedef enum OptixClusterAccelIndicesFormat
{
    OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_8BIT = 1,
    OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_16BIT = 2,
    OPTIX_CLUSTER_ACCEL_INDICES_FORMAT_32BIT = 4,
} OptixClusterAccelIndicesFormat;

typedef struct OptixClusterAccelBuildModeDescImplicitDest
{
    CUdeviceptr outputBuffer;
    size_t outputBufferSizeInBytes;
    CUdeviceptr tempBuffer;
    size_t tempBufferSizeInBytes;

    CUdeviceptr outputHandlesBuffer;
    unsigned int outputHandlesStrideInBytes;
    CUdeviceptr outputSizesBuffer;
    unsigned int outputSizesStrideInBytes;
} OptixClusterAccelBuildModeDescImplicitDest;

typedef struct OptixClusterAccelBuildModeDescExplicitDest
{
    CUdeviceptr tempBuffer;
    size_t tempBufferSizeInBytes;
    CUdeviceptr destAddressesBuffer;
    unsigned int destAddressesStrideInBytes;

    CUdeviceptr outputHandlesBuffer;
    unsigned int outputHandlesStrideInBytes;
    CUdeviceptr outputSizesBuffer;
    unsigned int outputSizesStrideInBytes;
} OptixClusterAccelBuildModeDescExplicitDest;

typedef struct OptixClusterAccelBuildModeDescGetSize
{
    CUdeviceptr outputSizesBuffer;
    unsigned int outputSizesStrideInBytes;
    CUdeviceptr tempBuffer;
    size_t tempBufferSizeInBytes;
} OptixClusterAccelBuildModeDescGetSize;

---

<u>444</u>

1566typedefstructOptixClusterAccelBuildInputTriangles
1567 {
1568OptixClusterAccelBuildFlagsflags;
1569
1573unsignedintmaxArgCount;
1575OptixVertexFormatvertexFormat;
1579unsignedintmaxSbtIndexValue;
1581unsignedintmaxUniqueSbtIndexCountPerArg;
1582
1584unsignedintmaxTriangleCountPerArg;
1586unsignedintmaxVertexCountPerArg;
1588unsignedintmaxTotalTriangleCount;
1590unsignedintmaxTotalVertexCount;
1592unsignedintminPositionTruncateBitCount;
1593}OptixClusterAccelBuildInputTriangles;
1594
1595typedefstructOptixClusterAccelBuildInputGrids
1596 {
1597OptixClusterAccelBuildFlagsflags;
1598// Max number of OptixClusterAccelBuildInputGridsArgs provided at build time for
OPTIX_CLUSTER_ACCEL_BUILD_TYPE_TEMPLATES_FROM_GRIDS
1599unsignedintmaxArgCount;
1600
1602OptixVertexFormatvertexFormat;
1606unsignedintmaxSbtIndexValue;
1607
1608
1610unsignedintmaxWidth;
1612unsignedintmaxHeight;
1613}OptixClusterAccelBuildInputGrids;
1614
1615typedefstructOptixClusterAccelBuildInputClusters
1616 {
1617OptixClusterAccelBuildFlagsflags;
1619unsignedintmaxArgCount;
1620
1621unsignedintmaxTotalClusterCount;
1622unsignedintmaxClusterCountPerArg;
1623}OptixClusterAccelBuildInputClusters;
1624
1625typedefstructOptixClusterAccelPrimitiveInfo
1626 {
1627unsignedintsbtIndex: 24;
1628unsignedintreserved: 5;
1630unsignedintprimitiveFlags: 3;
1631}OptixClusterAccelPrimitiveInfo;
1632
1634typedefenumOptixClusterIDValues{
1635OPTIX_CLUSTER_ID_INVALID= 0xFFFFFFFFu,
1636}OptixClusterIDValues;
1637
1639typedefstructOptixClusterAccelBuildInputTrianglesArgs
1640 {
1643unsignedintclusterId;
1645unsignedintclusterFlags;
1646
1647// Packing the following values into a single 32b value
1649unsignedinttriangleCount: 9;
1651unsignedintvertexCount: 9;
1654unsignedintpositionTruncateBitCount: 6;
1656unsignedintindexFormat: 4;
1658unsignedintopacityMicromapIndexFormat: 4;
1659
1661OptixClusterAccelPrimitiveInfobasePrimitiveInfo;
1662
1664unsignedshortindexBufferStrideInBytes;
1666unsignedshortvertexBufferStrideInBytes;

---

<u>445</u>

1668unsignedshortprimitiveInfoBufferStrideInBytes;
1670unsignedshortopacityMicromapIndexBufferStrideInBytes;
1671
1673CUdeviceptrindexBuffer;
1679CUdeviceptrvertexBuffer;
1681CUdeviceptrprimitiveInfoBuffer;
1683CUdeviceptropacityMicromapArray;
1685CUdeviceptropacityMicromapIndexBuffer;
1686
1691CUdeviceptrinstantiationBoundingBoxLimit;
1692}OptixClusterAccelBuildInputTrianglesArgs;
1693
1695typedefstructOptixClusterAccelBuildInputGridsArgs
1696 {
1699unsignedintbaseClusterId;
1701unsignedintclusterFlags;
1702
1704OptixClusterAccelPrimitiveInfobasePrimitiveInfo;
1705
1706// Packing the following values into a single 32b value
1708unsignedintpositionTruncateBitCount: 6;
1709unsignedintreserved: 26;
1710
1711// Packing the following values into a single 32b value
1713unsignedchardimensions[2];
1714unsignedshortreserved2;
1715}OptixClusterAccelBuildInputGridsArgs;
1716
1718typedefstructOptixClusterAccelBuildInputTemplatesArgs
1719 {
1721unsignedintclusterIdOffset;
1722
1725unsignedintsbtIndexOffset;
1726
1728CUdeviceptrclusterTemplate;
1731CUdeviceptrvertexBuffer;
1733unsignedintvertexStrideInBytes;
1734unsignedintreserved;
1735}OptixClusterAccelBuildInputTemplatesArgs;
1736
1738typedefstructOptixClusterAccelBuildInputClustersArgs
1739 {
1741unsignedintclusterHandlesCount;
1742unsignedintclusterHandlesBufferStrideInBytes;
1746CUdeviceptrclusterHandlesBuffer;
1747}OptixClusterAccelBuildInputClustersArgs;
1748
1749typedefstructOptixClusterAccelBuildInput
1750 {
1751OptixClusterAccelBuildTypetype;
1752
1753union
1754 {
1757OptixClusterAccelBuildInputTrianglestriangles;
1759OptixClusterAccelBuildInputClustersclusters;
1761OptixClusterAccelBuildInputGridsgrids;
1762};
1763}OptixClusterAccelBuildInput;
1764
1765typedefstructOptixClusterAccelBuildModeDesc
1766 {
1767OptixClusterAccelBuildModemode;
1768union
1769 {
1770OptixClusterAccelBuildModeDescImplicitDestimplicitDest;
1771OptixClusterAccelBuildModeDescExplicitDestexplicitDest;
1772OptixClusterAccelBuildModeDescGetSizegetSize;

---

<u>446</u>

1773};
1774}OptixClusterAccelBuildModeDesc;
1775
1776
1782
1783
1787typedefenumOptixPixelFormat
1788 {
1789OPTIX_PIXEL_FORMAT_HALF1= 0x220a,
1790OPTIX_PIXEL_FORMAT_HALF2= 0x2207,
1791OPTIX_PIXEL_FORMAT_HALF3= 0x2201,
1792OPTIX_PIXEL_FORMAT_HALF4= 0x2202,
1793OPTIX_PIXEL_FORMAT_FLOAT1= 0x220b,
1794OPTIX_PIXEL_FORMAT_FLOAT2= 0x2208,
1795OPTIX_PIXEL_FORMAT_FLOAT3= 0x2203,
1796OPTIX_PIXEL_FORMAT_FLOAT4= 0x2204,
1797OPTIX_PIXEL_FORMAT_UCHAR3= 0x2205,
1798OPTIX_PIXEL_FORMAT_UCHAR4= 0x2206,
1799OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_LAYER= 0x2209
1800}OptixPixelFormat;
1801
1805typedefstructOptixImage2D
1806 {
1808CUdeviceptrdata;
1810unsignedintwidth;
1812unsignedintheight;
1814unsignedintrowStrideInBytes;
1819unsignedintpixelStrideInBytes;
1821OptixPixelFormatformat;
1822}OptixImage2D;
1823
1827typedefenumOptixDenoiserModelKind
1828 {
1830OPTIX_DENOISER_MODEL_KIND_AOV= 0x2324,
1831
1833OPTIX_DENOISER_MODEL_KIND_TEMPORAL_AOV= 0x2326,
1834
1836OPTIX_DENOISER_MODEL_KIND_UPSCALE2X= 0x2327,
1837
1839OPTIX_DENOISER_MODEL_KIND_TEMPORAL_UPSCALE2X= 0x2328,
1840
1843OPTIX_DENOISER_MODEL_KIND_LDR= 0x2322,
1844OPTIX_DENOISER_MODEL_KIND_HDR= 0x2323,
1845
1847OPTIX_DENOISER_MODEL_KIND_TEMPORAL= 0x2325
1848
1849}OptixDenoiserModelKind;
1850
1854typedefenumOptixDenoiserAlphaMode
1855 {
1857OPTIX_DENOISER_ALPHA_MODE_COPY= 0,
1858
1860OPTIX_DENOISER_ALPHA_MODE_DENOISE= 1
1861}OptixDenoiserAlphaMode;
1862
1866typedefstructOptixDenoiserOptions
1867 {
1868// if nonzero, albedo image must be given in OptixDenoiserGuideLayer
1869unsignedintguideAlbedo;
1870
1871// if nonzero, normal image must be given in OptixDenoiserGuideLayer
1872unsignedintguideNormal;
1873
1875OptixDenoiserAlphaModedenoiseAlpha;
1876}OptixDenoiserOptions;
1877
1881typedefstructOptixDenoiserGuideLayer

---

1882 {
1883// image with three components: R, G, B.
1884OptixImage2Dalbedo;
1885
1886// image with two or three components: X, Y, Z.
1887// (X, Y) camera space for OPTIX_DENOISER_MODEL_KIND_LDR, OPTIX_DENOISER_MODEL_KIND_HDR models.
1888// (X, Y, Z) world space, all other models.
1889OptixImage2Dnormal;
1890
1891// image with two components: X, Y.
1892// pixel movement from previous to current frame for each pixel in screen space.
1893OptixImage2Dflow;
1894
1895// Internal images used in temporal AOV denoising modes,
1896// pixel format OPTIX_PIXEL_FORMAT_INTERNAL_GUIDE_LAYER.
1897OptixImage2DpreviousOutputInternalGuideLayer;
1898OptixImage2DoutputInternalGuideLayer;
1899
1900// image with a single component value that specifies how trustworthy the flow vector at x,y
position in
1901// OptixDenoiserGuideLayer::flow is. Range 0..1 (low->high trustworthiness).
1902// Ignored if data pointer in the image is zero.
1903OptixImage2DflowTrustworthiness;
1904
1905}OptixDenoiserGuideLayer;
1906
1909typedefenumOptixDenoiserAOVType
1910 {
1912OPTIX_DENOISER_AOV_TYPE_NONE= 0,
1913
1914OPTIX_DENOISER_AOV_TYPE_BEAUTY= 0x7000,
1915OPTIX_DENOISER_AOV_TYPE_SPECULAR= 0x7001,
1916OPTIX_DENOISER_AOV_TYPE_REFLECTION= 0x7002,
1917OPTIX_DENOISER_AOV_TYPE_REFRACTION= 0x7003,
1918OPTIX_DENOISER_AOV_TYPE_DIFFUSE= 0x7004
1919
1920}OptixDenoiserAOVType;
1921
1925typedefstructOptixDenoiserLayer
1926 {
1927// input image (beauty or AOV)
1928OptixImage2Dinput;
1929
1930// denoised output image from previous frame if temporal model kind selected
1931OptixImage2DpreviousOutput;
1932
1933// denoised output for given input
1934OptixImage2Doutput;
1935
1936// Type of AOV, used in temporal AOV modes as a hint to improve image quality.
1937OptixDenoiserAOVTypetype;
1938}OptixDenoiserLayer;
1939
1945
1946typedefstructOptixDenoiserParams
1947 {
1952CUdeviceptrhdrIntensity;
1953
1958floatblendFactor;
1959
1965CUdeviceptrhdrAverageColor;
1966
1971unsignedinttemporalModeUsePreviousLayers;
1972
1975floatflowMulX;
1976floatflowMulY;
1977}OptixDenoiserParams;

---

typedef struct OptixDenoiserSizes
{
    size_t stateSizeInBytes;

    size_t withOverlapScratchSizeInBytes;

    size_t withoutOverlapScratchSizeInBytes;

    unsigned int overlapWindowSizeInPixels;

    size_t computeAverageColorSizeInBytes;

    size_t computeIntensitySizeInBytes;

    size_t internalGuideLayerPixelSizeInBytes;
} OptixDenoiserSizes;


typedef enum OptixRayFlags
{
    OPTIX_RAY_FLAG_NONE = 0u,

    OPTIX_RAY_FLAG_DISABLE_ANYHIT = 1u « 0,

    OPTIX_RAY_FLAG_ENFORCE_ANYHIT = 1u « 1,

    OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT = 1u « 2,

    OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT = 1u « 3,

    OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES = 1u « 4,

    OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES = 1u « 5,

    OPTIX_RAY_FLAG_CULL_DISABLED_ANYHIT = 1u « 6,

    OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT = 1u « 7,

    OPTIX_RAY_FLAG_FORCE_OPACITY_MICROMAP_2_STATE = 1u « 10,
} OptixRayFlags;

typedef enum OptixTransformType
{
    OPTIX_TRANSFORM_TYPE_NONE = 0,
    OPTIX_TRANSFORM_TYPE_STATIC_TRANSFORM = 1,
    OPTIX_TRANSFORM_TYPE_MATRIX_MOTION_TRANSFORM = 2,
    OPTIX_TRANSFORM_TYPE_SRT_MOTION_TRANSFORM = 3,
    OPTIX_TRANSFORM_TYPE_INSTANCE = 4,
} OptixTransformType;

typedef struct OptixTraverseData
{
    unsigned int data[20];
} OptixTraverseData;

typedef enum OptixTraversableGraphFlags
{
    OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_ANY = 0,

    OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_GAS = 1u « 0,

    OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING = 1u « 1,
} OptixTraversableGraphFlags;

---

2123typedefenumOptixCompileOptimizationLevel
2124 {
2126OPTIX_COMPILE_OPTIMIZATION_DEFAULT= 0,
2128OPTIX_COMPILE_OPTIMIZATION_LEVEL_0= 0x2340,
2130OPTIX_COMPILE_OPTIMIZATION_LEVEL_1= 0x2341,
2132OPTIX_COMPILE_OPTIMIZATION_LEVEL_2= 0x2342,
2134OPTIX_COMPILE_OPTIMIZATION_LEVEL_3= 0x2343,
2135}OptixCompileOptimizationLevel;
2136
2140typedefenumOptixCompileDebugLevel
2141 {
2143OPTIX_COMPILE_DEBUG_LEVEL_DEFAULT= 0,
2145OPTIX_COMPILE_DEBUG_LEVEL_NONE= 0x2350,
2148OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL= 0x2351,
2150OPTIX_COMPILE_DEBUG_LEVEL_MODERATE= 0x2353,
2152OPTIX_COMPILE_DEBUG_LEVEL_FULL= 0x2352,
2153}OptixCompileDebugLevel;
2154
2158typedefenumOptixModuleCompileState
2159 {
2161OPTIX_MODULE_COMPILE_STATE_NOT_STARTED= 0x2360,
2162
2164OPTIX_MODULE_COMPILE_STATE_STARTED= 0x2361,
2165
2167OPTIX_MODULE_COMPILE_STATE_IMPENDING_FAILURE= 0x2362,
2168
2170OPTIX_MODULE_COMPILE_STATE_FAILED= 0x2363,
2171
2173OPTIX_MODULE_COMPILE_STATE_COMPLETED= 0x2364,
2174}OptixModuleCompileState;
2175
2186typedefenumOptixCreationFlags
2187 {
2188OPTIX_CREATION_FLAG_NONE= 0,
2189OPTIX_CREATION_FLAG_BLOCK_UNTIL_EFFECTIVE= 1 « 0,
2190}OptixCreationFlags;
2191
2192
2225typedefstructOptixModuleCompileBoundValueEntry{
2226size_tpipelineParamOffsetInBytes;
2227size_tsizeInBytes;
2228constvoid*boundValuePtr;
2229constchar*annotation;// optional string to display, set to 0 if unused. If unused,
2230// OptiX will report the annotation as "No annotation"
2231}OptixModuleCompileBoundValueEntry;
2232
2234typedefenumOptixPayloadTypeID{
2235OPTIX_PAYLOAD_TYPE_DEFAULT= 0,
2236OPTIX_PAYLOAD_TYPE_ID_0= (1 « 0u),
2237OPTIX_PAYLOAD_TYPE_ID_1= (1 « 1u),
2238OPTIX_PAYLOAD_TYPE_ID_2= (1 « 2u),
2239OPTIX_PAYLOAD_TYPE_ID_3= (1 « 3u),
2240OPTIX_PAYLOAD_TYPE_ID_4= (1 « 4u),
2241OPTIX_PAYLOAD_TYPE_ID_5= (1 « 5u),
2242OPTIX_PAYLOAD_TYPE_ID_6= (1 « 6u),
2243OPTIX_PAYLOAD_TYPE_ID_7= (1 « 7u)
2244}OptixPayloadTypeID;
2245
2259typedefenumOptixPayloadSemantics
2260 {
2261OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_NONE= 0,
2262OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ= 1u « 0,
2263OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_WRITE= 2u « 0,
2264OPTIX_PAYLOAD_SEMANTICS_TRACE_CALLER_READ_WRITE= 3u « 0,
2265
2266OPTIX_PAYLOAD_SEMANTICS_CH_NONE= 0,
2267OPTIX_PAYLOAD_SEMANTICS_CH_READ= 1u « 2,

---

OPTIX_PAYLOAD_SEMANTICS_CH_WRITE = 2u « 2,
OPTIX_PAYLOAD_SEMANTICS_CH_READ_WRITE = 3u « 2,

OPTIX_PAYLOAD_SEMANTICS_MS_NONE = 0,
OPTIX_PAYLOAD_SEMANTICS_MS_READ = 1u « 4,
OPTIX_PAYLOAD_SEMANTICS_MS_WRITE = 2u « 4,
OPTIX_PAYLOAD_SEMANTICS_MS_READ_WRITE = 3u « 4,

OPTIX_PAYLOAD_SEMANTICS_AH_NONE = 0,
OPTIX_PAYLOAD_SEMANTICS_AH_READ = 1u « 6,
OPTIX_PAYLOAD_SEMANTICS_AH_WRITE = 2u « 6,
OPTIX_PAYLOAD_SEMANTICS_AH_READ_WRITE = 3u « 6,

OPTIX_PAYLOAD_SEMANTICS_IS_NONE = 0,
OPTIX_PAYLOAD_SEMANTICS_IS_READ = 1u « 8,
OPTIX_PAYLOAD_SEMANTICS_IS_WRITE = 2u « 8,
OPTIX_PAYLOAD_SEMANTICS_IS_READ_WRITE = 3u « 8,
} OptixPayloadSemantics;

typedef struct OptixPayloadType
{
    unsigned int numPayloadValues;

    const unsigned int *payloadSemantics;
} OptixPayloadType;

typedef struct OptixModuleCompileOptions
{
    int maxRegisterCount;

    OptixCompileOptimizationLevel optLevel;

    OptixCompileDebugLevel debugLevel;

    const OptixModuleCompileBoundValueEntry* boundValues;

    unsigned int numBoundValues;

    unsigned int numPayloadTypes;

    const OptixPayloadType* payloadTypes;

    OptixModule baseModule;
} OptixModuleCompileOptions;

typedef struct OptixBuiltinISOptions
{
    OptixPrimitiveType        builtinISModuleType;
    int                        usesMotionBlur;
    unsigned int                buildFlags;
    unsigned int                curveEndcapFlags;
} OptixBuiltinISOptions;

typedef enum OptixProgramGroupKind
{
    OPTIX_PROGRAM_GROUP_KIND_RAYGEN = 0x2421,

    OPTIX_PROGRAM_GROUP_KIND_MISS = 0x2422,

    OPTIX_PROGRAM_GROUP_KIND_EXCEPTION = 0x2423,

    OPTIX_PROGRAM_GROUP_KIND_HITGROUP = 0x2424,

    OPTIX_PROGRAM_GROUP_KIND_CALLABLES = 0x2425
} OptixProgramGroupKind;

typedef enum OptixProgramGroupFlags

---

{
    OPTIX_PROGRAM_GROUP_FLAGS_NONE = 0
} OptixProgramGroupFlags;

typedef struct OptixProgramGroupSingleModule
{
    OptixModule module;
    const char* entryFunctionName;
} OptixProgramGroupSingleModule;

typedef struct OptixProgramGroupHitgroup
{
    OptixModule moduleCH;
    const char* entryFunctionNameCH;
    OptixModule moduleAH;
    const char* entryFunctionNameAH;
    OptixModule moduleIS;
    const char* entryFunctionNameIS;
} OptixProgramGroupHitgroup;

typedef struct OptixProgramGroupCallables
{
    OptixModule moduleDC;
    const char* entryFunctionNameDC;
    OptixModule moduleCC;
    const char* entryFunctionNameCC;
} OptixProgramGroupCallables;

typedef struct OptixProgramGroupDesc
{
    OptixProgramGroupKind kind;

    unsigned int flags;

    union
    {
        OptixProgramGroupHitgroup hitgroup;
        OptixProgramGroupSingleModule raygen;
        OptixProgramGroupSingleModule miss;
        OptixProgramGroupSingleModule exception;
        OptixProgramGroupCallables callables;
    };
} OptixProgramGroupDesc;

typedef struct OptixProgramGroupOptions
{
    const OptixPayloadType* payloadType;
} OptixProgramGroupOptions;

typedef enum OptixExceptionCodes
{
    OPTIX_EXCEPTION_CODE_STACK_OVERFLOW = -1,

    OPTIX_EXCEPTION_CODE_TRACE_DEPTH_EXCEEDED = -2,
}

OptixExceptionCodes;

typedef enum OptixExceptionFlags
{
    OPTIX_EXCEPTION_FLAG_NONE = 0,

    OPTIX_EXCEPTION_FLAG_STACK_OVERFLOW = 1u « 0,

    OPTIX_EXCEPTION_FLAG_TRACE_DEPTH = 1u « 1,

    OPTIX_EXCEPTION_FLAG_USER = 1u « 2,
}

---

} OptixExceptionFlags;

typedef struct OptixPipelineCompileOptions
{
    int usesMotionBlur;

    unsigned int traversableGraphFlags;

    int numPayloadValues;

    int numAttributeValues;

    unsigned int exceptionFlags;

    const char* pipelineLaunchParamsVariableName;

    size_t pipelineLaunchParamsSizeInBytes;

    unsigned int usesPrimitiveTypeFlags;

    int allowOpacityMicromaps;

    int allowClusteredGeometry;
} OptixPipelineCompileOptions;

typedef struct OptixPipelineLinkOptions
{
    unsigned int maxTraceDepth;

    unsigned int maxContinuationCallableDepth;
    unsigned int maxDirectCallableDepthFromState;
    unsigned int maxDirectCallableDepthFromTraversal;

    unsigned int maxTraversableGraphDepth;
} OptixPipelineLinkOptions;

typedef struct OptixShaderBindingTable
{
    CUdeviceptr raygenRecord;

    CUdeviceptr exceptionRecord;

    CUdeviceptr  missRecordBase;
    unsigned int missRecordStrideInBytes;
    unsigned int missRecordCount;

    CUdeviceptr  hitgroupRecordBase;
    unsigned int hitgroupRecordStrideInBytes;
    unsigned int hitgroupRecordCount;

    CUdeviceptr  callablesRecordBase;
    unsigned int callablesRecordStrideInBytes;
    unsigned int callablesRecordCount;

} OptixShaderBindingTable;

typedef struct OptixStackSizes
{
    unsigned int cssRG;
    unsigned int cssMS;
    unsigned int cssCH;
    unsigned int cssAH;
    unsigned int cssIS;
    unsigned int cssCC;
    unsigned int dssDC;
}

---

2649}OptixStackSizes;
2650
2651
2657
2662typedefenumOptixDevicePropertyCoopVecFlags
2663 {
2666OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_NONE= 0,
2667
2668// Standard cooperative vector features are supported
2669OPTIX_DEVICE_PROPERTY_COOP_VEC_FLAG_STANDARD= 1 « 0,
2670}OptixDevicePropertyCoopVecFlags;
2671
2672typedefenumOptixCoopVecElemType
2673 {
2674OPTIX_COOP_VEC_ELEM_TYPE_UNKNOWN= 0x2A00,
2676OPTIX_COOP_VEC_ELEM_TYPE_FLOAT16= 0x2A01,
2678OPTIX_COOP_VEC_ELEM_TYPE_FLOAT32= 0x2A03,
2680OPTIX_COOP_VEC_ELEM_TYPE_UINT8= 0x2A04,
2682OPTIX_COOP_VEC_ELEM_TYPE_INT8= 0x2A05,
2684OPTIX_COOP_VEC_ELEM_TYPE_UINT32= 0x2A08,
2686OPTIX_COOP_VEC_ELEM_TYPE_INT32= 0x2A09,
2688OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E4M3= 0x2A0A,
2690OPTIX_COOP_VEC_ELEM_TYPE_FLOAT8_E5M2= 0x2A0B,
2691}OptixCoopVecElemType;
2692
2693typedefenumOptixCoopVecMatrixLayout
2694 {
2695OPTIX_COOP_VEC_MATRIX_LAYOUT_ROW_MAJOR= 0x2A40,
2696OPTIX_COOP_VEC_MATRIX_LAYOUT_COLUMN_MAJOR= 0x2A41,
2697OPTIX_COOP_VEC_MATRIX_LAYOUT_INFERENCING_OPTIMAL= 0x2A42,
2698OPTIX_COOP_VEC_MATRIX_LAYOUT_TRAINING_OPTIMAL= 0x2A43,
2699}OptixCoopVecMatrixLayout;
2700
2707typedefstructOptixCoopVecMatrixDescription
2708 {
2709unsignedintN;
2710unsignedintK;
2711unsignedintoffsetInBytes;
2712OptixCoopVecElemTypeelementType;
2713OptixCoopVecMatrixLayoutlayout;
2714unsignedintrowColumnStrideInBytes;
2715unsignedintsizeInBytes;
2716}OptixCoopVecMatrixDescription;
2717
2718typedefstructOptixNetworkDescription
2719 {
2720OptixCoopVecMatrixDescription*layers;
2721unsignedintnumLayers;
2722}OptixNetworkDescription;
2723
2729
2730
2732typedefenumOptixQueryFunctionTableOptions
2733 {
2735OPTIX_QUERY_FUNCTION_TABLE_OPTION_DUMMY= 0
2736
2737}OptixQueryFunctionTableOptions;
2738
2740typedefOptixResult(OptixQueryFunctionTable_t)(intabiId,
2741unsignedintnumOptions,
2742OptixQueryFunctionTableOptions*/*optionKeys*/,
2743constvoid**/*optionValues*/,
2744void* functionTable,
2745size_tsizeOfTable);
2746
2747
2748// end group optix_types

---

2750

2751#endif// OPTIX_OPTIX_TYPES_H

8.27 main.dox File Reference