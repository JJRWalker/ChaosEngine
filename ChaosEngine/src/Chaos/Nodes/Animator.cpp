#include "chaospch.h"

#include "Animator.h"

#include "Chaos/Core/Time.h"
#include "Chaos/Nodes/Sprite.h"
#include "Chaos/Renderer/Renderer.h"
#include "Chaos/Core/Application.h"

namespace Chaos
{
    void Animator::Update(float delta)
    {
        if (m_play && m_animation.TotalFrames > 1)
        {
            //Get which frame we should display
            m_time += Time::GetDeltaTime();
            m_currentFrame = (uint32_t)floorf((m_time / m_length) * m_animation.TotalFrames) + m_frameOffset;
			
			//when finished, reset and if we aren't looping, stop
            if (m_time > m_length)
            {
                m_time = 0;
                if (!m_animation.Loop)
                    Stop();
            }
            
			
            //Getting that frame in terms of x and y using remainder and how many times the current frame goes into the x bounds cleanly
            m_spriteSheet->SetTexCoords(Vec2((float)(m_currentFrame % (int)m_spriteSheetBounds.X), floorf(m_currentFrame / m_spriteSheetBounds.X)), m_animation.FrameSize);
			
            
            Application::Get().GetRenderer().DrawQuad(GetPosition3D(), GetScale(), Vec2(GetRotation(), GetRotation()), m_spriteSheet);
        }
		else if (m_play)
		{
			Stop();
		}
    }
	
    void Animator::Play()
    {
        //deactivate the render component if playing, should probably pass the texture to the render component, however it doesn't take 
        //Subtextures at this time
        if (p_parent && p_parent->GetChild<Sprite>())
        {
            p_parent->GetChild<Sprite>()->Enabled = false;
        }
        m_play = true;
        m_time = 0;
    }
	
    //stops the current animation
    void Animator::Stop()
    {   
        //same as play, reactivate renderer if animation isn't playing
        if (p_parent && p_parent->GetChild<Sprite>())
        {
            p_parent->GetChild<Sprite>()->Enabled = true;
        }
        m_play = false;
        m_time = 0;
    }
	
    //change the animation properties, does not stop current animation 
    void Animator::SetAnimation(Animation animation)
    {
		m_length = animation.SeccondsPerFrame * animation.TotalFrames;   //calculating length in seconds
        animation.TotalFrames--; //subtracting 1 as frames internally start from 0, not 1;
        m_frameOffset = animation.StartFrame;
        m_animation = animation; //storing the animation data
        m_spriteSheet = SubTexture::Create(animation.SpriteSheet, Vec2(0, 0), animation.FrameSize); //creating a subtexture from the spritesheet given, starting at the first index
        m_spriteSheetBounds = Vec2(ceilf((float)animation.SpriteSheet->GetWidth() / animation.FrameSize.X),
								   ceilf((float)animation.SpriteSheet->GetHeight() / animation.FrameSize.Y)); // getting max coordinates for x and y given the frame size and the width
        //warning if there would be more frames than there is space for on this sprite sheet given the frame size given and the size of the sprite sheet 
        if (m_spriteSheetBounds.X * m_spriteSheetBounds.Y < animation.TotalFrames)
        {
            LOGCORE_WARN("WARNING: NUMBER OF FRAMES SPECIFIED IN ANIMATION EXCEEDS THE SPRITESHEETS AREA AT THE SIZE PER FRAME GIVEN");
        }
    }
}