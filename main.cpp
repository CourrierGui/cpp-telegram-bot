#include <tgbot/tgbot.h>
#include <yaml-cpp/yaml.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include <nlohmann/json.hpp>

#include <stdio.h>
#include <random>
#include <iostream>
#include <algorithm>
#include <regex>
#include <chrono>

using json = nlohmann::json;

//TODO: ajouter un rate sur les reply
//TODO: ajouter un argument pour savoir si c'est un reply ou non
//TODO: ajouter les groupings
//TODO: bf interpretor ?

constexpr const int nb_images = 300;
constexpr const char* URL = "https://api.unsplash.com/search/photos?query=beer&client_id=Lb2xA1iBU2AuHIiF3u5lWO-bhgHDINKml3H79mkCzlI&per_page=300";

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
  YAML::Node gifs;
  std::string token;
  if (argc > 2) {
    token = argv[1];
    node = YAML::LoadFile(argv[2]);
    gifs = YAML::LoadFile(argv[3]);
  } else {
    std::cout << "Invalid number of arguments: "
      << argv[0]
      << " <token>"
      << " <yaml_file>"
      << " <gifs_file>"
      << std::endl;
    return 0;
  }
  int32_t start_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  RandInt rand_int;

  curlpp::Cleanup cleanup;
  curlpp::Easy request;
  request.setOpt<curlpp::Options::Url>(URL);

  std::ostringstream sstream;
  sstream << request;

  json j = json::parse(sstream.str());

  YAML::Node on_trigger = node["on_trigger"];
  YAML::Node on_ping = node["on_ping"];
  MessageBuffer ping_buffer(on_ping);
  TgBot::Bot bot(token);

  bot.getEvents().onCommand("start", [&bot, start_time](TgBot::Message::Ptr message) {
    if (message->date > start_time && message->text == "/start@BestLanguageBot")
      bot.getApi().sendMessage(message->chat->id, "Hi!");
  });

  bot.getEvents().onCommand("test", [&bot, &gifs, start_time](TgBot::Message::Ptr message) {
    if (message->date > start_time && message->from->id == 472577275 && message->chat->id == 472577275) {
      std::clog << message->chat->id << ' ' << message->from->id << '\n';
      for (auto p: gifs) {
        bot.getApi().sendMessage(message->chat->id, p.first.Scalar());
        bot.getApi().sendAnimation(message->chat->id, p.second.Scalar());
      }

    } else return;
  });

  bot.getEvents().onCommand("ping", [&bot, &ping_buffer, start_time](TgBot::Message::Ptr message) {
    if (message->text == "/ping@BestLanguageBot" && message->date > start_time)
      bot.getApi().sendMessage(message->chat->id, ping_buffer(), false, message->messageId);
  });

  bot.getEvents().onCommand("chat", [&bot, &rand_int, &j, start_time](TgBot::Message::Ptr message) {
    if (message->date > start_time) {
      int id = rand_int(0, 30);
      std::string image_url = j["results"][id]["urls"]["small"];

      bot.getApi().sendPhoto(
        message->chat->id,
        image_url,
        "Je crois que tu voulais une bière plutôt.",
        message->messageId);
    }
  });

  bot.getEvents().onNonCommandMessage(
    [&bot, &on_trigger, &rand_int, &gifs, start_time](TgBot::Message::Ptr message) {
      if (message->date < start_time) return;

      std::clog
        << "User wrote "
        << message->text
        << '\n';

      for (auto regex: on_trigger) {
        std::regex reg((regex).first.Scalar());

        auto words_begin = std::sregex_iterator(
          message->text.begin(),
          message->text.end(),
          reg);

        if (std::distance(words_begin, std::sregex_iterator())) {
          int id = rand_int(0, regex.second.size());
          YAML::Node n = regex.second[id];

          if (n["text"]) {
            std::clog << "test\n";
            bot.getApi().sendMessage(
              message->chat->id,
              n["text"].Scalar(),
              false,
              message->messageId);
          }

          if (n["image"]) {
            bot.getApi().sendPhoto(
              message->chat->id,
              n["image"].Scalar());
          }

          if (n["gif"]) {
            bot.getApi().sendAnimation(
              message->chat->id,
              gifs[n["gif"].Scalar()].Scalar());
          }
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
