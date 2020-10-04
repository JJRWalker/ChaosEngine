/* date = October 3rd 2020 9:46 pm */

#ifndef _PLAYER_ANIMATION__RUNNING_STATE_H
#define _PLAYER_ANIMATION__RUNNING_STATE_H

#include <Core/State.h>

namespace Chaos
{
	class PlayerAnimatorController;
	class PlayerAnimation_RunningState : public State
	{
		public:
		PlayerAnimation_RunningState(PlayerAnimatorController* owner);
		
		void Manage() override;
		private:
		PlayerAnimatorController* m_animatorController;
		Animation m_animation;
		char* ANIMATION_FILE_PATH = "../Game/textures/runningSpriteSheet.png";
	};
}

#endif //_PLAYER_ANIMATION__RUNNING_STATE_H
