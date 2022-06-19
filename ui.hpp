#pragma once

#include<ftxui/screen/screen.hpp>
#include<ftxui/dom/elements.hpp>
#include<ftxui/component/component.hpp>
#include<ftxui/component/screen_interactive.hpp>

namespace ui {

	using namespace ftxui;

	std::string n_items;
	std::string n_biders;

	Component input_n_items = Input(&n_items, "items");
	Component input_n_biders = Input(&n_biders, "biders");

	auto component = Container::Vertical({
		input_n_items,
		input_n_biders
	});

	auto renderer = Renderer(component, [&] {
		return vbox({
				   text("Hello " + n_items + " " + n_biders),
				   separator(),
				   hbox(text(" items"), input_n_items->Render()),
				   hbox(text(" biders: "), input_n_biders->Render()),
			}) |
		border;
	});

	

	void draw() {
		auto screen = ScreenInteractive::TerminalOutput();
		screen.Loop(renderer);
	}

}