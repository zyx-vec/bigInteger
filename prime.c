#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "bigint.c"

#define bool int
#define FALSE 0
#define TRUE 1
#define LL long long
char zero[] = "0";
char one[] = "1";
char two[] = "2";
char largest[] = "922337203";
int count = 0;

LL modular_exp(LL a, LL b, LL n) {
	LL c = 0;
	LL d = 1;
	LL i = 31;
	for (; !((1<<i)&b); i--);
	
	for (; i >= 0; i--) {
		c = c << 2;
		d = (d*d) % n;
		if ( b&(1<<i) ) {
			c++;
			d = (d * a) % n;
		}
	}
	return d;
}

bool pseudo_prime(LL n) {
	if ( modular_exp(2, n-1, n) % n != 1 )
		return FALSE;
	return TRUE;
}

bool witness(LL a, LL n) {
	LL t = 0, u;
	while ((((n-1)>>t) & 0x01) == 0) t++;
	u = (n-1)>>t;

	LL x0 = modular_exp(a, u, n), x1;
	LL i = 1;
	for (; i <= t; i++) {
		x1 = (x0 * x0) % n;
		if (x1 == 1 && x0 != 1 && x0 != n-1)	// composite number
			return TRUE;
		x0 = x1;
	}

	if (x1 != 1)	// definitely composite number
		return TRUE;

	return FALSE;
}

bool miller_robin(int n, int s) {
	LL i = 1, a;
	for (; i <= s; i++) {
		a = rand() % (n-1) + 1;
		if ( witness(a, n) )
			return FALSE;
	}
	return TRUE;
}

BigInt modular_exp_(BigInt a, BigInt b, BigInt n) {
	to_binary(&b);
	int i = b.len_b;
	BigInt d;
	QR k;
	strcpy(d.decimal, one);
	for (; i > 0; i--) {
		d = mul(d.decimal, d.decimal);
		k = divd(d.decimal, n.decimal);
		strcpy(d.decimal, k.r);
		d.len_d = strlen(d.decimal);
		if ( bit(&b, i) ) {
			d = mul(d.decimal, a.decimal);
			k = divd(d.decimal, n.decimal);
			strcpy(d.decimal, k.r);
			d.len_d = strlen(d.decimal);
		}
	}
	return d;
}

bool pseudo_prime_(BigInt n) {
	BigInt tt;
	strcpy(tt.decimal, two);
	tt.len_d = 1;
	BigInt n_minus1 = sub(n.decimal, one);
	BigInt rr = modular_exp_(tt, n_minus1, n);
	QR qq = divd(rr.decimal, n.decimal);
	if ( compare(qq.r, one) != 0 )
		return FALSE;
	return TRUE;
}

bool witness_(char *a, BigInt n) {	// a as witness.
	LL t = 1;

	BigInt n_minus1 = sub(n.decimal, one);
	to_binary(&n_minus1);
	while ( bit(&n_minus1, t) == 0) t++;
	t--;

	LL i, j, k;
	BigInt exp_2;
	strcpy(exp_2.decimal, one);
	for (i = 0; i < t; i++) {
		exp_2 = mul(exp_2.decimal, two);
	}
	QR u = divd(n_minus1.decimal, exp_2.decimal);

	BigInt aa, uu;
	strcpy(aa.decimal, a);
	aa.len_d = strlen(a);
	strcpy(uu.decimal, u.q);
	uu.len_d = strlen(u.q);

	BigInt x0 = modular_exp_(aa, uu, n), x1, tmp;
	QR qrtt;

	i = 1;
	for (; i <= t; i++) {
		tmp = mul(x0.decimal, x0.decimal);
		qrtt = divd(tmp.decimal, n.decimal);
		
		strcpy(x1.decimal, qrtt.r);
		x1.len_d = strlen(qrtt.r);
		if ( compare(x1.decimal, one) == 0 &&\
		 		compare(x0.decimal, one) != 0 &&\
		 		 compare(x0.decimal, n_minus1.decimal) != 0 ) {
			return TRUE;	// composite number
		}
		x0 = x1;
	}

	if ( compare(x1.decimal, one) != 0 )	// definitely composite number
		return TRUE;

	return FALSE;
}

bool miller_robin_(BigInt n) {
	if (compare(n.decimal, two) == 0)
		return TRUE;
	
	if ( witness_(two, n) ) {
		return FALSE;
	}
	return TRUE;
}

// test normal integer, return bool as you like
void fermat(LL n) {
	if ( n == 2 || ((n%2) && pseudo_prime(n)) ) {
		printf("Fermat: %d almost prime\n", n);
		count++;
	}
	else
		printf("Fermat: %d definitely composite\n", n);
}

// test big integer
void fermat_(BigInt b) {
	if ( INT(b.decimal[0]) % 2 == 0 && compare(two, b.decimal) != 0 ) {
		printf("Fermat_: %s definitely composite\n", b.decimal);
	}
	else if ( compare(two, b.decimal) == 0 ) {
		printf("Fermat_: %s almost prime\n", b.decimal);
		count++;
	} else if ( pseudo_prime_(b) ) {
		printf("Fermat_: %s almost prime\n", b.decimal);
		count++;
	} else {
		printf("Fermat_: %s definitely composite\n", b.decimal);
	}
}


int main() {	
	char num[MAX];
	LL n;
	FILE *fp;
	fp = fopen("in.txt", "r");
	if (fp == NULL) {
		printf("open file failed!");
		return -1;
	}
	fgets(num, 512, fp);
	
	// read file in while loop.
	while ( !feof(fp) ) {
		fgets(num, 512, fp);
		
		if ( compare_normal(num, largest) < 0 ) {	// less then largest
			n = atoi(num);
			//fermat(n);
			if ( (n%2 && miller_robin(n, 10)) || n == 2) {
				printf("Miller&Robin: %d almost prime\n", n);
				count++;
			}
			else {
				printf("Miller&Robin: %d definitely composite\n", n);
			}
		}
		else {	// otherwise use BigInt;
			num[strlen(num)-1] = '\0';
			BigInt b, d;
			strcpy(b.decimal, num);
			b.len_d = strlen(num);
			d = b;
			fermat_(b);
			if (miller_robin_(d)) {
				printf("Miller&Robin_: %s almost prime\n", num);
				count++;
			} else {
				printf("Miller&Robin_: %s definitely composite\n", num);
			}
		}
		printf("----------\n");
	}
	printf("total number: %d", count);
	return 0;
}
