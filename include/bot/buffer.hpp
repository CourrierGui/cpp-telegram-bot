#pragma once

#include <yaml-cpp/yaml.h>

#include <random>
#include <algorithm>
#include <map>
#include <vector>

namespace cppbot {

	struct Message {
		std::string type;
		std::string message;
	};

	struct Argument {
		std::string name;
		std::string value;

		Argument(const std::string&,const std::string&);
	};

	auto resolve_argument(const std::string&,const Argument&) -> std::string;
	auto parse_simple_command(const YAML::Node&) -> std::vector<Message>;
	auto parse_trigger_command(const YAML::Node&) -> std::map<std::string, std::vector<Message>>;
	auto parse_variables(const YAML::Node& node) -> std::map<std::string,std::string>;
	void resolve_arguments(
		std::vector<Message>&,
		const std::map<std::string,std::string>&
	);
	auto get_variable(const std::string&) -> std::string;

	class RandInt {
		private:
			mutable std::mt19937 seed;
			mutable std::uniform_real_distribution<double> gen;

		public:
			RandInt();
			int operator()(int min, int max) const;
	};

	class MessageBuffer {
		private:
			mutable std::vector<Message> messages;
			mutable std::vector<Message>::iterator current;
			RandInt rand_int;

		public:
			MessageBuffer(const std::vector<Message>& node);
			auto operator()(void) const -> Message;
	};

}
