
   ***Build and Development Notes***

   This code was built using Visual Studio 2022 (community edition) 
   using its cross-platform development feature(s).  It was then pushed
   to a github public repository for sharing. This choice was made
   because of its ease in bring-up on another (likely) windows based PC.  


   Cloning this repository at the Visual Studio start screen *should*
   (or has in other instances) bring up the environment to the point
   where it will, once connected to a linux system, build this program
   and remotely execute using the remote functionality of the gdb
   debugger interface.
   
   The target system needs the following packages installed to ensure
   remote debugging. 
   apt-get install openssh-server build-essential gdb rsync make zip

   apt-get install libjsoncpp-dev
   
   After this is set, go to the tools pull down menu, select options
   go to the "cross platform" category and add a connection to the
   remote system, including a username and password.
   Microsoft has several tutorial pages including this one at:
   https://learn.microsoft.com/en-us/cpp/build/get-started-linux-cmake?view=msvc-170
   
   And this is in addition to several short youtube videos outlining
   how to operate this.



 This is the text of the "challenge"  that was presented:

 Design and implement a program in which N (where N >= 3) senders sends 
 messages at arbitrary rate to a receiver and receiver processes messages 
 received from the senders. A message consists of sender’s id and an 
 arbitrary data of not more than 10 bytes. Initially, each sender was given 
 a unique id (type int), so when a sender sending its message it prepends 
 its id to data. When the receiver receives messages from senders, the 
 receiver should process message from sender with lower id before 
 processing message from sender with higher id. For this challenge,  
 the processing method can print message.

 Example: three senders:
  sender_1 was assigned its id as 1
  sender_2 was assigned its id as 2
  sender_3 was assigned its id as 3
 sender_2 sends following message:
  2absd
 sender_1 sends following message:
  1tuyuw6
 sender_3 sends following message:
  3qwet
 When receiver receives above three messages it should print messages as follows:
 1tuyuw6, 2absd, 3qwet

  Discuss possible solutions and implement only one solution in C++. Structure
  your code using object-oriented principles to allow for future maintenance
  of the sender and receiver, such as adding new features. Include source-code
  comments as you would when submittng a change to a large existing C++
  project. Assume runtimeOS is Linux. Please provide instructions for
  compilation and running your program.

********* End of presented text and start of task breakdown *********************

Regarding possible solutions:
Single thread option:
The senders and receiver operate in an synchronous loop involving a single thread.
Senders would be called in turn synchrously to the receiver procesing application,
where the sender would read a time of day signal and queue a message after a 
proscribed time has elapsed.  This option has the simplicity of avoiding any race
conditions involved with putting the senders into individual threads.  It is also
best suited in an optimized embedded application where the overhead involved with
multi-tasking operating systems can create a significant burden.

Multiple thread option:
The sender(s) operate in their own threads and independently queue their messages
for processing asynchronously to each other and the receiver.  In this option,
the queueing requires protections against race conditions where multiple messages
could wind up being queued (or de-queued) at exactly the same time.  For this
option a thread-safe queue object (using mutexes) needs to be employed. This
solution does add overhead and complexity, but given the target environment
being linux, the added overhead (beyond what linux has already added) is minimial
Secondarily, this challenge loosely mimicks applications where multiple external
data sources are sent to a central processing application.  

The biggest example of this might very well be the webserver, where every 
querying host causes the IP driver, on establishing the TCP link will start
an independent process to send data to the web server daemon for processing.  
Working with multiple threads at this point also provides better
scalability into that type of model where the senders could likely be
independent applications.

One part of this challenge that I'm seeing that will have to be addressed
through design for maintainability involves the message format.  It was 
made vague enough that the possibility of the sender's ID number, which is 
meant to be a sorting key could be blurred by the data. There are no 
constraints on how the ID number and data is formatted. There is nothing in 
the specification specifying whether or not the number is in ascii coded 
decimal, hexadecimal, or even binary, nor is it a fixed length. The only 
implication to the former are the examples, but there's no real constraint 
specified.  
For example: 
A message coming from an a sender with the ID of 15, and an arbitrary 
data field like "2for", would be indistinguishable from a message coming
from sender with an ID number of 152 and a data field of "for".  

The first solution seen to this issue is the encapsulating of the message 
formatting and parsing an single object to be used by both the senders and 
processor.  This not only facilitates substitution in the event the message 
format changes, but also independent testing of its functionality. 


This puts the solution into four primary objects:
They are the sender, the receiver, the messages, thread safe queue.





   Now onto the pieces (the primary objects of this challenge):

1: The sender, given an ID number will periodically populate a buffer with a 
   message and send it to the receiver object. Rather than force the sender
   
   The requirement for multiple 
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

