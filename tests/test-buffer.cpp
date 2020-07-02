#include <bot/buffer.hpp>
#include <yaml-cpp/yaml.h>

#include <gtest/gtest.h>

#include <iostream>
#include <random>
#include <map>

class ParsingTest : public testing::Test {
	public:
		ParsingTest() {
			bot_config = YAML::LoadFile(
				"../tests/yaml-test/bot-test.yaml"
			);
		}
	protected:
		YAML::Node bot_config;
};

TEST(StringTemplate, Replace) {
	std::string test("This is a {{var}}");
	cppbot::Argument arg{"var", "test"};
	ASSERT_EQ("This is a test", resolve_argument(test, arg));
}

TEST_F(ParsingTest, TriggerParsing) {
	auto test = cppbot::parse_trigger_command(
		bot_config["on_trigger"]
	);
	ASSERT_EQ(1, test.count("[A-Z]"));
	ASSERT_EQ(1, test.count(".*pp") );
	ASSERT_EQ(1, test.count("\\s+") );
	ASSERT_EQ("text" , test["[A-Z]"][0].type   );
	ASSERT_EQ("toto" , test["[A-Z]"][0].message);
	ASSERT_EQ("gif"  , test[".*pp"][0].type    );
	ASSERT_EQ("homer", test[".*pp"][0].message );
	ASSERT_EQ("image", test["\\s+"][0].type    );
	ASSERT_EQ("bird" , test["\\s+"][0].message );
}

TEST_F(ParsingTest, SimpleParsing) {
	auto test = cppbot::parse_simple_command(
		bot_config["on_test"]
	);
	ASSERT_EQ("text", test[0].type);
	ASSERT_EQ("coucou", test[0].message);

	ASSERT_EQ("gif", test[1].type);
	ASSERT_EQ("au revoir", test[1].message);

	ASSERT_EQ("image", test[2].type);
	ASSERT_EQ("toto.png", test[2].message);
}

TEST_F(ParsingTest, ExtractVariables) {
	auto test = cppbot::parse_variables(
		bot_config["variables"]
	);
	ASSERT_EQ(test["name"], "toto");
	ASSERT_EQ(test["date"], "22 juin");
	ASSERT_EQ(test["two words"], "espaces dans les variables");
}

TEST_F(ParsingTest, ResoleVariables) {
	auto vars = cppbot::parse_variables(
		bot_config["variables"]
	);
	auto raw_vars = cppbot::parse_simple_command(
		bot_config["on_resolve"]
	);
	cppbot::resolve_arguments(raw_vars, vars);

	ASSERT_EQ("je m'appelle toto", raw_vars[0].message);
	ASSERT_EQ("je suis né le 22 juin", raw_vars[1].message);
	ASSERT_EQ("on peut mettre des espaces dans les variables", raw_vars[2].message);
	ASSERT_EQ("ce test est plus compliqué que les autres.", raw_vars[3].message);
}

TEST(ExtractVariables, Regex) {
	std::string test("ceci {{co nti ent}} des {{variables}}");
	cppbot::Argument contient{"co nti ent", "contient"};

	std::string var_name = cppbot::get_variable(test);
	ASSERT_EQ("co nti ent", var_name);
	test = cppbot::resolve_argument(test, contient);
	ASSERT_EQ("ceci contient des {{variables}}", test);

	var_name = cppbot::get_variable(test);
	ASSERT_EQ("variables", var_name);
	test = cppbot::resolve_argument(test, {"variables", "mots"});
	ASSERT_EQ("ceci contient des mots", test);
}
