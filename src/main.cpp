#include "downloader.h"

int main()
{
	Downloader downloader;
					
	std::vector<std::string> url; 
	//page for chunked.
	url.push_back("http://10.16.100.244/"); 
	//Pic download for content_length.
	url.push_back("http://10.16.100.244/files/speed_Best%20lalon%20Geeti%20poster.png");
	//rar download for content_length.
	url.push_back("http://10.16.100.241/ftps3d6/Dual%20Audio/LALON%20GEETI.rar");
    //url.push_back("127.0.0.1/");
	//url.push_back("http://banglasongs.fusionbd.com/downloads/download.php?file=mp3/bangla/Shomi_Ft_Project_365/02.Mon_Amar_Dehoghori.mp3");

	for (int i = 0; i < url.size(); i++)
		downloader.download(url[i]);

    return 0;
}


