/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2020 Sergei Bolotin.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SG_3D_VECTOR
#define SG_3D_VECTOR


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>
#include <stdio.h>

#include <ostream>

#include <SgMathSupport.h>




/***===================================================================================================*/
/**
 * A three-dimentional vector. Should be useful for various geometrical calculations.
 *
 */
/*=====================================================================================================*/
class Sg3dVector
{
protected:
  double        vec[3];                 //!< An array of 3 elements, stores elements of a vector.

public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object, fills it with zeros.
   */
  inline Sg3dVector(){vec[0]=vec[1]=vec[2]=0.0;};

  /**A constructor.
   * Creates a copy of the object using three elements of the vectors.
   * \param v1, v2, v3 are the values that will be assigned to the vector.
   */
  inline Sg3dVector(double v1, double v2, double v3) {vec[0]=v1; vec[1]=v2; vec[2]=v3;};

  /**A constructor.
   * Creates a copy of the object from vector V.
   * \param V -- vector to be copied.
   */
  inline Sg3dVector(const Sg3dVector& V) {vec[0]=V.vec[0]; vec[1]=V.vec[1]; vec[2]=V.vec[2];};

  /**A destructor.
   * Destroys the object.
   */
  inline ~Sg3dVector() {/*nothing to do here yet*/};


  //
  // Interfaces:
  //
  /**Returns reference at i-th element of the vector.
   * \param i -- an index; valid ranges are [0,1,2].
   */
  inline double& operator()(DIRECTION i) {return vec[i];};

  /**Returns value of i-th element of the vector. The function does not change the object.
   * \param i -- an index; valid ranges are [0,1,2].
   */
  inline double at (DIRECTION i) const {return vec[i];};


  //
  // Functions:
  //
  /**Returns a length of the vector.
   */
  inline double module() const {return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);};

  /**Calculates and returns a latitude in spherical coordinate system.
   */
  inline double phi() const {double f=module(); return f==0.0?0.0:asin(vec[2]/f);};

  /**Calculates and returns a longitude in spherical coordinate system.
   */
  inline double lambda() const 
    {double f = module()==0.0?0.0:atan2(vec[1], vec[0]); return f<0.0?f+2.0*M_PI:f;};

  /**Unifies the vector, returns a reference on it.
   */
  inline Sg3dVector& unify() {return *this/=module();};

  /**Returns a unified vector. The function does not change the object.
   */
  inline Sg3dVector unit() const {return Sg3dVector(*this)/module();};

  /**Zerofies a vector.
   */
  inline void clear() {vec[0]=vec[1]=vec[2]=0.0;};

  /**Assigns a value of the given vector.
   * \param V is a vector to be copied.
   */
  inline Sg3dVector& operator= (const Sg3dVector& v);

  /**Increments a meaning of the vector on a value of the given vector.
   * \param V is a vector to be added.
   */
  inline Sg3dVector& operator+=(const Sg3dVector& v);

  /**Decrements a meaning of the vector on a value of the given vector.
   * \param V is a vector to be substracted.
   */
  inline Sg3dVector& operator-=(const Sg3dVector& v);

  /**Multiplies a vector by the scalar.
   * \param d is a scale.
   */
  inline Sg3dVector& operator*=(double d);

  /**Divides a vector by the scalar.
   * \param d is a divider.
   */
  inline Sg3dVector& operator/=(double d);

  /**Returns a vector with an inverted sign.
   * \param V is a original vector.
   */
  friend inline Sg3dVector operator-(const Sg3dVector& v);

  /**Returns a sum of two vectors.
   * \param V1 is a first term,
   * \param V2 is a second term in the sum.
   */
  friend inline Sg3dVector operator+(const Sg3dVector& v1, const Sg3dVector& v2);

  /**Returns a difference of two vectors.
   */
  friend inline Sg3dVector operator-(const Sg3dVector&, const Sg3dVector&);

  /**Returns a vector divided by scalar.
   */
  friend inline Sg3dVector operator/(const Sg3dVector&, double);

  /**Returns a vector multiplied by scalar.
   */
  friend inline Sg3dVector operator*(const Sg3dVector&, double);

  /**Returns a vector multiplied by scalar.
   */
  friend inline Sg3dVector operator*(double, const Sg3dVector&);

  /**Makes a scalar product of two vectors.
   */
  friend inline double operator*(const Sg3dVector&, const Sg3dVector&);

  /**Makes a vector product of two vectors (because "%" has a priority of "*") 
   */
  friend inline Sg3dVector operator%(const Sg3dVector&, const Sg3dVector&);

  /**Makes a product of matrix and vector 
   */
  friend inline Sg3dVector operator*(const Sg3dMatrix&, const Sg3dVector&);

  /**Boolean operator, returns TRUE if two vectors are equal 
   */
  inline bool operator==(const Sg3dVector& v) const 
    {return (vec[0]==v.vec[0]) && (vec[1]==v.vec[1]) && (vec[2]==v.vec[2]);};

  /**Boolean operator, returns TRUE if two vectors are NOT equal 
   */
  inline bool operator!=(const Sg3dVector& v) const {return !(*this==v);};


  //
  // I/O:
  //
  /**Makes output to std::ostream, standard text formatted output.
   */
  inline friend std::ostream &operator<<(std::ostream& s, const Sg3dVector& v);
};
/*=====================================================================================================*/



/*=====================================================================================================*/
/*                                                                                                     */
/* Sg3dVector's inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
inline Sg3dVector& Sg3dVector::operator=(const Sg3dVector& v)
{
  vec[0] = v.vec[0];
  vec[1] = v.vec[1];
  vec[2] = v.vec[2];
  return *this;
};



//
inline Sg3dVector& Sg3dVector::operator+=(const Sg3dVector& v)
{
  vec[0] += v.vec[0];
  vec[1] += v.vec[1];
  vec[2] += v.vec[2];
  return *this;
};



//
inline Sg3dVector& Sg3dVector::operator-=(const Sg3dVector& v)
{
  vec[0] -= v.vec[0];
  vec[1] -= v.vec[1];
  vec[2] -= v.vec[2];
  return *this;
};



//
inline Sg3dVector& Sg3dVector::operator*=(double d)
{
  vec[0] *= d;
  vec[1] *= d;
  vec[2] *= d;
  return *this;
};



//
inline Sg3dVector& Sg3dVector::operator/=(double d)
{
  vec[0] /= d;
  vec[1] /= d;
  vec[2] /= d;
  return *this;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* Sg3dVector's inline friends:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
inline Sg3dVector operator-(const Sg3dVector& v)
{
  return Sg3dVector(-v.vec[0], -v.vec[1], -v.vec[2]);
};



//
inline Sg3dVector operator+(const Sg3dVector& v1, const Sg3dVector& v2)
{
  return Sg3dVector(v1.vec[0] + v2.vec[0], v1.vec[1] + v2.vec[1], v1.vec[2] + v2.vec[2]);
};



//
inline Sg3dVector operator-(const Sg3dVector& v1, const Sg3dVector& v2)
{
  return Sg3dVector(v1.vec[0] - v2.vec[0], v1.vec[1] - v2.vec[1], v1.vec[2] - v2.vec[2]);
};



//
inline Sg3dVector operator/(const Sg3dVector& v, double d)
{
  return Sg3dVector(v.vec[0]/d, v.vec[1]/d, v.vec[2]/d);
};



//
inline Sg3dVector operator*(const Sg3dVector& v, double d)
{
  return Sg3dVector(v.vec[0]*d, v.vec[1]*d, v.vec[2]*d);
};



//
inline Sg3dVector operator*(double d, const Sg3dVector& v)
{
  return Sg3dVector(d*v.vec[0], d*v.vec[1], d*v.vec[2]);
};



//
inline double operator*(const Sg3dVector& v1, const Sg3dVector& v2)
{
  return (v1.vec[0]*v2.vec[0] + v1.vec[1]*v2.vec[1] + v1.vec[2]*v2.vec[2]);
};



//
inline Sg3dVector operator%(const Sg3dVector& v1, const Sg3dVector& v2)
{
  return Sg3dVector(v1.vec[1]*v2.vec[2] - v1.vec[2]*v2.vec[1],
                    v1.vec[2]*v2.vec[0] - v1.vec[0]*v2.vec[2],
                    v1.vec[0]*v2.vec[1] - v1.vec[1]*v2.vec[0] );
};



//
inline std::ostream &operator<<(std::ostream& s, const Sg3dVector& v)
{
  return s << "(" << v.vec[0] << ", " <<  v.vec[1] << ", " <<  v.vec[2] << ")";
};
/*=====================================================================================================*/





#endif //SG_3D_VECTOR
