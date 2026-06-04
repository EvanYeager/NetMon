#include "netstats.h"
#include "speedtester.h"
#include <chrono>
#include <ctime>
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <string>
#include <atomic>

const int DOWN_BYTES_SENT = 10'000'000;
const int UP_BYTES_SENT = 1'000'000;

void SpeedTester::downloadTest(std::atomic<bool>& quitFlag) {
  std::string url = "https://speed.cloudflare.com/__down?bytes=" +
                    std::to_string(DOWN_BYTES_SENT);

  for (int i = 0; i < 5; i++) {
    CURL *curl = curl_easy_init();
    if (!curl)
      return;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(
        curl, CURLOPT_WRITEFUNCTION,
        +[](void *, size_t size, size_t nmemb, void *) {
          return size * nmemb; // discard data, just count bytes
        });
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
    auto start = std::chrono::steady_clock::now();
    CURLcode resp = curl_easy_perform(curl);
    if (resp != CURLE_OK) {
      curl_easy_cleanup(curl);
      continue; // if error, don't count it towards the stats
    }

    if (quitFlag) {
      curl_easy_cleanup(curl);
      return;
    }

    auto end = std::chrono::steady_clock::now();

    float elapsed = std::chrono::duration<double>(end - start).count();

    // curl_off_t sent;
    // CURLcode result = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &sent);
    // if (result != CURLE_OK) continue;
    // std::cout << sent << "\n";
    // Mbps speed = Mbps((double)sent / elapsed / 1'000'000.0);
    Mbps speed = Mbps((DOWN_BYTES_SENT * 8) / elapsed / 1'000'000);
    netstats::addDownloadTime(speed);

    curl_easy_cleanup(curl);
  }

}

void SpeedTester::uploadTest(std::atomic<bool>& quitFlag) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return;

  std::string url = "https://speed.cloudflare.com/__up";
  std::vector<char> uploadBuffer(UP_BYTES_SENT, 'x'); // arbitrary data to send

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, uploadBuffer.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)UP_BYTES_SENT);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(
      curl, CURLOPT_WRITEFUNCTION,
      +[](void *, size_t size, size_t nmemb, void *) { return size * nmemb; });

  for (int i = 0; i < 10; i++) {

    if (quitFlag) {
      curl_easy_cleanup(curl);
      return;
    }

    auto start = std::chrono::steady_clock::now();
    CURLcode resp = curl_easy_perform(curl);
    if (resp != CURLE_OK)
      continue;
    auto end = std::chrono::steady_clock::now();

    float elapsed = std::chrono::duration<double>(end - start).count();
    Mbps speed = Mbps((UP_BYTES_SENT * 8.0) / elapsed / 1'000'000.0);
    netstats::addUploadTime(speed);
  }

  curl_easy_cleanup(curl);
}
