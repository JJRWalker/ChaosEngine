/* date = October 3rd 2020 8:58 pm */

#ifndef _MACHINE_H
#define _MACHINE_H

#include <Chaos/Entity/Component.h>
#include <Core/State.h>

namespace Chaos
{
	
	class Machine : public Component
	{
		public:
		Machine(){};
		
		State* GetState() {return m_state;}
		void SetState(State* state) {delete m_state; m_state = state;}
		
		protected:
		State* m_state;
	};
}


#endif //_MACHINE_H
