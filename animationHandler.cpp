#include "animationhandler.h"

void animationHandler::loadAnimation(string path,string name,int speed)
{
    vector<Texture> animation;

    try
    {
        for (const auto& entry : filesystem::directory_iterator(path)) {

            std::filesystem::path file = entry.path();
            std::string filestr = file.string();
            const char* path = filestr.c_str();
            animation.push_back(LoadTextureFromImage(LoadImage(path)));
        }

        animation_names[name] = animations.size();
        animations.push_back(animation);
        current_frame.push_back(0);
        animation_time.push_back(0);
        fps.push_back(speed);
        increment_time.push_back(1 / (float)speed);
        enable.push_back(0);
        repetition.push_back(0);
        animation_information.push_back({});

        cout << "ANIMATION LOADED\n";
    }
    catch (exception e)
    {
        cout << "ANIMATION COULD NOT BE LOADED\n";
        exit(0);
    }
}

void animationHandler::playStaticAnimation(string name, float x, float y, float scale, float rotation,bool rep,float screenmode)
{
    vector<float> info = { x,y,scale,rotation,screenmode};
    animation_information[animation_names[name]] = info;
    repetition[animation_names[name]] = rep;
    enable[animation_names[name]]= 1;
}

void animationHandler::update(float newtime,int screenmode)
{
    //Update Frames
    if (prevtime == 0)prevtime = newtime;
    else
    {
        float deltatime = newtime - prevtime;
        for (int i = 0; i < animations.size(); i++)
        {
            if (enable[i])
            {
                animation_time[i] += deltatime;
                if (animation_time[i] > increment_time[i])
                {
                    current_frame[i] += 1;
                    if (current_frame[i] == animations[i].size())
                    {
                        if (repetition[i] == 1)
                            current_frame[i] = 0;
                        else
                            stopAnimation(i);
                    }
                    animation_time[i] = 0;
                }

            }
        }

        prevtime = newtime;
    }

    //Display Frames
    for (int i = 0; i < animations.size(); i++)
    {
        if (enable[i]&&(animation_information[i][4]==screenmode|| animation_information[i][4] == 0))
        {
            DrawTextureEx(animations[i][current_frame[i]], { animation_information[i][0],animation_information[i][1] }, animation_information[i][3], animation_information[i][2],WHITE);
        }
    }
}

void animationHandler::stopAnimation(string name)
{
    current_frame[animation_names[name]] = 0;
    animation_time[animation_names[name]]=0;
    enable[animation_names[name]]=0;
    repetition[animation_names[name]]=0;
}

void animationHandler::stopAnimation(int i)
{
    current_frame[i] = 0;
    animation_time[i] = 0;
    enable[i] = 0;
    repetition[i] = 0;
}