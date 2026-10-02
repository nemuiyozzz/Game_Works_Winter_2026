#include "Pch.h"
#include "Application.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Application::CreateInstance();
    Application::GetInstance().Initialize();

    if (Application::GetInstance().GetInitFail())
    {
        return -1;
    }

    Application::GetInstance().Run();


    if (Application::GetInstance().GetReleaseFail())
    {
        return -1;
    }

    Application::GetInstance().DestroyInstance();

    return 0;
}