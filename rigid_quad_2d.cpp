#include "rigid_quad_2d.hpp"

#include <cmath>
#include <limits>

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
    m_angular_velocity { 0.0f },
	m_total_torque { 0.0f },
	m_center { center }
{
	// inertia of a quad
	m_inertia = m_mass / 12.0f;
	m_inertia *= ( width * width + height * height );

	m_inv_inertia = 1.0f / m_inertia;

	update_corners ( );
}

void rigid_quad_2d::push ( const vec2& force )
{
	m_total_force += force;
}

void rigid_quad_2d::pull ( const vec2& force )
{
	m_total_force -= force;
}

void rigid_quad_2d::push ( const vec2& force,
                            const vec2& point )
{
	m_total_force += force;
	m_total_torque += ( force.mag ( ) * m_center.perp_dot ( point ) );
}

void rigid_quad_2d::pull ( const vec2& force,
                            const vec2& point )
{
	m_total_force -= force;
	m_total_torque -= ( force.mag ( ) * m_center.perp_dot ( point ) );
}

void rigid_quad_2d::impulse ( float impulse,
                              const vec2& normal )
{
    m_velocity += ( normal * ( impulse / m_mass ) );
}

void rigid_quad_2d::update ( float dt, float friction )
{
	// F = ma, a = dv/dt, v = dc/dt, c = c0 + F / m * dt ^ 2
    m_center += m_velocity * dt;
    m_velocity = ( m_total_force * m_inv_mass ) * dt; 

	// T = wI, w = do/dt, o = dr/dt, r = r0 + T / I * dt ^ 2
    m_rotation += m_angular_velocity * dt;
    m_angular_velocity = ( m_total_torque * m_inv_inertia ) * dt;

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

void rigid_quad_2d::collision ( const rigid_quad_2d& a,
                                const rigid_quad_2d& b,
                                collision_results& res )
{
    // for each corner, test if it is inside the other shape
    for ( unsigned int i = 0; i < k_num_corners; ++i ) {
        if ( is_point_inside_quad ( a.m_corners[i],
                                    b,
                                    res.normal ) ) {
            res.collided = true;
            res.point = a.m_corners [ i ];
            res.normal.normalize ( );
            res.normal.negate ( );
            return;
        }

        if ( is_point_inside_quad ( b.m_corners[i],
                                    a,
                                    res.normal ) ) {
            res.collided = true;
            res.point = b.m_corners [ i ];
            res.normal.normalize ( );
            res.normal.negate ( );
            return;
        }
    }

    res.collided = false;
}

bool rigid_quad_2d::is_point_inside_quad ( const vec2& p,
                                           const rigid_quad_2d& quad,
                                           vec2& collision_normal )
{
    vec2 edge;
    vec2 trans_p;
    vec2 closest_normal;
    vec2 edge_proj;
    vec2 normal;
    float closest_normal_dist = std::numeric_limits<float>::max();

    // build each edge
    for ( unsigned int i = 0; i < k_num_corners; ++i ) {
        unsigned int next = ( i + 1 ) % k_num_corners;

        // A = -(y2 - y1)
        // B = x2 - x1
        // C = -(A * x1 + B * y1)
        const vec2& first = quad.m_corners [ i ];
        const vec2& second = quad.m_corners [ next ];

        float a = -( second.y ( ) - first.y ( ) );
        float b = second.x ( ) - first.x ( );
        float c = -( a * first.x ( ) + b * first.y ( ) );

        // D = A * xp + B * yp + C
        float d = a * p.x ( ) + b * p.y ( ) + c;

        if ( d < 0.0f ) {
            return false;
        }

        // find the edge vector
        edge = quad.m_corners [ next ] - quad.m_corners [ i ];

        // project the point onto the edge
        trans_p = quad.m_corners [ next ] - p;
        edge_proj = trans_p.project_onto ( edge );
        normal = edge_proj - trans_p;

        d = normal.mag ( );

        // find the distance from p to that projected point
        if ( d < closest_normal_dist ) {
            closest_normal_dist = d;
            collision_normal = normal;
        }
    }

    return true;
}

