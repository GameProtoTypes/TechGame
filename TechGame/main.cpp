#include "TechGame.h"

// A helper macro which defines main function. Forgetting it will result in linker errors complaining about missing `_main` or `_WinMain@16`.
//URHO3D_DEFINE_APPLICATION_MAIN(TechGame);

//#TEMP:
int RunApplication() 
{
Urho3D::SharedPtr<Urho3D::Context> context(new Urho3D::Context()); 
Urho3D::SharedPtr<TechGame> application(new TechGame(context));
int r = application->Run(); 
return r;
} 

int main(int argc, char** argv) 
{ 
Urho3D::ParseArguments(argc, argv); 
return RunApplication();
}