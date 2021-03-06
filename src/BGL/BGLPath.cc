//
//  BGLPath.cc
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#include <iostream>
#include <iomanip>

#include "BGLPath.h"

using namespace std;
using namespace BGL;

namespace BGL {


Path::Path(int cnt, const Point* pts)
{
    Point prev = pts[0];
    flags = 0;
    for (int i = 1; i < cnt; i++) {
        Line ln(prev,pts[i]);
        attach(ln);
        prev = pts[i];
    }
}



// Comparison operators
bool Path::operator==(const Path &rhs) const
{
    if (size() != rhs.size()) {
        return false;
    }
    Lines::const_iterator itera = segments.begin();
    Lines::const_iterator iterb = rhs.segments.begin();
    while(itera != segments.end() && iterb != rhs.segments.end()) {
        if (*itera != *iterb) {
            return false;
        }
        itera++, iterb++;
    }
    return true;
}



// Compound assignment operators
Path& Path::operator+=(const Point &rhs) {
    Lines::iterator it;
    for (it = segments.begin(); it != segments.end(); it++) {
        *it += rhs;
    }
    return *this;
}



Path& Path::operator-=(const Point &rhs) {
    Lines::iterator it;
    for (it = segments.begin(); it != segments.end(); it++) {
        *it -= rhs;
    }
    return *this;
}



Path& Path::operator*=(double rhs) {
    Lines::iterator it;
    for (it = segments.begin(); it != segments.end(); it++) {
        *it *= rhs;
    }
    return *this;
}



Path& Path::operator*=(const Point &rhs) {
    Lines::iterator it;
    for (it = segments.begin(); it != segments.end(); it++) {
        *it *= rhs;
    }
    return *this;
}



Path& Path::operator/=(double rhs) {
    Lines::iterator it;
    for (it = segments.begin(); it != segments.end(); it++) {
        *it /= rhs;
    }
    return *this;
}



Path& Path::operator/=(const Point &rhs) {
    Lines::iterator it;
    for (it = segments.begin(); it != segments.end(); it++) {
        *it /= rhs;
    }
    return *this;
}



double Path::length() const
{
    double totlen = 0.0f;
    Lines::const_iterator itera = segments.begin();
    for(; itera != segments.end(); itera++) {
        totlen += itera->length();
    }
    return totlen;
}



double Path::windingArea() const
{
    double totarea = 0.0f;
    Lines::const_iterator itera = segments.begin();
    for(; itera != segments.end(); itera++) {
        totarea += itera->startPt.x * itera->endPt.y;
        totarea -= itera->endPt.x * itera->startPt.y;
    }
    return (totarea/2.0f);
}



bool Path::isClockwise() const
{
    return (windingArea() > 0);
}



double Path::area() const
{
    return fabs(windingArea());
}



void Path::setTemperature(double val)
{
    Lines::iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        itera->temperature = val;
    }
}



void Path::setWidth(double val)
{
    Lines::iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        itera->extrusionWidth = val;
    }
}



Bounds Path::bounds() const
{
    Bounds bnds;
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        bnds.expand(itera->startPt);
        bnds.expand(itera->endPt);
    }
    return bnds;
}



bool Path::couldAttach(const Line& ln) const
{
    if (size() < 1) return true;
    if (hasEndPoint(ln.startPt)) return true;
    if (hasEndPoint(ln.endPt)) return true;
    return false;
}



bool Path::couldAttach(const Path& path) const
{
    if (size() < 1) return true;
    if (hasEndPoint(path.startPoint())) return true;
    if (hasEndPoint(path.endPoint())) return true;
    return false;
}



bool Path::attach(const Line& ln)
{
    if (size() <= 0) {
        segments.push_back(ln);
        return true;
    }
    if (endPoint() == ln.startPt) {
        segments.push_back(ln);
        return true;
    }
    if (startPoint() == ln.endPt) {
        segments.push_front(ln);
        return true;
    }
    if (endPoint() == ln.endPt) {
        Line lnrev(ln);
        lnrev.reverse();
        segments.push_back(lnrev);
        return true;
    }
    if (startPoint() == ln.startPt) {
        Line lnrev(ln);
        lnrev.reverse();
        segments.push_front(lnrev);
        return true;
    }
    return false;
}



bool Path::attach(const Path& path)
{
    if (couldAttach(path)) {
        Lines::const_iterator itera = path.segments.begin();
        for(; itera != path.segments.end(); itera++) {
            attach(*itera);
        }
        return true;
    }
    return false;
}



string Path::svgPathWithOffset(double dx, double dy) const
{
    char buf[80];
    string out;
    if (size() == 0) {
        return out;
    }
    Line prev;
    Point start;
    double mult = 90.0f / 25.4f;
    Lines::const_iterator itera = segments.begin();
    bool isfirst = true;
    for (; itera != segments.end(); itera++) {
        if (!isfirst) {
            out.append(" ");
        }
        if (isfirst || prev.endPt != itera->startPt) {
            start = itera->startPt;
            snprintf(buf,sizeof(buf),"M%5.3f,%5.3f ",
                (itera->startPt.x+dx)*mult,
                (itera->startPt.y+dy)*mult);
            out.append(buf);
            isfirst = false;
        }
        if (itera->endPt == start) {
            snprintf(buf,sizeof(buf),"Z");
            isfirst = true;
        } else {
            snprintf(buf,sizeof(buf),"L%5.3f,%5.3f",
                (itera->endPt.x+dx)*mult,
                (itera->endPt.y+dy)*mult);
        }
        out.append(buf);
        prev = *itera;
    }
    return out;
}




ostream &Path::svgPathDataWithOffset(ostream& os, double dx, double dy) const
{
    if (size() == 0) {
        return os;
    }
    os.setf(ios::fixed);
    os.precision(3);
    double mult = 90.0f / 25.4f;
    Line prev;
    Point start;
    Lines::const_iterator itera = segments.begin();
    bool isfirst = true;
    for (; itera != segments.end(); itera++) {
        if (!isfirst) {
            os << endl << "    ";
        }
        if (isfirst || prev.endPt != itera->startPt) {
            start = itera->startPt;
            os << "M" << setw(8) << ((itera->startPt.x+dx)*mult);
            os << "," << setw(8) << ((itera->startPt.y+dy)*mult);
            os << endl << "    ";
            isfirst = false;
        }
        if (itera->endPt == start) {
            os << "Z";
            isfirst = true;
        } else {
            os << "L" << setw(8) << ((itera->endPt.x+dx)*mult);
            os << "," << setw(8) << ((itera->endPt.y+dy)*mult);
        }
        prev = *itera;
    }
    return os;
}



ostream &Path::svgPathWithOffset(ostream& os, double dx, double dy) const
{
    os << "<path fill=\"none\" d=\"";
    svgPathDataWithOffset(os, dx, dy);
    os << "\" />" << endl;
    return os;
}



bool Path::intersects(const Line &ln) const
{
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        Intersection isect = itera->intersectionWithSegment(ln);
        if (isect.type != NONE) {
            return true;
        }
    }
    return false;
}



bool Path::intersects(const Path &path) const
{
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        Lines::const_iterator iterb = path.segments.begin();
        for (; iterb != path.segments.end(); iterb++) {
            Intersection isect = itera->intersectionWithSegment(*iterb);
            if (isect.type != NONE) {
                return true;
            }
        }
    }
    return false;
}



Intersections &Path::intersectionsWith(const Line &ln, Intersections &outISects) const
{
    int segnum = 0;
    bool isclosed = isClosed();
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        Intersection isect = itera->intersectionWithSegment(ln);
        // Ignore point intersections with the startpoint of a segment.
        // It should have already been caught as the endpoint of the
        //  previous segment.
        if (isect.type != NONE) {
            if (isect.type != POINT ||
                isect.p1 != itera->startPt ||
                (segnum == 0 && !isclosed)
                ) {
                isect.segment = segnum;
                outISects.push_back(isect);
            }
        }
        segnum++;
    }
    return outISects;
}



bool Path::hasEdgeWithPoint(const Point &pt, Lines::const_iterator &outSeg) const
{
    Lines::const_iterator itera;
    for (itera = segments.begin(); itera != segments.end(); itera++) {
        if (itera->contains(pt)) {
            outSeg = itera;
            return true;
        }
    }
    return false;
}



bool Path::contains(const Point &pt) const
{
    if (!isClosed()) {
        return false;
    }
    Line longLine(pt,Point(1.0e9,pt.y));
    Line testLine;
    Point& sp = testLine.startPt;
    Point& ep = testLine.endPt;
    int icount = 0;
    Lines::const_iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        sp = itera->startPt;
        if (fabs(sp.y-pt.y) < CLOSEENOUGH) {
            sp.y += 1.5 * CLOSEENOUGH;
        }
        ep = itera->endPt;
        if (fabs(ep.y-pt.y) < CLOSEENOUGH) {
            ep.y += 1.5 * CLOSEENOUGH;
        }
        Intersection isect = testLine.intersectionWithSegment(longLine);
        if (isect.type != NONE) {
            icount++;
        }
    }
    return ((icount & 0x1) != 0);
}


// Strips out segments that are shorter than the given length.
void Path::stripSegmentsShorterThan(double minlen)
{
    Lines::iterator itera = segments.begin();
    while (itera != segments.end()) {
        if (itera->length() < minlen) {
            Line ln = *itera;
            itera = segments.erase(itera);
            if (itera == segments.end()) {
                if (segments.size() > 0) {
                    segments.back().endPt = ln.endPt;
                }
            } else {
                itera->startPt = ln.startPt;
            }
        } else {
            itera++;
        }
    }
}



// Strips out segments that are shorter than the given length.
void Path::simplify(double minErr)
{
    if (segments.size() < 2) {
        return;
    }
    Lines::iterator itera = segments.begin();
    Lines::iterator iterb = itera;
    for (iterb++; iterb != segments.end(); itera++, iterb++) {
        Line ln(itera->startPt, iterb->endPt);
        while (ln.minimumExtendedLineDistanceFromPoint(itera->endPt) <= minErr) {
            itera->endPt = iterb->endPt;
            iterb = segments.erase(iterb);
            if (iterb == segments.end()) {
                return;
            }
            ln.endPt = iterb->endPt;
        }
    }
}



Paths &Path::assemblePathsFromSegments(const Lines &segs, Paths &outPaths)
{
    Lines unhandled(segs);
    Path currPath;
    bool foundLink = false;
    while (unhandled.size() > 0) {
        if (currPath.size() == 0) {
            Line ln = unhandled.front();
            currPath.attach(ln);
            unhandled.pop_front();
        }
        foundLink = false;
        Lines::iterator itera = unhandled.begin();
        while (itera != unhandled.end()) {
            if (currPath.attach(*itera)) {
                itera = unhandled.erase(itera);
                foundLink = true;
            } else {
                itera++;
            }
        }
        if (!foundLink || unhandled.size() == 0) {
            outPaths.push_back(currPath);
            currPath = Path();
        }
    }
    return outPaths;
}



Paths &Path::repairUnclosedPaths(const Paths &paths, Paths &outPaths)
{
    Paths unhandled(paths);
    
    // filter out all completed paths.
    Paths::iterator itera = unhandled.begin();
    while (itera != unhandled.end()) {
        if (itera->isClosed()) {
            outPaths.push_back(*itera);
            itera = unhandled.erase(itera);
        } else {
            itera++;
        }
    }
    
    // Now we just have incomplete paths left.
    while (unhandled.size() > 0) {
        Path path = unhandled.front();
        unhandled.pop_front();
        for (;;) {
            // Find closest remaining incomplete path
            Paths::iterator closestIter;
            double closestDist = 9.0e9;
            double closingDist = path.startPoint().distanceFrom(path.endPoint());
            for (itera = unhandled.begin(); itera != unhandled.end(); itera++) {
                Path &path2 = *itera;
                double dist1 = path.endPoint().distanceFrom(path2.startPoint());
                double dist2 = path.endPoint().distanceFrom(path2.endPoint());
                if (dist1 < closestDist) {
                    closestDist = dist1;
                    closestIter = itera;
                }
                if (dist2 < closestDist) {
                    closestDist = dist2;
                    closestIter = itera;
                }
            }
            // If closest found incomplete path is closer than just closing the path, then attach it.
            if (closestDist < closingDist) {
                Path &path2 = *closestIter;
                path.attach(Line(path.endPoint(),path2.startPoint()));
                path.attach(path2);
                closestIter = unhandled.erase(closestIter);
            } else {
                // Closest found match is if we just close the path.
                if (path.size() < 2) {
                    break;
                }
                Line ln(path.endPoint(), path.startPoint());
                path.attach(ln);
            }
            if (path.isClosed()) {
                // Path has been closed.  On to the next path!
                outPaths.push_back(path);
                break;
            }
        }
    }
    return outPaths;
}



void Path::splitSegmentsAtIntersectionsWithPath(const Path &path)
{
    Lines::iterator itera;
    for (itera = segments.begin(); itera != segments.end(); itera++) {
        Lines::const_iterator iterb;
        for (iterb = path.segments.begin(); iterb != path.segments.end(); iterb++) {
            Intersection isect = itera->intersectionWithSegment(*iterb);
            if (isect.type != NONE) {
		Points isects;
                if (!itera->hasEndPoint(isect.p1)) {
		    isects.push_back(isect.p1);
		}
		if (isect.type == SEGMENT) {
		    if (!itera->hasEndPoint(isect.p2)) {
			if (isect.p1 != isect.p2) {
			    double dist1 = itera->startPt.distanceFrom(isect.p1);
			    double dist2 = itera->startPt.distanceFrom(isect.p2);
			    if (dist2 > dist1) {
			        isects.push_front(isect.p2);
			    } else {
			        isects.push_back(isect.p2);
			    }
			}
		    }
		}
                Points::iterator iterc;
		for (iterc = isects.begin(); iterc != isects.end(); iterc++) {
                    Point tempPt = itera->startPt;
                    itera->startPt = *iterc;
                    itera = segments.insert(itera, Line(tempPt, *iterc));
                }
            }
        }
    }
    //stripSegmentsShorterThan(CLOSEENOUGH);
}



Paths &Path::separateSelfIntersectingSubpaths(Paths &outPaths)
{
    splitSegmentsAtIntersectionsWithPath(*this);
    Path subpath1;
    Path subpath2;
    Lines::iterator it1;
    Lines::iterator it2;
    Lines::iterator it3;
    bool found = false;
    for (it1 = segments.begin(); !found && it1 != segments.end(); it1++) {
        subpath1.segments.push_back(*it1);
        for (it2 = it1; it2 != segments.end(); it2++) {
            if (it1 != it2 && it1->endPt == it2->endPt) {
                it3 = it1;
                for (it3++; it3 != segments.end(); it3++) {
                    subpath2.segments.push_back(*it3);
                    if (it3 == it2) {
                        break;
                    }
                }
                it3 = it2;
                for (it3++; it3 != segments.end(); it3++) {
                    subpath1.segments.push_back(*it3);
                }
                found = true;
                break;
            }
        }
    }
    if (!found) {
        outPaths.push_back(*this);
        return outPaths;
    }

    subpath1.separateSelfIntersectingSubpaths(outPaths);
    subpath2.separateSelfIntersectingSubpaths(outPaths);
    return outPaths;
}



void Path::reorderByPoint(const Point &pt)
{
    int limit = segments.size();
    while (limit-->0) {
        Line &ln = segments.front();
        if (ln.startPt == pt) {
            return;
        }
        if (ln.endPt != pt && ln.contains(pt)) {
            ln.startPt = pt;
            segments.push_back(Line(endPoint(),pt));
            return;
        }
        segments.push_back(segments.front());
        segments.pop_front();
    }
}



void Path::untag()
{
    flags = OUTSIDE;
    Lines::iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        itera->flags = USED;
    }
}



void Path::tagSegmentsRelativeToClosedPath(const Path &path)
{
    bool invert = (flags == INSIDE);
    simplify(2*EPSILON);
    splitSegmentsAtIntersectionsWithPath(path);
    Point midpt;
    Lines::iterator itera = segments.begin();
    for (; itera != segments.end(); itera++) {
        Line &seg = *itera;
        midpt.x = (seg.startPt.x + seg.endPt.x) / 2.0f;
        midpt.y = (seg.startPt.y + seg.endPt.y) / 2.0f;
        Lines::const_iterator foundSeg = path.segments.end();
        if (path.hasEdgeWithPoint(midpt, foundSeg)) {
            // Either shared or unshared segment.

            // Check if the matching segments point the same way.
            double dang = seg.angleDelta(*foundSeg);
            bool isShared = (fabs(dang) < M_PI_2);

            // If the paths wind in opposite directions, invert shared test.
            if (isClockwise() != path.isClockwise()) {
                isShared = !isShared;
            }

            // tweak sharedness, for use in checking against multiple paths.
            if (invert) {
                isShared = !isShared;
            }
            switch (seg.flags) {
                case USED:
                case OUTSIDE:
                case UNSHARED:
                    seg.flags = isShared? SHARED : UNSHARED;
                    break;
                case SHARED:
                    seg.flags = SHARED;
                    break;
                case INSIDE:
                    seg.flags = isShared? UNSHARED : SHARED;
                    break;
            }
        } else {
            bool isinside = path.contains(midpt);
            if (invert) {
                isinside = !isinside;
            }
            if (isinside) {
                // toggle insideness, for use in checking against multiple paths.
                switch (seg.flags) {
                case USED:
                    seg.flags = INSIDE;
                    break;
                case INSIDE:
                    seg.flags = OUTSIDE;
                    break;
                case OUTSIDE:
                    seg.flags = INSIDE;
                    break;
                case SHARED:
                    seg.flags = UNSHARED;
                    break;
                case UNSHARED:
                    seg.flags = SHARED;
                    break;
                }
            } else {
                if (seg.flags == USED) {
                    seg.flags = OUTSIDE;
                }
            }
        }
        cerr << "Tagged: " << seg << "  ";
        if (seg.flags == INSIDE)   { cerr << "I"; }
        if (seg.flags == OUTSIDE)  { cerr << "O"; }
        if (seg.flags == SHARED)   { cerr << "S"; }
        if (seg.flags == UNSHARED) { cerr << "U"; }
        cerr << endl;
    }
}



Paths& Path::assembleTaggedPaths(Path &path1, uint32_t flags1, Path &path2, uint32_t flags2, Paths &outPaths)
{
    // Tag segments for insideness, outsideness, or sharedness.
    cerr << endl << endl;
    cerr << "-----------------" << endl;
    path1.untag();
    path1.tagSegmentsRelativeToClosedPath(path2);
    cerr << "-----------------" << endl;
    path2.untag();
    path2.tagSegmentsRelativeToClosedPath(path1);
    cerr << "-----------------" << endl;
    
    uint32_t remaining = path1.size() + path2.size();
    
    // Mark all unwanted segments in path1 as used.
    Lines::iterator itera = path1.segments.begin();
    for (; itera != path1.segments.end(); itera++) {
        if ((itera->flags & flags1) == 0) {
            itera->flags = USED;
            remaining--;
        }
    }
    
    // Mark all unwanted segments in path2 as used.
    Lines::iterator iterb = path2.segments.begin();
    for (; iterb != path2.segments.end(); iterb++) {
        if ((iterb->flags & flags2) == 0) {
            iterb->flags = USED;
            remaining--;
        }
    }
    
    // Try assembling path from unused segments.
    Lines::iterator currseg;
    Lines::iterator otherseg;
    currseg = path1.segments.begin();
    otherseg = path2.segments.begin();
    int32_t pathLimit = 0;
    Path* patha = &path1;
    Path* pathb = &path2;

    outPaths.push_back(Path());
    Path* outPath = &outPaths.back();
    while (remaining > 0) {
        Line &seg = *currseg;
        if (seg.flags != USED && outPath->couldAttach(seg)) {
            // Found a connected unused segment.
            // Attach it to the current path.
            seg.flags = USED;
            outPath->attach(seg);
            remaining--;
            pathLimit = 0;
            currseg++;
            if (currseg == patha->segments.end()) {
                currseg = patha->segments.begin();
            }
            
            // If path was closed by this segment, remember it and start a new path.
            if (outPath->isClosed()) {
                outPath->simplify(2*EPSILON);
                outPaths.push_back(Path());
                outPath = &outPaths.back();
                pathLimit = 0;
            }
        } else {
            pathLimit++;
            
            // Swap over to the other path.
            Path* tmppath = patha;
            patha = pathb;
            pathb = tmppath;
            
            Lines::iterator tmpseg = currseg;
            currseg = otherseg;
            otherseg = tmpseg;
            
            // Find a connected unused segment in the new path.
            // Stop looking if we completely circumnavigate the path.
            int32_t limit = 0;
            for (limit = patha->size(); limit > 0; limit--) {
                currseg++;
                if (currseg == patha->segments.end()) {
                    currseg = patha->segments.begin();
                }
                if (currseg->flags != USED && outPath->couldAttach(*currseg)) {
                    break;
                }
            }
            
            if (limit == 0 && remaining > 0 && pathLimit >= 2) {
                // Failed to find another connected segment in either path.
                // Remember this path, and start a new one.
                outPath->simplify(2*EPSILON);
                outPaths.push_back(Path());
                outPath = &outPaths.back();
                pathLimit = 0;
            }
        }
    }
    if (outPath->size() == 0) {
        // Drop final path if empty.
        outPaths.pop_back();
    }
    
    return outPaths;
}



Paths &Path::unionOf(Path &path1, Path &path2, Paths &outPaths)
{
    return assembleTaggedPaths(path1, (OUTSIDE|SHARED), path2, OUTSIDE, outPaths);
}



Paths &Path::differenceOf(Path &path1, Path &path2, Paths &outPaths)
{
    return assembleTaggedPaths(path1, (OUTSIDE|UNSHARED), path2, INSIDE, outPaths);
}



Paths &Path::intersectionOf(Path &path1, Path &path2, Paths &outPaths)
{
    return assembleTaggedPaths(path1, (INSIDE|SHARED), path2, INSIDE, outPaths);
}



Paths &Path::unionOf(Paths &paths, Paths &outPaths)
{
    Paths::iterator it1;
    Paths::iterator it2;
    Paths::iterator it3;

    for (it1 = paths.begin(); it1 != paths.end(); it1++) {
        outPaths.push_back(*it1);
    }
    
    for (it1 = outPaths.begin(); it1 != outPaths.end(); ) {
        bool found = false;
        for (it2 = it1; it2 != outPaths.end(); it2++) {
            if (it1 != it2) {
                Paths tempPaths;
                Path::unionOf(*it1, *it2, tempPaths);
                if (tempPaths.size() < 2) {
                    for (it3 = tempPaths.begin(); it3 != tempPaths.end(); it3++) {
                        outPaths.push_back(*it3);
                    }
                    it2 = outPaths.erase(it2);
                    it1 = outPaths.erase(it1);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            it1++;
        }
    }
    return outPaths;
}



Paths &Path::differenceOf  (Paths &paths1, Paths &paths2, Paths &outPaths)
{
    Paths::iterator it1;
    Paths::iterator it2;

    outPaths = paths1;
    for (it2 = paths2.begin(); it2 != paths2.end(); it2++) {
        Paths tempPaths;
        for (it1 = outPaths.begin(); it1 != outPaths.end(); it1++) {
            Path::differenceOf(*it1, *it2, tempPaths);
        }
        outPaths = tempPaths;
    }
    return outPaths;
}



Lines &Path::containedSegments(const Line &line, Lines &outSegs) const
{
    Path linePath;
    linePath.attach(line);
    linePath.untag();
    linePath.tagSegmentsRelativeToClosedPath(*this);
    
    Lines::iterator itera = linePath.segments.begin();
    for (; itera != linePath.segments.end(); itera++) {
        Line &seg = *itera;
        if (seg.flags == INSIDE || seg.flags == SHARED || seg.flags == UNSHARED) {
            outSegs.push_back(seg);
        }
    }
    return outSegs;
}



Paths &Path::containedSubpathsOfPath(Path &path, Paths outPaths) const
{
    path.untag();
    path.tagSegmentsRelativeToClosedPath(*this);
    
    Lines outSegs;
    Lines::iterator itera = path.segments.begin();
    for (; itera != path.segments.end(); itera++) {
        Line &seg = *itera;
        if (seg.flags == INSIDE || seg.flags == SHARED || seg.flags == UNSHARED) {
            outSegs.push_back(seg);
        }
    }
    
    return assemblePathsFromSegments(outSegs, outPaths);
}



Paths &Path::leftOffset(double offsetby, Paths& outPaths)
{
    Path offsetPath;
    Lines offsetLines;
    Lines::iterator itera;
    Lines::iterator iterb;
    Lines::iterator iterc;
    Lines::iterator prevValid;
    int lastValidity = VALID;
    Point prevPt;
    bool closed = isClosed();

    if (segments.size() == 0) {
        return outPaths;
    }
    if (segments.size() == 1) {
        Line ln(segments.front());
        ln.leftOffset(offsetby);
        Path outPath;
        outPath.attach(ln);
        outPaths.push_back(outPath);
        return outPaths;
    }

    // Make raw insets lines.
    if (closed) {
        // TODO: handle closure
    } else {
        prevPt = segments.front().startPt;
    }
    for (iterb = segments.begin(), itera = iterb++, iterc = itera;
         iterb != segments.end();
         itera++, iterb++
        ) {
        double ang = itera->angle();
        double deltaAng = itera->angleDelta(*iterb);
        double bisectAng = (M_PI-deltaAng)/2.0f + ang;

        Point bisectPt(itera->endPt);
        bisectPt.polarOffset(bisectAng, offsetby);

        Line ln(prevPt, bisectPt);
        if (fabs(itera->angleDelta(ln)) > M_PI_2) {
            // Line got reversed by insetting.  Mark it invalid.
            if (lastValidity != VALID) {
                ln.flags = CONSECUTIVELY_INVALID;
                if (iterc->flags == INVALID) {
                    iterc->flags = CONSECUTIVELY_INVALID;
                }
            } else {
                ln.flags = INVALID;
            }
        } else {
            ln.flags = VALID;
            prevValid = itera;
        }
        lastValidity = ln.flags;
        offsetLines.push_back(ln);

        prevPt = bisectPt;
        iterc = itera;
    }
    if (closed) {
        // TODO: handle closure
    } else {
        Line ln(prevPt, itera->endPt);
        if (fabs(itera->angleDelta(ln)) > M_PI_2) {
            // Line got reversed by insetting.  Mark it invalid.
            if (lastValidity == INVALID) {
                ln.flags = CONSECUTIVELY_INVALID;
            } else {
                ln.flags = INVALID;
            }
        } else {
            ln.flags = VALID;
        }
        lastValidity = ln.flags;
        offsetLines.push_back(ln);
    }

    // Start pruning invalid segments.
    Lines::iterator nextValid = offsetLines.end();
    for (itera = offsetLines.begin(), iterb = segments.begin();
         itera != offsetLines.end();
         itera++, iterb++
        ) {
        if (itera->flags != VALID) {
            bool didUpdate = false;

            // Find next valid line.
            nextValid = itera;
            for (int limit = 2; limit-->0;) {
                while (nextValid != offsetLines.end() && nextValid->flags != VALID) {
                    nextValid++;
                }
                if (nextValid == offsetLines.end()) {
                    if (limit>0) {
                        nextValid = offsetLines.begin();
                        continue;
                    }
                }
                break;
            }
            if (nextValid == offsetLines.end()) {
                // No valid segments!  We have a null path.
                offsetLines.clear();
                break;
            }

            if (itera->flags == INVALID) {
                // case I
                Intersection isect = prevValid->intersectionWithExtendedLine(*nextValid);
                if (isect.type != NONE) {
                    prevValid->endPt = isect.p1;
                    nextValid->startPt = isect.p1;
                    itera = offsetLines.erase(itera);
                    iterb++;
                    didUpdate = true;
                }
            } else {
                // case II
                // TODO: find matching non-offset segments
                // TODO: do pairwise intersections of offsets from segments
                // set aside list of invalid segment's original segments.
                // for each original segment, trim off intersections from offset path
            }
            if (!didUpdate) {
                // TODO: Connect forward and backwards edge.
            }
        } else {
            prevValid = itera;
        }
    }
    // TODO: finish this.
    return outPaths;
}



Paths &Path::inset(double insetby, Paths& outPaths)
{
    if (isClockwise()) {
        insetby = -insetby;
    }
    return leftOffset(insetby, outPaths);
}



ostream& operator <<(ostream &os, const Path &path)
{
    os << "{";
    Lines::const_iterator it = path.segments.begin();
    for ( ; it != path.segments.end(); it++) {
        os << *it;
    }
    os << "}";
    return os;
}


}

