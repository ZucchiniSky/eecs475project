#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <gmpxx.h>
#include <utility>
#include "ec_ops.h"
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
     	mpz_class r = value;
    	mpz_class old_r = PRIME;
    mpz_class old_s = 1;
    mpz_class old_t = 0;
    
    mpz_class quotient = 0;
    cout << "starting\n";
    while (r != 0)
    {
        quotient = old_r / r;
        parallel_assign(old_r, r, quotient);
        //cout << "old_r: " << old_r << " " << "r: " << r << "\n";
        parallel_assign(old_s, s, quotient);
        //cout << "old_s: " << old_s << " " << "s: "<<s  << "\n";
        parallel_assign(old_t, t, quotient);
        //cout << "old_t: " << old_t << " " << "t: "<<t <<  "\n";
    }
    cout << "done\n";
    Zp inv(old_s);
    return inv;
    // this might be old_t instead...
}


ECpoint ECpoint::operator + (const ECpoint &a) const {
	// Implement  elliptic curve addition
    Zp two(2), three(3);
    Zp xR, delta;
    if (!(*this == a) && !(x == a.x))
    {
        // case one
        delta.setValue((a.y.getValue()-y.getValue())/(a.x.getValue()-x.getValue()));
        xR.setValue((delta.getValue() * delta.getValue()) - x.getValue() - a.x.getValue());
    } else if ((*this == a) && !(two * y == 0))
    {
        // case two
        delta.setValue((three.getValue() * x.getValue() * x.getValue() + A)/(two.getValue() * y.getValue()));
        xR.setValue((delta.getValue() * delta.getValue()) + two.getValue() * x.getValue());
    } else {
        // case three (identity element)
        return ECpoint(true);
    }
    Zp yR(delta.getValue() * (x.getValue() - xR.getValue()) - y.getValue());
    return ECpoint(xR, yR);
}


ECpoint ECpoint::repeatSum(ECpoint p, mpz_class v) const {
	//Find the sum of p+p+...+p (vtimes)
	cout << "repeatsum with v = " << v << endl;
    if (v == 0)
    {
        // this is p^0, which is the identity
        return ECpoint(true);
    }
    cout << "v != 0" << endl;
    ECpoint Q(0,0);
    cout << "created point" << endl;
    mpz_class val(1);
    cout << "initiated val" << endl;
    while (val <= v)
    {
    	val = val * 2;
    }
    cout << "first while" << endl;
    val = val / 2;
    cout << "val / 2" << endl;
    for (; val > 0; val = val / 2)
    {
    	cout << "in loop" << endl;
    	Q = Q + Q;
    	cout << "added Q" << endl;
    	if ((v % val) == 1)
    	{
    		cout << "in if" << endl;
    		Q = Q + p;
    		cout << "added P" << endl;
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
    Zp newVal(val.getValue());
    while (pow > 1)
    {
    	newVal = newVal * val;
        pow = pow - 1;
    }
    return newVal;
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
    Zp x(compressedPoint/2);
    Zp identity(0);
    bool modbit = (compressedPoint % 2 == 0);
    Zp posY = x * x;
    Zp negY = identity - x * x;
    Zp y = 0;
    if ((posY.getValue() % 2 == 0) && modbit)
    {
        y = posY;
    } else
    {
        y = negY;
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
	cout << "Hi" << endl;
	
	ECsystem ec;
	mpz_class incrementVal;	
	pair <ECpoint, mpz_class> keys = ec.generateKeys();
	
	cout << "gen'd keys" << endl;
	
	mpz_class plaintext = MESSAGE;
	ECpoint publicKey = keys.first;
	cout<<"Public key is: "<<publicKey<<"\n";
	
	cout<<"Enter offset value for sender's private key"<<endl;
	cin>>incrementVal;
	mpz_class privateKey = XB + incrementVal;
	/*
	pair<mpz_class, mpz_class> ciphertext = ec.encrypt(publicKey, privateKey, plaintext);	
	cout<<"Encrypted ciphertext is: ("<<ciphertext.first<<", "<<ciphertext.second<<")\n";
	mpz_class plaintext1 = ec.decrypt(ciphertext);
	
	cout << "Original plaintext is: " << plaintext << endl;
	cout << "Decrypted plaintext: " << plaintext1 << endl;


	if(plaintext == plaintext1)
		cout << "Correct!" << endl;
	else
		cout << "Plaintext different from original plaintext." << endl;	
		*/	
	return 1;

}


