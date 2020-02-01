#include "downloader.h"

int main(int argc, char** argv)
{
	Downloader downloader;
	std::vector<std::string> url; 

    /*
    if(argc > 1)
    {
        for(int i = 1; i < argc; i++)
            url.push_back(argv[i]);
    }
    else 
    {
        while(true)
        {
            std::string inp;
            std::cout << "Enter url : ";
            std::cin >> inp;
            url.push_back(inp);

            std::cout << "Add more url? y/n " << std::endl;
            std::cin >> inp;
            if(inp == "n")
                break;
        }
    }
    */
    //https://github.com/alireza-amirsamimi/xfce-panel-background-images/tree/master/images

    //url.push_back("http://127.0.0.1:5555/home/linux/Music/Ami%20Opar%20Hoye%20Boshe%20Achi%20Bangla.mp3");
	//page for chunked.
    url.push_back("https://github.com/alireza-amirsamimi/xfce-panel-background-images/tree/master/images");
    url.push_back("https://www.facebook.com/");


	for (int i = 0; i < url.size(); i++)
		downloader.download(url[i]);

    return 0;
}


