
#include "sketches.h"

#ifdef SKETCH_2016_12_111

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/cairo/cairo.h"
#include "glm/gtc/random.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_PARTICLES 21
#define NUM_HORMONES 100
#define NUM_ROOTS   1

class sketch_2016_12_10 : public App {
public:
    void setup() override;
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
    
};

void prepareSettings(App::Settings *settings)
{
    settings->setFullScreen(false);
}

void sketch_2016_12_10::setup()
{
    std::time_t m_Epoch = std::time(0);
    m_Perlin = Perlin(2, m_Epoch);
}

void sketch_2016_12_10::draw()
{
    gl::clear( Color( 1, 1, 1 ) );
    
    if(m_RepaintBackgroung)
    {
        m_RepaintBackgroung = false;
        

    }
    
    m_OffscreenContext.setSource(ColorA(1,1,1,1));
    m_OffscreenContext.paint();
    
    render(m_OffscreenContext);
    
    cairo::Context c(m_OnscreenBuffer);
    c.copySurface(m_OffscreenBuffer, m_OffscreenBuffer.getBounds());
    
    gl::draw( gl::Texture2d::create(m_OnscreenBuffer.getSurface()) );
}

void sketch_2016_12_10::render(cairo::Context &c)
{

    

    for(float r = 0; r < getWindowHeight(); r++)
    {
        for(float col = 0; col < getWindowWidth(); col++)
        {
            float v = m_Perlin.fBm(r,col);
            v = v * 0.5 + 0.5;
            
            v = std::max(0.0f, std::min(1.0f, v));
            
            c.setSource(ColorA(0.0,1.0,1.0));
            //c.rectangle(r, col, 1, 1);
            c.fill();
        }
    }


}


CINDER_APP( sketch_2016_12_10, RendererGl, prepareSettings )

#endif
