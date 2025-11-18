/***********************************************************************************/
/*!
 *  @brief      
 *  @file       test_functions.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <common/include/Matrix.h>
#include <common/include/Functions.h>

#include <testsupport/include/test_utils.h>
#include "unity.h"

static const float EPSILON = 1.0e-5f;

/* ------------------------------------------------------------------------------ */


static void test_span() {
	Span span(0, 5);

	// std::cout << span << std::endl;
	TEST_ASSERT_EQUAL(0, span.from());
	TEST_ASSERT_EQUAL(5, span.to());

	Span span2(span);

	// std::cout << span2 << std::endl;
	TEST_ASSERT_EQUAL(0, span2.from());
	TEST_ASSERT_EQUAL(5, span2.to());

	Span span3(span);
	// std::cout << span3 << std::endl;

	TEST_ASSERT_EQUAL(0, span3.from());
	TEST_ASSERT_EQUAL(5, span3.to());
}


static void test_eye() {

	Matrix<float> C(eye(5));
	// std::cout << "C = " << C << std::endl;

	Matrix<float> Iref(5, 5);
	Iref.identity();
	TEST_ASSERT_EQUAL(true, C == Iref);



	auto m = eyeComplex(5);
	TEST_ASSERT_EQUAL(true, m(0,0) == ComplexF(1.0f,0.0f));

	auto t = eyeComplex(5);
	auto t2 = array_transpose(t);
	TEST_ASSERT_EQUAL(true, t == t2);

}



static void test_hankel() {

	ComplexF data_c[] = {
			ComplexF(2.43811, 4.79434), ComplexF(4.83596, 4.03366), ComplexF(-0.0274149, 1.6688),
			ComplexF(3.30012, -3.36032), ComplexF(-4.23005, 3.88949), ComplexF(-2.51956, 1.49707),
			ComplexF(-2.70863, 1.2948), ComplexF(-1.83133, 2.0062),
			ComplexF(-2.68572, -1.71223), ComplexF(1.33072, -4.25839), };
	MatrixCmpl C(1, 10, data_c);


	//	MatrixCmpl C(1, 10);
	//	for (msize_t i = 0; i < 10; i++) {
	//		C(0, i) = ComplexF(randf(), randf());
	//	}

	// std::cout << "C = " << C << std::endl;

	/* calculate Hankel version */
	MatrixCmpl H(hankel(C));
	// std::cout << "D = " << H << std::endl;

	/* make sub matrix */
	H.resize(Span(0, 5), Span(0, 3));
	// std::cout << "H = " << H << std::endl;


	ComplexF data_ref[] = {
			ComplexF(2.43811, 4.79434), ComplexF(4.83596, 4.03366), ComplexF(-0.0274149, 1.6688),
			ComplexF(3.30012, -3.36032), ComplexF(4.83596, 4.03366), ComplexF(-0.0274149, 1.6688),
			ComplexF(3.30012, -3.36032), ComplexF(-4.23005, 3.88949), ComplexF(-0.0274149, 1.6688),
			ComplexF(3.30012, -3.36032), ComplexF(-4.23005, 3.88949), ComplexF(-2.51956, 1.49707),
			ComplexF(3.30012, -3.36032), ComplexF(-4.23005, 3.88949), ComplexF(-2.51956, 1.49707),
			ComplexF(-2.70863, 1.2948), ComplexF(-4.23005, 3.88949), ComplexF(-2.51956, 1.49707),
			ComplexF(-2.70863, 1.2948), ComplexF(-1.83133, 2.0062), ComplexF(-2.51956, 1.49707),
			ComplexF(-2.70863, 1.2948), ComplexF(-1.83133, 2.0062), ComplexF(-2.68572, -1.71223), };
	MatrixCmpl Ref(6, 4, data_ref);

	TEST_ASSERT_EQUAL(true, H == Ref);

}


static void test_size() {

	Matrix<float> C(3,9);

	Matrix<float> res(size(C));
	// std::cout << "res = " << res << std::endl;

	TEST_ASSERT_EQUAL(3 , res(0,0));
	TEST_ASSERT_EQUAL(9 , res(0,1));

	// std::cout << "C = " << size(C,0) << std::endl;
	// std::cout << "C = " << size(C,1) << std::endl;

	TEST_ASSERT_EQUAL(3 , size(C,0));
	TEST_ASSERT_EQUAL(9 , size(C,1));
}

static void test_norm() {

	ComplexF _input[] = { ComplexF(5.5234, 30.1438), ComplexF(5.2835, 7.0704), ComplexF(1.3868, -0.9535), };
	MatrixCmpl input(1, 3, _input);

	// std::cout << "input = " << input << std::endl;
	// std::cout << "norm(input) = " << norm(input) << std::endl;

	TEST_ASSERT_EQUAL_FLOAT(31.9358, norm(input));


	ComplexF v(1.3868, -0.9535);

	TEST_ASSERT_TRUE(std::abs(1.68297 - norm(v)) < EPSILON);

	// std::cout << "v = " << v << std::endl;
	// std::cout << "norm(v) = " << norm(v) << std::endl;
	v /= norm(v);

	ComplexF res(0.82402 + 1.13249e-06, -0.566521 + -3.80874e-05);


	TEST_ASSERT_EQUAL(true, norm(v - res) < EPSILON);

	// std::cout << "norm(v-res) = " << norm(v-res) << std::endl;

	ComplexF v2(1.3868, -0.9535);
	ComplexF v3 = (v2 / norm(v2));
	TEST_ASSERT_EQUAL(true, norm(v3 - res) < EPSILON);

}

static void test_angle() {

	ComplexF input(9.5527 , 5.1179);

	//	std::cout << "input = " << input << std::endl;
	//	std::cout << "angle(input) = " << angle(input) << std::endl;

	TEST_ASSERT_EQUAL_FLOAT(0.49184, angle(input));

	ComplexF input2(-13.3194, -6.3564);

	float ref = -2.69633;
	float a = angle(input2);

	// std::cout << "angle(input2) = " << a << std::endl;

	TEST_ASSERT_EQUAL(true, std::abs(ref - a) < EPSILON);
}

static void test_exp_complex() {


	ComplexF input(-13.3194, -6.3564);
	float a = angle(input);

	// std::cout << "input = " << input << std::endl;
	// std::cout << "angle = " << a << std::endl;

	ComplexF result;
	result = - expCmpl(a);

	// std::cout << "-expCmpl(angle) = " << result << std::endl;

	ComplexF ref(0.902496, 0.430697);
	// std::cout << "ref = " << ref << std::endl;

	ref -= result;
	// std::cout << "result - ref = " << result - ref << std::endl;

	TEST_ASSERT_EQUAL(true, std::abs(ref.real()) < EPSILON);
	TEST_ASSERT_EQUAL(true, std::abs(ref.imag()) < EPSILON);
}

static void test_conjugate() {

	ComplexF input(-13.3194, -6.3564);

	// std::cout << "input = " << input << std::endl;

	ComplexF conj;
	conj = conjugate(input);

	// std::cout << "conj = " << conj << std::endl;

	ComplexF ref(-13.3194, 6.3564);

	TEST_ASSERT_EQUAL(true, conj == ref);
}




static void test_diag() {

	float data_in[] = { 3, 6, 17, 15, 13, 15, 6, 12, 9, 1, 2, 7, 10, 19, 3, 6, 0, 6, 12, 16, 11, 8, 7, 9, 2, };
	MatrixF A(5, 5, data_in);

	float data_refB[] = { 3, 6, 10, 12, 2, };
	MatrixF RefB(1, 5, data_refB);

	float data_refC[] = { 6, 12, 19, 16, };
	MatrixF RefC(1, 4, data_refC);

	// std::cout << "A = " << A;

	MatrixF B(diag<float>(A));
	// std::cout << "B = " << B;

	MatrixF C(diag<float>(A, 1));
	// std::cout << "C = " << C;

	TEST_ASSERT_EQUAL(true, RefB == B);
	TEST_ASSERT_EQUAL(true, RefC == C);
}

static void test_pythag() {

	float a1 = 1.0;
	float a2 = 1.0;
	float z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(sqrtf(2), z);

	a1 = 0.0003456;
	a2 = 0.000891;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(0.000955678, z);

	a1 = 10;
	a2 = 0;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(10, z);

	a1 = 0;
	a2 = 11;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(11, z);

	a1 = 2;
	a2 = 4;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(4.47214, z);

	a1 = 4;
	a2 = 2;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(4.47214, z);

	a1 = -4;
	a2 = 2;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(4.47214, z);

	a1 = 4;
	a2 = -2;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(4.47214, z);

	a1 = -4;
	a2 = -2;
	z = pythag(a1, a2);
	// std::cout << "z = " << z << std::endl;
	TEST_ASSERT_EQUAL_FLOAT(4.47214, z);
}

static void test_squared() {

	float data_in[] = { 3, 6, 17, 15, 13 };
	MatrixF A(5, 1, data_in);

	//std::cout << "A = " << A;

	MatrixF B(A.transpose());
	//std::cout << "B = " << B;

	MatrixF C(A * A.transpose());

	//std::cout << "C = " << C;

	MatrixF D(squared(A));

	//std::cout << "D = " << D;

	TEST_ASSERT_EQUAL(true, C == D);

}

static void test_complex_sqrt() {

	// http://en.cppreference.com/w/cpp/numeric/complex/sqrt
	ComplexF in1(-1.0, 0);
	ComplexF ref1(0, 1.0);
	in1 = std::sqrt(in1);
	TEST_ASSERT_EQUAL(true, in1 == ref1);

	ComplexF in2(0.0, -1.0);
	ComplexF ref2(0.5 * sqrt(2.0), -0.5 * sqrt(2.0));
	ComplexF result = std::sqrt(in2);
	TEST_ASSERT_EQUAL(true, result == ref2);
}

/**
 * Run the tests
 * @return number of test failures
 */
int test_runner_functions() {

	UNITY_BEGIN();

	RUN_TEST(test_span);

	RUN_TEST(test_complex_sqrt);

	RUN_TEST(test_hankel);
	RUN_TEST(test_eye);
	RUN_TEST(test_size);

	RUN_TEST(test_norm);
	RUN_TEST(test_angle);

	RUN_TEST(test_conjugate);

	RUN_TEST(test_exp_complex);

	RUN_TEST(test_diag);
	RUN_TEST(test_pythag);

	RUN_TEST(test_squared);

	return UNITY_END();
}


//End of File

