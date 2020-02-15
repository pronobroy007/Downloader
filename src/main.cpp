#include "downloader.h"


void getProcessingData(std::vector<std::string> &url, std::vector<std::string> &downloadID);

int main(int argc, char** argv)
{
    URL urlObj;
	Downloader downloader;
	std::vector<std::string> url; 
	std::vector<std::string> downloadID; 

    /*
    */
    if(argc > 1)
    {
        if(strcmp(argv[1], "resume") == 0)
        {
            urlObj.resumeDownload = true;
            getProcessingData(url, downloadID);
        }
        else if(strcmp(argv[1], "update") == 0)
        {
            urlObj.resumeDownload = true;
            getProcessingData(url, downloadID);
        
            std::cout << "Select one to update url" << std::endl;
            for(int i = 0; i < url.size(); i++)
                std::cout << i <<"    <=>      "<< url[i] << std::endl;
            std::cout << ">> ";
            int val;
            std::cin >> val;

            std::cout << "Insert new url : ";
            std::string inp;
            std::cin >> inp;

            Database db;
            db.init();
            std::vector<std::string> buff;
            buff.push_back(inp);
            buff.push_back(downloadID[val]);
            std::string sql = "UPDATE Tmp SET Url=? WHERE User_ID=?";
            db.prepareStmt(buff, sql, 0);
            db.close();

            url.clear();
            getProcessingData(url, downloadID);
        }
        else
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
            if(inp != "y")
                break;
        }
    }

    //Head request. to see Accept-Ranges: support or not.
    //url.push_back("https://www.youtube.com/watch?v=EwTZ2xpQwpA");
    //url.push_back("http://i.imgur.com/z4d4kWk.jpg");

    //url.push_back("https://rtf.fjrifj.frl/1cbade070a5878aa53657f50c1793492/NdBgEiZAJWo");

    //url.push_back("http://127.0.0.1:5555/home/linux/Music/Ami%20Opar%20Hoye%20Boshe%20Achi%20Bangla.mp3");
	//page for chunked.
    //url.push_back("https://github.com/alireza-amirsamimi/xfce-panel-background-images/tree/master/images");
    //url.push_back("https://www.facebook.com/");


    //Resume download.................
    if(urlObj.resumeDownload == true)
    {
        std::cout << "Select one to resume download " << std::endl;
        for(int i = 0; i < url.size(); i++)
            std::cout << i << "     =>      " << url[i] << std::endl;
        std::cout << ">> ";
        int val;
        std::cin >> val;

        if(val >= 0 && val < url.size())
        {
            urlObj.downloadID = downloadID[val];
            downloader.init(url[val], urlObj);
            Response res = downloader.send_head_req(urlObj);
            downloader.download(urlObj, res.getFileSize(res.fileName));
        }
    }
    else
    {
        for (int i = 0; i < url.size(); i++)
        {
            downloader.init(url[i], urlObj);
            downloader.download(urlObj);
        }
    }

    return 0;
}


void getProcessingData(std::vector<std::string> &url, std::vector<std::string> &downloadID)
{
    Database db;
    std::vector<std::string> buff; 
    db.init();
    std::string sql = "SELECT * FROM Tmp WHERE Status = 'Processing'";
    if(db.prepareStmt(buff, sql, 5))
        for(int i = 0; i < db.data.size(); i ++)
        {
            url.push_back(db.data[i][1]);
            downloadID.push_back(db.data[i][0]);
        }
    db.close();
}

