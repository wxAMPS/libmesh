// The libMesh Finite Element Library.
// Copyright (C) 2002-2016 Benjamin S. Kirk, John W. Peterson, Roy H. Stogner

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



#ifndef LIBMESH_DEFAULT_COUPLING_H
#define LIBMESH_DEFAULT_COUPLING_H

// Local Includes -----------------------------------
#include "libmesh/ghosting_functor.h"

namespace libMesh
{

// Forward declarations
class PeriodicBoundaries;


/**
 * This class implements the default algebraic coupling in libMesh:
 * elements couple to themselves, but may also couple to neighbors
 * both locally and across periodic boundary conditions.
 *
 * \author Roy H. Stogner
 * \date 2016
 */
class DefaultCoupling : public GhostingFunctor
{
public:

  /**
   * Constructor.
   */
  DefaultCoupling() :
    _dof_coupling(libmesh_nullptr),
    _couple_neighbor_dofs(false),
#ifdef LIBMESH_ENABLE_PERIODIC
    _periodic_bcs(libmesh_nullptr),
#endif
    _mesh(libmesh_nullptr)
 {}

  // Change coupling matrix after construction
  void set_dof_coupling(const CouplingMatrix * dof_coupling)
  { _dof_coupling = dof_coupling; }

  // Decide whether to couple to neighbors after construction
  void set_coupled_neighbor_dofs(bool couple_neighbor_dofs)
  { _couple_neighbor_dofs = couple_neighbor_dofs; }

#ifdef LIBMESH_ENABLE_PERIODIC
  // Set PeriodicBoundaries to couple
  void set_periodic_boundaries(const PeriodicBoundaries * periodic_bcs)
  { _periodic_bcs = periodic_bcs; }
#endif

  // Set MeshBase for use in checking for periodic boundary ids
  void set_mesh(const MeshBase * mesh)
  { _mesh = mesh; }

  /**
   * If we have periodic boundaries, then we'll need the mesh to have
   * an updated point locator whenever we're about to query them.
   */
  virtual void mesh_reinit () libmesh_override;

  virtual void redistribute () libmesh_override
  { this->mesh_reinit(); }

  virtual void delete_remote_elements() libmesh_override
  { this->mesh_reinit(); }

  /**
   * For the specified range of active elements, find the elements
   * which will be coupled to them in the sparsity pattern.
   *
   * This will be only the elements themselves by default, but will
   * include side neighbors if an all-discontinuous-variable system is
   * detected and/or if the user specified --implicit_neighbor_dofs on
   * the command line or used set_implicit_neighbor_dofs() in their
   * code.
   */
  virtual void operator() (const MeshBase::const_element_iterator & range_begin,
                           const MeshBase::const_element_iterator & range_end,
                           processor_id_type p,
                           map_type & coupled_elements);

private:

  const CouplingMatrix * _dof_coupling;
  bool _couple_neighbor_dofs;
#ifdef LIBMESH_ENABLE_PERIODIC
  const PeriodicBoundaries * _periodic_bcs;
#endif
  const MeshBase * _mesh;
};

} // namespace libMesh

#endif // LIBMESH_DEFAULT_COUPLING_H
