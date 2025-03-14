#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <curl/curl.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

json fetchJsonData(const string& url) {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if (!curl) {
        cerr << "cURL gagal diinisialisasi!" << endl;
        return nullptr;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "cURL Error: " << curl_easy_strerror(res) << endl;
        return nullptr;
    }

    try {
        return json::parse(readBuffer);
    } catch (json::parse_error& e) {
        cerr << "JSON Parsing Error: " << e.what() << endl;
        return nullptr;
    }
}

long getBitcoinBalance(const string& address) {
    string url = "https://api.blockcypher.com/v1/btc/main/addrs/" + address + "?includeSubaccounts=true&includeUnconfirmed=true";
    json jsonData = fetchJsonData(url);

    if (jsonData.is_null() || !jsonData.contains("final_balance")) {
        cerr << "Gagal mengambil saldo BTC untuk alamat: " << address << endl;
        return 0;
    }

    try {
        return jsonData["final_balance"].get<long>();
    } catch (...) {
        return stol(jsonData["final_balance"].get<string>());
    }
}

double getBTCtoIDR() {
    string url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=idr";
    json jsonData = fetchJsonData(url);

    if (jsonData.is_null() || !jsonData.contains("bitcoin") || !jsonData["bitcoin"].contains("idr")) {
        cerr << "Gagal mengambil harga BTC ke IDR" << endl;
        return 0;
    }

    try {
        return jsonData["bitcoin"]["idr"].get<double>();
    } catch (...) {
        return stod(jsonData["bitcoin"]["idr"].get<string>());
    }
}

double getBTCtoUSD() {
    string url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";
    json jsonData = fetchJsonData(url);

    if (jsonData.is_null() || !jsonData.contains("bitcoin") || !jsonData["bitcoin"].contains("usd")) {
        cerr << "Gagal mengambil harga BTC ke USD" << endl;
        return 0;
    }

    try {
        return jsonData["bitcoin"]["usd"].get<double>();
    } catch (...) {
        return stod(jsonData["bitcoin"]["usd"].get<string>());
    }
}

int getFearAndGreedIndex() {
    string url = "https://api.alternative.me/fng/";
    json jsonData = fetchJsonData(url);

    if (jsonData.is_null() || jsonData["data"].empty()) {
        cerr << "Gagal mengambil Fear & Greed Index" << endl;
        return 0;
    }

    try {
        return jsonData["data"][0]["value"].get<int>();
    } catch (...) {
        return stoi(jsonData["data"][0]["value"].get<string>());
    }
}

map<string, int> getMempoolFeeRate() {
    string url = "https://mempool.space/api/v1/fees/recommended";
    json jsonData = fetchJsonData(url);

    if (jsonData.is_null()) {
        cerr << "Gagal mengambil data fee transaksi." << endl;
        return {{"slow", 0}, {"medium", 0}, {"fast", 0}};
    }

    try {
        return {
            {"slow", jsonData["hourFee"].get<int>()},
            {"medium", jsonData["halfHourFee"].get<int>()},
            {"fast", jsonData["fastestFee"].get<int>()}
        };
    } catch (...) {
        return {
            {"slow", stoi(jsonData["hourFee"].get<string>())},
            {"medium", stoi(jsonData["halfHourFee"].get<string>())},
            {"fast", stoi(jsonData["fastestFee"].get<string>())}
        };
    }
}

string formatNumber(long long number) {
    string numStr = to_string(number);
    int insertPosition = numStr.length() - 3;
    while (insertPosition > 0) {
        numStr.insert(insertPosition, ",");
        insertPosition -= 3;
    }
    return numStr;
}

string formatRupiah(long amountSatoshi, double btcToIDR) {
    double amountIDR = (amountSatoshi / 100000000.0) * btcToIDR;
    return "Rp. " + formatNumber((long long)amountIDR);
}

string getFearStatus(int index) {
    if (index < 25) return "Extreme Fear";
    if (index < 50) return "Fear";
    if (index < 75) return "Greed";
    return "Extreme Greed";
}

int main() {
    ifstream file(getenv("HOME") + string("/wallet.txt"));
    if (!file) {
        cerr << "File 'wallet.txt' tidak ditemukan." << endl;
        return 1;
    }

    vector<string> addresses;
    string line;
    while (getline(file, line)) {
        addresses.push_back(line);
    }
    file.close();

    long totalBalanceSatoshi = 0;
    for (const auto& address : addresses) {
        totalBalanceSatoshi += getBitcoinBalance(address);
    }

    double btcToIDR = getBTCtoIDR();
    double btcToUSD = getBTCtoUSD();
    int fearAndGreedIndex = getFearAndGreedIndex();
    auto feeRates = getMempoolFeeRate();

    if (btcToIDR == 0 || btcToUSD == 0) {
        cerr << "Tidak dapat memperoleh nilai tukar atau harga BTC." << endl;
        return 1;
    }

    cout << "\n----------------------------------" << endl;
    cout << "~Balance~" << endl;
    cout << formatNumber(totalBalanceSatoshi) << " sat" << endl;
    cout << formatRupiah(totalBalanceSatoshi, btcToIDR) << endl;

    if (fearAndGreedIndex != 0) {
        cout << "----------------------------------" << endl;
        cout << "~Fear & Greed Index~" << endl;
        cout << "Index: " << fearAndGreedIndex << endl;
        cout << "Status: " << getFearStatus(fearAndGreedIndex) << endl;
    }

    if (!feeRates.empty()) {
        cout << "----------------------------------" << endl;
        cout << "~Mempool fee rate~" << endl;
        cout << "Slow: " << feeRates["slow"] << " sat/vB" << endl;
        cout << "Medium: " << feeRates["medium"] << " sat/vB" << endl;
        cout << "Fast: " << feeRates["fast"] << " sat/vB" << endl;
    }

    cout << "----------------------------------" << endl;
    cout << "~Price~" << endl;
    cout << "BTC: $ " << formatNumber((long long)btcToUSD) << endl;
    cout << "----------------------------------" << endl;

    return 0;
}
