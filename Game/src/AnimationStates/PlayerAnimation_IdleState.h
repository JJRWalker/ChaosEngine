/* date = October 3rd 2020 9:17 pm */

#ifndef _PLAYER_ANIMATION__IDLE_STATE_H
#define _PLAYER_ANIMATION__IDLE_STATE_H
#include <Core/State.h>
#include <Chaos/Entity/Components/Animator.h>

namespace Chaos
{
	class PlayerAnimatorController;
	class PlayerAnimation_IdleState : public State
	{
		public:
		PlayerAnimation_IdleState(PlayerAnimatorController* owner);
		
		void Manage() override;
		private:
		PlayerAnimatorController* m_animatorController;
		Animation m_animation;
		char* ANIMATION_FILE_PATH = "../Game/textures/sprite-1.png";
	};
}


#endif //_PLAYER_ANIMATION__IDLE_STATE_H
