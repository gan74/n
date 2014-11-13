#include "utils.h"
#include <n/defines.h>
#include <ctime>
#include <iostream>

namespace n {

	const void *null = 0;

	void fatal(const char *msg, const char *file, int line) {
		std::cerr<<msg;
		if(file) {
			std::cerr<<" in file "<<file;
			if(line) {
				std::cerr<<" at line "<<line;
			}
		}
		std::cerr<<std::endl;
		exit(1);
	}

namespace math {
	uint randHelper() {
		uint bits = core::log2ui(RAND_MAX);
		uint num = 0;
		for(uint i = 0; i < sizeof(uint) * 8; i += bits) {
			num = (num << bits) | rand();
		}
		return num;
	}

	uint random(uint max, uint min) {
		static bool seed = false;
		if(!seed) {
			#ifdef N_DEBUG
			srand(0);
			#else
			time_t now = time(0);
			srand(hash(&now, sizeof(now)));
			#endif
			seed = true;
		}
		if(max < min) {
			uint t = max;
			max = min;
			min = t;
		}
		return (randHelper() % (max - min)) + min;
	}

	float random() {
		return (float)(randHelper() & 0x7FFFFF) * 1.0f / (0x7FFFFF + 1.0f);
	}

	float toDeg(float a) {
		return a * 180 / pi();
	}

	float toRad(float a) {
		return a / 180 * pi();
	}
} //math

namespace core {

	uint log2ui(uint n) {
		uint l;
		asm("bsr %1, %0\n"
		  : "=r"(l)
		  : "r" (n));
		return l;
	}

	uint hash(const void *key, uint len, uint seed) {
		uint m = 0x5bd1e995;
		int r = 24;
		uint h = seed ^ len;
		byte *data = (byte *)key;
		while(!(len < sizeof(uint))) {
			uint k = *(uint *)data;
			k *= m;
			k ^= k >> r;
			k *= m;
			h *= m;
			h ^= k;
			data += sizeof(uint);
			len -= sizeof(uint);
		}
		while(!(len < 4)) {
			h ^= data[len - 1] << (8 * (len - 1));
		}
		switch(len) {
			case 3:
				h ^= data[2] << 16;
			case 2:
				h ^= data[1] << 8;
			case 1:
				h ^= data[0];
				h *= m;
		};
		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;
		return h;
	}
} //core
} //n