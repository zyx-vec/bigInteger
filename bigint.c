#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define bool int
#define TRUE 1
#define FALSE 0
#define MAX 512
#define INT(c) (c - '0')

int mark[MAX];

typedef struct S {
	char decimal[MAX];
	int binary[MAX];
	int len_d;
	int len_b;
} BigInt;

typedef struct Q {
	char q[MAX];
	char r[MAX];
} QR;

// m > n => 1, m == n => 0, m < n => -1
int compare(char *m, char *n) {
	int len1 = strlen(m);
	int len2 = strlen(n);
	int j;
	for (j = len1-1; j > 0; j--) {
		if (m[j] == '0') {
			m[j] = '\0';
		} else
			break;
	}
	for (j = len2-1; j > 0; j--) {
		if (n[j] == '0') {
			n[j] = '\0';
		} else
			break;
	}
	int l = strlen(m) - strlen(n);
	if (l > 0)
		return 1;	// m > n
	else if (l < 0) {
		return -1;	// m < n
	} else {
		int len = strlen(m);
		int i;
		for (i = len-1; i >= 0; i--) {
			if (m[i] > n[i])
				return 1;	// m > n
			else if (m[i] < n[i])
				return -1;	// m < n
		}
	}
	return 0;
}

// normal order compare.
int compare_normal(char *m, char *n) {
	
	int l = strlen(m) - strlen(n);
	if (l > 0)
		return 1;	// m > n
	else if (l < 0) {
		return -1;	// m < n
	} else {
		int len = strlen(m);
		int i;
		for (i = 0; i < len; i++) {
			if (m[i] > n[i])
				return 1;	// m > n
			else if (m[i] < n[i])
				return -1;	// m < n
		}
	}
	return 0;	// equal
}

// m + n
BigInt add(char *m, char *n) {
	char sum[MAX];
	int length = strlen(m) > strlen(n) ? strlen(m) : strlen(n);
	int shorter = 0, min = strlen(n);	// 0 means n is shorter
	if (length == strlen(n)) {
		shorter = 1;					// 1 means m is shorter
		min = strlen(m);
	}

	//printf("%s\n%s\n%d\n", n, m, min);

	int i = 0, carry = 0, out;
	for (; i < length; i++) {
		if (i < min) {
			out = carry + INT(m[i]) + INT(n[i]);
		}
		else {
			if (shorter == 0) {
				out = carry + INT(m[i]);
			} else {
				out = carry + INT(n[i]);
			}
		}
		carry = out / 10;
		sum[i] = out % 10 + '0';
	}
	if (carry != 0) {
		sum[i] = carry + '0';
		i++;
	}
	sum[i] = '\0';
	//printf("sum = %s\n", sum);

	BigInt re;
	strcpy(re.decimal, sum);
	re.len_d = strlen(sum);
	return re;
}

// m - n
BigInt sub(char *a, char *b) {
	char diff[MAX];
	char m[MAX], n[MAX];
	strcpy(m, a); strcpy(n, b);
	// m >= n
	char tmp[MAX];
	if (compare(m, n) < 0) {
		strcpy(tmp, m);
		strcpy(m, n);
		strcpy(n, tmp);
		//printf("sub: %s %s\n", m, n);
	}
	int length = strlen(m);
	int min = strlen(n);

	int i;
	for (i = min; i < length; i++) {
		n[i] = '0';
	}
	n[i] = '\0';

	int out;
	int borr[MAX];
	memset(borr, 0, sizeof(int)*MAX);
	for (i = 0; i < length; i++) {
		out = INT(m[i]) - INT(n[i]) - borr[i];
		if (out < 0) {
			out += 10;
			borr[i+1] = 1;
		}
		diff[i] = out + '0';
	}

	diff[i] = '\0';
	i = strlen(diff)-1;
	for (; i > 0; i--) {
		if (diff[i] == '0')
			diff[i] = '\0';
		else
			break;
	}
	BigInt re;
	strcpy(re.decimal, diff);
	re.len_d = strlen(diff);
	return re;
}

// m * n
BigInt mul(char *m, char *n) {
	BigInt d;
	char zero[] = "0";
	strcpy(d.decimal, zero);

	int len = strlen(n);
	int len1 = strlen(m);

	char a[MAX];
	int i = 0, j = 0;

	int carry = 0, out;
	for (i = 0; i < len; i++) {	// m * n
		for (j = 0; j < i; j++) {	// fill '0'
			a[j] = '0';
		}
		int k = INT(n[i]);
		for (j = 0; j < len1; j++) {
			out = (k * (INT(m[j]))) + carry;
			a[j+i] = out % 10 + '0';
			carry = out / 10;
		}
		if (carry != 0) {
			a[j+i] = carry + '0';
			j++;
			carry = 0;
		}
		a[j+i] = '\0';
		d = add(d.decimal, a);
	}

	d.len_d = strlen(d.decimal);
	return d;
}

// m / n
QR divd(char *a, char *b) {
	char zero[] = "0";
	memset(mark, 0, MAX*sizeof(int));
	char m[MAX], n[MAX];
	strcpy(m, a); strcpy(n, b);

	QR re;
	int len = strlen(m), len1 = strlen(n);
	int i = 0, j = 0;

	//mark[3] = 0;
	// m < n
	if ( compare(m, n) < 0 ) {
		char zero[] = "0";
		strcpy(re.q, zero);
		strcpy(re.r, m);
		return re;
	} else if ( compare(m, n) == 0) {
		char one[] = "1";
		char zero[] = "0";
		strcpy(re.q, one);
		strcpy(re.r, zero);
		return re;
	} else {
		int index = 0;
		char curr[MAX];
		int curr_len;
		curr[0] = '\0';
		curr_len = strlen(curr);

		char tmp[MAX];	// normal order of n
		strcpy(tmp, n);
		strrev(tmp);
		//printf("%s %s\n", tmp, curr);

		while (index < len) {
			// char s1[] = "1223", s2[] = "34452352352345321";
			// TODO: eat
			 do {
				if (curr_len == 0) {
					if (m[len-1-index] != '0') {
						curr[curr_len] = m[len-1-index];
						curr[curr_len+1] = '\0';
						curr_len++;
					}
				} else {
					curr[curr_len] = m[len-1-index];
					curr[curr_len+1] = '\0';
					curr_len++;
				}
				index++;
			} while (compare_normal(curr, tmp) < 0 && index < len);
				
			//printf("%d: %s\n", index-1, curr);

			if (compare_normal(curr, tmp) < 0) {	// can't eat any more
				//printf("haha\n");
				if (curr_len == 0) {
					strcpy(re.r, zero);
				} else {
					strrev(curr);
					strcpy(re.r, curr);
				}
				break;
			}
			strrev(curr);
			
			BigInt att;		// attempt to divide
			strcpy(att.decimal, n);
			char old[MAX];
			char inr[] = "1";
			do {
				strcpy(old, att.decimal);
				inr[0] += 1;
				att = mul(n, inr);
			} while ( compare(att.decimal, curr) <= 0 );
			
			mark[index-1] = inr[0]-'1';
			//printf("index: %d, mark: %d\n", index-1, mark[index-1]);

			//printf("-%s - %s\n", curr, old);
			if (compare(curr, old) < 0) {
				printf("...\n");
			}
			//printf("curr = %s\n old = %s\n", curr, old);
			att = sub(curr, old);
			//printf("index = %d, att = %s\n", index-1, att.decimal);
			
			if ( index == len ) {	// over
				strcpy(re.r, att.decimal);
				break;
			}
			else {
				char zero[] = "0";
				if (compare(zero, att.decimal) == 0) {
					curr[0] = '\0';
				} else {
					strcpy(curr, att.decimal);
					strrev(curr);
				}
				curr_len = strlen(curr);
				//printf("curr = %s\n", curr);
			}
		}

		for (i = 0; i < len; i++) {
			//printf("%d ", mark[i]);
			if (mark[i] != 0)
				break;
		}
		int count = 0;
		for (; i < len; i++) {
			//printf("%d ", mark[i]);
			re.q[count] = mark[i] + '0';
			count++;
		}
		re.q[count] = '\0';
		strrev(re.q);
		//printf("\n");
	}

	return re;
}

int power(int m, int n) {
	if (n == 0)
		return 1;
	int i = 0;
	int re = 1;
	for (; i < n; i++) {
		re *= m;
	}
	return re;
}

void to_binary(BigInt *n) {
	int i = 0, j = 0, k = 0, c = 0;
	char base[] = "1024";	// 2 ^ 32 = 4294967296
	strrev(base);

	QR qr;
	strcpy(qr.q, (*n).decimal);
	while ( compare(qr.q, base) >= 0 ) {
		qr = divd(qr.q, base);
		// qr.q and qr.r;
		for (i = strlen(qr.r)-1; i > 0; i--) {
			k += INT(qr.r[i]) * power(10, i);
		}
		k += INT(qr.r[0]);
		(*n).binary[c] = k;
		k = 0;
		c++;
	}
	for (i = strlen(qr.q)-1; i > 0; i--) {
		k += INT(qr.q[i]) * power(10, i);
	}
	k += INT(qr.q[0]);
	(*n).binary[c] = k;
	/*
	printf("base is 1024! reverse order\n");
	for (i = 0; i <= c; i++) {
		printf("%d ", (*n).binary[i]);
	}*/
	char s[15];
	itoa(k, s, 2);
	(*n).len_b = c*10 + strlen(s);
	//printf("\n");
	//printf("\nbits number: %d\n", (*n).len_b);
}

int bit(BigInt *n, int i) {
	int table = i / 10;
	int offset = i % 10;
	if (offset == 0) {	// corner case
		offset = 10;
		table--;
	}
	return ((*n).binary[table] & (1 << (offset-1))) >> (offset-1);
}


/*
int main() {
	char s1[MAX] = "2", s2[MAX] = "7001";
	int n1, n2;
	scanf("%d%d", &n1, &n2);
	itoa(n2, s1, 10);
	itoa(n1, s2, 10);
	printf("%d, %d", n1, n2);

	// reverse order easy to caculate.
	strrev(s1); strrev(s2);

	// test to_binary and bit function
	BigInt data;
	strcpy(data.decimal, s2);
	to_binary(&data);
	int i;
	for (i = 0; i < data.len_b; i++) {
		printf("%d", bit(&data, i+1));
	}
	printf("\n----------\n");
	

	// test add sub mul divd
	BigInt a, s, m, d;
	QR qr;
	a = add(s1, s2);
	s = sub(s1, s2);
	m = mul(s1, s2);
	qr = divd(s2, s1);

	strrev(s1); strrev(s2);
	printf("%s + %s = %s\n", s1, s2, strrev(a.decimal));
	printf("|%s - %s| = %s\n", s1, s2, strrev(s.decimal));
	printf("%s * %s = %s\n", s1, s2, strrev(m.decimal));
	printf("%s mod %s = %s\n", s2, s1, strrev(qr.r));
	printf("%s / %s = %s\n", s2, s1, strrev(qr.q));

	printf("-----------\n");
	printf("%d + %d = %d\n", n1, n2, n1 + n2);
	printf("|%d - %d| = %d\n", n1, n2, n1 - n2);
	printf("%d * %d = %d\n", n1, n2, n1 * n2);
	printf("%d mod %d = %d\n", n1, n2, n1 % n2);
	printf("%d / %d = %d\n", n1, n2, n1 / n2);
	
	return 0;
}
*/