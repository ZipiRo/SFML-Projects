#include "Resource.h"

class ResourceManager
{
private:
    ResourceManager() {}

public:
    static Resource<SoundBuffer> Sounds;
    static Resource<Texture> Textures;

    static bool LoadSound(std::string name, std::string path)
    {
        SoundBuffer sound;
        if(!sound.loadFromFile(path))
            return false;

        Sounds.Insert(name, sound);
        return true;
    }

    static bool LoadTexture(std::string name, std::string path)
    {
        Texture texture;
        if(!texture.loadFromFile(path))
            return false;

        Textures.Insert(name, texture);
        return true;
    }
};

Resource<SoundBuffer> ResourceManager::Sounds;
Resource<Texture> ResourceManager::Textures;

bool LoadResources()
{
    if(!ResourceManager::LoadSound("Place", "resources/place.wav")) return false;
    if(!ResourceManager::LoadSound("Pop", "resources/pop.wav")) return false;
    if(!ResourceManager::LoadSound("Find", "resources/find.wav")) return false;
    if(!ResourceManager::LoadSound("Remove", "resources/remove.wav")) return false;

    if(!ResourceManager::LoadTexture("Map", "resources/map.png")) return false;
    if(!ResourceManager::LoadTexture("Path", "resources/path.png")) return false;
    if(!ResourceManager::LoadTexture("Maze", "resources/maze.png")) return false;

    return true;
}