/**
 * @file This file is part of EDGE.
 *
 * @author Alexander Breuer (anbreuer AT ucsd.edu)
 *
 * @section LICENSE
 * Copyright (c) 2016, Regents of the University of California
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 * Common functionality of the solvers for the elastic wave equations.
 **/

#ifndef ELASOLS_COMMON_HPP
#define ELASOLS_COMMON_HPP

#include <cassert>
#include "../common.hpp"
#include "linalg/Mappings.hpp"
#include "linalg/Matrix.h"

namespace edge {
  namespace elastic {
    namespace solvers {
      class common;
    }
  }
}

class edge::elastic::solvers::common {
  public:
    /**
     * Sets up the two-dimensional flux solvers for a single face.
     *
     * @param i_rhoL density of the left element.
     * @param i_rhoL density of the right element.
     * @param i_lamL Lame parameter lambda of the left element.
     * @param i_lamR Lame parameter lambda of the right element.
     * @param i_muL Lame parameter mu of the left element.
     * @param i_muR Lame parameter mu of the right element.
     * @param i_nx x-component of the face-normal.
     * @param i_ny y-component of the face-normal.
     * @param i_nz z-component of the face-normal.
     * @param o_fL set to flux solver applied to the left element's DOFs.
     * @param o_fR set to flux solver applied to the right element's DOFs.
     * @param i_freeSurface true if surface boundary conditions are added to o_fR.
     **/
    static void setupSolver2d( real_base i_rhoL, real_base i_rhoR,
                               real_base i_lamL, real_base i_lamR,
                               real_base i_muL,  real_base i_muR,
                               real_mesh i_nx, real_mesh i_ny, real_mesh i_nz,
                               real_base o_fL[5][5],
                               real_base o_fR[5][5],
                               bool      i_freeSurface = false ) {
#include "impl/elastic/generated/SolverElastic2D.inc"
    }

    /**
     * Computes the left side's contribution to the Riemann solution in the face-aligned coordinate system.
     *
     * @param i_rhoL left element's density rho.
     * @param i_rhoR right element's density rho.
     * @param i_lamL left element's Lame parameter lambda.
     * @param i_lamR right element's Lame parameter lambda.
     * @param i_muL left element's Lame parameter mu.
     * @param i_muR right element's Lame parameter mu.
     * @parma o_flMid will be set to matrix for the left element's contrinution.
     **/
    static void setupFlMid3d( real_base i_rhoL, real_base i_rhoR,
                              real_base i_lamL, real_base i_lamR,
                              real_base i_muL,  real_base i_muR,
                              real_base o_flMid[9][9] ) {
#include "impl/elastic/generated/FlMid3D.inc"
    }

    /**
     * Computes the right side's contribution to the Riemann solution in the face-aligned coordinate system.
     *
     * @param i_rhoL left element's density rho.
     * @param i_rhoR right element's density rho.
     * @param i_lamL left element's Lame parameter lambda.
     * @param i_lamR right element's Lame parameter lambda.
     * @param i_muL left element's Lame parameter mu.
     * @param i_muR right element's Lame parameter mu.
     * @parma o_frMid will be set to matrix for the right element's contrinution.
     **/
    static void setupFrMid3d( real_base i_rhoL, real_base i_rhoR,
                              real_base i_lamL, real_base i_lamR,
                              real_base i_muL,  real_base i_muR,
                              real_base o_frMid[9][9] ) {
#include "impl/elastic/generated/FrMid3D.inc"
    }

    /**
     * Sets up the three-dimensional flux solvers for a single face.
     *
     * @param i_rhoL density of the left element.
     * @param i_rhoL density of the right element.
     * @param i_lamL Lame parameter lambda of the left element.
     * @param i_lamR Lame parameter lambda of the right element.
     * @param i_muL Lame parameter mu of the left element.
     * @param i_muR Lame parameter mu of the right element.
     * @param i_nx x-component of the face-normal.
     * @param i_ny y-component of the face-normal.
     * @param i_nz z-component of the face-normal.
     * @param i_sx x-component of the first face-tangent.
     * @param i_sy y-component of the first face-tangent.
     * @param i_sz z-component of the first face-tangent.
     * @param i_tx x-component of the second face-tangent.
     * @param i_ty y-component of the second face-tangent.
     * @param i_tz z-component of the second face-tangent.
     * @param o_fL set to flux solver applied to the left element's DOFs.
     * @param o_fR set to flux solver applied to the right element's DOFs.
     * @param i_freeSurface true if free surface boundary conditions are added to o_fR.
     **/
    static void setupSolver3d( real_base i_rhoL, real_base i_rhoR,
                               real_base i_lamL, real_base i_lamR,
                               real_base i_muL,  real_base i_muR,
                               real_mesh i_nx, real_mesh i_ny, real_mesh i_nz,
                               real_mesh i_sx, real_mesh i_sy, real_mesh i_sz,
                               real_mesh i_tx, real_mesh i_ty, real_mesh i_tz,
                               real_base o_fL[9][9],
                               real_base o_fR[9][9],
                               bool      i_freeSurface = false ) {
      // intermediate matrices
      real_base l_t[9][9];
      real_base l_tm1[9][9];
      real_base l_flMid[9][9];
      real_base l_frMid[9][9];

      real_base l_tmpL[9][9];
      real_base l_tmpR[9][9];

      // setup the intermediate matrices
      elastic::common::setupTrafo3d( i_nx, i_ny, i_nz,
                                     i_sx, i_sy, i_sz,
                                     i_tx, i_ty, i_tz,
                                     l_t );

      elastic::common::setupTrafoInv3d( i_nx, i_ny, i_nz,
                                        i_sx, i_sy, i_sz,
                                        i_tx, i_ty, i_tz,
                                        l_tm1 );

      setupFlMid3d( i_rhoL, i_rhoR,
                    i_lamL, i_lamR,
                    i_muL,  i_muR,
                    l_flMid );

      setupFrMid3d( i_rhoL, i_rhoR,
                    i_lamL, i_lamR,
                    i_muL,  i_muR,
                    l_frMid );

      // compute the flux solvers
      linalg::Matrix::matMulB0( 9, 9, 9,
                                l_t[0], l_flMid[0], l_tmpL[0] );
      linalg::Matrix::matMulB0( 9, 9, 9,
                                l_t[0], l_frMid[0], l_tmpR[0] );

      linalg::Matrix::matMulB0( 9, 9, 9,
                                l_tmpL[0], l_tm1[0], o_fL[0] );

      /**
       * Free surface boundary conditions mirror (rotated) x-components of the stress tensor.
       * Reference: Eq. (41) in
       *            Dumbser, Michael, and Martin Kaeser.
       *            "An arbitrary high-order discontinuous Galerkin method for elastic waves on
       *            unstructured meshes—II. The three-dimensional isotropic case."
       *            Geophysical Journal International 167.1 (2006): 319-336.
       **/
      if( i_freeSurface == true ) {
        // multiply tmR with diag (-1, 1, 1, -1, 1, -1, 1, 1, 1 ) from the right.
        //                          0         3      5
        for( unsigned short l_ro = 0; l_ro < 9; l_ro++ ) {
          l_tmpR[l_ro][0] *= -1;
          l_tmpR[l_ro][3] *= -1;
          l_tmpR[l_ro][5] *= -1;
        }
      }

      linalg::Matrix::matMulB0( 9, 9, 9,
                                l_tmpR[0], l_tm1[0], o_fR[0] );
    }

    // checks the setup of the flux solvers against a few predefined values
    static void checkSolverSetup2d() {

    assert( N_QUANTITIES == 5 );

      real_base l_fL[5][5];
      real_base l_fR[5][5];

      setupSolver2d( 1, 1,
                     2, 2,
                     1, 1,
                    -0.2, std::sqrt(1-0.04), 0,
                     l_fL,
                     l_fR );

      // check some values
      assert( std::abs( l_fL[0][0] -  0.0592              ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][0] -  0.0008              ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][0] - -0.094060406122874   ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][0] -  0.1                 ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][0] -  0.0                 ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][1] -  0.4608              ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][1] -  0.9792              ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][1] - -0.00391918358845308 ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][1] -  0.0                 ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][1] - -0.489897948556636   ) < TOL.SOLVER );

      assert( std::abs( l_fR[0][2] -  0.384079991668402   ) < TOL.SOLVER );
      assert( std::abs( l_fR[1][2] -  0.20379754659956    ) < TOL.SOLVER );
      assert( std::abs( l_fR[2][2] - -0.4616              ) < TOL.SOLVER );
      assert( std::abs( l_fR[3][2] - -0.489897948556636   ) < TOL.SOLVER );
      assert( std::abs( l_fR[4][2] -  0.1                 ) < TOL.SOLVER );

      assert( std::abs( l_fR[0][3] -  0.4                 ) < TOL.SOLVER );
      assert( std::abs( l_fR[1][3] -  0.2                 ) < TOL.SOLVER );
      assert( std::abs( l_fR[2][3] - -0.489897948556636   ) < TOL.SOLVER );
      assert( std::abs( l_fR[3][3] - -0.52                ) < TOL.SOLVER );
      assert( std::abs( l_fR[4][3] -  0.0979795897113271  ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][4] - -0.979795897113271   ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][4] - -1.95959179422654    ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][4] -  0.1                 ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][4] - -0.0979795897113271  ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][4] -  0.98                ) < TOL.SOLVER );

      setupSolver2d( 1, 1,
                     2, 2,
                     1, 1,
                     0.5, -std::sqrt(0.75), 0,
                     l_fL,
                     l_fR );

      // check some values
      assert( std::abs( l_fL[0][0] -  0.34375             ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][0] -  0.03125             ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][0] - -0.162379763209582   ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][0] - -0.25                ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][0] -  0.0                 ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][1] -  0.28125             ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][1] -  0.84375             ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][1] - -0.0541265877365274  ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][1] -  0.0                 ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][1] -  0.433012701892219   ) < TOL.SOLVER );

      assert( std::abs( l_fR[0][2] -  0.757772228311384   ) < TOL.SOLVER );
      assert( std::abs( l_fR[1][2] -  0.541265877365274   ) < TOL.SOLVER );
      assert( std::abs( l_fR[2][2] - -0.3125              ) < TOL.SOLVER );
      assert( std::abs( l_fR[3][2] - 0.433012701892219    ) < TOL.SOLVER );
      assert( std::abs( l_fR[4][2] -  -0.25               ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][3] - -1.0                 ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][3] - -0.5                 ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][3] -  0.433012701892219   ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][3] -  0.625               ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][3] - -0.21650635094611    ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][4] -  0.866025403784439   ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][4] -  1.73205080756888    ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][4] - -0.25                ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][4] - -0.21650635094611    ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][4] -  0.875               ) < TOL.SOLVER );

      setupSolver2d( 1, 1,
                     2, 2,
                     1, 1,
                     1/std::sqrt(2), 1/std::sqrt(2), 0,
                     l_fL,
                     l_fR );

      // check some values
      assert( std::abs( l_fL[0][0] -  0.625               ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][0] -  0.125               ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][0] -  0.125               ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][0] - -0.25 * std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][0] -  0                   ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][1] -  0.125               ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][1] -  0.625               ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][1] -  0.125               ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][1] -  0.0                 ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][1] - -0.25 * std::sqrt(2) ) < TOL.SOLVER );

      assert( std::abs( l_fR[0][2] - -0.75                ) < TOL.SOLVER );
      assert( std::abs( l_fR[1][2] - -0.75                ) < TOL.SOLVER );
      assert( std::abs( l_fR[2][2] - -0.25                ) < TOL.SOLVER );
      assert( std::abs( l_fR[3][2] - -0.25 * std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fR[4][2] - -0.25 * std::sqrt(2) ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][3] -        -std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][3] - -0.5  * std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][3] - -0.25 * std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][3] -  0.75                ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][3] -  0.25                ) < TOL.SOLVER );

      assert( std::abs( l_fL[0][4] - -0.5 *  std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fL[1][4] -        -std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fL[2][4] - -0.25 * std::sqrt(2) ) < TOL.SOLVER );
      assert( std::abs( l_fL[3][4] -  0.25                ) < TOL.SOLVER );
      assert( std::abs( l_fL[4][4] -  0.75                ) < TOL.SOLVER );
    }

    /**
     * Sets up the flux solvers.
     *
     * @param i_nElements number of elements in the mesh.
     * @param i_nFaces number of faces in the mesh.
     * @param i_elMeDa mapping of element ids: mesh-to-data.
     * @param i_elDaMe mapping of element ids: data-to-mesh.
     * @param i_faceAdjacentElements elements adjacent to the faces.
     * @param i_elementAdjacentFaces faces adjacent to the elements.
     * @param i_faceChars mesh characteristics of the faces.
     * @param i_bgPars backgrorund parameters (velocities).
     * @param i_elChars element characteristics.
     * @param o_fluxSolversOwn will be set to the flux solvers for the elements' own tDOFs.
     * @param o_fluxSolversNeigh will be set to the flux solvers for the neighboring elements' tDOFs.
     **/
    static void setupSolvers(       int_el                 i_nElements,
                                    int_el                 i_nFaces,
                              const std::vector< int_el > &i_elMeDa,
                              const std::vector< int_el > &i_elDaMe,
                              const int_el               (*i_elVe)[ C_ENT[T_SDISC.ELEMENT].N_VERTICES ],
                              const int_el               (*i_faceAdjacentElements)[2],
                              const int_el               (*i_elementAdjacentFaces)[C_ENT[T_SDISC.ELEMENT].N_FACES],
                              const t_vertexChars         *i_vertexChars,
                              const t_faceChars           *i_faceChars,
                              const t_elementChars        *i_elChars,
                              const t_bgPars             (*i_bgPars)[1],
                                    t_fluxSolver         (*o_fluxSolversOwn)[  C_ENT[T_SDISC.ELEMENT].N_FACES],
                                    t_fluxSolver         (*o_fluxSolversNeigh)[C_ENT[T_SDISC.ELEMENT].N_FACES] ) {
      PP_INSTR_FUN("flux_solvers")

      // check the include of the solvers
      if( N_DIM == 2 ) checkSolverSetup2d();

      // iterate over elements and reset flux solvers (wrong configs should at least blow up our simulation)
#ifdef PP_USE_OMP
#pragma omp parallel for num_threads( std::max( parallel::g_nThreads-1, 1 ) )
#endif
      for( int_el l_el = 0; l_el < i_nElements; l_el++ ) {
        for( int_md l_fa = 0; l_fa < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fa++ ) {
          for( unsigned int l_q1 = 0; l_q1 < N_QUANTITIES; l_q1++ ) {
            for( unsigned int l_q2 = 0; l_q2 < N_QUANTITIES; l_q2++ ) {
              o_fluxSolversOwn[  l_el][l_fa].solver[l_q1][l_q2] = std::numeric_limits<real_base>::max();
              o_fluxSolversNeigh[l_el][l_fa].solver[l_q1][l_q2] = std::numeric_limits<real_base>::max();
            }
          }
        }
      }

      // iterate over faces
#ifdef PP_USE_OMP
#pragma omp parallel for
#endif
      for( int_el l_fa = 0; l_fa < i_nFaces; l_fa++ ) {
        // get left and right element
        int_el l_elL = i_faceAdjacentElements[l_fa][0];
        int_el l_elR = i_faceAdjacentElements[l_fa][1];

        // check if the neighbors exist, if not: bnd-conditions
        bool l_exL = l_elL != std::numeric_limits<int_el>::max();
        bool l_exR = l_elR != std::numeric_limits<int_el>::max();

        // find ids of neighboring flux solvers in the elements
        unsigned short l_fIdL = std::numeric_limits<unsigned short>::max();
        unsigned short l_fIdR = std::numeric_limits<unsigned short>::max();
        for( unsigned int l_elFa = 0; l_elFa < C_ENT[T_SDISC.ELEMENT].N_FACES; l_elFa++ ) {
          if( l_exL && i_elementAdjacentFaces[l_elL][l_elFa] == l_fa ) {
            // face should only by found once
            assert( l_fIdL == std::numeric_limits<unsigned short>::max() );

            l_fIdL = l_elFa;
          }
          if( l_exR && i_elementAdjacentFaces[l_elR][l_elFa] == l_fa ) {
            // face should only by found once
            assert( l_fIdR == std::numeric_limits<unsigned short>::max() );

            l_fIdR = l_elFa;
          }
        }
        EDGE_CHECK( !l_exL || l_fIdL != std::numeric_limits<unsigned short>::max() ) << l_fa;
        EDGE_CHECK( !l_exR || l_fIdR != std::numeric_limits<unsigned short>::max() ) << l_fa;

        // collect lame parameters
        real_base l_rhoL, l_rhoR, l_lamL, l_lamR, l_muL, l_muR;
        if( l_exL ) {
          l_rhoL = i_bgPars[l_elL][0].rho; l_lamL = i_bgPars[l_elL][0].lam; l_muL = i_bgPars[l_elL][0].mu;
        }
        else {
          assert( l_exR );
          // mirror right elements paramters for non-existing left element
          l_rhoL = i_bgPars[l_elR][0].rho; l_lamL = i_bgPars[l_elR][0].lam; l_muL = i_bgPars[l_elR][0].mu;
        }
        if( l_exR ) {
          l_rhoR = i_bgPars[l_elR][0].rho; l_lamR = i_bgPars[l_elR][0].lam; l_muR = i_bgPars[l_elR][0].mu;
        }
        else {
          assert( l_exL );
          // mirror left elements paramters for non-existing right element
          l_rhoR = i_bgPars[l_elL][0].rho; l_lamR = i_bgPars[l_elL][0].lam; l_muR  = i_bgPars[l_elL][0].mu;
        }

#if PP_N_DIM == 2
        // compute solvers for the left element
        if( l_exL ) {
          setupSolver2d( l_rhoL, l_rhoR,
                         l_lamL, l_lamR,
                         l_muL,  l_muR,
                         i_faceChars[l_fa].outNormal[0],
                         i_faceChars[l_fa].outNormal[1],
                         i_faceChars[l_fa].outNormal[2],
                         o_fluxSolversOwn[l_elL][l_fIdL].solver,
                         o_fluxSolversNeigh[l_elL][l_fIdL].solver,
                        (i_faceChars[l_fa].spType & FREE_SURFACE) == FREE_SURFACE );
        }

        // compute solvers for the right element
        if( l_exR ) {
          setupSolver2d( l_rhoR, l_rhoL,
                         l_lamR, l_lamL,
                         l_muR,  l_muL,
                        -i_faceChars[l_fa].outNormal[0],
                        -i_faceChars[l_fa].outNormal[1],
                        -i_faceChars[l_fa].outNormal[2],
                         o_fluxSolversOwn[l_elR][l_fIdR].solver,
                         o_fluxSolversNeigh[l_elR][l_fIdR].solver,
                         false );
        }
#elif PP_N_DIM == 3
        // compute solvers for the left element
        if( l_exL ) {
          setupSolver3d( l_rhoL, l_rhoR,
                         l_lamL, l_lamR,
                         l_muL,  l_muR,
                         i_faceChars[l_fa].outNormal[0],
                         i_faceChars[l_fa].outNormal[1],
                         i_faceChars[l_fa].outNormal[2],
                         i_faceChars[l_fa].tangent0[0],
                         i_faceChars[l_fa].tangent0[1],
                         i_faceChars[l_fa].tangent0[2],
                         i_faceChars[l_fa].tangent1[0],
                         i_faceChars[l_fa].tangent1[1],
                         i_faceChars[l_fa].tangent1[2],
                         o_fluxSolversOwn[l_elL][l_fIdL].solver,
                         o_fluxSolversNeigh[l_elL][l_fIdL].solver,
                        (i_faceChars[l_fa].spType & FREE_SURFACE) == FREE_SURFACE );
        }

        // compute solvers for the right element
        if( l_exR ) {
          // boundary conditions have the element on the left-side per definition
          EDGE_CHECK_NE( (i_faceChars[l_fa].spType & FREE_SURFACE), FREE_SURFACE );

          setupSolver3d( l_rhoR, l_rhoL,
                         l_lamR, l_lamL,
                         l_muR,  l_muL,
                        -i_faceChars[l_fa].outNormal[0],
                        -i_faceChars[l_fa].outNormal[1],
                        -i_faceChars[l_fa].outNormal[2],
                         i_faceChars[l_fa].tangent0[0],
                         i_faceChars[l_fa].tangent0[1],
                         i_faceChars[l_fa].tangent0[2],
                         i_faceChars[l_fa].tangent1[0],
                         i_faceChars[l_fa].tangent1[1],
                         i_faceChars[l_fa].tangent1[2],
                         o_fluxSolversOwn[l_elR][l_fIdR].solver,
                         o_fluxSolversNeigh[l_elR][l_fIdR].solver,
                         false );
        }
#else
        assert( false );
#endif

        // scale solvers with mesh-dependent scalars
        // derive vertex coords
        real_mesh l_veCoordsL[N_DIM][C_ENT[T_SDISC.ELEMENT].N_VERTICES];
        real_mesh l_veCoordsR[N_DIM][C_ENT[T_SDISC.ELEMENT].N_VERTICES];

        for( unsigned int l_ve = 0; l_ve < C_ENT[T_SDISC.ELEMENT].N_VERTICES; l_ve++ ) {
          int_el l_veIdL=std::numeric_limits<int_el>::max();
          int_el l_veIdR=std::numeric_limits<int_el>::max();

          if( l_exL ) l_veIdL = i_elVe[l_elL][l_ve];
          if( l_exR ) l_veIdR = i_elVe[l_elR][l_ve];

          for( unsigned int l_dim = 0; l_dim < N_DIM; l_dim++ ) {
            if( l_exL ) l_veCoordsL[l_dim][l_ve] = i_vertexChars[l_veIdL].coords[l_dim];
            if( l_exR ) l_veCoordsR[l_dim][l_ve] = i_vertexChars[l_veIdR].coords[l_dim];
          }
        }

        // compute determinant of the mapping's jacobian
        real_mesh l_jacL[N_DIM][N_DIM], l_jacR[N_DIM][N_DIM];
        if( l_exL ) linalg::Mappings::evalJac( T_SDISC.ELEMENT, l_veCoordsL[0], l_jacL[0] );
        if( l_exR ) linalg::Mappings::evalJac( T_SDISC.ELEMENT, l_veCoordsR[0], l_jacR[0] );

        real_mesh l_jL=std::numeric_limits<real_mesh>::max();
        real_mesh l_jR=std::numeric_limits<real_mesh>::max();
#if PP_N_DIM == 2
        if( l_exL ) l_jL = linalg::Matrix::det2x2( l_jacL );
        if( l_exR ) l_jR = linalg::Matrix::det2x2( l_jacR );
#elif PP_N_DIM == 3
        if( l_exL ) l_jL = linalg::Matrix::det3x3( l_jacL );
        if( l_exR ) l_jR = linalg::Matrix::det3x3( l_jacR );
#else
#error number of dimensions not supported.
#endif

        // Remark: For triangles and tets the determinant is not equal to the volume
        //         We apply the effect of the inverse mass matrix right here.
        //         Effectively this just scales the flux solver by the volume of the element
        if( T_SDISC.ELEMENT == TRIA3 && ORDER == 1 ) {
          if( l_exL ) l_jL /= (real_mesh) 2;
          if( l_exR ) l_jR /= (real_mesh) 2;
        }
        else if( T_SDISC.ELEMENT == TET4 && ORDER == 1 ) {
          if( l_exL ) l_jL /= (real_mesh) 6;
          if( l_exR ) l_jR /= (real_mesh) 6;
        }

        // ensure positive determinants
        assert( !l_exL || l_jL > 0 );  assert( !l_exR || l_jR > 0 ); assert( i_faceChars[l_fa].area > 0 );

        real_base l_scaleL=std::numeric_limits<real_base>::max();
        real_base l_scaleR=std::numeric_limits<real_base>::max();
        if( l_exL ) l_scaleL = -i_faceChars[l_fa].area / l_jL;
        if( l_exR ) l_scaleR = -i_faceChars[l_fa].area / l_jR;

#if defined PP_T_ELEMENTS_TET4
        // the det of the surface-jac is twice the area of the triangle
        // TODO: Replace this formulation with Jacobi-determinants.
        if( l_exL ) l_scaleL *= 2;
        if( l_exR ) l_scaleR *= 2;
#endif

        for( unsigned int l_q1 = 0; l_q1 < N_QUANTITIES; l_q1++ ) {
          for( unsigned int l_q2 = 0; l_q2 < N_QUANTITIES; l_q2++ ) {
            // check for valid values in the solvers
            assert( !l_exL || o_fluxSolversOwn[  l_elL][l_fIdL].solver[l_q1][l_q2] != std::numeric_limits<real_base>::max() );
            assert( !l_exR || o_fluxSolversOwn[  l_elR][l_fIdR].solver[l_q1][l_q2] != std::numeric_limits<real_base>::max() );
            assert( !l_exL || o_fluxSolversNeigh[l_elL][l_fIdL].solver[l_q1][l_q2] != std::numeric_limits<real_base>::max() );
            assert( !l_exR || o_fluxSolversNeigh[l_elR][l_fIdR].solver[l_q1][l_q2] != std::numeric_limits<real_base>::max() );

            // scale left elements' solvers
            if( l_exL ) o_fluxSolversOwn[  l_elL][l_fIdL].solver[l_q1][l_q2] *= l_scaleL;
            if( l_exL ) o_fluxSolversNeigh[l_elL][l_fIdL].solver[l_q1][l_q2] *= l_scaleL;

            // scale right elements' solvers
            if( l_exR ) o_fluxSolversOwn[  l_elR][l_fIdR].solver[l_q1][l_q2] *= l_scaleR;
            if( l_exR ) o_fluxSolversNeigh[l_elR][l_fIdR].solver[l_q1][l_q2] *= l_scaleR;
          }
        }
      }

      // take care of duplicated elements
      for( int_el l_el = 0; l_el < i_nElements; l_el++ ) {
        // get dominant id
        int_el l_elDo = i_elMeDa[ i_elDaMe[l_el] ];

        for( unsigned short l_fa = 0; l_fa < C_ENT[T_SDISC.ELEMENT].N_FACES; l_fa++ ) {
          for( unsigned short l_q1 = 0; l_q1 < N_QUANTITIES; l_q1++ ) {
            for( unsigned short l_q2 = 0; l_q2 < N_QUANTITIES; l_q2++ ) {
              o_fluxSolversOwn[l_el][l_fa].solver[l_q1][l_q2]   = o_fluxSolversOwn[l_elDo][l_fa].solver[l_q1][l_q2];
              o_fluxSolversNeigh[l_el][l_fa].solver[l_q1][l_q2] = o_fluxSolversNeigh[l_elDo][l_fa].solver[l_q1][l_q2];
            }
          }
        }
      }

    }
};

#endif
