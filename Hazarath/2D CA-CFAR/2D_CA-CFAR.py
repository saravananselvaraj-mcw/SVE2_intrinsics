"""
Radar Target Generation and Detection -- small test-case variant
Based on: radar_target_generation_and_detection.py
(Python conversion of Radar_Target_Generation_and_Detection.m)
 
This variant is sized specifically to produce a 10 x 12 Range-Doppler Map
(RDM) so it can be used to validate a separately-written 2D CA-CFAR
implementation (e.g. in C) against this Python one:
 
    RDM size                : 10 (range bins) x 12 (doppler bins)
    Training cells (Tr, Td) : 2 x 2
    Guard cells    (Gr, Gd) : 2 x 2
 
Right after the Range-Doppler Map is generated, this script prints it out
as full-precision double values, formatted as a ready-to-paste C array
literal, so the exact same input matrix can be hardcoded into another
program for a side-by-side comparison.
"""
 
import numpy as np
 
# ---------------------------------------------------------------------------
# Radar Specifications
# ---------------------------------------------------------------------------
# Frequency of operation = 77GHz
# Max Range              = 200m
# Range Resolution        = 1 m
# Max Velocity            = 100 m/s
# speed of light          = 3e8
# ---------------------------------------------------------------------------
 
# ---------------------------------------------------------------------------
# User Defined Range and Velocity of target
# ---------------------------------------------------------------------------
# Define the target's initial position and velocity.
# Note: velocity remains constant.
v = -20
Range_init = 110
 
c = 3e8
r_res = 1
r_max = 200
 
# ---------------------------------------------------------------------------
# FMCW Waveform Generation
# ---------------------------------------------------------------------------
B_chirp = c / (2 * r_res)
T_chirp = 5.5 * (2 * r_max / c)
slope = B_chirp / T_chirp
 
# Operating carrier frequency of Radar
fc = 77e9  # carrier freq
 
# ---------------------------------------------------------------------------
# NOTE on sizing: Nr/2 rows x Nd columns is what ends up in the RDM (see the
# "RANGE DOPPLER RESPONSE" section below). To get a 10 x 12 RDM for CFAR
# testing purposes, Nr = 20 and Nd = 12. These are deliberately tiny
# compared to the original script's Nr=1024/Nd=128 -- this sizing is only
# meant to produce a small, easy-to-inspect matrix for algorithm
# validation, not a physically realistic range/Doppler resolution.
# ---------------------------------------------------------------------------
Nd = 12  # number of chirps -> RDM columns (doppler bins)
Nr = 20  # samples per chirp -> RDM has Nr/2 = 10 rows (range bins)
 
# Timestamp for running the displacement scenario for every sample on each
# chirp.
t = np.linspace(0, Nd * T_chirp, Nr * Nd)  # total time for samples
 
# ---------------------------------------------------------------------------
# Signal generation and Moving Target simulation (vectorized -- see the
# full-size script for the equivalent explicit per-sample for-loop).
# ---------------------------------------------------------------------------
r_t = Range_init + v * t
td = 2 * r_t / c
delay = t - td
 
Tx = np.cos(2 * np.pi * (fc * t + slope * (t ** 2) / 2))
Rx = np.cos(2 * np.pi * (fc * delay + slope * (delay ** 2) / 2))
Mix = Tx * Rx
 
# ---------------------------------------------------------------------------
# RANGE MEASUREMENT (1st FFT -- unrelated to CFAR, kept only for parity with
# the original script; skip straight to "RANGE DOPPLER RESPONSE" below if
# you only care about the CFAR input matrix).
# ---------------------------------------------------------------------------
Mix = Mix.reshape((Nr, Nd), order='F')
signal_fft = np.fft.fft(Mix, n=Nr, axis=0)
 
L = T_chirp * B_chirp
signal_fft = np.abs(signal_fft / L)
 
# NOTE: capped at Nr*Nd (total available samples) so this doesn't try to
# slice past the end of the array for a small Nr/Nd -- with the original
# script's Nr=1024/Nd=128 this cap never triggers and behavior is
# unchanged; it only matters for small test sizes like this one.
half = min(int(round(L / 2)) + 1, Nr * Nd)
signal_fft = signal_fft.flatten(order='F')[:half]
 
f = B_chirp * np.arange(half) / L
R = (c * T_chirp * f) / (2 * B_chirp)
 
# ---------------------------------------------------------------------------
# RANGE DOPPLER RESPONSE
# ---------------------------------------------------------------------------
Mix = Mix.reshape((Nr, Nd), order='F')
 
# 2D FFT using the FFT size for both dimensions.
sig_fft2 = np.fft.fft2(Mix, s=(Nr, Nd))
 
# Take just one side of the signal from the Range dimension.
sig_fft2 = sig_fft2[:Nr // 2, :Nd]
sig_fft2 = np.fft.fftshift(sig_fft2)
RDM = np.abs(sig_fft2)
RDM = 10 * np.log10(RDM)  # <-- this is "the Doppler map" (RDM), size 10 x 12
 
# ---------------------------------------------------------------------------
# Print the just-generated Range-Doppler Map as full double-precision
# values, formatted as a ready-to-paste C array literal. This is the exact
# matrix that the CFAR loop below operates on (before its own internal
# normalization step -- see the comment further down), so it's what you
# should feed into a separately-written CFAR implementation for a
# side-by-side comparison.
# ---------------------------------------------------------------------------
num_range_bins, num_doppler_bins = RDM.shape
 
# Also print the same matrix in a plain readable table, for a quick visual
# sanity check alongside the C literal above.
np.set_printoptions(precision=17, suppress=False, floatmode='maxprec_equal',
                     linewidth=200)
 
# ---------------------------------------------------------------------------
# CFAR implementation
# ---------------------------------------------------------------------------
# Number of Training Cells in both dimensions.
Tr = 2
Td = 3
 
# Number of Guard Cells in both dimensions around the Cell Under Test (CUT).
Gr = 2
Gd = 2
 
# Offset the threshold by an SNR value in dB.
offset = 1.2
 
 
def db2pow(db):
    """Convert dB to linear power (matches MATLAB's db2pow)."""
    return 10 ** (db / 10)
 
 
def pow2db(power):
    """Convert linear power to dB (matches MATLAB's pow2db)."""
    return 10 * np.log10(power)
 
 
# NOTE: this normalization happens *after* the RDM was already printed
# above, so the values you copied are the raw dB Range-Doppler Map, not
# this normalized version. If your own C implementation is meant to match
# this Python CFAR bit-for-bit, either feed it this same
# `RDM = RDM / max(RDM)` normalized matrix instead, or replicate this
# normalization step in your C code before running CFAR.
RDM = RDM / np.max(RDM)  # Normalizing

print(f"// Range-Doppler Map (RDM), {num_range_bins} x {num_doppler_bins}, "f"values in dB, as generated (double precision):")
print(f"double rdm[{num_range_bins}][{num_doppler_bins}] = {{")
for row in RDM:
    row_str = ", ".join(repr(float(value)) for value in row)
    print(f"    {{{row_str}}},")
print("};")
print()

for i in range(Tr + Gr, num_range_bins - (Tr + Gr)):
    for j in range(Td + Gd, num_doppler_bins - (Td + Gd)):
        noise_level = 0.0
        for p in range(i - (Tr + Gr), i + (Tr + Gr) + 1):
            for q in range(j - (Td + Gd), j + (Td + Gd) + 1):
                if abs(i - p) > Gr or abs(j - q) > Gd:
                    noise_level += db2pow(RDM[p, q])
        threshold = pow2db(
            noise_level
            / (2 * (Td + Gd + 1) * 2 * (Tr + Gr + 1) - (Gr * Gd) - 1)
        )
        threshold = threshold + offset
        CUT = RDM[i, j]
        if CUT < threshold:
            RDM[i, j] = 0
        else:
            RDM[i, j] = 1
 
RDM[(RDM != 0) & (RDM != 1)] = 0
 
print("CFAR output (0/1 detection map), for reference:")
print(RDM)
