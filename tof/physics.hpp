#ifndef PHYSICS_HPP
#define PHYSICS_HPP

extern "C" {
	#include "raylib.h"

}
#include "mewall.h"

class KinematicBody {
public:
	float mass;
	vec2 force;
	vec2 velocity;
	vec2 position;
	float angular_velocity = 0.0f;
	float rotation = 0.0f;
	float target_rotation = 0.0f;

	KinematicBody() {}

	void applyForce(vec2 _force) {
		force += _force;
	}

	void applyForceX(float x) {
		force.x += x;
	}
	void applyForceY(float y) {
		force.y += y;
	}
	void setTargetRotation(float target_deg) {
		target_rotation = target_deg;
	}

	void move() {
		const float time_interval = GetFrameTime();
		// Update rotation
		rotation += angular_velocity * time_interval;
		
		// Adjust angular velocity to reach target rotation
		float rotation_diff = target_rotation - rotation;
		if (fabs(rotation_diff) < 5.0f) {
			angular_velocity = mew::lerp_value(angular_velocity, 0.0f, time_interval*10.0f);
		}
		if (fabs(rotation_diff) > 1.0f) {
			angular_velocity += rotation_diff * time_interval;
		} else {
			rotation = target_rotation;
		}

		if (force.isZero(0.1f)) { 
			velocity.lerp((vec2){0.0f, 0.0f}, time_interval);
		}
		if (force.isZero(0.01f)) { 
			force.zero();
			velocity.zero();
			velocity.lerp((vec2){0.0f, 0.0f}, time_interval);
			return;
		}
		vec2 acceleration = force / mass;
		vec2 delta_velocity = acceleration * time_interval; 
		position += velocity * time_interval;
		position += delta_velocity * time_interval / 2.0; 
		velocity += delta_velocity;
		force.lerp((vec2){0.0f, 0.0f}, time_interval);
	}

	const char* toString() {
		return TextFormat(
			"mass(%.2f), \n"
			"force{x:%f, y:%f}, \n"
			"velocity{x:%.2f, y:%.2f}, \n"
			"position{x:%.2f, y:%.2f}\n" 
			"rotation(%.0f)\n"
			"ang(%.2f)\n"
			"dt(%.2f)",
			mass, force.x, force.y, 
			velocity.x, velocity.y, 
			position.x, position.y,
			rotation, angular_velocity,
			GetFrameTime()*1000.0f
		);
	}

};

#endif