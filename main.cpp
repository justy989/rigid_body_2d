#include <iostream>
#include <stdexcept>
#include <chrono>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

using namespace std::chrono;

class app {
public:
	app ( const std::string& window_title,
		  int window_width,
	      int winddow_height );
	~app ( );

	void run ( );

private:

	void update ( float dt );
	void render ( );

	SDL_Window* m_window;
	SDL_GLContext m_gl_context;
};

app::app ( const std::string& window_title,
		   int window_width,
	       int window_height )
{

	if ( SDL_Init ( SDL_INIT_EVERYTHING ) ) {
		throw std::runtime_error ( std::string( "SDL_Init() failed: " ) + SDL_GetError() );
	}

	m_window = SDL_CreateWindow ( window_title.c_str(),
                   	              SDL_WINDOWPOS_CENTERED,
	                              SDL_WINDOWPOS_CENTERED,
	                              window_width,
	                              window_height,
	                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );

	if ( !m_window ) {
		throw std::runtime_error ( std::string( "SDL_CreateWindow() failed: " ) + SDL_GetError() );
	}

	m_gl_context = SDL_GL_CreateContext ( m_window );

	// some openGL setup
    // set clear color and clear depth
    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

    // initialize projection and modelview matricies
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity ( );
    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );

    // set the viewport
    glViewport ( 0, 0, window_width, window_height );
    glOrtho ( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
}

app::~app ( )
{
    SDL_DestroyWindow ( m_window );
	SDL_Quit ( );
}

void app::run ( )
{
	SDL_Event sdl_event { };
	bool done { false };

	// start timer
	auto frame_start = high_resolution_clock::now();
	auto frame_end = frame_start;
	long long ms_dt = 0;
	float dt = 0.0f;

	while( !done ) {
        // calculate time between frames

        // capture the time
		frame_end = high_resolution_clock::now ( );

		// find the change in time in seconds as a decimal
		ms_dt = duration_cast<microseconds> ( frame_end - frame_start ).count ( );
        dt = static_cast<float> ( ms_dt ) / 1000000.0f;

        // save off the last time snapshot
        frame_start = frame_end;

        // handle events
        while ( SDL_PollEvent ( &sdl_event ) != 0 ) {
            if ( sdl_event.type == SDL_QUIT ) {
                done = true;
                break;
            }
            else if ( sdl_event.type == SDL_KEYDOWN ) {
                if ( sdl_event.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                    break;
                }
            }
        }

        // update
        update ( dt );

        // render: clear buffers, render, swap buffers
        glClear ( GL_COLOR_BUFFER_BIT );
        render ( );
        SDL_GL_SwapWindow ( m_window );
    }
}

void app::update ( float dt )
{
	dt += 0.01f;
}

void app::render ( )
{
	glBegin ( GL_LINES );

	glColor3f ( 1.0f, 1.0f, 1.0f );

	glVertex3f ( 0.0f, 0.0f, 0.0f );
	glVertex3f ( 0.5f, 0.5f, 0.0f );

	glEnd ( );
}

int main ( int argc, char** argv ) {

	// do something unuseful with these
	for ( int i = 0; i < argc; i++ ) {
		argv++;
	}

	try {
		app a { "rigid_body_2d", 1024, 768 };
		a.run ();
	} catch ( const std::runtime_error& e ) {
		std::cerr << e.what ( ) << std::endl;
	}

	return 0;
}