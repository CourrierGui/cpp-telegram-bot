#include <bot/bot.hpp>
#include <bot/arg-parser.hpp>

#include <fstream>

//TODO: ajouter un rate sur les reply
//TODO: ajouter un argument pour savoir si c'est un reply ou non
//TODO: ajouter les groupings

using namespace cppbot;

void show_usage(const std::string& bin_name) {
		std::cerr << "Invalid number of arguments.\n"
			<< "Usage: "
			<< bin_name
			<< " <token_file>"
			<< " <bot_file>"
			<< " <config_file>"
			<< " <url_file>"
			<< std::endl;
}

int main(const int argc, const char** argv) {
	std::string token, client_id, config, var_file;

	if (argc >= 4) {
		std::fstream file;
		file.open(argv[1]);
		std::getline(file, token);
		file.close();

		config = argv[2];
		var_file = argv[3];

		if (argc == 5) {
			file.open(argv[4]);
			std::getline(file, client_id);
			file.close();
		}
	} else {
		show_usage(argv[0]);
		return 0;
	}

	Bot bot(token, var_file);
	token.clear();
	bot.process_config(config);

	if (argc == 5) {
		bot.init_unsplash(client_id);
		client_id.clear();
	}

	bot.start();
  return 0;
}
