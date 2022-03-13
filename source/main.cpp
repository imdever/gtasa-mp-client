#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include "MainHeader.h"
#include "PlayerClient.h"
#include "ImHook.h"
#include <functional>
#include <utility>
using namespace std;


#pragma comment(lib, "ws2_32.lib")
using namespace plugin;

MultiplayerPlugin main_plugin; // create main plugin

#define ADDR_BYPASS_VIDS_USA10					0x747483
#define ADDR_ENTRY								0xC8D4C0


void ViPr(DWORD addr, int size)
{
	DWORD d;
	VirtualProtect((PVOID)addr,size,PAGE_EXECUTE_READWRITE,&d);
}

MultiplayerPlugin::MultiplayerPlugin(): handle(NULL){

    ViPr(ADDR_BYPASS_VIDS_USA10,6); // OFF load screen
    *(BYTE *)ADDR_ENTRY = 5;
    memset((PVOID)ADDR_BYPASS_VIDS_USA10,0x90,6);
    ViPr(0x561AF0,7);
    memset((PVOID)0x561AF0,0x90,7);
    ViPr(0x53C159,5);
    memset((PVOID)0x53C159,0x90,5);
    ViPr(0x8CDEE4, sizeof(DWORD)); // --------------
    *(DWORD*)0x8CDEE4 = 0; // SET MAX WANTED LVL TO 0 ON CLIENT
    ViPr(0x969135, sizeof(BOOL));
    *(BOOL*)0x969135 = 1; //--------------------
    static int keyPressTime = 0;
    Events::gameProcessEvent += [this] {
    if (FindPlayerPed() && KeyPressed(VK_F9) && CTimer::m_snTimeInMilliseconds - keyPressTime > 500) {
        if (player == nullptr) {
            AbstractConnection* connection = AbstractConnection::connect("62.148.139.78", uint16_t(7777)); // вместо моего IP вписывай свой локальный... например 192.168.0.11
            player = new PlayerClient(connection);
            player->startReading();
            InitHook();
        }
    }
    };
}

void MultiplayerPlugin::SetMarkerPed(CPed *ped)//float x, float y, float z, int32_t playerid)
{
    //Command<Commands::ADD_BLIP_FOR_COORD>(x, y, z, playerid);
    Command<Commands::ADD_BLIP_FOR_CHAR>(ped);
}

void NewConnect()
{
    //CPed *player = FindPlayerPed();
    CMessages::AddMessageJumpQ("New connection", 150, 0, false);
}

void MultiplayerPlugin::output(string s) {
    static HANDLE handle = NULL;
    if (handle == NULL) {
        AllocConsole();
        handle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    DWORD written;
    WriteConsole(handle, s.c_str(), s.size(), &written, 0);
}

CPed* ped;

void NickTags()
{
            for(auto ped : CPools::ms_pPedPool){
                if (ped) {
                    CVector posn = ped->GetPosition();
                    RwV3d screenCoors; float w, h;
                    if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.25f }, &screenCoors, &w, &h, true, true))
                    {
                        // Настраивем вывод текста
                        CFont::SetOrientation(ALIGN_CENTER);
                        CFont::SetColor(CRGBA(255, 255, 255, 255));
                        CFont::SetDropShadowPosition(1);
                        CFont::SetBackground(false, false);
                        CFont::SetWrapx(500.0);
                        CFont::SetScale(0.5, 1.0);
                        CFont::SetFontStyle(FONT_SUBTITLES);
                        CFont::SetProportional(true);
                        static char str[32] = "Player";
                        CFont::PrintString(screenCoors.x, screenCoors.y, str);
                    }

                }
            }
}


CVehicle *MultiplayerPlugin::SpawnCar()
{
        CVector position = FindPlayerPed(-1)->TransformFromObjectSpace(CVector(0.0f, 5.0f, 0.0f));
        unsigned char oldFlags = CStreaming::ms_aInfoForModel[MODEL_INFERNUS].m_nFlags;
        CStreaming::RequestModel(MODEL_INFERNUS, GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
        if (CStreaming::ms_aInfoForModel[MODEL_INFERNUS].m_nLoadState == LOADSTATE_LOADED) {
            if (!(oldFlags & GAME_REQUIRED)) {
                CStreaming::SetModelIsDeletable(MODEL_INFERNUS);
                CStreaming::SetModelTxdIsDeletable(MODEL_INFERNUS);
            }
            CVehicle *vehicle = nullptr;
            // Выделяем обьект из пула
            switch (reinterpret_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[MODEL_INFERNUS])->m_nVehicleType) {
            case VEHICLE_MTRUCK:
                vehicle = new CMonsterTruck(MODEL_INFERNUS, 1);
                break;
            case VEHICLE_QUAD:
                vehicle = new CQuadBike(MODEL_INFERNUS, 1);
                break;
            case VEHICLE_HELI:
                vehicle = new CHeli(MODEL_INFERNUS, 1);
                break;
            case VEHICLE_PLANE:
                vehicle = new CPlane(MODEL_INFERNUS, 1);
                break;
            case VEHICLE_BIKE:
                vehicle = new CBike(MODEL_INFERNUS, 1);
                reinterpret_cast<CBike *>(vehicle)->m_nDamageFlags |= 0x10;
                break;
            case VEHICLE_BMX:
                vehicle = new CBmx(MODEL_INFERNUS, 1);
                reinterpret_cast<CBmx *>(vehicle)->m_nDamageFlags |= 0x10;
                break;
            case VEHICLE_TRAILER:
                vehicle = new CTrailer(MODEL_INFERNUS, 1);
                break;
            case VEHICLE_BOAT:
                vehicle = new CBoat(MODEL_INFERNUS, 1);
                break;
            default:
                vehicle = new CAutomobile(MODEL_INFERNUS, 1, true);
                break;
            }
            if (vehicle) {
                // Размещаем транспорт в игровом мире
                vehicle->SetPosn(position);
                vehicle->SetOrientation(0.0f, 0.0f, 0.0f);
                vehicle->m_nStatus = 4;
                vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
                CWorld::Add(vehicle);
                //CTheScripts::ClearSpaceForMissionEntity(position, vehicle); // удаляем другие обьекты, которые находятся в этих координатах
                if (vehicle->m_nVehicleClass == VEHICLE_BIKE)
                    reinterpret_cast<CBike *>(vehicle)->PlaceOnRoadProperly();
                else if (vehicle->m_nVehicleClass != VEHICLE_BOAT)
                    reinterpret_cast<CAutomobile *>(vehicle)->PlaceOnRoadProperly();
                return vehicle;
            }
        }
        return nullptr;
}


CPed* MultiplayerPlugin::spawnPed() {
#ifdef GTASA
    int modelID = 0;
    CStreaming::RequestModel(modelID, 0);
    CStreaming::LoadAllRequestedModels(false);
    ped = new CCivilianPed(PED_TYPE_CIVMALE, modelID);
    //CPed *ped = operator_new<CPlayerPed>(2, 1); // only mans, only cJ.
    if (ped) {
        ped->SetPosn(CVector(2495.330078f, -1676.422485f, 13.337957));
        ped->SetOrientation(0.0f, 0.0f, 0.0f);
        //ped->m_nPedType = ePedType::PED_TYPE_PLAYER1;
        CWorld::Add(ped);
        SetMarkerPed(ped);
        Events::processScriptsEvent.Add(NewConnect);
        Events::drawingEvent.Add(NickTags);
        ped->m_nCreatedBy = 2;
        ped->m_nPhysicalFlags.bBulletProof = 1;
        ped->m_nPhysicalFlags.bInvulnerable = 1;
        ped->m_nPhysicalFlags.bCollisionProof = 1;
        ped->m_nPhysicalFlags.bExplosionProof  = 1;
        ped->m_nPhysicalFlags.bMeeleProof   = 1;
        ped->m_nPhysicalFlags.bFireProof    = 1;
        ped->PositionAnyPedOutOfCollision();
    }
    return ped;
#else
    CHud::SetHelpMessage(L"Not done yet!", true, false);
#endif
}

/*
int pedModelIds[] = { 0, 7, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 43, 44, 45, 46,
    47, 48, 49, 50, 51, 52, 57, 58, 59, 60, 61, 62, 66, 67, 68, 70, 71, 72, 73, 78, 79, 80, 81, 82, 83, 84, 94, 95, 96, 97, 98, 99, 100, 101,
    102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 120, 121, 122, 123, 124, 125, 126, 127, 128, 132,
    133, 134, 135, 136, 137, 142, 143, 144, 146, 147, 153, 154, 155, 156, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 170, 171,
    173, 174, 175, 176, 177, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 200, 202, 203, 204, 206, 209, 210, 212, 213, 217, 220,
    221, 222, 223, 227, 228, 229, 230, 234, 235, 236, 239, 240, 241, 242, 247, 248, 249, 250, 252, 253, 254, 255, 258, 259, 260, 261, 262,
    9, 10, 11, 12, 13, 31, 38, 39, 40, 41, 53, 54, 55, 56, 63, 64, 69, 75, 76, 77, 85, 87, 88, 89, 90, 91, 92, 93, 129, 130, 131, 138, 139,
    140, 141, 145, 148, 150, 151, 152, 157, 169, 172, 178, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 201, 205, 207, 211, 214, 215,
    216, 218, 219, 224, 225, 226, 231, 232, 233, 237, 238, 243, 244, 245, 246, 251, 256, 257, 263 };*/

