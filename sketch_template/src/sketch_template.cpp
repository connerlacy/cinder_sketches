#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class sketch_template : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
    void resize() override;
	void draw() override;
    
    cairo::SurfaceImage m_OnscreenBuffer;
    cairo::SurfaceImage m_OffscreenBuffer;
    cairo::Context      m_OffscreenContext;
    
    Perlin m_Perlin;
    
    
};

void sketch_template::setup()
{
    m_Perlin = Perlin(3, 357);
}

void sketch_template::mouseDown( MouseEvent event )
{
}

void sketch_template::resize()
{
    m_OnscreenBuffer   = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenBuffer  = cairo::SurfaceImage(getWindowWidth(), getWindowHeight(), false);
    m_OffscreenContext = cairo::Context(m_OffscreenBuffer);
}

void sketch_template::update()
{
}

void sketch_template::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    m_OffscreenContext.setSource(ColorA(1,1,1,1));
    m_OffscreenContext.paint();
    
    cairo::Context c(m_OnscreenBuffer);
    c.copySurface(m_OffscreenBuffer, m_OffscreenBuffer.getBounds());
    
    gl::draw( gl::Texture2d::create(m_OnscreenBuffer.getSurface()) );
    
}

CINDER_APP( sketch_template, RendererGl )
