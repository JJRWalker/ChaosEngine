#include "Player.h"
#include "Chaos/Entity/Components/Animator.h"
#include "Chaos/Entity/Components/Render.h"
#include "Components/PlayerController.h"
#include "Components/PlayerAnimatorController.h"

namespace Chaos
{
	void Player::Start()
	{
		AddComponent<Render>();
		GetComponent<Render>()->SetTexture(Texture::Create("../Game/textures/sprite-1.png"));
		AddComponent<Animator>();
		Animation anim;
		anim.SpriteSheet = Texture::Create("../Game/textures/runningSpriteSheet.png");
		anim.Loop = true;
		anim.TotalFrames = 8;
		anim.SeccondsPerFrame = 0.08f;
		anim.PlaybackSpeed = 1;
		anim.FrameRate = 10;
		anim.FrameSize.X = 64.f;
		anim.FrameSize.Y = 64.f;
		anim.StartFrame = 1;
		
		GetComponent<Chaos::Animator>()->SetAnimation(anim);
		//GetComponent<Chaos::Animator>()->Play();
		
		AddComponent<PlayerController>();
		AddComponent<PlayerAnimatorController>();
		
		
		Entity::Start();
	}
}