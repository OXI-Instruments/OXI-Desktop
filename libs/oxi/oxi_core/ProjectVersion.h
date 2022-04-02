#ifndef PROJECTVERSION_H
#define PROJECTVERSION_H

enum ProjectVersion {
    //
    Version0 = 1,
    // Add arranger master slot
    Version1 = 2,
    // automatically derive latest version
    Last,
    Latest = Last - 1,
};

enum MultitrackVersion {
    //
    MultitrackVersion0 = 1,
    // automatically derive latest version
    MultitrackLast,
    MultitrackLatest = MultitrackLast - 1,
};

enum PolyVersion {
    //
    PolyVersion0 = 1,
    // automatically derive latest version
    PolyLast,
    PolyLatest = PolyLast - 1,
};

enum MonoVersion {
    //
    MonoVersion0 = 1,
    // automatically derive latest version
    MonoLast,
    MonoLatest = MonoLast - 1,
};

enum ChordVersion {
    //
    ChordVersion0 = 1,
    // automatically derive latest version
    ChordLast,
    ChordLatest = ChordLast - 1,
};

enum StochVersion {
    //
    StochVersion0 = 1,
    // automatically derive latest version
    StochLast,
    StochLatest = StochLast - 1,
};


#endif // PROJECTVERSION_H
