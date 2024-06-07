
 This is the "challenge"  presented:
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
