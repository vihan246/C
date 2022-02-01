#include <string>
#include <ctime>
#include <ostream>
#include <cmath>

// Class declaration. 
template <typename T>
class Hashtable
{
    public:
        Hashtable(bool d = false, unsigned int s = 11);
        ~Hashtable();
        int add(std::string k, const T& val);
        void reportAll (std::ostream &) const;
        const T& lookup (std::string k);
        
    private:
        void resize();
        int hash(std::string k) const;
        int ceiling(int num, int den) const;
        bool debug; 
        unsigned int size;
        int elements = 0;
        std::pair<std::string, T>** table;
        int r1, r2, r3, r4, r5;
        int arrSizes[17] = {11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877, 205759, 411527, 823117};
};

//Constructor; depending on the debug flag either sets the r values to predetermined random integers or 
// creates five randoms to use. 
template <typename T>
Hashtable<T>::Hashtable(bool d, unsigned int s)
{
    table = new std::pair<std::string, T>*[s];
    size = s;
    debug = d;
    if (debug)
    {
        r1 = 983132572;
        r2 = 1468777056;
        r3 = 552714139;
        r4 = 984953261;
        r5 = 261934300;
    }
    else
    {
        srand(time(0));
        r1 = rand() % size;
        r2 = rand() % size;
        r3 = rand() % size;
        r4 = rand() % size;
        r5 = rand() % size;
    }
}

// Destructor deletes all dynamically allocated memory.
template <typename T>
Hashtable<T>::~Hashtable()
{
    for (int i = 0 ; i< size; i++)
    {
        delete table[i];
    }
    delete[] table;
}

// Add function. 
template<typename T>
int Hashtable<T>::add(std::string k, const T& val)
{
    // base integers; probes is incremented each time a probe occurs in the later for loop
    int probes = 0; 
    int hashval = hash(k);
    // checks the base case of when the original hash value is free in the array
    if (table[hashval] == nullptr)
    {
        table[hashval] = new std::pair<std::string, T>(k, val);
        elements++;
    }
    else
    {
        // checks if load factor is over .5 and resizes if so
        if (((double)elements)/((double)size) >= .5)
        {
            resize();
        }
        bool loop = true; 
        int i = 1;
        // while loop will continue to do quadratic probing until a spot is found for the key
        while (loop)
        {
            int newHashval = (hashval + (int)pow(i, 2))%size;
            if (table[newHashval] == nullptr)
            {
                table[newHashval] = new std::pair<std::string, T>(k, val);
                loop = false;
                elements++;
            }
            probes++;
            i++;
        }
    }
    return probes;
}

// function for the hash values
template<typename T>
int Hashtable<T>::hash(std::string k) const
{
    // hashValues array contains the w-values that will be used later
    int hashValues[5] = {0};
    // for loop runs only the amount of times that it needs to. Uses ceiling function to determine how many times it needs
    // to run. 
    for (int i = 0; i < ceiling(k.length(), 6); i++)
    {
        int start = (k.length() - 1) - (i*6);
        int conversion = 0;
        for (int j = 0; j < 6; j++)
        {
            if ((j + (i*6)) >= k.length())
            {
                conversion += 0;
            }
            else
            {
                int c = (int)k[start - j];
                c -= 96;
                conversion += (c*pow(27, j));
            }
        }
        hashValues[4-i] = conversion;
    }

    int preOut = (((long long)r1 * (long long)hashValues[0]) + ((long long)r2*(long long)hashValues[1]) + ((long long)r3*(long long)hashValues[2]) 
    + ((long long)r4*(long long)hashValues[3]) + ((long long)r5*(long long)hashValues[4])) % size;
    
    return preOut;
}

// ceiling function for int division. 
template<typename T>
int Hashtable<T>::ceiling(int num, int den) const
{
    return ((num/den) + (num % den != 0));
}

// resize function is called every time load factor is above .5 when adding. 
template<typename T>
void Hashtable<T>::resize()
{
    int i =0 ;
    // while loop determines what the size of the new resized array should be. 
    while (arrSizes[i] < size)
    {
        i++;
    }
    std::pair<std::string, T>** newTable = new std::pair<std::string, T>*[arrSizes[i]];
    int oldsize = size; 
    size = arrSizes[i];
    for (int j = 0; j < oldsize; j++)
    {
        if (table[j] != nullptr)
        {
            int hashval = hash(table[j]->first);
            if (newTable[hashval] == nullptr)
            {
                newTable[hashval] = new std::pair<std::string, T>(table[j]->first, table[j]->second);
                elements++;
            }
            else
            {
                bool loop = true; 
                int f = 1;
                while (loop)
                {
                    int newHashval = (hashval + (int)pow(f, 2))%size;
                    if (newTable[newHashval] == nullptr)
                    {
                        newTable[newHashval] = new std::pair<std::string, T>(table[j]->first, table[j]->second);
                        loop = false;
                    }
                    f++;
                }
            }
        }
    }
    delete[] table; 
    table = newTable;
}
// iterates through the hashtable and prints out all of the keys and value pairs. 
template<typename T>
void Hashtable<T>::reportAll (std::ostream & out) const
{
    for (int i = 0; i < size; i++)
    {
        if (table[i] != nullptr)
        {
            out << table[i]->first << " " << table[i]->second << std::endl;
        }
    }
}

// lookup function returns the value of a key that has been looked up in the array. If a key is not found, 
// returns value of first T value in hashtable.  
template<typename T>
const T& Hashtable<T>::lookup (std::string k)
{
    int hashval = hash(k);
    if (table[hashval]->first == k)
    {
        return table[hashval]->second;
    }
    else
    {
        bool loop = true; 
        int i = 1;
        while (loop)
        {
            int newHashval = (hashval + (int)pow(i, 2))%size;
            if (table[newHashval] == nullptr)
            {
                for (int j = 0; j < size; j++)
                {
                    if (table[j] != nullptr)
                    {
                        return table[j] -> second;
                    }
                }
            }
            else if (table[newHashval]->first == k)
            {
                return table[newHashval]->second;
            }
            i++;
        }
    }
}