#include <chaospch.cpp>
#include <AnimationStates/PlayerAnimation_IdleState.h>
#include <Components/PlayerAnimatorController.h>

//OTHER ANIMATION STATES
#include <AnimationStates/PlayerAnimation_RunningState.h>

namespace Chaos
{
	PlayerAnimation_IdleState::PlayerAnimation_IdleState(PlayerAnimatorController* owner) : State(owner)
	{
		m_animatorController = owner;
		//set up animation here
		m_animation.SpriteSheet = Texture::Create(ANIMATION_FILE_PATH);
		m_animation.TotalFrames = 1;
		m_animation.FrameSize = Vec2 (54, 54);
		m_animation.SeccondsPerFrame = 0.08;
		m_animation.Loop = true;
		//play the animation
		m_animatorController->GetAnimator()->SetAnimation(m_animation);
		m_animatorController->GetAnimator()->Play();
		LOGTRACE("Idle anim...");
	}
	
	void PlayerAnimation_IdleState::Manage()
	{
		switch(m_animatorController->GetCurrentState())
		{
			case AnimationState::IDLE:
			{
			}break;
			case AnimationState::RUNNING:
			{
				m_animatorController->SetState(new PlayerAnimation_RunningState(m_animatorController));
			}break;
		}
	}
}