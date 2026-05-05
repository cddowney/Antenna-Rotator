// File: position_conversion.h
// Author: Camden Downey

#ifndef POSITION_CONVERSION_H
#define POSITION_CONVERSION_H

#define ENCODER_COUNTS_PER_REV 34020.0f // approximation of encoder counters per rotation of final gear

static inline float degrees_to_counts(float degrees)
{
    return (degrees / 360.0f) * ENCODER_COUNTS_PER_REV;
}

static inline float counts_to_degrees(float counts)
{
    return (counts / ENCODER_COUNTS_PER_REV) * 360.0f;
}

#endif
