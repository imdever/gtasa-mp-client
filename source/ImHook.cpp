// dllmain.cpp : Defines the entry point for the DLL application.
#include "ImHook.h"

typedef HRESULT(__stdcall* _EndScene)(IDirect3DDevice9* pDevice);
_EndScene oEndScene;
static WNDPROC OriginalWndProcHandler = nullptr;

bool initialized = false;
bool opentab = false;
bool openinfo = false;
bool openplayers = false;

HWND window;

char buf[256];

void update_ids()
{
    while(true)
    {

    }
}


void InitImGui(IDirect3DDevice9* pDevice) {
    D3DDEVICE_CREATION_PARAMETERS CP;
    pDevice->GetCreationParameters(&CP);
    //hWnd = CP.hFocusWindow;
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    SetCursor(io.MouseDrawCursor ? NULL : LoadCursor(NULL, IDC_ARROW));
    io.IniFilename = NULL;
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    io.Fonts->AddFontDefault();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);
    initialized = true;
    return;
}

//typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	POINT mPos;
	GetCursorPos(&mPos);
	ScreenToClient(window, &mPos);
	ImGui::GetIO().MousePos.x = mPos.x;
	ImGui::GetIO().MousePos.y = mPos.y;

	if (uMsg == WM_KEYUP)
	{
		if (wParam == VK_DELETE)
		{
			opentab = !opentab;
		}
		if(wParam == 0x49)
        {
            openinfo = !openinfo;
        }
        if(wParam == VK_F7)
        {
            openplayers = !openplayers;
        }

	}

	if (opentab)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(OriginalWndProcHandler, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkEndScene(IDirect3DDevice9* pDevice)
{
 if (!initialized){
        InitImGui(pDevice);

        ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
		OriginalWndProcHandler = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hWndProc);
		ImGui::GetIO().ImeWindowHandle = window;
		initialized = true;
 }
            if(opentab){
                ImGui_ImplDX9_NewFrame();
                ImGui_ImplWin32_NewFrame();

                ImGui::NewFrame();
                ImGui::Begin("SAO Chat",nullptr);
                {
                    //ImGui::InputText("Message",buf,sizeof(buf));
                    //ImGui::Button("Send");
                    //ImGui::Text("\nMessages:\n");
                    static char text[1024 * 16];//Сначала создается статический массив чаров
                    ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
                    if (ImGui::Button("Send"))
                    {
                        //Packetchat packet;
                        //packet.type = PacketType::PACKET_CHAT;
                        //packet.info = buf;
                        //sendPacket(&packet, sizeof(MultiplayerPacket));
                        ImGui::Text(text);
                        ImGui::Text("\n");
                        //text = NULL;
                    }
                }
                ImGui::End();

                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            }
            if(openinfo){
                ImGui_ImplDX9_NewFrame();
                ImGui_ImplWin32_NewFrame();

                ImGui::NewFrame();
                // Установить положение окна
                ImGui::SetNextWindowPos(ImVec2(0,0), 0, ImVec2(0, 0));
                // Устанавливаем размер окна
                ImGui::SetNextWindowSize(ImVec2(300, 300));
                ImGui::Begin("Info");
                {
                    //ImGui::Show
                    //PlayerClient np;
                    ImGui::Text("San Andreas Online v0.1a experimental\n");
                    ImGui::Text("Developers: ASMCoder, [d]zen, KodKalcor\n");
                    ImGui::Text("Press DEL to open the chat\n");

                }
                ImGui::End();

                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        }
        if(openplayers)
        {
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();
            ImGui::SetNextWindowSize(ImVec2(500, 500));
			ImGui::SetNextWindowPos(ImVec2(0,0), 0, ImVec2(0, 0));

			ImGui::Begin("Players");
			{
                ImGui::Text("ID\t\t\t\t\tName");
                ImGui::Separator();

                ImGui::Text("%d\t\t\t\t\t%s", 0, "Player");
                ImGui::Separator();

                ImGui::Text("%d\t\t\t\t\t%s", 1, "Player");
                ImGui::Separator();
			}
			ImGui::End();
			ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        }
        return oEndScene(pDevice);
}




void InitHook()
{
    GetModuleHandleA("d3d9.dll");//waiting till d3d9.dll gets loaded
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    window = FindWindowA(NULL, "GTA: San Andreas");
    // Endscene hook
    void** vTableDevice = *(void***)(*(DWORD*)0xC97C28);
    VTableHookManager* vmtHooks = new VTableHookManager(vTableDevice, 119);
    oEndScene = (_EndScene)vmtHooks->Hook(42, (void*)&hkEndScene);
}


