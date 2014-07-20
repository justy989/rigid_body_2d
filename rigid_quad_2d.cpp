#include "rigid_quad_2d.hpp"

#include <cmath>

rigid_quad_2d::rigid_quad_2d ( const vec2& center,
						       float width,
						       float height,
						       float mass,
						       float rotation ) :
	m_width { width },
	m_height { height },
	m_half_width { width * 0.5f },
	m_half_height { height * 0.5f },
	m_mass { mass },
	m_inv_mass { 1.0f / mass },
	m_inertia { 0.0f },
	m_inv_inertia { 0.0f },
	m_rotation { rotation },
	m_total_torque { 0.0f },
	m_center { center }
{
	// inertia of a quad
	m_inertia = m_mass / 12.0f;
	m_inertia *= ( width * width + height * height );

	m_inv_inertia = 1.0f / m_inertia;

	update_corners ( );
}

void rigid_quad_2d::force ( const vec2& force )
{
	m_total_force += force;
}

void rigid_quad_2d::force ( const vec2& force,
                            const vec2& point )
{
	m_total_force += force;
	m_total_torque -= ( force.mag ( ) * m_center.perp_dot ( point ) );
}

void rigid_quad_2d::update ( float dt, float friction )
{
	// F = ma, a = dv/dt, v = dc/dt, c = c0 + F / m * dt ^ 2
	m_center += ( m_total_force * m_inv_mass ) * dt * dt;

	// T = wI, w = do/dt, o = dr/dt, r = r0 + T / I * dt ^ 2
	m_rotation += ( m_total_torque * m_inv_inertia ) * dt * dt;

	update_corners ( );

	// reduce forces due to friction
	m_total_force -= ( m_total_force * friction );
	m_total_torque -= ( m_total_torque * friction );
}

void rigid_quad_2d::update_corners ()
{
	// start at the origin and set up each corner
	m_corners[ 0 ].set ( -m_half_width, -m_half_height );
	m_corners[ 1 ].set (  m_half_width, -m_half_height );
	m_corners[ 2 ].set (  m_half_width,  m_half_height );
	m_corners[ 3 ].set ( -m_half_width,  m_half_height );

	float cos_rot = cos ( m_rotation );
	float sin_rot = sin ( m_rotation );

	// rotate and offset them by the center
	for ( unsigned int i = 0; i < k_num_corners; ++i ) {
		float rot_x = m_corners[ i ].x() * cos_rot - m_corners [ i ].y() * sin_rot;
		float rot_y = m_corners[ i ].y() * cos_rot + m_corners [ i ].x() * sin_rot;

		m_corners [ i ].set ( rot_x + m_center.x(),
		                      rot_y + m_center.y() );
	}
}