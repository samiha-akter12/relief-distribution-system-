#include <iostream>
#include <fstream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>
#include <cstdlib>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_map>
using namespace std;

ofstream outfile("relief_inputs.txt");

void setColor(int colorCode) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
}
void clearScreen() {
    system("cls");
}
void slowPrint(const string &text, int delay = 30) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
    cout << endl;
}

struct Zone {
    int id;
    string address, area, district, country;
    int population, urgency;
    double distance;
    int foodKits, medKits, extraKits;
    double costPerKmPerKit;
    long long totalCost;
};

string disasterType, extraKitName;

int safeInt(const string &prompt, int minVal, int maxVal) {
    int x;
    while (true) {
        cout << prompt;
        outfile << prompt;
        cin >> x;
        outfile << x << endl;
        if (cin.fail()) {
            cin.clear(); cin.ignore(1000, '\n');
            cout << "[X] Please enter a valid number between " << minVal << " and " << maxVal << ".\n";
        } else if (x < minVal || x > maxVal) {
            cout << "[!] Sorry, our service currently supports values between " << minVal << " and " << maxVal << ".\n";
            cout << "    We'll try our best to increase our service limitations.\n";
        } else return x;
    }
}

double safeDouble(const string &prompt, double minVal, double maxVal) {
    double x;
    while (true) {
        cout << prompt;
        outfile << prompt;
        cin >> x;
        outfile << x << endl;
        if (cin.fail()) {
            cin.clear(); cin.ignore(1000, '\n');
            cout << "[X] Please enter a valid number.\n";
        } else if (x < minVal || x > maxVal) {
            cout << "[!] Sorry, our service currently supports values up to " << maxVal << ".\n";
            cout << "    We'll try our best to increase our service limitations.\n";
        } else return x;
    }
}

string safeText(const string &prompt) {
    string s;
    while (true) {
        cout << prompt;
        outfile << prompt;
        cin >> s;
        outfile << s << endl;
        bool hasDigit = false;
        for (char c : s) if (isdigit(c)) hasDigit = true;
        if (hasDigit) cout << "[X] Only letters are allowed.\n";
        else return s;
    }
}

/*bool zoneCompare(const Zone &a, const Zone &b) {
    if (a.urgency != b.urgency) return a.urgency > b.urgency;
    if (a.population != b.population) return a.population > b.population;
    return a.distance < b.distance;
}*/

bool zoneCompare(const Zone &a, const Zone &b) {
    if (a.urgency != b.urgency)
        return a.urgency > b.urgency; // Higher urgency first
    if (a.population != b.population)
        return a.population > b.population; // Higher population first
    return a.distance < b.distance; // Shorter distance first
}


void merge(vector<Zone> &a, int l, int m, int r) {
    vector<Zone> left(a.begin() + l, a.begin() + m + 1);
    vector<Zone> right(a.begin() + m + 1, a.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < left.size() && j < right.size()) {
        if (zoneCompare(left[i], right[j])) a[k++] = left[i++];
        else a[k++] = right[j++];
    }
    while (i < left.size()) a[k++] = left[i++];
    while (j < right.size()) a[k++] = right[j++];
}

void mergeSort(vector<Zone> &a, int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        mergeSort(a, l, m);
        mergeSort(a, m + 1, r);
        merge(a, l, m, r);
    }
}

  void inputZones(vector<Zone> &zones, int count) {
    vector<string> districtList = {
        "Dhaka", "Chittagong", "Khulna", "Rajshahi", "Barisal",
        "Sylhet", "Rangpur", "Mymensingh", "Cumilla", "Noakhali",
        "Jessore", "Bogura", "Narsingdi", "Narayanganj", "Pabna",
        "Gazipur", "Tangail", "Satkhira", "Jamalpur", "Kushtia"
    };

    vector<bool> used(20, false);

    // ----------- Step 1: Collect Inputs -----------
    for (int i = 0; i < count; i++) {
        clearScreen();
        setColor(11);
        cout << "=============== SELECTING ZONE " << i + 1 << " ===============\n";
        setColor(7);

        cout << "Select a district:\n";
        for (int d = 0; d < districtList.size(); ++d) {
            if (!used[d])
                cout << "  " << d + 1 << ". " << districtList[d] << "\n";
        }
cout << "\n";
  setColor(14);
        int dChoice;
        while (true) {
            dChoice = safeInt("Enter district number (1-20): ", 1, 20);
            if (!used[dChoice - 1]) {
                used[dChoice - 1] = true;
                break;
            } else {
                cout << "[X] That district has already been selected. Please choose another.\n";
            }
        }

        Zone z;
        z.id = i + 1;
        z.district = districtList[dChoice - 1];
        z.address = z.district + " Zone";
        z.area = "Main Area";
        z.country = "Bangladesh";

        z.population = safeInt("Population (1-50000): ", 1, 50000);
        z.urgency = safeInt("Urgency (1=Low, 2=Medium, 3=High): ", 1, 3);
        z.distance = safeDouble("Distance (km, max 5000): ", 0, 5000);

        z.foodKits = safeInt("Food Kits: ", 0, INT_MAX);
        z.medKits = safeInt("Medicine Kits: ", 0, INT_MAX);
        z.extraKits = safeInt(extraKitName + ": ", 0, INT_MAX);


         // Step 1: Define cost constants
const int perKmCost = 70;
const int foodKitCost = 300;
const int medKitCost = 500;
const int extraKitCost = 400;

// Step 2: Calculate total cost as per your formula
long long distanceCost = z.distance * perKmCost;
long long foodCost = z.foodKits * foodKitCost;
long long medCost = z.medKits * medKitCost;
long long extraCost = z.extraKits * extraKitCost;

// Step 3: Total cost = sum of all
z.totalCost = distanceCost + foodCost + medCost + extraCost;


        zones.push_back(z);
    }


    // ----------- Step 2: Show & Save Receipts -----------

    clearScreen();
        setColor(14);
        cout << "==================== DISTRIBUTION RECEIPT ====================\n";
    for (const Zone &z : zones) {

        //cout << "Zone ID: " << z.id << "\n";
         cout << "-------------------- Zone " << z.id << " --------------------\n";
        cout << "Disaster: " << disasterType << "\n";
        cout << "District: " << z.district << "\n";
        cout << "Country: " << z.country << "\n";
        cout << "Population: " << z.population << "\n";
        cout << "Urgency: " << z.urgency << "\n";
        cout << "Distance: " << z.distance << " km\n";
        cout << "Kits: Food=" << z.foodKits << ", Medicine=" << z.medKits << ", " << extraKitName << "=" << z.extraKits << "\n";
       // cout << "Cost/km/kit: " << z.costPerKmPerKit << "\n";
        cout << "Total Cost: " << z.totalCost << "\n";
        cout << "==============================================================\n\n";
        //setColor(7);
       // system("pause");

        // Save to file
        ofstream receiptFile("distribution_receipts.txt", ios::app);
        receiptFile << "==================== DISTRIBUTION RECEIPT ====================\n";
        receiptFile << "Zone ID: " << z.id << "\n";
        receiptFile << "Disaster: " << disasterType << "\n";
        receiptFile << "District: " << z.district << "\n";
        receiptFile << "Country: " << z.country << "\n";
        receiptFile << "Population: " << z.population << "\n";
        receiptFile << "Urgency: " << z.urgency << "\n";
        receiptFile << "Distance: " << z.distance << " km\n";
        receiptFile << "Kits: Food=" << z.foodKits << ", Medicine=" << z.medKits << ", " << extraKitName << "=" << z.extraKits << "\n";
        receiptFile << "perKmCost = 70 \nperfoodKitCost = 300 \npermedKitCost = 500 \nperextraKitCost = 400"<< "\n";
        receiptFile << "Total Cost: " << z.totalCost << "\n";
        receiptFile << "==============================================================\n\n";
        receiptFile.close();
    }setColor(10);
cout << "\nAll receipts displayed above.\n";
setColor(7);
system("pause"); //

}




/*void chooseBestZone(const vector<Zone> &zones) {
    const Zone &z = zones[0];
    cout << "\n[Recommended] First Relief Should Go To:\n";
    cout << "Zone " << z.id << ": " << z.address << " (" << z.district << ", " << z.country << ")\n";
    cout << "Reason: Highest urgency, largest population, shortest distance.\n";
}*/

string getOrdinalSuffix(int n) {
    if (n % 100 >= 11 && n % 100 <= 13) return "th";
    switch (n % 10) {
        case 1: return "st";
        case 2: return "nd";
        case 3: return "rd";
        default: return "th";
    }
}

 void chooseBestZone(const vector<Zone> &zones) {
    cout << "\n[Relief Priority Order Based on Urgency, Population, and Distance]\n\n";

    for (int i = 0; i < zones.size(); ++i) {
        const Zone &z = zones[i];
        cout << i + 1 << getOrdinalSuffix(i + 1) << " Priority: Zone " << z.id << " (" << z.address << ", " << z.district << ")\n";

        string reason;
        if (i == 0) {
            reason = "Highest urgency";
        } else {
            const Zone &best = zones[0];

            if (z.urgency < best.urgency) {
                reason = "Lower urgency";
            } else if (z.urgency == best.urgency && z.population < best.population) {
                reason = "Same urgency, but lower population";
            } else if (z.urgency == best.urgency && z.population == best.population && z.distance > best.distance) {
                reason = "Same urgency, same population but longer distance";
            } else {
                reason = "Lower priority based on urgency/population/distance";
            }
        }

        cout << "Reason: " << reason << "\n\n";
    }
}


/*string getOrdinalSuffix(int n) {
    if (n % 100 >= 11 && n % 100 <= 13) return "th";
    switch (n % 10) {
        case 1: return "st";
        case 2: return "nd";
        case 3: return "rd";
        default: return "th";
    }
}
*/

void calculateTotalCost(const vector<Zone> &zones) {
    long long total = 0;
    for (const Zone &z : zones) total += z.totalCost;
    cout << "\n[Total] Estimated Cost for All Relief Deliveries: " << total << "\n";
}

// Greedy: Huffman Node
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(NULL), right(NULL) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void buildHuffmanTree(string data) {
    unordered_map<char, int> freq;
    for (char c : data) freq[c]++;
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto p : freq) pq.push(new HuffmanNode(p.first, p.second));
    while (pq.size() > 1) {
        HuffmanNode *l = pq.top(); pq.pop();
        HuffmanNode *r = pq.top(); pq.pop();
        HuffmanNode *merged = new HuffmanNode('\0', l->freq + r->freq);
        merged->left = l;
        merged->right = r;
        pq.push(merged);
    }
    cout << "\n[Greedy] Huffman Coding Tree Built Successfully!\n";
}

// DP: 0/1 Knapsack for kit optimization
void knapsackOptimize(vector<Zone> &zones) {
    int capacity = 1000;
    vector<int> weight, value;
    for (const auto &z : zones) {
        weight.push_back(z.foodKits + z.medKits + z.extraKits);
        value.push_back(z.totalCost / 100);
    }
    vector<vector<int>> dp(zones.size() + 1, vector<int>(capacity + 1, 0));
    for (int i = 1; i <= zones.size(); i++) {
        for (int w = 0; w <= capacity; w++) {
            if (weight[i-1] <= w)
                dp[i][w] = max(dp[i-1][w], dp[i-1][w - weight[i-1]] + value[i-1]);
            else
                dp[i][w] = dp[i-1][w];
        }
    }
    cout << "\n[Dynamic] Knapsack Max Value: " << dp[zones.size()][capacity] << "\n";
}

 int main() {
    while (true) {
        clearScreen();
        setColor(10);
        cout << "\t\t\t\t========================================\n";
        cout << "\t\t\t\t           Assalamu Alaikum \n\n";
        cout << "\t\t\t\t  Welcome To RELIEF DISTRIBUTION SYSTEM \n";
        cout << "\t\t\t\t                             by [NST]\n";
        cout << "\t\t\t\t========================================\n\n";
        setColor(7);
        slowPrint("Press any key to begin...", 30);
        _getch();
        clearScreen();

        setColor(11);
        cout << "Disasters Supported:\n";
        cout << "1. Flood\n";
        cout << "2. Winter\n";
        cout << "3. Drought\n";
        cout << "4. War\n";
        setColor(7);

        int choice = safeInt("Enter disaster type (1-4): ", 1, 4);

        switch (choice) {
            case 1: disasterType = "Flood"; extraKitName = "Water Kits"; break;
            case 2: disasterType = "Winter"; extraKitName = "Blankets"; break;
            case 3: disasterType = "Drought"; extraKitName = "Water Kits"; break;
            case 4: disasterType = "War"; extraKitName = "Trauma Kits"; break;
        }

        setColor(10);
        cout << "\n Selected Disaster: " << disasterType << "\n";
        setColor(14);
        cout << "\n[Info] Package details for " << disasterType << " relief:\n";
        cout << "- Food Kits\n";
        cout << "- Medicine Kits\n";
        cout << "- " << extraKitName << "\n";
        setColor(7);

        int zoneCount = safeInt("Enter number of zones (1-20): ", 1, 20);
        vector<Zone> zones;
        inputZones(zones, zoneCount);

        mergeSort(zones, 0, zones.size() - 1);
        clearScreen();
        setColor(14);

        chooseBestZone(zones);
        system("pause");

        clearScreen();
        setColor(12);
        calculateTotalCost(zones);

        string data = "";
        for (auto &z : zones) data += z.address[0];
        buildHuffmanTree(data);
        knapsackOptimize(zones);

        setColor(10);
        cout << "\n Analysis complete!\n";
        setColor(7);

        setColor(11);
        int nextAction = safeInt("\n1. Go back to Main Menu\n2. Exit\n", 1, 2);
        if (nextAction == 2) {
            setColor(10);
            cout << "\nThank you for using the Relief Distribution System!\n";
            setColor(7);
            break;  // Exit loop
        }
    }

    outfile.close();
    return 0;
}
