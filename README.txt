a. King Lun Au
b. 6143-8340-29
c. In this assignment, the server will first read the content from the input file,
   it will then store and display the neighbor's information with coressponding values.
   Then, a TCP connection will be established to connect to the client. The information
   will send to the client, once the client have received the neigbor's information
   from all four servers, it will transfer a data table that contain all server's information
   to each server. The output will be displayed in both c file. Finally, a tree calculation 
   will be performed and displayed on the client side.

d. client.c: 
	     printMinCostEdges: Print the edges of generated spanning tree.

             prim: Generate minimum spanning tree.

             TCP_cli_socket: Set up and wait to receive data from the server side.
                             After that, gather all the neighbor's information and
                             save.

	     UDP_cli_socket: Send all the neighbor's information to the server side.

   server.c: 
             TCP_serv_socket: Connect to the client, and send its neighbors information
                             to the client.
             UDP_serv_socket: Set up and wait to receive the data from the client 

	     read_store: Read information from the file, and save.
             
   fork.c:   
	     fork_childs_send: Create four children to simulate four server connected with client under TCP 
	     protocal. Wait till they all die and then create another four children to 
	     perform servers action.

	     fork_childs_recv: Create four children to receive information from client under UDP protocal.

	     main: Use fork_childs_recv and fork_childs_send function to receive information from client under UDP protocal.


   makefile: make compilelation easier by 
             1.set "make client" equivalent to "gcc-o client client.c -lsocket -lnsl -lresolv"
   	     2.set "make server" equivalent to "gcc-o server server.c -lsocket -lnsl -lresolv"

e. 

- open X-Win32
- connect to aludra
- login to nunki, input the following and then enter the password
  ssh -Y kingluna@nunki.usc.edu  // whatever a student's email

// input the following by order in the command line:
- cd socket // go to the current directory of "socket"
- make clean  // clear everything
- make client // equivalent to "gcc-o client client.c -lsocket -lnsl -lresolv", use to compile
- make server // equivalent to "gcc-o server server.c -lsocket -lnsl -lresolv", use to compile
- ./client // Run the client.c

- open another X-Win32:
- connect to aludra
- login to nunki, input the following and then enter the password
  ssh -Y kingluna@nunki.usc.edu // whatever a student's email

// input the following by order in the command line:
- cd socket // go to the current directory of "socket"
- ./server // Run the server.c

f. 
   - Server use char buffer of a size of 256 with TCP 
   - Client transfer the int array to the server with UDP
   
g.
   No idiosyncrasy


h. I took a look with the following websites to get general ideas, and wrote the codes by 
   myself, so there are no specific range of lines are from the source code. 
    
   BinaryTides, http://www.binarytides.com/socket-programming-c-linux-tutorial/
   Code Project, http://www.codeproject.com/Articles/586000/Networking-and-Socket-programming-tutorial-in-C
   Tenouk.com, http://www.tenouk.com/Module40c.html





