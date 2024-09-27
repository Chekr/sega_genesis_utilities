#ifndef IMG_OBJECTS_H
#define IMG_OBJECTS_H


#include <stdint.h>
#include <string>
#include <list>
#include <vector>


struct RgbColor
{
    uint8_t r,g,b;

public:
    // comparator override
    bool operator <(RgbColor const& right) const
    {
        //return (strcmp(m_or, right.m_or) < 0);
        //return (r<right.r 
        //    || (r==right.r && g<right.g) 
        //    || (r==right.r && b==right.b && b<<right.b)
        //);
        if(r != right.r) { return r < right.r; }
        if(g != right.g) { return g < right.g; }
        return b < right.b;
    }
};

struct Pattern // has width and height
{
    std::string label;
    std::vector<uint16_t> paletteIds;
};

struct PatternTable // order in list defined PatternId
{
    uint16_t w,h; //width and height per-tile
    std::vector<Pattern> patterns;
};

struct Palette
{
    std::map<uint16_t,RgbColor> colors;
};

struct Tile
{
    uint16_t tileId,hflip,vFlip,paletteId,layer;
};

struct Sprite
{
    uint16_t x,y,w,h;
};

struct Plane
{
    std::list<Tile*> tiles;
};

#endif
