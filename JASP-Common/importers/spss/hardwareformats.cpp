#include "hardwareformats.h"
#include <stdexcept>

using namespace std;
using namespace spss;

HardwareFormats::HardwareFormats()
	: _endSource(mach_unknown)
	, _fpType(fp_unknown)
{
}


/**
 * @brief setFPType Sets the FP type.
 * @param type FP type to set.
 */
void HardwareFormats::setFPType(FPTypes type)
{
	const char * msg;

	switch(type)
	{
	case fp_IEEE754:
		_fpType = type;
		return;

	case fp_DECVAX_E:
		msg = "DEC VAX";
		break;
	case fp_IBM370:
		msg = "IBM mainframe";
		break;
	case fp_unknown:
		msg = "unknown origin";
		break;
	}
	throw runtime_error(string("Cannot read (floating point) numbers from ") + msg + " in file.");
}

/**
 * @brief Sets the Endainness.
 * @param type Type to set.
 */
void HardwareFormats::setEndian(Endians type)
{
	switch (type)
	{
	case mach_bigEndian:
	case mach_littleEndian:
		_endSource = type;
		return;

	default:
		throw runtime_error("Cannot figure out byte order in file.");
	}
}


/**
 * @brief analyse Deterimines the endin order of value.
 * @param value Value to analyse.
 * @param expectedValues Zero terminated array of expected values.
 */
void HardwareFormats::analyse(int32_t value, const int32_t *expectedValues)
{

	// Check for unmodified...
	if (_isInExpected(value, expectedValues))
		setEndian(_endThisMachine);
	else
	{
		// See if we can find it's reversed value.
		setEndian((_endThisMachine == mach_littleEndian) ? mach_bigEndian : mach_littleEndian);
		fixup(&value);
		if (_isInExpected(value, expectedValues) == false)
			setEndian(mach_unknown); // Throws an exception.
	}
}

/**
 * @brief analyse Deterimines the endin order of value.
 * @param value Value to analyse.
 * @param expectedValues Zero terminated array of expected values.
 */
void HardwareFormats::analyse(double value, const double *expectedValues)
{
	// NB This simple implmneation only searches for IEEE floating point values.

	// Check for endinnss?...
	if (_endSource != mach_unknown)
		// No, Simply verify the value is recognisable..
		setFPType( (_isInExpected(value, expectedValues)) ? _fpThisMachine : fp_unknown );
	else
	{
		// Try for other endinness.
		setEndian((_endThisMachine == mach_littleEndian) ? mach_bigEndian : mach_littleEndian);
		fixup(&value);
		analyse(value, expectedValues);
	}
}
