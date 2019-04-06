/*MineNotif v0.1
*
* Developed by Wistaro
*
* This sofware will listen the chat of Minecraft and send desktop notifications in case of mentions or PM.
*
* Help and feedback on my Discord: Wistaro#9308
*/
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string.h>
#include <windows.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"


using namespace std;
static const char *NotifSong = "notif.wav";

int main(int argc, char* argv[]){

    /*Window initialisation*/
    cout << "**********************************************" << endl;
    printf("~ MineNotif v0.1 ~ \n Made by Wistaro\n");
    cout << "**********************************************" << endl <<endl;

    cout << "-----------------------------------------------------------" << endl;

    SDL_Window* fenetre(0);


    // Initialisation de la SDL
    printf("Initializing files...\n");
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "Failed to load SDL : " << SDL_GetError() << endl << endl;
        SDL_Quit();
        system("pause");
        return -1;
    }

    /*Mixer init*/
    int result = 0;
    int flags = MIX_INIT_OGG;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Failed to init SDL\n \n");
        system("pause");
        return 0;
    }
    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
    Mix_Music *music = Mix_LoadMUS(NotifSong);

    /*Loading logfile*/
    int nbLignes, cpt, i, j, k;
    int notif = 0;
    i = 0;
    size_t pos_chat;
    string ligne, lastligne,lastlignebuff,chatOnly,psdMp, mpMsg, logPath, ligneBuff, psdUser;
    string detectIsMe;
    string wordDetected, mpMessageNotif, isNotif, isLogs;
    string lowerCaseString;
    const char * lastligneConst;
    char flagNotif = 0;
    psdMp = "x";

    /*buffer*/

    char buffMsg[1024];

    /*Export data from config file*/
        ifstream configfile("config.txt" , ios::in);

        if(configfile.good()){
                while(getline( configfile, ligneBuff)){
                    i++;
                    if(i == 1) logPath = ligneBuff;
                    if(i == 2) psdUser = ligneBuff;
                    if(i == 3) wordDetected = ligneBuff;
                    if(i == 4) isNotif = ligneBuff;
                    if(i == 5) isLogs = ligneBuff;
                }

        }else{
            printf("Impossible de lire le fichier de configuration de MineNotif\n \n");
            system("pause");
            return 0;
        }

    /*Some transformations*/
    transform(psdUser .begin(), psdUser .end(), psdUser .begin(), ::tolower);
    transform(wordDetected .begin(), wordDetected .end(), wordDetected .begin(), ::tolower);

    cout << "Loading data from " << logPath << endl;
    cout << "Detected username:  " << psdUser << endl;
    cout << "Additionnal word detected:  " << wordDetected  << endl;

     if(isNotif == "notifs=true"){
         cout << "Desktop notifications: on" << endl;
     }else{
         cout << "Desktop notifications: off" << endl;
     }
      if(isLogs == "displayLogs=true"){
         cout << "Displays logs messages: on" << endl;
     }else{
         cout << "Display logs messages: off" << endl;
     }


     cout << "-----------------------------------------------------------" << endl << endl;

    while(1){
            nbLignes = 0;
            cpt = 0;

    ifstream logfile(logPath+"/latest.log" , ios::in);

    if(!logfile.good()){
        printf("\nImpossible de lire le fichier de log!\n \n");
        system("pause");
        return 0;
    }

    while(std::getline( logfile, ligne))
    {
        nbLignes++;
    }

logfile.clear();
logfile.seekg(0, ios::beg);
while(logfile)
        {
            getline(logfile, ligne);
            cpt++;
            if (cpt ==  nbLignes)
               lastligne = ligne;
        }

        lastligneConst  = lastligne.c_str();


        if(lastlignebuff != lastligne && strstr(  lastligneConst, "[CHAT]") != NULL){

            pos_chat = lastligne.find('CHAT')+3;
            chatOnly = lastligne.substr (pos_chat);


             if(strstr(  chatOnly.c_str() , "-> moi") != NULL){
                notif = 1; //mp notif
                psdMp = chatOnly.substr(1, (chatOnly.find("->")-2));
                mpMsg = chatOnly.substr(chatOnly.find("-> moi")+8);

                cout << "MP de  " << psdMp << ">> " << mpMsg  << endl;

            }else{
                    cout << "CHAT >> ";
                    CharToOemA(chatOnly.c_str(), buffMsg);

                    cout  << buffMsg <<endl;
                    //cout  << chatOnly <<endl;
            }

                lowerCaseString = lastligne;
                transform(lowerCaseString.begin(), lowerCaseString.end(), lowerCaseString.begin(), ::tolower);

                if(strstr(lowerCaseString.c_str(), wordDetected.c_str()) != NULL || strstr(lowerCaseString.c_str(),psdUser.c_str()) != NULL){
                     //on détecte si c'est pas moi qui parle
                     detectIsMe = lastligne.substr(1,  lastligne.find(">"));
                     transform(detectIsMe.begin(), detectIsMe.end(), detectIsMe.begin(), ::tolower);

                       if(strstr(  detectIsMe.c_str() ,psdUser.c_str() ) != NULL){
                            notif = 0;
                       }else{
                            notif =1;
                       }

                }


        }else if(lastlignebuff != lastligne){
         if(isLogs == "displayLogs=true"){
                    cout << "LOG >> "<< lastligneConst << endl;
                }
        }
    lastlignebuff = lastligne;

        if(notif == 1 && psdMp != "x"){
            //notification de MP
            mpMessageNotif = "Vous avez recu un message privé de "+psdMp+" !\n"+mpMsg;
            Mix_PlayMusic(music, 1);
            if(isNotif == "notifs=true"){
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                         "Message privé reçu",
                         mpMessageNotif.c_str(),
                         fenetre);
            }
            notif = 0;
            psdMp = "x";
        }else if(notif == 1){
            //Autre notification
            Mix_PlayMusic(music, 1);
             if(isNotif == "notifs=true"){
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                         "Notification",
                         "Vous avez recu une notification!.",
                         fenetre);
             }
            notif = 0;


        }
    }



    /*Exiting programm*/
    while (!SDL_QuitRequested()) {
        SDL_Delay(250);
    }

    Mix_FreeMusic(music);
    SDL_Quit();
    return 0;
}
