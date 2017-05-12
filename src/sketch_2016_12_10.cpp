
#include "sketches.h"

#ifdef SKETCH_2016_12_10

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/cairo/cairo.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/gl/Shader.h"
#include "cinder/qtime/AvfWriter.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace std::chrono;


#define NUM_PARTICLES 21
#define NUM_SPACES 4

class sketch_2016_12_10 : public App {
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseMove(MouseEvent event)override;
    void mouseDrag(MouseEvent event)override;
    void keyDown(KeyEvent event) override;
    void resize() override;
    void draw() override;
    
    cairo::SurfaceImage m_OnscreenBuffer;
    cairo::SurfaceImage m_OffscreenBuffer;
    cairo::Context      m_OffscreenContext;
    
    void render(cairo::Context &c);
    
    Perlin m_PerlinRGB[3], m_Perlin;
    
    void   reset();
    ColorA randColor();
    vec2   newOrigin(int randomOrCenter);
    
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
    
    Font m_Font;
    
    string startTime;
    string startTimeS;
    string endTime;
    string endTimeS;
    
    float  matScale = 1.0f;
    float  partcileScale = 1.25f;
    
};

void prepareSettings(App::Settings *settings)
{
    settings->setFullScreen(false);
    settings->setWindowSize(800,800);
    settings->setBorderless(true);
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

vec2 sketch_2016_12_10::newOrigin(int randomOrCenter)
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
    m_Font = Font("NotCourierSans", 9.57);
    m_Epoch = std::time(0);
    m_Perlin = Perlin(2, m_Epoch);
    for(int i = 0; i < 3; i++)
    {
        m_PerlinRGB[i] = Perlin(4, m_Epoch * i);
    }
    
    randSeed(m_Epoch);
    
    m_NightMode = false;//randBool();
    
    m_Position = getWindowCenter();
    
    reset();
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
    
    milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    startTime = to_string(ms.count());
    
    string spaces;
    
    for(auto s : startTime)
    {
        //std::cout << s << "\n";
        
        spaces.push_back(s);
        
        for(int i = 0; i < NUM_SPACES; i++)
        {
            spaces.append(" ");
        }
    }
    
    startTimeS = spaces;
    
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

void sketch_2016_12_10::keyDown(cinder::app::KeyEvent event)
{
    if(event.getChar() == 's')
    {
        milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
        endTime = to_string(ms.count());
        endTimeS.clear();
        
        for(auto s : endTime)
        {
            endTimeS.push_back(s);
            
            for(int i = 0; i < NUM_SPACES; i++)
            {
                endTimeS.append(" ");
            }
        }
        
        
        draw();
        
        Surface s = copyWindowSurface();
        writeImage("/Users/connerlacy/Desktop/exports/" + startTime + "_" + endTime + ".png", s);
    }
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
        if(!m_NightMode)
        {
            m_OffscreenContext.setSource(ColorA(1,1,1,1));
        }
        else
        {
            m_OffscreenContext.setSource(ColorA(0,0,0.08,1));
        }
        
        m_OffscreenContext.paint();
        
        m_OffscreenContext.setSource(ColorA(1,0.6,0.6));
        m_OffscreenContext.rectangle(24, 24, 756, 756);
    }
    
    render(m_OffscreenContext);
    
    cairo::Context c(m_OnscreenBuffer);
    c.copySurface(m_OffscreenBuffer, m_OffscreenBuffer.getBounds());
    
    gl::draw( gl::Texture2d::create(m_OnscreenBuffer.getSurface()) );

    
    gl::drawString(startTimeS + ":    " + endTimeS, vec2(25,9), ColorA(0.0,0.0,0.0), m_Font);
}

void sketch_2016_12_10::render(cairo::Context &c)
{
    //float radius = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.05;
    float radMin = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.01;
    float radMax = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.06;
    float pathRadMin = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.05;
    float pathRadMax = (getWindowWidth() > getWindowHeight() ? getWindowHeight() : getWindowWidth()) * 0.4;
    //float lineWidth   = 10.0f;
    float alpha       = 0.75;
    
    if(m_NightMode){alpha = 0.03;}
    
    // Color Clock
    if( (getElapsedFrames() % m_ColorClock) == 0)
    {
        m_Color = ColorA(randColor(), alpha); m_ColorClock = randInt(10,20);
    }
    
    // Paint Clock
    if(getElapsedFrames() % m_PaintClock == 0)
    {
        m_Paint = !m_Paint; m_PaintClock = randInt(20, 60);
        m_PathRadiusOffset  = m_Perlin.fBm(randFloat(0.08,0.12),(float)getElapsedFrames()/3.0) * 30.0;
        /*
        if(!m_Paint)
        {
            m_PaintOnOffCounter++;
            
            // Position Clock
            if( (m_PaintOnOffCounter % m_PositionClock) == 0)
            {
                randPosition();
                m_PositionClock = randInt(3,7);
            }
        }
         */
    }
    
    if( (getElapsedFrames() % m_PositionClock) == 0)
    {
        m_Position = getWindowCenter();
        //newOrigin(0);
        m_PositionClock = randInt(180,500);
    }
    
    c.setSource(ColorA(m_Color,alpha));
    
    //if(m_Paint)
    {
        m_CircleRadius  = m_Perlin.fBm(0.4,(float)getElapsedFrames()/30.3) * radMax + radMin;
        m_PathRadius    = m_Perlin.fBm(0.5,(float)getElapsedFrames()/80.0) * pathRadMax + pathRadMin - m_CircleRadius/2.0;
        
        m_CircleRadius *= partcileScale;
        m_PathRadius   *= partcileScale;
        
        Path2d path;
        path.moveTo(vec2(0,m_PathRadius + m_PathRadiusOffset));
        
        mat3 m;
        m = translate(m, vec2(m_Position));
        m = rotate(m, (float)M_PI*5.0f * m_Perlin.fBm(0.1,(float)getElapsedFrames()/100.0 + 1.0f));
        m = scale(m, vec2(matScale));
        path.transform(m);
        
        //c.circle(path.getPoints()[0], m_CircleRadius);
        c.circle(path.getPoints()[0], m_CircleRadius);
        c.fill();
    }
    
    //c.setSource(ColorA(0.1,0.1,0.1));
    //c.showText("test");
}



CINDER_APP( sketch_2016_12_10, RendererGl, prepareSettings )

#endif
