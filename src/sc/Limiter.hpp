/**
 * @file This file is part of EDGE.
 *
 * @author Alexander Breuer (anbreuer AT ucsd.edu)
 *
 * @section LICENSE
 * Copyright (c) 2017, Regents of the University of California
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
 * Subcell limiter.
 **/
#ifndef SC_LIMITER_HPP
#define SC_LIMITER_HPP

#include "constants.hpp"
#include "io/logging.h"
#include "linalg/Matrix.h"
#include "impl/advection/solvers/AderDg.hpp"

namespace edge {
  namespace sc {
    template< t_entityType   TL_T_EL,
              unsigned short TL_O_SP,
              unsigned short TL_N_CRS >
    class Limiter;
  }
}

/**
 * Subcell limiter.
 *
 * @paramt TL_T_EL element type.
 * @paramt TL_O_SP spatial order of the DG-solver.
 * @paramt TL_N_CRS number of fused forward runs.
 **/
template< t_entityType   TL_T_EL,
          unsigned short TL_O_SP,
          unsigned short TL_N_CRS >
class edge::sc::Limiter {
  private:
    //! number of dimensions
    static unsigned short const TL_N_DIS = C_ENT[TL_T_EL].N_DIM;

    //! number of quantities
    static unsigned short const TL_N_QTS = 1; // TODO: should be a template parameter

    //! number of vertices
    static unsigned short const TL_N_VES = C_ENT[TL_T_EL].N_VERTICES;

    //! number of faces
    static unsigned short const TL_N_FAS = C_ENT[TL_T_EL].N_FACES;

    //! number of DG modes
    static unsigned short const TL_N_MDS = CE_N_ELEMENT_MODES( TL_T_EL, TL_O_SP );

    //! number of DG flux matrices
    static unsigned short const TL_N_FMAT = CE_N_FLUX_MATRICES( TL_T_EL );

    //! number of sub-edges per element edge
    static unsigned short const TL_N_SES = CE_N_SUB_EDGES( TL_N_DIS, TL_O_SP );

    //! number of sub-faces per element face
    static unsigned short const TL_N_SFS = CE_N_SUB_FACES( TL_T_EL, TL_O_SP );

    //! number of subcells
    static unsigned short const TL_N_SCS = CE_N_SUB_CELLS( TL_T_EL, TL_O_SP );

    /**
     * Determines, based on the sub-face type, the id of the flux solver for the adjacent sub-cell's contribution.
     *
     * @param i_scTySf type of the sub-face.
     * @return id of the of the adjacent sub-cell's flux solver.
     **/
    static unsigned short fsIdAd( unsigned short i_scTySf ) {
      return (i_scTySf <   TL_N_FAS) ? i_scTySf + TL_N_FAS :
             (i_scTySf < 2*TL_N_FAS) ? i_scTySf - TL_N_FAS :
             (i_scTySf < 3*TL_N_FAS) ? i_scTySf + TL_N_FAS :
                                       i_scTySf - TL_N_FAS;
    }

    /**
     * Rolls back the DG surface integration for an DG element's face and replaces it with the sub-cell integral.
     * The face integral is replaced only if the candidate solution of the adjacent element is not admissible.
     *   Remark: This function assumes a limited plus element and an adjacent (face as bridge), limited element.
     *
     * @param i_dt time step.
     * @param i_admAdP admissibility of the adjacent element's DG solution for the previous solution.
     * @param i_admAdC admissibility of the adjacent element's DG solution for the candidate solution.
     * @param i_fMatDg DG flux matrix for the local contribution.
     * @param i_fMatDgAd DG flux matrix for the contribution of the adjacent element.
     * @param i_scatterSf scatter operator for the element's sub-cells adjacent to the face in the reference element.
     * @param i_scatterSfAd scatter operator for the adjacent element's sub-cells adjacent to the face w.r.t. the reference element.
     * @param i_sfInt sfInt operator, which computes the DG face integral from the sub-cell fluxes.
     * @param i_fluxSolver flux solver for the element's local contribution to the face integral.
     * @param i_fluxSolverAd flux solver for the adjacent element's contribution to the face integral.
     * @param i_tDofsDgP time integrated DOFs of the DG element's previous solution.
     * @param i_tDofsDgAdP time integrated DOFs of the adjacent element's previous solution
     * @param i_tDofsScAdP previous sub-cell limited solution of the adjacent element for sub-cells adjacent to the element's face (if available).
     * @param i_dofsDgP DOFs of the element's previous DG solution.
     * @param i_dofsDgAdP DOFs of the adjacent element's previous DG solution.
     * @param io_dofsDg current candidate DG solution of the element, for which the DG surface integral is replaced with the sub-cell surface integral.
     *
     * @paramt TL_T_REAL floating point type.
     **/
    template< typename TL_T_REAL >
    static void surfIntRb( TL_T_REAL         i_dt,
                           bool      const   i_admAdP[TL_N_CRS],
                           bool      const   i_admAdC[TL_N_CRS],
                           TL_T_REAL const   i_fMatDg[TL_N_MDS][TL_N_MDS],
                           TL_T_REAL const   i_fMatDgAd[TL_N_MDS][TL_N_MDS],
                           TL_T_REAL const   i_scatterSf[TL_N_MDS][TL_N_SFS],
                           TL_T_REAL const   i_scatterSfAd[TL_N_MDS][TL_N_SFS],
                           TL_T_REAL const   i_sfInt[TL_N_SFS][TL_N_MDS],
                           TL_T_REAL const   i_fluxSolver,
                           TL_T_REAL const   i_fluxSolverAd,
                           TL_T_REAL const   i_tDofsDgP[  TL_N_QTS][TL_N_MDS][TL_N_CRS],
                           TL_T_REAL const   i_tDofsDgAdP[TL_N_QTS][TL_N_MDS][TL_N_CRS],
                           TL_T_REAL const   i_tDofsScAdP[TL_N_QTS][TL_N_SFS][TL_N_CRS],
                           TL_T_REAL const   i_dofsDgP[   TL_N_QTS][TL_N_MDS][TL_N_CRS],
                           TL_T_REAL const   i_dofsDgAdP[ TL_N_QTS][TL_N_MDS][TL_N_CRS],
                           TL_T_REAL         io_dofsDg[   TL_N_QTS][TL_N_MDS][TL_N_CRS] ) {
      // determine if a rollback is required: a single fused sim of the limited element is not admissible
      bool l_rb = false;
      for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
        l_rb = l_rb || (i_admAdC[l_cr] == false);
      }

      // only continue if a rollback is required
      if( l_rb ) {
        // sub-cell solution on both sides of the DG element's face
        TL_T_REAL l_scFa[2][TL_N_QTS][TL_N_SFS][TL_N_CRS];

        // pure DG-scatter for the element. limiter replaces entire solution anyways, if replacement available.
        edge::sc::Kernels<
          TL_T_EL,
          TL_O_SP,
          TL_N_QTS,
          TL_N_CRS >::scatterFaVanilla( i_dofsDgP,
                                        i_scatterSf,
                                        l_scFa[0] );

        // DG-scatter and replacement by stored sub-cell DOFs (if available) for the adjacent element
        edge::sc::Kernels<
          TL_T_EL,
          TL_O_SP,
          TL_N_QTS,
          TL_N_CRS >::scatterReplaceFaVanilla( i_dofsDgAdP,
                                               i_scatterSfAd,
                                               i_tDofsScAdP,
                                               i_admAdP,
                                               l_scFa[1] );

        // compute sub-cell FV fluxes between the two DG elements
        TL_T_REAL l_fluxes[TL_N_QTS][TL_N_SFS][TL_N_CRS];

        for( unsigned short l_sf = 0; l_sf < TL_N_SFS; l_sf++ ) {
          for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
            l_fluxes[0][l_sf][l_cr]  = i_fluxSolver   * l_scFa[0][0][l_sf][l_cr]; // own contrib
            l_fluxes[0][l_sf][l_cr] += i_fluxSolverAd * l_scFa[1][0][l_sf][l_cr]; // adj contrib
            l_fluxes[0][l_sf][l_cr] *= i_dt;
          }
        }

        // face integral of the sub-cell fluxes
        TL_T_REAL l_sIntSc[TL_N_QTS][TL_N_MDS][TL_N_CRS];

        // integrate fluxes over DG element's boundary and project back to DG space
        edge::sc::Kernels<
          TL_T_EL,
          TL_O_SP,
          TL_N_QTS,
          TL_N_CRS >::sfIntVanilla( l_fluxes,
                                    i_sfInt,
                                    l_sIntSc );

        // face integral of the DG fluxes
        TL_T_REAL l_sIntDg[TL_N_QTS][TL_N_MDS][TL_N_CRS];
        for( unsigned short l_md = 0; l_md < TL_N_MDS; l_md++ )
          for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ )
            l_sIntDg[0][l_md][l_cr] = 0;

        // local
        advection::solvers::AderDg<
          TL_T_EL,
          TL_O_SP,
          TL_O_SP,
          TL_N_CRS >::surfInt( i_fMatDg,
                               i_fluxSolver,
                               i_tDofsDgP[0],
                               l_sIntDg[0] );

        // adjacent
        advection::solvers::AderDg<
          TL_T_EL,
          TL_O_SP,
          TL_O_SP,
          TL_N_CRS >::surfInt( i_fMatDgAd,
                               i_fluxSolverAd,
                               i_tDofsDgAdP[0],
                               l_sIntDg[0] );

        // update the DOFs accordingly
        for( unsigned short l_md = 0; l_md < TL_N_MDS; l_md++ ) {
          for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
            if( i_admAdC[l_cr] == false ) {
              io_dofsDg[0][l_md][l_cr] -= l_sIntDg[0][l_md][l_cr];
              io_dofsDg[0][l_md][l_cr] += l_sIntSc[0][l_md][l_cr];
            }
          }
        }

      }
    }

    /**
     * Computes the limited solution of the given element.
     *
     * @param i_dt time step.
     * @param i_scSfSc sub-cells adjacent to sub-cells (sub-faces as bridge).
     * @param i_scTySf types of sub-faces adjacent to sub-cells.
     * @param i_fIdElFaEl faces ids w.r.t. the reference element of adjacent elements.
     * @param i_admP admissibility of the element's previous solution.
     * @param i_admAdP admissiblity of the element's adjacent elements' previous solutions.
     * @param i_admC admissiblity of the element's candidate solution.
     * @param i_scatter sub-cell scatter operator.
     * @param i_scatteSf sub-cell scatter operator for the sub-cells at the element's surface.
     * @param i_fluxS DG flux solver.
     * @param i_dofsDgP DOFs of the previous DG solution.
     * @param i_dofsScAdP sub-cell solution of the adjacent sub-cells (DG-faces as bridge).
     * @param i_dofsDgAdP DG solution of the adjacent DG-elements (faces as bridge).
     * @param io_dofsSc will be updated with the limited sub-cell solution.
     **/
    template< typename TL_T_LID,
              typename TL_T_REAL >
    static void limit( TL_T_REAL              i_dt,
                       TL_T_LID       const   i_scSfSc[TL_N_SCS + TL_N_FAS * TL_N_SFS][TL_N_FAS],
                       unsigned short const   i_scTySf[TL_N_SCS][TL_N_FAS],
                       unsigned short const   i_fIdElFaEl[TL_N_FAS],
                       bool           const   i_admP[TL_N_CRS],
                       bool           const  *i_admAdP[TL_N_FAS],
                       bool           const   i_admC[TL_N_CRS],
                       TL_T_REAL      const   i_scatter[TL_N_MDS][TL_N_SCS],
                       TL_T_REAL      const   i_scatterSf[TL_N_FAS][TL_N_MDS][TL_N_SFS],
                       TL_T_REAL      const  *i_fluxS[TL_N_FAS*4],
                       TL_T_REAL      const   i_dofsDgP[TL_N_QTS][TL_N_MDS][TL_N_CRS],
                       TL_T_REAL      const (*i_dofsScAdP[TL_N_FAS])[TL_N_SFS][TL_N_CRS],
                       TL_T_REAL      const (*i_dofsDgAdP[TL_N_FAS])[TL_N_MDS][TL_N_CRS],
                       TL_T_REAL              io_dofsSc[TL_N_QTS][TL_N_SCS][TL_N_CRS] ) {
      // TODO: outsource, scaling of the dg solver
      TL_T_REAL l_dgSca = i_dt * TL_N_SCS;
                l_dgSca /= TL_N_SFS;
      // DG solver scaled by inverse det of surface-jac (twice the area of the triangle)
      if( TL_T_EL == TRIA3 ) l_dgSca *= 2;

      TL_T_REAL l_subCell[TL_N_QTS][TL_N_SCS][TL_N_CRS]; // TODO: scratch

      // derive element's sub-cell solution
      edge::sc::Kernels<
        TL_T_EL,
        TL_O_SP,
        TL_N_QTS,
        TL_N_CRS >::scatterReplaceVanilla( i_dofsDgP,
                                           i_scatter,
                                           io_dofsSc,
                                           i_admP,
                                           l_subCell );

      // derive sub-cell solution of adjacent elements (faces as bride, only sub-cells adjacent to resp. DG-faces)
      TL_T_REAL l_subCellNe[TL_N_FAS][TL_N_QTS][TL_N_SFS][TL_N_CRS];

      for( unsigned short l_fa = 0; l_fa < TL_N_FAS; l_fa++ ) {
        unsigned short l_fId = i_fIdElFaEl[l_fa];

        edge::sc::Kernels<
          TL_T_EL,
          TL_O_SP,
          TL_N_QTS,
          TL_N_CRS >::scatterReplaceFaVanilla( i_dofsDgAdP[l_fa],
                                               i_scatterSf[l_fId],
                                               i_dofsScAdP[l_fa],
                                               i_admAdP[l_fa],
                                               l_subCellNe[l_fa] );
      }

      // assemble sub-grid
      // TODO: replace with strides in scatter ops
      TL_T_REAL l_sg[TL_N_QTS][TL_N_SCS + TL_N_FAS * TL_N_SFS][TL_N_CRS];
      for( unsigned short l_sc = 0; l_sc < TL_N_SCS; l_sc++ )
        for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ )
          l_sg[0][l_sc][l_cr] = l_subCell[0][l_sc][l_cr];

      for( unsigned short l_fa = 0; l_fa < TL_N_FAS; l_fa++ )
        for( unsigned short l_sf = 0; l_sf < TL_N_SFS; l_sf++ )
          for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ )
            l_sg[0][TL_N_SCS + l_fa*TL_N_SFS + l_sf][l_cr] = l_subCellNe[l_fa][0][l_sf][l_cr];

      // compute sub-cell solution
      for( unsigned short l_sc = 0; l_sc < TL_N_SCS; l_sc++ ) {
        // init with current solution
        for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ )
          io_dofsSc[0][l_sc][l_cr] = l_sg[0][l_sc][l_cr];

        // iterate over faces of sub-cell
        for( unsigned short l_fa = 0; l_fa < TL_N_FAS; l_fa++ ) {
          // assemble ids of the flux solvers
          unsigned short l_fsId[2];
          l_fsId[0] = i_scTySf[l_sc][l_fa];
          l_fsId[1] = fsIdAd( l_fsId[0] );

          // scaling of the flux-solver
          TL_T_REAL l_fsSca = (l_fsId[0] < l_fsId[1]) ? l_dgSca : -l_dgSca;

          // iterate over fused runs
          for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
            // own contrib
            io_dofsSc[0][l_sc][l_cr] += l_sg[0][  l_sc][l_cr] * i_fluxS[ l_fsId[0] ][0] * l_fsSca;

            // neighboring contrib
            TL_T_LID l_neSc = i_scSfSc[l_sc][l_fa];
            io_dofsSc[0][l_sc][l_cr] += l_sg[0][l_neSc][l_cr] * i_fluxS[ l_fsId[1] ][0] * l_fsSca;
          }
        }
      }
    }

  public:
    /**
     * Applies the a-posteriori sub-cell limiter.
     *
     * @param i_first first limited plus element (limited + adjacent elements through faces).
     * @param i_nLps number of limited plus elements.
     * @param i_dt time step.
     * @param i_fMat DG flux matrices.
     * @param i_scOps sub-cell operators.
     * @param i_scConn sub-cell connectivity.
     * @param i_elChars element characteristics.
     * @param i_fsDg DG flux solvers.
     * @param i_elFaEl DG elements adjacent to elements (faces as bridge).
     * @param i_vIdElFaEl vertex ids of adjacent DG-elements (faces as bridge).
     * @param i_fIdElFaEl face ids of adjacent DG-elements (faces as bridge).
     * @param i_admP admissibility of the previous solution.
     * @param i_admC admissibility of the candidate solution.
     * @param o_admL will be set to admissibility of the limited solution.
     * @param i_tDofsDg temporary DOFs of the DG-solution.
     * @param io_dofsDg will be updated with DG-projection of the limited solution.
     * @param i_tDofsSc temporary sub-cell solution for sub-cells adjacent to DG-faces.
     * @param io_dofsSc will be updated with sub-cell solution.
     * @param i_extP extrema of the previous solution.
     * @param o_extL will be set to extrema of the limited solution.
     *
     * @paramt TL_T_LID integral type of local ids.
     * @paramt TL_T_REAL floating point type.
     **/
    template< typename TL_T_LID,
              typename TL_T_REAL >
    static void aPost( TL_T_LID                                     i_first,
                       TL_T_LID                                     i_nLps,
                       TL_T_REAL                                    i_dt,
                       TL_T_REAL           const                    i_fMat[TL_N_FMAT][TL_N_MDS][TL_N_MDS],
                       sc::t_ops<
                         TL_T_REAL,
                         TL_T_EL,
                         TL_O_SP >         const                    i_scOps,
                       sc::t_connect<
                         TL_T_LID,
                         TL_T_EL,
                         TL_O_SP >         const                    i_scConn,
                       TL_T_REAL           const                  (*i_fsDg)[TL_N_FAS*2],
                       TL_T_LID            const                  (*i_elFaEl)[TL_N_FAS],
                       unsigned short      const                  (*i_vIdElFaEl)[TL_N_VES],
                       unsigned short      const                  (*i_fIdElFaEl)[TL_N_FAS],
                       bool                const                  (*i_admP)[TL_N_CRS],
                       bool                const                  (*i_admC)[TL_N_CRS],
                       bool                                       (*o_admL)[TL_N_CRS],
                       TL_T_REAL           const (* const * const   i_tDofsDg[2])[TL_N_MDS][TL_N_CRS],
                       TL_T_REAL                                  (*io_dofsDg)[TL_N_QTS][TL_N_MDS][TL_N_CRS],
                       TL_T_REAL           const                  (*i_tDofsSc)[TL_N_FAS][TL_N_QTS][TL_N_SFS][TL_N_CRS],
                       TL_T_REAL                                  (*io_dofsSc)[TL_N_QTS][TL_N_SCS][TL_N_CRS],
                       TL_T_REAL                                  (*i_extP)[2][TL_N_QTS][TL_N_CRS],
                       TL_T_REAL                                  (*o_extL)[2][TL_N_QTS][TL_N_CRS] ) {
      // iterate over limited plus elements
      for( TL_T_LID l_lp = i_first; l_lp < i_first+i_nLps; l_lp++ ) {
        // dense id
        TL_T_LID l_el = i_scConn.lpEl[l_lp];

        // limited id
        TL_T_LID l_li = i_scConn.lpLi[l_lp];

        // assemble pointers to flux solvers
        TL_T_REAL const * l_fluxS[TL_N_FAS*2 + TL_N_FAS*2];
        // TODO: place holder for homogeneous solvers
        for( unsigned short l_fs = 0; l_fs < TL_N_FAS*2; l_fs++ ) {
          l_fluxS[l_fs             ] = &i_fsDg[l_el][l_fs];
          l_fluxS[l_fs + TL_N_FAS*2] = &i_fsDg[l_el][l_fs];
        }

        // derive the adjacent limited elements (faces as bridge)
        TL_T_LID l_lpFaLi[TL_N_FAS];
        for( unsigned short l_fa = 0; l_fa < TL_N_FAS; l_fa++ ) {
          if( i_scConn.lpFaLp[l_lp][l_fa] < std::numeric_limits< TL_T_LID >::max() )
            l_lpFaLi[l_fa] = i_scConn.lpLi[ i_scConn.lpFaLp[l_lp][l_fa] ];
          else
            l_lpFaLi[l_fa] = std::numeric_limits< TL_T_LID >::max();
        }

        // iterate over faces and replace DG surface integral with sub-cell surface integral if required
        for( unsigned short l_fa = 0; l_fa < TL_N_FAS; l_fa++ ) {
EDGE_CHECK_EQ( i_vIdElFaEl[l_el][l_fa], 0 );

          TL_T_LID l_liAd = l_lpFaLi[l_fa];

          // only continue if the face is adjacent to a limited element
          if( l_liAd != std::numeric_limits< TL_T_LID >::max() ) {
            TL_T_LID l_elAd = i_elFaEl[l_el][l_fa];

            // id of the face of adjacent element w.r.t. reference element
            unsigned short l_fId = i_fIdElFaEl[l_el][l_fa];

            unsigned short l_fMatId = advection::solvers::AderDg<
              TL_T_EL,
              TL_O_SP,
              TL_O_SP,
              TL_N_CRS >::fMatId( MESH_TYPE_NONE,
                                  l_fa,
                                  i_vIdElFaEl[l_el][l_fa],
                                  l_fId );

            // rollback DG surface integration of the face and replace with sub-cell integral
            surfIntRb( i_dt,
                       i_admP[l_liAd],
                       i_admC[l_liAd],
                       i_fMat[l_fa],
                       i_fMat[l_fMatId],
                       i_scOps.scatterSurf[l_fa],
                       i_scOps.scatterSurf[TL_N_FAS+l_fId],
                       i_scOps.sfInt[l_fa],
                       i_fsDg[l_el][l_fa],
                       i_fsDg[l_el][TL_N_FAS+l_fa],
                       i_tDofsDg[0][l_el],
                       i_tDofsDg[0][l_elAd],
                       i_tDofsSc[l_liAd][l_fId],
                       i_tDofsDg[1][l_el],
                       i_tDofsDg[1][l_elAd],
                       io_dofsDg[l_el] );
          }
        }

        /*
         * perform limiting if required
         */
        if( l_li != std::numeric_limits< TL_T_LID >::max() ) {
          // derive if limiting is requ. and init admissibility of the limited solution
          bool l_lim = false;
          for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
            l_lim = l_lim || (i_admC[l_li][l_cr] == false);
            o_admL[l_li][l_cr] = i_admC[l_li][l_cr];
          }

          if( l_lim ) {
            // admissibility of adjacent elements
            bool const *l_admAdP[TL_N_FAS];
            // sub-cell solution of adjacent elmeents
            TL_T_REAL const (*l_dofsScAdP[TL_N_FAS])[TL_N_SFS][TL_N_CRS];
            // DG solution of adjacent elements
            TL_T_REAL const (*l_dofsDgAdP[TL_N_FAS])[TL_N_MDS][TL_N_CRS];

            // assemble info of adjacent (face as bridge) elements
            for( unsigned short l_fa = 0; l_fa < TL_N_FAS; l_fa++ ) {
              unsigned short l_fId = i_fIdElFaEl[l_el][l_fa];
              TL_T_LID l_adEl = i_elFaEl[l_el][l_fa];
              TL_T_LID l_adLi = l_lpFaLi[l_fa];

              if( l_adLi < std::numeric_limits< TL_T_LID >::max() ) {
                l_admAdP[l_fa]    = i_admP[l_adLi];
                l_dofsScAdP[l_fa] = i_tDofsSc[l_adLi][l_fId];
                l_dofsDgAdP[l_fa] = i_tDofsDg[1][l_adEl];
              }
              else {
                l_admAdP[l_fa]    = nullptr;
                l_dofsScAdP[l_fa] = nullptr;
                l_dofsDgAdP[l_fa] = nullptr;
              }
            }

            // compute limited sub-cell solution
            limit( i_dt,
                   i_scConn.scSfSc,
                   i_scConn.scTySf,
                   i_fIdElFaEl[l_el],
                   i_admP[l_li],
                   l_admAdP,
                   i_admC[l_li],
                   i_scOps.scatter,
                   i_scOps.scatterSurf+TL_N_FAS,
                   l_fluxS,
                   i_tDofsDg[1][l_el],
                   l_dofsScAdP,
                   l_dofsDgAdP,
                   io_dofsSc[l_li] );

            // DG DOFs of the projected sub-cell solution
            TL_T_REAL l_dofsDg[TL_N_QTS][TL_N_MDS][TL_N_CRS];
            // project the DG solution back
            edge::sc::Kernels< TL_T_EL,
                               TL_O_SP,
                               TL_N_QTS,
                               TL_N_CRS >::gatherVanilla( io_dofsSc[l_li],
                                                               i_scOps.gather,
                                                               l_dofsDg );
            // copy only relevant DOFs back
            for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
              if( i_admC[l_li][l_cr] == false ) {
                for( unsigned short l_md = 0; l_md < TL_N_MDS; l_md++ ) {
                  io_dofsDg[l_el][0][l_md][l_cr] = l_dofsDg[0][l_md][l_cr];
                }
              }
            }

            // update admissibility based on DG-projection of the sub-cell solution
            bool l_admL[TL_N_CRS];
            TL_T_REAL l_scDgL[TL_N_QTS][TL_N_SCS][TL_N_CRS];
            TL_T_REAL l_exDgL[2][N_QUANTITIES][TL_N_CRS];

            edge::sc::Kernels< TL_T_EL,
                               TL_O_SP,
                               TL_N_QTS,
                               TL_N_CRS >::dgExtremaVanilla(  io_dofsDg[l_el],
                                                              i_scOps.scatter,
                                                              l_scDgL,
                                                              l_exDgL[0],
                                                              l_exDgL[1] );

            edge::sc::Detections< 1,
                                  TL_N_CRS >::dmp( i_extP[ i_scConn.liEx[l_li] ],
                                                   i_extP,
                                                   l_exDgL,
                                                   i_scConn.liVeEx[l_li+1]-i_scConn.liVeEx[l_li],
                                                   i_scConn.liVeEx[l_li],
                                                   l_admL );
            for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
              if( i_admC[l_li][l_cr] == false ) {
                o_admL[l_li][l_cr] = l_admL[l_cr];
              }
            }

//for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
//  o_admL[l_li][l_cr] = false;
//}

            // update the extrema
            TL_T_REAL l_extSc[2][1][TL_N_CRS];
            edge::sc::Kernels< TL_T_EL,
                               TL_O_SP,
                               TL_N_QTS,
                               TL_N_CRS >::scExtrema(  io_dofsSc[l_li],
                                                       l_extSc[0],
                                                       l_extSc[1] );

            for( unsigned short l_cr = 0; l_cr < TL_N_CRS; l_cr++ ) {
              if( i_admC[l_li][l_cr] == false ) {
                if( l_admL[l_cr] == false ) {
                  o_extL[l_li][0][0][l_cr] = l_extSc[0][0][l_cr];
                  o_extL[l_li][1][0][l_cr] = l_extSc[1][0][l_cr];
                }
                else {
                  o_extL[l_li][0][0][l_cr] = l_exDgL[0][0][l_cr];
                  o_extL[l_li][1][0][l_cr] = l_exDgL[1][0][l_cr];
                }
              }
            }


          }
        }

      }
    }
};

#endif