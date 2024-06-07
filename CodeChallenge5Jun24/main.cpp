/*
* This is the "challenge"  presented:
*  Design and implement a program in which N (where N >= 3) senders sends 
*  messages at arbitrary rate to a receiver and receiver processes messages 
*  received from the senders. A message consists of sender’s id and an 
*  arbitrary data of not more than 10 bytes. Initially, each sender was given 
*  a unique id (type int), so when a sender sending its message it prepends 
*  its id to data. When the receiver receives messages from senders, the 
*  receiver should process message from sender with lower id before 
*  processing message from sender with higher id. For this challenge,  
*  the processing method can print message.
*
*  Example: three senders:
*   sender_1 was assigned its id as 1
*   sender_2 was assigned its id as 2
*   sender_3 was assigned its id as 3
*  sender_2 sends following message:
*   2absd
*  sender_1 sends following message:
*   1tuyuw6
*  sender_3 sends following message:
*   3qwet
*  When receiver receives above three messages it should print messages as follows:
*  1tuyuw6, 2absd, 3qwet
*
*  Discuss possible solutions and implement only one solution in C++. Structure
*  your code using object-oriented principles to allow for future maintenance
*  of the sender and receiver, such as adding new features. Include source-code
*  comments as you would when submittng a change to a large existing C++
*  project. Assume runtimeOS is Linux. Please provide instructions for
*  compilation and running your program.
********* End of presented text and start of task breakdown *********************
* 
There are four pieces to this that are going to be common to all potential solutions:  
They are the sender, the receiver, the messages, with several sets 
of options in each of the cases

Now onto the pieces (the primary objects of this challenge):

1: The sender, given an ID number will periodically populate a buffer with a 
   message and send it to the receiver object. The requirement for multiple 
   senders means that we're going to need to have multiple instances of it 
   running concurrently.  Options for this include the period
   It's life will consist of setting a timer with a value, putting together a
   message, and sending it to the receiver in turn.
   
   
2: This message format, with what's left to interpretation, has the potential 
   of blurring the ID from the arbitrary data (like if the ID is a decimal 
   number and the first byte(s) of the arbitrary data happen to also happen to
   be numeric)

   To facilitate rapid initial functionality, this application will restict 
   the ID format to a decimal number and the data will be restricted to never 
   starting with numerics nor use the value 0x00 for any of its bytes, 
   reserving it to signal the end of the data part of the message (similar  
   to the example). Given that a standard integer ranges up to 4,294,967,296, 
   these restrictions will hold the message length to a maximum of 20 bytes 
   and provide a simple means of parsing the ID from the payload.

   To facilitate reformatting the message to more versatile formats, it will 
   be assembled and broken down using functions tied to it as part of a 
   "message" class.


   One alternative is to use the filesystem to route the message traffic.
   Specifically, the senders would add them to the json file and the
   receiver would watch the status of the file and examine it when it
   sees a change, pulling the messages out and printing them when the
   message count reached the threshold.

   This code was built using Visual Studio 2022 and cross-compiled
   on an embedded Linux system (Beaglebone black)
   Cloning this repository at the Visual Studio
   apt-get install openssh-server build-essential gdb rsync make zip
   apt-get install libjsoncpp-dev
*/


#include <cstdio>
#include <iostream>
#include <cstdint>
#include <thread>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include <cstddef>4
#include <map>
using namespace std;



/// <summary>
/// This class handles formatting and parsing messages 
/// it for "processing"
/// </summary>
/// <param name=""></param>
class message
{

public:
    message(void) = default;
    bool StuffMessageByComponents(uint32_t SenderID, const char* MessageData);
    int StuffMessageWithBuffer(const char* input_buffer);
    uint32_t ReportSenderID(void);
    int ProduceData(char* output_buffer, int size_limit);
    int ProduceMessage(char* output_buffer, int size_limit);


private:
    uint32_t SenderID;
#define MAX_MESSAGE_LEN 20
#define MAX_MSGDATA_LEN 10
    char buffer[MAX_MESSAGE_LEN+1];
};


void MessageClassModuleTest(void);
void ExerciseMessageFunctions(uint32_t ID, const char* MessageData)
{
    message TestMsg;
    char msgbuf[50];
    if (TestMsg.StuffMessageByComponents(ID, MessageData) == false)
        cout << "goodstuff";
    else
        cout << "badstuff";

    memset(msgbuf, 0, sizeof(msgbuf));
    cout << " id=" << TestMsg.ReportSenderID();
    memset(msgbuf, 0, sizeof(msgbuf));
    TestMsg.ProduceData(msgbuf, sizeof(msgbuf));
    cout << " data=" << msgbuf ;
    TestMsg.ProduceMessage(msgbuf, sizeof(msgbuf));
    cout << " msg=" << msgbuf << endl;
    //testcode for storemessage oversize message test TestMsg.StoreMessage("5234567890xxxxxxxxxxyy");
    TestMsg.StuffMessageWithBuffer(msgbuf);
    cout << " msg=" << msgbuf << endl;
}
void MessageClassModuleTest(void)
{

    ExerciseMessageFunctions(12, (const char*)"Benign");
    ExerciseMessageFunctions((uint32_t)4294967295, "ABCDEFGHIJ");
    ExerciseMessageFunctions((uint32_t)4294967295, "K");
    ExerciseMessageFunctions((uint32_t)4294967297, "L");
    ExerciseMessageFunctions((uint32_t)4294967295, "3");
    ExerciseMessageFunctions((uint32_t)4294967295, "MNOPQRSTUVW");
    
    return;
}

/// <summary>
/// MessageStuffer
/// </summary>
/// <param name="SenderID"></param>
/// <param name="MessageData"></param>
/// <returns>false if everything went okay, true if there's going to be...trouble</returns>
bool message::StuffMessageByComponents(uint32_t SenderID, const char *MessageData)
{
    int IdEndPoint;
    this->SenderID = SenderID;
    memset (buffer, 0, sizeof(buffer));
    snprintf(buffer, MAX_MESSAGE_LEN, "%u", this->SenderID);
    IdEndPoint = strlen(buffer);
    strncat((buffer + IdEndPoint), MessageData, (MAX_MESSAGE_LEN - IdEndPoint));
    if (IdEndPoint + strlen(MessageData) > MAX_MESSAGE_LEN)
        return true; // Message truncation happened..probably the payload being too long
    if (MessageData[0] >= '0' && MessageData[0] <= '9')
        return true; // Message data violation! corrupted ID
    return false;
}



uint32_t message::ReportSenderID(void)
{
    return SenderID;
}
int message::ProduceData(char* output_buffer, int size_limit)
{
    int DataStartPoint = 0;
    while ((buffer[DataStartPoint] >= '0' && buffer[DataStartPoint] <= '9') && DataStartPoint < (MAX_MESSAGE_LEN - MAX_MSGDATA_LEN))
        DataStartPoint++;
    strncpy(output_buffer, (buffer + DataStartPoint), size_limit);
    return 0;
}

int message::ProduceMessage(char* output_buffer, int size_limit)
{
    strncpy(output_buffer, buffer, size_limit);
    return 0;
}

int message::StuffMessageWithBuffer(const char* input_buffer)
{
    int input_msglen = strlen(input_buffer);
    int faults = 0;
    memset(buffer,0, sizeof(buffer));
    if (input_msglen  > MAX_MESSAGE_LEN)
    {
        memcpy(buffer, input_buffer, MAX_MESSAGE_LEN);
        faults = -1;
    }
    else
    {
        memcpy(buffer, input_buffer, input_msglen);
    }
    sscanf(input_buffer, "%u", &SenderID);
    return faults;
}
class receiver
{
public:
    receiver(void);
    ~receiver(void);
    map < uint32_t, message> message_repository;
    void take_message(uint32_t id, char buffer[]);
};
receiver::take_message(uint32_t id, char buffer[])
{
    message_repository[id] = buffer;
}


class sender
{
   


public:

    sender(void);
    sender(int idnum, int period,map<uint32_t,message>  receiver);
    ~sender(void);
    void spammer(void);

private:
    uint32_t MessageSendPeriod;
    uint32_t SenderIDValue;
    message *delivery_channel;
};
sender::sender(void)
{
    sender(1, 1,NULL);
}
sender::sender(int idnum, int period, map<uint32_t, message> receiver)
{
    SenderIDValue = idnum;
    MessageSendPeriod = period;
    delivery_channel = receiver;

}
sender::~sender(void)
{

}

void sender::spammer(void)
{
    //auto start = std::chrono::high_resolution_clock::now();
    message spam;
    //char data[10];
    char MessageBuffer[MAX_MESSAGE_LEN+1];
    char UniqueData[MAX_MSGDATA_LEN + 1];
    uint16_t spamcount = 0;

    while (MessageSendPeriod != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(MessageSendPeriod));
 
        snprintf(UniqueData, (MAX_MSGDATA_LEN), "spm0x%4.4x",spamcount++);
        spam.StuffMessageByComponents(SenderIDValue, UniqueData);
        spam.ProduceMessage(MessageBuffer, MAX_MESSAGE_LEN + 1);
        // test output to verify that the messages above are coming out
        cout << MessageBuffer << endl;

        Messagelog[spam.ReportSenderID] = MessageBuffer;
    }

}


int main()
{
    string line_in;
    static std::map<uint32_t, message> MessageLog;

    sender sender1 = sender(1, 1200,MessageLog);
    sender sender2 = sender(2, 1500,MessageLog);
    sender sender3 = sender(3, 1800,MessageLog);
    sender sender4 = sender(4, 1950,MessageLog);
    std::thread s1(&sender::spammer, &sender1); // s1 runs sender::spammer on object sender1
    std::thread s2(&sender::spammer, &sender2); // s1 runs sender::spammer on object sender2
    std::thread s3(&sender::spammer, &sender3); // s1 runs sender::spammer on object sender3
    std::thread s4(&sender::spammer, &sender3); // s1 runs sender::spammer on object sender3
    printf("Starting demo for %s!\n", "CodeChallenge5Jun24");
 
    //while (MessageSendPeriod != 0)
    //{
    //    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    //}
    //cin >> line_in;
    //cin >> line_in;
    //cin >> line_in;

    return 0;

}
