#pragma once

#include <nlohmann/json.hpp>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

using json = nlohmann::json;

constexpr const int nb_images = 300;
constexpr const char* URL = "https://api.unsplash.com/search/photos?query=beer&client_id=&per_page=300";

/* curlpp::Cleanup cleanup; */
/* curlpp::Easy request; */
/* request.setOpt<curlpp::Options::Url>(URL); */

/* std::ostringstream sstream; */
/* sstream << request; */

/* json j = json::parse(sstream.str()); */

/*
- example_url:
	- url: "https://example.com/search?q={{arg_one}},f={{arg_two}}/"
- on_ping:
	- url:
		type: example_url
		args:
			arg_one: "arg_val"
			arg_two: "arg_val"
 */
