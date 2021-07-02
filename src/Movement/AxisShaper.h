/*
 * InputShaper.h
 *
 *  Created on: 20 Feb 2021
 *      Author: David
 */

#ifndef SRC_MOVEMENT_AXISSHAPER_H_
#define SRC_MOVEMENT_AXISSHAPER_H_

#define SUPPORT_DAA		(1)

#include <RepRapFirmware.h>
#include <General/NamedEnum.h>
#include <ObjectModel/ObjectModel.h>
#include "InputShaperPlan.h"

// These names must be in alphabetical order and lowercase
NamedEnum(InputShaperType, uint8_t,
	custom,
#if SUPPORT_DAA
	daa,
#endif
	ei2,
	ei3,
	none,
	zvd,
	zvdd,
);

class DDA;
class BasicPrepParams;
class MoveSegment;

class AxisShaper INHERIT_OBJECT_MODEL
{
public:
	AxisShaper() noexcept;

	float GetFrequency() const noexcept { return frequency; }
	float GetDamping() const noexcept { return zeta; }
	InputShaperType GetType() const noexcept { return type; }
	InputShaperPlan PlanShaping(DDA& dda, BasicPrepParams& params, bool shapingEnabled) const noexcept;

	GCodeResult Configure(GCodeBuffer& gb, const StringRef& reply) THROWS(GCodeException);	// process M593

	static MoveSegment *GetUnshapedSegments(DDA& dda, const BasicPrepParams& params) noexcept;

protected:
	DECLARE_OBJECT_MODEL

private:
	MoveSegment *GetAccelerationSegments(const DDA& dda, const BasicPrepParams& params, InputShaperPlan& plan) const noexcept;
	MoveSegment *GetDecelerationSegments(const DDA& dda, const BasicPrepParams& params, InputShaperPlan& plan) const noexcept;
	MoveSegment *FinishSegments(const DDA& dda, const BasicPrepParams& params, MoveSegment *accelSegs, MoveSegment *decelSegs) const noexcept;
	float GetExtraAccelStartDistance(const DDA& dda) const noexcept;
	float GetExtraAccelEndDistance(const DDA& dda) const noexcept;
	float GetExtraDecelStartDistance(const DDA& dda) const noexcept;
	float GetExtraDecelEndDistance(const DDA& dda) const noexcept;

	static constexpr unsigned int MaxExtraImpulses = 4;
	static constexpr float DefaultFrequency = 40.0;
	static constexpr float DefaultDamping = 0.1;
	static constexpr float DefaultMinimumAcceleration = 10.0;

	unsigned int numExtraImpulses;						// the number of extra impulses
	float frequency;									// the undamped frequency
	float zeta;											// the damping ratio, see https://en.wikipedia.org/wiki/Damping. 0 = undamped, 1 = critically damped.
	float minimumAcceleration;							// the minimum value that we reduce average acceleration to
	float coefficients[MaxExtraImpulses];				// the coefficients of all the impulses
	float durations[MaxExtraImpulses];					// the duration in seconds of each impulse
	float totalDuration;								// the total input shaping time in seconds, which is the sum of the durations
	float totalShapingClocks;							// the total input shaping time in step clocks
	float clocksLostAtStart, clocksLostAtEnd;			// the acceleration time lost due to input shaping
	float overlappedCoefficients[2 * MaxExtraImpulses];	// the coefficients if we use a shaped start immediately followed by a shaped end
	float overlappedDuration;
	float overlappedShapingClocks;
	float overlappedClocksLost;
	float overlappedAverageAcceleration;
	InputShaperType type;
};

#endif /* SRC_MOVEMENT_AXISSHAPER_H_ */
