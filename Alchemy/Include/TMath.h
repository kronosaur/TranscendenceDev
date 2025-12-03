//	TMath.h
//
//	TMath classes
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once
#include <limits.h>

//	Modern std library definitions for certain values

//	(double) std:: infinity
//
#define R_INF std::numeric_limits<double>::infinity()

//	(double) std:: negative infinity
//
#define R_NINF -std::numeric_limits<double>::infinity()

//	(double) std:: generic NaN
//
#define R_NAN std::nan("")

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
