#include "bigint.h"
#include <iostream>
#include <string>
#include <climits>
#include <cmath>
using namespace std;

//******************************************************************
//START OF THE CONSTRUCTORS SECTION
//******************************************************************

/*
// Create a default BigInt with base 10.
// This function is complete. You do not have to add anything.
*/
BigInt::BigInt(){
  base = 10;
  isPositive = true;
}

/*
// Create a BigInt with a specified base.
// This function is complete. You do not have to add anything.
*/
BigInt::BigInt(int setbase){
    if(setbase < 2 || setbase > 36){
        throw InvalidBaseException();
    }
    base = setbase;
    isPositive = true;
}

/*
// Destructure. You do not have to add anything.
// This function is complete
*/
BigInt::~BigInt(){}

/*
//  Create a BigInt from int (base 10).
//  - Convert the int (base 10) input into a specified base (setbase).
//  - Don't forget to set the base accordingly
//  For example:
//     BigInt k(6,2) <- int 6 base 10 to BigInt base 2
//          - Result:  BigInt k with value 110 (base 2)
//     BigInt k(154,16) <- 154 base 10 to BigInt base 16
//          - Result: BigInt k with value 9A (base 16)
*/
BigInt::BigInt(int input,int setbase){
    if(setbase < 2 || setbase > 36){
        throw InvalidBaseException();
    }
    /************* You complete *************/
  base = setbase;
  int t = input;
  if (t < 0)
  {
    t = t * (-1);
    isPositive = false;
    while (t != 0)
    {
      int dig = t%setbase;
      vec.push_back(dig);
      t = t/setbase;
    }  
  }
  else if (t==0)
  {
    isPositive = true;
    vec.push_back(0);
  }
  else
  {
    isPositive = true;
    while (t != 0)
    {
      int dig = t%setbase;
      vec.push_back(dig);
      t = t/setbase;
    }
  }

}

/*
//  Create a BigInt from string.
//  Don't forget to set the base accordingly
//  The input strings will contain only
//      - numbers (0-9) and/or 
//      - uppercase letters (A-Z) [A=10, B=11, ... , Z=36]
//  Note: char '0' = int 48 (see ASCII table)
//  Note: char 'A' = int 65
*/
BigInt::BigInt(const string &s, int setbase){
    if(setbase < 2 || setbase > 36){
        throw InvalidBaseException();
    }
    /************* You complete *************/
  base = setbase; 
   unsigned int i = 0;
  if (s.substr(0,1)== "-")
  {
    isPositive = false;
    string newS = s.substr(1, (s.length()-1));
    for (i=0; i< newS.length(); i++)
    {
      char digTemp = newS.at((newS.length()-1)-i);
      int digFin = (int)digTemp;
      if (digFin > 64)
      {
        digFin -= 55;
      }
      else 
      {
        digFin -= 48;
      }
      vec.push_back(digFin);
    }
  }
  else 
  {
    isPositive = true;
    for (i=0; i< s.length(); i++)
    {
      char digTemp = s.at((s.length()-1)-i);
      int digFin = (int)digTemp;
      if (digFin > 64)
      {
        digFin -= 55;
      }
      else 
      {
        digFin -= 48;
      }
      vec.push_back(digFin);
    }
  }  
}

/*
//  Create a BigInt from another BigInt.
//  This is a copy constructor.
//  Simply call the assignment operator (=) below.
*/
BigInt::BigInt(const BigInt &b){

    /************* You complete *************/

  *this = b;


}

/*
//  Assignment operator. (i.e., BigInt a = b;)
//  Overriding a's data with BigInt b's data
//       - vec
//       - isPositive
//       - base
*/
BigInt & BigInt::operator = (const BigInt &b){

    /************* You complete *************/

  isPositive = b.isPositive;
  base = b.base;
  vec.clear();
  for (unsigned int i=0; i< b.vec.size(); i++)
  {
    vec.push_back(b.vec[i]);
  }
    return *this;
}


//******************************************************************
//END OF THE CONSTRUCTORS SECTION
//******************************************************************

//******************************************************************
//START OF THE DISPLAYS SECTION
//******************************************************************

/*
// Convert to string and return the string
// Display it in its corresponding base
//  Note: char '0' = int 48 (see the ASCII table)
//  Note: char 'A' = int 65
*/
string BigInt::to_string(){

    /************* You complete *************/

  string output;
  if (!(isPositive))
  {
    output += "-";
  }
  for (unsigned int i = 0; i < vec.size(); i++)
  {
    if (vec[(vec.size()-1)-i] < 10)
    {
      output += (char)(vec[(vec.size()-1)-i]+48);
    }
    else
    {
      output += (char)(vec[(vec.size()-1)-i]+55);
    }
  }

  return output;//for now
}

/*
//  Convert BigInt to integer base 10 and return that int
//    If BigInt >= INT_MAX, return INT_MAX.
//    If BigInt =< INT_MIN, return INT_MIN.
//  Note: 1. INT_MAX and INT_MIN are already defined for you
//           in the 'climits' library.
//        2. INT_MAX = 2147483647, INT_MIN = -2147483648
//           therefore, INT_MIN does not equal to (-INT_MAX)
*/
int BigInt::to_int() const{

    /************* You complete *************/
  long long int max = 2147483647;
  long long int min = -2147483648;
  long long int output = 0;
  for (unsigned int i = 0; i < vec.size(); i++)
  {
    if (isPositive)
    {
      output += vec[i] * pow(base, i);
      if (output >= max)
      {
        return INT_MAX;
      }
    }
    else
    {
      output -= vec[i] * pow(base, i);
      if (output <= min)
      {
        return INT_MIN;
      }
    }
  }
  return output;//for now
}

//******************************************************************
//END OF THE DISPLAYS SECTION
//******************************************************************

//******************************************************************
//START OF THE EQUALITY CHECK OPERATORS SECTION
//******************************************************************

/*
//  Compare a and b.
//     If a = b, return 0.
//     If a < b, return -1.
//     If a > b, return 1.
//  See the HINT below
*/
int BigInt::compare(const BigInt &b) const{
    if(base != b.base){
        throw DiffBaseException();
    }
    /************* You complete *************/

    //HINT:
    //1. do NOT call the to_int() or to_string() function
    //2. do NOT convert bigInt to other datatypes first (e.g., long long, int)
    
    //First let's compare the signs
    //    - if the signs aren't the same,
    //              how do you know which one is larger?

    //If they have the same sign, let's compare their vec sizes 
    //    - what if both are positive, but a's vec is larger (a or b is larger?)
    //    - other cases?
    
    //If they have the same vec size, let's compare their digits one by one
    // Starting from the most-significant digits. --> in what vector position?
    // Remember that the BigInt vectors are in reverse order!
    //    - what if both are positive, but the digit of a is greater than that of b (a or b is larger?)
    //    - other cases?
    
    //If ALL digits are the same, then they MUST be equal!!
     //if a is positive and b isn't
    if ((isPositive) && (!b.isPositive))
    {
      return 1;
    }
    if ((!isPositive) && (b.isPositive))
    {
      return -1;
    }
    
    if ((!isPositive) && (!b.isPositive))
    {
      if (vec.size() > b.vec.size())
      {
        return -1;
      }
      if (vec.size() < b.vec.size())
      {
        return 1;
      }
     
      for (unsigned int i = 0; i < vec.size(); i++)
      {
        if (vec[vec.size()-1-i] > b.vec[vec.size()-1-i])
        {
          return -1;
        }
        if (vec[vec.size()-1-i] < b.vec[vec.size()-1-i]) 
        {
          return 1;
        }
      }
      
    }
    else
    {
      if (vec.size() > b.vec.size())
      {
        return 1;
      }
      else if (vec.size() < b.vec.size())
      {
        return -1;
      }
      else
      {
        for (unsigned int i = 0; i < vec.size(); i++)
        {
          if (vec[vec.size()-1-i] > b.vec[vec.size()-1-i])
          {
            return 1;
          }
          if (vec[vec.size()-1-i] < b.vec[vec.size()-1-i]) 
          {
            return -1;
          }
        }
      }
    }
    return 0;
}


//Call the compare function above to check if a == b
bool operator == (const BigInt &a, const BigInt &b){
  
    /************* You complete *************/

  if (a.compare(b) == 0)
  {
    return true;
  }
  
  return false;//for now
}

//Call the compare function above to check if a != b
bool operator != (const BigInt &a, const BigInt &b){
  
    /************* You complete *************/

  if (a.compare(b) != 0)
  {
    return true;
  }
  
  
  
  return false;//for now
}

//Call the compare function above to check if a <= b
bool operator <= (const BigInt &a, const BigInt &b){
  
    /************* You complete *************/

  if (a.compare(b) <= 0)
  {
    return true;
  }
  
  
  
  return false;//for now
}

//Call the compare function above to check if a >= b
bool operator >= (const BigInt &a, const BigInt &b){
  
    /************* You complete *************/

  if (a.compare(b) >= 0)
  {
    return true;
  }
  
  
  
  return false;//for now
}

//Call the compare function above to check if a > b
bool operator > (const BigInt &a, const BigInt &b){
  
    /************* You complete *************/

  if (a.compare(b) == 1)
  {
    return true;
  }
  
  
  
  return false;//for now
}

//Call the compare function above to check if a < b
bool operator < (const BigInt &a, const BigInt &b){
  
    /************* You complete *************/

  if (a.compare(b) == -1)
  {
    return true;
  }
  
  
  return false;//for now
}

//******************************************************************
//END OF THE EQUALITY CHECK OPERATORS SECTION
//******************************************************************

//******************************************************************
//START OF THE ARITHMETIC OPERATORS SECTION
//******************************************************************

/*
//======================
// NON-MEMBER function
//======================
// 1. Create a copy of a
// 2. Call += operator on the copy and b
// 3. Return the copy
// Note: Should take you exactly 3 lines of code
*/
BigInt operator + (const  BigInt &a, const BigInt & b){

  /************* You complete *************/
  
  BigInt copy(a);
  copy += b;
  return copy;//for now
}

/*
//==================
// MEMBER function
//==================
//  Addition assignment operator.
//    i.e., a += b
*/
const BigInt & BigInt::operator += (const BigInt &b){
    if(base != b.base){
        throw DiffBaseException();
    }
    /************* You complete *************/
  
  if (isPositive != b.isPositive)
  {
    if (isPositive)
    {
      BigInt copy(b);
      copy.isPositive = true;
      *this -= copy; 
    }
    else
    {
      isPositive = !isPositive;
      *this -= b; 
      if ((this -> vec.size()==1)&&(this -> vec[0]==0))
      {
        isPositive = true;
      }
      else
      {
        isPositive = !isPositive;
      }
    }
  }
  else
  {
    size_t end = max(vec.size(), b.vec.size());
    int carry = 0;
    int digA = 0;
    int digB = 0;
    
    for (size_t i = 0; i< end; i++)
    {
      if (i < vec.size())
      {
        digA = vec[i];
      }
      else
      {
        digA = 0;
      }
      if (i < b.vec.size())
      {
        digB = b.vec[i];
      }
      else
      {
        digB=0;
      }
      int sum = digA + digB + carry; 
      if (sum >= base)
      {
        carry = sum/base; 
      }
      else 
      {
        carry =0;
      }
      if (i<vec.size())
      {
        vec[i] = sum%base;
      }
      else
      {
        vec.push_back(sum%base);
      }
    }
    
    if (carry)
    {
      vec.push_back(carry);
    }
    
  }
   
  return *this;
}

/*
//======================
// NON-MEMBER function
//======================
// 1. Create a copy of a
// 2. Call -= operator on the copy and b
// 3. Return the copy
// Note: Should take you exactly 3 lines of code
*/
BigInt operator - (const  BigInt &a, const BigInt & b){

  /************* You complete *************/
  BigInt copy(a);
  copy -= b;
  return copy;//for now
}


/*
//==================
// MEMBER function
//==================
//  Subtraction assignment operator.
//    i.e., a -= b
//  Note: Think of base cases (e.g., subtract by itself)
*/
const BigInt & BigInt::operator -= (const BigInt &b){
    if(base != b.base){
        throw DiffBaseException();
    }
    /************* You complete *************/
  
  if (isPositive != b.isPositive)
  {
    if (isPositive)
    {
      BigInt copy(b);
      copy.isPositive = true;
      *this += copy;
    }
    else
    {
      isPositive = true;
      *this += b;
      isPositive = false;
    }
  }
  else if (*this == b)
  {
    vec.clear();
    vec.push_back(0);
    isPositive = true;
  }
  else
  {
    size_t end = max(vec.size(), b.vec.size());
    int carry = 0;
    int digA = 0; 
    int digB = 0;
    BigInt copyA = *this;
    for (size_t i = 0; i < end; i++)
    {
      if (i< vec.size())
      {
        digA = vec[i];
      }
      else 
      {
        digA = 0;
      }
      if (i < b.vec.size())
      {
        digB = b.vec[i];
      }
      else
      {
        digB = 0; 
      }
      
      int dif; 
      if (isPositive)
      {
        if (*this > b)
        {
          dif = digA - digB - carry;
        }
        else
        {
          dif = digB - digA - carry;
        }
      }
      else
      {
        if(*this > b)
        {
          dif = digB - digA - carry;
        }
        else
        {
          dif = digA - digB - carry;
        }
      }
      
      if (dif < 0)
      {
        dif+= base;
        carry = 1;
      }
      else
      {
        carry = 0; 
      }
      
      if (i < vec.size())
      {
        vec[i] = dif;
      }
      else
      {
        vec.push_back(dif);
      }
    }
    if (copyA > b)
    {
      isPositive = true;
    }
    else
    {
      isPositive = false;
    }
    
    for (int i = vec.size()-1;i>0;i--)
    {
      if (vec[i] == 0)
      {
        vec.pop_back();
      }
      else
      {
        break;
      }
    }
  }
 
  return *this;
}

/*
//======================
// NON-MEMBER function
//======================
// 1. Create a copy of a
// 2. Call *= operator on the copy and b
// 3. Return the copy
// Note: Should take you exactly 3 lines of code
*/
BigInt operator * (const  BigInt &a, const BigInt & b){

    /************* You complete *************/

  BigInt copyA(a);
  copyA *= b;
  return copyA;//for now
}



/*
//==================
// MEMBER function
//==================
//  Multiplication assignment operator.
//    i.e., a *= b
//  Implement Long Multiplication
//  Note: What are the base cases?? (e.g., multiply by 0)
*/
const BigInt & BigInt::operator *= (const BigInt &b){
    if(base != b.base){
        throw DiffBaseException();
    }
    /************* You complete *************/
    if (((vec.size() == 1) && (vec[0] == 0)) || ((b.vec.size()==1)&&(b.vec[0]==0)))
    {
      vec.clear();
      vec.push_back(0);
      isPositive = true;
      return *this;
    }
    else if ((vec.size() == 1) && (vec[0] == 1))
    {
      BigInt copyB(b);
      if (isPositive != b.isPositive)
      {
        copyB.isPositive = false;
      }
      else
      {
        copyB.isPositive = true;
      }
      *this = copyB;
      return *this;
    }
    else if ((b.vec.size() == 1) && (b.vec[0] == 1))
    {
      if (isPositive != b.isPositive)
      {
        isPositive = false;
      }
      else
      {
        isPositive = true;
      }
      
      return *this;
    }
    else if ((vec.size() == 1) && (vec[0] == 1) && (!isPositive))
    {
      BigInt copy(b);
      copy.isPositive = !copy.isPositive;
      *this = copy;
      return *this;
    }
    else if ((b.vec.size() == 1) && (b.vec[0] == 1) && (!b.isPositive))
    {
      isPositive = !isPositive;
      return *this;
    }
    else
    {
      BigInt total(0, base);
      int carry = 0;
      for (size_t i = 0; i < b.vec.size(); i++)
      {
        BigInt temp(base);
        for (size_t z = 0; z < i; z++)
        {
          temp.vec.push_back(0);
        }
        for (size_t j = 0; j < vec.size(); j++)
        {
          int product = (vec[j] * b.vec[i]) + carry;
          
          if (product >= base)
          {
            carry = product/base;
          }
          else 
          {
            carry = 0;
          }
          temp.vec.push_back(product%base);
        }
        
        if (carry)
        {
          temp.vec.push_back(carry);
        }
        carry = 0;
        
        total += temp; 
      }
      
      if (isPositive != b.isPositive)
      {
        total.isPositive = false;
      }
      else
      {
        total.isPositive = true;
      }
      *this = total; 
      
      
      
      for (int i = vec.size()-1;i>0;i--)
      {
        if (vec[i] == 0)
        {
          vec.pop_back();
        }
        else
        {
          break;
        }
      }
    
    }
    return *this;
}

/*
//======================
// NON-MEMBER function
//======================
// 1. Create a copy of a
// 2. Call /= operator on the copy and b
// 3. Return the copy
// Note: Should take you exactly 3 lines of code
*/
BigInt operator / (const  BigInt &a, const BigInt & b){

    /************* You complete *************/
    BigInt copyA(a);
    copyA /= b;
    


    return copyA;//for now
}


/*
//==================
// MEMBER function
//==================
//  Division assignment operator.
//    - Call 'divisionMain' to get the quotient;
*/
const BigInt & BigInt::operator /= (const BigInt &b){
    if(base != b.base){
        throw DiffBaseException();
    }
    if(b.vec.size() == 1 && b.vec[0] == 0){
        throw DivByZeroException();
    }
    /************* You complete *************/

  BigInt quotient(base);
  BigInt remainder(base);
  this -> divisionMain(b, quotient, remainder);
  *this = quotient;
    return *this;
}

/*
//======================
// NON-MEMBER function
//======================
// 1. Create a copy of a
// 2. Call %= operator on the copy and b
// 3. Return the copy
// Note: Should take you exactly 3 lines of code
*/
BigInt operator % (const  BigInt &a, const BigInt & b){

    /************* You complete *************/
  
  
  BigInt copyA(a);
    copyA %= b;
    
    return copyA;//for now
}


/*
//==================
// MEMBER function
//==================
//  Modulus assignment operator.
//    - Call 'divisionMain' to get the remainder;
//    - Note: remainder takes the sign of the dividend.
*/
const BigInt & BigInt::operator %= (const BigInt &b){
    if(base != b.base){
        throw DiffBaseException();
    }
    if(b.vec.size() == 1 && b.vec[0] == 0){
        throw DivByZeroException();//divide by zero.
    }
    /************* You complete *************/

  BigInt quotient(base);
  BigInt remainder(base);
  this -> divisionMain(b, quotient, remainder);
  *this = remainder;

    return *this;
}

/*
//  Main function for the Division (/=) and Modulus (%=) operators.
//     - Compute (q)uotient and (r)emainder
//     - Implement Long Division
//  Note: 1. This should behave like integer division
//        2. What are the base cases?? (e.g., div by itself)
*/
void BigInt::divisionMain(const BigInt &b, BigInt &quotient, BigInt &remainder){

    /************* You complete *************/

  bool outSign = (isPositive == b.isPositive);
  BigInt zero(0, base);
  BigInt one(1, base);
  BigInt negOne(-1, base);
  
  if (*this==zero)
  {
    quotient = zero;
    remainder = zero;
  }
  else if (*this == b)
  {
    quotient = one;
    remainder = zero;
  }
  else if ((b == one)||(b==negOne))
  {
    quotient = *this;
    quotient.isPositive = outSign;
    remainder=zero;
  }
  else
  {
    bool signA = isPositive;
    isPositive = true;
    BigInt copyB(b);
    copyB.isPositive = true;
    int pos = vec.size() -1;
    BigInt dividend_seq(vec[pos], base);
    while (dividend_seq < copyB)
    {
      pos -= 1;
      if (pos < 0)
      {
        break;
      }
      dividend_seq.vec.insert(dividend_seq.vec.begin(), vec[pos]);
    }
    
    while (pos >= 0)
    {
      int count = 0;
      while (dividend_seq >= copyB)
      {
        dividend_seq -= copyB;
        count++;
      }
      
      quotient.vec.insert(quotient.vec.begin(), count);
      if (dividend_seq.vec.size()==1 && dividend_seq.vec[0] == 0)
      {
        dividend_seq.vec.clear();
      }
      
      if (pos-1 >= 0)
      {
        dividend_seq.vec.insert(dividend_seq.vec.begin(), vec[pos-1]);
      }
      pos--;
    }
    if (quotient.vec.empty())
    {
      quotient = zero;
    }
    else
    {
      quotient.isPositive = outSign;
    }
    if (dividend_seq.vec.empty())
    {
      remainder.vec.push_back(0);
      remainder.isPositive = true;
    }
    else
    {
      remainder = dividend_seq;
      remainder.isPositive = signA;
    }
    
    if (*this < copyB)
    {
      remainder = *this;
      remainder.isPositive = signA;
    }
  }
  
}



/*
//======================
// NON-MEMBER function
//======================
// 1. Create a copy of a
// 2. Call the 'exponentiation' function on the copy and b
// 3. Return the copy
// Note: Should take you exactly 3 lines of code
*/
BigInt pow(const  BigInt &a, const BigInt & b){

    /************* You complete *************/
  
    BigInt copyA(a);
    copyA.exponentiation(b);
  
  
  
    return copyA;//for now
}

/*
//==================
// MEMBER function
//==================
//  Exponentiation assignment function.
//     - i.e., a.exponentiation(b);
// Note: 1. implement Exponentiation by Squaring (see the writeup)
//       2. b should be treated as BigInt, not int or other data type.
*/
const BigInt & BigInt::exponentiation(const BigInt &b){
    if(base != b.base){
        throw DiffBaseException();
    }
    if(!b.isPositive){
        throw ExpByNegativeException();
    }
    /************* You complete *************/
    
    BigInt output(1, base);
    BigInt pow(b);
    BigInt zero(0, base);
    BigInt one(1, base);
    BigInt two(2, base);
    
    while (pow > zero)
    {
      if ((pow %two) == zero)
      {
        *this *= *this;
        pow /= two;
      }
      else
      {
        output *= *this;
        pow -= one;
      }
    }
    *this = output;
  
    return *this; 
}

/*
//======================
// NON-MEMBER function
//======================
// 1. Create a copy of a
// 2. Call the 'modulusExp' function on the copy and b
// 3. Return the copy
// Note: Should take you exactly 3 lines of code
*/
BigInt modPow(const BigInt &a, const BigInt &b, const BigInt &m){

    /************* You complete *************/
   
    BigInt copyA(a);
    copyA.modulusExp(b, m);
  
    return copyA; //for now
  
}


/*
//==================
// MEMBER function
//==================
//  Modulus Exponentiation assignment function.
//     - i.e., a.modulusExp(b)
// Note: 1. implement Modulus Exponentiation (see the writeup)
//       2. b should be treated as BigInt, not int or other data type.
// Hint: same implementation as exponentiation, but take modulus 
//          after every call to the *= operator.
*/
const BigInt & BigInt::modulusExp(const BigInt &b, const BigInt &m){
    if(base != b.base || base != m.base){
        throw DiffBaseException();
    }
    if(!b.isPositive){
        throw ExpByNegativeException();
    }
    /************* You complete *************/
    BigInt output(1, base);
    BigInt pow(b);
    BigInt zero(0, base);
    BigInt one(1, base);
    BigInt two(2, base);
    
    while (pow > zero)
    {
      if ((pow %two) == zero)
      {
        *this *= *this;
        *this %= m;
        pow /= two;
      }
      else
      {
        output *= *this;
        pow -= one;
        output %= m;
      }
    }
    *this = output;
  
    return *this; 
  

  
  
    return *this;
}

//******************************************************************
//END OF THE ARITHMETIC OPERATORS SECTION
//******************************************************************