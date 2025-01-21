#ifndef PARTICLES
#define PARTICLES
extern "C" {
	#include "raylib.h"
}
#include "mewall.h"
#include <unordered_map>
#include <vector>

using mew::vec2;

#pragma pack(push, 1)

struct Bound {
	vec2 min, max;
};

struct Particle {
	vec2  position      = (vec2){0,0};
	vec2  target        = (vec2){0,0};
	float	live_time     = 0.0f;
	float	max_live_time = 0.0f;
	float start_time    = 0.0f;
	float	rotation      = 0.0f;
	float size					= 1.0f;
	float speed					= 1.0f;
	Color color					= WHITE;
};

float GetRandomValue(float from, float to) {
	return from + (to-from)*(float)GetRandomValue(0, INT_MAX) / INT_MAX;
}

vec2 getRandom(Bound min, Bound max) {
	vec2 pos;
	while (pos.x > max.min.x && pos.x < max.max.x)
		pos.x = GetRandomValue(min.min.x, min.max.x);
	while (pos.y > max.min.y && pos.y < max.max.y)
		pos.y = GetRandomValue(min.min.y, min.max.y);
	return pos;
}

vec2 getRandom(Bound b) {
	vec2 pos;
	pos.x = GetRandomValue(b.min.x, b.max.x);
	pos.y = GetRandomValue(b.min.y, b.max.y);
	return pos;
}

vec2 getRandom(Rectangle min_rect, Rectangle max_rect) {
	return getRandom(
		(Bound){(vec2){min_rect.x, min_rect.y}, 
			(vec2){min_rect.x+min_rect.width, min_rect.y+min_rect.height}},
		(Bound){(vec2){max_rect.x, max_rect.y}, 
			(vec2){max_rect.x+max_rect.width, max_rect.y+max_rect.height}});
}

vec2 getRandom(Rectangle rect) {
	return getRandom((Bound){
		(vec2){rect.x, rect.y}, 
		(vec2){rect.x+rect.width, rect.y+rect.height}});
}

#pragma pack(pop)

class ParticleCluster {
private:
	size_t m_size = 0.0f;
	Particle* m_cluster;
	double spawn_time;
	float elapsed_time;
public:
	ParticleCluster() {}
	ParticleCluster(size_t size): 
		m_size(size), m_cluster(new Particle[size]()) {}

	// ParticleCluster(const ParticleCluster& ref) {
	// 	ParticleCluster(ref.m_size);
	// 	memcpy(m_cluster, ref.m_cluster, m_size*sizeof(Particle));
	// 	spawn_time = ref.spawn_time;
	// 	elapsed_time = ref.elapsed_time;
	// }

	ParticleCluster* clone(const ParticleCluster& ref) {
		ParticleCluster* cls = new ParticleCluster(ref.m_size);
		memcpy(cls->m_cluster, ref.m_cluster, m_size*sizeof(Particle));
		cls->spawn_time = ref.spawn_time;
		cls->elapsed_time = ref.elapsed_time;
		return cls;
	}
	
	ParticleCluster* clone() {
		ParticleCluster* cls = new ParticleCluster(m_size);
		memcpy(cls->m_cluster, m_cluster, m_size*sizeof(Particle));
		cls->spawn_time = spawn_time;
		cls->elapsed_time = elapsed_time;
		return cls;
	}
	
	void produce(Color color, 
		float max_live_time, 
		float max_rotation,
		float min_fade_time,
		float max_fade_time,
		float min_size,
		float max_size,
		Rectangle min_rect,
		Rectangle max_rect
	) {
		for (size_t i = 0; i < m_size; ++i) {
			m_cluster[i].color = color;
			m_cluster[i].max_live_time = max_live_time;
			m_cluster[i].rotation = GetRandomValue(0.0f, max_rotation);
			m_cluster[i].start_time = GetRandomValue(min_fade_time, max_fade_time);
			m_cluster[i].size = GetRandomValue(min_size, max_size);
			m_cluster[i].position = getRandom(min_rect);
			m_cluster[i].target = getRandom(max_rect);
			m_cluster[i].live_time = 0.0f;
			m_cluster[i].rotation = 0.0f;
			m_cluster[i].speed = 1.0f;
		}
	}

	void setPosition(vec2 target) {
		for (size_t i = 0; i < m_size; ++i) {
			m_cluster[i].position += target;
			m_cluster[i].target   += target; 
		}
	}

	void setColor(Color color) {
		for (size_t i = 0; i < m_size; ++i) {
			m_cluster[i].color = color;
		}
	}

	void setSpeed(float speed) {
		for (size_t i = 0; i < m_size; ++i) {
			m_cluster[i].speed = speed;
		}
	}

	void spawn() {
		spawn_time = GetTime();
	}

	void update() {
		elapsed_time = GetTime() - spawn_time; 
		float delta_time = GetFrameTime();
		for (size_t i = 0; i < m_size; ++i) {
			auto& current = m_cluster[i];
			if (elapsed_time >= current.start_time) {
				current.live_time = elapsed_time;
				current.position.lerp(current.target, delta_time*current.speed);
			}
		}
	}

	void render(Vector2 offset) {
		for (size_t i = 0; i < m_size; ++i) {
			auto& current = m_cluster[i];
			if (current.live_time > current.max_live_time+current.start_time) {
				continue;
			}
			DrawRectanglePro(
				(Rectangle){
					offset.x+current.position.x, offset.y+current.position.y, 
					current.size, current.size},
				(Vector2){
					(current.size/2.0f),
					(current.size/2.0f)},
				current.rotation,
				current.color
			);
		}
	}

	bool alive() {
		for (size_t i = 0; i < m_size; ++i) {
			if (m_cluster[i].live_time <= m_cluster[i].max_live_time+m_cluster[i].start_time) {
				return true;
			}
		}
		return false;
	}
};

class ParticleSystem {
public:
	std::unordered_map<const char*, ParticleCluster> collection;
	std::vector<ParticleCluster*> clusters;
	ParticleSystem() {}
	
	void add(const char* name, ParticleCluster cluster) {
		collection.insert({name, cluster});
	}

	void spawn(const char* name, vec2 target, float speed = 1.0f) {
		auto _cluster = collection.find(name);
		MewUserAssert(_cluster != collection.end(), "cannot find collection");
		clusters.push_back(_cluster->second.clone());
		clusters.back()->setPosition(target);
		clusters.back()->setSpeed(speed);
		clusters.back()->spawn();
	}
	
	void spawn(const char* name, vec2 target, Color color, float speed = 1.0f) {
		auto _cluster = collection.find(name);
		MewUserAssert(_cluster != collection.end(), "cannot find collection");
		clusters.push_back(_cluster->second.clone());
		clusters.back()->setPosition(target);
		clusters.back()->setColor(color);
		clusters.back()->setSpeed(speed);
		clusters.back()->spawn();
	}

	void update() {
		size_t size = clusters.size();
		for (size_t i = 0; i < size; ++i) {
			clusters[i]->update();
			if (!clusters[i]->alive()) {
				delete clusters[i];
				clusters.erase(clusters.begin()+i);
			}
		}
	}

	void render(Vector2 offset) {
		for (auto& cluster: clusters) {
			cluster->render(offset);
		}
	}
};


#endif