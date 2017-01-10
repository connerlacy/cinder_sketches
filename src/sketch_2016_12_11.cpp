
#include "sketches.h"

#ifdef SKETCH_2016_12_11

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/cairo/cairo.h"
#include "glm/gtc/random.hpp"

#include "Hyphae.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_PARTICLES 21
#define NUM_HORMONES 100
#define NUM_ROOTS   1

class sketch_2016_12_10 : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseMove(MouseEvent event)override;
    void mouseDrag(MouseEvent event)override;
    void resize() override;
    void draw() override;
    
    cairo::SurfaceImage m_OnscreenBuffer;
    cairo::SurfaceImage m_OffscreenBuffer;
    cairo::Context      m_OffscreenContext;
    
    void render(cairo::Context &c);
    
    Perlin m_PerlinRGB[3], m_Perlin;
    
    void   reset();
    ColorA randColor();
    vec2   randPosition();
    
    bool   m_MouseDown = false;
    
    ivec2  m_MousePos;
    
    bool   m_RepaintBackgroung = true;
    ColorA m_Color;
    int    m_ColorClock = 1;
    
    bool   m_Paint = false;
    int    m_PaintClock = 1;
    int    m_PaintOnOffCounter;
    
    vec2   m_Position;
    int    m_PositionClock = 1;
    
    float m_CircleRadius;
    float m_PathRadius;
    float m_PathRadiusOffset;
    
    std::time_t m_Epoch;
    
    bool m_NightMode = true;
    
    
    std::vector<vec2> m_Hormones;
    std::vector<vec2> m_Roots;
    float             m_DiskRadius = 300.0f;
    float             m_HormoneRadius = m_DiskRadius/(NUM_HORMONES*2.0);
    
    Hyphae m_Hyphae;
    
};

void prepareSettings(App::Settings *settings)
{
    settings->setFullScreen(false);
}

ColorA sketch_2016_12_10::randColor()
{
    float time = getElapsedSeconds()/0.5;
    float mult = 4.0f;
    ColorA color; // = ColorA(, abs(m_PerlinRGB[1].fBm(2.0f, time)) * mult, abs(m_PerlinRGB[2].fBm(3.0f, time)) * mult );
    
    for(int i = 0; i < 3; i++)
    {
        color[i] = std::min(1.0f , (float)abs(m_PerlinRGB[0].fBm( i + 1.0f, time)) * mult + 0.3f );
    }
    
    return color;
}

vec2 sketch_2016_12_10::randPosition()
{
    float stepSize = getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth();
    stepSize *= 0.99;
    float stepX = 0;
    float stepY = 0;
    
    float timeDivisor = 1.0f;
    float freq = 1 / 205.0;
    
    float t = getElapsedFrames()/timeDivisor;
    
    int r = randInt(100);
    
    float px = m_Perlin.fBm(vec2(r, t) * freq + 1.0f);
    float py = m_Perlin.fBm(vec2(r + r, t) * freq + 1.0f);
    
    stepX = px;
    stepY = py;
    
    m_Position = vec2(stepX*getWindowWidth(), stepY*getWindowHeight()) + getWindowCenter();
    
    m_Position = vec2(randInt(getWindowWidth()), randInt(getWindowHeight()));
    
    randSeed(m_Epoch + getElapsedFrames());
    
    return m_Position;
    
    
    
}

void sketch_2016_12_10::setup()
{
    m_Epoch = std::time(0);
    m_Perlin = Perlin(2, m_Epoch);
    for(int i = 0; i < 3; i++)
    {
        m_PerlinRGB[i] = Perlin(4, m_Epoch * i);
    }
    
    randSeed(m_Epoch);
    
    m_NightMode = randBool();
    
    m_Position = getWindowCenter();
    
    reset();
    
    for(int i =0;i < NUM_HORMONES; i++)
    {
        m_Hormones.push_back( glm::diskRand(300.0f) );
    }
    
    for(int i = 0; i < NUM_ROOTS; i++)
    {
        m_Roots.push_back(glm::diskRand(300.0f));
    }
    
    for(int i =0; i < 100; i++)
    {
        //m_Hyphae.update();
    }
}

void sketch_2016_12_10::reset()
{
    m_OnscreenBuffer   = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenBuffer  = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenContext = cairo::Context(m_OffscreenBuffer);
    
    m_RepaintBackgroung = true;
}

void sketch_2016_12_10::mouseDown( MouseEvent event )
{
    m_MouseDown = true;
    reset();
}

void sketch_2016_12_10::mouseUp( MouseEvent event )
{
    m_MouseDown = false;
}

void sketch_2016_12_10::mouseMove( MouseEvent event )
{
    m_MousePos = event.getPos();
}

void sketch_2016_12_10::mouseDrag( MouseEvent event )
{
    m_MousePos = event.getPos();
}

void sketch_2016_12_10::resize()
{
    reset();
}

void sketch_2016_12_10::draw()
{
    gl::clear( Color( 1, 1, 1 ) );
    
    if(m_RepaintBackgroung)
    {
        m_RepaintBackgroung = false;
        m_OffscreenContext.setSource(ColorA(1,1,1,1));
        m_OffscreenContext.paint();
    }
    
    render(m_OffscreenContext);
    
    cairo::Context c(m_OnscreenBuffer);
    c.copySurface(m_OffscreenBuffer, m_OffscreenBuffer.getBounds());
    
    gl::draw( gl::Texture2d::create(m_OnscreenBuffer.getSurface()) );
}

void sketch_2016_12_10::render(cairo::Context &c)
{
    //m_OffscreenContext.setSource(ColorA(1,1,1,1));
    //m_OffscreenContext.painrt();
    
    c.setSource(ColorA(0,0,0,1));
    c.circle(getWindowCenter(), m_Hyphae.m_Boundary);
    c.stroke();
    
    //console() << m_Hyphae.getAllNodes().size() << "\n";
    
    m_Hyphae.update();
    
    for(int i = 0; i < m_Hyphae.m_Veins.size(); i++)
    {
        c.circle(m_Hyphae.m_Veins[i].back().getPosition(), m_Hyphae.m_Veins[i].back().getRadius());
    }
    
    for(int i =0;i < NUM_ROOTS; i++)
    {
       // c.circle(m_Roots[i] + (vec2)getWindowCenter(), 20.0f);
    }
    c.fill();
}


CINDER_APP( sketch_2016_12_10, RendererGl, prepareSettings )

#endif
