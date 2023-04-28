#include"filePath.h"

string getExePath(){
#ifdef WIN32
    char szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[0] = 0; 
	string path = szFilePath;
	return path;
#else
    char buf[MAX_PATH + 1] = { 0 };
    ssize_t len=readlink("/proc/self/exe",buf,sizeof(buf)-1);
    if(len!=-1){
        buf[len]='\0';
        char* lastSlash = strrchr(buf,'/');
        if(lastSlash!=NULL){
            *lastSlash='\0';
        }
        std::cout<<"Executable path:"<<buf<<std::endl;
        return buf;
    }
    else{
        std::cerr<<"Failed to get Executable path:"<<std::endl;
    }
#endif
}

string getPath_obj(std::string& exePath,std::string& name){
#ifdef WIN32
    std::string str = exePath;
    str += "\\data\\" + name;
    std::string objFile = str + ".obj";
    return objFile;
#else
    std::string str = exePath;
    std::string postfix=".obj";
    str += "/data/" + name;
    std::string objFile = str + postfix;
    return objFile.c_str();
#endif
}

string getPath_mtl(std::string path){
#ifdef WIN32
    auto i = path.rfind('\\');
    std::string mtlBase = path.substr(0, i);
    return mtlBase;
#else
    auto i = path.rfind('/');
    std::string mtlBase = path.substr(0, i);
    return mtlBase;
#endif
}