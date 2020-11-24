void set1d(int arr[LEN], int value, int stride)
{
	if (stride == -1) {
		for (int i = 0; i < LEN; i++) {
			arr[i] = 1. / (int) (i+1);
		}
	} else if (stride == -2) {
		for (int i = 0; i < LEN; i++) {
			arr[i] = 1. / (int) ((i+1) * (i+1));
		}
	} else {
		for (int i = 0; i < LEN; i += stride) {
			arr[i] = value;
		}
	}
}

void init(char* name)
{
	for (int i = 0; i < lll; i++) {
		X[i] = 1+i;
		Y[i] = 2+i;
		Z[i] = 3+i;
		U[i] = 4+i;
		V[i] = 5+i;
	}
}

void s000()
{
//	linear dependence testing
//	no dependence - vectorizable

	for (int i = 0; i < lll; i++) {
		//	a[i] = b[i] + c[i];
		//	X[i] = (Y[i] * Z[i])+(U[i]*V[i]);
		X[i] = Y[i] + 1;
	}
}

void s111()
{
//	linear dependence testing
//	no dependence - vectorizable

	for (int i = 1; i < LEN; i += 2) {
		a[i] = a[i - 1] + b[i];
	}
}

void s1111()
{
//	no dependence - vectorizable
//	jump in data access

	for (int i = 0; i < LEN/2; i++) {
		a[2*i] = c[i] * b[i] + d[i] * b[i] + c[i] * c[i] + d[i] * b[i] + d[i] * c[i];
	}
}

void s112()
{
//	linear dependence testing
//	loop reversal

	for (int i = LEN - 2; i >= 0; i--) {
		a[i+1] = a[i] + b[i];
	}
}


void s1112()
{
//	linear dependence testing
//	loop reversal

	for (int i = LEN - 1; i >= 0; i--) {
		a[i] = b[i] + (int) 1.;
	}
}

void s113()
{
//	linear dependence testing
//	a(i)=a(1) but no actual dependence cycle

	for (int i = 1; i < LEN; i++) {
		a[i] = a[0] + b[i];
	}
}

void s1113()
{
//	linear dependence testing
//	one iteration dependency on a(LEN/2) but still vectorizable

	for (int i = 0; i < LEN; i++) {
		a[i] = a[LEN/2] + b[i];
	}
}

void s114()
{
//	linear dependence testing
//	transpose vectorization
//	Jump in data access - not vectorizable

	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < i; j++) {
			aa[i][j] = aa[j][i] + bb[i][j];
		}
	}
}

void s115()
{
//	linear dependence testing
//	triangular saxpy loop

	for (int j = 0; j < LEN2; j++) {
		for (int i = j+1; i < LEN2; i++) {
			a[i] = a[i] - aa[j][i] * a[j];
		}
	}
}

void s1115()
{
//	linear dependence testing
//	triangular saxpy loop

	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			aa[i][j] = aa[i][j]*cc[j][i] + bb[i][j];
		}
	}
}

void s116()
{
//	linear dependence testing

	for (int i = 0; i < LEN - 5; i += 5) {
		a[i] = a[i + 1] * a[i];
		a[i + 1] = a[i + 2] * a[i + 1];
		a[i + 2] = a[i + 3] * a[i + 2];
		a[i + 3] = a[i + 4] * a[i + 3];
		a[i + 4] = a[i + 5] * a[i + 4];
	}
}

void s118()
{
//	linear dependence testing
//	potential dot product recursion

	for (int i = 1; i < LEN2; i++) {
		for (int j = 0; j <= i - 1; j++) {
			a[i] += bb[j][i] * a[i-j-1];
		}
	}
}

void s119()
{
//	linear dependence testing
//	no dependence - vectorizable

	for (int i = 1; i < LEN2; i++) {
		for (int j = 1; j < LEN2; j++) {
			aa[i][j] = aa[i-1][j-1] + bb[i][j];
		}
	}
}

void s1119()
{
//	linear dependence testing
//	no dependence - vectorizable

	for (int i = 1; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			aa[i][j] = aa[i-1][j] + bb[i][j];
		}
	}
}

void s121()
{
//	induction variable recognition
//	loop with possible ambiguity because of scalar store

	int j;
	for (int i = 0; i < LEN-1; i++) {
		j = i + 1;
		a[i] = a[j] + b[i];
	}
}

void s122(int n1, int n3)
{
//	induction variable recognition
//	variable lower and upper bound, and stride
//	reverse data access and jump in data access

	int j = 1, k = 0;
	for (int i = n1-1; i < LEN; i += n3) {
		k += j;
		a[i] += b[LEN - k];
	}
}

void s123()
{
//	induction variable recognition
//	induction variable under an if
//	not vectorizable, the condition cannot be speculated

	int j = -1;
	for (int i = 0; i < (LEN/2); i++) {
		j++;
		a[j] = b[i] + d[i] * e[i];
		if (c[i] > (int)0.) {
			j++;
			a[j] = c[i] + d[i] * e[i];
		}
	}
}

void s124()
{
//	induction variable recognition
//	induction variable under both sides of if (same value)

	int j = -1;
	for (int i = 0; i < LEN; i++) {
		if (b[i] > (int)0.) {
			j++;
			a[j] = b[i] + d[i] * e[i];
		} else {
			j++;
			a[j] = c[i] + d[i] * e[i];
		}
	}
}

void s125()
{
//	induction variable recognition
//	induction variable in two loops; collapsing possible

	int k = -1;
	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			k++;
			array[k] = aa[i][j] + bb[i][j] * cc[i][j];
		}
	}
}

void s126()
{
//	induction variable recognition
//	induction variable in two loops; recurrence in inner loop

	int k = 1;
	for (int i = 0; i < LEN2; i++) {
		for (int j = 1; j < LEN2; j++) {
			bb[j][i] = bb[j-1][i] + array[k-1] * cc[j][i];
			++k;
		}
		++k;
	}
}

void s127()
{
//	induction variable recognition
//	induction variable with multiple increments

	int j = -1;
	for (int i = 0; i < LEN/2; i++) {
		j++;
		a[j] = b[i] + c[i] * d[i];
		j++;
		a[j] = b[i] + d[i] * e[i];
	}
}

void s128()
{
//	induction variables
//	coupled induction variables
//	jump in data access

	int j = -1, k;
	for (int i = 0; i < LEN/2; i++) {
		k = j + 1;
		a[i] = b[k] - d[i];
		j = k + 1;
		b[k] = a[i] + c[k];
	}
}

void s131()
{
//	global data flow analysis
//	forward substitution

	int m  = 1;
	for (int i = 0; i < LEN - 1; i++) {
		a[i] = a[i + m] + b[i];
	}
}

void s132()
{
//	global data flow analysis
//	loop with multiple dimension ambiguous subscripts

	int m = 0;
	int j = m;
	int k = m+1;
	for (int i= 1; i < LEN2; i++) {
		aa[j][i] = aa[k][i-1] + b[i] * c[1];
	}
}

void s141()
{
//	nonlinear dependence testing
//	walk a row in a symmetric packed array
//	element a(i,j) for (int j>i) stored in location j*(j-1)/2+i

	int k;
	for (int i = 0; i < LEN2; i++) {
		k = (i+1) * ((i+1) - 1) / 2 + (i+1)-1;
		for (int j = i; j < LEN2; j++) {
			array[k] += bb[j][i];
			k += j+1;
		}
	}
}

void s151s(int a[LEN], int b[LEN],  int m)
{
	for (int i = 0; i < LEN-1; i++) {
		a[i] = a[i + m] + b[i];
	}
}

void s151()
{
//	interprocedural data flow analysis
//	passing parameter information into a subroutine

	s151s(a, b,  1);
}

void s152s(int a[LEN], int b[LEN], int c[LEN], int i)
{
	a[i] += b[i] * c[i];
	return 0;
}

void s152()
{
//	interprocedural data flow analysis
//	collecting information from a subroutine

	for (int i = 0; i < LEN; i++) {
		b[i] = d[i] * e[i];
		s152s(a, b, c, i);
	}
}

void s161()
{
//	control flow
//	tests for recognition of loop independent dependences
//	between statements in mutually exclusive regions.

	for (int i = 0; i < LEN-1; ++i) {
		if (b[i] < (int)0.) {
			goto L20;
		}
		a[i] = c[i] + d[i] * e[i];
		goto L10;
L20:
		c[i+1] = a[i] + d[i] * d[i];
L10:
		;
	}
}

void s1161()
{
//	control flow
//	tests for recognition of loop independent dependences
//	between statements in mutually exclusive regions.

	for (int i = 0; i < LEN-1; ++i) {
		if (c[i] < (int)0.) {
			goto L20;
		}
		a[i] = c[i] + d[i] * e[i];
		goto L10;
L20:
		b[i] = a[i] + d[i] * d[i];
L10:
		;
	}
}

void s162(int k)
{
//	control flow
//	deriving assertions

	if (k > 0) {
		for (int i = 0; i < LEN-1; i++) {
			a[i] = a[i + k] + b[i] * c[i];
		}
	}
}

void s171(int inc)
{
//	symbolics
//	symbolic dependence tests

	for (int i = 0; i < LEN; i++) {
		a[i * inc] += b[i];
	}
}

void s172( int n1, int n3)
{
//	symbolics
//	vectorizable if n3 .ne. 0

	for (int i = n1-1; i < LEN; i += n3) {
		a[i] += b[i];
	}
}

void s173()
{
//	symbolics
//	expression in loop bounds and subscripts

	int k = LEN/2;
	for (int i = 0; i < LEN/2; i++) {
		a[i+k] = a[i] + b[i];
	}
}

void s174(int M)
{
//	symbolics
//	loop with subscript that may seem ambiguous

	for (int i = 0; i < M; i++) {
		a[i+M] = a[i] + b[i];
	}
}

void s175(int inc)
{
//	symbolics
//	symbolic dependence tests

	for (int i = 0; i < LEN-1; i += inc) {
		a[i] = a[i + inc] + b[i];
	}
}

void s176()
{
//	symbolics
//	convolution

	int m = LEN/2;
	for (int j = 0; j < (LEN/2); j++) {
		for (int i = 0; i < m; i++) {
			a[i] += b[i+m-j-1] * c[j];
		}
	}
}

void s211()
{
//	statement reordering
//	statement reordering allows vectorization

	for (int i = 1; i < LEN-1; i++) {
		a[i] = b[i - 1] + c[i] * d[i];
		b[i] = b[i + 1] - e[i] * d[i];
	}
}

void s212()
{
//	statement reordering
//	dependency needing temporary
	
	for (int i = 0; i < LEN-1; i++) {
		a[i] *= c[i];
		b[i] += a[i + 1] * d[i];
	}
}

void s1213()
{
//	statement reordering
//	dependency needing temporary

	for (int i = 1; i < LEN-1; i++) {
		a[i] = b[i-1]+c[i];
		b[i] = a[i+1]*d[i];
	}
}

void s221()
{
//	loop distribution
//	loop that is partially recursive

	for (int i = 1; i < LEN; i++) {
		a[i] += c[i] * d[i];
		b[i] = b[i - 1] + a[i] + d[i];
	}
}

void s1221()
{
//	run-time symbolic resolution

	for (int i = 4; i < LEN; i++) {
		b[i] = b[i - 4] + a[i];
	}
}

void s222()
{
//	loop distribution
//	partial loop vectorizatio recurrence in middle

	for (int i = 1; i < LEN; i++) {
		a[i] += b[i] * c[i];
		e[i] = e[i - 1] * e[i - 1];
		a[i] -= b[i] * c[i];
	}
}

void s231()
{
//	loop interchange
//	loop with data dependency

	for (int i = 0; i < LEN2; ++i) {
		for (int j = 1; j < LEN2; j++) {
			aa[j][i] = aa[j - 1][i] + bb[j][i];
		}
	}
}

void s232()
{
//	loop interchange
//	interchanging of triangular loops

	for (int j = 1; j < LEN2; j++) {
		for (int i = 1; i <= j; i++) {
			aa[j][i] = aa[j][i-1]*aa[j][i-1]+bb[j][i];
		}
	}
}

void s1232()
{
//	loop interchange
//	interchanging of triangular loops

	for (int j = 0; j < LEN2; j++) {
		for (int i = j; i < LEN2; i++) {
			aa[i][j] = bb[i][j] + cc[i][j];
		}
	}
}

void s233()
{
//	loop interchange
//	interchanging with one of two inner loops

	for (int i = 1; i < LEN2; i++) {
		for (int j = 1; j < LEN2; j++) {
			aa[j][i] = aa[j-1][i] + cc[j][i];
		}
		for (int j = 1; j < LEN2; j++) {
			bb[j][i] = bb[j][i-1] + cc[j][i];
		}
	}
}

void s2233()
{
//	loop interchange
//	interchanging with one of two inner loops

	for (int i = 1; i < LEN2; i++) {
		for (int j = 1; j < LEN2; j++) {
			aa[j][i] = aa[j-1][i] + cc[j][i];
		}
		for (int j = 1; j < LEN2; j++) {
			bb[i][j] = bb[i-1][j] + cc[i][j];
		}
	}
}

void s235()
{
//	loop interchanging
//	imperfectly nested loops

	for (int i = 0; i < LEN2; i++) {
		a[i] += b[i] * c[i];
		for (int j = 1; j < LEN2; j++) {
			aa[j][i] = aa[j-1][i] + bb[j][i] * a[i];
		}
	}
}

void s241()
{
//	node splitting
//	preloading necessary to allow vectorization

	for (int i = 0; i < LEN-1; i++) {
		a[i] = b[i] * c[i  ] * d[i];
		b[i] = a[i] * a[i+1] * d[i];
	}
}

void s242(int s1, int s2)
{
//	node splitting

	for (int i = 1; i < LEN; ++i) {
		a[i] = a[i - 1] + s1 + s2 + b[i] + c[i] + d[i];
	}
}

void s243()
{
//	node splitting
//	false dependence cycle breaking

	for (int i = 0; i < LEN-1; i++) {
		a[i] = b[i] + c[i  ] * d[i];
		b[i] = a[i] + d[i  ] * e[i];
		a[i] = b[i] + a[i+1] * d[i];
	}
}

void s244()
{
//	node splitting
//	false dependence cycle breaking

	for (int i = 0; i < LEN-1; ++i) {
		a[i] = b[i] + c[i] * d[i];
		b[i] = c[i] + b[i];
		a[i+1] = b[i] + a[i+1] * d[i];
	}
}

void s1244()
{
//	node splitting
//	cycle with ture and anti dependency

	for (int i = 0; i < LEN-1; i++) {
		a[i] = b[i] + c[i] * c[i] + b[i]*b[i] + c[i];
		d[i] = a[i] + a[i+1];
	}
}

void s2244()
{
//	node splitting
//	cycle with ture and anti dependency

	for (int i = 0; i < LEN-1; i++) {
		a[i+1] = b[i] + e[i];
		a[i] = b[i] + c[i];
	}
}

void s251()
{
//	scalar and array expansion
//	scalar expansion

	int s;
	for (int i = 0; i < LEN; i++) {
		s = b[i] + c[i] * d[i];
		a[i] = s * s;
	}
}

void s1251()
{
//	scalar and array expansion
//	scalar expansion

	int s;
	for (int i = 0; i < LEN; i++) {
		s = b[i]+c[i];
		b[i] = a[i]+d[i];
		a[i] = s*e[i];
	}
}

void s2251()
{
//	scalar and array expansion
//	scalar expansion

	int s = (int)0.0;
	for (int i = 0; i < LEN; i++) {
		a[i] = s*e[i];
		s = b[i]+c[i];
		b[i] = a[i]+d[i];
	}
}

void s3251()
{
//	scalar and array expansion
//	scalar expansion

	for (int i = 0; i < LEN-1; i++){
		a[i+1] = b[i]+c[i];
		b[i]   = c[i]*e[i];
		d[i]   = a[i]*e[i];
	}
}


void s252()
{
//	scalar and array expansion
//	loop with ambiguous scalar temporary

	int t = 0, s;
	for (int i = 0; i < LEN; i++) {
		s = b[i] * c[i];
		a[i] = s + t;
		t = s;
	}
}

void s253()
{
//	scalar and array expansion
//	scalar expansio assigned under if

	int s;
	for (int i = 0; i < LEN; i++) {
		if (a[i] > b[i]) {
			s = a[i] - b[i] * d[i];
			c[i] += s;
			a[i] = s;
		}
	}
}

void s254()
{
//	scalar and array expansion
//	carry around variable

	int x = b[LEN-1];
	for (int i = 0; i < LEN; i++) {
		a[i] = (b[i] + x) * (int).5;
		x = b[i];
	}
}

void s255()
{
//	scalar and array expansion
//	carry around variables, 2 levels

	int x = b[LEN-1];
	int y = b[LEN-2];
	for (int i = 0; i < LEN; i++) {
		a[i] = (b[i] + x + y) * (int).333;
		y = x;
		x = b[i];
	}
}

void s256()
{
//	scalar and array expansion
//	array expansion

	for (int i = 0; i < LEN2; i++) {
		for (int j = 1; j < LEN2; j++) {
			a[j] = (int)1.0 - a[j - 1];
			cc[j][i] = a[j] + bb[j][i]*d[j];
		}
	}
}

void s257()
{
//	scalar and array expansion
//	array expansion

	for (int i = 1; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			a[i] = aa[j][i] - a[i-1];
			aa[j][i] = a[i] + bb[j][i];
		}
	}
}

void s258()
{
//	scalar and array expansion
//	wrap-around scalar under an if

	int s = 0.;
	for (int i = 0; i < LEN2; ++i) {
		if (a[i] > 0.) {
			s = d[i] * d[i];
		}
		b[i] = s * c[i] + d[i];
		e[i] = (s + (int)1.) * aa[0][i];
	}
}

void s261()
{
//	scalar and array expansion
//	wrap-around scalar under an if

	int t;
	for (int i = 1; i < LEN; ++i) {
		t = a[i] + b[i];
		a[i] = t + c[i-1];
		t = c[i] * d[i];
		c[i] = t;
	}
}

void s271()
{
//	control flow
//	loop with singularity handling

	for (int i = 0; i < LEN; i++) {
		if (b[i] > (int)0.) {
			a[i] += b[i] * c[i];
		}
	}
}

void s272(int t)
{
//	control flow
//	loop with independent conditional

	for (int i = 0; i < LEN; i++) {
		if (e[i] >= t) {
			a[i] += c[i] * d[i];
			b[i] += c[i] * c[i];
		}
	}
}

void s273()
{
//	control flow
//	simple loop with dependent conditional

	for (int i = 0; i < LEN; i++) {
		a[i] += d[i] * e[i];
		if (a[i] < (int)0.)
			b[i] += d[i] * e[i];
		c[i] += a[i] * d[i];
	}
}

void s274()
{
//	control flow
//	complex loop with dependent conditional

	for (int i = 0; i < LEN; i++) {
		a[i] = c[i] + e[i] * d[i];
		if (a[i] > (int)0.) {
			b[i] = a[i] + b[i];
		} else {
			a[i] = d[i] * e[i];
		}
	}
}

void s275()
{

//	control flow
//	if around inner loop, interchanging needed

	for (int i = 0; i < LEN2; i++) {
		if (aa[0][i] > (int)0.) {
			for (int j = 1; j < LEN2; j++) {
				aa[j][i] = aa[j-1][i] + bb[j][i] * cc[j][i];
			}
		}
	}
}

void s2275()
{
//	loop distribution is needed to be able to interchange

	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			aa[j][i] = aa[j][i] + bb[j][i] * cc[j][i];
		}
		a[i] = b[i] + c[i] * d[i];
	}
}

void s276()
{
//	control flow
//	if test using loop index

	int mid = (LEN/2);
	for (int i = 0; i < LEN; i++) {
		if (i+1 < mid) {
			a[i] += b[i] * c[i];
		} else {
			a[i] += b[i] * d[i];
		}
	}
}

void s277()
{
//	control flow
//	test for dependences arising from guard variable computation.

	for (int i = 0; i < LEN-1; i++) {
		if (a[i] >= (int)0.) {
			goto L20;
		}
		if (b[i] >= (int)0.) {
			goto L30;
		}
		a[i] += c[i] * d[i];
L30:
		b[i+1] = c[i] + d[i] * e[i];
L20:
		;
	}
}

void s278()
{
//	control flow
//	if/goto to block if-then-else

	for (int i = 0; i < LEN; i++) {
		if (a[i] > (int)0.) {
			goto L20;
		}
		b[i] = -b[i] + d[i] * e[i];
		goto L30;
L20:
		c[i] = -c[i] + d[i] * e[i];
L30:
		a[i] = b[i] + c[i] * d[i];
	}
}

void s279()
{
//	control flow
//	vector if/gotos

	for (int i = 0; i < LEN; i++) {
		if (a[i] > (int)0.) {
			goto L20;
		}
		b[i] = -b[i] + d[i] * d[i];
		if (b[i] <= a[i]) {
			goto L30;
		}
		c[i] += d[i] * e[i];
		goto L30;
L20:
		c[i] = -c[i] + e[i] * e[i];
L30:
		a[i] = b[i] + c[i] * d[i];
	}
}

void s1279()
{
//	control flow
//	vector if/gotos

	for (int i = 0; i < LEN; i++) {
		if (a[i] < (int)0.) {
			if (b[i] > a[i]) {
				c[i] += d[i] * e[i];
			}
		}
	}
}

void s2710( int x)
{
//	control flow
//	scalar and vector ifs

	for (int i = 0; i < LEN; i++) {
		if (a[i] > b[i]) {
			a[i] += b[i] * d[i];
			if (LEN > 10) {
				c[i] += d[i] * d[i];
			} else {
				c[i] = d[i] * e[i] + (int)1.;
			}
		} else {
			b[i] = a[i] + e[i] * e[i];
			if (x > (int)0.) {
				c[i] = a[i] + d[i] * d[i];
			} else {
				c[i] += e[i] * e[i];
			}
		}
	}
}

void s2711()
{
//	control flow
//	semantic if removal

	for (int i = 0; i < LEN; i++) {
		if (b[i] != (int)0.0) {
			a[i] += b[i] * c[i];
		}
	}
}

void s2712()
{
//	control flow
//	if to elemental min

	for (int i = 0; i < LEN; i++) {
		if (a[i] > b[i]) {
			a[i] += b[i] * c[i];
		}
	}
}

void s281()
{
//	crossing thresholds
//	index set splitting
//	reverse data access

	int x;
	for (int i = 0; i < LEN; i++) {
		x = a[LEN-i-1] + b[i] * c[i];
		a[i] = x-(int)1.0;
		b[i] = x;
	}
}

void s1281()
{
//	crossing thresholds
//	index set splitting
//	reverse data access

	int x;
	for (int i = 0; i < LEN; i++) {
		x = b[i]*c[i]+a[i]*d[i]+e[i];
		a[i] = x-(int)1.0;
		b[i] = x;
	}
}

void s291()
{
//	loop peeling
//	wrap around variable, 1 level

	int im1 = LEN-1;
	for (int i = 0; i < LEN; i++) {
		a[i] = (b[i] + b[im1]) * (int).5;
		im1 = i;
	}
}

void s292()
{
//	loop peeling
//	wrap around variable, 2 levels
//	similar to S291

	int im1 = LEN-1;
	int im2 = LEN-2;
	for (int i = 0; i < LEN; i++) {
		a[i] = (b[i] + b[im1] + b[im2]) * (int).333;
		im2 = im1;
		im1 = i;
	}
}

void s293()
{
//	loop peeling
//	a(i)=a(0) with actual dependence cycle, loop is vectorizable

	for (int i = 0; i < LEN; i++) {
		a[i] = a[0];
	}
}

void s2101()
{
//	diagonals
//	main diagonal calculation
//	jump in data access

	for (int i = 0; i < LEN2; i++) {
		aa[i][i] += bb[i][i] * cc[i][i];
	}
}

void s2102()
{
//	diagonals
//	identity matrix, best results vectorize both inner and outer loops

	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			aa[j][i] = (int)0.;
		}
		aa[i][i] = (int)1.;
	}
}
void s2111()
{
//	wavefronts, it will make jump in data access

	for (int j = 1; j < LEN2; j++) {
		for (int i = 1; i < LEN2; i++) {
			aa[j][i] = aa[j][i-1] + aa[j-1][i];
		}
	}
}

void s312()
{
//	reductions
//	product reduction

	int prod;
		prod = (int)1.;
		for (int i = 0; i < LEN; i++) {
			prod *= a[i];
		}
}

void s313()
{
//	reductions
//	dot product

	int dot = (int)0.;
	for (int i = 0; i < LEN; i++) {
		dot += a[i] * b[i];
	}
}
void s314()
{
//	reductions
//	if to max reduction

	int x = a[0];
	for (int i = 0; i < LEN; i++) {
		if (a[i] > x) {
			x = a[i];
		}
	}
}

void s315()
{
//	reductions
//	if to max with index reductio 1 dimension

	for (int i = 0; i < LEN; i++)
		a[i] = (i * 7) % LEN;

	int x = a[0];
	int index = 0;
	for (int i = 0; i < LEN; ++i) {
		if (a[i] > x) {
			x = a[i];
			index = i;
		}
	}
	int chksum = x + (int) index;
}

void s316()
{
//	reductions
//	if to min reduction

	int x = a[0];
	for (int i = 1; i < LEN; ++i) {
		if (a[i] < x) {
			x = a[i];
		}
	}
}

void s317()
{
//	reductions
//	product reductio vectorize with
//	1. scalar expansion of factor, and product reduction
//	2. closed form solution: q = factor**n

	int q = (int)1.;
	for (int i = 0; i < LEN/2; i++) {
		q *= (int).99;
	}
}

void s318( int inc)
{
//	reductions
//	isamax, max absolute value, increments not equal to 1

	int k = 0;
	int index = 0;
	int max = abs(a[0]);
	k += inc;
	for (int i = 1; i < LEN; i++) {
		if (abs(a[k]) <= max) {
			goto L5;
		}
		index = i;
			max = abs(a[k]);
L5:
			k += inc;
	}
	int chksum = max + (int) index;
}

void s319()
{
//	reductions
//	coupled reductions

	int sum = 0.;
	for (int i = 0; i < LEN; i++) {
		a[i] = c[i] + d[i];
		sum += a[i];
		b[i] = c[i] + e[i];
		sum += b[i];
	}
}

void s3110()
{
//	reductions
//	if to max with index reductio 2 dimensions
//	similar to S315

	int max = aa[(0)][0];
	int xindex = 0;
	int yindex = 0;
	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			if (aa[i][j] > max) {
				max = aa[i][j];
				xindex = i;
				yindex = j;
			}
		}
	}
	int chksum = max + (int) xindex + (int) yindex;
}

void s13110()
{
//	reductions
//	if to max with index reductio 2 dimensions

	int max = aa[(0)][0];
	int xindex = 0;
	int yindex = 0;
	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			if (aa[i][j] > max) {
				max = aa[i][j];
			}
		}
	}
	int chksum = max + (int) xindex + (int) yindex;
}

void s3111()
{
//	reductions
//	conditional sum reduction

	int sum = 0.;
	for (int i = 0; i < LEN; i++) {
		if (a[i] > 0) {
			sum += a[i];
		}
	}
}

void s3112()
{
//	reductions
//	sum reduction saving running sums

	int sum = 0;
	for (int i = 0; i < LEN; i++) {
		sum += a[i];
		b[i] = sum;
	}
}

void s3113()
{
//	reductions
//	maximum of absolute value

	int max = abs(a[0]);
	for (int i = 0; i < LEN; i++) {
		if ((abs(a[i])) > max) {
			max = abs(a[i]);
		}
	}
}

void s321()
{
//	recurrences
//	first order linear recurrence

	for (int i = 1; i < LEN; i++) {
		a[i] += a[i-1] * b[i];
	}
}

void s322()
{
//	recurrences
//	second order linear recurrence

	for (int i = 2; i < LEN; i++) {
		a[i] = a[i] + a[i - 1] * b[i] + a[i - 2] * c[i];
	}
}

void s323()
{
//	recurrences
//	coupled recurrence

	for (int i = 1; i < LEN; i++) {
		a[i] = b[i-1] + c[i] * d[i];
		b[i] = a[i] + c[i] * e[i];
	}
}

void s331()
{
//	search loops
//	if to last-1

	int j = -1;
	for (int i = 0; i < LEN; i++) {
		if (a[i] < 0) {
			j = i;
		}
	}
	int chksum = j;
}

void s332( int t)
{
//	search loops
//	first value greater than threshoLEN

	int index = -2;
	int value = -1.;
	for (int i = 0; i < LEN; i++) {
		if (a[i] > t) {
			index = i;
			value = a[i];
			goto L20;
		}
	}
L20:
	int chksum = value + (int) index;
}

void s341()
{
//	packing
//	pack positive values
//	not vectorizable, value of j in unknown at each iteration

	int j = -1;
	for (int i = 0; i < LEN; i++) {
		if (b[i] > (int)0.) {
			j++;
			a[j] = b[i];
		}
	}
}

void s342()
{
//	packing
//	unpacking
//	not vectorizable, value of j in unknown at each iteration

	int j = -1;
	for (int i = 0; i < LEN; i++) {
		if (a[i] > (int)0.) {
			j++;
			a[i] = b[j];
		}
	}
}

void s343()
{
//	packing
//	pack 2-d array into one dimension
//	not vectorizable, value of k in unknown at each iteration

	int k = -1;
	for (int i = 0; i < LEN2; i++) {
		for (int j = 0; j < LEN2; j++) {
			if (bb[j][i] > (int)0.) {
				k++;
				array[k] = aa[j][i];
			}
		}
	}
}

void s351()
{
//	loop rerolling
//	unrolled saxpy

	int alpha = c[0];
	for (int i = 0; i < LEN; i += 5) {
		a[i] += alpha * b[i];
		a[i + 1] += alpha * b[i + 1];
		a[i + 2] += alpha * b[i + 2];
		a[i + 3] += alpha * b[i + 3];
		a[i + 4] += alpha * b[i + 4];
	}
}

void s1351()
{
//	induction pointer recognition

	int* __restrict__ A = a;
	int* __restrict__ B = b;
	int* __restrict__ C = c;
	for (int i = 0; i < LEN; i++) {
		*A = *B+*C;
		A++;
		B++;
		C++;
	}
}

void s352()
{
//	loop rerolling
//	unrolled dot product

	int dot = (int)0.;
	for (int i = 0; i < LEN; i += 5) {
		dot = dot + a[i] * b[i] + a[i + 1] * b[i + 1] + a[i + 2]
			* b[i + 2] + a[i + 3] * b[i + 3] + a[i + 4] * b[i + 4];
	}
}

void s353(int* __restrict__ ip)
{
//	loop rerolling
//	unrolled sparse saxpy
//	gather is required

	int alpha = c[0];
	for (int i = 0; i < LEN; i += 5) {
		a[i] += alpha * b[ip[i]];
		a[i + 1] += alpha * b[ip[i + 1]];
		a[i + 2] += alpha * b[ip[i + 2]];
		a[i + 3] += alpha * b[ip[i + 3]];
		a[i + 4] += alpha * b[ip[i + 4]];
	}
}

void s421()
{
//	storage classes and equivalencing
//	equivalence- no overlap

	set1d(xx, 1, 1);

	yy = xx;
	for (int i = 0; i < LEN - 1; i++) {
		xx[i] = yy[i+1] + a[i];
	}
}

void s1421()
{
//	storage classes and equivalencing
//	equivalence- no overlap

	set1d(xx, 1., 1);

	xx = &b[LEN/2];
	for (int i = 0; i < LEN/2; i++) {
		b[i] = xx[i] + a[i];
	}
	return 0;
}

void s422()
{
//	storage classes and equivalencing
//	common and equivalence statement
//	anti-dependence, threshold of 4

	xx = array + 4;
	set1d(xx, 0, 1);

	for (int i = 0; i < LEN; i++) {
		xx[i] = array[i + 8] + a[i];
	}
}

void s423()
{
//	storage classes and equivalencing
//	common and equivalenced variables - with anti-dependence

	int vl = 64;
	xx = array+vl;
	set1d(xx, 1, 1);
	for (int i = 0; i < LEN - 1; i++) {
		array[i+1] = xx[i] + a[i];
	}
}

void s424()
{
//	storage classes and equivalencing
//	common and equivalenced variables - overlap
//	vectorizeable in strips of 64 or less

	int vl = 63;
	xx = array + vl;
	set1d(xx, 0., 1);

	for (int i = 0; i < LEN - 1; i++) {
		xx[i+1] = array[i] + a[i];
	}
}

void s431()
{
//	parameters
//	parameter statement

	int k1=1;
	int k2=2;
	int k=2*k1-k2;

	for (int i = 0; i < LEN; i++) {
		a[i] = a[i+k] + b[i];
	}
}

void s441()
{
//	non-logical if's
//	arithmetic if

	for (int i = 0; i < LEN; i++) {
		if (d[i] < (int)0.) {
			a[i] += b[i] * c[i];
		} else if (d[i] == (int)0.) {
			a[i] += b[i] * b[i];
		} else {
			a[i] += c[i] * c[i];
		}
	}
}

void s442()
{
//	non-logical if's
//	computed goto

	for (int i = 0; i < LEN; i++) {
		switch (indx[i]) {
			case 1:  goto L15;
			case 2:  goto L20;
			case 3:  goto L30;
			case 4:  goto L40;
		}
L15:
		a[i] += b[i] * b[i];
		goto L50;
L20:
		a[i] += c[i] * c[i];
		goto L50;
L30:
		a[i] += d[i] * d[i];
		goto L50;
L40:
		a[i] += e[i] * e[i];
L50:
		;
	}
}

void s443()
{
//	non-logical if's
//	arithmetic if

	for (int i = 0; i < LEN; i++) {
		if (d[i] <= (int)0.) {
			goto L20;
		} else {
			goto L30;
		}
L20:
		a[i] += b[i] * c[i];
		goto L50;
L30:
		a[i] += b[i] * b[i];
L50:
		;
	}
}

void s451()
{
//	intrinsic functions
//	intrinsics

	for (int i = 0; i < LEN; i++) {
		a[i] = sinf(b[i]) + cosf(c[i]);
	}
}

void s452()
{
//	intrinsic functions
//	seq function

	for (int i = 0; i < LEN; i++) {
		a[i] = b[i] + c[i] * (int) (i+1);
	}
}

void s453()
{
//	induction varibale recognition

	int s = 0;
	for (int i = 0; i < LEN; i++) {
		s += (int)2;
		a[i] = s * b[i];
	}
}

void s471()
{
//	call statements

	int m = LEN;
	set1d(x, 0, 1);
	for (int i = 0; i < m; i++) {
		x[i] = b[i] + d[i] * d[i];
		b[i] = c[i] + d[i] * e[i];
	}
}

void s481()
{
//	non-local goto's
//	stop statement

	for (int i = 0; i < LEN; i++) {
		if (d[i] < 0) {
			exit (0);
		}
		a[i] += b[i] * c[i];
	}
}

void s482()
{
//	non-local goto's
//	other loop exit with code before exit

	for (int i = 0; i < LEN; i++) {
		a[i] += b[i] * c[i];
		if (c[i] > b[i]) break;
	}
}

void s491(int* __restrict__ ip)
{
//	vector semantics
//	indirect addressing on lhs, store in sequence
//	scatter is required

	for (int i = 0; i < LEN; i++) {
		a[ip[i]] = b[i] + c[i] * d[i];
	}
}

void s4112(int* __restrict__ ip, int s)
{
//	indirect addressing
//	sparse saxpy
//	gather is required

	for (int i = 0; i < LEN; i++) {
		a[i] += b[ip[i]] * s;
	}
}

void s4113(int* __restrict__ ip)
{
//	indirect addressing
//	indirect addressing on rhs and lhs
//	gather and scatter is required

	for (int i = 0; i < LEN; i++) {
		a[ip[i]] = b[ip[i]] + c[i];
	}
}

void s4114(int* ip, int n1)
{
//	indirect addressing
//	mix indirect addressing with variable lower and upper bounds
//	gather is required

	int k;
	for (int i = n1-1; i < LEN; i++) {
		k = ip[i];
		a[i] = b[i] + c[LEN-k+1-2] * d[i];
		k += 5;
	}
}

void s4115(int* __restrict__ ip)
{
//	indirect addressing
//	sparse dot product
//	gather is required

	int sum = 0.;
	for (int i = 0; i < LEN; i++) {
		sum += a[i] * b[ip[i]];
	}
}

void s4116(int* __restrict__ ip, int j, int inc)
{
//	indirect addressing
//	more complicated sparse sdot
//	gather is required

	int off;
	int sum = 0.;
	for (int i = 0; i < LEN2-1; i++) {
		off = inc + i;
		sum += a[off] * aa[j-1][ip[i]];
	}
}

void s4117()
{
//	indirect addressing
//	seq function

	for (int i = 0; i < LEN; i++) {
		a[i] = b[i] + c[i/2] * d[i];
	}
}

void s4121()
{
//	statement functions
//	elementwise multiplication

	for (int i = 0; i < LEN; i++) {
		a[i] += mult(b[i],c[i]);
	}
}

void vif()
{
//	control loops
//	vector if

	for (int i = 0; i < LEN; i++) {
		if (b[i] > 0) {
			a[i] = b[i];
		}
	}
}

void vpv()
{
//	control loops
//	vector plus vector

	for (int i = 0; i < LEN; i++) {
		a[i] += b[i];
	}
}

void vtv()
{
//	control loops
//	vector times vector

	for (int i = 0; i < LEN; i++) {
		a[i] *= b[i];
	}
}

void vpvtv()
{
//	control loops
//	vector plus vector times vector

	for (int i = 0; i < LEN; i++) {
		a[i] += b[i] * c[i];
	}
}

void vpvts( int s)
{
//	control loops
//	vector plus vector times scalar

	for (int i = 0; i < LEN; i++) {
		a[i] += b[i] * s;
	}
}

void vpvpv()
{
//	control loops
//	vector plus vector plus vector

	for (int i = 0; i < LEN; i++) {
		a[i] += b[i] + c[i];
	}
}

void vtvtv()
{
//	control loops
//	vector times vector times vector

	for (int i = 0; i < LEN; i++) {
		a[i] = a[i] * b[i] * c[i];
	}
}

void vsumr()
{
//	control loops
//	vector sum reduction

	int sum = 0.;
	for (int i = 0; i < LEN; i++) {
		sum += a[i];
	}
}

void vdotr()
{
//	control loops
//	vector dot product reduction

	int dot = 0.;
	for (int i = 0; i < LEN; i++) {
		dot += a[i] * b[i];
	}
}

void vbor()
{
//	control loops
//	basic operations rates, isolate arithmetic from memory traffic
//	all combinations of three, 59 flops for 6 loads and 1 store.

	int a1, b1, c1, d1, e1, f1;
	for (int i = 0; i < LEN2; i++) {
		a1 = a[i];
		b1 = b[i];
		c1 = c[i];
		d1 = d[i];
		e1 = e[i];
		f1 = aa[0][i];
		a1 = a1 * b1 * c1 + a1 * b1 * d1 + a1 * b1 * e1 + a1 * b1 * f1 +
			a1 * c1 * d1 + a1 * c1 * e1 + a1 * c1 * f1 + a1 * d1 * e1
			+ a1 * d1 * f1 + a1 * e1 * f1;
		b1 = b1 * c1 * d1 + b1 * c1 * e1 + b1 * c1 * f1 + b1 * d1 * e1 +
			b1 * d1 * f1 + b1 * e1 * f1;
		c1 = c1 * d1 * e1 + c1 * d1 * f1 + c1 * e1 * f1;
		d1 = d1 * e1 * f1;
		x[i] = a1 * b1 * c1 * d1;
	}
}

void set(int* ip) {
	for (int i = 0; i < LEN; i = i+5) {
		ip[i]	= (i+4);
		ip[i+1] = (i+2);
		ip[i+2] = (i);
		ip[i+3] = (i+3);
		ip[i+4] = (i+1);
	}

	for (int i = 0; i < LEN; i++) {
		indx[i] = (i+1) % 4+1;
	}
}
