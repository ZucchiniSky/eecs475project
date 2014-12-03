#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <gmpxx.h>
#include <utility>
#include "ec_ops.h"
#include <vector>
using namespace std;
void parallel_assign(mpz_class &a, mpz_class &b, mpz_class quotient);

void parallel_assign(mpz_class &a, mpz_class &b, mpz_class quotient)
{
    mpz_class temp = b;
    b = a - quotient * temp;
    a = temp;
}

Zp Zp::inverse() const{
	// Implement the Extended Euclidean Algorithm to return the inverse mod PRIME
    mpz_class s = 0;
    mpz_class t = 1;
    
    mpz_class r = PRIME;
    mpz_class old_r = value;
    mpz_class old_s = 1;
    mpz_class old_t = 0;
    
    mpz_class quotient = 0;
    while (r != 0)
    {
        quotient = old_r / r;
        parallel_assign(old_r, r, quotient);
        parallel_assign(old_s, s, quotient);
        parallel_assign(old_t, t, quotient);
    }
    old_s = old_s % PRIME;
    if (old_s < 0)
    {
        old_s = old_s + PRIME;
    }
    Zp inv;
    inv.setValue(old_s);
    return inv;
}


ECpoint ECpoint::operator + (const ECpoint &a) const {
	// Implement  elliptic curve addition
    if (infinityPoint)
    {
    	if (a.infinityPoint)
    	{
    		return ECpoint(true);
    	}
    	return ECpoint(a.x, a.y);
    }
    if (a.infinityPoint)
    {
    	return ECpoint(x, y);
    }
    Zp two(2), three(3);
    Zp xR, yR, delta;
    if (!(x == a.x))// && !(y == a.y))
    {
        // case one
        delta = (a.y-y) * ((a.x-x).inverse());
        xR = delta * delta - x - a.x;
    } else if (x == a.x && y == a.y && !(two * y == 0))
    {
        // case two
        delta = (three * x * x + A) * ((two * y).inverse());
        xR = delta * delta - two * x;
    } else {
        // case three (identity element)
        return ECpoint(true);
    }
    yR = delta * (x - xR) - y;
    return ECpoint(xR, yR);
}


ECpoint ECpoint::repeatSum(ECpoint p, mpz_class v) const {
	//Find the sum of p+p+...+p (vtimes)
    if (v == 0)
    {
        // this is p^0, which is the identity
        return ECpoint(true);
    }
    ECpoint Q(true);
    mpz_class mod(1);
    int bits = 0;
    while (mod <= v)
    {
        mod = mod * 2;
        bits++;
    }
    mpz_class shrinker = v;
    vector<mpz_class> bitrep;
    for (int i = 0; i < bits; i++)
    {
        bitrep.push_back(shrinker % 2);
        shrinker = shrinker / 2;
    }
    for (int i = 0; i < bits; i++)
    {
        Q = Q + Q;
        if (bitrep.at(bits - 1 - i) == 1)
        {
            Q = Q + p;
        }
    }
    return Q;
}

Zp ECsystem::power(Zp val, mpz_class pow) {
	//Find the sum of val*val+...+val (pow times)
    if (pow == 0)
    {
        return Zp(1);
    }
    Zp Q(1);
    mpz_class mod(1);
    int bits = 0;
    while (mod <= pow)
    {
        mod = mod * 2;
        bits++;
    }
    mpz_class shrinker = pow;
    vector<mpz_class> bitrep;
    for (int i = 0; i < bits; i++)
    {
        bitrep.push_back(shrinker % 2);
        shrinker = shrinker / 2;
    }
    for (int i = 0; i < bits; i++)
    {
        Q = Q * Q;
        if (bitrep.at(bits - 1 - i) == 1)
        {
            Q = Q * val;
        }
    }
    return Q;
}


mpz_class ECsystem::pointCompress(ECpoint e) {
	//It is the gamma function explained in the assignment.
	//Note: Here return type is mpz_class because the function may
	//map to a value greater than the defined PRIME number (i.e, range of Zp)
	//This function is fully defined.	
	mpz_class compressedPoint = e.x.getValue();
	compressedPoint = compressedPoint<<1;
	
	if(e.infinityPoint) {
		cout<<"Point cannot be compressed as its INF-POINT"<<flush;
		abort();
		}
	else {
		if (e.y.getValue()%2 == 1)
			compressedPoint = compressedPoint + 1;
		}
		//cout<<"For point  "<<e<<"  Compressed point is <<"<<compressedPoint<<"\n";
		return compressedPoint;

}

ECpoint ECsystem::pointDecompress(mpz_class compressedPoint){
	//Implement the delta function for decompressing the compressed point
	
	ECpoint test1(23,32);
	ECpoint test2(true);
	ECpoint test3 = test1+test2;
	cout << "test3: " << test3 << endl;
	
    Zp x;
    x.setValue(compressedPoint / 2);
    Zp identity(0);
    mpz_class modbit = compressedPoint % 2;
    Zp z;
    z.setValue((x * x * x).getValue() + A * x.getValue() + B);
    if (z.getValue() < 0)
    {
        z.setValue(z.getValue() + PRIME);
    }
    Zp y = 0;
    Zp quadRes1 = power(z, (PRIME + 1) / 4);
    Zp quadRes2 = identity - quadRes1;
    if (quadRes1.getValue() % 2 == modbit)
    {
        y = quadRes1;
    } else if (quadRes2.getValue() % 2 == modbit)
    {
        y = quadRes2;
    } else
    {
        cout << "Point decompression error" << flush;
        abort();
    }
    return ECpoint(x, y);
}


pair<mpz_class, mpz_class> ECsystem::encrypt(ECpoint publicKey, mpz_class privateKey,mpz_class plaintext){
	// You must implement elliptic curve encryption
	//  Do not generate a random key. Use the private key that is passed from the main function
    ECpoint Q = G * privateKey;
    ECpoint R = publicKey * privateKey;
    return make_pair<mpz_class, mpz_class>(pointCompress(Q), plaintext ^ pointCompress(R));
}


mpz_class ECsystem::decrypt(pair<mpz_class, mpz_class> ciphertext){
	// Implement EC Decryption
    ECpoint R = pointDecompress(ciphertext.first) * privateKey;
    return ciphertext.second ^ pointCompress(R);
}


/*
 * main: Compute a pair of public key and private key
 *       Generate plaintext (m1, m2)
 *       Encrypt plaintext using elliptic curve encryption
 *       Decrypt ciphertext using elliptic curve decryption
 *       Should get the original plaintext
 *       Don't change anything in main.  We will use this to 
 *       evaluate the correctness of your program.
 */


int main(void){
	srand(time(0));

	ECsystem ec;
	mpz_class incrementVal;	
	pair <ECpoint, mpz_class> keys = ec.generateKeys();
	
	mpz_class plaintext = MESSAGE;
	ECpoint publicKey = keys.first;
	cout<<"Public key is: "<<publicKey<<"\n";
	
	cout<<"Enter offset value for sender's private key"<<endl;
	cin>>incrementVal;
	mpz_class privateKey = XB + incrementVal;
	pair<mpz_class, mpz_class> ciphertext = ec.encrypt(publicKey, privateKey, plaintext);	
	cout<<"Encrypted ciphertext is: ("<<ciphertext.first<<", "<<ciphertext.second<<")" << endl;
	mpz_class plaintext1 = ec.decrypt(ciphertext);
	
	cout << "Original plaintext is: " << plaintext << endl;
	cout << "Decrypted plaintext: " << plaintext1 << endl;


	if(plaintext == plaintext1)
		cout << "Correct!" << endl;
	else
		cout << "Plaintext different from original plaintext." << endl;		
	return 1;

}


