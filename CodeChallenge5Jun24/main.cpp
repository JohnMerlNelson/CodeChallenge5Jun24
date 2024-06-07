


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
*  and running your program.
********* End of presented text and start of task breakdown *********************/

//#define COMPILE_MODULE_TEST_ROUTINES


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

#ifdef COMPILE_MODULE_TEST_ROUTINES
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
#endif



/// <summary>
/// This function assembles the message to transmit and sets the ID value
/// when passed the id number and data
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

/// <summary>
/// This is the corrolary to the message assembly from components and is meant
/// to populate the message structure given a complete transmission formatted
/// message
/// </summary>
/// <param name="input_buffer"></param>
/// <returns></returns>
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


/// <summary>
/// This function reports the SenderID as set.
/// </summary>
/// <param name=""></param>
/// <returns></returns>
uint32_t message::ReportSenderID(void)
{
    return SenderID;
}

/// <summary>
/// This function, though not explicitly used in this challenge, is
/// meant to provide access to just the data portion of the message
/// </summary>
/// <param name="output_buffer"></param>
/// <param name="size_limit"></param>
/// <returns></returns>
int message::ProduceData(char* output_buffer, int size_limit)
{
    int DataStartPoint = 0;
    while ((buffer[DataStartPoint] >= '0' && buffer[DataStartPoint] <= '9') && DataStartPoint < (MAX_MESSAGE_LEN - MAX_MSGDATA_LEN))
        DataStartPoint++;
    strncpy(output_buffer, (buffer + DataStartPoint), size_limit);
    return 0;
}

/// <summary>
/// This function produces the combined message (formatted ID and data together)
/// </summary>
/// <param name="output_buffer"></param>
/// <param name="size_limit"></param>
/// <returns></returns>
int message::ProduceMessage(char* output_buffer, int size_limit)
{
    strncpy(output_buffer, buffer, size_limit);
    return 0;
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



/// <summary>
/// This class is meant to do one thing: send messages
/// </summary>
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
    message spam;
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

    cout << "Starting demo for %s!\n" << "CodeChallenge5Jun24";

    sender sender1 = sender(1, 1200,MessageLog);
    sender sender2 = sender(2, 1500,MessageLog);
    sender sender3 = sender(3, 1800,MessageLog);
    sender sender4 = sender(4, 1950,MessageLog);
    std::thread s1(&sender::spammer, &sender1); // s1 runs sender::spammer on object sender1
    std::thread s2(&sender::spammer, &sender2); // s1 runs sender::spammer on object sender2
    std::thread s3(&sender::spammer, &sender3); // s1 runs sender::spammer on object sender3
    std::thread s4(&sender::spammer, &sender3); // s1 runs sender::spammer on object sender3
 
    while (MessageSendPeriod != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    //cin >> line_in;
    //cin >> line_in;
    //cin >> line_in;

    return 0;

}
