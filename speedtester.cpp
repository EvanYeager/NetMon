#include "netstats.h"
#include "speedtester.h"
#include <chrono>
#include <ctime>
#include <curl/curl.h>
#include <curl/easy.h>
#include <string>

const int BYTES_SENT = 10'000'000;


void SpeedTester::downloadTest() {
  CURL *curl = curl_easy_init();
  if (!curl)
    return;
  std::string url =
      "https://speed.cloudflare.com/__down?bytes=" + std::to_string(BYTES_SENT);
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(
      curl, CURLOPT_WRITEFUNCTION,
      +[](void *, size_t size, size_t nmemb, void *) {
        return size * nmemb; // discard data, just count bytes
      });

  for (int i = 0; i < 8; i++) {
    auto start = std::chrono::steady_clock::now();
    CURLcode resp = curl_easy_perform(curl);
    if (resp != CURLE_OK)
      continue; // if error, don't count it towards the stats

    auto end = std::chrono::steady_clock::now();

    float elapsed = std::chrono::duration<double>(end - start).count();

    Mbps speed = Mbps((BYTES_SENT * 8.0) / elapsed / 1'000'000.0);
    netstats::addDownloadTime(speed);

    auto now = std::chrono::system_clock::now();
  }

  curl_easy_cleanup(curl);
}

void SpeedTester::uploadTest() {
  CURL *curl = curl_easy_init();
  if (!curl)
    return;

  std::string url = "https://speed.cloudflare.com/__up";
  std::vector<char> uploadBuffer(BYTES_SENT, 'x'); // arbitrary data to send

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, uploadBuffer.data());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)BYTES_SENT);
  curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(
      curl, CURLOPT_WRITEFUNCTION,
      +[](void *, size_t size, size_t nmemb, void *) { return size * nmemb; });

  for (int i = 0; i < 8; i++) {
    auto start = std::chrono::steady_clock::now();
    CURLcode resp = curl_easy_perform(curl);
    if (resp != CURLE_OK)
      continue;
    auto end = std::chrono::steady_clock::now();

    float elapsed = std::chrono::duration<double>(end - start).count();
    Mbps speed = Mbps((BYTES_SENT * 8.0) / elapsed / 1'000'000.0);
    netstats::addUploadTime(speed);
  }

  curl_easy_cleanup(curl);
}
