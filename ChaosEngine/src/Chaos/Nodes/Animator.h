/* date = February 28th 2021 11:08 am */

#ifndef _ANIMATOR_H
#define _ANIMATOR_H

#include "Chaos/Nodes/Node.h"
#include "Chaos/Renderer/Texture.h"
#include "Chaos/Renderer/SubTexture.h"
#include "Chaos/DataTypes/Vec2.h"

namespace Chaos
{
	//NOTE: Frames start counting from zero as if they were in an array, however total frames should be the total whole number of frames (does not start at zero) as you may want to perform operations based on the number of frames, and multiplying by zero (if there's one frame) won't get you very far 
    struct Animation
    {
        Ref<Texture> SpriteSheet;
        uint32_t TotalFrames;
        Vec2 FrameSize;
		float SeccondsPerFrame = 0;
        uint32_t FrameRate = 30;
        uint32_t PlaybackSpeed = 1;
        uint32_t StartFrame = 0;
        bool Loop = true;
    };
    class Animator : public Node
    {
        public:
        Animator() {};
        Animator(Animation animation) { SetAnimation(animation); }
        void Update(float delta) override;
        void Play();
        void Stop();
        void SetAnimation(Animation animation);
        float GetLength() {return m_length; } //returns the length in seconds
        uint32_t GetCurrentFrame() {return m_currentFrame;}
		
        private:
        Ref<SubTexture> m_spriteSheet;  //sheet to draw frames from
        Animation m_animation;
        Vec2 m_spriteSheetBounds = Vec2::Zero();    //how many frames the spritesheet can fit in x and y
        uint32_t m_currentFrame = 0;
        uint32_t m_frameOffset = 0;
        float m_length = 0;
        float m_time = 0;
        bool m_play = false;
    };
}

#endif //_ANIMATOR_H
