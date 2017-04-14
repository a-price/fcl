/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2017, Georgia Tech Research Corporation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Georgia Tech Research Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Andrew Price <arprice@gatech.edu> */

#include "fcl/narrowphase/detail/primitive_shape_algorithm/triangle_triangle.h"
#include "fcl/narrowphase/detail/primitive_shape_algorithm/triangle_triangle-inl.h"

#include "fcl/math/constants.h"
#include "test_fcl_hungarian.h"
#include "test_fcl_hungarian-inl.h"

#include <gtest/gtest.h>

using namespace fcl;
using namespace fcl::detail;

template <typename Scalar>
void verifyContacts(const Vector3<Scalar>* expected, const Vector3<Scalar>* actual, const int numContacts)
{
  typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> CostMatrix;

  CostMatrix C(numContacts, numContacts);
  for (int i = 0; i < numContacts; ++i)
  {
    for (int j = 0; j < numContacts; ++j)
    {
      C(i,j) = (expected[i]-actual[j]).squaredNorm();
    }
  }

  fcl::test::Hungarian<Scalar> H(C);

  std::vector<int> permutation = H.getAssignment();

  EXPECT_LT(H.getSolutionCost(), 1e-6);
  for (int i = 0; i < numContacts; ++i)
  {
    EXPECT_LT((expected[i]-actual[permutation[i]]).norm(), 1e-6);
  }
}

template <typename S>
void test_general_tris()
{
	std::vector<Vector3<S>> expected;
	std::array<Vector3<S>, 6> contacts;
	unsigned int num_contacts;
    S penetration_depth;
    Vector3<S> normal;
	bool res;
	res = intersectTriangles<S, true>({0,0,0},{0,1,0},{0,0,1},
	                                  {0,0,0},{0,-1,0},{1,0,0},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	EXPECT_TRUE(res);
	EXPECT_EQ(1, num_contacts);
	EXPECT_TRUE(fabs(penetration_depth) < 1e-6);
	EXPECT_TRUE(contacts[0].norm() < 1e-6);


	res = intersectTriangles<S, true>({1,0,0},{0,1,0},{0,-1,0},
	                                  {0,0,0},{0,1,1},{0,-1,1},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	EXPECT_TRUE(res);
	EXPECT_EQ(1, num_contacts);
	EXPECT_TRUE(fabs(penetration_depth) < 1e-6);
	EXPECT_TRUE(contacts[0].norm() < 1e-6);


	res = intersectTriangles<S, true>({1,0,0},{0,1,0},{0,-1,0},
	                                  {0,0,0.5},{0,1,1},{0,-1,1},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	EXPECT_FALSE(res);


	res = intersectTriangles<S, true>({1,0,0},{0,1,0},{0,-1,0},
	                                  {1,0,0.1},{0,1,0.1},{0,-1,0.1},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	EXPECT_FALSE(res);


	res = intersectTriangles<S, true>({1,0,0},{0,1,0},{0,-1,0},
	                                  {1,0,0.1},{0,1,0.1},{0,-1,0.2},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	EXPECT_FALSE(res);


	res = intersectTriangles<S, true>({1,0,0},{0,1,0},{0,-1,0},
	                                  {0.5,0,0},{0,1,0},{0,-1,0},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	expected = {{0.5,0,0},{0,1,0},{0,-1,0}};
	EXPECT_TRUE(res);
	EXPECT_EQ(3, num_contacts);
	EXPECT_TRUE(fabs(penetration_depth) < 1e-6);
	verifyContacts(expected.data(), contacts.data(), num_contacts);


	res = intersectTriangles<S, true>({1,0,0},{0,1,0},{0,-1,0},
	                                  {0.5,0,0},{0,0.5,0},{0,0.25,0},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	expected = {{0.5,0,0},{0,0.5,0},{0,0.25,0}};
	EXPECT_TRUE(res);
	EXPECT_EQ(3, num_contacts);
	EXPECT_TRUE(fabs(penetration_depth) < 1e-6);
	verifyContacts(expected.data(), contacts.data(), num_contacts);


	res = intersectTriangles<S, true>({0.5,0,0},{0,0.5,0},{0,0.25,0},
	                                  {1,0,0},{0,1,0},{0,-1,0},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	expected = {{0.5,0,0},{0,0.5,0},{0,0.25,0}};
	EXPECT_TRUE(res);
	EXPECT_EQ(3, num_contacts);
	EXPECT_TRUE(fabs(penetration_depth) < 1e-6);
	verifyContacts(expected.data(), contacts.data(), num_contacts);


	res = intersectTriangles<S, true>({1,-0.1,0},{-0.5,1,0},{-0.5,-0.1,0},
	                                  {0,0.1,-0.1},{0,0.1,1},{0,-1,-0.1},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	expected = {{0,0.1,0},{0,-0.1,0}};
	EXPECT_TRUE(res);
	EXPECT_EQ(2, num_contacts);
	EXPECT_TRUE(fabs(penetration_depth-0.1) < 1e-6);
	EXPECT_TRUE(normal.cross(Vector3<S>::UnitZ()).norm() < 1e-6);
	verifyContacts(expected.data(), contacts.data(), num_contacts);


	res = intersectTriangles<S, true>({0,0.1,-0.1},{0,0.1,1},{0,-1,-0.1},
	                                  {1,-0.1,0},{-0.5,1,0},{-0.5,-0.1,0},
	                                  contacts.data(), &num_contacts,
	                                  &penetration_depth, &normal);
	expected = {{0,0.1,0},{0,-0.1,0}};
	EXPECT_TRUE(res);
	EXPECT_EQ(2, num_contacts);
	EXPECT_TRUE(fabs(penetration_depth-0.1) < 1e-6);
	EXPECT_TRUE(normal.cross(Vector3<S>::UnitZ()).norm() < 1e-6);
	verifyContacts(expected.data(), contacts.data(), num_contacts);
}

//==============================================================================
// Parallel edge test
//    _______
//   |\      |
//   | \___  |
//   | |\  | |
//   | | \ | |
//   | |__\| |
//   |     \ |
//   |______\|
//
// Transverse edge test
//    _______
//   |\      |
//   | \___  |
//   | |\ /| |
//   | | X | |
//   | |/_\| |
//   |     \ |
//   |______\|
// v_a[0]    v_a[1]

template <typename Scalar>
void test_coplanar_triangle_pairs()
{
  // Outline of a rectangle
  Vector3<Scalar> v_a[4] = {{-0.5,0.5,-0.5},
                            {-0.5,0.5, 0.5},
                            { 0.5,0.5,-0.5},
                            { 0.5,0.5, 0.5}};
  Vector3<Scalar> v_b[4] = {{-0.25,0.5,-0.25},
                            {-0.25,0.5, 0.25},
                            { 0.25,0.5,-0.25},
                            { 0.25,0.5, 0.25}};

  // Vertex lists for triangles
  int t_a[2][3]  = {{0,1,2},  // Surrounding triangles
                    {3,2,1}};
  int t_bp[2][3] = {{0,1,2}, // Parallel crossing
                    {3,2,1}};
  int t_bx[2][3] = {{0,1,3}, // Transverse crossing
                    {3,2,0}};
  Vector3<Scalar> contact_points[6];
  unsigned int num_contact_points;
  Scalar penetration_depth;
  Vector3<Scalar> normal;

  // Test smaller triangle inside larger triangle

  bool res = intersectTriangles<Scalar, true>(
        v_b[t_bp[0][0]], v_b[t_bp[0][1]], v_b[t_bp[0][2]],
        v_a[t_a[0][0]], v_a[t_a[0][1]], v_a[t_a[0][2]],
        contact_points, &num_contact_points, &penetration_depth, &normal);

  EXPECT_TRUE(res);
  EXPECT_EQ(3u, num_contact_points);
  EXPECT_EQ((Scalar)0, penetration_depth);
  std::vector<Vector3<Scalar>> expected = {v_b[t_bp[0][0]], v_b[t_bp[0][1]], v_b[t_bp[0][2]]};
  verifyContacts(expected.data(), contact_points, num_contact_points);

  // Flip the triangle order
  res = intersectTriangles<Scalar, true>(
        v_a[t_a[0][0]], v_a[t_a[0][1]], v_a[t_a[0][2]],
        v_b[t_bp[0][0]], v_b[t_bp[0][1]], v_b[t_bp[0][2]],
        contact_points, &num_contact_points, &penetration_depth, &normal);

  EXPECT_TRUE(res);
  EXPECT_EQ(3u, num_contact_points);
  EXPECT_EQ((Scalar)0, penetration_depth);
  verifyContacts(expected.data(), contact_points, num_contact_points);

  // Test transverse crossing

  expected = {v_b[0], v_b[1], Vector3<Scalar>(0.0, 0.5, 0.0)};
  res = intersectTriangles<Scalar, true>(
        v_a[t_a[0][0]], v_a[t_a[0][1]], v_a[t_a[0][2]],
        v_b[t_bx[0][0]], v_b[t_bx[0][1]], v_b[t_bx[0][2]],
        contact_points, &num_contact_points, &penetration_depth, &normal);

  EXPECT_TRUE(res);
  EXPECT_EQ(3u, num_contact_points);
  EXPECT_EQ((Scalar)0, penetration_depth);
  verifyContacts(expected.data(), contact_points, num_contact_points);

  // Flip the triangle order
  res = intersectTriangles<Scalar, true>(
        v_b[t_bx[0][0]], v_b[t_bx[0][1]], v_b[t_bx[0][2]],
        v_a[t_a[0][0]], v_a[t_a[0][1]], v_a[t_a[0][2]],
        contact_points, &num_contact_points, &penetration_depth, &normal);

  EXPECT_TRUE(res);
  EXPECT_EQ(3u, num_contact_points);
  EXPECT_EQ((Scalar)0, penetration_depth);
  verifyContacts(expected.data(), contact_points, num_contact_points);

  // Test identical triangles

  expected = {v_a[0], v_a[1], v_a[2]};
  res = intersectTriangles<Scalar, true>(
        v_a[t_a[0][0]], v_a[t_a[0][1]], v_a[t_a[0][2]],
        v_a[t_a[0][1]], v_a[t_a[0][2]], v_a[t_a[0][0]],
        contact_points, &num_contact_points, &penetration_depth, &normal);

  EXPECT_TRUE(res);
  EXPECT_EQ(3u, num_contact_points);
  EXPECT_EQ((Scalar)0, penetration_depth);
  verifyContacts(expected.data(), contact_points, num_contact_points);

  // Test hexagram (i.e. star of David)

  std::vector<Vector3<Scalar>> hex;
  const Scalar PI = fcl::constants<Scalar>::pi();
  for (int i = 0; i < 6; ++i)
  {
    Scalar angle = static_cast<Scalar>((360/6)*i) * PI/180.0;
    hex.push_back({static_cast<Scalar>(cos(angle)),
                   static_cast<Scalar>(sin(angle)),
                   0.0});
  }

  res = intersectTriangles<Scalar, true>(
        hex[1], hex[3], hex[5],
        hex[0], hex[2], hex[4],
        contact_points, &num_contact_points, &penetration_depth, &normal);

  expected = { 2.0/3.0 * hex[0] + 1.0/3.0 * hex[2],
               1.0/3.0 * hex[0] + 2.0/3.0 * hex[2],
               2.0/3.0 * hex[2] + 1.0/3.0 * hex[4],
               1.0/3.0 * hex[2] + 2.0/3.0 * hex[4],
               2.0/3.0 * hex[4] + 1.0/3.0 * hex[0],
               1.0/3.0 * hex[4] + 2.0/3.0 * hex[0]};
  EXPECT_TRUE(res);
  EXPECT_EQ(6u, num_contact_points);
  EXPECT_EQ((Scalar)0, penetration_depth);
  verifyContacts(expected.data(), contact_points, num_contact_points);
}

//==============================================================================
GTEST_TEST(FCL_TRI_TRI_INTERSECTION, general_tris)
{
  test_general_tris<float>();
  test_general_tris<double>();
}

//==============================================================================
GTEST_TEST(FCL_TRI_TRI_INTERSECTION, coplanar_tris)
{
  test_coplanar_triangle_pairs<float>();
  test_coplanar_triangle_pairs<double>();
}

//==============================================================================
int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}