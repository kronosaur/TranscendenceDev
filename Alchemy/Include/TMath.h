//	TMath.h
//
//	TMath classes
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once
#include <limits.h>

//	Modern std library definitions for certain values

//	(double) std:: infinity
//	Use IS_AN_INF(rValue) or IS_P_INF(rValue) to check for positive infinity values
//
#define R_INF std::numeric_limits<double>::infinity()

//	(double) std:: negative infinity
//  Use IS_AN_INF(rValue) or IS_N_INF(rValue) to check for negative infinity values
//
#define R_NINF -std::numeric_limits<double>::infinity()

//	(double) std:: generic NaN
//	Use IS_NAN(rValue) to check for NaN values
//
#define R_NAN std::nan("")

//	check if a value is a NaN
//	(NaN is never equal to anything, including NaN)
//
#define IS_NAN(a) std::isnan(a)

//	check if a value is an infinity (either pos or negative)
//	direct infinity comparisons are indeterminate and return false
//
#define IS_AN_INF(a) std::isinf(a)

//	check if a value is a positive infinity
//
#define IS_P_INF(a) (std::isinf(a) && a > 0)

//	check if a value is a negative infinity
//
#define IS_N_INF(a) (std::isinf(a) && a < 0)

//	Percentage conversion functions

//  mathPercent
//
//  Returns num/den in percentage terms (0-100).
//
template <class VALUE> VALUE mathPercent (VALUE num, VALUE den)

    {
    return ((VALUE)100 * num / den);
    }

//  mathPercent
//
//  Returns num/den in percentage terms (0-100).
//
inline int mathPercent (int num, int den)
    {
    return mathRound(100.0 * num / den);
    }
