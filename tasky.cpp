#include <splashkit.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <experimental/filesystem>

using namespace std::experimental::filesystem;
using std::to_string;

/**
 * A struct representing a user with a username and password.
 */
struct User {
  string username; /**< The username of the user */
  string password; /**< The password of the user */
};

/**
 * A struct representing a task with a username, title, description, status, priority, due date, start date, and tags.
 */
enum TaskStatus {
  TODO = 1, /**< The task is to be done */
  IN_PROGRESS = 2, /**< The task is in progress */
  COMPLETED = 3, /**< The task is done */
  NO_STATUS = 4, /**< The task has no status */
};
/**
 * A struct representing a task with a username, title, description, status, priority, due date, start date, and tags.
 */
enum Priority {
  URGENT = 1, /**< The task is urgent */
  HIGH = 2, /**< The task is high priority */
  NORMAL = 3, /**< The task is normal priority */
  LOW = 4, /**< The task is low priority */
  NO_PRIORITY = 5, /**< The task has no priority */
};
/**
 * A function to convert a task status to a string.
 * @param status The task status to convert
 * @returns The string representation of the task status
 */
string to_string(TaskStatus status) {
  switch (status) {
    case TODO: return "TODO";
    case IN_PROGRESS: return "IN PROGRESS";
    case COMPLETED: return "COMPLETED";
    default: return "";
  }
}
/**
 * A function to convert a priority to a string.
 * @param priority The priority to convert
 * @returns The string representation of the priority
 */
string to_string(Priority priority) {
  switch (priority) {
    case URGENT: return "URGENT";
    case HIGH: return "HIGH";
    case NORMAL: return "NORMAL";
    case LOW: return "LOW";
    default: return "";
  }
}

/**
 * A struct representing a task with a username, title, description, status, priority, due date, start date, and tags.
 */
struct Task {
  string username; /**< The username of the task */
  string title; /**< The title of the task */
  string description; /**< The description of the task */

  TaskStatus status; /**< The status of the task */
  Priority priority; /**< The priority of the task */

  string due_date; /**< The due date of the task */
  string start_date; /**< The start date of the task */

  vector<string> tags; /**< The tags of the task */
};
/**
 * A function to convert a vector of tags to a string.
 * @param tags The vector of tags to convert
 * @returns The string representation of the tags
 */
string to_string(vector<string> tags) {
  string result = "";
  for (int i = 0; i < tags.size(); i++) { // Loop through the tags and concatenate them to the result string
    result += tags[i];
    if (i != tags.size() - 1) result += ", "; // Add a comma and space if it is not the last tag
  }
  return result;
}

/**
 * A namespace to provide helper functions for tasks.
 */
namespace Helper {
  /**
   * A class to provide helper functions for tasks.
   * This class provides functions to check if a date is valid and to sort tasks by date.
   * The date format is "YYYY-MM-DD".
   * The sorting algorithm used is bubble sort.
   * The sorting function is passed as a callback.
   * The sorting function should take a vector of tasks and a boolean reference as arguments.
   * The boolean reference is used to check if any swaps were made during the sorting process.
   * The sorting function should swap tasks if the first task is greater than the second task.
   * The sorting function should set the boolean reference to true if any swaps were made.
   * The sorting function should return void.
   */
  class Date {
    public:
    /**
     * A function to check if a date is valid.
     * @param date The date to check
     * @returns True if the date is valid, false otherwise
     */
    static bool is_date_valid(const string& date) {
      tm timeStruct = {};
      std::istringstream iss(date);
      
      // Parse the date string using the "YYYY-MM-DD" format
      iss >> std::get_time(&timeStruct, "%Y-%m-%d");

      // Check if parsing failed (invalid format)
      if (iss.fail())
        return false;

      // Normalize the date using mktime and check if it matches the original values
      timeStruct.tm_isdst = -1; // Daylight saving time not in use
      std::time_t time = std::mktime(&timeStruct);
      if (time == -1)
        return false; // Invalid date


      // Compare the normalized values to check if the date is valid
      std::tm* normalizedTime = std::localtime(&time);
      return normalizedTime->tm_year == timeStruct.tm_year &&
        normalizedTime->tm_mon == timeStruct.tm_mon &&
        normalizedTime->tm_mday == timeStruct.tm_mday;
    }
    /**
     * A function to sort tasks by date. Uses the bubble sort algorithm
     * @param arr The array of tasks to sort
     * @param sort The sorting function to use
     */
    static void sort_date(vector<Task> &arr, void (*sort_condition)(vector<Task>&, bool &swapped, int &j)) {
      bool swapped = false;
      for (int i = 0; i < arr.size() - 1; i++) {
        for (int j = 0; j < arr.size() - i - 1; j++) 
          sort_condition(arr, swapped, j); // Call the sorting function condition as a callback
        if (!swapped) break; // If no swaps were made, the array is sorted
      }
    }
  };

  /**
   * A class to provide helper functions for reading input from the user.
   * This class provides functions to read strings, integers, dates, and tags from the user.
   * The functions display a prompt to the user and return the input entered by the user.
   * The functions validate the input entered by the user and prompt the user to enter valid input if necessary.
   * The functions use the utilities provided in the splashkit library.
   * The functions are static and can be called without creating an instance of the class.
   * The functions are designed to be used in place of the functions provided in the utilities.h file. 
   */
  class Reader {
    public:
    /**
     * Read a string from the user, displaying the prompt provided.
     * @param prompt The prompt to display to the user.
     * @return The string entered by the user.
     */
    static string read_string(string prompt) {
      write(prompt);
      return read_line();
    }

    /**
     * Read an integer from the user, displaying the prompt provided.
     * @param prompt The prompt to display to the user.
     * @return The integer value entered by the user.
     */
    static int read_integer(string prompt) {
      string output = read_string(prompt);
      if (!is_integer(output)) { // Check if the input is an integer
        write_line("Please enter a valid integer.");
        return read_integer(prompt);
      }
      return convert_to_integer(output);
    }
    
    /**
     * Read an integer from the user, displaying the prompt provided, and ensuring
     * that the value is within the specified range.
     * @param prompt The prompt to display to the user.
     * @param min The minimum value that the user can enter.
     * @param max The maximum value that the user can enter.
     * @return The integer value entered by the user.
     */
    static int read_integer(string prompt, int min, int max) {
      int value = read_integer(prompt);
      if (value < min || value > max) { // Check if the value is within the specified range
        write_line("Please enter a valid integer within the specified range.");
        return read_integer(prompt, min, max);
      }
      return value;
    }

    /**
     * Read a date from the user, displaying the prompt provided.
     * @param prompt The prompt to display to the user.
     * @return The date entered by the user.
     */
    static string read_date(string prompt) {
      string date = read_string(prompt);
      if (!Date::is_date_valid(date)) { // Check if the date is valid
        write_line("Please enter a valid date in the format YYYY-MM-DD.");
        return read_date(prompt);
      }
      return date;
    }

    /**
     * Read a list of tags from the user, displaying the prompt provided.
     * @param prompt The prompt to display to the user.
     * @return The list of tags entered by the user.
     */
    static vector<string> read_tags(string prompt) {
      string tags = read_string(prompt);
      vector<string> tag_list = split(tags, ','); // Split the tags by comma
      return tag_list;
    }
  };
}

class Menu {
  private:
  /**
   * A function to display a line of dashes to separate sections of the menu.
   */
  static void print_line() {
    write_line("---------------------------------");
  } 
  /**
   * A function to display a line of dashes to separate sections of the menu.
   * @param heading The heading to display in the menu.
   */
  static void print_heading(string heading) {
    write_line();
    print_line();
    write_line("Tasky - " + heading);
    print_line();
  }

  public:
  /**
   * A function to display the main menu for the user.
   */
  static void display_user_menu() {
    print_heading("Main Menu");
    write_line("1. Login");
    write_line("2. Register");
    write_line("3. Exit");
  }
  /**
   * A function to display the login or register screen for the user.
   * @param heading The heading to display in the menu.
   * @return The user entered by the user.
   */
  static User display_login_or_register(string heading) {
    print_heading(heading);
    string username = Helper::Reader::read_string("Username: ");
    string password = Helper::Reader::read_string("Password: ");
    return User{username, password};
  }
  /**
   * A function to display the main menu for the user.
   */
  static void display_main_menu() {
    print_heading("Main Menu");
    write_line("1. Add Task");
    write_line("2. View Task");
    write_line("3. Select Task");
    write_line("4. Logout");
  }

  /**
   * A function to display a task to the user.
   * @param task The task to display.
   * @param id The id of the task.
   */
  static void display_task(const Task& task, int id) {
    write_line();
    write_line("ID: " + to_string(id));
    write_line("Title: " + task.title);
    write_line("Description: " + task.description);
    write_line("Status: " + to_string(task.status));
    write_line("Priority: " + to_string(task.priority));
    write_line("Due Date: " + task.due_date);
    write_line("Start Date: " + task.start_date);
    if (task.tags.size() > 0) write_line("Tags: " + to_string(task.tags));
  }
  /**
   * A function to display a list of tasks to the user.
   * @param tasks The list of tasks to display.
   */
  static void display_tasks(const vector<Task> &tasks, User user) {
    for (int i = 0; i < tasks.size(); i++) {
      if (tasks[i].username == user.username)
        display_task(tasks[i], i);
    }
  }
  /**
   * A function to display a list of tasks to the user with a heading.
   * @param tasks The list of tasks to display.
   * @param heading The heading to display.
   */
  static void display_tasks(const vector<Task>& tasks, string heading, User user) {
    print_heading(heading);
    display_tasks(tasks, user);
  }
  /**
   * A function to display the add task screen for the user.
   * @return The task entered by the user.
   */
  static Task display_add_task() {
    print_heading("Add Task");
    string title = Helper::Reader::read_string("Title: ");
    string description = Helper::Reader::read_string("Description: ");
    TaskStatus status = (TaskStatus)Helper::Reader::read_integer("Status (1. TODO, 2. IN PROGRESS, 3. DONE): ", 1, 3);
    Priority priority = (Priority)Helper::Reader::read_integer("Priority (1. URGENT, 2. HIGH, 3. NORMAL, 4. LOW): ", 1, 4);
    string start_date = Helper::Reader::read_date("Start Date (YYYY-MM-DD): ");
    string due_date = Helper::Reader::read_date("Due Date (YYYY-MM-DD): ");
    vector<string> tags = Helper::Reader::read_tags("Tags (separated by commas): ");
    return Task{"", title, description, status, priority, due_date, start_date, tags};
  }
  /**
   * A function to display the view task screen for the user.
   */
  static void display_view_task_menu() {
    print_heading("View Task");
    write_line("1. View All Tasks");
    write_line("2. View Tasks by Status");
    write_line("3. View Tasks by Priority");
    write_line("4. View Tasks by Due Date");
    write_line("5. View Tasks by Start Date");
    write_line("6. Back");
  }
  /**
   * A function to display the select task screen for the user.
   */
  static void display_select_task_menu() {
    print_heading("Select Task");
    write_line("1. Complete Task");
    write_line("2. Update Task");
    write_line("3. Delete Task");
    write_line("4. Back");
  }
  /**
   * A function to display the update task screen for the user.
   */
  static void display_sort_task_menu() {
    print_heading("Sort Task");
    write_line("1. Sort by Due Date");
    write_line("2. Sort by Start Date");
    write_line("3. Back");
  }
};

/**
 * A struct representing a database with a list of users and tasks.
 * The database stores the users and tasks in vectors.
 */
struct Database {
  vector<User> users; /**< The list of users in the database */ 
  vector<Task> tasks; /**< The list of tasks in the database */

  /**
   * A function to load the data from a file.
   * The function reads the data from a file named "data.json".
   * The function reads the users and tasks from the data and adds them to the users and tasks vectors.
   * The function uses the json library to read the data from the file.
   * The function does nothing if the file does not exist or is empty.
   */
  void load_data() {
    json data = json_from_file("data.json");

    // Check if the data is not empty
    if (json_count_keys(data) != 0) {
      // Read the users from the data
      vector<json> users_json;
      json_read_array(data, "users", users_json);

      // Loop through the users and add them to the users vector
      for (int i = 0; i < users_json.size(); i++) {
        json user_json = users_json[i];
        string username = json_read_string(user_json, "username");
        string password = json_read_string(user_json, "password");
        users.push_back(User{username, password});
      }

      // Read the tasks from the data
      vector<json> tasks_json;
      json_read_array(data, "tasks", tasks_json);

      // Loop through the tasks and add them to the tasks vector
      for (int i = 0; i < tasks_json.size(); i++) {
        json task_json = tasks_json[i];
        string username = json_read_string(task_json, "username");
        string title = json_read_string(task_json, "title");
        string description = json_read_string(task_json, "description");
        TaskStatus status = (TaskStatus)json_read_number(task_json, "status");
        Priority priority = (Priority)json_read_number(task_json, "priority");
        string due_date = json_read_string(task_json, "due_date");
        string start_date = json_read_string(task_json, "start_date");
        vector<string> tags;
        json_read_array(task_json, "tags", tags);
        tasks.push_back(Task{username, title, description, status, priority, due_date, start_date, tags});
      }
    }
    free_all_json();
  }

  /**
   * A function to save the data to a file.
   * The function creates a json object and adds the users and tasks to the data.
   * The function writes the data to a file named "data.json".
   * The function creates a directory named "json" if it does not exist.
   * The function writes the data to the file using the json_to_file function.
   */
  void save_data() {
    json data = create_json();

    // Add the users to the data
    vector<json> users_json;
    for (int i = 0; i < users.size(); i++) {
      json user_json = create_json();
      json_set_string(user_json, "username", users[i].username);
      json_set_string(user_json, "password", users[i].password);
      users_json.push_back(user_json);
    }
    json_set_array(data, "users", users_json);

    // Add the tasks to the data
    vector<json> tasks_json;
    for (int i = 0; i < tasks.size(); i++) {
      json task_json = create_json();
      json_set_string(task_json, "username", tasks[i].username);
      json_set_string(task_json, "title", tasks[i].title);
      json_set_string(task_json, "description", tasks[i].description);
      json_set_number(task_json, "status", tasks[i].status);
      json_set_number(task_json, "priority", tasks[i].priority);
      json_set_string(task_json, "due_date", tasks[i].due_date);
      json_set_string(task_json, "start_date", tasks[i].start_date);
      json_set_array(task_json, "tags", tasks[i].tags);
      tasks_json.push_back(task_json);
    }
    json_set_array(data, "tasks", tasks_json);

    // if file does not exist
    if (json_count_keys(json_from_file("data.json")) == 0) {
      create_directory("json");
      std::ofstream file("json/data.json");
    }

    // Write the data to the file
    json_to_file(data, "data.json");
    free_all_json();
  }
};

struct Manager {
  User user;
  Database db;

  bool is_running = true;
  bool is_logged_in = false;

  /**
   * A function to register the user.
   * The function checks if the username already exists in the database.
   * The function adds the user to the users vector in the database.
   * The function returns true if the registration is successful, false otherwise.
   * @param user The user to register.
   */
  bool register_user(const User& user) {
    for (int i = 0; i < db.users.size(); i++) {
      if (db.users[i].username == user.username) {
        return false;
      }
    }

    db.users.push_back(user);
    return true;
  }

  /**
   * A function to login a user.
   * The function checks if the username and password match a user in the database.
   * The function returns true if the login is successful, false otherwise.
   */
  bool login_user(const User& user) {
    for (int i = 0; i < db.users.size(); i++) {
      if (db.users[i].username == user.username && db.users[i].password == user.password) {
        return true;
      }
    }
    return false;
  }

  /**
   * A function to add a task to the database.
   * The function checks if the user is logged in before adding the task.
   * The function sets the username of the task to the username of the user.
   * The function adds the task to the tasks vector in the database.
   * The function displays a message if the user is not logged in.
   * @param task The task to add to the database.
   */
  void add_task(Task& task) {
    if (is_logged_in) {
      task.username = user.username;
      db.tasks.push_back(task);
    }
    else write_line("Please login to add a task.");
  }

  /**
   * A function to carry out the view tasks menu
   * The function displays a menu to the user with options to view tasks by different criteria.
   * The function displays the tasks to the user based on the selected criteria.
   */
  void view_tasks() {
    bool go_back = false;
    int choice;

    do {
      Menu::display_view_task_menu();
      choice = Helper::Reader::read_integer("Enter your choice: ", 1, 7);

      switch (choice) {
        case 1: {
          Menu::display_tasks(db.tasks, "All Tasks", user);
          break;
        }
        case 2: {
          vector<Task> todoTasks;
          vector<Task> inProgressTasks;
          vector<Task> completedTasks;
          for (int i = 0; i < db.tasks.size(); i++) {
            Task task = db.tasks[i];
            if (task.status == TODO) todoTasks.push_back(task);
            if (task.status == IN_PROGRESS) inProgressTasks.push_back(task);
            if (task.status == COMPLETED) completedTasks.push_back(task);
          }
          Menu::display_tasks(todoTasks, "Todo Tasks", user);
          Menu::display_tasks(inProgressTasks, "In Progress Tasks", user);
          Menu::display_tasks(completedTasks, "Completed Tasks", user);
          break;
        }
        case 3: {
          vector<Task> urgentTasks;
          vector<Task> highTasks;
          vector<Task> normalTasks;
          vector<Task> lowTasks;
          for (int i = 0; i < db.tasks.size(); i++) {
            Task task = db.tasks[i];
            if (task.priority == URGENT) urgentTasks.push_back(task);
            if (task.priority == HIGH) highTasks.push_back(task);
            if (task.priority == NORMAL) normalTasks.push_back(task);
            if (task.priority == LOW) lowTasks.push_back(task);
          }
          Menu::display_tasks(urgentTasks, "Urgent Tasks", user);
          Menu::display_tasks(highTasks, "High Tasks", user);
          Menu::display_tasks(normalTasks, "Normal Tasks", user);
          Menu::display_tasks(lowTasks, "Low Tasks", user);
          break;  
        }
        case 4: {
          vector<Task> sortedTasks = db.tasks;
          Helper::Date::sort_date(sortedTasks, [](vector<Task>& tasks, bool &swapped, int &j) {
            if (tasks[j].due_date > tasks[j + 1].due_date) {
              Task temp = tasks[j];
              tasks[j] = tasks[j + 1];
              tasks[j + 1] = temp;
              swapped = true;
            }
          });
          Menu::display_tasks(sortedTasks, "Tasks by Due Date", user);
          break;
        }
        case 5: {
          vector<Task> sortedTasks = db.tasks;
          Helper::Date::sort_date(sortedTasks, [](vector<Task>& tasks, bool &swapped, int &j) {
            if (tasks[j].start_date > tasks[j + 1].start_date) {
              Task temp = tasks[j];
              tasks[j] = tasks[j + 1];
              tasks[j + 1] = temp;
              swapped = true;
            }
          });
          Menu::display_tasks(sortedTasks, "Tasks by Start Date", user);
          break;
        }
        case 6: 
          go_back = true;
          break;
      }
    } while (!go_back);
  }

  /**
   * A function to select a task from the database.
   * The function prompts the user to enter the task ID.
   * The function displays the task details to the user.
   * The function displays a menu to the user with options to complete, update, delete, or go back.
   */
  void select_task() {
    int id = Helper::Reader::read_integer("Enter the task ID: ");
    bool is_running = true;
    if (id < 0 || id > db.tasks.size()) {
      write_line("Invalid task ID.");
      return;
    }
    Task task = db.tasks[id];

    do {
      Menu::display_task(task, id);
      Menu::display_select_task_menu();
      int choice = Helper::Reader::read_integer("Enter your choice: ", 1, 4);

      switch (choice) {
        case 1:
          task.status = COMPLETED;
          db.tasks[id] = task;
          write_line("Task completed successfully.");
          is_running = false;
          break;
        case 2: {
          int update_choice;

          do {
            Menu::display_select_task_menu();
            update_choice = Helper::Reader::read_integer("Enter your choice: ", 1, 8);

            switch (update_choice) {
              case 1:
                task.title = Helper::Reader::read_string("Enter the new title: ");
                break;
              case 2:
                task.description = Helper::Reader::read_string("Enter the new description: ");
                break;
              case 3:
                task.status = (TaskStatus)Helper::Reader::read_integer("Enter the new status (1. TODO, 2. IN PROGRESS, 3. COMPLETED): ", 1, 3);
                break;
              case 4:
                task.priority = (Priority)Helper::Reader::read_integer("Enter the new priority (1. URGENT, 2. HIGH, 3. NORMAL, 4. LOW): ", 1, 4);
                break;
              case 5:
                task.due_date = Helper::Reader::read_date("Enter the new due date: ");
                break;
              case 6:
                task.start_date = Helper::Reader::read_date("Enter the new start date: ");
                break;
              case 7:
                task.tags = Helper::Reader::read_tags("Enter the new tags (separated by comma): ");
                break;
              case 8:
                break;
            }
          } while (update_choice != 8);
          db.tasks[id] = task;
          break;
        }
        case 3:
          for (int i = 0; i < db.tasks.size(); i++) {
            if (i == id) {
              db.tasks.erase(db.tasks.begin() + i);
              break;
            }
          }
          write_line("Task deleted successfully.");
          is_running = false;
          break;
        case 4:
          is_running = false;
          break;
      }
    } while (is_running);
  }
};

int main() {
  Manager manager = Manager{
    User{"", ""},
    Database{
      vector<User>{},
      vector<Task>{}
    }
  };
  manager.db.load_data();

  int choice;
  do {
    Menu::display_user_menu();
    choice = Helper::Reader::read_integer("Enter your choice: ", 1, 3);

    switch (choice) {
      case 1: {
        User user = Menu::display_login_or_register("Login");
        if (manager.login_user(user)) {
          manager.user = user;
          manager.is_logged_in = true;
          write_line("Login successful.");
        }
        else write_line("Invalid username or password.");
        break;
      }
      case 2: {
        User user = Menu::display_login_or_register("Register");
        if (manager.register_user(user)) {
          manager.user = user;
          manager.is_logged_in = true;
          write_line("Registration successful.");
        }
        else write_line("Username already exists.");
        break;
      }
      case 3:
        manager.is_running = false;
        break;
    }

    if (manager.is_logged_in) {
      do {
        Menu::display_main_menu();
        choice = Helper::Reader::read_integer("Enter your choice: ", 1, 5);

        switch (choice) {
          case 1: {
            Task task = Menu::display_add_task();
            manager.add_task(task);
            break;
          }
          case 2:
            manager.view_tasks();
            break;
          case 3:
            manager.select_task();
            break;
          case 4:
            manager.is_logged_in = false;
            break;
        }
      } while (manager.is_logged_in);
    }
  } while (manager.is_running);

  manager.db.save_data();
}

// clang++ tasky.cpp -o tasky -lstdc++fs -l SplashKit && ./tasky