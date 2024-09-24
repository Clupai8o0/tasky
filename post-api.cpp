#include <iostream>
#include <string>
#include <curl/curl.h>

// Callback function to write the response data to a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

int main() {
  CURL* curl;
  CURLcode res;

  // The URL to which the POST request will be sent
  std::string url = "https://tasky-server-six.vercel.app/echo";

  // JSON data to send in the POST request
  std::string jsonData = "{\"title\":\"foo\",\"body\":\"bar\",\"userId\":1}";

  // Response data buffer
  std::string readBuffer;

  // Initialize curl session
  curl = curl_easy_init();

  if (curl) {
    // Set the URL for the POST request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set the callback function to handle response data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Specify that this is a POST request
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // Set the POST fields (JSON body)
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

    // Set headers (Content-Type: application/json)
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request, res will get the return code
    res = curl_easy_perform(curl);

    // Check if the request was successful
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
    else {
      // Output the response data (JSON)
      std::cout << "Response from server: " << std::endl;
      std::cout << readBuffer << std::endl;
    }

    // Clean up
    curl_slist_free_all(headers); // Free headers
    curl_easy_cleanup(curl);      // Clean up the curl session
  }

  return 0;
}
