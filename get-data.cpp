#include <iostream>
#include <string>
#include <curl/curl.h>
#include <splashkit.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

struct User {
  std::string username;
  std::string password;
};

int main() {
  CURL* curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://172.25.0.1:3000/get-data");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    
    json data = json_from_string(readBuffer);
    vector<User> users;
  
    if (json_count_keys(data) != 0) {
      // Read the users from the data
      vector<json> users_json;
      json_read_array(data, "users", users_json);

      // Loop through the users and add them to the users vector
      for (int i = 0; i < users_json.size(); i++) {
        json user_json = users_json[i];
        string username = json_read_string(user_json, "username");
        string password = json_read_string(user_json, "password");
        users.push_back(User{ username, password });
      }
    }
    free_all_json();
    write_line(users[0].username);

    std::cout << "Response data: " << readBuffer << std::endl;
  }

}