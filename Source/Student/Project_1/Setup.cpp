#include <pch.h>
#include "Projects/ProjectOne.h"
#include "Agent/CameraAgent.h"

void ProjectOne::setup()
{
    // Create your inital agents
    //agents->create_behavior_agent("ExampleAgent", BehaviorTreeTypes::Example);
    auto Rose = agents->create_behavior_agent("Rose", BehaviorTreeTypes::Rose);
    auto Lisa = agents->create_behavior_agent("Lisa", BehaviorTreeTypes::Lisa);
    auto Jisoo = agents->create_behavior_agent("Jisoo", BehaviorTreeTypes::Jisoo);
    auto Fan = agents->create_behavior_agent("Fan", BehaviorTreeTypes::Fan);

    // myfirstagent->set_pitch(45.0f); // tilt up right
    // myfirstagent->set_roll(45.0f); // tilt down right
    Rose->set_color(Vec3(1.f, 0.753f, 0.796f));
    Fan->set_color(Vec3(0.29f, 0.42f, 0.29f));
    // you can technically load any map you want, even create your own map file,
    // but behavior agents won't actually avoid walls or anything special, unless you code that yourself
    // that's the realm of project 2 though
    terrain->goto_map(0);

    // you can also enable the pathing layer and set grid square colors as you see fit
    // works best with map 0, the completely blank map
    terrain->pathLayer.set_enabled(true);
    terrain->pathLayer.set_value(0, 0, Colors::Red);

    // camera position can be modified from this default as well
    auto camera = agents->get_camera_agent();
    camera->set_position(Vec3(-62.0f, 70.0f, terrain->mapSizeInWorld * 0.5f));
    camera->set_pitch(0.610865); // 35 degrees

    audioManager->SetVolume(0.5f);
    audioManager->PlaySoundEffect(L"Assets\\Audio\\retro.wav");
    // uncomment for example on playing music in the engine (must be .wav)
    // audioManager->PlayMusic(L"Assets\\Audio\\motivate.wav");
    // audioManager->PauseMusic(...);
    // audioManager->ResumeMusic(...);
    // audioManager->StopMusic(...);
}