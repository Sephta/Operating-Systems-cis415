# InstaQuack (fake University of Oregon Social Platform)

In this project we designed a system of interacting with stored data asynchronously using the pub/sub model. 
The data being passed around is a series of structs emulating “entries” that contain photos, captions, and meta data. 
To store these structs we implemented a circular ring buffer (well, multiple buffers) to enqueue and dequeue entries 
whenever a publisher or subscriberhasaccess to them. To emulate the pub/sub behavior we used POSIX threads, or pthreads 
that are very useful for designing and implementing asynchronous programs.These threads duringruntime would be created 
and fed commands from the main InstaQuack server (the main program thread). For my implementationI set up my main thread 
to accept a filename as an argumentand to then read commands  from  a  main  file.  These  commands  would  describe 
behaviors  such  as  creating  pubs/subs, creating Topic-Entry buffers, and then initializing and starting the rest of 
the system. Each pthread created to run a publisher or subscriber  would  be  fed its  own  file  containing  commands.
These  threads  would run  all  at  once and asynchronously interact with the topic buffers active during runtime.The commands 
fed to them would emulate behavior of putting new entries into the Topic buffers, and fetching entries to print out to an 
external HTML file. For my implementation I would generate a unique HTML file for every subscriber, and each subscriber would
print the entries it came across duringruntime to the HTML file in a clean and readable format.
