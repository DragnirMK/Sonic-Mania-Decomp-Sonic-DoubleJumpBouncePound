#include "Player.h"
#include "GameAPI/Game.h"
#include "Camera.h"
#include "ImageTrail.h"
#include "Shield.h"
#include "../Menu/UIWidgets.h"
#include "Zone.h"

ObjectPlayer *Player;
ObjectImageTrail *ImageTrail;
ObjectShield *Shield;
ObjectZone *Zone;
ObjectUIWidgets *UIWidgets;

void Player_JumpAbility_Sonic_Hook(void)
{
    RSDK_THIS(Player);

    bool32 doublejumpDisabled = false;
    bool32 bouncePoundDisabled = false;

    if (self->invincibleTimer) {
        // Do not check shield state
    }
    else {
        switch (self->shield) {
            case SHIELD_BUBBLE:
                doublejumpDisabled = true;
                bouncePoundDisabled = true;
                break;

            case SHIELD_FIRE:
                doublejumpDisabled = true;
                break;

            case SHIELD_LIGHTNING:
                doublejumpDisabled = true;
                break;

            default: break;
        }
    }

    if (ControllerInfo[self->controllerID].keyA.press) {
        if (!doublejumpDisabled) {
            self->state = Player_DoubleJump_Sonic;
        }
    }

    if (!self->invertGravity && ControllerInfo[self->controllerID].keyDown.down && ControllerInfo[self->controllerID].keyY.press && !bouncePoundDisabled) {
        self->state = Player_GroundPound_Sonic;
    }
}
void Player_DoubleJump_Sonic(void)
{
    RSDK_THIS(Player);
    
    if (self->jumpAbilityState == 2) {
        RSDK.PlaySfx(UIWidgets->sfxWoosh, false, 255);
        if (!self->underwater) {
            self->velocity.y = self->invertGravity ? 0x40000 : -0x40000;
        }
        else {
            self->velocity.y = self->invertGravity ? 0x20000 : -0x20000;
        }
    }

    self->state = Player_State_Air;
}
void Player_GroundPound_Sonic(void)
{
    RSDK_THIS(Player);

    if (self->jumpAbilityState <= 2) {
        self->velocity.x >>= 1;
        self->velocity.y = self->underwater ? 0x40000 : 0x80000;
        self->jumpAbilityState = 0;
        RSDK.SetSpriteAnimation(self->aniFrames, ANI_HAMMERDROP, &self->animator, true, 2);
        self->nextAirState = StateMachine_None;
        self->nextGroundState = StateMachine_None;

        RSDK.PlaySfx(Player->sfxRelease, false, 0xFF);

        if (self->camera && !Zone->autoScrollSpeed) {
            self->scrollDelay = 8;
            self->camera->state = Camera_State_FollowX;
        }
        self->state = Player_BouncePound_Sonic;
    }
    else {
        self->state = Player_State_Air;
    }
}
void Player_BouncePound_Sonic(void)
{
    RSDK_THIS(Player);

    if (self->onGround) {
        self->controlLock = 0;
        self->onGround = false;

        RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);

        self->animator.speed = MIN((abs(self->groundVel) >> 12) + 0x30, 0xF0);

        Hitbox* hitbox = RSDK.GetHitbox(&self->animator, 0);
        Player_SpawnMightyHammerdropDust(0x10000, hitbox);
        Player_SpawnMightyHammerdropDust(-0x10000, hitbox);
        Player_SpawnMightyHammerdropDust(0x18000, hitbox);
        Player_SpawnMightyHammerdropDust(-0x18000, hitbox);
        Player_SpawnMightyHammerdropDust(0x20000, hitbox);
        Player_SpawnMightyHammerdropDust(-0x20000, hitbox);

        // Bounce if button is held
        if (ControllerInfo[self->controllerID].keyY.down) {
            int32 vel = 0;
            if (self->underwater != 1) // only apply lower velocity if in water level types, pools use regular vel
                vel = self->gravityStrength + 0x78000;
            else
                vel = self->gravityStrength + 0x40000;
            self->onGround = false;

            self->velocity.x = (self->groundVel * RSDK.Cos256(self->angle) + vel * RSDK.Sin256(self->angle)) >> 8;
            self->velocity.y = (self->groundVel * RSDK.Sin256(self->angle) - vel * RSDK.Cos256(self->angle)) >> 8;

            RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
            self->animator.speed = (abs(self->groundVel) >> 12) + 0x30;
            RSDK.PlaySfx(Shield->sfxBubbleBounce, false, 255);
        }

        self->angle = 0;
        self->collisionMode = CMODE_FLOOR;
        self->applyJumpCap = false;
        self->jumpAbilityState = 3;

        if (self->invincibleTimer > 0) {
            if (self->invincibleTimer < 8)
                self->invincibleTimer = 8;
            self->state = Player_State_Air;
        }
        else {
            self->invincibleTimer = -8;
            self->state = Player_State_Air;
        }
    }
    else {
        Player_HandleAirFriction();
        Player_HandleAirMovement();

        if (self->velocity.y <= 0x10000) {
            self->state = Player_State_Air;
            RSDK.SetSpriteAnimation(self->aniFrames, ANI_JUMP, &self->animator, false, 0);
        }
    }
}