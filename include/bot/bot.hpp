#pragma once

#include <yaml-cpp/yaml.h>
#include <tgbot/tgbot.h>

#include <iostream>
#include <string>
#include <fstream>

class Bot {
	public:
		Bot(const std::string&, const std::string&);
		void start();
		void init_unsplash(const std::string&);
		void process_config(const std::string&);

	private:
		std::string token;
		TgBot::Bot bot;
		int32_t start_time;
		YAML::Node variables;

		static int32_t get_start_time();
		void add_command(
			const std::string&,
			const TgBot::EventBroadcaster::MessageListener&
		);
		void send_message(
			const YAML::Node&,
			TgBot::Message::Ptr&
		);
};
