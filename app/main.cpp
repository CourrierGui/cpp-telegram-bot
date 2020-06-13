#include <bot/bot.hpp>

#include <fstream>

//TODO: ajouter un rate sur les reply
//TODO: ajouter un argument pour savoir si c'est un reply ou non
//TODO: ajouter les groupings
//TODO: bf interpretor ?

int main(int argc, char** argv) {
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
		std::cout << "Invalid number of arguments.\nUsage: "
			<< argv[0]
			<< " <token_file>"
			<< " <bot_file>"
			<< " <config_file>"
			<< " <url_file>"
			<< std::endl;
	}

	Bot bot(token, var_file);
	bot.process_config(config);

	if (argc == 5)
		bot.init_unsplash(client_id);

	bot.start();
  return 0;
}
