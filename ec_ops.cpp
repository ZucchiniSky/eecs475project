#include "ec_ops.h"
#include <cstdio>
#include <cstdlib>

//==================== Methods for Zp ====================
Zp Zp::operator+(const Zp &a) const {
	mpz_class result = (this->value + a.value) % PRIME;
	Zp c;
	c.setValue(result);
	return c;
}

Zp Zp::operator*(const Zp &a) const {
	mpz_class result = (this->value * a.value) % PRIME;
	Zp c;
	c.setValue(result);
	return c;
}

Zp Zp::operator-(const Zp &a) const {
	mpz_class result = (this->value - a.value)  % PRIME;
	Zp c;
	c.setValue(result);
	return c;
}

bool Zp::operator==(const Zp &a) const {
	if (this->value == a.value)
		return true;
	// since % in gmpxx returns something between -PRIME+1 and PRIME-1,
	// we need the following two checks as well
	if (this->value == a.value - PRIME)
		return true;
	if (this->value == a.value + PRIME)
		return true;
	return false;
}

ostream& operator<<(ostream& output, const Zp &a){
	output << a.value;
	return output;
}

//================== Methods for ECpoint ==================
bool ECpoint::operator == (const ECpoint &a) const {
	if(this->x == a.x && this->y == a.y)
		return true;
	return false;
}

ECpoint ECpoint::operator * (const mpz_class &a) const {
	return repeatSum(*this, a);
}




ostream& operator << (ostream& output, const ECpoint& a){
	if(a.infinityPoint == true)
		output << "(INF_POINT)";
	else
		output << "(" << a.x << "," << a.y << ")";
	return output;
}

//================= Methods for ECsystem =================

pair <ECpoint, mpz_class> ECsystem::generateKeys(){
	//Generate the private key and public key for the user to whom message is sent
	//Returns only the "P" value of public key and "a" value of private key, 
	//as other parameters are globally defined
	
	privateKey = XA;
	publicKey = G*privateKey; 
	return pair <ECpoint, mpz_class> (publicKey, privateKey);
}


