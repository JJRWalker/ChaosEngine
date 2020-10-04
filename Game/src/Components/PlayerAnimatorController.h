/* date = October 3rd 2020 8:18 pm */
#ifndef _PLAYER_ANIMATOR_CONTROLLER_H
#define _PLAYER_ANIMATOR_CONTROLLER_H

#include <Core/Machine.h>
#include <Chaos/Entity/Components/Animator.h>

namespace Chaos
{
	enum class AnimationState
	{
		IDLE,
		RUNNING
	};
	
	class PlayerAnimatorController : public Machine
	{
		public:
		PlayerAnimatorController();
		void Start() override;
		
		void SetAnimationState(AnimationState state);
		
		Animator* GetAnimator() {return m_animator;}
		AnimationState GetCurrentState() {return m_currentAnimationState;}
		
		private:
		AnimationState m_currentAnimationState;
		Animator* m_animator;
	};
}

#endif //_PLAYER_ANIMATOR_CONTROLLER_H
