#ifndef PARTICLES
#define PARTICLES
extern "C" {
	#include "raylib.h"
}
#include "mewall.h"
#include "utilities.hpp"
#include <unordered_map>
#include <vector>

using mew::vec2;

#pragma pack(push, 1)

struct ParticleCluster;

struct Particle {
	Vector2 pos;
	Vector2 size;
	float fade_time;
	float live_time;
	float max_live_time;
	Color color;
	bool disabled = true;
	ParticleCluster* parent = nullptr;
};

struct ParticleCluster {
	std::vector<Particle*> used;
};

class ParticleSystem {
private:
	std::vector<Particle> particles;
	std::vector<ParticleCluster> clusters;
public:
	ParticleSystem() {
		particles.resize(1000000, {0});
	}

	void draw() {
		for (auto& c: clusters) {
			for (auto*& p: c.used) {
				if (p->disabled) continue;
				DrawRectangleV(p->pos, p->size, p->color);
			}
		}
	}

	void update() {
		for (uint i = 0; i < clusters.size(); ++i) {
			auto c = clusters[i];
			size_t lived = 0;
			for (auto*& p: c.used) {
				p->disabled = !(p->live_time > p->fade_time) || (p->live_time > p->max_live_time);
				if (!p->disabled) {
					lived++;
				}
				p->live_time += GetFrameTime();
			}
			if (lived == 0) {
				clusters.erase(clusters.begin()+i);
			}
		}
	}

	void spawn(size_t amount, Vector2 pos, Vector2 size, float max_live_time, Vector2 fade_time, Color color) {
		ParticleCluster cluster;
		for (auto& p: particles) {
			if (p.parent == nullptr) {
				p.color = color;
				p.pos = pos;
				p.size = size;
				p.disabled = false;
				p.fade_time = GetRandomValue(fade_time.x, fade_time.y);
				p.max_live_time = max_live_time;
				p.live_time = 0;
				cluster.used.push_back(&p);
			}
		}
		MewUserAssert(cluster.used.size() != amount, "cannot spawn particles");
	}

};

inline ParticleSystem* __current_ps = nullptr;

ParticleSystem* getParticleSystem() {
	if (__current_ps == nullptr) {
		__current_ps = new ParticleSystem();
	}
	return __current_ps;
}




#pragma pack(pop)



#endif