// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  Copyright (C) 2009-2013 by Andreas Lauser
  Copyright (C) 2012 by Bernd Flemisch

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
 * \file
 * \copydoc Opm::FluidSystems::H2ON2LiquidPhase
 */
#ifndef OPM_H2O_N2_LIQUIDPHASE_FLUID_SYSTEM_HPP
#define OPM_H2O_N2_LIQUIDPHASE_FLUID_SYSTEM_HPP

#include "BaseFluidSystem.hpp"
#include "NullParameterCache.hpp"

#include <opm/material/IdealGas.hpp>
#include <opm/material/components/N2.hpp>
#include <opm/material/components/H2O.hpp>
#include <opm/material/components/SimpleH2O.hpp>
#include <opm/material/components/TabulatedComponent.hpp>
#include <opm/material/binarycoefficients/H2O_N2.hpp>
#include <opm/material/common/Valgrind.hpp>

#include <opm/material/common/Exceptions.hpp>
#include <opm/material/common/ErrorMacros.hpp>

#include <iostream>
#include <cassert>

namespace Opm {
namespace FluidSystems {

/*!
 * \ingroup Fluidsystems
 *
 * \brief A liquid-phase-only fluid system with water and nitrogen as
 *        components.
 */
template <class Scalar, bool useComplexRelations = true>
class H2ON2LiquidPhase
    : public BaseFluidSystem<Scalar, H2ON2LiquidPhase<Scalar, useComplexRelations> >
{
    typedef H2ON2LiquidPhase<Scalar, useComplexRelations> ThisType;
    typedef BaseFluidSystem<Scalar, ThisType> Base;

    // convenience typedefs
    typedef Opm::H2O<Scalar> IapwsH2O;
    typedef Opm::TabulatedComponent<Scalar, IapwsH2O > TabulatedH2O;
    typedef Opm::N2<Scalar> SimpleN2;

public:
    //! \copydoc BaseFluidSystem::ParameterCache
    typedef NullParameterCache ParameterCache;

    /****************************************
     * Fluid phase related static parameters
     ****************************************/

    //! \copydoc BaseFluidSystem::numPhases
    static const int numPhases = 1;

    //! Index of the liquid phase
    static const int liquidPhaseIdx = 0;

    //! \copydoc BaseFluidSystem::phaseName
    static const char *phaseName(OPM_OPTIM_UNUSED unsigned phaseIdx)
    {
        assert(phaseIdx == liquidPhaseIdx);

        return "liquid";
    }

    //! \copydoc BaseFluidSystem::isLiquid
    static bool isLiquid(unsigned /*phaseIdx*/)
    {
        //assert(phaseIdx == liquidPhaseIdx);
        return true; //only water phase present
    }

    //! \copydoc BaseFluidSystem::isCompressible
    static bool isCompressible(unsigned /*phaseIdx*/)
    {
        //assert(0 <= phaseIdx && phaseIdx < numPhases);
        // the water component decides for the liquid phase...
        return H2O::liquidIsCompressible();
    }

    //! \copydoc BaseFluidSystem::isIdealGas
    static bool isIdealGas(unsigned /*phaseIdx*/)
    {
        //assert(0 <= phaseIdx && phaseIdx < numPhases);
        return false; // not a gas (only liquid phase present)
    }

    //! \copydoc BaseFluidSystem::isIdealMixture
    static bool isIdealMixture(unsigned /*phaseIdx*/)
    {
        //assert(0 <= phaseIdx && phaseIdx < numPhases);
        // we assume Henry's and Rault's laws for the water phase and
        // and no interaction between gas molecules of different
        // components, so all phases are ideal mixtures!
        return true;
    }

    /****************************************
     * Component related static parameters
     ****************************************/

    //! \copydoc BaseFluidSystem::numComponents
    static const int numComponents = 2;

    //! The index of the water component
    static const int H2OIdx = 0;
    //! The index of the component for molecular nitrogen
    static const int N2Idx = 1;

    //! The type of the component for pure water
    typedef TabulatedH2O H2O;
    //typedef SimpleH2O H2O;
    //typedef IapwsH2O H2O;

    //! The type of the component for pure molecular nitrogen
    typedef SimpleN2 N2;

    //! \copydoc BaseFluidSystem::componentName
    static const char *componentName(unsigned compIdx)
    {
        static const char *name[] = {
            H2O::name(),
            N2::name()
        };

        assert(0 <= compIdx && compIdx < numComponents);
        return name[compIdx];
    }

    //! \copydoc BaseFluidSystem::molarMass
    static Scalar molarMass(unsigned compIdx)
    {
        //assert(0 <= compIdx && compIdx < numComponents);
        return (compIdx == H2OIdx)
            ? H2O::molarMass()
            : (compIdx == N2Idx)
            ? N2::molarMass()
            : 1e100;
    }

    /*!
     * \brief Critical temperature of a component [K].
     *
     * \param compIdx The index of the component to consider
     */
    static Scalar criticalTemperature(unsigned compIdx)
    {
        //assert(0 <= compIdx && compIdx < numComponents);
        return (compIdx == H2OIdx)
            ? H2O::criticalTemperature()
            : (compIdx == N2Idx)
            ? N2::criticalTemperature()
            : 1e100;
    }

    /*!
     * \brief Critical pressure of a component [Pa].
     *
     * \param compIdx The index of the component to consider
     */
    static Scalar criticalPressure(unsigned compIdx)
    {
        //assert(0 <= compIdx && compIdx < numComponents);
        return (compIdx == H2OIdx)
            ? H2O::criticalPressure()
            : (compIdx == N2Idx)
            ? N2::criticalPressure()
            : 1e100;
    }

    /*!
     * \brief The acentric factor of a component [].
     *
     * \param compIdx The index of the component to consider
     */
    static Scalar acentricFactor(unsigned compIdx)
    {
        //assert(0 <= compIdx && compIdx < numComponents);
        return (compIdx == H2OIdx)
            ? H2O::acentricFactor()
            : (compIdx == N2Idx)
            ? N2::acentricFactor()
            : 1e100;
    }

    /****************************************
     * thermodynamic relations
     ****************************************/

    /*!
     * \copydoc BaseFluidSystem::init
     *
     * If a tabulated H2O component is used, we do our best to create
     * tables that always work.
     */
    static void init()
    {
        init(/*tempMin=*/273.15,
             /*tempMax=*/623.15,
             /*numTemp=*/100,
             /*pMin=*/0.0,
             /*pMax=*/20e6,
             /*numP=*/200);
    }

    /*!
     * \brief Initialize the fluid system's static parameters using
     *        problem specific temperature and pressure ranges.
     *
     * \param tempMin The minimum temperature used for tabulation of water [K]
     * \param tempMax The maximum temperature used for tabulation of water [K]
     * \param nTemp The number of ticks on the temperature axis of the  table of water
     * \param pressMin The minimum pressure used for tabulation of water [Pa]
     * \param pressMax The maximum pressure used for tabulation of water [Pa]
     * \param nPress The number of ticks on the pressure axis of the  table of water
     */
    static void init(Scalar tempMin, Scalar tempMax, unsigned nTemp,
                     Scalar pressMin, Scalar pressMax, unsigned nPress)
    {
        if (H2O::isTabulated) {
            TabulatedH2O::init(tempMin, tempMax, nTemp,
                               pressMin, pressMax, nPress);
        }
    }

    //! \copydoc BaseFluidSystem::density
    template <class FluidState, class LhsEval = typename FluidState::Scalar>
    static LhsEval density(const FluidState &fluidState,
                           const ParameterCache &/*paramCache*/,
                           unsigned phaseIdx)
    {
        typedef Opm::MathToolbox<typename FluidState::Scalar> FsToolbox;

        assert(0 <= phaseIdx  && phaseIdx < numPhases);

        const auto& T = FsToolbox::template toLhs<LhsEval>(fluidState.temperature(phaseIdx));
        const auto& p = FsToolbox::template toLhs<LhsEval>(fluidState.pressure(phaseIdx));

        LhsEval sumMoleFrac = 0;
        for (unsigned compIdx = 0; compIdx < numComponents; ++compIdx)
            sumMoleFrac += FsToolbox::template toLhs<LhsEval>(fluidState.moleFraction(phaseIdx, compIdx));

        assert(phaseIdx == liquidPhaseIdx);

        if (!useComplexRelations)
            // assume pure water
            return H2O::liquidDensity(T, p);
        else
        {
            // See: Ochs 2008
            const LhsEval& rholH2O = H2O::liquidDensity(T, p);
            const LhsEval& clH2O = rholH2O/H2O::molarMass();

            const auto& xlH2O = FsToolbox::template toLhs<LhsEval>(fluidState.moleFraction(liquidPhaseIdx, H2OIdx));
            const auto& xlN2 = FsToolbox::template toLhs<LhsEval>(fluidState.moleFraction(liquidPhaseIdx, N2Idx));

            // this assumes each nitrogen molecule displaces exactly one
            // water molecule in the liquid
            return clH2O*(H2O::molarMass()*xlH2O + N2::molarMass()*xlN2)/sumMoleFrac;
        }
    }

    //! \copydoc BaseFluidSystem::viscosity
    template <class FluidState, class LhsEval = typename FluidState::Scalar>
    static LhsEval viscosity(const FluidState &fluidState,
                             const ParameterCache &/*paramCache*/,
                             unsigned phaseIdx)
    {
        typedef Opm::MathToolbox<typename FluidState::Scalar> FsToolbox;

        assert(phaseIdx == liquidPhaseIdx);

        const auto& T = FsToolbox::template toLhs<LhsEval>(fluidState.temperature(phaseIdx));
        const auto& p = FsToolbox::template toLhs<LhsEval>(fluidState.pressure(phaseIdx));

        // assume pure water for the liquid phase
        return H2O::liquidViscosity(T, p);
    }

    //! \copydoc BaseFluidSystem::fugacityCoefficient
    template <class FluidState, class LhsEval = typename FluidState::Scalar>
    static LhsEval fugacityCoefficient(const FluidState &fluidState,
                                       const ParameterCache &/*paramCache*/,
                                       unsigned phaseIdx,
                                       unsigned compIdx)
    {
        typedef Opm::MathToolbox<typename FluidState::Scalar> FsToolbox;

        assert(phaseIdx == liquidPhaseIdx);
        assert(0 <= compIdx  && compIdx < numComponents);

        const auto& T = FsToolbox::template toLhs<LhsEval>(fluidState.temperature(phaseIdx));
        const auto& p = FsToolbox::template toLhs<LhsEval>(fluidState.pressure(phaseIdx));

        if (compIdx == H2OIdx)
            return H2O::vaporPressure(T)/p;
        return Opm::BinaryCoeff::H2O_N2::henry(T)/p;
    }

    //! \copydoc BaseFluidSystem::diffusionCoefficient
    template <class FluidState, class LhsEval = typename FluidState::Scalar>
    static LhsEval diffusionCoefficient(const FluidState &fluidState,
                                        const ParameterCache &/*paramCache*/,
                                        unsigned phaseIdx,
                                        unsigned /*compIdx*/)

    {
        typedef Opm::MathToolbox<typename FluidState::Scalar> FsToolbox;

        assert(phaseIdx == liquidPhaseIdx);

        const auto& T = FsToolbox::template toLhs<LhsEval>(fluidState.temperature(phaseIdx));
        const auto& p = FsToolbox::template toLhs<LhsEval>(fluidState.pressure(phaseIdx));

        return BinaryCoeff::H2O_N2::liquidDiffCoeff(T, p);
    }

    //! \copydoc BaseFluidSystem::enthalpy
    template <class FluidState, class LhsEval = typename FluidState::Scalar>
    static LhsEval enthalpy(const FluidState &fluidState,
                            const ParameterCache &/*paramCache*/,
                            unsigned phaseIdx)
    {
        typedef Opm::MathToolbox<typename FluidState::Scalar> FsToolbox;

        assert (phaseIdx == liquidPhaseIdx);

        const auto& T = FsToolbox::template toLhs<LhsEval>(fluidState.temperature(phaseIdx));
        const auto& p = FsToolbox::template toLhs<LhsEval>(fluidState.pressure(phaseIdx));
        Valgrind::CheckDefined(T);
        Valgrind::CheckDefined(p);

        // TODO: way to deal with the solutes???
        return H2O::liquidEnthalpy(T, p);
    }

    //! \copydoc BaseFluidSystem::thermalConductivity
    template <class FluidState, class LhsEval = typename FluidState::Scalar>
    static LhsEval thermalConductivity(const FluidState &fluidState,
                                       const ParameterCache &/*paramCache*/,
                                       const unsigned phaseIdx)
    {
        typedef Opm::MathToolbox<typename FluidState::Scalar> FsToolbox;

        assert(phaseIdx == liquidPhaseIdx);

        if(useComplexRelations){
            const auto& T = FsToolbox::template toLhs<LhsEval>(fluidState.temperature(phaseIdx));
            const auto& p = FsToolbox::template toLhs<LhsEval>(fluidState.pressure(phaseIdx));
            return H2O::liquidThermalConductivity(T, p);
        }
        else
            return  0.578078;   // conductivity of water[W / (m K ) ] IAPWS evaluated at p=.1 MPa, T=8C
    }

    //! \copydoc BaseFluidSystem::heatCapacity
    template <class FluidState, class LhsEval = typename FluidState::Scalar>
    static LhsEval heatCapacity(const FluidState &fluidState,
                                const ParameterCache &/*paramCache*/,
                                unsigned phaseIdx)
    {
        typedef Opm::MathToolbox<typename FluidState::Scalar> FsToolbox;

        assert (phaseIdx == liquidPhaseIdx);

        const auto& T = FsToolbox::template toLhs<LhsEval>(fluidState.temperature(phaseIdx));
        const auto& p = FsToolbox::template toLhs<LhsEval>(fluidState.pressure(phaseIdx));

        return H2O::liquidHeatCapacity(T, p);
    }
};

} // namespace FluidSystems

} // namespace Opm

#endif
