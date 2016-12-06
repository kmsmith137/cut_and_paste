// g++ -std=c++11 -Wall -O3 -march=native -o test-linear-algebra-kernels test-linear-algebra-kernels.cpp

#include <cassert>
#include <iostream>

#include "../rng.hpp"
#include "simd_trimatrix.hpp"

using namespace std;


template<typename T, unsigned int S, unsigned int N>
void randomize_vector(std::mt19937 &rng, simd_ntuple<T,S,N> &v, vector<T> &buf)
{
    buf.resize(N*S);
    gaussian_rand(rng, &buf[0], N*S);
    v.loadu(&buf[0]);
}


template<typename T, unsigned int S, unsigned int N>
void randomize_triangular_matrix(std::mt19937 &rng, simd_trimatrix<T,S,N> &m, vector<T> &buf)
{
    int NN = (N*(N+1))/2;
    buf.resize(NN*S);

    for (int i = 0; i < N; i++) {
	int j = (i*(i+1)*S) / 2;
	gaussian_rand(rng, &buf[j], i*S);
	uniform_rand(rng, &buf[j+i*S], S, 1.0, 2.0);
    }

    m.loadu(&buf[0]);
}


// -------------------------------------------------------------------------------------------------


template<typename T>
static vector<T> reference_multiply_lower(vector<T> &mat, vector<T> &v, int S, int N)
{
    int NN = (N*(N+1))/2;
    assert(mat.size() == NN*S);
    assert(v.size() == N*S);

    vector<T> ret(N*S, 0.0);
    
    for (int i = 0; i < N; i++) {
	for (int j = 0; j <= i; j++) {
	    int ij = (i*(i+1)*S)/2 + j*S;
	    for (int s = 0; s < S; s++)
		ret[i*S+s] += mat[ij+s] * v[j*S+s];
	}
    }

    return ret;
}


template<typename T>
static vector<T> reference_multiply_upper(vector<T> &mat, vector<T> &v, int S, int N)
{
    int NN = (N*(N+1))/2;
    assert(mat.size() == NN*S);
    assert(v.size() == N*S);

    vector<T> ret(N*S, 0.0);
    
    for (int i = 0; i < N; i++) {
	for (int j = 0; j <= i; j++) {
	    int ij = (i*(i+1)*S)/2 + j*S;
	    for (int s = 0; s < S; s++)
		ret[j*S+s] += mat[ij+s] * v[i*S+s];
	}
    }

    return ret;
}


template<typename T>
static vector<T> reference_multiply_symmetric(vector<T> &mat, vector<T> &v, int S, int N)
{
    int NN = (N*(N+1))/2;
    assert(mat.size() == NN*S);
    assert(v.size() == N*S);

    vector<T> ret(N*S, 0.0);
    
    for (int i = 0; i < N; i++) {
	for (int j = 0; j <= i; j++) {
	    int ij = (i*(i+1)*S)/2 + j*S;

	    for (int s = 0; s < S; s++)
		ret[i*S+s] += mat[ij+s] * v[j*S+s];

	    if (i == j)
		continue;

	    for (int s = 0; s < S; s++)
		ret[j*S+s] += mat[ij+s] * v[i*S+s];
	}
    }

    return ret;
}


// -------------------------------------------------------------------------------------------------


template<typename T, unsigned int S, unsigned int N>
void test_linear_algebra_kernels_N(std::mt19937 &rng)
{
    vector<T> mbuf;
    simd_trimatrix<T,S,N> m;
    randomize_triangular_matrix(rng, m, mbuf);
    
    vector<T> vbuf;
    simd_ntuple<T,S,N> v;
    randomize_vector(rng, v, vbuf);

    vector<T> wbuf;
    simd_ntuple<T,S,N> w, x;
    double epsilon;

    // multiply_lower()
    w = m.multiply_lower(v);
    wbuf = reference_multiply_lower(mbuf, vbuf, S, N);
    epsilon = w.compare(&wbuf[0]);
    assert(epsilon < 1.0e-6);
    
    // multiply_upper()
    w = m.multiply_upper(v);
    wbuf = reference_multiply_upper(mbuf, vbuf, S, N);
    epsilon = w.compare(&wbuf[0]);
    assert(epsilon < 1.0e-6);
    
    // multiply_symmetric()
    w = m.multiply_symmetric(v);
    wbuf = reference_multiply_symmetric(mbuf, vbuf, S, N);
    epsilon = w.compare(&wbuf[0]);
    assert(epsilon < 1.0e-6);

    // solve_lower()
    w = m.solve_lower(v);
    x = m.multiply_lower(w);
    epsilon = x.compare(v);
    assert(epsilon < 1.0e-6);
}


template<typename T, unsigned int S>
void test_linear_algebra_kernels(std::mt19937 &rng)
{
    test_linear_algebra_kernels_N<T,S,1> (rng);
    test_linear_algebra_kernels_N<T,S,2> (rng);
    test_linear_algebra_kernels_N<T,S,3> (rng);
    test_linear_algebra_kernels_N<T,S,4> (rng);
    test_linear_algebra_kernels_N<T,S,5> (rng);
    test_linear_algebra_kernels_N<T,S,6> (rng);
    test_linear_algebra_kernels_N<T,S,7> (rng);
    test_linear_algebra_kernels_N<T,S,8> (rng);
}


void test_all_linear_algebra_kernels(std::mt19937 &rng)
{
    for (int iter = 0; iter < 100; iter++) {
	test_linear_algebra_kernels<float,4> (rng);
	test_linear_algebra_kernels<float,8> (rng);
    }

    cout << "test_all_linear_algebra_kernels: pass\n";
}


int main()
{
    std::random_device rd;
    std::mt19937 rng(rd());

    test_all_linear_algebra_kernels(rng);
    return 0;
}
