#include <bot/buffer.hpp>
#include <iostream>

RandInt::RandInt() {
	std::random_device rd;
	seed = std::mt19937(rd());
	gen = std::uniform_real_distribution<double>(0.0, 1.0);
}

int RandInt::operator()(int min, int max) const {
	return min + gen(seed) * (max-min);
}

MessageBuffer::MessageBuffer(const YAML::Node& root) : messages(), rand_int() {
	auto it = std::back_inserter(messages);

	if (root.IsSequence()) {
		for (const auto& map: root) {
			if (map.IsMap()) {
				it = map;
			}
		}
		end = messages.size() - 1;
	}
}

auto MessageBuffer::operator()(void) const -> YAML::Node {
	if (end == 0) {
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(messages.begin(), messages.end(), g);
		end = messages.size()-1;
	}
	auto beg        = messages.begin();
	auto res_it     = beg + rand_int(0, end);
	auto res = *res_it;
	std::iter_swap(res_it, beg+end);
	--end;
	return res;
}
