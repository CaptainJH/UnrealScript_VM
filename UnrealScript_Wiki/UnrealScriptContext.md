
# UnrealScript Context


## The reflection system of Unreal Engine
The VM rely heavily on the information stored in UProperty, UStruct, etc. The need these info to specify stack size, to allocate memory for different defined in script.

All of class, function and variable defined in UnrealScript will be stored in the reflection data, even the local variable defined inside UnrealScript function. These info are constructed during the script parsing stage. Ususally, they are added through **FClassMetaData**'s different interface, such as AddFunction, AddState, AddProperty, etc. 

One thing worth mentioning is, right after the new reflection data is added to the **FClassMetaData**, the data itself is still not complete. Some data member such as UProperty::Offset will be set after the **UStruct::Link** is called. All of these is happend in script parsing, so when the script is parsed, all of the reflection data should be complete.

## The UObject address stored in Script bytecode
Some script instrinsic will store the UObject address with it in the bytecode, such as EX_LocalVariable. The address are created during the last code building, so when we startup the UE, during the script loading serializtion, these address will be changed and updated to the corrent address, these work happens in  **UStruct::SerializeExpr**, and it is mainly achieved by **ULinkerLoader**.