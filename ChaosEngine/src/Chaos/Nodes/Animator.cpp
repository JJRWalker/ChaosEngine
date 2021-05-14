#include "chaospch.h"

#include "Animator.h"

#include "Chaos/Core/Time.h"

namespace Chaos
{
    void Animator::OnFixedUpdate(float delta)
    {
        if (GetTexture() && m_play && m_animation.TotalFrames > 1)
        {
            //Get which frame we should display
            m_time += delta;
            m_currentFrame = (uint32_t)roundf((m_time / m_length) * (m_animation.TotalFrames - 1)) + m_frameOffset;
			
			//when finished, reset and if we aren't looping, stop
            if (m_time > m_length)
            {
                if (!m_animation.Loop)
				{
					m_time = 0;
                    Stop();
					return;
				}
				m_time = m_length - m_time;
            }
            
			
            //Getting that frame in terms of x and y using remainder and how many times the current frame goes into the x bounds cleanly
            SetCoords(Vec2((float)(m_currentFrame % (int)m_spriteSheetBounds.X), floorf(m_currentFrame / m_spriteSheetBounds.X)));
			
        }
		else if (m_play)
		{
			Stop();
		}
    }
	
    void Animator::Play()
    {
        m_play = true;
        m_time = 0;
    }
	
    //stops the current animation
    void Animator::Stop()
    {   
        //same as play, reactivate renderer if animation isn't playing
        m_play = false;
        m_time = 0;
    }
	
    //change the animation properties, does not stop current animation 
    void Animator::SetAnimation(Animation animation)
    {
		SetTexture(animation.SpriteSheet);
		m_length = animation.SecondsPerFrame * animation.TotalFrames;   //calculating length in seconds
        //animation.TotalFrames--; //subtracting 1 as frames internally start from 0, not 1;
        m_frameOffset = animation.StartFrame;
        m_animation = animation; //storing the animation data
		//creating a subtexture from the spritesheet given, starting at the first index
        m_spriteSheetBounds = Vec2(ceilf((float)GetTexture()->GetWidth() / animation.FrameSize.X),
								   ceilf((float)GetTexture()->GetHeight() / animation.FrameSize.Y)); // getting max coordinates for x and y given the frame size and the width
        //warning if there would be more frames than there is space for on this sprite sheet given the frame size given and the size of the sprite sheet 
		
		SetTotalCells(m_spriteSheetBounds);
		
        if (m_spriteSheetBounds.X * m_spriteSheetBounds.Y < animation.TotalFrames)
        {
            LOGCORE_WARN("WARNING: NUMBER OF FRAMES SPECIFIED IN ANIMATION EXCEEDS THE SPRITESHEETS AREA AT THE SIZE PER FRAME GIVEN");
        }
    }
}