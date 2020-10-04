/* date = October 3rd 2020 8:52 pm */

#ifndef _STATE_H
#define _STATE_H


namespace Chaos
{
	class Machine;
	class State
	{
		public:
		State(Machine* owner) : m_owner(owner){};
		
		virtual void Manage() = 0;
		
		protected:
		Machine* m_owner;
	};
}


#endif //_STATE_H
