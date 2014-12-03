#ifndef ZP_H
#define ZP_H

#include <iostream>
#include <utility>
#include <gmpxx.h>
using namespace std;


/*
 * Parameters for EC and transform into constants
 */

#define PRIME_STR "57896044618658097711785492504343953926634992332820282019728792003956564819203"
#define ORDER_STR "57896044618658097711785492504343953926473211886304323019964499781607006751467"
#define A_STR  "-3"
#define B_STR  "-51BD"
#define GX_STR "00000000 00000000 00000000 00000000 00000000 00000000 00000000 0000001d"
#define GY_STR "404b36e1 a74def81 37f29Fa5 9849d32e 32e4337c 7ed795bb 8f8f8c42 55313db1"
#define MESSAGE_STR "9999999382193891289191898192839192912191212"
#define XA_STR      "27649501093875983024875987109873744387487392029991029934758710384789774874747" //private Key of receiver
#define XB_STR      "57186659798102937103987098738741922938473470129777292030047589723837472987319" //private key of sender


/*
const mpz_class PRIME(PRIME_STR);
const mpz_class ORDER(ORDER_STR);
const mpz_class A(A_STR);
const mpz_class B(B_STR, 16);
const mpz_class GX(GX_STR, 16);
const mpz_class GY(GY_STR, 16);
const mpz_class MESSAGE(MESSAGE_STR);
const mpz_class XA(XA_STR); //private key of receiver
const mpz_class XB(XB_STR); //private key of sender
*/
const mpz_class PRIME(11);
const mpz_class ORDER(5);
const mpz_class A(1);
const mpz_class B(6);
const mpz_class GX(2);
const mpz_class GY(7);
const mpz_class MESSAGE("189018");
const mpz_class XA(2); //private key of receiver
const mpz_class XB(3); //private key of sender
/*
 * class Zp: An element in GF(p)
 */
class Zp{

	// Overloading cout
	friend ostream& operator<<(ostream& output, const Zp& a);

	private:
	mpz_class value;
	
	public:
	Zp(){}
	Zp(const mpz_class v){ value = v; value %= PRIME; }
	Zp(const int v){ value = v; value %= PRIME; }
	void setValue(const mpz_class v){ 
		if (v < 0) {
			value = PRIME + v;
		}
		else {
			value = v;
		}
	}
	mpz_class getValue() const { return value; }

	Zp operator + (const Zp &a) const;
	Zp operator - (const Zp &a) const;
	Zp operator * (const Zp &a) const;
	bool operator == (const Zp &a) const;
	Zp inverse() const;
};


/* 
 * class ECpoint: A point on an elliptic curve
 */
class ECpoint{

	// Overloading cout
	friend ostream& operator<<(ostream& output, const ECpoint& a);

	public:
	Zp x;
	Zp y;
	bool infinityPoint; //If true, the point is the infinity point

	ECpoint(){
		infinityPoint = false;}
	ECpoint(Zp xx, Zp yy){ x = xx; y =  yy; infinityPoint = false;}
	ECpoint(bool inf){ infinityPoint = inf;}

	ECpoint repeatSum(ECpoint p, mpz_class v) const;
	bool operator == (const ECpoint &a) const;
	ECpoint operator + (const ECpoint &a) const;
	ECpoint operator * (const mpz_class &a) const;
};


/*
 * class ECsystem: Encryption and decryption functions of ec
 */
class ECsystem{
	private:
		mpz_class privateKey;
		ECpoint publicKey;
		ECpoint G; //Generator G
		Zp power(Zp base, mpz_class pow);
		mpz_class pointCompress(ECpoint e);
		ECpoint pointDecompress(mpz_class compressedPoint);
	public:

		ECsystem(){ G = ECpoint(GX, GY);}
		ECpoint getPublicKey(){ return publicKey;}
		
		pair <ECpoint, mpz_class> generateKeys();		

		
		pair<mpz_class,mpz_class> encrypt(ECpoint publicKey, mpz_class privateKey, mpz_class plaintext);
		mpz_class decrypt(pair<mpz_class, mpz_class> cm);
};

#endif

