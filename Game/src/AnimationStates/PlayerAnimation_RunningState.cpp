#include <AnimationStates/PlayerAnimation_RunningState.h>
#include <Components/PlayerAnimatorController.h>

//other states
#include <AnimationStates/PlayerAnimation_IdleState.h>

namespace Chaos
{
	PlayerAnimation_RunningState::PlayerAnimation_RunningState(PlayerAnimatorController* owner) : State(owner)
	{
		m_animatorController = owner;
		//set up animation here
		m_animation.SpriteSheet = Texture::Create(ANIMATION_FILE_PATH);
		m_animation.FrameSize = Vec2(64,64);
		m_animation.TotalFrames = 8;
		m_animation.SeccondsPerFrame = 0.08;
		m_animation.Loop = true;
		LOGTRACE("Running anim...");
		m_animatorController->GetAnimator()->SetAnimation(m_animation);
		m_animatorController->GetAnimator()->Play();
	}
	
	void PlayerAnimation_RunningState::Manage()
	{
		switch(m_animatorController->GetCurrentState())
		{
			case AnimationState::IDLE:
			{
				m_animatorController->SetState(new PlayerAnimation_IdleState(m_animatorController));
			}break;
			case AnimationState::RUNNING:
			{
			}break;
		}
	}
}