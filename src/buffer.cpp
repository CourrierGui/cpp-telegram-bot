#include <bot/buffer.hpp>

#include <regex>
#include <iostream>

namespace cppbot {

Argument::Argument(
	const std::string& name,
	const std::string& value)
	: name("{{"+name+"}}"),
		value(value)
{
}

std::string resolve_argument(const std::string& input, const Argument& argument) {
	std::string res(input);
	std::string::size_type pos = res.find(argument.name);
	if (pos != std::string::npos) {
		res.replace(pos, strlen(argument.name.data()), argument.value);
	}
	return res;
}

auto parse_trigger_command(const YAML::Node& node)
	-> std::map<std::string, std::vector<Message>>
{
	std::map<std::string, std::vector<Message>> res;
	for (const auto& map: node) {
		for (const auto& kv: map) {
			res[kv.first.Scalar()] = parse_simple_command(kv.second);
		}
	}
	return res;
}

auto parse_simple_command(const YAML::Node& node)
	-> std::vector<Message>
{
	std::vector<Message> res;

	for (const auto& map: node) {
		for (const auto& kv: map) {
			res.push_back({kv.first.Scalar(), kv.second.Scalar()});
		}
	}
	return res;
}

auto get_variable(const std::string& str)
	-> std::string
{
	std::regex regex("\\{\\{([\\S\\s]+?)\\}\\}");
	std::smatch sm;
	if (std::regex_search(str, sm, regex) && sm.size() == 2) {
		return sm[1].str();
	} else {
		return "";
	}
}

void resolve_arguments(
	std::vector<Message>& raw_vars,
	const std::map<std::string,std::string>& variables)
{
	for (auto& message: raw_vars)
	{
		auto var_name = get_variable(message.message);
		while (!var_name.empty())
		{
			message.message = resolve_argument(
				message.message,
				{var_name, variables.at(var_name)}
			);
			var_name = get_variable(message.message);
		}
	}
}

auto parse_variables(const YAML::Node& node)
	-> std::map<std::string,std::string>
{
	std::map<std::string,std::string> res;
	for (const auto& map: node) {
		for (const auto& kv: map) {
			res[kv.first.Scalar()] = kv.second.Scalar();
		}
	}
	return res;
}

RandInt::RandInt() {
	std::random_device rd;
	seed = std::mt19937(rd());
	gen = std::uniform_real_distribution<double>(0.0, 1.0);
}

int RandInt::operator()(int min, int max) const {
	return min + gen(seed) * (max-min);
}

MessageBuffer::MessageBuffer(const std::vector<Message>& messages)
	: messages(messages), rand_int()
{
	current = this->messages.begin();
}

auto MessageBuffer::operator()(void) const -> Message {
	return *(messages.begin() + rand_int(0, messages.size()));
}

}
