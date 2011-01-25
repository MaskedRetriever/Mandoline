//
//  BGLLine.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//


#ifndef BGL_LINE_H
#define BGL_LINE_H

#include <math.h>
#include <iostream>
#include <list>
#include "config.h"
#include "BGLCommon.h"
#include "BGLAffine.h"
#include "BGLIntersection.h"
#include "BGLPoint.h"

using namespace std;

namespace BGL {


class Line {
public:
    // Member variables
    Point startPt;
    Point endPt;
    int16_t flags;
    float temperature;
    float extrusionWidth;

    // Constructors
    Line() :
        startPt(),
	endPt(),
	flags(0),
	temperature(0),
	extrusionWidth(0)
    {
    }

    Line(const Point& p1, const Point& p2) :
        startPt(p1),
	endPt(p2),
	flags(0),
	temperature(0),
	extrusionWidth(0)
    {
    }

    Line(const Line& ln) :
        startPt(ln.startPt),
        endPt(ln.endPt),
	flags(ln.flags),
        temperature(ln.temperature),
	extrusionWidth(ln.extrusionWidth)
    {
    }

    // Assignment operator
    Line& operator=(const Line &rhs) {
	if (this != &rhs) {
	    startPt = rhs.startPt;
	    endPt = rhs.endPt;
	    flags = rhs.flags;
	    temperature = rhs.temperature;
	    extrusionWidth = rhs.extrusionWidth;
	}
	return *this;
    }

    // Compound assignment operators
    Line& operator+=(const Point &rhs) {
        this->startPt += rhs;
        this->endPt += rhs;
	return *this;
    }
    Line& operator-=(const Point &rhs) {
        this->startPt -= rhs;
        this->endPt -= rhs;
	return *this;
    }
    Line& operator*=(float rhs) {
        this->startPt *= rhs;
        this->endPt *= rhs;
	return *this;
    }
    Line& operator*=(const Point &rhs) {
        this->startPt *= rhs;
        this->endPt *= rhs;
	return *this;
    }
    Line& operator/=(float rhs) {
        this->startPt /= rhs;
        this->endPt /= rhs;
	return *this;
    }
    Line& operator/=(const Point &rhs) {
        this->startPt /= rhs;
        this->endPt /= rhs;
	return *this;
    }

    // Binary arithmetic operators
    const Line operator+(const Point &rhs) const {
	return Line(*this) += rhs;
    }
    const Line operator-(const Point &rhs) const {
	return Line(*this) -= rhs;
    }
    const Line operator*(float rhs) const {
	return Line(*this) *= rhs;
    }
    const Line operator*(const Point &rhs) const {
	return Line(*this) *= rhs;
    }
    const Line operator/(float rhs) const {
	return Line(*this) /= rhs;
    }
    const Line operator/(const Point &rhs) const {
	return Line(*this) /= rhs;
    }

    // Comparison operators
    bool operator==(const Line &rhs) const {
        return ((startPt == rhs.startPt && endPt == rhs.endPt) ||
            (startPt == rhs.endPt && endPt == rhs.startPt));
    }
    bool operator!=(const Line &rhs) const {
        return !(*this == rhs);
    }

    bool hasEndPoint(const Point& pt) const {
        return (pt == startPt || pt == endPt);
    }

    // Transformations
    Line& scale(float scale) {
	*this *= scale;
	return *this;
    }
    Line& scale(const Point& vect) {
	*this *= vect;
	return *this;
    }
    Line& scaleAroundPoint(const Point& center, float scale) {
	*this -= center;
	*this *= scale;
	*this += center;
	return *this;
    }
    Line& scaleAroundPoint(const Point& center, const Point& vect) {
	*this -= center;
	*this *= vect;
	*this += center;
	return *this;
    }

    // Calculations
    float length() const {
        return startPt.distanceFrom(endPt);
    }
    float angle() const {
        return startPt.angleToPoint(endPt);
    }
    float angleDelta(const Line& ln) const {
        float delta = ln.angle() - angle();
	if (delta < -M_PI) {
	    delta += M_PI * 2.0f;
	} else if (delta > M_PI) {
	    delta -= M_PI * 2.0f;
	}
	return delta;
    }

    // Misc
    Line& reverse() {
        Point tmpPt = startPt;
	startPt = endPt;
	endPt = tmpPt;
	return *this;
    }
    bool isLinearWith(const Point& pt) const;
    bool hasInBounds(const Point &pt) const;
    bool contains(const Point &pt) const;
    Point closestSegmentPointTo(const Point &pt) const;
    Point closestExtendedLinePointTo(const Point &pt) const;
    float minimumSegmentDistanceFromPoint(const Point &pt) const;
    float minimumExtendedLineDistanceFromPoint(const Point &pt) const;
    Intersection intersectionWithSegment(const Line &ln) const;
    Intersection intersectionWithExtendedLine(const Line &ln) const;

    Line& leftOffset(float offsetby);

    // Friend functions
    friend ostream& operator <<(ostream &os,const Line &pt);
};

typedef list<Line> Lines;



}


#endif

