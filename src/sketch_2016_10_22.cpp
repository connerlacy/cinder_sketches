
#include "sketches.h"

#ifdef SKETCH_2016_10_22

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_PARTICLES 100

class sketch_2016_10_22 : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
    void resize() override;
	void draw() override;
    
    cairo::SurfaceImage m_OnscreenBuffer;
    cairo::SurfaceImage m_OffscreenBuffer;
    cairo::Context      m_OffscreenContext;
    
    void render(cairo::Context &c);
    
    Perlin m_Perlin[NUM_PARTICLES];
    
    
};

void sketch_2016_10_22::setup()
{
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        m_Perlin[i] = Perlin(3, 357 * (i + 1));
    }
}

void sketch_2016_10_22::mouseDown( MouseEvent event )
{
}

void sketch_2016_10_22::resize()
{
    m_OnscreenBuffer   = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenBuffer  = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenContext = cairo::Context(m_OffscreenBuffer);
}

void sketch_2016_10_22::update()
{
}

void sketch_2016_10_22::draw()
{
	gl::clear( Color( 1, 1, 1 ) );
    

    
    render(m_OffscreenContext);
    
    cairo::Context c(m_OnscreenBuffer);
    c.copySurface(m_OffscreenBuffer, m_OffscreenBuffer.getBounds());
    
    gl::draw( gl::Texture2d::create(m_OnscreenBuffer.getSurface()) );
    
}



void sketch_2016_10_22::render(cairo::Context &c)
{
    // Make sure background is actually white;
    static bool once = false;
    if(!once)
    {
        once = true;
        //m_OffscreenContext.setSource(ColorA(1,0.8,0.0,1.0));
        //m_OffscreenContext.paint();
    }
    
    c.setSource(ColorA(1,0.9,0.7,0.025));
    c.rectangle(getWindowBounds());
    c.fill();
    
    
    for(float i = 1; i < NUM_PARTICLES; i++)
    {
        float timeDivide = 15.0f;
        float x = i * getWindowWidth() / NUM_PARTICLES;
        float y = m_Perlin[int(i)].fBm(getElapsedSeconds()/(timeDivide));
        float radius = getWindowWidth() / (NUM_PARTICLES + 10);
        
        
        y *= getWindowHeight()/2.0;
        y += getWindowHeight()/2.0;
        /*
        c.setSource(ColorA(0.5,0.0,0.5,0.1));
        c.line(vec2(x, getWindowCenter().y), vec2(x,y));
        c.stroke();
        
        c.circle(x, getWindowCenter().y, 3.0f);
        c.fill();
        */
        
        c.setSource(ColorA(0.5,0.0,0.5,1));
        c.circle(x, y, radius/2.0);
        c.fill();
    }
}

CINDER_APP( sketch_2016_10_22, RendererGl )

#endif

