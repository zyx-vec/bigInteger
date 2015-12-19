#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "bigint.c"

char zero[] = "0";
char one[] = "1";

int modular_exp(int a, int b, int n) {
	int c = 0;
	int d = 1;
	int i = 31;
	for (; !((1<<i)&b); i--);
	
	for (; i >= 0; i--) {
		c = c << 1;
		d = (d*d) % n;
		//printf("%d\n", d);
		if ( b&(1<<i) ) {
			c++;
			d = (d * a) % n;
		}
	}
	return d;
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
		//printf("%s\n", d.decimal);
		if ( bit(&b, i) ) {
			d = mul(d.decimal, a.decimal);
			k = divd(d.decimal, n.decimal);
			strcpy(d.decimal, k.r);
			d.len_d = strlen(d.decimal);
		}
	}
	return d;
}

int main() {
	int a, b, n;
	scanf("%d%d%d", &a, &b, &n);

	printf("int: %d\n", modular_exp(a, b, n));

	char aa[] = "119", bb[] = "599", cc[] = "82";
	strrev(aa); strrev(bb); strrev(cc);
	BigInt b1, b2, b3;
	strcpy(b1.decimal, aa);
	b1.len_d = strlen(aa);
	strcpy(b2.decimal, bb);
	b2.len_d = strlen(bb);
	strcpy(b3.decimal, cc);
	b3.len_d = strlen(cc);

	BigInt t = modular_exp_(b1, b2, b3);
	printf("big int: %s\n", strrev(t.decimal));

	return 0;
}
