
#include "sketches.h"

#ifdef SKETCH_2016_11_26

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/cairo/cairo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define NUM_PARTICLES 300

class sketch_2016_11_26 : public App {
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
    
    Perlin m_PerlinX[NUM_PARTICLES], m_PerlinY[NUM_PARTICLES], m_Perlin;
    vec2 m_Positions[NUM_PARTICLES];
    
    void reset();
    
    
};

void prepareSettings(App::Settings *settings)
{
    settings->setFullScreen(true);
}

void sketch_2016_11_26::setup()
{
    std::time_t seconds = std::time(0);
    m_Perlin = Perlin(4, seconds);
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        m_PerlinX[i] = Perlin(4, seconds * i);
        m_PerlinY[i] = Perlin(4, sqrt(seconds) * i);
    }

    
    reset();
    
 
}

void sketch_2016_11_26::reset()
{
    for(int i=0; i < NUM_PARTICLES; i++)
    {
        m_Positions[i] = getWindowCenter();
    }
    
    m_OnscreenBuffer   = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenBuffer  = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenContext = cairo::Context(m_OffscreenBuffer);
    

}

void sketch_2016_11_26::mouseDown( MouseEvent event )
{
    reset();
}

void sketch_2016_11_26::resize()
{
    m_OnscreenBuffer   = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenBuffer  = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenContext = cairo::Context(m_OffscreenBuffer);
}

void sketch_2016_11_26::update()
{
    
    float stepSize = getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth();
    stepSize *= 0.99;
    float stepX = 0;
    float stepY = 0;
    
    float timeDivisor = 1.0f;
    float freq = 1 / 25.0;
    
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        float t = getElapsedSeconds()/timeDivisor;
        
        float px = m_Perlin.fBm(vec2(i, t) * freq + 1.0f);
        float py = m_Perlin.fBm(vec2(i + NUM_PARTICLES, t) * freq + 1.0f);
        
        stepX = px;
        stepY = py;

        m_Positions[i] = vec2(stepX*stepSize, stepY*stepSize) + getWindowCenter();
    }
}

void sketch_2016_11_26::draw()
{
	gl::clear( Color( 1, 1, 1 ) );
    

    
    render(m_OffscreenContext);
    
    cairo::Context c(m_OnscreenBuffer);
    c.copySurface(m_OffscreenBuffer, m_OffscreenBuffer.getBounds());
    
    gl::draw( gl::Texture2d::create(m_OnscreenBuffer.getSurface()) );
    
}



void sketch_2016_11_26::render(cairo::Context &c)
{
    float distThresh = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.045;
    float radius = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.0035;
    float lineWidth = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.001;
    float alpha = 0.33;
    
    //c.setOperator(17);
    c.setSource(ColorA(125.0f/255.0f,209.0f/255.0f,208.0f/255.0f,0.4));
    c.paint();
    
    c.setSource(ColorA(226.0f/255.0f,78.0f/255.0f,61.0f/255.0f,0.4));
    
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        c.circle(m_Positions[i], radius);
        c.fill();
    }
    
    c.setSource(ColorA(226.0f/255.0f,78.0f/255.0f,61.0f/255.0f,0.4));
    for(int i = 0; i < NUM_PARTICLES; i++)
    {
        float dist = 0.0;
        vec2 start = m_Positions[i];
        
        for(int j = i+1; j < NUM_PARTICLES; j++)
        {
            dist = distance(start, m_Positions[j]);
            
            if(dist < distThresh)
            {
                c.setLineWidth( lineWidth );
                c.line(start, m_Positions[j]);
                c.stroke();
            }
        }
    }
    
}

CINDER_APP( sketch_2016_11_26, RendererGl, prepareSettings )

#endif
