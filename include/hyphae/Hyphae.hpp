//
//  Hyphae.hpp
//  sketches
//
//  Created by connerlacy on 12/11/16.
//
//

#ifndef Hyphae_hpp
#define Hyphae_hpp

#include <stdio.h>
#include <chrono>
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class VeinNode
{
public:
    VeinNode(vec2 position, float radius = 10, vec2 direction = ivec2(0,1)) : m_Radius(radius), m_Position(position), m_Direction(direction){}
    
    VeinNode createNewNode(bool isBranch = false)
    {
        vec2 newDirection = m_Direction + vec2( randFloat(-m_DirectionModAmt,m_DirectionModAmt), randFloat(-m_DirectionModAmt, m_DirectionModAmt));
        float newRadius = m_Radius * 0.99;
        
        if(isBranch)
        {
            vec3 c = cross(vec3(newDirection,1.0), vec3(0,0,1));
            newDirection = vec2(c.x,c.y) + vec2( randFloat(-m_DirectionModAmt,m_DirectionModAmt), randFloat(-m_DirectionModAmt, m_DirectionModAmt));
            newRadius *= 0.8;
        }
        
        vec2 newPosition  = m_Position + normalize(newDirection) * (newRadius + m_Radius);
        return VeinNode(newPosition, newRadius, newDirection);
    }
    
    float getRadius(){return m_Radius;}
    vec2  getPosition(){return m_Position;}
    vec2  getDirection(){return m_Direction;}
    
private:
    float m_Radius;
    vec2  m_Position;
    vec2  m_Direction;
    float m_DirectionModAmt = 0.0;
    
};


class Hyphae
{
public:
    Hyphae()
    {
        std::vector<VeinNode> branch = { VeinNode(getWindowCenter()) };
        std::vector<VeinNode> branch2 = { VeinNode(getWindowCenter() + vec2(-50, -80), 10, vec2(0,-1) ) };
        m_Veins.push_back( branch );
        //m_Veins.push_back( branch2 );
    }
    
    void update()
    {
        std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
        randSeed(ms.count());
        
        // Grow
        for(std::vector<std::vector<VeinNode>>::iterator it = m_Veins.begin(); it != m_Veins.end(); it++)
        {
            VeinNode potentialNode = it->back().createNewNode();
            
            if( abs( distance(potentialNode.getPosition(), getWindowCenter()) ) > (200.0f - potentialNode.getRadius()) )
            {
                break;
            }
            
            if(nodeOverlaps(potentialNode))
            {
                break;
            }
            
            it->push_back(potentialNode);
        }
        
        // Branch
        int veinIdx = randInt( m_Veins.size() );
        int nodeIdx = randInt(m_Veins[veinIdx].size());
        VeinNode randomNode = m_Veins[veinIdx][nodeIdx].createNewNode(true);
        
        if( abs( distance(randomNode.getPosition(), getWindowCenter()) ) > (200.0f - randomNode.getRadius()) )
        {
            return;
        }
        
        if(nodeOverlaps(randomNode))
        {
            return;
        }
        
        std::vector<VeinNode> branch = { VeinNode(randomNode) };
        m_Veins.push_back( branch );
    }
    
    bool nodeOverlaps(VeinNode& node)
    {
        for(std::vector<std::vector<VeinNode>>::iterator it = m_Veins.begin(); it != m_Veins.end(); it++)
        {
            for(std::vector<VeinNode>::iterator itn = it->begin(); itn != it->end(); itn++)
            {
                int requiredDistance = itn->getRadius() + node.getRadius();
                int dist = abs( distance( itn->getPosition(), node.getPosition() ) );
                
                //console() << "Required: " << requiredDistance << "     Actual: " << dist << "\n";
                
                if(  dist < requiredDistance )
                {
                    
                    return true;
                }
            }
        }
        
        return false;
    }
    
    std::vector< std::vector<VeinNode> > m_Veins;
    float m_Boundary = 200.0f;
    
};


#endif /* Hyphae_hpp */
