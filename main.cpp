#include <iostream>
#include <vector>

#define OLC_PGE_APPLICATION
#include "lib/olcPixelGameEngine.hpp"

#define FRAMETIME 0.1f

class DecalAnim
{
private:
    unsigned int m_totalFrames;
    unsigned int m_currentFrame;
    float m_frametime;
    olc::Decal* m_decal;

public:
    DecalAnim(olc::Decal& decal, unsigned int frames) : m_decal{decal}, m_totalFrames{frames}{}

    void update(float fElapsedTime)
    {
        m_frametime += fElapsedTime;
        if (m_frametime > FRAMETIME)
        {
            m_frametime -= FRAMETIME;
            if (m_currentFrame++ > m_totalFrames)
                m_currentFrame = 0;
        }
    }
    
    void reset()
    {
        m_currentFrame = 0;
        m_frametime = 0.0f;
    }

    unsigned int getFrame() { return m_currentFrame; }
    olc::Decal& getDecal() { return m_decal; }
};

class SpriteStateMachine
{
private:
    std::vector<DecalAnim&> m_animations;
    unsigned int m_state;

public:
    SpriteStateMachine(){}

    void addAnim(DecalAnim& animation)
    {
        m_animations.push_back(animation);
    }

    bool setState(unsigned int state)
    {
       if (state > m_animations.size()) return 0;

       m_state = state; 
    }

    unsigned int getFrame(){ return m_animations[m_state].getFrame(); }
    olc::Decal& getDecal() { return m_animations[m_state].getDecal(); }

};

 //PGE class override
class Test : public olc::PixelGameEngine
{
private:
    std::unique_ptr<olc::Sprite> sprFragment;
	std::unique_ptr<olc::Decal> decFragment;
    SpriteStateMachine& m_character;

    olc::vf2d m_pos = {0.0f, 0.0f};
    float m_speed = 20;
    float m_scale = 32;
    float m_posModif = 0;

public:
    //Construct
    Test(SpriteStateMachine& character) : m_character{character}
    { sAppName = "Test"; }

    //Initialization
    bool OnUserCreate() override
    {
        //Create decal from image file
        sprFragment = std::make_unique<olc::Sprite>("./resources/skellyAttack.png");
		decFragment = std::make_unique<olc::Decal>(sprFragment.get());
        std::cout << "Created!\n";
        return true;
    }

    //Frame update
    bool OnUserUpdate(float fElapsedTime) override
    {
        //Just close window
        if (GetKey(olc::Key::ESCAPE).bPressed)
            return 0;

        if (GetKey(olc::Key::D).bPressed)
        {
            m_scale = 32;
            m_posModif = 0;
        }
        if (GetKey(olc::Key::A).bPressed)
        {
            m_scale = -32;
            m_posModif = 32;
        }
        if (GetKey(olc::Key::RIGHT).bHeld) { m_pos.x += (m_speed * fElapsedTime); m_scale = -32; m_posModif = 32; }
        if (GetKey(olc::Key::LEFT).bHeld) { m_pos.x -= (m_speed * fElapsedTime); m_scale = 32; m_posModif = 0; }
        if (GetKey(olc::Key::DOWN).bHeld) m_pos.y += (m_speed * fElapsedTime);
        if (GetKey(olc::Key::UP).bHeld) m_pos.y -= (m_speed * fElapsedTime);


        std::cout << fElapsedTime << '\n';

        olc::vf2d finalPos{};
        finalPos.x = (m_pos.x + m_posModif);
        finalPos.y = m_pos.y;
        //Draw decal
        Clear(olc::WHITE);
        // DrawDecal((olc::vi2d)m_pos, decFragment.get(), {1.0f, 1.0f}, olc::WHITE);
        DrawPartialDecal(finalPos, {m_scale, 32.0f}, decFragment.get(), {0.0f, 0.0f}, {32, 32}, olc::WHITE);
        // DrawSprite(m_pos, sprFragment.get());

        return true;
    }
};

int main()
{       
    Test example();
    if (example.Construct(256, 240, 4, 4))
        example.Start();

    return 0;
}