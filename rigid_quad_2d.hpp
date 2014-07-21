#ifndef RIGID_QUAD_2D
#define RIGID_QUAD_2D

#include "vector2.hpp"

class rigid_quad_2d {
public:
	rigid_quad_2d ( const vec2& center,
			        float width,
			        float height,
			        float mass,
			        float roation = 0.0f );


	// apply a force evenly to the center of mass
	void force ( const vec2& force );

	// apply a force on a point relative to the object
	void force ( const vec2& force,
			     const vec2& point );

    // used in collision resolution, impulse the force and torque in
    // the direction of the normal using the coefficient of restitution
    void impulse ( const vec2& point,
                   const vec2& normal,
                   const vec2& collider_force );

	// update the object over time, decaying the force
	// and torque with friction
	void update ( float dt, float friction );
    
    struct collision_results {
        bool collided;
        vec2 point;
        vec2 normal;
    };

    static void collision ( const rigid_quad_2d& a,
                            const rigid_quad_2d& b,
                            collision_results& res );

	static const unsigned int k_num_corners = 4;

	inline float width ( ) const;
	inline float height ( ) const;

	inline float mass ( ) const;
	inline float inertia ( ) const;
	inline float rotation ( ) const;

	inline const vec2& center ( ) const;
	inline const vec2& corner ( unsigned int index ) const;
	inline const vec2& total_force ( ) const;
	inline float total_torque ( ) const;

private:

	void update_corners ();

    static bool is_point_inside_quad ( const vec2& p,
                                       const rigid_quad_2d& quad,
                                       vec2& collision_normal );

private:

	float m_width;
	float m_height;
	float m_half_width;
	float m_half_height;

	float m_mass;
	float m_inv_mass;

	float m_inertia;
	float m_inv_inertia;

	float m_rotation;

	vec2 m_total_force;
	float m_total_torque;

	vec2 m_center;
	vec2 m_corners[k_num_corners];
};

inline float rigid_quad_2d::width ( ) const { return m_width; }
inline float rigid_quad_2d::height ( ) const { return m_height; }

inline float rigid_quad_2d::mass ( ) const { return m_mass; }
inline float rigid_quad_2d::inertia ( ) const { return m_inertia; }
inline float rigid_quad_2d::rotation ( ) const { return m_rotation; }

inline const vec2& rigid_quad_2d::center ( ) const { return m_center; }
inline const vec2& rigid_quad_2d::corner ( unsigned int index ) const { return m_corners [ index ]; }
inline const vec2& rigid_quad_2d::total_force ( ) const { return m_total_force; }
inline float rigid_quad_2d::total_torque ( ) const { return m_total_torque; }

#endif
