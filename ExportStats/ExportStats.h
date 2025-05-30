#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/includes.h"
#include <unordered_map>
#include <string>

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class ExportStats: public BakkesMod::Plugin::BakkesModPlugin
	//,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	//void onLoad() override;
	//void onUnload() override; // Uncomment and implement if you need a unload method

public:
	void onLoad() override;
	void onUnload() override;

private:



	// Match and stat handling

	void onStatTickerMessage(void* params);

	void ClearJson();
	void ScheduleStatsLogging();
	void LogStats();
	void FinalizeStats();

	// Helper
	std::filesystem::path GetJsonPath();

	std::unordered_map<std::string, int> boostUsed;
	std::unordered_map<std::string, int> previousBoost;


	
	std::unordered_map<std::string, std::string> playerIdToName;

	std::unordered_map<std::string, int> demosInflicted;
	std::unordered_map<std::string, int> demosTaken;

	std::chrono::time_point<std::chrono::steady_clock> matchStartTime;
	int totalMatchDurationSeconds = 0;

	bool matchIsActive = false;

	//void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window

};
