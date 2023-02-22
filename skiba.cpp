#include <string> 
#include <iostream>

#include "cli.cpp"
#include "vcs.cpp"
#include "cli.h" 
#include "vcs.h" 

bool list_history_for_file(const fs::path& filename) {
std::vector<std::pair<int, std::unordered_map<std::string, std::string>>> snapshots;

for (const auto &entry : fs::recursive_directory_iterator(".archive")) {
if (fs::is_directory(entry.path())) {
int snapshot_id = std::stoi(entry.path().filename().string().substr(10));
std::string hashes_file_path = (entry.path() / "hashes.log").string();
  std::ifstream hashes_log_file(hashes_file_path);
  std::unordered_map<std::string, std::string> snapshot_hashes;
  for (std::string line; getline(hashes_log_file, line); ) {
    std::istringstream iss(line);
    std::string file;
    std::string hash;
    iss >> file >> hash;
    snapshot_hashes[file] = hash;
  }
  snapshots.emplace_back(snapshot_id, snapshot_hashes);
}
}
std::sort(snapshots.begin(), snapshots.end(), [](auto a, auto b) { return a.first > b.first;});

for (const auto& [snapshot_id, snapshot_hashes] : snapshots) {
std::string expected_hash = snapshot_hashes[filename.string().substr(2)];
std::string actual_hash = compute_file_hash(filename);
if (expected_hash == actual_hash) {
std::cout << "File was not modified since snapshot " << snapshot_id << std::endl;
} else {
std::cout << "File was modified after snapshot " << snapshot_id << std::endl;
}
}

return true;
}  \\вывод истории изменений файла