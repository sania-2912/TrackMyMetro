#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <limits.h>
#include <queue>
#include <cmath>
#include <iomanip>  // Include this header for setprecision
using namespace std;

// Structure to represent a station
struct Station {
    string stName;
    string code;
    string colour;
    string layout;
    double latitude;
    double longitude;
};

struct Edge {
    string destination;
    double distance;  // Changed from float to double
    string line;  
};

map<string, vector<Edge>> graph;
map<string, Station> stationInfo;
map<pair<string, string>, float> fareChart;

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0;
    double lat1_rad = lat1 * M_PI / 180.0;
    double lon1_rad = lon1 * M_PI / 180.0;
    double lat2_rad = lat2 * M_PI / 180.0;
    double lon2_rad = lon2 * M_PI / 180.0;

    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;

    double a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1_rad) * cos(lat2_rad) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

void initializeFareChart() {
    fareChart[{"PCMC Bhavan", "Sant Tukaram Nagar"}] = 10.0;
    fareChart[{"Sant Tukaram Nagar", "Nashik Phata"}] = 10.0;
    fareChart[{"Nashik Phata", "Kasarwadi"}] = 15.0;
    fareChart[{"Kasarwadi", "Phugewadi"}] = 15.0;
    fareChart[{"Phugewadi", "Dapodi"}] = 20.0;
    fareChart[{"Dapodi", "Bopodi"}] = 20.0;
    fareChart[{"Bopodi", "Khadki"}] = 20.0;
    fareChart[{"Khadki", "Range Hills"}] = 20.0;
    fareChart[{"Range Hills", "Shivaji Nagar"}] = 25.0;
    fareChart[{"Shivaji Nagar", "Civil Court"}] = 25.0;
    fareChart[{"Civil Court", "Budhwar Peth"}] = 30.0;
    fareChart[{"Budhwar Peth", "Mandai"}] = 30.0;
    fareChart[{"Mandai", "Swargate"}] = 35.0;
    fareChart[{"Vanaz", "Anand Nagar"}] = 10.0;
    fareChart[{"Anand Nagar", "Garware College"}] = 15.0;
    fareChart[{"Garware College", "Deccan Gymkhana"}] = 20.0;
    fareChart[{"Deccan Gymkhana", "Civil Court"}] = 20.0;
    fareChart[{"Civil Court", "Mangalwar Peth"}] = 25.0;
    fareChart[{"Mangalwar Peth", "Pune Railway Station"}] = 25.0;
    fareChart[{"Pune Railway Station", "Ruby Hall"}] = 25.0;
    fareChart[{"Ruby Hall", "Bund Garden"}] = 30.0;
    fareChart[{"Bund Garden", "Yerwada"}] = 30.0;
    fareChart[{"Yerwada", "Kalyani Nagar"}] = 35.0;
    fareChart[{"Kalyani Nagar", "Ramwadi"}] = 35.0;

    // Bidirectional Connections
    for (auto& entry : fareChart) {
        fareChart[{entry.first.second, entry.first.first}] = entry.second;
    }
}

void dijkstra(string source, string destination) {
    priority_queue<pair<float, pair<string, pair<string, float>>>, 
                   vector<pair<float, pair<string, pair<string, float>>>>, 
                   greater<pair<float, pair<string, pair<string, float>>>>> pq;
    map<string, float> dist;
    map<string, float> fare; 
    map<string, string> from;
    map<string, string> lineAtStation;

    for (const auto& entry : graph) {
        dist[entry.first] = INT_MAX;
        fare[entry.first] = 0;
        from[entry.first] = "";
    }

    dist[source] = 0;
    pq.push({0, {source, {"", 0}}});  

    while (!pq.empty()) {
        string current = pq.top().second.first;
        string currentLine = pq.top().second.second.first;
        float currentDist = pq.top().first;
        float currentFare = pq.top().second.second.second;
        pq.pop();

        if (current == destination) {
            cout << "Shortest distance: " << fixed << setprecision(2) << dist[destination] << " km" << endl;
            cout << "Total fare: Rs" << fixed << setprecision(2) << currentFare << endl;
            cout << "Path: ";
            vector<string> path;
            for (string s = destination; !s.empty(); s = from[s]) {
                path.push_back(s);
            }
            for (int i = path.size() - 1; i >= 0; --i) {
                cout << path[i];
                if (i > 0) cout << " -> ";
            }
            cout << endl;
            return;
        }

        for (const auto& edge : graph[current]) {
            string neighbor = edge.destination;
            float weight = edge.distance;
            string edgeLine = edge.line;

            if (currentLine != "" && currentLine != edgeLine) {
                if (stationInfo[current].layout != "Elevated") {
                    continue;
                }
            }

            float newFare = currentFare + fareChart[{current, neighbor}];

            if (currentDist + weight < dist[neighbor]) {
                dist[neighbor] = currentDist + weight;
                fare[neighbor] = newFare;
                from[neighbor] = current;
                lineAtStation[neighbor] = edgeLine;
                pq.push({dist[neighbor], {neighbor, {edgeLine, newFare}}});
            }
        }
    }

    cout << "No path found from " << source << " to " << destination << "." << endl;
}

void addStation(string name, string code, string colour, string layout, double lat, double lon) {
    stationInfo[name] = {name, code, colour, layout, lat, lon};
}

void addConnection(string station1, string station2, string line) {
    double distance = calculateDistance(
        stationInfo[station1].latitude,
        stationInfo[station1].longitude,
        stationInfo[station2].latitude,
        stationInfo[station2].longitude
    );

    graph[station1].push_back({station2, distance, line});
    graph[station2].push_back({station1, distance, line});
}


int main() {
    vector<Station> Stations = {
         {"PCMC Bhavan", "PIM", "Purple", "Elevated", 18.6298, 73.7997},
        {"Sant Tukaram Nagar", "STN", "Purple", "Elevated", 18.6209, 73.8070},
        {"Nashik Phata", "NSP", "Purple", "Elevated", 18.6018, 73.8140},
        {"Kasarwadi", "KSD", "Purple", "Elevated", 18.5865, 73.8207},
        {"Phugewadi", "PHW", "Purple", "Elevated", 18.5760, 73.8261},
        {"Dapodi", "DPO", "Purple", "Elevated", 18.5555, 73.8498},
        {"Bopodi", "BPI", "Purple", "Elevated", 18.5434, 73.8611},
        {"Khadki", "KKI", "Purple", "Elevated", 18.5319, 73.8733},
        {"Range Hills", "RHI", "Purple", "Elevated", 18.5204, 73.8821},
        {"Shivaji Nagar", "SVJ", "Purple", "Underground", 18.5119, 73.8977},
        {"Civil Court", "CCT", "Purple", "Underground", 18.5225, 73.8550},  
        {"Kasba Peth", "KSP", "Purple", "Underground", 18.5077, 73.8698},
        {"Mandai", "MDI", "Purple", "Underground", 18.4979, 73.8684},
        {"Swargate", "SGT", "Purple", "Underground", 18.5010, 73.8665},
        {"Vanaz", "VNZ", "Aqua", "Elevated", 18.5120, 73.8079},
        {"Anand Nagar", "AND", "Aqua", "Elevated", 18.5141, 73.8029},
        {"Ideal Colony", "IDC", "Aqua", "Elevated", 18.5241, 73.8004},
        {"Nal Stop", "NSP", "Aqua", "Elevated", 18.5305, 73.8037},
        {"Garware College", "GWC", "Aqua", "Elevated", 18.5372, 73.8123},
        {"Deccan Gymkhana", "DGY", "Aqua", "Elevated", 18.5377, 73.8184},
        {"Chhatrapati Sambhaji Udyan", "CSU", "Aqua", "Elevated", 18.5365, 73.8206},
        {"PMC Bhavan", "PMB", "Aqua", "Elevated", 18.5259, 73.8362},
        {"Civil Court", "CCT_A", "Aqua", "Elevated", 18.5225, 73.8550},  
        {"Mangalwar Peth", "MWP", "Aqua", "Elevated", 18.5195, 73.8567},
        {"Pune Railway Station", "PRS", "Aqua", "Elevated", 18.5141, 73.8577},
        {"Ruby Hall Clinic", "RHC", "Aqua", "Elevated", 18.5076, 73.8609},
        {"Bund Garden", "BDG", "Aqua", "Elevated", 18.5077, 73.8640},
        {"Yerwada", "YWD", "Aqua", "Elevated", 18.5353, 73.8883},
        {"Kalyani Nagar", "KYN", "Aqua", "Elevated", 18.5514, 73.9015},
        {"Ramwadi", "RWD", "Aqua", "Elevated", 18.5679, 73.9112}
    };

    
    for (const auto& station : Stations) {
        addStation(station.stName, station.code, station.colour, station.layout, station.latitude, station.longitude);
    }

    
    addConnection("PCMC Bhavan", "Sant Tukaram Nagar", "Purple");
    addConnection("Sant Tukaram Nagar", "Nashik Phata", "Purple");
    addConnection("Nashik Phata", "Kasarwadi", "Purple");
    addConnection("Kasarwadi", "Phugewadi", "Purple");
    addConnection("Phugewadi", "Dapodi", "Purple");
    addConnection("Dapodi", "Bopodi", "Purple");
    addConnection("Bopodi", "Khadki", "Purple");
    addConnection("Khadki", "Range Hills", "Purple");
    addConnection("Range Hills", "Shivaji Nagar", "Purple");
    addConnection("Shivaji Nagar", "Civil Court", "Purple");
    addConnection("Civil Court", "Kasba Peth", "Purple");
    addConnection("Kasba Peth", "Mandai", "Purple");
    addConnection("Mandai", "Swargate", "Purple");
    addConnection("Vanaz", "Anand Nagar", "Aqua");
    addConnection("Anand Nagar", "Ideal Colony", "Aqua");
    addConnection("Ideal Colony", "Nal Stop", "Aqua");
    addConnection("Nal Stop", "Garware College", "Aqua");
    addConnection("Garware College", "Deccan Gymkhana", "Aqua");
    addConnection("Deccan Gymkhana", "Chhatrapati Sambhaji Udyan", "Aqua");
    addConnection("Chhatrapati Sambhaji Udyan", "PMC Bhavan", "Aqua");
    addConnection("PMC Bhavan", "Civil Court", "Aqua");
    addConnection("Civil Court", "Mangalwar Peth", "Aqua");
    addConnection("Mangalwar Peth", "Pune Railway Station", "Aqua");
    addConnection("Pune Railway Station", "Ruby Hall Clinic", "Aqua");
    addConnection("Ruby Hall Clinic", "Bund Garden", "Aqua");
    addConnection("Bund Garden", "Yerwada", "Aqua");
    addConnection("Yerwada", "Kalyani Nagar", "Aqua");
    addConnection("Kalyani Nagar", "Ramwadi", "Aqua");

    string source, destination;
    cout << "Enter source station name: ";
    getline(cin, source);
    cout << "Enter destination station name: ";
    getline(cin, destination);

    if (stationInfo.find(source) == stationInfo.end() || stationInfo.find(destination) == stationInfo.end()) {
        cout << "Invalid station name entered!" << endl;
    } else {
        dijkstra(source, destination);
    }

    return 0;
}
