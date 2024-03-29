# You can see the exersice in pdf

In my shared memory i store the request segment from child, an array with lines of
desired segments and the finished children.

I have parent proccesor who create N childs using fork, parent code wait until a 
child request a line and then he upload it to shared memory until all children finished
their requests.

For child code: The first request became by asking random segment and line from parent,
and for all others with 0.7 propability asking the same segment, 0.3 for random and every
time random line. Child core runs until the child finished all of his requests. If
the child is the first child who ask for the x segment, then it asks from father to
upload this segment and all childs who asks for this segment reading the line of this 
segment. When no one child exists to read the segment another child make a request to 
parent and gets to share memory this segment. Loop until child finished all requests.

I using request_semaphore. I intialized to 1 because i want the fist child who arrive to 
this semaphore to ingore him and to make the first request. Then all other children who
arrive firsts whith different segments wait behind this semaphore in a queue (FIFO). Until 
children finished whith segment in shared memory. I post him when it happens.

I using an array with segment_semaphoreses one per segment. All children who have same 
segments with others but they didn't came before ohters, wait in this semaphore
until the previous child of this segment post him.

I use 2 semaphores child_ready and parent_answer for communication bettween parent and child
for upload a segment

run make compile for compile files
and make run for run programm with arguements text.txt 100 3 5
You can change arguements in MakeFile for different executions
