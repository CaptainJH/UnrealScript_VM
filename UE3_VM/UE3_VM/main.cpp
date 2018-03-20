#include "UnStack.h"
#include "UnScript.h"
#include "Utils.h"
#include "UnName.h"
#include "FArray.h"
#include "UnitTest.h"

struct Vector4
{
	float X, Y, Z, W;

	bool operator==(const Vector4& rhs) const
	{
		return X == rhs.X 
			&& Y == rhs.Y 
			&& Z == rhs.Z
			&& W == rhs.W;
	}
};


int main()
{
	std::cout << "==============UE3_VM==============\n" << std::endl;

	ScriptRuntimeContext::LoadFromFile("D:\\Repo\\UnrealScript_VM\\UnrealScript_Wiki\\bytecode\\UnLinker_Export.txt");
	FName::LoadNames("D:\\Repo\\UnrealScript_VM\\UnrealScript_Wiki\\bytecode\\UnLinker_Name_Export.txt");

	UnitTestFunction("JHQ_ArrayTest", 9);
	UnitTestFunction("JHQ_StringTest", FString("Hello World"));
	Vector4 v4 = { 2550, 1440, 1920, 1080 };
	UnitTestFunction("JHQ_StructTest", v4);

}