#include <iostream>
#include <string>

#include <git2.h>

int main(int argc, char* argv[]) {
    const std::string repo_path = ".";
    git_repository* repo;
    int error = git_repository_open(&repo, repo_path.c_str());
    if (error != 0) {
        std::cerr << "Failed to open repository: " << git_error_last()->message << std::endl;
        return error;
    }

    git_status_options status_options = GIT_STATUS_OPTIONS_INIT;
    int init_error = git_status_init_options(&status_options, GIT_STATUS_OPTIONS_VERSION);
    if (init_error != 0) {
        std::cerr << "Failed to initialize status options: " << git_error_last()->message << std::endl;
        git_repository_free(repo);
        return init_error;
    }
    status_options.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    status_options.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
        GIT_STATUS_OPT_SORT_CASE_SENSITIVELY | GIT_STATUS_OPT_EXCLUDE_SUBMODULES;
    status_options.pathspec.count = 0;

    error = git_status_foreach(repo, &status_options, [](const char* path, unsigned int flags, void* payload) -> int {
        std::cout << path << " ";
        if (flags & GIT_STATUS_INDEX_NEW) {
            std::cout << "added to index ";
        }
        if (flags & GIT_STATUS_INDEX_MODIFIED) {
            std::cout << "modified in index ";
        }
        if (flags & GIT_STATUS_WT_NEW) {
            std::cout << "untracked in workdir ";
        }
        if (flags & GIT_STATUS_WT_MODIFIED) {
            std::cout << "modified in workdir ";
        }
        if (flags & GIT_STATUS_WT_TYPECHANGE) {
            std::cout << "typechanged in workdir ";
        }
        std::cout << std::endl;
        return 0;
        }, nullptr);

    if (error != 0) {
        std::cerr << "Failed to iterate over repository status: " << git_error_last()->message << std::endl;
    }

    int free_error = git_repository_free(repo);
    if (free_error != 0) {
        std::cerr << "Failed to free repository: " << git_error_last()->message << std::endl;
        return free_error;
    }

    return error;
}
