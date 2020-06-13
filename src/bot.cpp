#include <bot/bot.hpp>
#include <bot/buffer.hpp>

#include <map>
#include <regex>

void Bot::send_message(
	const YAML::Node& n,
	TgBot::Message::Ptr& message)
{
	if (n["text"]) {
		bot.getApi().sendMessage(
			message->chat->id,
			n["text"].Scalar(),
			false,
			message->messageId
		);
	}

	if (n["image"]) {
		bot.getApi().sendPhoto(
			message->chat->id,
			n["image"].Scalar()
		);
	}

	if (n["gif"]) {
		std::string gif;
		if (variables["gif"][n["gif"].Scalar()]) {
			gif = variables["gif"][n["gif"].Scalar()].Scalar();
		} else {
			gif = n["gif"].Scalar();
		}
		bot.getApi().sendAnimation(message->chat->id, gif);
	}
}

Bot::Bot(const std::string& token, const std::string& var_file)
	: bot(token),
		start_time(get_start_time()),
		variables(YAML::LoadFile(var_file))
{
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
		std::cerr
			<< "error: "
			<< e.what()
			<< '\n';
	}
}

void Bot::init_unsplash(const std::string& id) {

}

void Bot::process_config(const std::string& file) {
	YAML::Node config = YAML::LoadFile(file);
	for (const auto& map: config) {
		if (map.first.Scalar() != "on_trigger") {
			MessageBuffer buffer(map.second);

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
			YAML::Node on_trigger = config["on_trigger"];
			RandInt rand_int;
			bot.getEvents().onNonCommandMessage(
				[this, on_trigger, rand_int](TgBot::Message::Ptr message) {
					if (message->date < start_time) return;

					for (auto regex: on_trigger) {
						std::regex reg((regex).first.Scalar());

						auto words_begin = std::sregex_iterator(
							message->text.begin(),
							message->text.end(),
							reg
						);

						if (std::distance(words_begin, std::sregex_iterator())) {
							int id = rand_int(0, regex.second.size());
							YAML::Node n = regex.second[id];
							send_message(n, message);
							break;
						}
					}
				});
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
