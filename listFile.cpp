#include <sys/types.h>
#include <dirent.h>
#include <string> 
#include <iostream>
#include <string.h>
using namespace std;

bool isContain(char *a,const char *b)
{
    return (strstr(a,b) ? true : false);
}
void read_directory(std::string directory, std::string filename)
{
    DIR* dirp = opendir(directory.c_str());
    if (dirp == NULL)
    {
        cout << "Wrong directory" << endl;
        return;
    }
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if (dp->d_type != DT_DIR)
        {
            if (isContain(dp->d_name, filename.c_str()))
                cout << directory << "/" << dp->d_name << endl;
            continue;
        }
        if (!strcmp(dp->d_name, "."))
            continue;
        if (!strcmp(dp->d_name, ".."))
            continue;
        string dir = directory + "/" + dp->d_name; 
        read_directory(dir, filename);
    }
    closedir(dirp);
}
int main()
{
    string dir, filename;
    cout << "Give me the directory to find: " << endl;
    cin >> dir;
    cout << "Give me the file name: " << endl;
    cin >> filename;
    read_directory(dir, filename);
    return 0;
}
