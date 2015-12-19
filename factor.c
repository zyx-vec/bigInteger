#include <stdio.h>

typedef long long int LL;
#define bool int
#define FALSE 0
#define TRUE 1

int modular_exp(int a, int b, LL n) {
	int c = 0;
	LL d = 1;
	int i = 31;
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

bool witness(int a, int n) {
	int t = 0, u;
	while ((((n-1)>>t) & 0x01) == 0) t++;
	u = (n-1)>>t;

	int x0 = modular_exp(a, u, n), x1;
	int i = 1;
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
	int i = 1, a;
	for (; i <= s; i++) {
		a = rand() % (n-1) + 1;
		//printf("%d ", a);
		if ( witness(a, n) )
			return FALSE;
	}
	return TRUE;
}

int gcd(LL a, LL b) {
	a = (a < 0)? -a: a;
	//printf("hello\n");
	if (b == 0)
		return a;
	else {
		return gcd(b, a % b);
	}
}

void factor(LL N) {
	int x = 5, x1 = 2, k = 1, l = 1;			// B1
	LL n = N;
	int b2 = 1;
	while (1) {
		if ( pseudo_prime(n) && b2 ) {			// B2
			printf("%lld ", n);
			return;
		} else {
			b2 = 1;
			LL g = gcd(x1-x, n);				// B3
			if (g != 1) {
				printf("%d ", g);
				if (g == n) {
					printf("Failed!\n");
					return;
				}
				else {
					n = n / g;
					x = x % n;
					x1 = x1 % n;
				}
			} else {							// B4
				k = k-1;
				if (k == 0) {
					x1 = x;
					l = l << 1;
					k = l;
				}
				x = (x*x + 1) % n;
				b2 = 0;
			}
		}
	}
}


int main() {
	//printf("%d\n", gcd(-4, 10));
	factor(1560842743);
	return 0;
}
