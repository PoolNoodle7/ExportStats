//pch.h for precompiled headers
#include "pch.h"

#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")



#include <fstream>
#include <string>
#include <filesystem>
#include <ctime>
#include <unordered_map>

#include "ExportStats.h"

//Experimental JSON library

//#include "../ExportStats/nlohmann/json.hpp"
//using json = nlohmann::json;

#include "nlohmann/json.hpp"

//Demo tracking
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

std::unordered_map<std::string, std::string> mapNameLookup = {

<<<<<<< HEAD
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
{"stadium_day_p", "DFH Stadium (Day)"},
=======
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
    {"stadium_day_p", "DFH Stadium (Day)"},
>>>>>>> d15c36071911330e880f1eea9af432b98a39f8d0

{ "Stadium_Foggy_P", "DFH Stadium (Stormy)" },
{ "woods_p", "Drift Woods" },
{ "Woods_Night_P", "Drift Woods (Night)" },
{ "FF_Dusk_P", "Estadio Vida" },
//No warning for Farmstead
{"farm_p", "Farmstead"},

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


        //Find JSON
        std::filesystem::path path = GetJsonPath();

        json j;

        // Step 1: Read existing JSON file if it exists
        std::ifstream inFile(path);
        if (inFile.is_open()) {
            try {
                inFile >> j;
            }
            catch (...) {
                LOG("Failed to parse JSON file. Starting with empty JSON.");
                j = json{};
            }
            inFile.close();
        }

        // Step 2: Append the match duration
        j["matchDuration"] = totalMatchDurationSeconds;

        // Step 3: Write back to the file
        std::ofstream outFile(path);
        if (outFile.is_open()) {
            outFile << j.dump(4); // pretty-print with 4 spaces
            outFile.close();
            LOG("Appended match duration to JSON file.");
        }
        else {
            LOG("Failed to open JSON file for writing.");
        }

        FinalizeStats();
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


json ExportStats::LogStats()
{

    ServerWrapper server = gameWrapper->GetCurrentGameState();
    if (server.IsNull()) return json{};

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
        return json{};
    }

    json j;
    j["map"] = mapName;
    j["matchDuration"] = 0;
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


        //ESTIMATED boost usage tracking
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

        //JSON strucutre for player stats
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

    return j;
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

json ExportStats::ReadJson() {
    std::filesystem::path path = GetJsonPath();
    json readJSON;

    std::ifstream inFile(path);
    if (inFile.is_open()) {
        try {
            inFile >> readJSON;
            LOG("Successfully read full JSON from file.");
        }
        catch (const std::exception& e) {
            LOG("Failed to parse JSON: {}", e.what());
        }
        inFile.close();
    }
    else {
        LOG("Failed to open JSON file for reading.");
    }

    return readJSON;
}


void ExportStats::FinalizeStats()
{
    LOG("Finalizing match stats...");

    json readJSON = ReadJson();

    if (readJSON.empty())
    {
        LOG("No final stats were recorded (LogStats returned empty JSON).");
    }
    else
    {
        LOG("Final stats JSON created successfully.");
        LOG("Saving to file: {}", GetJsonPath().string());

        std::ofstream file(GetJsonPath());
        if (file.is_open())
        {
            file << readJSON.dump(4);
            file.close();
        }

        LOG("Attempting to send final stats to API...");
        SendJsonToAPI_WinINet(readJSON.dump());
    }

    LOG("FinalizeStats() complete.");
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

//LINK FOR API: https://ibeachzsite-production.up.railway.app/api/newgamedata

bool ExportStats::SendJsonToAPI_WinINet(const std::string& jsonStr)
{
<<<<<<< HEAD
    int apiErrorCode = 0; //Stoer API error codes, if any


    HINTERNET hInternet = InternetOpenA("ExportStatsPlugin", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {

        //Error code saved
        apiErrorCode = GetLastError();
        return false;
    }
=======
    HINTERNET hInternet = InternetOpenA("ExportStatsPlugin", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;
>>>>>>> d15c36071911330e880f1eea9af432b98a39f8d0

    HINTERNET hConnect = InternetConnectA(hInternet, "ibeachzsite-production.up.railway.app", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    // Note: INTERNET_FLAG_SECURE enables HTTPS
    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", "/api/newgamedata", NULL, NULL, NULL,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string headers =
        "Content-Type: application/json\r\n"
        "x-api-secret: tE\\\\k2Ze?g%%fV:2£1N2G^J<7XL2\r\n";

    // Add headers to request
    if (!HttpAddRequestHeadersA(hRequest, headers.c_str(), -1L, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE)) {
        LOG("Failed to add headers.");
    }

    // Send the request
    BOOL success = HttpSendRequestA(hRequest, NULL, 0, (LPVOID)jsonStr.c_str(), jsonStr.size());

<<<<<<< HEAD
    //Check if request was successful
    if (!success) {
        LOG("HttpSendRequestA failed. Error code: {}", GetLastError());
    }
    else {
        // Reads status code, still checking if the request was successful
        DWORD statusCode = 0;
        DWORD statusSize = sizeof(statusCode);
        if (HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusSize, NULL)) {

            //ACTUALLY SAVE THE STATUS CODE
            apiErrorCode = static_cast<int>(statusCode);
            LOG("HTTP Status Code: {}", statusCode);
        }

        else {
            LOG("Failed to retrieve status code. Error: {}", GetLastError());
        }

        // Read response body
        char buffer[4096];
        DWORD bytesRead = 0;
        std::string responseBody;
        while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            responseBody.append(buffer);
        }

        if (!responseBody.empty()) {
            LOG("Server response: {}", responseBody);
        }
    }

    //Clean up
=======
    // Cleanup
>>>>>>> d15c36071911330e880f1eea9af432b98a39f8d0
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

<<<<<<< HEAD
    // Check for specific error code 502 and retry ONCE
    if (apiErrorCode == 502 && !hasRetriedSend) {

        //Prevent multiple retries
        hasRetriedSend = true;
        LOG("502 Bad Gateway. Retrying in 3 seconds...");

        //Call method again after 3 seconds
        gameWrapper->SetTimeout([this, jsonStr](GameWrapper*) {
            SendJsonToAPI_WinINet(jsonStr);
            }, 3.0f);
    }


=======
    if (!success) {
        LOG("HttpSendRequestA failed. Error code: {}", GetLastError());
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
>>>>>>> d15c36071911330e880f1eea9af432b98a39f8d0

    return success == TRUE;
}

