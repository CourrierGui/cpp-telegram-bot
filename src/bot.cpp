#include <bot/bot.hpp>

#include <map>
#include <regex>

namespace cppbot {

void Bot::send_message(const Message& n, TgBot::Message::Ptr& message) {
	if (n.type == "text")
		bot.getApi().sendMessage(message->chat->id, n.message, false, message->messageId);
	else if (n.type == "image")
		bot.getApi().sendPhoto(message->chat->id, n.message);
	else if (n.type == "gif")
		bot.getApi().sendAnimation(message->chat->id, n.message);
}

Bot::Bot(const std::string& token, const std::string& var_file)
	: bot(token), start_time(get_start_time()), variables(YAML::LoadFile(var_file)) {

}

void Bot::start() {
	try {
		std::clog
			<< "Bot username: "
			<< bot.getApi().getMe()->username.c_str()
			<< '\n';
		TgBot::TgLongPoll longPoll(bot);

		while (true) {
			std::clog << "Long poll started.\n";
			longPoll.start();
		}

	} catch (TgBot::TgException& e) {
		std::cerr << "error: " << e.what() << '\n';
	}
}

void Bot::init_unsplash(const std::string&) {

}

void Bot::process_config(const std::string& file) {
	YAML::Node config = YAML::LoadFile(file);
	for (const auto& map: config) {
		if (map.first.Scalar() != "on_trigger") {
			MessageBuffer buffer(parse_simple_command(map.second));

			std::string name = map.first.Scalar();
			name.replace(0, 3, "");
			auto cmd = [this,buffer,name](TgBot::Message::Ptr message) {
				if (message->date > start_time
						&& message->text == "/"+name+"@BestLanguageBot")
				{
					send_message(buffer(), message);
				}
			};
			add_command(name, cmd);

		} else {
			auto on_trigger = parse_trigger_command(config["on_trigger"]);
			RandInt rand_int;
			bot.getEvents().onNonCommandMessage(
				[this, on_trigger, rand_int](TgBot::Message::Ptr message) {
					if (message->date < start_time) return;

					for (auto kv: on_trigger) {
						std::clog << kv.first << '\n';
						std::regex reg(kv.first);

						auto words_begin = std::sregex_iterator(
							message->text.begin(),
							message->text.end(),
							reg
						);

						if (std::distance(words_begin, std::sregex_iterator())) {
							int id = rand_int(0, kv.second.size());
							Message n = kv.second[id];
							send_message(n, message);
							break;
						}
					}
				}
			);
		}
	}
}

int32_t Bot::get_start_time() {
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}

void Bot::add_command(
	const std::string& command,
	const TgBot::EventBroadcaster::MessageListener& handler)
{
	bot.getEvents().onCommand(command, handler);
}

/* bot.getEvents().onCommand("chat", [&bot, &rand_int, &j, start_time](TgBot::Message::Ptr message) { */
/*   if (message->date > start_time) { */
/*     int id = rand_int(0, 30); */
/*     std::string image_url = j["results"][id]["urls"]["small"]; */

/*     bot.getApi().sendPhoto( */
/*       message->chat->id, */
/*       image_url, */
/*       "Je crois que tu voulais une bière plutôt.", */
/*       message->messageId); */
/*   } */
/* }); */

}
