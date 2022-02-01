#include "MinHeap.h"
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>

// Function that converts all of the characters in a string to lowercase
// so that the cases of all the words can be uniform. 
std::string toLower(std::string input)
{
    std::string output = input;
    for (int i = 0; i < output.length(); i++)
    {
        char temp = output[i];
        if ((temp >= 'A') && (temp <= 'Z'))
        {
            temp += 32;
            output[i] = char(temp);
        }
    }
    return output;
}

// function that calculates the heuristic values. Compares
// the characters at each index of two strings and increments an int
// if they are different. Returns this int. 
int findDistance(std::string first, std::string second)
{
    int dist = 0;
    for (int i = 0; i < first.length(); i++)
    {
        if (first[i] != second[i])
        {
            dist++;
        }
    }   
    return dist;
}

// Function that actually performs the A* search. It takes in a reference to 
// the expansions int, the steps int, and the set containing all of the words. 
// These are pass by reference because for the first two I wanted to be able to
// edit them inside the function. The set is pass by reference as there would
// be no need to edit it or make a copy. The function also takes in the starting 
// string and the ending string. 
void aSearch(int& expansions, int& steps, std::set<std::string>& wordList, std::string start, std::string end)
{
    // map that keeps track of all the g values for each string added to the 
    // minHeap.
    std::map<std::string, int> gDist;
    MinHeap<std::string> minheap(2);
    // solution will either be 0 or the amount of steps needed to reach end. 
    int solution=0; 
    // g value for the starting string is 0. 
    gDist[start] = 0;
    minheap.add(start, 0);
    int g = 0;
    // variable that determines whether the loop will continue or not. Needed this 
    // because I didn't use an updatePriority function, and as such can't just wait
    // for the heap to be empty. 
    bool loop = true;
    while (loop)
    {
        // expansions is incremented every time a word is popped from the heap.
        std::string current = minheap.peek();
        minheap.remove();
        expansions++;
        g = gDist[current];
        // for loops change each letter of the word.
        for (int i = 0; i < current.length(); i++)
        {
            std::string temp = current;
            for (int j = 97; j < 123; j++)
            {
                temp[i] = char(j);
                // checks if the new word is in the wordlist.
                if (wordList.find(temp) != wordList.end())
                {
                    // if the new word is equal to the end word, we have found our path
                    // and need to exit the search. Solution = g+1 because the path would 
                    // still have to go through one more edge to reach the end. 
                    if (temp == end)
                    {
                        solution = g+1;
                        loop = false;
                        break;
                    }
                    // if statement checks if a node has been visited prior. 
                    if (gDist.find(temp) == gDist.end())
                    {
                        gDist[temp] = g+1;
                        minheap.add(temp, ((g+1)+findDistance(temp, end))*(temp.length()+1) + findDistance(temp, end));
                    }
                    // elif statement activates if the node has been visited, but the new 
                    // g value is less that the one seen previously. Tie breaking is taken care of
                    // inside the MinHeap.
                    else if (g+1 < gDist[temp])
                    {
                        gDist[temp] = g+1;
                        minheap.add(temp, ((g+1)+findDistance(temp, end))*(temp.length()+1) + findDistance(temp, end));
                    }
                }
            }
            if (!loop)
            {
                break;
            }
        }
        // if statements break depending on conditions. 
        if (!loop)
        {
            break;
        }
        else if (minheap.isEmpty())
        {
            loop = false; 
            break;
        }
    }
    // outside variable steps is set equal to either 0 or the answer to the problem.
    steps = solution;
}

int main(int argc, char* argv[])
{
    // start word is the second argument, end is the third, and file name is the fourth. 
    std::string start = argv[1];
    std::string end = argv[2];
    std::ifstream inputStream(argv[3]);

    // int reads in the first line of the .txt file, determining the amount of words in 
    // the dictionary. 
    int numWords = 0;
    inputStream >> numWords;
    
    //set that holds the words is initialized here. For loop then runs and reads in the 
    // words from .txt file. 
    std::set<std::string> words;
    for (int i =0; i< numWords; i++)
    {
        std::string word;
        inputStream >> word; 
        std::string lower = toLower(word);
        words.insert(lower);
    }
    // start and end are converted to lowercase.
    start = toLower(start);
    end = toLower(end);
    // output variables
    int steps = 0;
    int expansions = 0;
    // a* search
    aSearch(expansions, steps, words, start, end);

    // if the amount of steps is zero it means that we have been unable to find a path to
    // the end string. 
    if (steps != 0)
    {
        std::cout<<steps<<std::endl;
    }
    else
    {
        std::cout<<"No transformation" << std::endl;
    }
    std::cout<<expansions<<std::endl;
    return 0;
}