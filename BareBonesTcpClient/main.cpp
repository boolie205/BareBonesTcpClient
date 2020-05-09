#include <iostream>
#include <ws2tcpip.h>
#include <string>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
   string ipAddress = "127.0.0.1"; // ip of server
   int port = 54000; // listening port on server
  
   //init winsoc
   WSADATA data;
   WORD ver = MAKEWORD(2,2);
   int wsResult = WSAStartup(ver, &data);
   if (wsResult !=0)
   {
       cerr << "cant start winsoc, Err#" << wsResult << endl;
       return;
   }
   //create socket
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock == INVALID_SOCKET)
   {
       cerr << "cant create socket, err # " << WSAGetLastError() << endl;
       WSACleanup();
       return;
   }

   // fill in hint structure
   sockaddr_in hint;
   hint.sin_family = AF_INET;
   hint.sin_port = htons(port);
   inet_pton(AF_INET,ipAddress.c_str(), &hint.sin_addr);

   //connect
   int conResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
   if (conResult == SOCKET_ERROR)
   {
       cerr << "cant connect to server, err#" << WSAGetLastError() << endl;
       closesocket(sock);
       WSACleanup();
   }
   //DO-While loop  to send and receive data
   char buf[4096];
   string userInput;

   do
   {
       //prompt user for some text
       cout << ">";
       getline(cin,userInput);
       if (userInput.size() > 0)
       {
       //send text 
       int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0); // gotta add +1 for terminating 0 so even if my size of message is 5, its actually 6 because of trailing \0 denoting string end.
       //wait for response.
       if (sendResult != SOCKET_ERROR)
       {
           ZeroMemory(buf, 4096);
           int bytesReceived = recv(sock, buf, 4096, 0);
           if (bytesReceived > 0)
           {
               cout << "SERVER>" << string(buf, 0, bytesReceived) << endl;
           }
       }
       //echo response in console
       }
   } while (userInput.size() > 0);
   //gracefully shut down
   closesocket(sock);
   WSACleanup();
}