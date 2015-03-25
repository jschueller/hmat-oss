/*
  HMat-OSS (HMatrix library, open source software)

  Copyright (C) 2014-2015 Airbus Group SAS

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  http://github.com/jeromerobert/hmat-oss
*/

/*! \file
  \ingroup HMatrix
  \brief Spatial cluster tree for the Dofs.
*/
#ifndef _ADMISSIBLITY_HPP
#define _ADMISSIBLITY_HPP

#include <cstddef>
#include <string>

namespace hmat {

// Forward declarations
class ClusterTree;

class AdmissibilityCondition
{
public:
  virtual ~AdmissibilityCondition() {}
  /*! \brief Returns true if 2 nodes are admissible together.

    This is used for the tree construction on which we develop the HMatrix.
    Two leaves are admissible if they satisfy the criterion allowing the
    compression of the resulting matrix block.

    \return true  if 2 nodes are admissible.

   */
  virtual bool isAdmissible(const ClusterTree& rows, const ClusterTree& cols) = 0;
  virtual std::string str() const = 0;
};

/**
 * @brief Hackbusch formula based admissibility
 *  is Hackbusch formula :
 *     min (diameter (), other-> diameter ()) <= eta * distanceTo (other);
 * @param eta    a parameter used in the evaluation of the admissibility.
 * @param maxElementsPerBlock limit memory size of a bloc with AcaFull and Svd compression
 */
class StandardAdmissibilityCondition : public AdmissibilityCondition
{
public:
  StandardAdmissibilityCondition(double eta, size_t maxElementsPerBlock = 5000000);
  bool isAdmissible(const ClusterTree& rows, const ClusterTree& cols);
  std::string str() const;
  void setEta(double eta);
  // For API compatibility
  static StandardAdmissibilityCondition DEPRECATED_INSTANCE;
private:
  double eta_;
  size_t maxElementsPerBlock;
};

} //  end namespace hmat
#endif
