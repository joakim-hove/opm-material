// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  Copyright (C) 2015 by Andreas Lauser

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
 * \copydoc Opm::OilPvtMultiplexer
 */
#ifndef OPM_OIL_PVT_MULTIPLEXER_HPP
#define OPM_OIL_PVT_MULTIPLEXER_HPP

#include "ConstantCompressibilityOilPvt.hpp"
#include "DeadOilPvt.hpp"
#include "LiveOilPvt.hpp"

namespace Opm {
#define OPM_OIL_PVT_MULTIPLEXER_CALL(codeToCall)                        \
    switch (approach_) {                                                \
    case ConstantCompressibilityOilPvt: {                               \
        auto &pvtImpl = getRealPvt<ConstantCompressibilityOilPvt>();    \
        codeToCall;                                                     \
        break;                                                          \
    }                                                                   \
    case DeadOilPvt: {                                                  \
        auto &pvtImpl = getRealPvt<DeadOilPvt>();                       \
        codeToCall;                                                     \
        break;                                                          \
    }                                                                   \
    case LiveOilPvt: {                                                  \
        auto &pvtImpl = getRealPvt<LiveOilPvt>();                       \
        codeToCall;                                                     \
        break;                                                          \
    }                                                                   \
    case NoOilPvt:                                                      \
        OPM_THROW(std::logic_error, "Not implemented: Oil PVT of this deck!"); \
    }

/*!
 * \brief This class represents the Pressure-Volume-Temperature relations of the oil
 *        phase in the black-oil model.
 *
 * This is the base class which which provides an API for the actual PVT implementation
 * classes which based on dynamic polymorphism. The rationale to use dynamic polymorphism
 * here is that this enables the fluid system to easily switch the used PVT relations for
 * the individual fluid phases.
 *
 * Note that, since the application for this class is the black-oil fluid system, the API
 * exposed by this class is pretty specific to the black-oil model.
 */
template <class Scalar>
class OilPvtMultiplexer
{
public:
    enum OilPvtApproach {
        NoOilPvt,
        LiveOilPvt,
        DeadOilPvt,
        ConstantCompressibilityOilPvt
    };

    OilPvtMultiplexer()
    {
        approach_ = NoOilPvt;
    }

    ~OilPvtMultiplexer()
    {
        switch (approach_) {
        case ConstantCompressibilityOilPvt: {
            delete &getRealPvt<ConstantCompressibilityOilPvt>();
            break;
        }
        case DeadOilPvt: {
            delete &getRealPvt<DeadOilPvt>();
            break;
        }
        case LiveOilPvt: {
            delete &getRealPvt<LiveOilPvt>();
            break;
        }

        case NoOilPvt:
            break;
        }
    }

#if HAVE_OPM_PARSER
    /*!
     * \brief Initialize the parameters for water using an ECL deck.
     *
     * This method assumes that the deck features valid DENSITY and PVTO/PVDO/PVCDO keywords.
     */
    void initFromDeck(DeckConstPtr deck, EclipseStateConstPtr eclState)
    {
        if (deck->hasKeyword("PVCDO"))
            setApproach(ConstantCompressibilityOilPvt);
        else if (deck->hasKeyword("PVDO"))
            setApproach(DeadOilPvt);
        else if (deck->hasKeyword("PVTO"))
            setApproach(LiveOilPvt);

        OPM_OIL_PVT_MULTIPLEXER_CALL(pvtImpl.initFromDeck(deck, eclState));
    }
#endif // HAVE_OPM_PARSER


    void initEnd()
    { OPM_OIL_PVT_MULTIPLEXER_CALL(pvtImpl.initEnd()); }

    /*!
     * \brief Returns the dynamic viscosity [Pa s] of the fluid phase given a set of parameters.
     */
    template <class Evaluation>
    Evaluation viscosity(unsigned regionIdx,
                         const Evaluation& temperature,
                         const Evaluation& pressure,
                         const Evaluation& Rs) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.viscosity(regionIdx, temperature, pressure, Rs)); return 0; }

    /*!
     * \brief Returns the dynamic viscosity [Pa s] of the fluid phase given a set of parameters.
     */
    template <class Evaluation>
    Evaluation saturatedViscosity(unsigned regionIdx,
                                  const Evaluation& temperature,
                                  const Evaluation& pressure) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.saturatedViscosity(regionIdx, temperature, pressure)); return 0; }

    /*!
     * \brief Returns the formation volume factor [-] of the fluid phase.
     */
    template <class Evaluation>
    Evaluation formationVolumeFactor(unsigned regionIdx,
                                     const Evaluation& temperature,
                                     const Evaluation& pressure,
                                     const Evaluation& Rs) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.formationVolumeFactor(regionIdx, temperature, pressure, Rs)); return 0; }

    /*!
     * \brief Returns the formation volume factor [-] of the fluid phase.
     */
    template <class Evaluation>
    Evaluation saturatedFormationVolumeFactor(unsigned regionIdx,
                                              const Evaluation& temperature,
                                              const Evaluation& pressure) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.saturatedFormationVolumeFactor(regionIdx, temperature, pressure)); return 0; }

    /*!
     * \brief Returns the density [kg/m^3] of the fluid phase given a set of parameters.
     */
    template <class Evaluation>
    Evaluation density(unsigned regionIdx,
                       const Evaluation& temperature,
                       const Evaluation& pressure,
                       const Evaluation& Rs) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.density(regionIdx, temperature, pressure, Rs)); return 0; }

    /*!
     * \brief Returns the dynamic viscosity [Pa s] of the fluid phase given a set of parameters.
     */
    template <class Evaluation>
    Evaluation saturatedDensity(unsigned regionIdx,
                                const Evaluation& temperature,
                                const Evaluation& pressure) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.saturatedDensity(regionIdx, temperature, pressure)); return 0; }

    /*!
     * \brief Returns the gas dissolution factor \f$R_s\f$ [m^3/m^3] of saturated oil.
     */
    template <class Evaluation>
    Evaluation saturatedGasDissolutionFactor(unsigned regionIdx,
                                             const Evaluation& temperature,
                                             const Evaluation& pressure) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.saturatedGasDissolutionFactor(regionIdx, temperature, pressure)); return 0; }

    /*!
     * \brief Returns the saturation pressure [Pa] of oil given the mass fraction of the
     *        gas component in the oil phase.
     *
     * Calling this method only makes sense for live oil. All other implementations of
     * the black-oil PVT interface will just throw an exception...
     */
    template <class Evaluation>
    Evaluation saturationPressure(unsigned regionIdx,
                                     const Evaluation& temperature,
                                     const Evaluation& Rs) const
    { OPM_OIL_PVT_MULTIPLEXER_CALL(return pvtImpl.saturationPressure(regionIdx, temperature, Rs)); return 0; }

    void setApproach(OilPvtApproach appr)
    {
        switch (appr) {
        case ConstantCompressibilityOilPvt:
            realOilPvt_ = new Opm::ConstantCompressibilityOilPvt<Scalar>;
            break;

        case DeadOilPvt:
            realOilPvt_ = new Opm::DeadOilPvt<Scalar>;
            break;

        case LiveOilPvt:
            realOilPvt_ = new Opm::LiveOilPvt<Scalar>;
            break;

        case NoOilPvt:
            OPM_THROW(std::logic_error, "Not implemented: Oil PVT of this deck!");
        }

        approach_ = appr;
    }

    /*!
     * \brief Returns the concrete approach for calculating the PVT relations.
     *
     * (This is only determined at runtime.)
     */
    OilPvtApproach approach() const
    { return approach_; }

    // get the concrete parameter object for the oil phase
    template <OilPvtApproach approachV>
    typename std::enable_if<approachV == LiveOilPvt, Opm::LiveOilPvt<Scalar> >::type& getRealPvt()
    {
        assert(approach() == approachV);
        return *static_cast<Opm::LiveOilPvt<Scalar>* >(realOilPvt_);
    }

    template <OilPvtApproach approachV>
    typename std::enable_if<approachV == LiveOilPvt, const Opm::LiveOilPvt<Scalar> >::type& getRealPvt() const
    {
        assert(approach() == approachV);
        return *static_cast<Opm::LiveOilPvt<Scalar>* >(realOilPvt_);
    }

    template <OilPvtApproach approachV>
    typename std::enable_if<approachV == DeadOilPvt, Opm::DeadOilPvt<Scalar> >::type& getRealPvt()
    {
        assert(approach() == approachV);
        return *static_cast<Opm::DeadOilPvt<Scalar>* >(realOilPvt_);
    }

    template <OilPvtApproach approachV>
    typename std::enable_if<approachV == DeadOilPvt, const Opm::DeadOilPvt<Scalar> >::type& getRealPvt() const
    {
        assert(approach() == approachV);
        return *static_cast<Opm::DeadOilPvt<Scalar>* >(realOilPvt_);
    }

    template <OilPvtApproach approachV>
    typename std::enable_if<approachV == ConstantCompressibilityOilPvt, Opm::ConstantCompressibilityOilPvt<Scalar> >::type& getRealPvt()
    {
        assert(approach() == approachV);
        return *static_cast<Opm::ConstantCompressibilityOilPvt<Scalar>* >(realOilPvt_);
    }

    template <OilPvtApproach approachV>
    typename std::enable_if<approachV == ConstantCompressibilityOilPvt, const Opm::ConstantCompressibilityOilPvt<Scalar> >::type& getRealPvt() const
    {
        assert(approach() == approachV);
        return *static_cast<Opm::ConstantCompressibilityOilPvt<Scalar>* >(realOilPvt_);
    }

private:
    OilPvtApproach approach_;
    void *realOilPvt_;
};

} // namespace Opm

#endif