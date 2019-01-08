//	TMath.h
//
//	TMath classes
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

template <class VALUE> VALUE mathPercent (VALUE num, VALUE den)

//  mathPercent
//
//  Returns num/den in percentage terms (0-100).

    {
    return ((VALUE)100 * num / den);
    }

inline int mathPercent (int num, int den)
    {
    return mathRound(100.0 * num / den);
    }