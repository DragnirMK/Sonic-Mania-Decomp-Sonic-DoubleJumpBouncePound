#include "../GameAPI/C/GameAPI/Game.h"
#include "Objects/Player.h"
#include "Objects/Camera.h"
#include "Objects/ImageTrail.h"
#include "Objects/Shield.h"
#include "Menu/UIWidgets.h"
#include "Objects/Zone.h"

#if RETRO_USE_MOD_LOADER
DLLExport bool32 LinkModLogic(EngineInfo *info, const char *id);
#endif

void InitModAPI(void) {
Player_State_Ground                 = Mod.GetPublicFunction(NULL, "Player_State_Ground");
Player_State_Air                    = Mod.GetPublicFunction(NULL, "Player_State_Air");
Player_State_DropDash               = Mod.GetPublicFunction(NULL, "Player_State_DropDash");
Player_JumpAbility_Sonic            = Mod.GetPublicFunction(NULL, "Player_JumpAbility_Sonic");
Player_Gravity_True                 = Mod.GetPublicFunction(NULL, "Player_Gravity_True");
Player_HandleAirMovement            = Mod.GetPublicFunction(NULL, "Player_HandleAirMovement");
Player_HandleAirFriction            = Mod.GetPublicFunction(NULL, "Player_HandleAirFriction");
Player_SpawnMightyHammerdropDust    = Mod.GetPublicFunction(NULL, "Player_SpawnMightyHammerdropDust");
Camera_State_FollowX                = Mod.GetPublicFunction(NULL, "Camera_State_FollowX");

Mod.RegisterStateHook(Player_JumpAbility_Sonic, Player_JumpAbility_Sonic_Hook, false);

MOD_REGISTER_OBJECT_HOOK(Player);
MOD_REGISTER_OBJECT_HOOK(ImageTrail);
MOD_REGISTER_OBJECT_HOOK(Shield);
MOD_REGISTER_OBJECT_HOOK(UIWidgets);
MOD_REGISTER_OBJECT_HOOK(Zone);
}

#if RETRO_USE_MOD_LOADER
#define ADD_PUBLIC_FUNC(func) Mod.AddPublicFunction(#func, (void *)(func))

void InitModAPI(void);

bool32 LinkModLogic(EngineInfo *info, const char *id)
{
#if MANIA_USE_PLUS
    LinkGameLogicDLL(info);
#else
    LinkGameLogicDLL(*info);
#endif

    globals = Mod.GetGlobals();

    modID = id;

    InitModAPI();

    return true;
}
#endif