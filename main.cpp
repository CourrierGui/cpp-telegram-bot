#include <stdio.h>
#include <tgbot/tgbot.h>
#include <random>
#include <iostream>
#include <algorithm>
#include <regex>
#include <yaml-cpp/yaml.h>

struct RandInt {
  std::mt19937 seed;
  std::uniform_real_distribution<double> gen;

  RandInt() {
    std::random_device rd;
    seed = std::mt19937(rd());
    gen = std::uniform_real_distribution<double>(0.0, 1.0);
  }

  int operator()(int min, int max) {
    return min + gen(seed) * (max-min);
  }
};

class MessageBuffer {
  private:
    std::vector<std::string> messages;
    RandInt rand_int;
    int end;

  public:
    MessageBuffer(const YAML::Node& node) : messages(), rand_int() {
      auto it = std::back_inserter(messages);
      for (auto val: node) {
        it = val.as<std::string>();
      }
      end = messages.size() - 1;
    }

    std::string operator()(void) {
      if (end == 0) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(messages.begin(), messages.end(), g);
        end = messages.size()-1;
      }
      auto beg        = messages.begin();
      auto res_it     = beg + rand_int(0, end);
      std::string res = *res_it;
      std::iter_swap(res_it, beg+end);
      --end;
      return res;
    }
};

int main(int argc, char** argv) {

  YAML::Node node;
  std::string token;
  if (argc > 2) {
    token = argv[1];
    node = YAML::LoadFile(argv[2]);
  } else {
    std::cout << "Invalid number of arguments: "
      << argv[0]
      << " <token>"
      << " <yaml_file>"
      << std::endl;
    return 0;
  }

  YAML::Node on_trigger = node["on_trigger"];
  YAML::Node on_ping = node["on_ping"];
  MessageBuffer buffer(on_ping);
  TgBot::Bot bot(token);

  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
    std::cout << message->chat->id << std::endl;
    bot.getApi().sendMessage(message->chat->id, "Hi!");
  });

  bot.getEvents().onCommand("ping", [&bot, &buffer](TgBot::Message::Ptr message) {
    if (message->text == "/ping@BestLanguageBot")
      bot.getApi().sendMessage(message->chat->id, buffer(), false, message->messageId);
  });

  bot.getEvents().onNonCommandMessage([&bot, &on_trigger](TgBot::Message::Ptr message) {

    printf("User wrote %s\n", message->text.c_str());
    for (auto it=on_trigger.begin(); it!=on_trigger.end(); ++it) {
      std::regex reg((*it).first.as<std::string>());

      auto words_begin = std::sregex_iterator(message->text.begin(), message->text.end(), reg);
      if (std::distance(words_begin, std::sregex_iterator())) {
        if ((*it).second["text"]) {
          bot.getApi().sendMessage(message->chat->id, (*it).second["text"].as<std::string>(), false, message->messageId);
        }
        /* if ((*it).second["gif"]) { */
        /*   bot.getApi().sendAnimation(message->chat->id, (*it).second["gif"].as<std::string>()); */

        /* } */
        break;
      }
    }
  });

  try {
    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (TgBot::TgException& e) {
    printf("error: %s\n", e.what());
  }

  return 0;
}
