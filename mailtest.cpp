#include "mailtest.h"
#include <iostream>

mailtest::mailtest()
{
    /*std::cout << "1" << std::endl;
    mailio::imaps connection("imap.gmail.com", 993);
    std::cout << "2" << std::endl;
    try {
        auto res = connection.authenticate("gyoergysarvari@gmail.com", "nw__)$oU\"3@y+2!qEF2!fS$k",
                            mailio::imaps::auth_method_t::LOGIN);
        std::cout << "auth res: " << res << std::endl;
    } catch(mailio::imap_error e){
        std::cout << "auth exception: x" << e.what() << "x" << std::endl;
        exit(1);
    }

    std::cout << "3" << std::endl;
    mailio::imaps::mailbox_folder_t folders = connection.list_folders("");
    std::cout << "4" << std::endl;

    for (const auto& f: folders.folders){
        std::cout << "folder: " << f.first << std::endl;
    }

    std::cout << "5" << std::endl;*/

}
