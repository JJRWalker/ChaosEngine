#include "Components/PlayerAnimatorController.h"
#include "Chaos/Entity/Entity.h"
//Animation States
#include <AnimationStates/PlayerAnimation_IdleState.h>


namespace Chaos
{
	PlayerAnimatorController::PlayerAnimatorController()
	{
		
	}
	
	void PlayerAnimatorController::Start()
	{
		m_animator = GetEntity()->GetComponent<Animator>();
		m_state = new PlayerAnimation_IdleState(this);
	}
	
	void PlayerAnimatorController::SetAnimationState(AnimationState state)
	{
		if (m_currentAnimationState != state)
		{
			m_currentAnimationState = state;
			m_state->Manage();
		}
	}
	
	
}