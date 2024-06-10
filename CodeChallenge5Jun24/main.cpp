


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

///This #define enables module test routines for the message object
//#define DO_MODULE_TEST_ROUTINES

#define MAX_MESSAGE_LEN 20
#define MAX_MSGDATA_LEN 10
#define MESSAGE_BATCH_PROCESSING_THRESHOLD 3
#define MESSAGE_SENDER_COUNT 4

#include <cstdio>
#include <iostream>
#include <cstdint>
#include <thread>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cstddef>
#include <queue>
#include <mutex>
#include <map>
#include <condition_variable>

using namespace std;



/// <summary>
/// This class handles the assembly and disassembly of the messages that
/// the sender and receiver objects below will use as part of their
/// operation.
/// </summary>
/// <param name=""></param>
class message
{

public:
    message(void) = default;
    int StuffMessageByComponents(uint32_t SenderID, const char* MessageData);
    int StuffMessageWithBuffer(const char* input_buffer);
    int StuffMessageWithString(string* input_buffer);
    uint32_t ReportSenderID(void);
    int ProduceData(char* output_buffer, int size_limit);
    int ProduceMessage(char* output_buffer, int size_limit);


private:
    uint32_t SenderID;
    char buffer[MAX_MESSAGE_LEN+1];
};




/// <summary>
/// This function assembles the message to transmit and sets the ID value
/// when passed the id number and data
/// </summary>
/// <param name="SenderID">ID number of the sender making this message</param>
/// <param name="MessageData">the data payload of this message</param>
/// <returns>false if everything went okay, true if there's going to be...trouble</returns>
int message::StuffMessageByComponents(uint32_t SenderID, const char *MessageData)
{
    int IdEndPoint;
    this->SenderID = SenderID;
    memset (buffer, 0, sizeof(buffer));
    snprintf(buffer, MAX_MESSAGE_LEN, "%u", this->SenderID);
    IdEndPoint = strlen(buffer);
    strncat((buffer + IdEndPoint), MessageData, (MAX_MESSAGE_LEN - IdEndPoint));
    if (IdEndPoint + strlen(MessageData) > MAX_MESSAGE_LEN)
        return -1; // Message truncation happened..probably the payload being too long
    if (MessageData[0] >= '0' && MessageData[0] <= '9')
        return -1; // Message data violation! corrupted ID
    return 0;
}

/// <summary>
/// This is the corrolary to the message assembly from components and is meant
/// to populate the message structure given a complete transmission formatted
/// message
/// </summary>
/// <param name="input_buffer">The old school string containing a message</param>
/// <returns>0 for success, -1 if problems were encounterd indicating potential loss
/// of data</returns>
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
    // collect the sender's ID number for speedy access later
    // Old-school methods of converting a number to a string were employed here
    // as a result of sscanf(input_buffer, "%uj", &temp_id); not performing to my liking.
    int offset = 0;
    uintmax_t temp_id=0;
    while (((buffer[offset] >= '0') && (buffer[offset] <= '9')) && (offset < MAX_MESSAGE_LEN))
    {
        temp_id = (temp_id * (uintmax_t) 10) + (uintmax_t)(buffer[offset] - '0');
        offset++;
    };
    if (temp_id > UINT32_MAX)
    {
        SenderID = (uint32_t)(temp_id & (0xffffffff));
        faults = -1;
    }
    else
    {
        SenderID = (uint32_t)temp_id;
    }
    return faults;
}

/// <summary>
/// This is the corrolary to the message assembly from components and is meant
/// to populate the message structure given a complete transmission formatted
/// message
/// </summary>
/// <param name="input_buffer">The C++ class string containing a message</param>
/// <returns>0 for success, -1 if problems were encounterd indicating potential loss
/// of data</returns>
int message::StuffMessageWithString(string *input_buffer)
{
    int input_msglen = input_buffer->length();
    int faults = 0;
    memset(buffer, 0, sizeof(buffer));
    if (input_msglen > MAX_MESSAGE_LEN)
    {
        //input_buffer->
        memcpy(buffer, input_buffer->c_str(), MAX_MESSAGE_LEN);
        faults = -1;
    }
    else
    {
        memcpy(buffer, input_buffer->c_str(), input_msglen);
    }
    // collect the sender's ID number for speedy access later
    // Old-school methods of converting a number to a string were employed here
    // as a result of sscanf(input_buffer, "%uj", &temp_id); not performing to my liking.
    int offset = 0;
    uintmax_t temp_id = 0;
    while (((buffer[offset] >= '0') && (buffer[offset] <= '9')) && (offset < MAX_MESSAGE_LEN))
    {
        temp_id = (temp_id * (uintmax_t)10) + (uintmax_t)(buffer[offset] - '0');
        offset++;
    };
    if (temp_id > UINT32_MAX)
    {
        SenderID = (uint32_t)(temp_id & (0xffffffff));
        faults = -1;
    }
    else
    {
        SenderID = (uint32_t)temp_id;
    }
    return faults;
}


/// <summary>
/// This function reports the SenderID as set.
/// </summary>
/// <param name=""></param>
/// <returns>the id number of the sender of this message</returns>
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
/// <returns>0 for success</returns>
int message::ProduceData(char* output_buffer, int size_limit)
{
    int DataStartPoint = 0;
    // find the offset to the start of the data part of the message
    while ((buffer[DataStartPoint] >= '0' && buffer[DataStartPoint] <= '9') && DataStartPoint < (MAX_MESSAGE_LEN - MAX_MSGDATA_LEN))
        DataStartPoint++;

    // With that starting point, copy the data to the supplied buffer for the calling function
    strncpy(output_buffer, (buffer + DataStartPoint), size_limit);
    return 0;
}

/// <summary>
/// This function produces the combined message (formatted ID and data together)
/// </summary>
/// <param name="output_buffer">A pointer to a character buffer to hold the message</param>
/// <param name="size_limit">the size of that buffer to prevent overruns</param>
/// <returns>0 for success</returns>
int message::ProduceMessage(char* output_buffer, int size_limit)
{
    // copy the entire message to the calling function's supplied buffer
    strncpy(output_buffer, buffer, size_limit);
    return 0;
}

#ifdef DO_MODULE_TEST_ROUTINES
void MessageClassModuleTest(void);
/// <summary>
/// This function parses a message as contained in a C++ string and  
/// then it exercises the reporting functions
/// </summary>
/// <param name="Message">The C++ class string containing the message</param>
void TestStuffMessageByString(string *Message)
{
    message TestMsg;
    char msgbuf[50];
    if (TestMsg.StuffMessageWithString(Message) == 0)
        cout << "goodstuff";
    else
        cout << "badstuff";

    memset(msgbuf, 0, sizeof(msgbuf));
    cout << " id=" << TestMsg.ReportSenderID();
    memset(msgbuf, 0, sizeof(msgbuf));
    TestMsg.ProduceData(msgbuf, sizeof(msgbuf));
    cout << " data=" << msgbuf;
    TestMsg.ProduceMessage(msgbuf, sizeof(msgbuf));
    cout << " msg=" << msgbuf ;
    cout << " orig message=" << *Message << endl;
}




/// <summary>
/// This function parses a message as contained in a traditional
/// C (null terminated) string 
/// then it exercises the reporting functions
/// </summary>
/// <param name="Message">the traditional C string message to process</param>
void TestStuffMessageByCString(const char* Message)
{
    message TestMsg;
    char msgbuf[50];
    if (TestMsg.StuffMessageWithBuffer(Message) == 0)
        cout << "goodstuff";
    else
        cout << "badstuff";
    //testcode for storemessage oversize message test: TestMsg.StoreMessage("5234567890xxxxxxxxxxyy");

    memset(msgbuf, 0, sizeof(msgbuf));
    cout << " id=" << TestMsg.ReportSenderID();
    memset(msgbuf, 0, sizeof(msgbuf));
    TestMsg.ProduceData(msgbuf, sizeof(msgbuf));
    cout << " data=" << msgbuf;
    TestMsg.ProduceMessage(msgbuf, sizeof(msgbuf));
    cout << " msg=" << msgbuf;
    cout << " orig message=" << Message << endl;
}

/// <summary>
/// This function builds a message given the ID and the message data
/// then it exercises the reporting functions
/// </summary>
/// <param name="ID">the sender ID to put in the message</param>
/// <param name="MessageData">the message payload</param>
void TestStuffMessageByComponets(uint32_t ID, const char* MessageData)
{
    message TestMsg;
    char msgbuf[50];
    if (TestMsg.StuffMessageByComponents(ID, MessageData) == 0)
        cout << "goodstuff";
    else
        cout << "badstuff";
    //testcode for storemessage oversize message test: TestMsg.StoreMessage("5234567890xxxxxxxxxxyy");

    memset(msgbuf, 0, sizeof(msgbuf));
    cout << " id=" << TestMsg.ReportSenderID();
    memset(msgbuf, 0, sizeof(msgbuf));
    TestMsg.ProduceData(msgbuf, sizeof(msgbuf));
    cout << " data=" << msgbuf ;
    TestMsg.ProduceMessage(msgbuf, sizeof(msgbuf));
    cout << " msg=" << msgbuf;
    cout << " orig id=" << ID;
    cout << " orig payload=" << MessageData << endl;
}

/// <summary>
/// This function runs the test function(s) above exercising the code
/// for the purpose of validation
/// </summary>
void MessageClassModuleTest(void)
{

    TestStuffMessageByComponets(12, (const char*)"Benign");
    TestStuffMessageByComponets((uint32_t)4294967295, "ABCDEFGHIJ");
    TestStuffMessageByComponets((uint32_t)4294967295, "K");
    TestStuffMessageByComponets((uint32_t)4294967297, "L");
    TestStuffMessageByComponets((uint32_t)4294967295, "3");
    TestStuffMessageByComponets((uint32_t)4294967295, "MNOPQRSTUVW");
    
    TestStuffMessageByCString("12Benign");
    TestStuffMessageByCString("4294967295ABCDEFGHIJ");
    TestStuffMessageByCString("4294967295K");
    TestStuffMessageByCString("4294967297L");
    TestStuffMessageByCString("42949672953");
    TestStuffMessageByCString("4294967295MNOPQRSTUVW");
    TestStuffMessageByCString("5234567890xxxxxxxxxxyy");
    TestStuffMessageByCString("52345678xxsxxxxxxxxxxyy");

    string tempstring;
    tempstring = "12Benign";
    TestStuffMessageByString(&tempstring);
    tempstring = "4294967295ABCDEFGHIJ";
    TestStuffMessageByString(&tempstring);
    tempstring = "4294967295K";
    TestStuffMessageByString(&tempstring);
    tempstring = "4294967297L";
    TestStuffMessageByString(&tempstring);
    tempstring = "42949672953";
    TestStuffMessageByString(&tempstring);
    tempstring = "4294967295MNOPQRSTUVW";
    TestStuffMessageByString(&tempstring);
    tempstring = "5234567890xxxxxxxxxxyy";
    TestStuffMessageByString(&tempstring);
    tempstring = "12345678xxxxxxxxxxxxxxyy";
    TestStuffMessageByString(&tempstring);


    return;
}
#endif





/// <summary>
/// This is a thread-safe queue borrowed and adapted from
/// https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/
/// It differs from the example by the addition of a queue size 
/// reporting function.
/// </summary>
/// <typeparam name="T">Implementation datatype</typeparam>
template <typename T>
class TSQueue {
private:
    // Underlying queue 
    std::queue<T> m_queue;

    // mutex for thread synchronization 
    std::mutex m_mutex;

    // Condition variable for signaling 
    std::condition_variable m_cond;

public:
    // Pushes an element to the queue 
    void push(T item)
    {

        // Acquire lock 
        std::unique_lock<std::mutex> lock(m_mutex);

        // Add item 
        m_queue.push(item);

        // Notify one thread that 
        // is waiting 
        m_cond.notify_one();
    }

    // Pops an element off the queue 
    T pop()
    {

        // acquire lock 
        std::unique_lock<std::mutex> lock(m_mutex);

        // wait until queue is not empty 
        m_cond.wait(lock,
            [this]() { return !m_queue.empty(); });

        // retrieve item 
        T item = m_queue.front();
        m_queue.pop();

        // return item 
        return item;
    }
    int size()
    {
        // acquire lock 
        std::unique_lock<std::mutex> lock(m_mutex);

        // retrieve item 
        int count = m_queue.size();

        // return item 
        return count;
    }
};



/// <summary>
/// This is the challenge's "receiver" class.  Uts ubutuak 
/// </summary>
class receiver
{
public:
    receiver();
    ~receiver(void);

    void CheckForAndProcessMessages(TSQueue<string>* buffah);
    std::multimap<uint32_t, string> MessageCollection;
    string line_in;
    string latest;
    message LastMsg;

};


receiver::receiver()
{
}

receiver::~receiver(void)
{
}

/// <summary>
/// This function works the task of receiving and processing messages
/// </summary>
/// <param name="IncomingMessages">The queue through which the messages are received</param>
void receiver::CheckForAndProcessMessages(TSQueue<string>* IncomingMessages)
{
    int messagecount = IncomingMessages->size();
    if (messagecount >= MESSAGE_BATCH_PROCESSING_THRESHOLD)
    {
        // collect and archive three messages
        for (int lcv = 0; lcv < MESSAGE_BATCH_PROCESSING_THRESHOLD; lcv++)
        {
            latest = IncomingMessages->pop();
            LastMsg.StuffMessageWithString(&latest);
            // Unit testing showed this queuing business, with 4 threads,
            // pumped out over 8000 messages without loss.
            MessageCollection.insert(std::pair<uint32_t, string>(LastMsg.ReportSenderID(), latest));

        }
        // show content:
        for (std::map<uint32_t, string>::iterator it = MessageCollection.begin(); it != MessageCollection.end(); ++it)
            std::cout << (*it).second << ", ";
        cout << endl;
        MessageCollection.clear();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}


/// <summary>
/// This class is meant to do one thing: send messages
/// </summary>
class sender
{
public:

    sender(int idnum, int period,TSQueue<string>  *buffah);
    ~sender(void);
    void spammer(void);
    uint32_t MessageSendPeriod;
    void do_send_message(char* UniqueData);
private:
    // This is the ID number to be assigned to this instance of sender.
    uint32_t SenderIDValue;
    // This is the pointer to the output queue that channels the
    // messages to the receiver.
    TSQueue<string> *delivery_channel;
};

/// <summary>
/// This constructor sets up the sender ID, the queue to send the messages
/// out, and a period to give the spammer task a rate in which to
/// send the messages at
/// </summary>
/// <param name="idnum">sender ID</param>
/// <param name="period">Period between message transmissions, in milliseconds</param>
/// <param name="buffah">The queue to send messages through</param>
sender::sender(int idnum, int period, TSQueue<string> *buffah)
{
    SenderIDValue = idnum;
    MessageSendPeriod = period;
    delivery_channel = buffah;

}
sender::~sender(void)
{

}

/// <summary>
/// This function sends a message
/// </summary>
/// <param name="UniqueData">This is the message payload</param>
void sender::do_send_message(char *UniqueData)
{
    message MessageToSend;
    string msgstring;
    char MessageBuffer[MAX_MESSAGE_LEN + 1];

    MessageToSend.StuffMessageByComponents(SenderIDValue, UniqueData);
    MessageToSend.ProduceMessage(MessageBuffer, MAX_MESSAGE_LEN + 1);

    msgstring.assign(MessageBuffer);

    delivery_channel->push(msgstring);

}

/// <summary>
/// This function will continuously put out messages at the rate defined by
/// the message send period in milliseconds
/// </summary>
/// <param name=""></param>
void sender::spammer(void)
{
    string msgstring;
    uint16_t spamcount = 0;
    char UniqueData[MAX_MSGDATA_LEN + 1];

    while (MessageSendPeriod != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(MessageSendPeriod));
 
        ///#ifdef DO_MODULE_TEST_ROUTINES
        // This line is test/demonstration code for do_send_message
        snprintf(UniqueData, (MAX_MSGDATA_LEN), "spm0x%4.4x",spamcount);
        spamcount++;
        ///"endif DO_MODULE_TEST_ROUTINES


        //TODO:  Add application specific message payload creation calls here
        
        do_send_message(UniqueData);
    }

}


int main()
{

    TSQueue<string> IncomingMessages;
    receiver main_receiver;
 
    cout << "Starting demo for CodeChallenge5Jun24!\n" << "";

#ifdef DO_MODULE_TEST_ROUTINES
    MessageClassModuleTest();
#endif

    //TODO: set up sender as an aray of MESSAGE_SENDER_COUNT elements and
    // fire off that many senders to spam
    sender sender1 = sender(1, 120,&IncomingMessages);
    sender sender2 = sender(2, 150,&IncomingMessages);
    sender sender3 = sender(3, 180,&IncomingMessages);
    sender sender4 = sender(4, 195,&IncomingMessages);
    std::thread s1(&sender::spammer, &sender1); // s1 runs sender::spammer on object sender1
    std::thread s2(&sender::spammer, &sender2); // s1 runs sender::spammer on object sender2
    std::thread s3(&sender::spammer, &sender3); // s1 runs sender::spammer on object sender3
    std::thread s4(&sender::spammer, &sender4); // s1 runs sender::spammer on object sender3
 
    while (true)
    {
        main_receiver.CheckForAndProcessMessages(&IncomingMessages);
    }

    s1.detach();
    s2.detach();
    s3.detach();
    s4.detach();
    cout << "done";
 
    return 0;

}
