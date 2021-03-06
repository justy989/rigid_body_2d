#include <iostream>
#include <stdexcept>
#include <chrono>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "rigid_quad_2d.hpp"

using namespace std::chrono;

void draw_quad ( const rigid_quad_2d& quad );

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

	bool m_left_key_down;
	bool m_right_key_down;
	bool m_forward_key_down;
	bool m_backward_key_down;

    bool m_collided;

	rigid_quad_2d m_player;
	rigid_quad_2d m_attach;

    rigid_quad_2d m_obj;

    vec2 m_collided_point;
    vec2 m_collided_normal;
};

app::app ( const std::string& window_title,
		   int window_width,
	       int window_height ) :
	m_left_key_down { false },
	m_right_key_down { false },
	m_forward_key_down { false },
	m_backward_key_down { false },
    m_collided { false },
	m_player { vec2 { 0.0f, 0.0f },
		       0.15f, 0.2f,
		       0.2f, 0.0f },
    m_attach { vec2 { 0.15f, 0.2f },
	           0.05f, 0.1f,
	           0.07f, 0.0f },
    m_obj { vec2{ -0.3f, -0.3f },
            0.2f, 0.3f,
            0.3f, 0.0f }
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
            }else if ( sdl_event.type == SDL_KEYDOWN ) {
                switch ( sdl_event.key.keysym.sym ) {
                	case SDLK_ESCAPE:
                		done = true;
                		break;
                	case SDLK_w:
                		m_forward_key_down = true;
                		break;
            		case SDLK_s:
                		m_backward_key_down = true;
                		break;
                    case SDLK_a:
                		m_left_key_down = true;
                		break;
                	case SDLK_d:
                		m_right_key_down = true;
                		break;
                	default:
                	    break;
                }
            }else if ( sdl_event.type == SDL_KEYUP ) {
                switch ( sdl_event.key.keysym.sym ) {
                	case SDLK_w:
                		m_forward_key_down = false;
                		break;
            		case SDLK_s:
                		m_backward_key_down = false;
                		break;
                    case SDLK_a:
                		m_left_key_down = false;
                		break;
                	case SDLK_d:
                		m_right_key_down = false;
                		break;
                	default:
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
    rigid_quad_2d::collision_results res;

	vec2 force;

	if ( m_left_key_down ) {
		force += vec2 { -1.0f, 0.0f };
	}

	if ( m_right_key_down ) {
		force += vec2 { 1.0f, 0.0f };
	}

	if ( m_forward_key_down ) {
		force += vec2 { 0.0f, 1.0f };
	}

	if ( m_backward_key_down ) {
		force += vec2 { 0.0f, -1.0f };
	}

    m_player.push ( force );

    vec2 rope = m_player.corner ( 0 ) - m_attach.corner ( 0 );

    if( rope.mag () > 0.15f ) {
        vec2 rope_negation = rope;
        rope_negation.normalize ();
        rope_negation *= 0.1f;
        rope -= rope_negation;

        m_attach.pull ( -rope, m_attach.corner ( 0 ) );
        m_player.push ( -rope, m_player.corner ( 0 ) );
    }

    float friction = 0.1f;

    m_player.update ( dt, friction );
    m_attach.update ( dt, friction );
    m_obj.update ( dt, friction );

    rigid_quad_2d::collision ( m_player, m_attach, res );

    if( res.collided ) {
        m_collided = true;
        m_collided_point = res.point;
        m_collided_normal = res.normal;

        // vec2 pp = res.point - m_player.center ( );
        // vec2 ap = res.point - m_attach.center ( );

        // float j_num = ( pp - ap ).dot ( res.normal ) * -( 1.0f + 0.5f );
        // float j_denum = res.normal.dot ( res.normal * ( m_player.inv_mass() + m_attach.inv_mass() ) );
        
        // float j = j_num / j_denum;

        // m_player.impulse ( j, res.normal );
        // m_attach.impulse ( j, -res.normal );
    }
    else{
        m_collided = false;
    }
}

void app::render ( )
{
	glBegin ( GL_LINES );

    if( m_collided ){
        glColor3f ( 0.0f, 1.0f, 1.0f );
    }
    else{
        glColor3f ( 1.0f, 1.0f, 1.0f );
    }

	draw_quad ( m_player );
    draw_quad ( m_attach );
    draw_quad ( m_obj );

	glColor3f ( 1.0f, 0.0f, 0.0f );

    glVertex3f ( m_player.corner ( 0 ).x(), m_player.corner ( 0 ).y(), 0.0f );
	glVertex3f ( m_attach.corner ( 0 ).x(), m_attach.corner ( 0 ).y(), 0.0f );

    if ( m_collided ) {
        glVertex3f ( m_collided_point.x() - 0.1f, m_collided_point.y(), 0.0f );
        glVertex3f ( m_collided_point.x() + 0.1f, m_collided_point.y(), 0.0f );

        glVertex3f ( m_collided_point.x(), m_collided_point.y() - 0.1f, 0.0f );
        glVertex3f ( m_collided_point.x(), m_collided_point.y() + 0.1f, 0.0f );

        glColor3f ( 0.0f, 1.0f, 0.0f );

        glVertex3f ( m_collided_point.x(), m_collided_point.y(), 0.0f );
        glVertex3f ( m_collided_point.x() + m_collided_normal.x() * 0.15f,
                     m_collided_point.y() + m_collided_normal.y() * 0.15f,
                     0.0f );
    }

	glEnd ( );
}

void draw_quad ( const rigid_quad_2d& quad )
{
	for ( unsigned int i = 0; i < rigid_quad_2d::k_num_corners; ++i ) {
		unsigned int next = ( i + 1 ) % rigid_quad_2d::k_num_corners;

		glVertex3f ( quad.corner(i).x(), quad.corner(i).y(), 0.0f );
		glVertex3f ( quad.corner(next).x(), quad.corner(next).y(), 0.0f );
	}
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
