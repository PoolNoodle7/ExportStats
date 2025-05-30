#include "pch.h"
#include <fstream>
#include <string>
#include <filesystem>
#include <ctime>
#include <unordered_map>

#include "ExportStats.h"
#include "../ExportStats/nlohmann/json.hpp"
using json = nlohmann::json;

//Demo tracking
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

std::unordered_map<std::string, std::string> mapNameLookup = {
    // Standard Maps
    {"stadium_p", "DFH Stadium"},
    {"stadium_day_p", "DFH Stadium (Day)"},
    {"stadium_snowy_p", "DFH Stadium (Snowy)"},
    {"stadium_stormy_p", "DFH Stadium (Stormy)"},
    {"stadium_circuit_p", "DFH Stadium (Circuit)"},

    {"cheetah_p", "Utopia Coliseum"},
    {"cheetah_day_p", "Utopia Coliseum (Day)"},
    {"cheetah_dusk_p", "Utopia Coliseum (Dusk)"},
    {"cheetah_snowy_p", "Utopia Coliseum (Snowy)"},
    {"cheetah_gilded_p", "Utopia Coliseum (Gilded)"},

    {"urban_p", "Urban Central"},
    {"urban_night_p", "Urban Central (Night)"},
    {"urban_dawn_p", "Urban Central (Dawn)"},
    {"urban_rainy_p", "Urban Central (Rainy)"},
    {"urban_haunted_p", "Urban Central (Haunted)"},

    {"park_p", "Beckwith Park"},
    {"park_night_p", "Beckwith Park (Night)"},
    {"park_stormy_p", "Beckwith Park (Stormy)"},
    {"park_midnight_p", "Beckwith Park (Midnight)"},
    {"park_snowy_p", "Beckwith Park (Snowy)"},

    {"trainstation_p", "Mannfield"},
    {"trainstation_day_p", "Mannfield (Day)"},
    {"trainstation_night_p", "Mannfield (Night)"},
    {"trainstation_dusk_p", "Mannfield (Dusk)"},
    {"trainstation_snowy_p", "Mannfield (Snowy)"},
    {"trainstation_stormy_p", "Mannfield (Stormy)"},

    {"wasteland_p", "Wasteland"},
    {"wasteland_night_p", "Wasteland (Night)"},
    {"wasteland_day_p", "Wasteland (Day)"},
    {"wasteland_pitched_p", "Wasteland (Pitched)"},

    {"arc_p", "Neo Tokyo"},
    {"arc_rainy_p", "Neo Tokyo (Rainy)"},
    {"arc_comic_p", "Neo Tokyo (Comic)"},
    {"arc_hacked_p", "Neo Tokyo (Hacked)"},

    {"aquadome_p", "AquaDome"},
    {"aquadome_day_p", "AquaDome (Day)"},

    {"championsfield_p", "Champions Field"},
    {"championsfield_day_p", "Champions Field (Day)"},
    {"championsfield_night_p", "Champions Field (Night)"},
    {"championsfield_nfl_p", "Champions Field (NFL)"},
    {"championsfield_nikefc_p", "Champions Field (Nike FC)"},

    {"farm_p", "Farmstead"},
    {"farm_night_p", "Farmstead (Night)"},
    {"farm_dusk_p", "Farmstead (Dusk)"},
    {"farm_pitched_p", "Farmstead (Pitched)"},
    {"farm_spooky_p", "Farmstead (Spooky)"},
    {"farm_upside_down_p", "Farmstead (The Upside Down)"},

    {"temple_p", "Forbidden Temple"},
    {"temple_day_p", "Forbidden Temple (Day)"},
    {"temple_fireice_p", "Forbidden Temple (Fire & Ice)"},

    {"saltyshores_p", "Salty Shores"},
    {"saltyshores_night_p", "Salty Shores (Night)"},
    {"saltyshores_fest_p", "Salty Shores (Salty Fest)"},

    {"neonfields_p", "Neon Fields"},
    {"neonfields_night_p", "Neon Fields (Night)"},

    {"deadeye_p", "Deadeye Canyon"},
    { "deadeye_oasis_p", "Deadeye Canyon (Oasis)" },

    { "sovereign_p", "Sovereign Heights" },
    { "sovereign_dusk_p", "Sovereign Heights (Dusk)" },

    { "estadio_p", "Estadio Vida" },
    { "estadio_dusk_p", "Estadio Vida (Dusk)" },

    { "drift_p", "Drift Woods" },
    { "drift_dawn_p", "Drift Woods (Dawn)" },
    { "drift_night_p", "Drift Woods (Night)" },

    { "futura_p", "Futura Garden" },
    { "futura_day_p", "Futura Garden (Day)" },

    { "rivals_p", "Rivals Arena" },
    { "rivals_day_p", "Rivals Arena (Day)" },

    { "starbase_p", "Starbase ARC" },
    { "starbase_aftermath_p", "Starbase ARC (Aftermath)" },

        // Alternate Maps
    { "throwbackstadium_p", "Throwback Stadium" },
    { "throwbackstadium_snowy_p", "Throwback Stadium (Snowy)" },

    { "underpass_p", "Underpass" },
    { "underpass_old_p", "Underpass (Old)" },
    { "tokyo_underpass_p", "Tokyo Underpass (Rainy)" },

    { "cosmic_p", "Cosmic" },
    { "cosmic_old_p", "Cosmic (Old)" },

    { "dunkhouse_p", "Dunk House" },
    { "dunkhouse_night_p", "Dunk House (Night)" },

    { "galleon_p", "Galleon" },
    { "galleon_retro_p", "Galleon Retro" },

    { "pillars_p", "Pillars" },
    { "octagon_p", "Octagon" },
    { "octagon_old_p", "Octagon (Old)" },
    { "doublegoal_p", "Double Goal" },
    { "doublegoal_old_p", "Double Goal (Old)" },
    { "utopia_retro_p", "Utopia Retro" },
    { "arctagon_p", "Arctagon" },
    { "badlands_p", "Badlands" },
    { "badlands_night_p", "Badlands (Night)" },
    { "barricade_p", "Barricade" },
    { "basin_p", "Basin" },
    { "calavera_p", "Calavera" },
    { "carbon_p", "Carbon" },
    { "core707_p", "Core 707" },
    { "corridor_p", "Corridor" },
    { "colossus_p", "Colossus" },
    { "hourglass_p", "Hourglass" },
    { "loophole_p", "Loophole" },
    { "quadron_p", "Quadron" },
    { "theblock_p", "The Block" },
    { "theblock_dusk_p", "The Block (Dusk)" },

        //Manually Added Maps
    { "Underwater_P", "Aquadome" },
    { "Underwater_GRS_P", "Aquadome (Salty Shallows" },
    { "Park_P", "Beckwith Park" },
    { "Park_Night_P", "Beckwith Park (Night)" },
    { "Park_Rainy_P", "Beckwith Park (Rainy)" },
    { "cs_p", "Champions Field" },
    { "cs_day_p", "Champions Field (Day)" },
    { "outlaw_p", "Deadeye Canyon" },
    { "Outlaw_Oasis_P", "Deadeye Canyon (Oasis)" },
    { "Stadium_P", "DFH Stadium" },
    { "Stadium_Race_Day_p", "DFH Stadium (Circuit)" },
        //No warning for DFH Stadium Day
    { "Stadium_Foggy_P", "DFH Stadium (Stormy)" },
    { "woods_p", "Drift Woods" },
    { "Woods_Night_P", "Drift Woods (Night)" },
    { "FF_Dusk_P", "Estadio Vida" },
        //No warning for Farmstead
    { "Farmstead_Night_P", "Farmstead (Night)" },
    { "Farm_HW_P", "Farmstead (Spooky)" },
    { "Farm_GRS_P", "Farmstead (Pitched)" },
    { "CHN_Stadium_P", "Fobidden Temple" },
    { "CHN_Stadium_Day_P", "Forbidden Temple (Day)" },
    { "FNI_Stadium_P", "Forbidden Temple (Fire and Ice)" },
    { "UF_Day_P", "Futura Garden" },
    { "EuroStadium_P", "Mannfield" },
    { "EuroStadium_Dusk_P", "Mannfield (Dusk)" },
    { "EuroStadium_Night_P", "Mannfield (Night)" },
    { "EuroStadium_Rainy_P", "Mannfield (Stormy)" },
    { "NeoTokyo_Standard_P", "Neo Tokyo" },
    { "NeoTokyo_Toon_p", "Neo Tokyo (Comic)" },
    { "NeoTokyo_Hax_P", "Neo Tokyo (Hacked)" },
    { "NeoTokyo_Arcade_P", "Neo Tokyo (Arcade)" },
    { "music_p", "Neon Fields" },
    { "beach_P", "Salty Shores" },
    { "beach_night_p", "Salty Shores (Night)" },
    { "Beach_Night_GSR_P", "Salty Shores (Salty Fest)" },
    { "street_p", "Sovereign Heights" },
    { "ARC_Darc_P", "Starbase Arc (Aftermath)" },
    { "TrainStation_P", "Urban Central" },
    { "TrainStation_Dawn_P", "Urban Central (Dawn)" },
    { "TrainStation_Night_P", "Urban Central (Night)" },
    { "UtopiaStadium_P", "Utopia Coliseum" },
    {"UtopiaStadium_Dusk_P", "Utopia Coliseum (Dusk)"},
	{"UtopiaStadium_Lux_P", "Utopia Coliseum (Gilded)" },
    {"wasteland_s_p", "Wasteland" },
    {"wasteland_Night_S_P", "Wasteland (Night)"},
	{"Wasteland_GSR_P", "Wasteland (Pitched)" },

    //Additional Maps
    { "ARC_P", "Arctagon"},
    { "Wasteland_P", "Badlands" },
    {"WasteLand_Night_P", "Badlands (Night)"},
    { "NeoTokyo_P", "Tokyo Underpass" }


};

BAKKESMOD_PLUGIN(ExportStats, "Export Stats from all matches", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

std::unordered_map<std::string, int> boostUsed;
std::unordered_map<std::string, std::string> playerIdToName;

void ExportStats::onLoad()
{
    _globalCvarManager = cvarManager;


    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.PostBeginPlay", [this](std::string) {
        ClearJson();
        boostUsed.clear();
        previousBoost.clear();

        playerIdToName.clear();
        demosTaken.clear();
  
        demosInflicted.clear();

		//Force clear the JSON file at the start of each match
        std::ofstream f(GetJsonPath());
        f << "{}";
        f.close();

        matchIsActive = true;

        LOG("Match started, matchStats.json cleared.");

        matchStartTime = std::chrono::steady_clock::now();

        ScheduleStatsLogging();

    gameWrapper->HookEventWithCallerPost<ServerWrapper>(
        "Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
        [this](ServerWrapper caller, void* params, std::string eventName) {
            onStatTickerMessage(params);
        }
    );

    });


    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", [this](std::string) {
        matchIsActive = false;

        auto endTime = std::chrono::steady_clock::now();
        totalMatchDurationSeconds = static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(endTime - matchStartTime).count()
            );

        LOG("Match ended. Duration: {} seconds", totalMatchDurationSeconds);

        LogStats();  // <-- Ensure final duration is written to file
        });

}

void ExportStats::onUnload() {
    boostUsed.clear();
    previousBoost.clear();

    playerIdToName.clear();
    demosTaken.clear();
    demosInflicted.clear();
    matchIsActive = false;
}


void ExportStats::ScheduleStatsLogging()
{
    gameWrapper->SetTimeout([this](GameWrapper*) {
        if (matchIsActive)
        {
            LogStats();
        }
        ScheduleStatsLogging(); // repeat regardless, but skip logging if match isn't active
        }, 1.0f);
}


void ExportStats::ClearJson()
{
    std::ofstream file(GetJsonPath());
    if (file.is_open())
    {
        file << "{}";
        file.close();
    }
}


void ExportStats::LogStats()
{

    ServerWrapper server = gameWrapper->GetCurrentGameState();
    if (server.IsNull()) return;

    std::string internalMapName = gameWrapper->GetCurrentMap();
    std::string mapName = mapNameLookup.contains(internalMapName)
        ? mapNameLookup[internalMapName]
        : internalMapName;

    if (!mapNameLookup.contains(internalMapName)) {
        LOG("Warning: Map name {} not found in lookup table", internalMapName);
    }


    ArrayWrapper<CarWrapper> cars = server.GetCars();
    LOG("Number of cars found: {}", cars.Count());

    if (cars.Count() == 0) {
        LOG("No cars found, skipping stat log.");
        return;
    }

    json j;
    j["map"] = mapName;
    j["matchDuration"] = totalMatchDurationSeconds;
    j["players"] = json::array();


    for (int i = 0; i < cars.Count(); ++i)
    {
        CarWrapper car = cars.Get(i);
        if (car.IsNull()) {
            LOG("Car {} is null", i);
            continue;
        }

        PriWrapper pri = car.GetPRI();
        if (pri.IsNull())
        {
            LOG("Car {} has null PRI", i);
            continue;
        }

        LOG("Valid player found: {}", pri.GetPlayerName().ToString());

	

        UniqueIDWrapper uid = pri.GetUniqueIdWrapper();

        
        std::string playerId = uid.GetIdString();
        if (playerId.find("Unknown") != std::string::npos) {
            LOG("Skipping player with unknown ID");
            continue;
        }
        std::string name = pri.GetPlayerName().ToString();


        playerIdToName[playerId] = name;

        int teamIndex = pri.GetTeamNum();
        std::string teamName = (teamIndex == 0) ? "BLUE" : "ORANGE";

        int goals = pri.GetMatchGoals();
        int assists = pri.GetMatchAssists();
        int saves = pri.GetMatchSaves();
        int shots = pri.GetMatchShots();
        int points = pri.GetMatchScore();

        BoostWrapper boost = car.GetBoostComponent();
        if (!boost.IsNull()) {
            int currentBoost = static_cast<int>(boost.GetCurrentBoostAmount() * 100);
            int previous = previousBoost[playerId];
            int delta = previous - currentBoost;

            if (delta > 0 && delta <= 100) { // reasonable usage range
                boostUsed[playerId] += delta;
            }

            previousBoost[playerId] = currentBoost;
        }

        json p;

 
        p["playerName"] = name;
        p["playerId"] = playerId;
        p["team"] = teamName;
        p["goals"] = goals;
        p["assists"] = assists;
        p["saves"] = saves;
        p["shots"] = shots;
        p["points"] = points;
        p["boostUsed"] = boostUsed[playerId];
        p["demosInflicted"] = demosInflicted[playerId];
        p["demosTaken"] = demosTaken[playerId];


        LOG("Logging: {} | G: {}, A: {}, S: {}, Sh: {}, P: {}, D: {}, BU: {}, DemosINF: {}, DemosTAK: {}",
            name, goals, assists, saves, shots, points,
            pri.GetMatchDemolishes(), boostUsed[playerId],
            demosInflicted[playerId], demosTaken[playerId]);


        j["players"].push_back(p);
    }

    std::ofstream file(GetJsonPath());
    if (file.is_open())
    {
        file << j.dump(4);
        file.close();
    }
}

void ExportStats::onStatTickerMessage(void* params) {
    struct StatTickerParams {
        uintptr_t Receiver;
        uintptr_t Victim;
        uintptr_t StatEvent;
    };

    auto* statParams = reinterpret_cast<StatTickerParams*>(params);
    if (!statParams) return;

    PriWrapper receiver(statParams->Receiver);
    PriWrapper victim(statParams->Victim);
    StatEventWrapper statEvent(statParams->StatEvent);

    if (receiver.IsNull() || victim.IsNull() || statEvent.IsNull()) return;

    std::string statName = statEvent.GetEventName();
    if (statName == "Demolish") {
        std::string receiverId = receiver.GetUniqueIdWrapper().GetIdString();
        std::string victimId = victim.GetUniqueIdWrapper().GetIdString();

        // Increment demo counts
        demosInflicted[receiverId]++;
        demosTaken[victimId]++;

        LOG("Demo recorded: {} demoed {}", receiver.GetPlayerName().ToString(), victim.GetPlayerName().ToString());
    }
}


void ExportStats::FinalizeStats()
{
    LOG("Finalizing match stats...");

    LogStats();
    LOG("Final match stats saved to: {}", GetJsonPath().string());
}

std::filesystem::path ExportStats::GetJsonPath()
{
    const char* home = std::getenv("USERPROFILE");
    if (!home)
    {
        LOG("Error: USERPROFILE environment variable not found.");
        return "matchStats.json";
    }
    return std::filesystem::path(home) / "Desktop" / "matchStats.json";
}