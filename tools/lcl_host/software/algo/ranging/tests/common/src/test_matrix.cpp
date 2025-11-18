/***********************************************************************************/
/*!
 *  @brief      
 *  @file       test_matrix.cpp
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
#include <common/include/VectorTypes.h>

#include <testsupport/include/test_utils.h>
#include "unity.h"

static const float EPSILON = 1.0e-5f;

/* ------------------------------------------------------------------------------ */

//static void test_IgnoredTest(void) {
//	TEST_IGNORE_MESSAGE("This Test Was Ignored On Purpose");
//}

static void test_scalar() {

	Matrix<float> A(4, 4);
	Matrix<float> B(4, 4);
	Matrix<float> C(4, 4);

	TEST_ASSERT_EQUAL(true, A.iszero());
	TEST_ASSERT_EQUAL(true, B.iszero());
	TEST_ASSERT_EQUAL(true, C.iszero());

	C = A * B;
	TEST_ASSERT_EQUAL(true, C.iszero());

	// std::cout << "A = " << A;
	// std::cout << "B = " << B;
	// std::cout << "C = " << C;

	C = C * 3.0;
	TEST_ASSERT_EQUAL(true, C.iszero());

	// std::cout << "C = " << C;

	C += 27.27;

	for (msize_t i = 0; i < 4; i++)
		for (msize_t j = 0; j < 4; j++)
			TEST_ASSERT_EQUAL(27.27f, C(i, j));

	// std::cout << "C = " << C;

	C -= 20.27;

	// std::cout << "C = " << C;
	for (msize_t i = 0; i < 4; i++)
		for (msize_t j = 0; j < 4; j++)
			TEST_ASSERT_EQUAL(7.0f, C(i, j));

}

static void test_add_sub() {

	Matrix<float> A(4, 4);
	Matrix<float> B(4, 4);

	TEST_ASSERT_EQUAL(true, A.iszero());
	TEST_ASSERT_EQUAL(true, B.iszero());

	A += 1;
	B += 2;

	// std::cout << "A = " << A;
	// std::cout << "B = " << B;

	Matrix<float> C(4, 4);

	C = A + B;
	// std::cout << "C = " << C;

	for (msize_t i = 0; i < 4; i++)
		for (msize_t j = 0; j < 4; j++)
			TEST_ASSERT_EQUAL(3.0, C(i, j));

	C = A - B;
	// std::cout << "C = " << C;

	for (msize_t i = 0; i < 4; i++)
		for (msize_t j = 0; j < 4; j++)
			TEST_ASSERT_EQUAL(-1.0, C(i, j));
}

static void test_mult() {

	float data_a[] = { 1, 2, 3, 4, 5, 6 };
	float data_b[] = { 7, 8, 9, 10, 11, 12 };
	Matrix<float> A(2, 3, data_a);
	Matrix<float> B(3, 2, data_b);

	Matrix<float> C(2, 2);

	float data_result[] = { 58, 64, 139, 154 };
	Matrix<float> Result(2, 2, data_result);

	// std::cout << "A = " << A;
	// std::cout << "B = " << B;

	C = A * B;
	// std::cout << "C = " << C;

	TEST_ASSERT_EQUAL(true, Result == C);
}

static void test_identity() {

	Matrix<float> I(7, 7);

	/* fill identity */
	I.identity();

	TEST_ASSERT_EQUAL(true, I.isidentity());

	float data_ref[] = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
			0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, };
	MatrixF Ref(7, 7, data_ref);

	TEST_ASSERT_EQUAL(true, Ref == I);


	Matrix<float> B(5, 3, MATRIX_CLEAR_DATA);
	TEST_ASSERT_EQUAL(false, B.isidentity());

	Matrix<float> A(3, 3, MATRIX_CLEAR_DATA);
	TEST_ASSERT_EQUAL(false, A.isidentity());

	A.identity();
	A(0,0) = 2.0f;
	TEST_ASSERT_EQUAL(false, A.isidentity());
	A.identity();
	A(0,2) = 99.99f;
	TEST_ASSERT_EQUAL(false, A.isidentity());



}

static void test_scalar2() {

	Matrix<float> A(2, 2);
	Matrix<float> B(2, 2);

	A.identity();
	B = A;

	TEST_ASSERT_EQUAL(true, A == B);

	// std::cout << "A = " << A;
	// std::cout << "B = " << B;

	Matrix<float> C(2, 2);

	C = A + B;

	// std::cout << "A = " << A;
	// std::cout << "B = " << B;
	// std::cout << "C = " << C;

	C += C;

	// std::cout << "C = " << C;

	Matrix<float> cmp(2, 2);
	cmp.identity();
	cmp *= 4.0;

	// std::cout << "Test : " << ((C == cmp) ? "Ok" : "Failed") << std::endl;

	TEST_ASSERT_EQUAL(true, cmp == C);

}

static void test_transpose() {

	float data_in[] = { 3, 6, 17, 15, 13, 15, 6, 12, 9, 1, 2, 7, 10, 19, 3, 6, 0, 6, 12, 16, 11, 8, 7, 9, 2, };
	MatrixF A(5, 5, data_in);

	float data_ref[] = { 3, 15, 2, 6, 11, 6, 6, 7, 0, 8, 17, 12, 10, 6, 7, 15, 9, 19, 12, 9, 13, 1, 3, 16, 2, };
	MatrixF Ref(5, 5, data_ref);

	// std::cout << "A = " << A;
	A = A.transpose();
	// std::cout << "A = " << A;

	TEST_ASSERT_EQUAL(true, Ref == A);

}

static void test_transpose_cmpl() {

	ComplexF data_in[] = {ComplexF(3, 6), ComplexF(17, 15), ComplexF(13, 15), ComplexF(6, 12), };

	MatrixCmpl A(2, 2, data_in);

	ComplexF data_ref[] = {ComplexF(3, 6), ComplexF(13, 15), ComplexF(17, 15), ComplexF(6, 12), };
	MatrixCmpl Ref(2, 2, data_ref);

	// std::cout << "A = " << A;
	A = A.transpose();
	// std::cout << "A = " << A;

	TEST_ASSERT_EQUAL(true, Ref == A);

}

static void test_ctranspose_cmpl() {

	ComplexF data_in[] = {ComplexF(3, 6), ComplexF(17, 15), ComplexF(13, 15), ComplexF(6, 12), };

	MatrixCmpl A(2, 2, data_in);

	ComplexF data_ref[] = {ComplexF(3, -6), ComplexF(13, -15), ComplexF(17, -15), ComplexF(6, -12), };
	MatrixCmpl Ref(2, 2, data_ref);

	// std::cout << "A = " << A;
	A = A.ctranspose();
	// std::cout << "A = " << A;

	TEST_ASSERT_EQUAL(true, Ref == A);

}

static void test_complex1() {

	ComplexF first(3.0, 2.0);
	ComplexF second(1.0, 4.0);
	ComplexF result;

	result = first * second;

	// std::cout << "complex mult: " << result << std::endl;

	TEST_ASSERT_EQUAL(-5, result.real());
	TEST_ASSERT_EQUAL(14, result.imag());

}
static void test_complex2() {

	// std::cout << "sizeof(ComplexF) = " << sizeof(ComplexF) << std::endl;
	TEST_ASSERT_EQUAL(2 * sizeof(float), sizeof(ComplexF));

	Matrix<ComplexF> B(2, 2);

	/* check floats of complex are in consecutive array */
	//	std::cout << "addr " << std::hex << &B(0, 0) << std::endl;
	//	std::cout << "addr " << std::hex << &B(0, 1) << std::endl;
	//	std::cout << "addr " << std::hex << &B(1, 0) << std::endl;
	//	std::cout << "addr " << std::hex << &B(1, 1) << std::endl;
	uintptr_t addr = (uintptr_t) &B(0, 0);
	TEST_ASSERT_EQUAL(addr + 8, (uintptr_t)&B(0, 1));
	TEST_ASSERT_EQUAL(addr + 16, (uintptr_t)&B(1, 0));
	TEST_ASSERT_EQUAL(addr + 24, (uintptr_t)&B(1, 1));

	/* matrix of complex */
	Matrix<ComplexF> A(2, 2);

	A(0, 0) = ComplexF(3, 2);
	B(0, 0) = ComplexF(1, 4);

	// std::cout << "A " << A;
	// std::cout << "B " << B;

	A *= B;

	// std::cout << "A " << A;

	TEST_ASSERT_EQUAL(-5, A(0, 0).real());
	TEST_ASSERT_EQUAL(14, A(0, 0).imag());

	A(0, 0) = 0;

	// std::cout << "A " << A;
	TEST_ASSERT_EQUAL(0, A(0, 0).real());
	TEST_ASSERT_EQUAL(0, A(0, 0).imag());

	ComplexF Z(2, 3);
	Z *= Z;
	// std::cout << "Z " << Z;
	TEST_ASSERT_EQUAL(true, Z == ComplexF(-5, 12));

	Z *= 2;
	// std::cout << "Z " << Z;
	TEST_ASSERT_EQUAL(true, Z == ComplexF(-10, 24));

//	static const msize_t m = 10;
//	Matrix<ComplexF> Y(m, m);
//	for (msize_t i = 0; i < m; i++)
//		for (msize_t j = 0; j < m; j++)
//			Y(i, j) = ComplexF(-10 + random() % 20, -10 + random() % 20);
//	// std::cout << "Y " << Y;
//
//	Y *= Y;
//	std::cout << "Y " << Y;

}


static void test_resize() {

	float data_in[] = {
			 3, 6, 17, 15, 13,
			15, 6, 12,  9,  1,
			 2, 7, 10, 19,  3,
			 6, 0,  6, 12, 16,
			11, 8,  7,  9,  2, };
	MatrixF A(5, 5, data_in);

	// std::cout << "A = " << A << std::endl;

	A.resize(Span(1, 2), Span(0, 2));

	// std::cout << "A = " << A << std::endl;

	float data_ref[] = {
			15, 6, 12,
			2, 7, 10,
	};

	/* make new sub matrix of 2 rows and 3 cols */
	MatrixF Ref(2, 3, data_ref);

	TEST_ASSERT_EQUAL(true, A == Ref);
}

static void test_sub() {

	float data_in[] = {
			 3, 6, 17, 15, 13,
			15, 6, 12,  9,  1,
			 2, 7, 10, 19,  3,
			 6, 0,  6, 12, 16,
			11, 8,  7,  9,  2, };
	MatrixF A(5, 5, data_in);

	// std::cout << "A = " << A << std::endl;

	MatrixF R(A.sub(Span(1, 2), Span(0, 2)));

	// std::cout << "A = " << A << std::endl;

	float data_ref[] = {
			15, 6, 12,
			2, 7, 10,
	};

	/* make new sub matrix of 2 rows and 3 cols */
	MatrixF Ref(2, 3, data_ref);

	TEST_ASSERT_EQUAL(true, R == Ref);
}

static void test_zero_part() {

	float data_in[] = {
			 3, 6, 17, 15, 13,
			15, 6, 12,  9,  1,
			 2, 7, 10, 19,  3,
			 6, 0,  6, 12, 16,
			11, 8,  7,  9,  2, };
	MatrixF A(5, 5, data_in);

	A.zero();

	MatrixF Ref(5, 5);

	TEST_ASSERT_TRUE( A == Ref);
}

static void test_zero() {

	float data_in[] = {
			 3, 6, 17, 15, 13,
			15, 6, 12,  9,  1,
			 2, 7, 10, 19,  3,
			 6, 0,  6, 12, 16,
			11, 8,  7,  9,  2, };
	MatrixF A(5, 5, data_in);

	A.zero(Span(1, 2), Span(0, 2));

	float data_ref[] = {
			 3, 6, 17, 15, 13,
			 0, 0,  0,  9,  1,
			 0, 0,  0, 19,  3,
			 6, 0,  6, 12, 16,
			11, 8,  7,  9,  2, };
	MatrixF Ref(5, 5, data_ref);

	TEST_ASSERT_TRUE( A == Ref);
}

static void test_update() {

	Matrix<float> C(6, 6);

	C += 8.0;
	// std::cout << "C = " << C << std::endl;

	Matrix<float> up(2, 2);
	up += 1.0;

	C.update(Span(0, 1), Span(0, 1), up);
	// std::cout << "C = " << C << std::endl;

	C.update(Span(4, 5), Span(4, 5), up);
	// std::cout << "C = " << C << std::endl;

	float _ref[] = { 1, 1, 8, 8, 8, 8, 1, 1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 8, 8, 8, 8, 1, 1, };
	Matrix<float> ref(6, 6, _ref);

	TEST_ASSERT_EQUAL(true, C == ref);
}

static void test_copy() {

	float _ref[] = { 1, 1, 8, 8, 8, 8, 1, 1, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 8, 8, 8, 8, 1, 1, };
	Matrix<float> ref(6, 6, _ref);
	Matrix<float> C(ref);

	TEST_ASSERT_EQUAL(true, C == ref);
}

static void test_construct() {
	Matrix<float> ref(6, 6, MATRIX_CLEAR_DATA);
	Matrix<float> C(6, 6, nullptr);
	TEST_ASSERT_EQUAL(true, C == ref);
}

static void test_equal_operator() {
	Matrix<float> ref(6, 6, MATRIX_CLEAR_DATA);
	Matrix<float> C(6, 6, MATRIX_CLEAR_DATA);
	C(1,1) = 2.22f;
	TEST_ASSERT_EQUAL(false, C == ref);
}

static void test_iszero() {
	Matrix<float> C(6, 6, MATRIX_CLEAR_DATA);
	C.zero();
	TEST_ASSERT_EQUAL(true, C.iszero());
	C(1,1) = 2.22f;
	TEST_ASSERT_EQUAL(false, C.iszero());
}

static void test_allocator() {
	float_vector d_arr(4, 0.0f);
	TEST_ASSERT_EQUAL(4, d_arr.size());

	float_vector e_arr(0, 0.0f);
	TEST_ASSERT_EQUAL(0, e_arr.size());

	{
		VectorAllocator<float> v;
		auto p = v.allocate(2);
		TEST_ASSERT_NOT_EQUAL(p, nullptr);
		v.deallocate(p, 2);
	}
}


/**
 * Run the tests
 * @return number of test failures
 */
int test_runner_matrix() {

	UNITY_BEGIN();

	RUN_TEST(test_construct);
	RUN_TEST(test_scalar);
	RUN_TEST(test_scalar2);
	RUN_TEST(test_identity);
	RUN_TEST(test_mult);
	RUN_TEST(test_transpose);
	RUN_TEST(test_transpose_cmpl);
	RUN_TEST(test_ctranspose_cmpl);
	RUN_TEST(test_add_sub);

	RUN_TEST(test_complex1);
	RUN_TEST(test_complex2);

	RUN_TEST(test_resize);
	RUN_TEST(test_sub);
	RUN_TEST(test_zero);
	RUN_TEST(test_zero_part);

	RUN_TEST(test_update);

	RUN_TEST(test_copy);	
	RUN_TEST(test_equal_operator);
	RUN_TEST(test_iszero);

	RUN_TEST(test_allocator);

	return UNITY_END();
}


//End of File

