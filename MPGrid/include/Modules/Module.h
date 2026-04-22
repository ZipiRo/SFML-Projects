struct Module
{
    virtual void Start(ApplicationContext&) {}
    virtual void Init(ApplicationContext&) {}
    virtual void Update(ApplicationContext&) = 0;
    virtual void Draw(ApplicationContext&) {}

    virtual void SidebarInterface(ApplicationContext&) {}
    virtual void SettingsInterface(ApplicationContext&) {}

    virtual std::string GetSidebarTitle() = 0;
    virtual std::string GetSettingsTitle() = 0;
};

#include "Topo.h"
#include "Pathfinder.h"
#include "Mazer.h"