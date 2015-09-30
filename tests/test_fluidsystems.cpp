// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  Copyright (C) 2011-2013 by Andreas Lauser

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
 *
 * \brief This test makes sure that the programming interface is
 *        observed by all fluid systems
 */
#include "config.h"

#include <opm/material/localad/Evaluation.hpp>
#include <opm/material/localad/Math.hpp>

#include "checkFluidSystem.hpp"

// include all fluid systems in opm-material
#include <opm/material/fluidsystems/SinglePhaseFluidSystem.hpp>
#include <opm/material/fluidsystems/TwoPhaseImmiscibleFluidSystem.hpp>
#include <opm/material/fluidsystems/BlackOilFluidSystem.hpp>
#include <opm/material/fluidsystems/BrineCO2FluidSystem.hpp>
#include <opm/material/fluidsystems/H2ON2FluidSystem.hpp>
#include <opm/material/fluidsystems/H2ON2LiquidPhaseFluidSystem.hpp>
#include <opm/material/fluidsystems/H2OAirFluidSystem.hpp>
#include <opm/material/fluidsystems/H2OAirMesityleneFluidSystem.hpp>
#include <opm/material/fluidsystems/H2OAirXyleneFluidSystem.hpp>

// include all fluid states
#include <opm/material/fluidstates/PressureOverlayFluidState.hpp>
#include <opm/material/fluidstates/SaturationOverlayFluidState.hpp>
#include <opm/material/fluidstates/TemperatureOverlayFluidState.hpp>
#include <opm/material/fluidstates/CompositionalFluidState.hpp>
#include <opm/material/fluidstates/NonEquilibriumFluidState.hpp>
#include <opm/material/fluidstates/ImmiscibleFluidState.hpp>
#include <opm/material/fluidstates/SimpleModularFluidState.hpp>

// include the tables for CO2 which are delivered with opm-material by default
#include <opm/material/common/UniformTabulated2DFunction.hpp>

namespace Opm {
namespace FluidSystemsTest {
#include <opm/material/components/co2tables.inc>
} }

// include dune's MPI helper header
#include <dune/common/version.hh>
#if DUNE_VERSION_NEWER(DUNE_COMMON, 2,3)
#include <dune/common/parallel/mpihelper.hh>
#else
#include <dune/common/mpihelper.hh>
#endif

// check the API of all fluid states
template <class Scalar>
void testAllFluidStates()
{
    typedef Opm::FluidSystems::H2ON2<Scalar, /*enableComplexRelations=*/false> FluidSystem;

    // SimpleModularFluidState
    {   Opm::SimpleModularFluidState<Scalar,
                                     /*numPhases=*/2,
                                     /*numComponents=*/0,
                                     /*FluidSystem=*/void,
                                     /*storePressure=*/false,
                                     /*storeTemperature=*/false,
                                     /*storeComposition=*/false,
                                     /*storeFugacity=*/false,
                                     /*storeSaturation=*/false,
                                     /*storeDensity=*/false,
                                     /*storeViscosity=*/false,
                                     /*storeEnthalpy=*/false> fs;

        checkFluidState<Scalar>(fs); }

    {   Opm::SimpleModularFluidState<Scalar,
                                     /*numPhases=*/2,
                                     /*numComponents=*/2,
                                     FluidSystem,
                                     /*storePressure=*/true,
                                     /*storeTemperature=*/true,
                                     /*storeComposition=*/true,
                                     /*storeFugacity=*/true,
                                     /*storeSaturation=*/true,
                                     /*storeDensity=*/true,
                                     /*storeViscosity=*/true,
                                     /*storeEnthalpy=*/true> fs;

        checkFluidState<Scalar>(fs); }

    // CompositionalFluidState
    {   Opm::CompositionalFluidState<Scalar, FluidSystem> fs;
        checkFluidState<Scalar>(fs); }

    // NonEquilibriumFluidState
    {   Opm::NonEquilibriumFluidState<Scalar, FluidSystem> fs;
        checkFluidState<Scalar>(fs); }

    // ImmiscibleFluidState
    {   Opm::ImmiscibleFluidState<Scalar, FluidSystem> fs;
        checkFluidState<Scalar>(fs); }

    typedef Opm::CompositionalFluidState<Scalar, FluidSystem> BaseFluidState;
    BaseFluidState baseFs;

    // TemperatureOverlayFluidState
    {   Opm::TemperatureOverlayFluidState<BaseFluidState> fs(baseFs);
        checkFluidState<Scalar>(fs); }

    // PressureOverlayFluidState
    {   Opm::PressureOverlayFluidState<BaseFluidState> fs(baseFs);
        checkFluidState<Scalar>(fs); }

    // SaturationOverlayFluidState
    {   Opm::SaturationOverlayFluidState<BaseFluidState> fs(baseFs);
        checkFluidState<Scalar>(fs); }
}

template <class Scalar, class Evaluation, class LhsEval = Evaluation>
void testAllFluidSystems()
{
    typedef Opm::LiquidPhase<Scalar, Opm::H2O<Scalar>> Liquid;
    typedef Opm::GasPhase<Scalar, Opm::N2<Scalar>> Gas;

    // black-oil
    {   typedef Opm::FluidSystems::BlackOil<Scalar> FluidSystem;
        if (false) checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // Brine -- CO2
    {   typedef Opm::FluidSystems::BrineCO2<Scalar, Opm::FluidSystemsTest::CO2Tables> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // H2O -- N2
    {   typedef Opm::FluidSystems::H2ON2<Scalar, /*enableComplexRelations=*/false> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {   typedef Opm::FluidSystems::H2ON2<Scalar, /*enableComplexRelations=*/true> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // H2O -- N2 -- liquid phase
    {   typedef Opm::FluidSystems::H2ON2LiquidPhase<Scalar, /*enableComplexRelations=*/false> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {   typedef Opm::FluidSystems::H2ON2LiquidPhase<Scalar, /*enableComplexRelations=*/true> FluidSystem;
         checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // H2O -- Air
    {   typedef Opm::SimpleH2O<Scalar> H2O;
        const bool enableComplexRelations=false;
        typedef Opm::FluidSystems::H2OAir<Scalar, H2O, enableComplexRelations> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {   typedef Opm::SimpleH2O<Scalar> H2O;
        const bool enableComplexRelations=true;
        typedef Opm::FluidSystems::H2OAir<Scalar, H2O, enableComplexRelations> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {   typedef Opm::H2O<Scalar> H2O;
        const bool enableComplexRelations=false;
        typedef Opm::FluidSystems::H2OAir<Scalar, H2O, enableComplexRelations> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {   typedef Opm::H2O<Scalar> H2O;
        const bool enableComplexRelations=true;
        typedef Opm::FluidSystems::H2OAir<Scalar, H2O, enableComplexRelations> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // H2O -- Air -- Mesitylene
    {   typedef Opm::FluidSystems::H2OAirMesitylene<Scalar> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // H2O -- Air -- Xylene
    {   typedef Opm::FluidSystems::H2OAirXylene<Scalar> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // 2p-immiscible
    {   typedef Opm::FluidSystems::TwoPhaseImmiscible<Scalar, Liquid, Liquid> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {   typedef Opm::FluidSystems::TwoPhaseImmiscible<Scalar, Liquid, Gas> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {  typedef Opm::FluidSystems::TwoPhaseImmiscible<Scalar, Gas, Liquid> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    // 1p
    {   typedef Opm::FluidSystems::SinglePhase<Scalar, Liquid> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }

    {   typedef Opm::FluidSystems::SinglePhase<Scalar, Gas> FluidSystem;
        checkFluidSystem<Scalar, FluidSystem, Evaluation, LhsEval>(); }
}

class TestAdTag;

int main(int argc, char **argv)
{
    typedef double Scalar;
    typedef Opm::LocalAd::Evaluation<Scalar, TestAdTag, 3> Evaluation;

    Dune::MPIHelper::instance(argc, argv);

    // ensure that all fluid states are API-compliant
    testAllFluidStates<Scalar>();
    testAllFluidStates<Evaluation>();

    // ensure that all fluid systems are API-compliant: Each fluid system must be usable
    // for both, scalars and function evaluations. The fluid systems for function
    // evaluations must also be usable for scalars.
    testAllFluidSystems<Scalar, Scalar>();
    testAllFluidSystems<Scalar, Evaluation>();
    testAllFluidSystems<Scalar, Evaluation, Scalar>();

    return 0;
}
