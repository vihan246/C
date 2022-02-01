After reading in the words from the text file, I proceed to run my a* search function. What it does is add 
the starting word to a MinHeap, and then proceed to change the character in each position of the string in 
an effort to find the words one step away. While this is happening, I check if that new word is in the set 
of words. If it is, I then calculate the priority value using the amount of steps already taken, and a 
provided heuristic. Once this is done I add the newly constructed word to the heap, breaking ties as I ensure
it reaches the correct position within the heap. As I keep popping the top node, eventually either a path is found or the heap ends up being empty. I used a boolean to determine whether the while loop continued so that I could break out of it easily once I had found a path.